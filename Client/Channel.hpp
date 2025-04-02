/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yochakib <yochakib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 20:53:44 by yochakib          #+#    #+#             */
/*   Updated: 2024/07/13 21:30:15 by yochakib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"
#include "Channel.hpp"
#include "../Server/Server.hpp"

class Client;
class Server;
bool send_message_to_client(std::string message,Client& user);

class Channel 
{
	public:
        Channel(const std::string& name, const std::string& topic)
        	: channel_name(name), channel_topic(topic) ,topicLocked(false), limit(0) 
		{Allchannels_.push_back(*this);}
		~Channel() {};

		//Getters
		std::string getName() const;
		std::string getTopic() const;
		std::set<char> getMode() const;
		std::vector<Client*>& getUsers();
		std::vector<Client*>& getOperators();
		std::string	getPassword() const;
		size_t get_userLimit() const;
		std::string getTopicAuthor() const;

		//Setters
    	void setMode(const char mode, bool adding, const std::string& param, Client& user, Server& server);
    	void setPassword(const std::string& pass);
		void setLimit(const int channelLimit);
		void setInvite(bool invited);
    	void setTopicLocked(bool is_locked);
		void setTopicAuthor(const std::string& author);
		void setTopic(const std::string& topic);
	
		//Methods
    	void addUser(Client* client);
    	void addOperator(Client& client);
    	void removeUser(Client* client);
    	void removeOperator(Client& client);
    	void kickUser(Client& client);
    	void inviteUser(Client& client);
		Client* findClientByNickname_(const std::string& nickname);
		
		bool hasTopicLoked() const;
		bool hasInviteOnly() const;
    	bool hasPassword() const;
    	bool hasUser(const Client& client);
   		bool isOperator(Client& client);

	private:

		bool Is_InviteOnly;
    	std::string channel_name;
    	std::string channel_topic;
    	std::vector<Client*> users_list;
    	std::vector<Client*> channel_operators;
		std::vector<Channel> Allchannels_;
    	std::set<char> modes;
		std::string	   key;
		std::string	   topic;
		std::string	   topic_author;
		bool topicLocked;
    	int limit; 
		
};



#endif