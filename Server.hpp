#ifndef SERVER_HPP
# define SERVER_HPP
# include "ServerData.hpp"
# include "Socket.hpp"
# include "Request.hpp"
# include <vector>
# include <map>
# include <iostream>
# include <stdlib.h>
# include <list>
# include <sys/poll.h>

namespace ws
{
    class Server
    {
        private :
            std::vector<struct pollfd>  fds;
            std::vector<Socket>         sockets;
            std::map<int, ws::Request>  requests;
            std::map<int, std::string>  requestMap;
        public :
            
            /*      Constructor       */
            Server(std::vector<ServerData> data)
            {
                struct pollfd 	fd;
                int				sd;

                for (size_t i = 0; i < data.size(); i++)
                {
                    Socket socket = Socket(data[i].getHost().c_str(), data[i].getPort(), data[i]);
                    socket.startSocket();
                    sd = socket.getSocketD();
                    if (sd < 0)
                    {
                        std::cerr << "Error, socket failed." << std::endl;
                        exit(1);
                    }
                    fd.fd = sd;
                    fd.events = POLLIN;
                    fds.push_back(fd);
                    sockets.push_back(socket);
                }
            }

            /*      Destructor        */
            ~Server() {}

            /*        METHODES        */
            Socket *findSocket(int sd)
            {
                for (size_t i = 0; i < fds.size(); i++)
                {
                    if (sd == sockets[i].getSocketD())
                        return (&(sockets[i]));
                }
                return (nullptr);
            }

            Socket *findClient(int sd)
            {
                for (size_t i = 0; i < sockets.size(); i++)
                {
                    for (size_t j = 0; j < sockets.at(i).getClients().size(); j++)
                    {
                        if (sd == sockets.at(i).getClient(j))
                            return (&sockets.at(i));
                    }
                }
                return (nullptr);
            }

            void		acceptClient(int i)
            {
                struct pollfd 	fd;
                int				new_sd;

                do
                {
                    new_sd = findSocket(fds[i].fd)->acceptSocket();
                    if (new_sd < 0)
                        break;
                    fd.fd = new_sd;
                    fd.events = POLLIN;
                    fd.revents = POLLIN;
                    fds.push_back(fd);
                } while (new_sd >= 0);
            }

            bool isServer(int sd)
            {
                for (size_t i = 0; i < sockets.size(); i++)
                {
                    if (sd == sockets[i].getSocketD())
                        return (true);
                }
                return (false);
            }

            bool		recvData(int i)
            {
                std::string     requestText;
                char 	        buffer[30000];
                int	 	        rc = 0;

                std::memset(&buffer, 0, sizeof(buffer));
                ws::Socket *socket = findClient(fds[i].fd);
                // std::cout << "***********RECV**********" << std::endl;

                rc = recv(fds[i].fd, buffer, (sizeof(buffer) - 1), 0);
                if (rc == -1)
                    return (false);

                if (rc == 0) {
                    std::cout << "Connection closed." << std::endl;
                    return (false);
                }
                /* Check if request has finished receiving data      */
                requestMap[fds[i].fd].append(buffer, rc);
                requests[fds[i].fd].setServer(socket->getConfig());
                requests[fds[i].fd].setBody(requestMap[fds[i].fd]);
                requests[fds[i].fd].set_inittt();
				if (requests[fds[i].fd].get_init() != 0)
                {
                    std::cout << "***********FINISHED**********" << std::endl;
					fds[i].revents = POLLOUT;
                    requests[fds[i].fd].get_matched();
                    std::memset(&requestMap[fds[i].fd], 0, sizeof(requestMap[fds[i].fd]));
                }
                // std::cout << "***********END RECV**********" << std::endl;
                return (true);
            }

            bool	sendData(int i)
            {
                // get response
                std::cout << "*********SEND RESPONSE********" << std::endl;
                if (requests[fds[i].fd].get_method() == 1 && requests[fds[i].fd].get_status() == 200)
                {
                    // method GET
                    std::cout << "***********   GET   **********" << std::endl;
                    LocationData   location = requests[fds[i].fd].getMyLocation();
                    std::cout << "\tRoot       : " << location.getRoot() << std::endl;
                    std::cout << "\tAutoindex  : " << location.getAutoindex() << std::endl;
                    std::cout << "\tCgiGet       : " << location.getCgiGet() << std::endl;
                    std::cout << "\tCgiPost       : " << location.getCgiPost() << std::endl;
                    std::cout << "\tCgiDelete  : " << location.getCgiDelete() << std::endl;
                    std::cout << "\tDefaultPage: " << location.getDefaultPage() << std::endl;
                }
                else if (requests[fds[i].fd].get_method() == 2 && requests[fds[i].fd].get_status() == 200)
                {
                    //if (check_upload_support(this->final_path))
                        requests[fds[i].fd].upload();
                    //else
			            // requests[fds[i].fd].post();
                }
                else if (requests[fds[i].fd].get_method() == 3 && requests[fds[i].fd].get_status() == 200)
                {
                    // method DELETE
                }
                else
                {
                    // error
                }
                std::cout << "**********CLOSE SOCKET********" << std::endl;
                ws::Socket *socket = findClient(fds[i].fd);
                requests.erase(fds[i].fd);
                socket->removeClient(fds[i].fd);
                close(fds[i].fd);
                fds[i].fd = -1;
                fds.erase(fds.begin() + i);
                return (true);
            }

            void startServer(void)
            {
                int p = 1;
                int timeout = (60 * 60 * 1000);

                std::cout << " Servers are starting..." << std::endl;
                while (true)
                {
                    p = poll(&fds.front(), fds.size(), timeout);
                    
                    if (p < 0)
                    {
                        std::cerr << "Error, Poll failed." << std::endl;
                        break;
                    }
                    if (p != 0)
                    {
                        size_t size = fds.size();
                        for (size_t i = 0; i < size; i++) 
                        {
                            if (fds[i].revents == 0)
                                continue;
                            if (isServer(fds[i].fd))
                                acceptClient(i);
                            else if (fds[i].revents & POLLERR || fds[i].revents & POLLNVAL || fds[i].revents & POLLHUP)
                            {
                                ws::Socket *socket = findClient(fds[i].fd);
                                socket->removeClient(fds[i].fd);
                                close(fds[i].fd);
                                fds[i].fd = -1;
                                fds.erase(fds.begin() + i);
                            }
                            else
                            {
                                if (fds[i].revents == POLLIN)
                                {
                                    if (!recvData(i))
                                        fds[i].events = POLLHUP;
                                }
                                if (fds[i].revents == POLLOUT)
                                {
                                    if (!sendData(i))
                                        fds[i].events = POLLHUP;
                                }
                            }
                        }
                    }
                }
                // Close all the sockets that are open
                for (size_t i = 0; i < fds.size(); i++)
                {
                    if (fds[i].fd >= 0)
                        close(fds[i].fd);
                }
		    }
    };
}

#endif
