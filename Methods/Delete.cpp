/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slaajour <slaajour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 07:19:30 by slaajour          #+#    #+#             */
/*   Updated: 2023/08/02 08:05:08 by slaajour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Request.hpp"
#include "../LocationData.hpp"
#include "../ServerData.hpp"
#include "../Response.hpp"

namespace ws
{
	bool writePerm(const char* filename)
	{
		struct stat fileStat;
		if (stat(filename, &fileStat) != 0)
			return false;
		return (fileStat.st_mode & S_IWUSR) != 0;
	}
	
	void Response::_delete(Request& _request)
	{
		ServerData server = _request.getServer();
		std::cout << "DELETE" << std::endl;
		LocationData loc;
		loc = _request.getMyLocation();
		std::string path = _request.getFinalPath();
		if (isDirectoryMine(path))
		{	
			if (path[path.size() - 1] != '/')
				setStatusCode(409);
			else
				setStatusCode(403);
		}
		else
		{
			if (fileExists(path.c_str()))
			{
				if (writePerm(path.c_str()))
				{
					int deleteResult = std::remove(path.c_str());
					if (deleteResult == 0)
						setStatusCode(204);
					else
						setStatusCode(500);
				}
				else
					setStatusCode(403);
			}
			else
				setStatusCode(404);		
		}
		fileName = server.getDefaultErrorPages() + std::to_string(getStatusCode()) + ".html";
	}
}