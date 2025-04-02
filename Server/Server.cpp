/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yochakib <yochakib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/20 16:04:26 by ssaadaou          #+#    #+#             */
/*   Updated: 2024/08/01 19:45:58 by yochakib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "../Client/Client.hpp"

Server::Server()
{
    this->fd = -1;
    this->port = 0;
    this->passwrd = "";
    this->ip = new char[INET_ADDRSTRLEN];
}

Server::~Server()
{
    delete[] this->ip;
}

std::string Server::get_ip_address(int fd)
{
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int clt = getpeername(fd, (struct sockaddr *)&addr, &addr_size);
    if (clt == -1)
        throw std::runtime_error("Failed to get peer name");
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
    return ip;
}

std::string Server::get_current_time()
{
    time_t now = time(0);
    struct tm tstruct;
    char buff[80];
    tstruct = *localtime(&now);
    strftime(buff, sizeof(buff), "%Y-%m-%d %X", &tstruct);
    return buff;
}

void Server::parseInput(int ac, char **av)
{
    if (ac != 3)
        throw std::runtime_error("Usage: ./ircserv port passwrd");

    if (std::string(av[1]).find_first_not_of("0123456789") != std::string::npos)
        throw std::runtime_error("Port must be a number");

    std::istringstream iss(av[1]);
    if (!(iss >> this->port))
        throw std::runtime_error("Port must be a number");
    if (this->port < 1024 || this->port > 65535)
        throw std::runtime_error("Port must be between 1024 and 65535");
    std::cout << "Port: " << this->port << std::endl;

    if (!std::string(av[2]).empty())
        this->passwrd = av[2];
    else
        throw std::runtime_error("Passwrd must be a string");
    std::cout << "Passwrd: " << this->passwrd << std::endl;
}

void Server::setSocket()
{
    struct sockaddr_in socketAddr;
    struct pollfd poll_fd;
    
    this->fd = socket(AF_INET, SOCK_STREAM, 0); 
    if (this->fd == -1)
        throw std::runtime_error("Failed to create socket");
    fcntl(this->fd, F_SETFL, O_NONBLOCK);

    int opt = 1;
    if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        close(this->fd);
        throw std::runtime_error("Failed to set socket options");
    }
    
    socketAddr.sin_family = AF_INET;        
    socketAddr.sin_port = htons(this->port); 
    socketAddr.sin_addr.s_addr = INADDR_ANY; 

    if (bind(this->fd, (struct sockaddr *)&socketAddr, sizeof(socketAddr)) == -1)
    {
        close(this->fd);
        throw std::runtime_error("Failed to bind socket");
    }
    if (listen(this->fd, 10) == -1)
    {
        close(this->fd);
        throw std::runtime_error("Failed to listen for incoming connections");
    }
        
    poll_fd.fd = this->fd;
    poll_fd.events = POLLIN;
    poll_fd.revents = 0;
    this->poll_fd.push_back(poll_fd);
}

int Server::acceptClient()
{
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    int clientFd = accept(this->fd, (struct sockaddr *)&clientAddr, &clientAddrSize);
    if (clientFd == -1)
    {
        std::cerr << "Failed to accept connection" << std::endl;
        close(this->fd);
        return -1;
    }
    struct pollfd poll_fd = {clientFd, POLLIN, 0};
    this->poll_fd.push_back(poll_fd);

    Client newClient(clientFd);
    this->clients[clientFd] = newClient;
    return clientFd;
}

void Server::run()
{
    int clientFd;

    while (true)
    {
        signal(SIGPIPE, SIG_IGN);
        int ret = poll(this->poll_fd.data(), this->poll_fd.size(), 0);
        if (ret == -1)
            throw std::runtime_error("Failed to poll");
        if (ret == 0)
            continue;
        for (size_t i = 0; i < this->poll_fd.size(); i++)
        {
            if (this->poll_fd[i].revents & POLLIN)
            {
                if (this->poll_fd[i].fd == this->fd)
                {
                    clientFd = this->acceptClient();
                    if (clientFd == -1)
                        return;
                    std::cout << "New client connected: " << clientFd << std::endl;
                }
                else if (this->poll_fd[i].revents & POLLHUP)
                {
                    handleQuit(this->poll_fd[i].fd);
                    std::cout << "Client disconnected" << std::endl;
                    close(this->poll_fd[i].fd);
                    this->poll_fd.erase(this->poll_fd.begin() + i);
                }
                else
                    handleCommand(this->poll_fd[i].fd);
            }
        }
		std::map<int, Client>& clients = this->getClients();
		std::map<int, Client>::iterator it;
        for (it = clients.begin(); it != clients.end(); ++it) 
		{
			if (it->second.getRegistered() && !it->second.getMsg().empty())
			{
				std::string msg = it->second.getMsg();
				parse_command( msg, it->second);
				it->second.setMsg("");
			}
		}
    }
}

Channel* Server::findChannelByName(const std::string& channel_name)
{
	size_t i;
	for (i = 0; i < AllChannels_.size() ; ++i)
	{
		if (AllChannels_[i].getName() == channel_name) 
		{
            return &AllChannels_[i];
        }
	}
	return NULL;
}

void Server::broadCast(std::string message, Client& user, std::string channel) 
{
    
    Channel* targetChannel = findChannelByName(channel);
    if (!targetChannel) 
    {
        return;
    }
    std::vector<Client*> users = targetChannel->getUsers();
    for (std::vector<Client*>::iterator it = users.begin(); it != users.end(); ++it) 
    {
        if (message.find("!~") != std::string::npos) 
        {
            send_message_to_client(message, **it);
        }
        else 
        {
            if (*it != &user) 
            { 
                send_message_to_client(message, **it);
            }

        }
    }
}

void Server::handleQuit(int clientFd)
{
    if (!this->clients[clientFd].getChannels().empty())
    {
        std::vector<Channel> &tab = this->AllChannels_;
        std::vector<Channel>::iterator it = tab.begin();
        while (it != tab.end())
        {
            bool clientRemoved = false;
            if (findClient_inChannel(tab, this->clients[clientFd], it->getName()))
            {
                std::vector<Client *> &users = it->getUsers();
                std::vector<Client *>::iterator userIt = users.begin();
                while (userIt != users.end())
                {
                    if (*userIt && (*userIt)->getNickname() == this->clients[clientFd].getNickname())
                    {
                        if (it->isOperator(this->clients[clientFd]))
                        {
                            std::vector<Client *> &operators = it->getOperators();
                            operators.erase(std::remove(operators.begin(), operators.end(), *userIt), operators.end());
                            
                            if (operators.empty() && users.size() > 1)
                            {
                                std::vector<Client *>::iterator nextUser = userIt;
                                ++nextUser;
                                if (nextUser == users.end()) 
                                    nextUser = users.begin();
                                if (*nextUser) 
                                    it->addOperator(**nextUser);
                            }
                        }
                        users.erase(userIt);
                        clientRemoved = true;
                        break;
                    }
                    else
                    {
                        ++userIt;
                    }
                }
                if (users.empty())
                {
                    it = tab.erase(it);
                    continue;
                }
                std::vector<Client *> &operators = it->getOperators();
                std::vector<Client *>::iterator nextUser;
                if (operators.empty() && !users.empty())
                {
                    nextUser = users.begin();
                    if (nextUser != users.end() && *nextUser)
                    {
                        it->addOperator(**nextUser);
                        
                        std::string quit_message = ":" + this->clients[clientFd].getNickname() + "!~" + this->clients[clientFd].getRealname() + "@" + this->clients[clientFd].getHostname() + " PART " + it->getName() + " :Leaving";
                        std::string quit_message2 = ":" + this->clients[clientFd].getNickname() + "!~" + this->clients[clientFd].getRealname() + "@" + this->clients[clientFd].getHostname() + " MODE " + it->getName() + " +o " + (*nextUser)->getNickname();
                        std::string quit_message3 = ": " + this->clients[clientFd].getHostname() + ":New Admin for this channel " + it->getName() + " is " + (*nextUser)->getNickname();
                        
                        broadCast(quit_message, this->clients[clientFd], it->getName());
                        broadCast(quit_message2, this->clients[clientFd], it->getName());
                        broadCast(quit_message3, this->clients[clientFd], it->getName());
                    }
                }
            }
            if (!clientRemoved)
            {
                ++it;
            }
        }
    }
    this->clients.erase(clientFd);
    close(clientFd);
}

