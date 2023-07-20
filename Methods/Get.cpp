/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slaajour <slaajour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 07:18:12 by slaajour          #+#    #+#             */
/*   Updated: 2023/07/20 08:49:44 by slaajour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Request.hpp"
#include "../LocationData.hpp"
#include "../ServerData.hpp"
#include "../Response.hpp"

void ws::Response::get(int socket, Request& _request)
{
	LocationData loc;
	loc = _request.getMyLocation();
    std::string path = _request.getFinalPath();
	if (isDirectoryMine(path))
	{
		if (path[path.size() - 1] != '/')
		{
			path += '/';
            this->setStatusCode(301);
            ws::sendResponse(socket, getStatusCode(), "hello 301");
			return ;
		}
		else
		{
			if (loc.getDefaultPage().empty())
			{
				if (loc.getAutoindex() == true)
				{
                    setStatusCode(200);
					std::string autoindex = "<html><body><h1 style='color:pink;'> Salma 🐹 </h1><h3 style='color:blue;'> Ouissal 🐾 </h3><hr><p>Hello world</p></body></html>";
					ws::sendResponse(socket, getStatusCode(), autoindex);
					return ;
				}
				else
                {
                    setStatusCode(403);
                    ws::sendResponse(socket, getStatusCode(), "hello no autoindex");
					return ;
                }
			}
			else
			{
				if(loc.getCgi() == true)
				{
					//run cgi on requested file with GET REQUEST_METHOD
					// Return Code Depend on cgi
				}
				else
				{
					setStatusCode(200);
                    ws::sendResponse(socket, getStatusCode(), "hello ok moi?");
					return ;
				}
			}
		}
	}
	else
	{
		if(loc.getCgi() == true)
		{
			//run cgi on requested file with GET REQUEST_METHOD
			// Return Code Depend on cgi
		}
		else
		{
			setStatusCode(200);
            ws::sendResponse(socket, getStatusCode(), "hello ok");
			return ;
		}
	}
}