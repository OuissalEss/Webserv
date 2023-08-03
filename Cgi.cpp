/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slaajour <slaajour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 12:22:39 by slaajour          #+#    #+#             */
/*   Updated: 2023/08/03 12:30:02 by slaajour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "LocationData.hpp"
#include "ServerData.hpp"
#include "Response.hpp"
#include <fcntl.h>
 
namespace ws
{
    std::string generateRandomFileName()
    {
        std::stringstream ss;
        
        std::time_t now = std::time(0);
        std::tm* localTime = std::localtime(&now);
        char timeStr[15];
        std::strftime(timeStr, sizeof(timeStr), "%Y%m%d%H%M%S", localTime);
        
        ss << "www/CGI/";
        ss << timeStr;
        ss << std::rand() % 1000; // Adding a random number to ensure uniqueness
        ss << ".txt"; // You can change the file extension as needed
        
        return ss.str();
    }
    
    char* setEnv(const std::string& str)
    {
        char* envStr = new char[str.size() + 1];
        std::strcpy(envStr, str.c_str());
        return envStr;
    }

    char**        BuildEnv(ws::Request& _request, const std::string& cgiFilePath)
    {
        std::map<std::string, std::string>	header_map = _request.get_header_map();
        std::string extension = cgiFilePath.substr(cgiFilePath.find_last_of(".") + 1);
        char** env = new char*[16];

        if (_request.get_method() == 1)
            env[0] = setEnv("REQUEST_METHOD=GET");
        else if (_request.get_method() == 2)
            env[0] = setEnv("REQUEST_METHOD=POST");
        env[1] = setEnv("SERVER_PORT=" + std::to_string(_request.getServer().getPort()));
        env[2] = setEnv("SCRIPT_NAME=" + cgiFilePath);
        env[3] = setEnv("SCRIPT_FILENAME=" + cgiFilePath);
        env[4] = setEnv("PATH_INFO=" + cgiFilePath);
        env[5] = setEnv("QUERY_STRING=" + _request.getQuery());
        env[6] = setEnv("REDIRECT_STATUS=200");
        env[7] = setEnv("SERVER_HOST=" + _request.getServer().getHost());
        env[8] = setEnv("HTTP_HOST=" + _request.getServer().getHost());
        env[9] = setEnv("HTTP_PORT=" + std::to_string(_request.getServer().getPort()));
        std::string ua = header_map["\rUser-Agent"];
        env[10] = setEnv("HTTP_USER_AGENT=" + ua);
        std::string c = header_map["\rCookie"];
        env[11] = setEnv("HTTP_COOKIE=" + c);
        env[12] = setEnv("REMOTE_ADDR=" + _request.getServer().getHost());
        if (_request.get_method() == 2)
        {
            // env[13] = setEnv("CONTENT_LENGTH=" + header_map["\rContent-Length"]);
            std::string cl = header_map["\rContent-Length"];
            env[13] = setEnv("CONTENT_LENGTH=" + cl);
            std::string ct = header_map["\rContent-Type"].erase(header_map["\rContent-Type"].size() - 1);
            env[14] = setEnv("CONTENT_TYPE=" + ct);
            env[15] = NULL;
        }
        else
            env[13]= NULL;
        int i = 0;
        while (env[i])
        {
            std::cout << "env[" << i << "] = " << env[i] << std::endl;
            i++;
        }
        std::cout << "------" << std::endl;
        return env;
    }

    std::string executeCGI(const std::string& cgiFilePath, ws::Request& _request) 
    {
        std::cout << "cgiFilePath: " << cgiFilePath << std::endl;
        std::string extension = cgiFilePath.substr(cgiFilePath.find_last_of(".") + 1);
        _request.setCgiName(generateRandomFileName());
        std::cout << "cgi name: " << _request.getCgiName() << std::endl;
        

        char *argv[4];
        int infile = 0;
        if (extension == "py")
            argv[0] = strdup("/Users/slaajour/desktop/ouissal/www/CGI/python-cgi");
        else if (extension == "php")
            argv[0] = strdup("/Users/slaajour/desktop/ouissal/www/CGI/php-cgi");

        std::cout << "CGI FILE PATH " << cgiFilePath.c_str() << std::endl;
        argv[1] = strdup(cgiFilePath.c_str());
        argv[2] = NULL;

        char **envp = BuildEnv(_request, cgiFilePath);
        int outfile = open(_request.getCgiName().c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
        // int outfile = open("www/CGI/output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
        std::cout << std::endl;
        std::cout << "cgi file: *" << _request.getCgiFile() << "*" << std::endl;
        std::cout << std::endl;
        if (_request.get_method() == 2)
            infile = open(_request.getCgiFile().c_str(), O_RDONLY);
        if (infile == -1)
        {
            std::cout << "error opening file" << std::endl;
            exit(1);
        }
        pid_t pid = fork();
        if (pid == -1) {
            // Forking failed
            std::perror("fork");
            return ("fork failed");
        }
        if (pid == 0) {
            dup2(outfile, 1);
            dup2(infile, 0);
            close(outfile);
            if (infile != 0)
                close(infile);
            if (execve(argv[0], argv, envp) == -1) {
                std::perror("execve");
                std::exit(EXIT_FAILURE);
            }
        }
            close(outfile);
            if (infile != 0)
                close(infile);

            int status = 0;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status))
            {
                std::string str = "HTTP/1.1 200 OK\r\n";

                std::ifstream inputFile;
                inputFile.open(_request.getCgiName().c_str());
                std::string line;
                if (extension == "py") {
                    str += "Content-Type: text/html\r\n";
                    str += "\r\n";
                }
                while (std::getline(inputFile, line)) {
                    str += line;
                    str += "\n";
                }
                inputFile.close();

                std::ofstream outputFile;
                outputFile.open(_request.getCgiName().c_str());
                outputFile << str;
                outputFile.close();
            }
        // std::cout << str << std::endl;
        return "/Users/slaajour/desktop/ouissal/" + _request.getCgiName();
        // else
        // {
            // Parent process
            // Wait for the child process to complete

        //     // Handle the result if needed
        //     if (WIFEXITED(status))
        //     {
        //         int exitStatus = WEXITSTATUS(status);
        //         std::cout << "exit status: " << exitStatus << std::endl;
        //         // Do something with the exit status of the child process
        //         // (the CGI script's exit status)
        //     }
        // }
        
        // Clean up the environment array
        // for (size_t i = 0; envp[i] != NULL; ++i)
        // {
        //     delete[] envp[i];
        // }
        // delete[] envp;
    }
}
