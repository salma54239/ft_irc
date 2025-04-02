/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCbot.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yochakib <yochakib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/23 11:29:16 by yochakib          #+#    #+#             */
/*   Updated: 2024/07/26 21:22:56 by yochakib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "IRCbot.hpp"


IRCbot::IRCbot() {};

IRCbot::~IRCbot()
{
    close(this->sockfd);
}
void IRCbot::handleServerMessage(const std::string &msg)
{
    std::vector<std::string> parts;
    std::string command;
    std::istringstream Iss(msg);
    while (std::getline(Iss, command, ' '))
    {
        parts.push_back(command);
    }
    if (parts[0] == "PRIVMSG" &&  parts[2] == "USMELL" && parts.size() == 4)
    {
        std::string response = "PRIVMSG " +  parts[3] +" :" + "You have been pinged, you need a bath. Urgently!" +"\r\n";
        if (send(this->sockfd, response.c_str(), response.length(), 0) < 0)
        {
            throw std::runtime_error("Failed to send USMELL message");
        }
    }
}

void IRCbot::set_socket()
{
    this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->sockfd < 0) 
    {
        throw std::runtime_error("Failed to create bot socket");
    }
}
void IRCbot::setusername(const std::string &user)
{
    this->username = user;
}
void IRCbot::setnickname(const std::string &nick)
{
    this->nickname = nick;
}
void IRCbot::setrealname(const std::string &real)
{
    this->realname = real;
}
void IRCbot::sethostname(const std::string &host)
{
    this->hostname = host;
}

bool valid_port(std::string &str) { return str.find_first_not_of("0123456789") == std::string::npos; }

void IRCbot::set_args(int port, std::string server_password, std::string ip)
{
    this->port = port;
    this->server = server_password;
    this->ip = ip;
}
bool parseInput(int ac, char **av)
{
    if (ac != 4)
    {
        std::cout << "Usage: ./BOT <port> <password> <ip>" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string port(av[1]);
    if (!*av[2])
    {
        std::cout << "Required : <password>" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (!*av[3])
    {
        std::cout << "Required : <ip>" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (!valid_port(port))
    {
        std::cout << "Port must be a number" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(port.size() > 5 || atoi(port.c_str()) < 1024 || atoi(port.c_str()) > 65535)
    {
        std::cout << "Port must be between 1024 and 65535" << std::endl;
        exit(EXIT_FAILURE);
    }
    return true;
}
// default Constructor

void IRCbot::connect_to_server()
{
    set_socket();
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr)); // Zero out the structure
    serv_addr.sin_family = AF_INET; // IPV4
    serv_addr.sin_port = htons(this->port); // function convert port to network byte order
    
    if (connect(this->sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        throw std::runtime_error("Failed to connect to server");
    }
    this->setnickname("bot");
    this->setusername("bot");
    this->setrealname("bot");
    this->sethostname("bot");

    std::string pass = "PASS " + this->server + "\r\n";
    std::string user = "USER a a a a \r\n";
    std::string nickn = "NICK " + this->nickname + "\r\n";
    
    if (send(this->sockfd, pass.c_str(), pass.length(), 0) < 0)
    {
        throw std::runtime_error("Failed to send password");
    }
    sleep(1);
    if (send(this->sockfd, user.c_str(), user.length(), 0) < 0)
    {
        throw std::runtime_error("Failed to send user");
    }
    sleep(1);
    if (send(this->sockfd, nickn.c_str(), nickn.length(), 0) < 0)
    {
        throw std::runtime_error("Failed to send nickname");
    }
    run();
}

void    IRCbot::run()
{
    char buffer[1024];
    std::string msg("");
    int bytes;
    
    while (1)
    {
        bytes = recv(this->sockfd, buffer, 1024, 0); // 0 : no flags
        if (bytes < 0)
        {
            throw std::runtime_error("Failed to receive message");
        }
        else if (bytes == 0)
        {
            std::cout << "Connection closed" << std::endl;
            break;
        }
        buffer[bytes] = '\0';
        msg = buffer;
        
        if (bytes > 0 && !msg.empty())
        {
            std::cout << msg << std::endl;
            handleServerMessage(msg);
        }
        msg.clear();
    }
}

int main(int ac,char **av)
{
    try
    {
        if (!parseInput(ac, av) == 1)
            return 1;
        IRCbot bot;
        bot.set_args(atoi(av[1]), av[2], av[3]);
        bot.connect_to_server();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

}