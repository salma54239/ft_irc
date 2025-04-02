/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MODE.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yochakib <yochakib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/26 16:51:56 by yochakib          #+#    #+#             */
/*   Updated: 2024/07/27 16:47:49 by yochakib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "../Server/Server.hpp"


bool Server::handle_MODE(std::vector<std::string>& parts, Client& user)
{
    std::string channel_name = parts[1];
    Channel* existing_channel = this->findChannelByName(channel_name);
    if (existing_channel)
    {
        if (existing_channel->hasUser(user))
        {
            std::string modes = parts[2];
            size_t param_index = 3;
            bool adding = true;
            if (!existing_channel->isOperator(user) && parts.size() > 2)
            {
                send_message_to_client(":servername 482 " + existing_channel->getName() + ":You're not channel operator" , user);
                return (false);
            }
            else if (parts.size() == 2)
            {
                send_message_to_client(":servername 324 " + user.getNickname() + " " + existing_channel->getName() + " :" + setToString(existing_channel->getMode()), user);
            }
            else
            {
                for (size_t i = 0; i < modes.size(); ++i)
                {
                    if (modes[i] == '+')
                    {
                        adding = true;
                    }
                    else if (modes[i] == '-')
                    {
                        adding = false;
                    }
                    else
                    {
                        char mode = modes[i];
                        std::string param;
                        if (mode == 'o' || (mode == 'k' && adding == true) || ( mode == 'l' && adding == true))
                        {
                            if (param_index >= parts.size())
                            {
                                send_message_to_client(":servername 461 : Missing parameter for mode " + std::string(1, mode) + ".", user);
                                return false;
                            }
                            param = parts[param_index++];
                        }
                        existing_channel->setMode(mode, adding, param, user, *this);
                    }
                }
            }
        }
        else
        {
            send_message_to_client(":servername 442 " + user.getNickname() + " " + existing_channel->getName() + " :You're not on that channel", user);
            return (false);
        }
    }
    else
    {
        send_message_to_client(":servername 403" + user.getRealname() +  channel_name + " :No such channel .", user);
        return (false);
    }
    return (true);
}