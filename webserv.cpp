#include "ServerData.hpp"
#include "LocationData.hpp"
#include "parseConfig.hpp"
#include "Server.hpp"
#include "Socket.hpp"
#include <iostream>
#include <vector>

int	main(int argc, char **argv)
{
	std::vector<ws::ServerData> servers;

	try
	{
		if (argc != 2)
			throw ws::ServerData::Error("Error: Wrong number of arguments");
		servers = ws::parseConfigFile(argv[1]);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	try
	{
		ws::Server server(servers);
		server.startServer();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	// for (std::vector<ServerData>::iterator it = servers.begin(); it != servers.end(); it++)
	// {
	// 	std::cout << "Server            : " << it->getHost() << std::endl;
	// 	std::cout << "Port              : " << it->getPort() << std::endl;
	// 	std::cout << "ServerName        : " << it->getServerName() << std::endl;
	// 	std::cout << "BodySizeLimit     : " << it->getBodySizeLimit() << std::endl;
	// 	std::cout << "DefaultErrorPages : " << it->getDefaultErrorPages() << std::endl;
	// 	std::map<std::string, LocationData>	locations = it->getLocations();
	// 	for (std::map<std::string, LocationData>::iterator it2 = locations.begin(); it2 != locations.end(); it2++)
	// 	{
	// 		std::cout << "Location	   : " << it2->first << std::endl;
	// 		std::cout << "\tRoot	   : " << it2->second.getRoot() << std::endl;
	// 		std::cout << "\tAutoindex  : " << it2->second.getAutoindex() << std::endl;
	// 		std::cout << "\tCgiGet	   : " << it2->second.getCgiGet() << std::endl;
	// 		std::cout << "\tCgiPost	   : " << it2->second.getCgiPost() << std::endl;
	// 		std::cout << "\tCgiDelete  : " << it2->second.getCgiDelete() << std::endl;
	// 		std::cout << "\tDefaultPage: " << it2->second.getDefaultPage() << std::endl;
	// 	}
	// }
}