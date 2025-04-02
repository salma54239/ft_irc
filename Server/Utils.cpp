/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssaadaou <ssaadaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/11 17:45:24 by ssaadaou          #+#    #+#             */
/*   Updated: 2024/05/11 18:07:58 by ssaadaou         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Server.hpp"


std::vector<std::string> Server::splitMsg(const std::string& msg)
{
    std::vector<std::string> result;
    std::istringstream iss(msg);
    std::string token;
    while ( iss >> token)
        result.push_back(token);
    return result;
}

