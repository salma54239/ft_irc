/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssaadaou <ssaadaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/20 16:04:14 by ssaadaou          #+#    #+#             */
/*   Updated: 2024/07/24 22:28:20 by ssaadaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <string>
#include <sstream>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <poll.h>
#include <stdio.h>
#include <map>
#include <fcntl.h>
#include "../Client/Client.hpp"
#include <fstream>
#include <arpa/inet.h>

class Channel;
class Client;

class Server
{
	private:
		int fd;
		char *ip;
		int port;
		std::string passwrd;
		std::vector<pollfd> poll_fd;
		std::map<int, Client> clients;
		std::vector<Channel> AllChannels_;

	public:
		Server();
		~Server();
		void run();
		void parseInput(int ac, char **av);
		void setSocket();
		int acceptClient();
		void sendMessage(int clientFd, const std::string &message);
		int receiveMessage(int clientFd);
		void handleCommand(int clientFd);
		std::vector<std::string> splitMsg(const std::string &msg);
		bool NicknameExists(const std::string &nickname);
		bool isValidNickname(const std::string &nickname);
		bool isValidUser(const std::string &username);
		std::string get_ip_address(int fd);
		std::string get_current_time();
		void sendError(int clientFd, const std::string &num, const std::string &msg);

		Channel* findChannelByName(const std::string& channel_name);
		std::map<int, Client>& getClients() {return (clients);}
		bool parse_command(std::string &message, Client& user);
		std::vector<Channel>& getAllChannels() { return AllChannels_;}
		void add_to_ChannelList(Channel* new_channel);
		Client* findClientByNickname(const std::string& nickname);
		bool handle_JOIN(std::vector<std::string> &parts, Client& user);
		bool handle_MODE(std::vector<std::string> &parts, Client& user);
		bool handle_TOPIC(std::vector<std::string> &parts, Client& user);
		bool handle_PRIVMSG(std::vector<std::string> &parts, Client& user);
		bool handle_KICK(std::vector<std::string>& parts, Client& user);
		bool handle_INVITE(std::vector<std::string>& parts, Client& user);
		void broadCast(std::string message,Client& users,std::string channel);
		void handleQuit(int clientFd);
};

bool send_message_to_client(std::string message,Client& user);
bool is_validchannel_name(const std::string& channel_name);
bool check_valid_password(std::vector<std::string> parts, std::string channel);
bool findClient_inChannel(std::vector<Channel> &tab, Client &user, std::string channel_name);
std::string parseChannelName(const std::string& channelName);
std::vector<std::string>	split_user_input(std::string &input);

#endif
