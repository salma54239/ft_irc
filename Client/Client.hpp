/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssaadaou <ssaadaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/05 17:58:42 by ssaadaou          #+#    #+#             */
/*   Updated: 2024/07/26 18:49:11 by ssaadaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <set>
#include <string>
#include <iostream>
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <sstream>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <poll.h>
#include <algorithm>

#include "../Client/Channel.hpp"
#include "../Server/Server.hpp"


class Channel;
class Server;

class Client
{
        
    public:
        Client();
        Client(int socketFd) : socketFD_(socketFd), nickname("*"), username(""), realname(""), servername("") ,hostname(""), msg(""), partialMsg("") , i(0),isInvited_(false) ,pass(false), Change_nickname(false), Quit(false), isRegistered(false) {}
        ~Client() {};
        
        //setters
        void setSocketFd(int socketFd) { this->socketFD_ = socketFd; }
        void setNickname(const std::string& nickname) { this->nickname = nickname; }
        void setUsername(const std::string& username) { this->username = username; }
        void setRealname(const std::string& realname) { this->realname = realname; }
        void setHostname(const std::string& hostname) { this->hostname = hostname; }
        void setServername(const std::string& servername) { this->servername = servername; }
        void setMsg(const std::string& msg) { this->msg = msg; }
        void setQuit(bool Quit) { this->Quit = Quit; }
        void setPass(bool pass) { this->pass = pass; }
        void setChange_nickname(bool Change_nickname) { this->Change_nickname = Change_nickname; }
        void set_old_nickname(const std::string& old_nickname) { this->old_nickname = old_nickname; }
        void setRegistered(bool isRegistered) { this->isRegistered = isRegistered; }
        void setAuthenticated(bool Authenticated) { this->Authenticated = Authenticated; }
        void setPartialMsg(const std::string& partialMsg) { this->partialMsg = partialMsg; } 
        bool isOperator() { return this->isAdmin_; }
        void set_i(int i) {this->i = i;}
		int get_i() {return (this->i);}
        
        //getters
        int getSocketFd() const {return this->socketFD_; }
        std::string getNickname() const {return this->nickname; }
        std::string getUsername() const {return this->username; }
        bool getQuit() {return this->Quit; }
        bool getChange_nickname() {return this->Change_nickname; }
        std::string get_old_nickname() {return this->old_nickname; }
        std::string getRealname() const {return this->realname; }
        std::string getHostname() const {return this->hostname; }	
        std::string getServername() const {return this->servername; }
        std::string getMsg() const { return this->msg; }
        bool getAuthenticated() const { return this->Authenticated; }
        std::vector<Channel*>& getChannels() { return this->listofChannels_; }
        std::string getPartialMsg() {return this->partialMsg; }
        bool getPass() const {return this->pass;}
        bool getRegistered() const { return this->isRegistered;}
        void checkRegistration();
        void appendBuffer(const char* buffer);
        // Methods
        bool isInvited()const {return(this->isInvited_);};
        void	setInviteOnly(bool isInvited_);
        void joinChannel(Channel& channel, std::string& hostname, Server& server);
        void Add2ListofChannels(Channel& Channel);
        
    private:
        int socketFD_;
        std::string nickname;
        std::string username;
        std::string realname;
        std::string servername;
        std::string hostname;
        std::string msg;
        std::string old_nickname;
        std::string partialMsg;
        int i;
        bool	isInvited_;
        bool isAdmin_;
        bool pass;
        bool Change_nickname;
        bool Quit;
        bool isRegistered;
        bool Authenticated;
        std::vector<Channel*> listofChannels_;
};

bool send_message_to_client(std::string message,Client& user);
std::string setToString(const std::set<char>& charSet);

#endif