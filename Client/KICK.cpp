/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yochakib <yochakib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/15 19:20:09 by yochakib          #+#    #+#             */
/*   Updated: 2024/07/27 16:35:02 by yochakib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"
#include "../Server/Server.hpp"


bool Server::handle_KICK(std::vector<std::string>& parts, Client& user) 
{
    std::string channel_name = parts[1];
    std::vector<std::string> nickname_list = split_user_input(parts[2]);
    std::string reason = parts.size() > 3 ? parts[3] : "Kicked";
    Channel* existing_channel = this->findChannelByName(channel_name);
    
    if (existing_channel)
    {
        if (existing_channel->isOperator(user))
        {
            for (std::vector<std::string>::iterator it = nickname_list.begin(); it != nickname_list.end(); ++it)
            {
                std::string nickname = *it;
                Client* target = this->findClientByNickname(nickname);
                if (target && existing_channel->hasUser(*target))
                {
                    std::string kick_message = ":" + user.getNickname() + "!~" + "SERVER_NAME " + "KICK " + existing_channel->getName() + " " + target->getNickname() + " :" + reason;
                    send_message_to_client(kick_message, *target);
                    
                    bool was_operator = existing_channel->isOperator(*target);
                    if (was_operator)
                    {
                        existing_channel->removeOperator(*target);
                    }
                    existing_channel->removeUser(target);
                    // Handle operator reassignment if needed
                    if (was_operator && existing_channel->getOperators().empty() && !existing_channel->getUsers().empty())
                    {
                    std::vector<Client*>& users = existing_channel->getUsers();
                    std::vector<Client*>::iterator it = users.begin();
                        if (it != users.end())
                        {
                            Client* new_operator = *it;
                            existing_channel->addOperator(*new_operator);
                            
                            // Notify about new operator
                            std::string op_message = ":SERVER_NAME MODE " + existing_channel->getName() + " +o " + new_operator->getNickname();
                            broadCast(op_message, user, existing_channel->getName());
                        }
                    }
                    broadCast(kick_message, user, existing_channel->getName());
                }
                else if (target)
                {
                    send_message_to_client(":" + user.getNickname() + "!~" + "SERVER_NAME " + "441 " + user.getNickname() + " " + target->getNickname() + " :They aren't on that channel", user);
                }
                else
                {
                    send_message_to_client(":" + user.getNickname() + "!~" + "SERVER_NAME " + "401 " + user.getNickname() + " " + nickname + " :No such nick/channel", user);
                }
            }
        }
        else
        {
            send_message_to_client(":" + user.getNickname() + "!~" + "SERVER_NAME " + "482 " + user.getNickname() + " " + channel_name + " :You're not channel operator", user);
        }
    }
    else
    {
        send_message_to_client(":" + user.getNickname() + "!~" + "SERVER_NAME " + "403 " + user.getNickname() + " " + channel_name + " :No such channel", user);
    }
    return true;
}