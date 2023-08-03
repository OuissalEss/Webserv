/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slaajour <slaajour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 07:43:09 by slaajour          #+#    #+#             */
/*   Updated: 2023/08/01 10:24:21 by slaajour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Response.hpp"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

namespace ws
{
    std::string getHttpDate()
    {
        std::time_t now = std::time(nullptr);
        const std::size_t bufferSize = 128;
        char buffer[bufferSize];
        // Format the date in the required HTTP format
        std::strftime(buffer, bufferSize, "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&now));
        return std::string(buffer);
    }

    std::string getStatusLine(int statusCode)
    {
        std::string statusLine;
        if (statusCode == 200)
            statusLine = "HTTP/1.1 200 OK\r\n";
        else if (statusCode == 204)
            statusLine = "HTTP/1.1 204 No Content\r\n";
        else if (statusCode == 301)
            statusLine = "HTTP/1.1 301 Moved Permanently\r\n";
        else if (statusCode == 400)
            statusLine = "HTTP/1.1 400 Bad Request\r\n";
        else if (statusCode == 403)
            statusLine = "HTTP/1.1 403 Forbidden\r\n";
        else if (statusCode == 404)
            statusLine = "HTTP/1.1 404 Not Found\r\n";
        else if (statusCode == 405)
            statusLine = "HTTP/1.1 405 Method Not Allowed\r\n";
        else if (statusCode == 409)
            statusLine = "HTTP/1.1 409 Conflict\r\n";
        else if (statusCode == 413)
            statusLine = "HTTP/1.1 413 Payload Too Large\r\n";
        else if (statusCode == 414)
            statusLine = "HTTP/1.1 414 URI Too Long\r\n";
        else if (statusCode == 500)
            statusLine = "HTTP/1.1 500 Internal Server Error\r\n";
        else if (statusCode == 501)
            statusLine = "HTTP/1.1 501 Not Implemented\r\n";
        return statusLine;
    }

    std::string getResponseHeader(int statusCode, Response& _response)
    {
        (void)statusCode;
        std::string response;
        if (statusCode == 301)
        {
            response += "Location: " + _response.getFileName() + "\r\n";
            response += "Content-Length: 0\r\n";
            _response.setDone(true);
            // std::cout << "response = " << response << std::endl;
            return response;
        }
        std::cout << "file name = " << _response.getFileName() << std::endl;
        std::string mimeType = getMimeType(_response.getFileName());
        response += mimeType;

        std::ifstream file(_response.getFileName(), std::ios::binary);
        file.seekg(0, std::ios::end);
        int file_size = file.tellg();
        file.close();
        response += "Content-Length: " + std::to_string(file_size) + "\r\n";
        // response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
        response += "Date: " + getHttpDate() + "\r\n";
        response += "\r\n";
        return response;
    }

    void    sendResponse(int statusCode, int fdSocket, Response& _response)
    {
        std::string response = "";
        int headerSize = 0;
        if (_response.getFd() == -2 && _response.getIsCgi() == false)
        {
            std::string statusLine = getStatusLine(statusCode);
            response = statusLine + getResponseHeader(statusCode, _response);
            if (statusCode != 301)
                _response.setFd(open(_response.getFileName().c_str(), O_RDONLY));
            if (_response.getFd() == -1)
                throw std::runtime_error("Error opening file");
            headerSize = response.size();
        }
        else if (_response.getFd() == -2 && _response.getIsCgi() == true)
        {
            _response.setFd(open(_response.getFileName().c_str(), O_RDONLY));
            if (_response.getFd() == -1)
                throw std::runtime_error("Error opening file");
            headerSize = response.size();
        }
        if (statusCode != 301)
        {
            int chunkSize = 2048*100;
            char chunk[chunkSize];
            lseek(_response.getFd(), _response.getCurrentPosition(), SEEK_SET);
            ssize_t bytes_read = read(_response.getFd(), chunk, chunkSize);
            response += std::string(chunk, bytes_read);
        }

        if (response.size() > 0)
        {
            int s = send(fdSocket, response.c_str(), response.size(), 0);
            _response.setCurrentPosition(_response.getCurrentPosition() + s - headerSize);
        }
        else
        {
            if (_response.getIsAutoIndex() == true)
                std::remove(_response.getFileName().c_str());
            _response.setDone(true);
            close(_response.getFd());
        }
    }
    
    void Response::error(Request& _request)
    {
        ServerData server = _request.getServer();
        setStatusCode(_request.get_status());
		fileName = server.getDefaultErrorPages() + std::to_string(getStatusCode()) + ".html";
    }

    std::string getMimeType(std::string path)
    {
        std::string extension = path.substr(path.find_last_of(".") + 1);
        std::string response;
    
        // std::cout << "extension =====> " << extension << std::endl;
        if (extension == "html" || extension == "htm" || extension == "shtml")
            response += "Content-Type: text/html\r\n";
        else if (extension == "css")
            response += "Content-Type: text/css\r\n";
        else if (extension == "xml")
            response += "Content-Type: text/xml\r\n";
        else if (extension == "gif")
            response += "Content-Type: image/gif\r\n";
        else if (extension == "jpeg" || extension == "jpg")
            response += "Content-Type: image/jpeg\r\n";
        else if (extension == "js")
            response += "Content-Type: application/x-javascript\r\n";
        else if (extension == "atom")
            response += "Content-Type: application/atom+xml\r\n";
        else if (extension == "rss")
            response += "Content-Type: application/rss+xml\r\n";
        else if (extension == "pdf")
            response += "Content-Type: application/pdf\r\n";
        else if (extension == "zip")
            response += "Content-Type: application/zip\r\n";
        else if (extension == "php")
            response += "Content-Type: application/x-httpd-php\r\n";
        else if (extension == "mml")
            response += "Content-Type: text/mathml\r\n";
        else if (extension == "txt")
            response += "Content-Type: text/plain\r\n";
        else if (extension == "jad")
            response += "Content-Type: text/vnd.sun.j2me.app-descriptor\r\n";
        else if (extension == "wml")
            response += "Content-Type: text/vnd.wap.wml\r\n";
        else if (extension == "htc")
            response += "Content-Type: text/x-component\r\n";
        else if (extension == "png")
            response += "Content-Type: image/png\r\n";
        else if (extension == "wbmp")
            response += "Content-Type: image/vnd.wap.wbmp\r\n";
        else if (extension == "ico")
            response += "Content-Type: image/x-icon\r\n";
        else if (extension == "jng")
            response += "Content-Type: image/x-jng\r\n";
        else if (extension == "bmp")
            response += "Content-Type: image/x-ms-bmp\r\n";
        else if (extension == "svg")
            response += "Content-Type: image/svg+xml\r\n";
        else if (extension == "webp")
            response += "Content-Type: image/webp\r\n";
        else if (extension == "mp3")
            response += "Content-Type: audio/mpeg\r\n";
        else if (extension == "mp4")
            response += "Content-Type: video/mp4\r\n";
        else if (extension == "mov")
            response += "Content-Type: video/quicktime\r\n";
        
        return response;
    }
};