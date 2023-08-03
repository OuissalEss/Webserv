#include "../Request.hpp"
#include "../Response.hpp"
#include "../LocationData.hpp"
#include "../ServerData.hpp"
#include <fcntl.h>

namespace ws
{
	bool fileExists1(const std::string& filename)
	{
		FILE* file = std::fopen(filename.c_str(), "r");
		if (file)
		{
			std::fclose(file);
			return true;
		}
		return false;
	}
	bool isDirectory1(std::string path)
	{
		struct stat st;
		if (stat(path.c_str(), &st) == 0) {
			return S_ISDIR(st.st_mode);
		}
		return false;
	}
	bool isDirectoryEmpty(const std::string& directoryPath) {
	    DIR* dir = opendir(directoryPath.c_str());
	    if (dir == nullptr) {
	        std::cerr << "Error opening directory1: " << directoryPath << std::endl;
	        return false;
	    }

	    struct dirent* entry;
	    int count = 0;
	    while ((entry = readdir(dir)) != nullptr) {
	        if (std::string(entry->d_name) != "." && std::string(entry->d_name) != "..") {
	            count++;
	        }
	    }
	    closedir(dir);

	    return count == 0;
	}
	bool isFile(const char* path)
	{
    	std::ifstream file(path);
    	return file.good();
	}
	bool isIndexFile(const std::string& filename) {
    std::string lowercaseFilename = filename;
    std::transform(lowercaseFilename.begin(), lowercaseFilename.end(), lowercaseFilename.begin(), ::tolower);
    if (lowercaseFilename.find(".php") != std::string::npos || lowercaseFilename.find(".html") != std::string::npos) {
        return true;
    } else {
        return false;
    }
}

	int hasIndexFiles(const std::string& directoryPath) {
    	DIR* dir = opendir(directoryPath.c_str());
    if (dir == nullptr) {
        std::cerr << "Error opening director2: " << directoryPath << std::endl;
        return 0;
    }

    std::vector<std::string> files;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) 
	{
        if (entry->d_type == DT_REG) 
		{
            std::string filename = entry->d_name;
			if (filename.find("php") != std::string::npos)
				return 1;
			if (filename.find("html") != std::string::npos)
				return 1;
        }
    }
    closedir(dir);
	return 0;
}

	void Response::post(Request& _request)
	{
		ServerData server = _request.getServer();
		LocationData loc;
		loc = _request.getMyLocation();
		std::string path = _request.getFinalPath();
		if (isDirectory1(path))
		{
			std::string defaultFilePath = path + loc.getDefaultPage();
			if (path[path.size() - 1] != '/')
			{
				path += '/';
				this->setStatusCode(301);
				fileName = path;
				return ;
			}
			else
			{
				if (isDirectoryEmpty(path))
				{
					printf("dir is empty\n");
					setStatusCode(403);
					fileName = server.getDefaultErrorPages() + std::to_string(getStatusCode()) + ".html";
					return ;
				}
				else
				{
					if (hasIndexFiles(path))
           			{
           			    if (loc.getCgi() == true)
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
							printf("has cgi off\n");
           			        setStatusCode(403);
           			        fileName = server.getDefaultErrorPages() + std::to_string(getStatusCode()) + ".html";
           			        return;
           			    }
					}
					else
					{
						printf("has no index files\n");
						setStatusCode(403);
						fileName = server.getDefaultErrorPages() + std::to_string(getStatusCode()) + ".html";
						return ;
					}
				}
			}
		}
		else if (isFile(path.c_str()))
		{
			if (loc.getCgi() == true && isIndexFile(path.c_str()))
			{
				setStatusCode(200);
				std::string cgiPath = path;
				// std::cout << "cgiPath: " << cgiPath << std::endl;
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
				printf("file is not index file\n");
				setStatusCode(404);
				fileName = server.getDefaultErrorPages() + std::to_string(getStatusCode()) + ".html";
				return ;
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