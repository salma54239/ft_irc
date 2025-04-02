/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Prompt.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssaadaou <ssaadaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 19:59:49 by ssaadaou          #+#    #+#             */
/*   Updated: 2024/07/26 18:31:37 by ssaadaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void    Server::sendMessage(int clientFd, const std::string& message)
{
    std::string msg = message + "\r\n";
    send(clientFd, msg.c_str(), msg.size(), 0);
}


bool checkNewLine(char *buffer)
{
    int i = 0;
    while (buffer[i])
    {
        if (buffer[i] == '\n')
            return true;
        i++;
    }
    return false;
}

int Server::receiveMessage(int clientFd)
{
    char buffer[1024];
    bzero(buffer, 1024);
    std::string Msg;
    int bytesRead = recv(clientFd, buffer, sizeof(buffer), 0);
    if (bytesRead == -1)
        throw std::runtime_error("Failed to receive data");
        
    if (bytesRead == 0)
    {
        std::cout << "Client disconnected" << std::endl;
        close(clientFd);
        return 0;
    }
    
    if (checkNewLine(buffer))
    {
        if(this->clients[clientFd].getPartialMsg() == "")
        {
            this->clients[clientFd].setPartialMsg(std::string(buffer));
            return 1;
        }
        else
        {
            this->clients[clientFd].setPartialMsg(this->clients[clientFd].getPartialMsg() + std::string(buffer));
            return 1;
        }
        
    }
    else if(checkNewLine(buffer) == false)
    {
        std::string partialMsg = this->clients[clientFd].getPartialMsg() + std::string(buffer);
        this->clients[clientFd].setPartialMsg(partialMsg);
        return -2;
    
    }    
    return 0;
}