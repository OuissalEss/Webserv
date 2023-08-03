#include "ServerData.hpp"
#include "LocationData.hpp"
#include "parseConfig.hpp"
#include "Server.hpp"
#include "Socket.hpp"
#include <iostream>
#include <vector>

void    hand_signal(int sig)
{
	(void)sig;
    return ;
}

int	main(int argc, char **argv)
{
	std::vector<ws::ServerData> servers;

	try
	{
		if (argc != 2)
			throw ws::ServerData::Error("Error: Wrong number of arguments");
		servers = ws::parseConfigFile(argv[1]);

		ws::checkConfig(servers);

		signal(SIGPIPE, &hand_signal);

		ws::Server server(servers);
		server.startServer(servers);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	// for (std::vector<ws::ServerData>::iterator it = servers.begin(); it != servers.end(); it++)
	// {
	// 	std::cout << "Server            : " << it->getHost() << std::endl;
	// 	std::cout << "Port              : " << it->getPort() << std::endl;
	// 	std::cout << "ServerName        : " << it->getServerName() << std::endl;
	// 	std::cout << "BodySizeLimit     : " << it->getBodySizeLimit() << std::endl;
	// 	std::cout << "DefaultErrorPages : " << it->getDefaultErrorPages() << std::endl;
	// 	std::map<std::string, ws::LocationData>	locations = it->getLocations();
	// 	for (std::map<std::string, ws::LocationData>::iterator it2 = locations.begin(); it2 != locations.end(); it2++)
	// 	{
	// 		std::cout << "Location	   : " << it2->first << std::endl;
	// 		std::cout << "\tRoot	   : " << it2->second.getRoot() << std::endl;
	// 		std::cout << "\tAutoindex  : " << it2->second.getAutoindex() << std::endl;
	// 		std::cout << "\tCgi 	   : " << it2->second.getCgi() << std::endl;
	// 		std::cout << "\tCgiGet	   : " << it2->second.getCgiGet() << std::endl;
	// 		std::cout << "\tCgiPost	   : " << it2->second.getCgiPost() << std::endl;
	// 		std::cout << "\tCgiDelete  : " << it2->second.getCgiDelete() << std::endl;
	// 		std::cout << "\tDefaultPage: " << it2->second.getDefaultPage() << std::endl;
	// 		std::cout << "\tRedirect   : " << it2->second.getRedirect() << std::endl;
	// 	}
	// }
}