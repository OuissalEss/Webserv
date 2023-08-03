/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slaajour <slaajour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 07:18:12 by slaajour          #+#    #+#             */
/*   Updated: 2023/08/03 11:09:08 by slaajour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Request.hpp"
#include "../LocationData.hpp"
#include "../ServerData.hpp"
#include "../Response.hpp"
#include <fcntl.h>

namespace ws
{
	std::map<std::string, std::string> getFileIcons()
	{
		std::map<std::string, std::string> fileIcons;
		fileIcons[".txt"] = "<span>&#128196;</span>";   // 📄
		fileIcons[".pdf"] = "<span>&#128196;</span>";   // 📄
		fileIcons[".jpg"] = "<span>&#128247;</span>";   // 📷
		fileIcons[".png"] = "<span>&#128444;</span>";   // 🖼️
		fileIcons[".mp3"] = "<span>&#127925;</span>";   // 🎵
		fileIcons[".mp4"] = "<span>&#127916;</span>";   // 🎦
		fileIcons["directory"] = "<span>&#128193;</span>"; // 📁
		// Add more file types and icons as needed
		return fileIcons;
	}

	bool isDirectory(std::string path)
	{
		struct stat st;
		if (stat(path.c_str(), &st) == 0)
			return S_ISDIR(st.st_mode);
		return false;
	}

	std::string generateAutoIndex(const std::string& directoryPath)
	{
		std::string html;
		html += "<!DOCTYPE html>\n";
		html += "<html><head><title>Index of " + directoryPath + "</title></head><body>";
		html += "<div style=\"padding: 20px; font-family: Arial, sans-serif;\">";
		html += "<h1>Index of " + directoryPath + "</h1>";
		html += "<ul style=\"list-style-type: none; padding-left: 0;\">";

		DIR* dir = opendir(directoryPath.c_str());
		if (dir == NULL)
		{
			std::cerr << "Error opening directory: " << directoryPath << std::endl;
			return "";
		}
		std::map<std::string, std::string> fileIcons = getFileIcons();
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL)
		{
			std::string filename(entry->d_name);
			// Skip the current directory
			if (filename == ".")
				continue;
			if (filename == "..")
			{
				std::string icon = "<span>&#128281;</span>"; // 📁
				html += "<li style=\"margin-bottom: 6px;\">";
				html += "<span style=\"font-size: 12px; color: #6C757D;\">" + icon + "&nbsp;&nbsp;&nbsp; </span>";
				html += "<a style=\"text-decoration: none; color: #007BFF;\" href=\"../\"> " + filename + "</a>";
				html += "</li>";
				continue;
			}
			std::string icon = "<span>&#128196;</span>";
			if (isDirectory(directoryPath + "/" + filename))
				icon = fileIcons["directory"];
			else
			{
				size_t dotPos = filename.find_last_of(".");
				if (dotPos != std::string::npos && dotPos < filename.length() - 1)
				{
					std::string extension = filename.substr(dotPos);
					std::map<std::string, std::string>::iterator it = fileIcons.find(extension);
					if (it != fileIcons.end()) {
						icon = it->second;
					}
				}
			}
			html += "<li style=\"margin-bottom: 6px;\">";
			html += "<span style=\"font-size: 12px; color: #6C757D;\">" + icon + "&nbsp;&nbsp;&nbsp; </span>";
			if (isDirectory(directoryPath + "/" + filename))
				html += "<a style=\"text-decoration: none; color: #007BFF;\" href=\"" + filename + "/\"> " + filename + "</a>";
			else
				html += "<a style=\"text-decoration: none; color: #007BFF;\" href=\"" + filename + "\"> " + filename + "</a>";
			html += "</li>";
		}
		closedir(dir);
		html += "</ul>";
		html += "</div>";
		html += "</body></html>";
		return html;
	}

	void Response::get(Request& _request)
	{
		ServerData server = _request.getServer();
		LocationData loc;
		loc = _request.getMyLocation();
		std::string path = _request.getFinalPath();
		std::string location = _request.getLocation();
		if (loc.getRedirect() != "")
		{
			this->setStatusCode(301);
			fileName = "https://" + loc.getRedirect();
			return ;
		}
		if (isDirectoryMine(path))
		{
			if (path[path.size() - 1] != '/')
			{
				location += '/';
				this->setStatusCode(301);
				fileName = location;
				return ;
			}
			else
			{
				if (loc.getDefaultPage().empty())
				{
					if (loc.getAutoindex() == true)
					{
						setStatusCode(200);
						std::string data = generateAutoIndex(path);
						std::string autoIndexFile = path + "autoindex.html";
						int autoIndex = open(autoIndexFile.c_str(), O_RDWR | O_CREAT, 0777);
						write(autoIndex, data.c_str(), strlen(data.c_str()));
						close(autoIndex);
						fileName = autoIndexFile;
						isAutoIndex = true;
						return ;
					}
					else
					{
						setStatusCode(403);
						fileName = server.getDefaultErrorPages() + std::to_string(getStatusCode()) + ".html";
						return ;
					}
				}
				else
				{
					std::string defaultFilePath = path + loc.getDefaultPage();
					if (fileExists(defaultFilePath))
           			{
						std::string extension = defaultFilePath.substr(defaultFilePath.find_last_of(".") + 1);
           				if (loc.getCgi() == true && (extension == "php" || extension == "py"))
           				{
							setStatusCode(200);
							isCgi = true;
							fileName = executeCGI(defaultFilePath, _request);
							if (fileName == "fork failed")
							{
								isCgi = false;
								setStatusCode(500);
								fileName = server.getDefaultErrorPages() + std::to_string(getStatusCode()) + ".html";
								return ;			
							}
           				}
           				else
           				{
           				    setStatusCode(200);
							fileName = defaultFilePath;
           				    return;
           				}
					}
					else
					{
						setStatusCode(404);
						fileName = server.getDefaultErrorPages() + std::to_string(getStatusCode()) + ".html";
						return ;
					}
				}
			}
		}
		else
		{
			if (fileExists(path))
           	{
				std::string extension = path.substr(path.find_last_of(".") + 1);
           	    if (loc.getCgi() == true && (extension == "php" || extension == "py"))
           	    {
					setStatusCode(200);
					isCgi = true;
					fileName = executeCGI(path, _request);
					if (fileName == "fork failed")
					{
						isCgi = false;
						setStatusCode(500);
						fileName = server.getDefaultErrorPages() + std::to_string(getStatusCode()) + ".html";
						return ;			
					}
           	    }
           	    else
           	    {
           	        setStatusCode(200);
					fileName = path;
           	        return;
           	    }
			}
			else
			{
				setStatusCode(404);
				fileName = server.getDefaultErrorPages() + std::to_string(getStatusCode()) + ".html";
				return ;
			}
		}
	}
}
