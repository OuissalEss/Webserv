/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slaajour <slaajour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 07:19:30 by slaajour          #+#    #+#             */
/*   Updated: 2023/07/20 08:49:13 by slaajour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Request.hpp"
#include "../LocationData.hpp"
#include "../ServerData.hpp"
#include "../Response.hpp"

// Helper function to delete a file or an empty directory
int removeFileOrDir(const char* path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return -1;

    if (S_ISDIR(statbuf.st_mode))
        return rmdir(path);
    else
        return std::remove(path);
}

// Function to delete all contents of a directory
int removeDirectory(const char* path)
{
    DIR* dir = opendir(path);
    if (dir == NULL)
        return -1;

    int result = 0;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        // Skip "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Build the full path of the entry
        char full_path[PATH_MAX];
        snprintf(full_path, PATH_MAX, "%s/%s", path, entry->d_name);

        // Delete the entry (file or directory)
        int res = removeFileOrDir(full_path);
        if (res != 0)
            result = res;
    }

    closedir(dir);

    // After deleting the contents, remove the empty directory
    int removeResult = rmdir(path);
    if (removeResult != 0)
        result = removeResult;

    return result;
}

bool hasWriteAccess(const char* path)
{
    // Check if the directory exists
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return false;

    // Check write access with access function
    return access(path, W_OK) == 0;
}

void ws::Response::_delete(int socket, Request& _request)
{
	LocationData loc;
	loc = _request.getMyLocation();
	std::string path = _request.getFinalPath();
	if (isDirectoryMine(path))
	{	
		if (path[path.size() - 1] != '/')
		{
			setStatusCode(409);
			ws::sendResponse(socket, getStatusCode(), "hello");
			return ;
		}
		else
		{
			if (loc.getCgi() == true)
			{
				if (loc.getDefaultPage().empty())
				{
					setStatusCode(403);
					ws::sendResponse(socket, getStatusCode(), "hello");
					return ;
				}
				else
				{
					//Run Cgi
				}
			}
			else
			{
				int deleteResult = removeDirectory(path.c_str());
                if (deleteResult == 0)
				{
					setStatusCode(204);
					ws::sendResponse(socket, getStatusCode(), "hello");
                    return ;
				}
                else
                {
                    // Check if it has write access on the folder
					if (hasWriteAccess(path.c_str()))
					{
						setStatusCode(500);
						ws::sendResponse(socket, getStatusCode(), "hello");
                        return ;
					}
                    else
					{
						setStatusCode(403);
						ws::sendResponse(socket, getStatusCode(), "hello");
                        return ;
					}
                }
			}
		}
	}
	else
	{
		if (loc.getCgi() == true)
		{
			//Return code depend on cgi
		}
		else
		{
			if (hasWriteAccess(path.c_str()))
            {
                int deleteResult = std::remove(path.c_str());
                if (deleteResult == 0)
                {
					setStatusCode(204);
					ws::sendResponse(socket, getStatusCode(), "hello");
                    return ;
				}
                else
                {
					setStatusCode(500);
					ws::sendResponse(socket, getStatusCode(), "hello");
                    return ;
				}
            }
            else
            {
				setStatusCode(403);
				ws::sendResponse(socket, getStatusCode(), "hello");
                return ;
			}
		}
	}
}
