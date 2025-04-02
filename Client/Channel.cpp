/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yochakib <yochakib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 16:55:31 by yochakib          #+#    #+#             */
/*   Updated: 2024/07/31 15:01:31 by yochakib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Channel.hpp"
#include "../Server/Server.hpp"

std::string Channel::getName() const
{
	return (this->channel_name);
}

std::string Channel::getTopic() const
{
	return (this->topic);
}

size_t Channel::get_userLimit() const
{
	return (this->limit);
}

std::set<char> Channel::getMode() const
{
	return (this->modes);
}

std::vector<Client*>& Channel::getUsers()
{
	return (this->users_list);
}

std::vector<Client*>& Channel::getOperators()
{
	return (this->channel_operators);
}

void Channel::addUser(Client* client)
{
    if (!hasUser(*client)) 
	{ 
        users_list.push_back(client);
    }
}

void Channel::addOperator(Client& client) 
{
    if (!isOperator(client)) 
	{
        channel_operators.push_back(&client);
    }
}

void Channel::removeUser(Client* client) 
{
	for (std::vector<Client*>::iterator it = users_list.begin(); it != users_list.end(); ++it) 
	{
		if ( (*it)->getSocketFd() == client->getSocketFd())
		{
			users_list.erase(it);
			return;
		}
	}
}


void Channel::removeOperator(Client& client) 
{
	for (std::vector<Client*>::iterator it = channel_operators.begin(); it != channel_operators.end(); ++it) 
	{
		if( (*it)->getSocketFd() == client.getSocketFd())  
		{
			channel_operators.erase(it);
			break;
		}
	}
}

void Channel::setTopic(const std::string& topic)
{
	this->topic = topic;
}

void Channel::setPassword(const std::string& pass) 
{
    this->key = pass;
}

void Channel::setTopicLocked(bool is_locked) 
{
    topicLocked = is_locked;
}

bool Channel::hasTopicLoked() const
{
	return topicLocked;
}

void Channel::setTopicAuthor(const std::string& author)
{
	this->topic_author = author;
}
void Channel::setLimit(const int channelLimit)
{
	this->limit = channelLimit;
}

void Channel::setMode(const char mode, bool adding, const std::string& param, Client& user, Server& server) 
{
    if (adding)
	{
		this->modes.insert(mode);
		if (mode == 'k')
		{ 
			if (param.empty())
			{
				send_message_to_client(":servername 461 Mode +k requires a parameter", user);
				return;
			}
			this->setPassword(param);
			server.broadCast(":" + user.getNickname()+ "!~" + "SERVER_NAME" + " MODE " + this->getName() + " +k " + param, user, this->getName());
		}
		else if (mode == 'l')
		{
			if (param.empty())
			{
				send_message_to_client(":servername 461 Mode +l requires a parameter", user);
				return;
			}
			this->setLimit(std::atoi(param.c_str()));
			server.broadCast(":" + user.getNickname()+ "!~" + "SERVER_NAME" + " MODE "+ this->getName() + " +l " + param, user, this->getName());
		}
		else if (mode == 't')
		{
			this->setTopicLocked(true);
			server.broadCast(":" + user.getNickname()+ "!~" + "SERVER_NAME" + " MODE "+ this->getName() +  " +t", user, this->getName());
		}
		else if (mode == 'i')
		{
			this->setInvite(true);
			server.broadCast(":" + user.getNickname()+ "!~" + "SERVER_NAME" + " MODE " + this->getName() + " +i", user, this->getName());
		}
		else if (mode == 'o')
		{
			if (param.empty())
			{
				send_message_to_client(":servername 461 Mode +o requires a parameter", user);
				return;
			}
			Client* target = this->findClientByNickname_(param);
			if (target)
			{
				this->addOperator(*target);
				server.broadCast(":" + user.getNickname()+ "!~" + "SERVER_NAME" + " MODE " + this->getName()  + " +o " + param, user, this->getName());
			}
			else if (target == NULL)
			{
				send_message_to_client(":servername 472 No such nick/channel: " + param, user);
			}
		}
		else if (mode == 's' || mode == 'n')
		{
			modes.erase(mode);
		}
		else
		{
			modes.erase(mode);
			send_message_to_client(":servername 472 Unknown mode: " + std::string(1, mode), user);
		}
	}
	else
	{
		modes.erase(mode);
		if (mode == 'k')
		{
			this->key.clear();
			server.broadCast(":" + user.getNickname()+ "!~" + "SERVER_NAME" + " MODE " + this->getName() + " -k", user, this->getName());
		}
		else if (mode == 'l')
		{
			this->limit = 0;
			server.broadCast(":" + user.getNickname()+ "!~" + "SERVER_NAME" + " MODE " + this->getName() + " -l", user, this->getName());
		}
		else if (mode == 't')
		{
			this->setTopicLocked(false);
			server.broadCast(":" + user.getNickname()+ "!~" + "SERVER_NAME" + " MODE "+ this->getName() +" -t", user, this->getName());
		}
		else if (mode == 'i')
		{
			this->setInvite(false);
			server.broadCast(":" + user.getNickname()+ "!~" + "SERVER_NAME" + " MODE " + this->getName() + " -i", user, this->getName());
		}
		else if (mode == 'o')
		{
			if (param.empty())
			{
				send_message_to_client(":servername 461 Mode -o requires a parameter", user);
				return;
			}
			Client* target = this->findClientByNickname_(param);
			if (target)
			{
				bool was_operator = this->isOperator(*target);
                if (was_operator)
                {
                    this->removeOperator(*target);
                }
                this->removeUser(target);
                // Handle operator reassignment if needed
                if (was_operator && this->getOperators().empty() && !this->getUsers().empty())
                {
                	std::vector<Client*>& users = this->getUsers();
                	std::vector<Client*>::iterator it = users.begin();
                    if (it != users.end())
                    {
                        Client* new_operator = *it;
                        this->addOperator(*new_operator); 
                            // Notify about new operator
						std::string op_message = ":SERVER_NAME MODE " + this->getName() + " +o " + new_operator->getNickname();
                        server.broadCast(op_message, user, this->getName());
							
					}
				}
				this->removeOperator(*target);
				server.broadCast(":" + user.getNickname()+ "!~" + "SERVER_NAME" +  " MODE " + this->getName() + " -o " + param, user, this->getName());
			}
			else if (target == NULL)
			{
				send_message_to_client(":servername 472 No such nick/channel: " + param, user);
			}
		}
		else
		{
			send_message_to_client(":servername 472 Unknown mode: " + std::string(1, mode), user);
		}
	}
}

std::string Channel::getTopicAuthor() const
{
	return (this->topic_author);
}

std::string Channel::getPassword() const
{
	return (this->key);
}

void Channel::kickUser(Client& client)
{
	removeUser(&client);
}

bool Channel::hasInviteOnly() const
{
	return (this->Is_InviteOnly);
}

void Channel::setInvite(bool invited)
{
	this->Is_InviteOnly = invited;
}

void Channel::inviteUser(Client& client)
{
    client.setInviteOnly(true);
}

bool Channel::hasPassword () const
{
	return (!key.empty());
}

bool Channel::hasUser(const Client& client)
{
    for (std::vector<Client*>::const_iterator it = users_list.begin(); it != users_list.end(); ++it) 
	{
		if ((*it)->getNickname() == client.getNickname())
		{
            return true;
        }
    }
    return false;
}

bool Channel::isOperator(Client& client)
{
    for (std::vector<Client*>::const_iterator it = channel_operators.begin(); it != channel_operators.end(); ++it) 
	{
        if ((*it)->getNickname() == client.getNickname())
		{
            return true;
        }
    }
    return false;
}
	



Client* Channel::findClientByNickname_(const std::string& nickname)
{
	for (std::vector<Client *>::iterator it = users_list.begin(); it != users_list.end(); ++it) 
	{
		if ((*it)->getNickname() == nickname) 
		{
			return (*it);
		}
	}
	return NULL;
}