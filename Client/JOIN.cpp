/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JOIN.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yochakib <yochakib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 23:05:08 by yochakib          #+#    #+#             */
/*   Updated: 2024/07/31 15:02:18 by yochakib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include "Client.hpp"
#include "../Server/Server.hpp"




bool send_message_to_client(std::string message,Client& user)
{
	message += "\r\n";
	size_t msg_size = message.size();
	const char *buffer = message.c_str();
	ssize_t bytes_sent = send(user.getSocketFd(), buffer, msg_size, 0);
	return (bytes_sent != -1); // needs protection
}

bool is_validchannel_name(const std::string& channel_name)
{
	if (channel_name.empty() || channel_name.size() > 50)
	{
		return (false);
	}
	if (channel_name[0] == '#' && channel_name.size() == 1)
	{
		return false;
	}
	if (channel_name[0] != '#' && channel_name[0] != '&' && channel_name[0] != '+' && channel_name[0] != '!')
	{
    	return false;
	}
	for (size_t i = 1; i < channel_name.length(); ++i) 
	{
        if (channel_name[i] == ' ' || channel_name[i] == '\a' || channel_name[i] == ',' || channel_name[i] == ':' || channel_name[i] == '#' || channel_name[i] == '&' ) 
		{
            return (false);
        }
    }
	return (true);
}
std::string parseChannelName(const std::string& channelName) 
{
    if (!is_validchannel_name(channelName)) 
	{
        return ("");
    }
    return (channelName);
}

bool check_valid_password(std::vector<std::string> parts, std::string channel)
{
	std::vector<std::string> Channel_passwrds;
	std::string passwords = parts[2];
	std::istringstream passIss(passwords);
	std::string channel_pass;
	while (std::getline(passIss, channel_pass, ','))
	{
		Channel_passwrds.push_back(channel_pass);
	}
	for (size_t i = 0; i < Channel_passwrds.size(); ++i) 
	{
		if (Channel_passwrds[i].compare(channel) == 0)
		{
			return (true);
		}
		else
		{
			continue;
		}
	}
	return (false);
}
std::vector<std::string> split_channels(std::string message, Client& user)
{
	std::vector<std::string> channels;
	std::string channel_part = message;
	std::istringstream channelIss(channel_part);
	std::string channel_name;
	while (std::getline(channelIss, channel_name, ',')) 
	{
		std::string valid_channel_name = parseChannelName(channel_name);
		if (!valid_channel_name.empty()) 
		{
			channels.push_back(valid_channel_name);
		}
		else
		{
			send_message_to_client(":server_name 403 " + channel_name + " :No such channel", user);
		}
	}
	return channels;
}
std::vector<std::string> split_passwords(std::string message)
{
	std::vector<std::string> passwords;
	std::string key_part = message;
	std::istringstream PASSIss(key_part);
	std::string key;
	while (std::getline(PASSIss, key, ',')) 
	{
		passwords.push_back(key);
	}
	return passwords;
}

bool Server::handle_JOIN(std::vector<std::string> &parts, Client& user)
{

	
	std::vector<std::string> channels = split_channels(parts[1], user);
	std::vector<std::string> passwords;
	if (parts.size() > 2)
	{
		passwords = split_passwords(parts[2]);
	}
	
    for (size_t i = 0; i < channels.size(); ++i)
    {
		std::string hostname = this->get_ip_address(user.getSocketFd());
		std::string channel_name = channels[i];
		std::string password;
		
		if (passwords.size() > i)
		{
			password = passwords[i];
		}
		Channel* existing_channel = this->findChannelByName(channel_name);
		if (existing_channel != NULL)
		{
			if (existing_channel->hasUser(user))
			{
				send_message_to_client(":servername 443 " + user.getNickname() + existing_channel->getName() + "'.", user);
				return (false);
			}
			else if (existing_channel->hasPassword())
			{
				if (parts.size() == 3 && check_valid_password(parts, existing_channel->getPassword()))
				{
					user.joinChannel((*existing_channel), hostname, *this);
				}
				else
				{
					send_message_to_client(":server_name 475 " + existing_channel->getName() + ":Cannot join channel (+k)", user);
					return (false);	
				}
			}
			else
				user.joinChannel(*existing_channel, hostname, *this);
		}
        else
        {

			Channel new_channel = Channel(channel_name, "");
			if (!password.empty())
			{
				new_channel.setPassword(password);
			}
			new_channel.addOperator(user);
			new_channel.addUser(&user);
			this->add_to_ChannelList(&new_channel);
			user.joinChannel(new_channel, hostname, *this);
        }
	}
	return (true);
}


