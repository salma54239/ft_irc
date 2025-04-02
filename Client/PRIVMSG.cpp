/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PRIVMSG.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yochakib <yochakib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 20:18:31 by yochakib          #+#    #+#             */
/*   Updated: 2024/06/28 19:49:01 by yochakib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "../Server/Server.hpp"

std::string parsePRIVMSG(std::vector<std::string>& parts)
{
    std::string msg;
    if (parts[2][0] == ':')
    {
        for (size_t i = 2; i < parts.size(); ++i)
        {
            msg += parts[i];
            if (i != parts.size() - 1)
                msg += " ";
        }
    }
    else
    {
        msg = parts[2];
    }
    return (msg);
}
std::vector<std::string> split_target(std::string message)
{
	std::vector<std::string> target;
	std::string targets_part = message;
	std::istringstream targetIss(targets_part);
	std::string target_name;
	while (std::getline(targetIss, target_name, ',')) 
	{
		target.push_back(target_name);
	}
	return target;
}

bool Server::handle_PRIVMSG(std::vector<std::string>& parts, Client& user)
{
    std::vector<std::string> target_list = split_target(parts[1]);
    std::string message = parsePRIVMSG(parts);
    for (size_t i = 0; i < target_list.size() ; ++i)
    {
        std::string target = target_list[i];
        if (target[0] == '#' || target[0] == '&')
        {
            Channel* existing_channel = this->findChannelByName(target);
            if (existing_channel)
            {
                if (existing_channel->hasUser(user))
                {
                    std::vector<Client*> users = existing_channel->getUsers();
                    for (size_t i = 0; i < users.size(); ++i)
                    {
                        if (users[i]->getNickname() == user.getNickname())
                            continue;
                        send_message_to_client(":" + user.getNickname() + "!~"  + user.getHostname() + " PRIVMSG " + target + " " + message, *users[i]);
                    }
                }
                else
                {
                    send_message_to_client(":servername 442 " + user.getNickname() + " :You're not on that channel", user);
                }
            }
            else
            {
                send_message_to_client(":servername 403 " + user.getNickname() + " :No such channel", user);
            }
        }
        else
        {
            Client* target_client = findClientByNickname(target);
            if (target_client && target_client->getNickname() == "bot")
            {
                std::string target_message;
                for (size_t i = 0; i < parts.size(); i++)
                {
                    target_message += parts[i];
                    if (i != parts.size() - 1)
                        target_message += " ";
                }
                send_message_to_client(target_message, *target_client);
                return (true);
            }
            else if (target_client && target_client->getNickname() != "bot")
            {
                send_message_to_client(":" + user.getNickname() + "!~"  + user.getHostname() + " PRIVMSG " + target + " " + message, *target_client);
            }
            else
            {
                send_message_to_client(":servername 401 " + user.getNickname() + " :No such nickname", user);
            }
        }
    }
   return (true);
}