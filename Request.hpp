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
# include <sstream>

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
			int 								flag2;
			int 								stopRn;
			int 								file_cr;
			int									doubleKind;
			int									cookieflag;
			int									slash;
			std::string							html;
			ServerData							server;
			std::string 						key;
			std::string 						value;
			std::string							location;
			std::string 						extention;
			std::string 						final_path;
			std::string 						body;
			std::string 						query;
			std::string 						cgiFile;
			std::fstream 						filee;
			LocationData						myLocation;
			LocationData						uploadLocation;
			std::string 						last_body;
			std::map<std::string, std::string>	body_map;
			std::map<std::string, std::string>	header_map;

		public:
			Request() : init(0), method(2), status(200), body_kind(1) ,flag(0),delim(0),compareFlag(0),flag2(0), stopRn(0), file_cr(0), doubleKind(0),cookieflag(0),slash(0){};
			Request(ServerData &s) : init(0), method(2), status(200), body_kind(1), server(s) {};
			~Request() {};

			//setters
			void setHtml(std::string html){this->html = html;};
			void setslash(int s) {this->slash = s;};
			void setServer(ServerData s){this->server = s;};
			void setBody(std::string body){this->body = body;};
			void set_method(int method){this->method = method;};
			void set_status(int status){this->status = status;};
			void set_body_kind(int body_kind){this->body_kind = body_kind;};
			void set_init(int init){this->init = init;};
			void set_flag(int flag) {this->flag = flag;}
			void set_delim(int delim) {this->delim = delim;};
			void set_compareFlag(int compareFlag) {this->compareFlag = compareFlag;};
			void set_flag2(int flag2) {this->flag2 = flag2;};
			void set_double_kind(int d) {this->doubleKind = d;};
			void setStoprn(int k) {this->stopRn = k;};
			void set_file_cr(int file_cr) {this->file_cr = file_cr;};
			void setFinalPath(std::string k){this->final_path = k;};
			void setlocation(std::string s){this->location = s;};
			void setLastBody(std::string s){this->last_body = s;};

			//getters
			LocationData getMyLocation(){return this->myLocation;};
			int get_status(){return this->status;};
			int get_init(){return this->init;};
			int get_method(){return this->method;};
			int get_body_kind(){return this->body_kind;};
			int get_flag() {return this->flag;};
			int get_delim() {return this->delim;};
			int get_compareFlag() {return this->compareFlag;};
			int get_flag2() {return this->flag2;};
			int get_stopRn() {return this->stopRn;};
			int get_file_cr() {return this->file_cr;};
			int get_double_kind() {return this->doubleKind;};
			std::map<std::string, std::string> get_header_map() {return header_map;}
			int getslash(){return this->slash;};
			std::string getHtml(){return this->html;};
			std::string getFinalPath(){return this->final_path;};
			std::string getLocation(){return this->location;};
			std::string getBody(){return this->body;};
			LocationData getMyLocation() const {return this->myLocation;};
			ServerData getServer() const {return this->server;};
			std::string getQuery() {return this->query;};
			std::string getCgiFile() {return this->cgiFile;};
			std::string getExtention() {return this->extention;};

			//#################################
			//void post();
			void check_errors();
			void slash_n(std::string s);
			void parse_header(std::string body);
			void set_inittt();
			std::string get_matched(std::string s);
			std::string remove_slash(std::string s);
			void error();
			void upload1(std::string body);
			void upload2(std::string body);
			std::string to_rn(std::string body);
			std::string compare(std::string s);
			int not_allowed_char(std::string uri);
			int check_rn(std::string body);
			int check_rn2(std::string body);
			int wait_for_zero(std::string body);
			int wait_for_size(std::string body);
			std::string generateName();
	};
}
#endif