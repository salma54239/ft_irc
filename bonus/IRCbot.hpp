/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCbot.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yochakib <yochakib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/23 11:29:57 by yochakib          #+#    #+#             */
/*   Updated: 2024/06/23 18:08:38 by yochakib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCBOT_HPP
# define IRCBOT_HPP
#include "../Client/Client.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

class IRCbot 
{
    public:
    IRCbot();
    IRCbot(const std::string &server, int port, const std::string &user, const std::string &nick);
    ~IRCbot();

    void set_socket();
    void setusername(const std::string &user);
    void setnickname(const std::string &nick);
    void setrealname(const std::string &real);
    void sethostname(const std::string &host);
    void set_args(int port, std::string server_password, std::string ip);
    void connect_to_server();
    void run();

    private:
    
    int port;                  // IRC server port
    std::string server;        // IRC server address
    std::string ip;      // IP address of the server


    std::string			username;
    std::string			realname;
    std::string			nickname;
    std::string			hostname;

    int sockfd;                // Socket file descriptor
    
    void handleServerMessage(const std::string &msg);
};

#endif