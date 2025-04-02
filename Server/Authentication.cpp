/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Authentication.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yochakib <yochakib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 18:08:07 by ssaadaou          #+#    #+#             */
/*   Updated: 2024/07/26 21:04:42 by yochakib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

std::string onlyPass(std::string msg)
{
    std::string code;
    size_t i = 3;
    while (msg[i] && msg[i] != ' ')
        i++;
    i++;
    while (msg[i] && msg[i] != '\n' && msg[i] != '\r' && msg[i] != '\0')
    {
        code += msg[i];
        i++;
    }
    return code;
}


void Server::sendError(int clientFd, const std::string &num, const std::string &msg)
{
    std::string error = ":" + this->clients[clientFd].getHostname() + " " + num + " " + this->clients[clientFd].getNickname() + " :" + msg + "\r\n";
    send(clientFd, error.c_str(), error.size(), 0);
}

bool Server::NicknameExists(const std::string &nickname)
{
    std::map<int, Client>::iterator it;
    for (it = this->clients.begin(); it != this->clients.end(); it++)
    {
        if (it->second.getNickname() == nickname)
            return true;
    }
    return false;
}

bool Server::isValidNickname(const std::string &nickname)
{
    std::string validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789[]{}-_\\|";
    
    if (nickname.empty() || nickname.length() > 16)
        return false;
    if(isdigit(nickname[0])) 
        return false; 
    for (size_t i = 0; i < nickname.length(); ++i)
    {
        if (validChars.find(nickname[i]) == std::string::npos) 
            return false;
    }
    return true;
}

bool Server::isValidUser(const std::string &user)
{
    std::string validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789*";
    
    for (size_t i = 0; i < user.length(); ++i)
    {
        if (validChars.find(user[i]) == std::string::npos) 
            return false;
    }
    return true;
}

void Client::checkRegistration()
{
    if (this->getPass() == true && this->getNickname() != "*" && this->getUsername() != "" && this->getRealname() != "" && this->getHostname() != "")
        this->setRegistered(true);
}

bool findClient_inChannel(std::vector<Channel> &tab, Client &user, std::string channel_name)
{
    for (std::vector<Channel>::iterator it = tab.begin(); it != tab.end(); it++)
    {
        if (it->getName() == channel_name)
        {
            std::vector<Client *> users = it->getUsers();
            for (std::vector<Client *>::iterator it2 = users.begin(); it2 != users.end(); it2++)
            {
                if ((*it2)->getNickname() == user.getNickname())
                {
                    return true;
                }
            }
        }
    }
    return false;
}


void Server::handleCommand(int clientFd)
{ 
    std::string Input;
    int  rec = receiveMessage(clientFd);
        
    if (rec == -2)
        return;
    else if (rec == 0)
    {
        this->clients[clientFd].setPartialMsg("");
        return;
    }
    
    Input = this->clients[clientFd].getPartialMsg();
    this->clients[clientFd].setPartialMsg("");
    Input.erase(std::remove(Input.begin(), Input.end(), '\r'), Input.end());
    Input.erase(std::remove(Input.begin(), Input.end(), '\n'), Input.end());
    std::vector<std::string> msg = splitMsg(Input);
    
    if (msg.empty())
        return;
    
    if ((msg[0] == "PASS" || msg[0] == "pass"))
    {
        std::string code = onlyPass(Input);
        if (this->clients[clientFd].getPass() == false)
        {
            if (code == this->passwrd)
                this->clients[clientFd].setPass(true);
            else if (code != this->passwrd)
                sendError(clientFd, "464", "Invalid password");
        }
        else
            sendError(clientFd, "462", "You may not reregister");
    }
    else if ((msg[0] == "NICK" || msg[0] == "nick" ))
    {
        if (msg.size() > 2 )
            return sendError(clientFd, "461", "Not enough parameters");
        if ( msg.size() == 2 && this->clients[clientFd].getPass() == false)
            return sendError(clientFd, "451", "You have not registered");
        if (msg.size() < 2 || msg[1].empty())
            return sendError(clientFd, "431", "No nickname given");
        if (isValidNickname(msg[1]) == false)
            return sendError(clientFd, "432", "Erroneous nickname");
        if (NicknameExists(msg[1]))
            return sendError(clientFd, "433", "Nickname is already in use");
        this->clients[clientFd].set_old_nickname(this->clients[clientFd].getNickname());
        if (!this->clients[clientFd].getChannels().empty())
        {
            this->clients[clientFd].setChange_nickname(true);
            this->clients[clientFd].setNickname(msg[1]);
        }
        else if (this->clients[clientFd].getChannels().empty())
            this->clients[clientFd].setNickname(msg[1]);

        this->clients[clientFd].checkRegistration();
        this->clients[clientFd].setAuthenticated(false);
        if ( this->clients[clientFd].getRegistered() == true && this->clients[clientFd].get_i() > 0 )
        {
            std::string buff =  ":" + this->clients[clientFd].get_old_nickname() + "!~@" + this->clients[clientFd].getHostname() + " NICK :" + msg[1] + "\r\n";
            sendMessage(clientFd, buff);            
        }
    }
    else if ((msg[0] == "USER" || msg[0] == "user" ))
    {
        if (this->clients[clientFd].getPass() == false)
            return sendError(clientFd, "451", "You have not registered");
            
        if ( msg.size() < 5 || msg[1].empty() || msg[2].empty() || msg[3].empty() || msg[4].empty())
            return sendError(clientFd, "461", "Not enough parameters");
        if ((isValidUser(msg[1]) == false ))
            return  sendError(clientFd, "432", "Erroneous username");
   
        this->clients[clientFd].setUsername(msg[1]);
        this->clients[clientFd].setHostname(msg[2]);
        this->clients[clientFd].setServername(msg[3]);
        if(msg[4][0] == ':' )
        {
            int pos = Input.find_last_of(":");
            std::string tmp = Input.substr(pos + 1);
            msg[4] = tmp;
            msg.erase(msg.begin() + 5, msg.end());
            if (msg[4].empty())
                return sendError(clientFd, "461", "Not enough parameters");
        }
        this->clients[clientFd].setRealname(msg[4]);
    
        this->clients[clientFd].checkRegistration();
    }
    else if (msg[0] == "QUIT" && this->clients[clientFd].getRegistered() == true)
    {
        handleQuit(clientFd);
        return;
    }
    else if(this->clients[clientFd].getRegistered() == true)
        this->clients[clientFd].setMsg(Input);   
    else
        sendMessage(clientFd, "Invalid Command");

    if (this->clients[clientFd].getRegistered() && this->clients[clientFd].getAuthenticated() == false && this->clients[clientFd].get_i() == 0)
    {
        this->clients[clientFd].set_i(1);
        std::string welcome_message = ": 001 " + this->clients[clientFd].getNickname() + " : Welcome to Internet Chat Relay \r\n";
        std::string host_message = ": 002 " + this->clients[clientFd].getNickname() + " : Your Host is " + this->get_ip_address(clientFd) + ", running version 1.0 \r\n";
        std::string server_message = ": 003 " + this->clients[clientFd].getNickname() + " : Ther server was created on " + get_current_time()  + " \r\n";
        send(clientFd, welcome_message.c_str(), welcome_message.length(), 0);
        send(clientFd, host_message.c_str(), host_message.length(), 0);
        send(clientFd, server_message.c_str(), server_message.length(), 0);
        this->clients[clientFd].setAuthenticated(true);
    }
}


