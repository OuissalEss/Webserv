#ifndef LOCATIONDATA_HPP
# define LOCATIONDATA_HPP
# include <string>

namespace ws
{
	class LocationData {
		public :
			typedef	std::string			string;

			LocationData() : root(""), defaultPage(""), redirect(""), autoindex(false), cgi(false), cgiGet(false), cgiPost(false), cgiDelete(false) {};
			~LocationData() {};

			// SETTERS
			void	setRoot(string root) { this->root = root; }
			void	setDefaultPage(string defaultPage) { this->defaultPage = defaultPage; }
			void	setRedirect(string redirect) { this->redirect = redirect; }
			void	setAutoindex(bool autoindex) { this->autoindex = autoindex; }
			void	setCgi(bool cgi) { this->cgi = cgi; }
			void	setCgiGet(bool cgiGet) { this->cgiGet = cgiGet; }
			void	setCgiPost(bool cgiPost) { this->cgiPost = cgiPost; }
			void	setCgiDelete(bool cgiDelete) { this->cgiDelete = cgiDelete; }
			void	setUpload(bool upload) { this->upload = upload; }

			// GETTERS
			string	getRoot() const { return (this->root); }
			string	getDefaultPage() const { return (this->defaultPage); }
			string	getRedirect() const { return (this->redirect); }
			bool	getAutoindex() const { return (this->autoindex); }
			bool	getCgi() const { return (this->cgi); }
			bool	getCgiGet() const { return (this->cgiGet); }
			bool	getCgiPost() const { return (this->cgiPost); }
			bool	getCgiDelete() const { return (this->cgiDelete); }
			bool	getUpload() const { return (this->upload); }

		private :
			string		root;
			string		defaultPage;
			string		redirect;
			bool		autoindex;
			bool		cgi;
			bool		cgiGet;
			bool		cgiPost;
			bool		cgiDelete;
			bool		upload;
	};
}

#endif