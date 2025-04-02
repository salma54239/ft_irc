/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yochakib <yochakib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 20:03:00 by ssaadaou          #+#    #+#             */
/*   Updated: 2024/06/24 14:13:42 by yochakib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"


int main(int ac, char **av)  
{ 
    try 
    {
        Server server;
        server.parseInput(ac, av);
        server.setSocket();
        server.run();
    }
    catch (std::exception &e) 
    {
        std::cerr << e.what() << std::endl;
    }
}
