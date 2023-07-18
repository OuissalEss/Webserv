#ifndef SOCKET_HPP
# define SOCKET_HPP
# include "ServerData.hpp"
# include <cstdio>
# include <sys/socket.h>
# include <netinet/in.h>
# include <cstring>
# include <unistd.h>
# include <cstdlib>
# include <iostream>
# include <sys/event.h>
# include <arpa/inet.h>
# include <vector>
# include <fcntl.h>

namespace ws
{
    class Socket
    {
        private:
            int					socketD;
            std::vector<int>    clients;
            char const          *ipaddr;
            uint const          port;
            struct sockaddr_in	address;
            ws::ServerData		config;

        public:

            /*      Constructor       */
            Socket(char const *ipaddr, uint port, ws::ServerData &config) : socketD(0), clients(0), ipaddr(ipaddr), port(port), address(), config(config)
            {
                std::memset(&address, 0, sizeof(address));
				std::memset(&address.sin_zero, 0, sizeof(address.sin_zero));
				address.sin_family = AF_INET;
				address.sin_port = htons(port);
                address.sin_addr.s_addr = htonl(INADDR_ANY);
            }

            /*      Destructor        */
            ~Socket() {}

            /*        GETTERS         */
            int					getSocketD( void ) const { return (socketD); }
            std::vector<int> 	getClients( void ) const { return (clients); }
            int					getClient( int index ) const { return (clients.at(index)); }
            ws::ServerData		getConfig( void ) const { return (config); }

            /*        SETTERS         */
            bool    setClient(int clientsd)
            {
                for (size_t i = 0 ; i < clients.size() ; i++) {
					if (clientsd == clients[i]) {
						std::cout << "Client is already in the list." << std::endl;
						return (false);
					}
				}
				clients.push_back(clientsd);
				return (true);
            }

            /*        METHODES        */
            bool	removeClient(int clientsd) {
				std::vector<int>::iterator it = clients.begin();
				for (;it != clients.end();++it) {
					if (*it == clientsd)
						break;
				}
				clients.erase(it);
				return (true);
			}

            void	createSocket()
            {
                socketD = socket(AF_INET, SOCK_STREAM, 0);
                if (socketD < 0)
                {
                    std::cout << "Error can't create socket." << std::endl;
					return ;
				}
				std::cout << "Socket is created successfully." << std::endl;
            }

            void	bindSocket()
            {
                if (bind(socketD, (struct sockaddr *)&address, sizeof(address)) < 0)
				{
					std::cout << "Can't bind socket." << std::endl;
					return ;
				}
				std::cout << "Socket is binded successfully." << std::endl;
            }

            void	listenSocket()
            {
                if (listen(socketD, 10000) < 0)
                {
					std::cout << "Error can't listen on socket." << std::endl;
					return ;
				}
				std::cout << "Listening on " << ipaddr << ":" << port << std::endl;
            }

            int		acceptSocket(void)
            {
                int	clientsd;
			
				clientsd = accept(socketD, NULL, NULL);
				if (clientsd != -1)
					setClient(clientsd);
				return (clientsd);
            }

            void    startSocket()
            {
                int x = 1;

                createSocket();
                if (setsockopt(socketD, SOL_SOCKET, SO_REUSEADDR, &x, sizeof(x)) < 0)
				{
					std::cerr << "Setsockopt failed." << std::endl;
					close(socketD);
					exit(-1);
				}
				if (fcntl(socketD, F_SETFL, O_NONBLOCK) < 0) 
				{
					std::cerr << "fcntl failed." << std::endl;
					close(socketD);
					exit(-1);
				}
				bindSocket();
				listenSocket();
            }
    };
};

#endif