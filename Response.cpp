/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slaajour <slaajour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 07:43:09 by slaajour          #+#    #+#             */
/*   Updated: 2023/07/20 08:27:50 by slaajour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Response.hpp"

std::string getHttpDate()
{
    std::time_t now = std::time(nullptr);
    const std::size_t bufferSize = 128;
    char buffer[bufferSize];

    // Format the date in the required HTTP format
    std::strftime(buffer, bufferSize, "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&now));

    return std::string(buffer);
}
void    ws::sendResponse(int socket, int statusCode, const std::string& body)
{
	
    std::string statusLine;
    if (statusCode == 200)
        statusLine = "HTTP/1.1 200 OK\r\n";
    else if (statusCode == 403)
        statusLine = "HTTP/1.1 403 Forbidden\r\n";
    else if (statusCode == 404)
        statusLine = "HTTP/1.1 404 Not Found\r\n";
    else if (statusCode == 405)
        statusLine = "HTTP/1.1 405 Method Not Allowed\r\n";
    else if (statusCode == 301)
        statusLine = "HTTP/1.1 301 Moved Permanently\r\n";
    else if (statusCode == 500)
        statusLine = "HTTP/1.1 500 Internal Server Error\r\n";
    else if (statusCode == 501)
        statusLine = "HTTP/1.1 501 Not Implemented\r\n";
    else if (statusCode == 505)
        statusLine = "HTTP/1.1 505 HTTP Version Not Supported\r\n";
    else if (statusCode == 400)
        statusLine = "HTTP/1.1 400 Bad Request\r\n";

    std::string response = statusLine;
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
	response += "Date: " + getHttpDate() + "\r\n";
    response += "\r\n";
    response += body;

    send(socket, response.c_str(), response.size(), 0);
}