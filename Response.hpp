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
            std::string     fileName;
            bool            firstTime;
            bool            done;
            bool            isAutoIndex;
            bool            isCgi;
            int             _statusCode;
            int             _fd;
            size_t          currentPosition;

        public:
                /* Constructor */
            Response() : firstTime(true), done(false), isAutoIndex(false), isCgi(false), _fd(-2), currentPosition(0) {}
                /* Destructor */   
            ~Response() {}

                /* Setters */
            void    setStatusCode(int statusCode) { _statusCode = statusCode; }
            void    setFd(int fd) { _fd = fd; }
            void    setDone(bool done) { this->done = done; }
            void    setFirstTime(bool firstTime) { this->firstTime = firstTime; }
            void    setIsCgi(bool isCgi) { this->isCgi = isCgi; }
            void    setIsAutoIndex(bool isAutoIndex) { this->isAutoIndex = isAutoIndex; }
            void    setCurrentPosition(size_t currentPosition) { this->currentPosition = currentPosition; }

                /* Getters */
            bool        getFirstTime() { return firstTime; }
            bool        getDone() { return done; }
            int         getStatusCode() { return _statusCode; }
            int         getFd() { return _fd; }
            std::string getFileName() { return fileName; }
            bool        getIsCgi() { return isCgi; }
            bool        getIsAutoIndex() { return isAutoIndex; }
            size_t      getCurrentPosition() { return currentPosition; }

                /* Methods */
            void    get(Request& _request);
            void    post(Request& _request);
            void    _delete(Request& _request);
            void    error(Request& _request);
            
            bool isDirectoryMine(std::string path)
            {
                struct stat st;
                if (stat(path.c_str(), &st) == 0) {
                    return S_ISDIR(st.st_mode);
                }
                return false;
            }

            bool fileExists(const std::string& filename)
	        {
	        	FILE* file = std::fopen(filename.c_str(), "r");
	        	if (file)
	        	{
	        		std::fclose(file);
	        		return true;
	        	}
	        	return false;
	        }
    };
    void    sendResponse(int statusCode, int fdSocket, Response& _response);
    std::string executeCGI(const std::string& cgiFilePath, Request& _request);
    std::string getMimeType(std::string path);
}

#endif

