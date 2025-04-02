/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yochakib <yochakib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 14:56:01 by yochakib          #+#    #+#             */
/*   Updated: 2024/07/13 21:25:53 by yochakib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Client.hpp"
#include "../Server/Server.hpp"

Client::Client()
{
}

void	Client::setInviteOnly(bool isInvited)
{
	this->isInvited_ = isInvited;
}

void Client::Add2ListofChannels(Channel& Channel)
{
	this->listofChannels_.push_back(&Channel);
}
std::string setToString(const std::set<char>& charSet) // for Limechat msg
{
    std::string result;
    for (std::set<char>::const_iterator it = charSet.begin(); it != charSet.end(); ++it) 
	{
        result += *it;
    }
    return result;
}
void Client::joinChannel(Channel& channel, std::string& hostname, Server& server)
{
	(void)hostname;
	bool joinSucceeded = false; 
	std::set<char> channelModes = channel.getMode();
	if (channelModes.empty())
	{
		channel.addUser(this);
		this->listofChannels_.push_back(&channel);
		joinSucceeded = true;
	}
	else
	{
		for (std::set<char>::iterator it = channelModes.begin(); it != channelModes.end(); ++it)
		{
			if ((*it) == 'i')
			{
				if (this->isInvited())
				{
					channel.addUser(this);
					this->Add2ListofChannels(channel);
					joinSucceeded = true;
				}
				else
				{
					send_message_to_client(":server_name 473 " + channel.getName() + ":Cannot join channel (+i) '.", *this);
					return;
				}
			}
			else if ((*it) == 'l')
			{
				if (channel.getUsers().size() < channel.get_userLimit())
				{
					channel.addUser(this);
					this->Add2ListofChannels(channel);
					joinSucceeded = true;
				}
				else
				{
					send_message_to_client(":server_name 471 " + channel.getName() + " Cannot join channel (+l).", *this);
					return;
				}
			}
			else 
			{
				channel.addUser(this);
				this->Add2ListofChannels(channel);
				joinSucceeded = true;
			}
		}
	}
	if (joinSucceeded)
	{
		std::vector<Client*> users = channel.getUsers();
		server.broadCast(":" + this->getNickname() + "!~" + this->getRealname() + "@" + hostname + " JOIN " + channel.getName(), *this , channel.getName());
		send_message_to_client(":servername MODE " + channel.getName() + " : " + setToString(channel.getMode()), *this);
		if (channel.getTopic().empty())
			send_message_to_client(":server_name 331 " + this->getNickname() + " " + channel.getName() + " :No topic is set", *this);
		else
			send_message_to_client(":server_name 332 " + this->getNickname() + " " + channel.getName() + " :" + channel.getTopic(), *this);
		std::string namesList = ":server_name 353 " + this->getNickname() + " = " + channel.getName() + " :";
		if (this->getChange_nickname() == true)
		{
			for (size_t i = 0; i < users.size(); ++i)
			{
				if (users[i]->getNickname() == this->get_old_nickname())
				{
					users[i]->setNickname(this->getNickname());
					break;
				}
			}
			for (size_t j = 0; j < users.size(); ++j)
			{
				if (channel.isOperator(*users[j]))
					namesList += "@";
				namesList += users[j]->getNickname();
				if (j != users.size() - 1)
					namesList += " ";
			}
		}
		else
		{
			std::vector<Client*> users = channel.getUsers();
			for (size_t i = 0; i < users.size(); ++i)
			{
				if (channel.isOperator(*users[i]))
					namesList += "@";
				namesList += users[i]->getNickname();
				if (i != users.size() - 1)
					namesList += " ";
			}
		}
		send_message_to_client(namesList, *this);
		send_message_to_client(":server_name 366 " + this->getNickname() + " " + channel.getName() + " :End of NAMES list", *this);
	}
}