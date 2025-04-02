/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yochakib <yochakib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 21:14:30 by yochakib          #+#    #+#             */
/*   Updated: 2024/07/25 01:22:41 by yochakib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Client *Server::findClientByNickname(const std::string &nickname)
{
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second.getNickname() == nickname)
		{
			return &it->second;
		}
	}
	return NULL;
}

std::vector<std::string>	split_user_input(std::string &input)
{
	std::vector<std::string> parts;
	std::string part;
	std::istringstream iss(input);

	while (std::getline(iss, part, ' '))
	{
		parts.push_back(part);
	}
	return (parts);
}


bool Server::parse_command(std::string &message, Client& user)
{
	std::vector<std::string> parts = split_user_input(message);
	std::string command = parts[0];
	if (command == "JOIN" && parts.size() >= 2)
	{
		handle_JOIN(parts, user);
	}
	else if (command == "JOIN" && parts.size() < 2)
	{
		send_message_to_client(":servername 461 JOIN :Not enough parameters", user);
		return (false);
	}
	else if (command == "MODE" && parts.size() >= 2)
	{
		handle_MODE(parts, user);
	}
	else if (command == "MODE" && parts.size() < 2)
	{
		send_message_to_client(":servername 461 MODE :Not enough parameters", user);
		return (false);
	}
	else if (command == "TOPIC" && parts.size() >= 2)
	{
		handle_TOPIC(parts, user);
	}
	else if (command == "TOPIC" && parts.size() < 2)
	{
		send_message_to_client(":servername 461 TOPIC :Not enough parameters", user);
		return (false);
	}
	else if (command == "PRIVMSG" && parts.size() >= 3)
	{
		handle_PRIVMSG(parts, user);
	}
	else if (command == "PRIVMSG" && parts.size() < 3)
	{
		send_message_to_client(":servername 461 PRIVMSG :Not enough parameters", user);
		return (false);
	}
	else if (command == "KICK" && parts.size() >= 3)
	{
		handle_KICK(parts, user);
	}
	else if (command == "KICK" && parts.size() < 3)
	{
		send_message_to_client(":servername 461 KICK :Not enough parameters", user);
		return (false);
	}
	else if (command == "INVITE" && parts.size() >= 3)
	{
		handle_INVITE(parts, user);
	}
	else if (command == "INVITE" && parts.size() < 3)
	{
		send_message_to_client(":servername 461 INVITE :Not enough parameters", user);
		return (false);
	}
	else
	{
		std::string command = parts[0];
		send_message_to_client(":servername 421 " + user.getNickname() + " :Unknown command", user);
		return (false);
	}
	return (true);
}

 
void Server::add_to_ChannelList(Channel* new_channel)
{
	std::string name = new_channel->getName();
	if (!this->findChannelByName(name))
	{
		this->getAllChannels().push_back(*new_channel);
	}
}