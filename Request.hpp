#ifndef REQUEST_HPP
# define REQUEST_HPP
# include "LocationData.hpp"
# include <iostream>
# include <map>
# include <cstring>
# include <fstream>
# include <string>
# include <locale>
# include <cstdlib>
# include <sys/stat.h>
# include <vector>
# include <stdio.h>
# include <sys/socket.h>
# include <unistd.h>
# include <stdlib.h>
# include <netinet/in.h>
# include <string.h>
# include <iostream>
# include <iostream>
# include <fstream>
# include <sys/socket.h>
# include <unistd.h>
# include <cmath>
# include "ServerData.hpp"
# include <dirent.h>

namespace ws
{
	class Request
	{
		private:
			int 								init;
			int 								method;
			int 								status;
			int 								body_kind;
			int 								flag;
			int 								delim;
			int 								compareFlag;
			std::string 						key;
			std::string 						value;
			std::string							location;
			std::string 						extention;
			std::string 						final_path;
			std::string 						body;
			ServerData							server;
			LocationData						myLocation;
			std::vector<char>					last_body;
			std::map<std::string, std::string>	body_map;
			std::map<std::string, std::string>	header_map;

		public:
			Request() : init(0), method(2), status(200), body_kind(1) {};
			Request(ServerData &s) : init(0), method(2), status(200), body_kind(1), server(s) {};
			~Request() {};

			//setters
			void setServer(ServerData s){this->server = s;};
			void setBody(std::string body){this->body = body;};
			void set_method(int method){this->method = method;};
			void set_status(int status){this->status = status;};
			void set_body_kind(int body_kind){this->body_kind = body_kind;};
			void set_init(int init){this->init = init;};

			//getters
			LocationData getMyLocation(){return this->myLocation;};
			int get_status(){return this->status;};
			int get_init(){return this->init;};
			int get_method(){return this->method;};
			int get_body_kind(){return this->body_kind;};
			std::string getFinalPath(){return this->final_path;};

			//#################################
			void post();
			void check_errors();
			void parse_request_head(std::fstream& file, int *j);
			void parse_header(std::string body);
			void set_inittt();
			void get_matched();
			void error();
			void upload();
			bool file_exists(const std::string& filePath);
			std::string compare(std::string s);
			int not_allowed_char(std::string uri);
			int check_rn(std::string body);
			int wait_for_zero(std::string body);
			int wait_for_size(std::string body);
			std::string generateName();
			bool isDirectory(const char* path) {
				struct stat st;
				if (stat(path, &st) == 0) {
					return S_ISDIR(st.st_mode);
				}
				return false;
			};

			bool hasIndexFile(const char* directoryPath) {
				DIR* directory = opendir(directoryPath);
				if (directory == NULL) {
					std::cerr << "Failed to open directory: " << directoryPath << std::endl;
					return false;
				}

				struct dirent* entry;
				while ((entry = readdir(directory)) != NULL) {
					if (strcmp(entry->d_name, "index.py") == 0 || strcmp(entry->d_name, "index.php"))
						closedir(directory);
					return true;
				}

				closedir(directory);
				return false;
			}
	};
}
#endif