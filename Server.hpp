#ifndef SERVER_HPP
# define SERVER_HPP
# include "ServerData.hpp"
# include "Socket.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include <vector>
# include <map>
# include <iostream>
# include <stdlib.h>
# include <list>
# include <sys/poll.h>
#include <fstream>

namespace ws
{
    class Server
    {
        private :
            std::vector<struct pollfd>  fds;
            std::vector<Socket>         sockets;
            std::map<int, Request>      requests;
            std::map<int, Response>     responses;
            std::map<int, std::string>  requestMap;
            std::vector<ServerData>     servers;

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

                // std::cout << "New connection, socket fd is " << fds[i].fd << std::endl;
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
                // std::cout << "Connection accepted." << std::endl;
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

                rc = recv(fds[i].fd, buffer, (sizeof(buffer) - 1), 0);
                std::cout << "buffer: " << buffer << std::endl;
                /* Check to see if the recv failed. */
                if (rc == -1)
                    return (false);

                /* Check to see if the connection has been closed by the client */
                if (rc == 0) {
                    std::cout << "Connection closed." << std::endl;
                    return (false);
                }

                requestMap[fds[i].fd].append(buffer, rc);

                /* check if request has surpassed the limit */
                for (int k = 0; k < rc; ++k)
                {
                    if (requestMap[fds[i].fd].size() > socket->getConfig().getBodySizeLimit() * 1000000)
                    {
                        std::cout << "********* LIMIT ********" << std::endl;
                        requests[fds[i].fd].set_status(413);
                        requests[fds[i].fd].set_init(1);
                        return (true);
                    }
                }

                requests[fds[i].fd].setServer(socket->getConfig());
                requests[fds[i].fd].setBody(requestMap[fds[i].fd]);
                if (requests[fds[i].fd].get_init() == 0)
                    requests[fds[i].fd].set_inittt();
                requests[fds[i].fd].get_matched(requests[fds[i].fd].getLocation());

                if (requests[fds[i].fd].get_status() == 200 && requests[fds[i].fd].get_method() == 2)
                {
                    if (requests[fds[i].fd].get_body_kind() == 1 && requests[fds[i].fd].getMyLocation().getUpload() == true)
                    {
                        std::cout << "********* upload 1 ********" << std::endl;
					    requests[fds[i].fd].upload1(requests[fds[i].fd].getBody());
                    }
				    else if (requests[fds[i].fd].get_body_kind() == 2 && requests[fds[i].fd].getMyLocation().getUpload() == true)
                    {
                        std::cout << "********* upload 2 ********" << std::endl;
					    requests[fds[i].fd].upload2(requests[fds[i].fd].getBody());
                    }
                }
                else
                    requests[fds[i].fd].set_init(1);
                
                /*  */
                std::map<std::string, std::string>	header_map = requests[fds[i].fd].get_header_map();
                if (header_map["\rHost"].size() > 0)
                {
                    if(socket->getConfig().getServerName() != header_map["\rHost"])
                    {
                        for (size_t i = 0; i < servers.size(); i++)
                        {
                            if (servers[i].getServerName() == header_map["\rHost"])
                            {
                                requests[fds[i].fd].setServer(servers[i]);
                                break;
                            }
                        }
                    }
                }

                /* Check if request has finished receiving data */
				if (requests[fds[i].fd].get_init() != 0)
                {
					fds[i].events = POLLOUT;
                    std::memset(&requestMap[fds[i].fd], 0, sizeof(requestMap[fds[i].fd]));
                    requestMap[fds[i].fd].clear();
                }
                return (true);
            }

            bool	sendData(int i)
            {
                /* get response from response class */
                // std::cout << "PATH ====== " << requests[fds[i].fd].getFinalPath() << std::endl;
                if (responses[fds[i].fd].getFirstTime() == true)
                {
                    if (requests[fds[i].fd].get_method() == 1 && requests[fds[i].fd].get_status() == 200)
                    {
                        std::cout << "********* GET ********" << std::endl;
                        responses[fds[i].fd].get(requests[fds[i].fd]);
                    }
                    else if (requests[fds[i].fd].get_method() == 2 && requests[fds[i].fd].get_status() == 200)
                    {
                        std::cout << "********* POST ********" << std::endl;
                        responses[fds[i].fd].post(requests[fds[i].fd]);
                    }
                    else if (requests[fds[i].fd].get_method() == 3 && requests[fds[i].fd].get_status() == 200)
                    {
                        std::cout << "********* DELETE ********" << std::endl;
                        responses[fds[i].fd]._delete(requests[fds[i].fd]);
                    }
                    else
                    {
                        std::cout << "********* ERROR ********" << std::endl;
                        responses[fds[i].fd].error(requests[fds[i].fd]);
                    }
                    responses[fds[i].fd].setFirstTime(false);
                    // responseMap[fds[i].fd] = responses[fds[i].fd].getContent();
                }

                /* send response to client */
                std::cout << "********* SEND RESPONSE ********" << std::endl;
                sendResponse(responses[fds[i].fd].getStatusCode(), fds[i].fd, responses[fds[i].fd]);

                /* check if response is finished sending to close the connection */
                if (responses[fds[i].fd].getDone() == true)
                {
                    std::cout << "********* DONE ********" << std::endl;
                    responses.erase(fds[i].fd);
                    requestMap.erase(fds[i].fd);
                    requests.erase(fds[i].fd);
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);



                    // if (requests[fds[i].fd].get_header_map()["\rConnection"] != "keep-alive\r")
                    // {
                    //     requests.erase(fds[i].fd);

                    //     return (false);
                    // }
                    // requests.erase(fds[i].fd);
                    // fds[i].events = POLLIN;
                    return (false);
                }
                return (true);
            }

            void startServer(std::vector<ws::ServerData> &servers)
            {
                this->servers = servers;
                int p = 1;

                std::cout << " Servers are starting..." << std::endl;
                while (true)
                {
                    p = poll(&fds.front(), fds.size(), -1);
                    if (p < 0)
                    {
                        std::cerr << "Error, Poll failed." << std::endl;
                        break;
                    }
                    size_t size = fds.size();
                    for (size_t i = 0; i < size; i++)
                    {
                        if (fds[i].revents == 0)
                            continue;
                        if (isServer(fds[i].fd))
                            acceptClient(i);
                        else if (fds[i].revents == POLLERR || fds[i].revents == POLLNVAL || fds[i].revents == POLLHUP)
                        {
                            ws::Socket *socket = findClient(fds[i].fd);
                            if (socket == nullptr)
                                continue;
                            socket->removeClient(fds[i].fd);
                            close(fds[i].fd);
                            fds[i].fd = -1;
                        }
                        else  if (fds[i].revents == POLLIN)
                        {
                            if (!recvData(i))
                                fds[i].events = POLLHUP;
                        }
                        else if (fds[i].revents == POLLOUT)
                        {
                            if (!sendData(i))
                                fds[i].events = POLLHUP;
                        }
                    }
                    /* Remove closed sockets from the fds vector */
                    for (size_t i = 0; i < size; i++) 
                    {
                        if (fds[i].fd == -1)
                            fds.erase(fds.begin() + i);
                        else
                            ++i;
                    }
                }
                /* Close all the sockets that are open */
                for (size_t i = 0; i < fds.size(); i++)
                {
                    if (fds[i].fd >= 0)
                        close(fds[i].fd);
                }
		    }
    };
}

#endif