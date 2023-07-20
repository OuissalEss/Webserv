#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Request.hpp"
# include "LocationData.hpp"
# include "ServerData.hpp"
# include <ctime>

namespace ws
{
    class Response
    {
            /* Attributes */
        private:
            int     _statusCode;

        public:
                /* Setters */
            void    setStatusCode(int statusCode) { _statusCode = statusCode; }

                /* Getters */
            int     getStatusCode() { return _statusCode; }

                /* Methods */
            void    get(int socket, Request& _request);
            void    _delete(int socket, Request& _request);
            
            bool isDirectoryMine(std::string path)
            {
                struct stat st;
                if (stat(path.c_str(), &st) == 0) {
                    return S_ISDIR(st.st_mode);
                }
                return false;
            }

    };

    void sendResponse(int socket, int statusCode, const std::string& content);

}


#endif

