/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TOPIC.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elliech <elliech@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 13:29:43 by yochakib          #+#    #+#             */
/*   Updated: 2024/07/06 23:04:53 by elliech          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "../Server/Server.hpp"

bool Server::handle_TOPIC(std::vector<std::string>& parts, Client& user)
{
    std::string channel_name = parts[1];
    Channel* existing_channel = this->findChannelByName(channel_name);
    if (existing_channel)
    {
        if(existing_channel->hasUser(user))
        {
            if (parts.size() >= 3)
            {
                if (!existing_channel->hasTopicLoked()) //topic not locked regular users can set topic
                {
                    std::string new_topic = parts[2];
                    if (new_topic == ":")
                    {
                        existing_channel->setTopic("");
                        existing_channel->setTopicAuthor(user.getNickname());
                        broadCast(":" + user.getNickname() + "!~" + "SERVER_NAME " + "TOPIC "+ existing_channel->getName() + " "+ existing_channel->getTopic(), user, existing_channel->getName());
                        broadCast(":servername 333 " + user.getNickname() + " " + existing_channel->getName() + " " + existing_channel->getTopicAuthor(), user, existing_channel->getName());
                    }
                    else
                    {
                        if(parts[2][0] == ':')
                        {
                            new_topic = parts[2].substr(1);
                            for (size_t i = 3; i < parts.size(); ++i)
                            {
                                new_topic += " " + parts[i];
                            }
                        }
                        else
                        {
                            new_topic = parts[2];
                        }
                        
                        existing_channel->setTopic(new_topic);
                        existing_channel->setTopicAuthor(user.getNickname());
                        broadCast(":" + user.getNickname() + "!~" + "SERVER_NAME " + "TOPIC "+ existing_channel->getName() + " "+ existing_channel->getTopic(), user, existing_channel->getName());
                        broadCast(":servername 333 " + user.getNickname() + " " + existing_channel->getName() + " " + existing_channel->getTopicAuthor(), user, existing_channel->getName());
                    }
                }
                else if (existing_channel->hasTopicLoked() && existing_channel->isOperator(user)) // topic locked ops 
                {
                    std::string new_topic = parts[2];
                    if (new_topic == ":")
                    {
                        existing_channel->setTopic("");
                        existing_channel->setTopicAuthor(user.getNickname());
                        broadCast(":" + user.getNickname() + "!~" + "SERVER_NAME " + "TOPIC "+ existing_channel->getName() + " "+ existing_channel->getTopic(), user, existing_channel->getName());
                        broadCast(":servername 333 " + user.getNickname() + " " + existing_channel->getName() + " " + existing_channel->getTopicAuthor(), user, existing_channel->getName());
                        return (true);
                    }
                    else
                    {
                        if(parts[2][0] == ':' || (parts[2][0] == ':' && parts[3][0] == ':'))
                        {
                            new_topic = parts[2].substr(1);
                            for (size_t i = 3; i < parts.size(); ++i)
                            {
                                new_topic += " " + parts[i];
                            }
                        }
                        else
                        {
                            new_topic = parts[2];
                        }
                        
                        existing_channel->setTopic(new_topic);
                        existing_channel->setTopicAuthor(user.getNickname());
                        broadCast(":" + user.getNickname() + "!~" + "SERVER_NAME " + "TOPIC "+ existing_channel->getName() + " "+ existing_channel->getTopic(), user, existing_channel->getName());
                        broadCast(":servername 333 " + user.getNickname() + " " + existing_channel->getName() + " " + existing_channel->getTopicAuthor(), user, existing_channel->getName());
                    }
                }
                else
                {
                    send_message_to_client(":servername 482 " + user.getNickname() + " :You're not channel operator", user);
                }
            }
            else if (parts.size() == 2)
            {
                if (existing_channel->getTopic().empty())
                {
                    send_message_to_client(":servername 331 " + user.getNickname() + " " + existing_channel->getName() + " :No topic is set", user);
                }
                else
                {
                    send_message_to_client(":" + user.getNickname() + "!~" + "SERVER_NAME " + "TOPIC "+ existing_channel->getName() + " "+ existing_channel->getTopic(), user);
                    send_message_to_client(":servername 333 " + user.getNickname() + " " + existing_channel->getName() + " " + existing_channel->getTopicAuthor(), user);
                }
            }
            else
            {
                send_message_to_client(":servername 461 TOPIC :Not enough parameters ", user);
                return (false);
            }
        }
        else
        {
            send_message_to_client(":servername 442 " + user.getNickname() + " :You're not on that channel", user);
            return (false);
        }
    }
    else if (parts.size() == 2 && parts[1] == ":")
    {
        send_message_to_client(":servername 461 TOPIC :Not enough parameters ", user);
        return (false);
    }
    else
    {
        send_message_to_client(":servername 403 " + user.getNickname() + " :No such channel", user);
        return (false);
    }
    return (true);
}