/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   INVITE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yochakib <yochakib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/20 15:28:29 by elliech           #+#    #+#             */
/*   Updated: 2024/06/22 16:11:59 by yochakib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server/Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

bool Server::handle_INVITE(std::vector<std::string>& parts, Client& user)
{
    std::string client = parts[1];
    std::string channel_name = parts[2];

    Channel* existing_channel = this->findChannelByName(channel_name);
    Client* target_client = findClientByNickname(client);

    if (existing_channel)
    {
        if (target_client)
        {
            if (existing_channel->hasUser(user))
            {
                if (existing_channel->hasInviteOnly())
                {
                    if (existing_channel->isOperator(user))
                    {
                        existing_channel->inviteUser(*target_client);
                        send_message_to_client(":servername 341 " + user.getNickname() + " " + client + " " + channel_name, user);
                        send_message_to_client(":" + user.getNickname() + "!~" + user.getHostname() + " INVITE " + client + " " + channel_name, *target_client);
                        return (true);
                    }
                    else
                    {
                        send_message_to_client(":servername 482 " + user.getNickname() + " " + channel_name + " :You're not channel operator", user);
                        return (false);
                    }
                }
                else 
                {
                    existing_channel->inviteUser(*target_client);
                    send_message_to_client(":servername 341 " + user.getNickname() + " " + client + " " + channel_name, user);
                    send_message_to_client(":" + user.getNickname() + " INVITE " + client + " " + channel_name, *target_client); 
                    return (true);  
                }
            }
            else
            {
                send_message_to_client(":servername 442 " + user.getNickname() + " " + channel_name + " :You're not on that channel", user);
                return (false);
            }
        }
        else
        {
            send_message_to_client(":servername 401 " + client + " " + channel_name + " :No such nick/channel", user);
            return (false);
        }
    }
    else
    {
        send_message_to_client(":servername 403 " + user.getNickname() + " " + channel_name + " :No such channel", user);
        return (false);
    }
}