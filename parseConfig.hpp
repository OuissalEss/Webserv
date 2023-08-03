#ifndef PARSECONFIG_HPP
# define PARSECONFIG_HPP
# include "ServerData.hpp"
# include "LocationData.hpp"
# include <iostream>
# include <vector>

namespace ws
{
    LocationData	getLocationData(std::ifstream &infile, std::string &line, int &lineNumber)
    {
        LocationData location;
        getline(infile, line);
        lineNumber++;
        if (line.find("method : ") != std::string::npos)
        {
            std::string methods = line.substr(line.find("method : ") + 9);
            if (methods.find("GET") != std::string::npos)
                location.setCgiGet(true);
            if (methods.find("POST") != std::string::npos)
                location.setCgiPost(true);
            if (methods.find("DELETE") != std::string::npos)
                location.setCgiDelete(true);
        }else
            throw ServerData::Error("Error: Config file is not well formated | line " + std::to_string(lineNumber));

        getline(infile, line);
        lineNumber++;
        if (line.find("root : ") != std::string::npos)
            location.setRoot(line.substr(line.find("root : ") + 7));
        else
            throw ServerData::Error("Error: Config file is not well formated | line " + std::to_string(lineNumber));
        if (line[line.size() - 1] != '/')
            location.setRoot(location.getRoot() + "/");

        getline(infile, line);
        lineNumber++;
        if (line.find("autoindex : ") != std::string::npos)
        {
            std::string booln = line.substr(line.find("autoindex : ") + 12);
            if (booln.find("on") != std::string::npos)
                location.setAutoindex(true);
            else if (booln.find("off") != std::string::npos)
                location.setAutoindex(false);
            else
                throw ServerData::Error("Error: Config file is not well formated | line " + std::to_string(lineNumber));
        }else
            throw ServerData::Error("Error: Config file is not well formated | line " + std::to_string(lineNumber));

        getline(infile, line);
        lineNumber++;
        if (line.find("cgi : ") != std::string::npos)
        {
            std::string booln = line.substr(line.find("cgi : ") + 6);
            if (booln.find("on") != std::string::npos)
                location.setCgi(true);
            else if (booln.find("off") != std::string::npos)
                location.setCgi(false);
            else
                throw ServerData::Error("Error: Config file is not well formated | line " + std::to_string(lineNumber));
        }else
            throw ServerData::Error("Error: Config file is not well formated | line " + std::to_string(lineNumber));

        getline(infile, line);
        lineNumber++;
        if (line.find("upload : ") != std::string::npos)
        {
            std::string booln = line.substr(line.find("upload : ") + 6);
            if (booln.find("on") != std::string::npos)
                location.setUpload(true);
            else if (booln.find("off") != std::string::npos)
                location.setUpload(false);
            else
                throw ServerData::Error("Error: Config file is not well formated | line " + std::to_string(lineNumber));
        }else
            throw ServerData::Error("Error: Config file is not well formated | line " + std::to_string(lineNumber));
        
        getline(infile, line);
        lineNumber++;
        if (line.find("default : ") != std::string::npos)
            location.setDefaultPage(line.substr(line.find("default : ") + 10));
        else
            throw ServerData::Error("Error: Config file is not well formated | line " + std::to_string(lineNumber));
        getline(infile, line);
        if (line.find("redirect : ") != std::string::npos)
        {
            lineNumber++;
            location.setRedirect(line.substr(line.find("redirect : ") + 11));
            getline(infile, line);
        }
        lineNumber++;
        if (line.find(")") == std::string::npos)
            throw ServerData::Error("Error: Config file is not well formated | line " + std::to_string(lineNumber));
        return (location);
    }

    ServerData	getServerData(std::ifstream &infile, std::string &line, int &lineNumber)
    {
        ServerData	server;
        while (getline(infile, line) && line.find("}") == std::string::npos)
        {
            lineNumber++;
            // std::cout << "----> |" << line << "" << std::endl;
            if (line.find("host : ") != std::string::npos)
                server.setHost(line.substr(line.find("host : ") + 7));
            else if (line.find("port : ") != std::string::npos)
                server.setPort(atoi(line.substr(line.find("port : ") + 7).c_str()));
            else if (line.find("server_name : ") != std::string::npos)
            {
                server.setServerName(line.substr(line.find("server_name : ") + 14));
            }
            else if (line.find("bodysize_limit : ") != std::string::npos)
                server.setBodySizeLimit(atoi(line.substr(line.find("bodysize_limit : ") + 17).c_str()));
            else if (line.find("default_error_pages : ") != std::string::npos)
                server.setDefaultErrorPages(line.substr(line.find("default_error_pages : ") + 22));
            else if (line.find("location") != std::string::npos && line.find("/") != std::string::npos && line.find("(") != std::string::npos)
            {
                std::string			locationName = line.substr(line.find("/"), (line.find("(") - line.find("/") - 1));
                LocationData	location = getLocationData(infile, line, lineNumber);

                std::map<std::string, LocationData>	locations = server.getLocations();
                locations.insert(std::pair<std::string, LocationData>(locationName, location));
                server.setLocations(locations);
            }
            else if (!line.empty())
                throw ServerData::Error("Error: Config file is not well formated | line " + std::to_string(lineNumber));
        }
        lineNumber++;
        return (server);
    }

    std::vector<ServerData>	readConfig(std::string fileName)
    {
        int 					lineNumber = 0;
        std::string				line;
        std::ifstream			infile(fileName.c_str());
        std::vector<ServerData>	servers;

        // Read Config file
        if (!infile.is_open())
            throw ServerData::Error("Error: Config file not found");
        while (getline(infile, line))
        {
            lineNumber++;
            if (line.find("server") != std::string::npos)
            {
                ServerData	server = getServerData(infile, line, lineNumber);
                servers.push_back(server);
            }
        }
        infile.close();
        return (servers);
    }

    std::vector<ServerData>	parseConfigFile(std::string fileName)
    {
        std::vector<ServerData> servers = readConfig(fileName);
        return (servers);
    }

    void    checkConfig(std::vector<ServerData> &servers)
    {
        for (size_t i = 0; i < servers.size(); i++)
        {
            if (servers[i].getHost() != "127.0.0.1" && servers[i].getHost() != "0.0.0.0")
                throw ServerData::Error("Error: Wrong host");
            if (servers[i].getPort() < 1024 || servers[i].getPort() > 49151)
                throw ServerData::Error("Error: Wrong port");
            if (servers[i].getServerName().empty())
                throw ServerData::Error("Error: Wrong server_name");
            if (servers[i].getBodySizeLimit() < 0)
                throw ServerData::Error("Error: Wrong bodysize_limit");
            if (servers[i].getDefaultErrorPages().empty())
                throw ServerData::Error("Error: Wrong default_error_pages");
            std::map<std::string, LocationData>	locations = servers[i].getLocations();
            for (std::map<std::string, LocationData>::iterator it = locations.begin(); it != locations.end(); it++)
            {
                if (it->second.getRoot().empty())
                    throw ServerData::Error("Error: Wrong root");
            }
        }
        /* CHECK IF SERVERS ARE DIFFERENT */
        /* mazal dak l3jeb dyal la page de correction 🦦*/
        for (size_t i = 0; i < servers.size(); i++)
        {
            for (size_t j = i + 1; j < servers.size(); j++)
            {
                if (servers[i].getHost() == servers[j].getHost() && servers[i].getPort() == servers[j].getPort() && servers[i].getServerName() == servers[j].getServerName())
                    throw ServerData::Error("Error: Servers are the same");
            }
        }
    }
}

#endif