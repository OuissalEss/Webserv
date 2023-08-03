#ifndef SERVERDATA_HPP
# define SERVERDATA_HPP
# include <string>
# include <map>
# include <unistd.h>
# include <fstream>
# include "LocationData.hpp"


namespace ws
{
	class ServerData
	{
		public:
			typedef	std::string							string;
			typedef	std::map<string, LocationData>		myMap;

			ServerData() : host(""), port(0), serverName(""), bodySizeLimit(0), defaultErrorPages(""), locations(), path("") {
				char tmp[1024];

				if (getcwd(tmp, 1024) == NULL) {
					throw ServerData::Error("Error: getcwd failed");
				}
				path = string(tmp);
			};
			~ServerData() {};

			// SETTERS
			void		setHost(string const &host) { this->host = host; }
			void		setPort(size_t const &port) { this->port = port; }
			void		setServerName(string const &serverName) { this->serverName = serverName; }
			void		setBodySizeLimit(size_t const &bodySizeLimit) { this->bodySizeLimit = bodySizeLimit; }
			void		setDefaultErrorPages(string const &defaultErrorPages) { this->defaultErrorPages = defaultErrorPages; }
			void		setLocations(myMap const &locations) { this->locations = locations; }
			void		setPath(string const &path) { this->path = path; }

			// GETTERS
			string		getHost() const { return (this->host); }
			size_t		getPort() const { return (this->port); }
			string		getServerName() const { return (this->serverName); }
			size_t		getBodySizeLimit() const { return (this->bodySizeLimit); }
			string		getDefaultErrorPages() const { return (this->defaultErrorPages); }
			myMap		getLocations() const { return (this->locations); }
			string		getPath() const { return (this->path); }


			// METHODS

			// EXCEPTIONS
			class Error : public std::exception {
				private:
					string		message;
				public:
					Error(string message) throw() { this->message = message; }
					virtual ~Error() throw() {}
					virtual const char * what() const throw() {
						return (message.c_str());
					}
			};

		private :
			string		host;
			size_t		port;
			string		serverName;
			size_t		bodySizeLimit;
			string		defaultErrorPages;
			myMap		locations;
			string		path;
	};
}

#endif