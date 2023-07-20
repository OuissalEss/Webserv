#include "Request.hpp"

void ws::Request::post()
{
	try
	{
		if (isDirectory(final_path.c_str()))
		{
			std::cout << final_path[this->final_path.size()] << std::endl;
			if (this->final_path[this->final_path.size() - 1] != '/')
			{
				this->set_status(301);
				return ;
			}
			else if (hasIndexFile(this->final_path.c_str()))
			{
				//send_to_cgi();
				return ;
			}
			else
			{
				this->set_status(403);
				return;
			}
		}
		else 
		{
			std::string file;
			std::string ext;
			int i = final_path.size() - 1;
			while (i > 0)
			{
				if (final_path[i] == '/')
					break;
				i--;
			}
			file = final_path.substr(i);
			i = 0;
			while (file[i])
			{
				if (file[i] == '.')
					break;
				i++;
			}
			ext = file.substr(i + 1);
			if (strcmp(ext.c_str(),"py") == 0 || strcmp(ext.c_str(),"php") == 0)
			{
				printf("ready to send to cgi\n");
				//send_to_cgi
			}
			else
			{
				status = 403;
				printf("error 403\n");
			}
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << "" << '\n';
	}
};
