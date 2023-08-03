#include "Request.hpp"

namespace ws
{
	void ws::Request::error()
	{
		// std::cout << "Error" << this->status << std::endl;
		exit(0);
	};

	int ws::Request::not_allowed_char(std::string uri)
	{
		int i = 0;
		while(uri[i])
		{
			if (uri[i] == ' ' && uri[i + 1] && uri[i + 1] == 'H')
				break;
			else
				i++;
		}
		uri = uri.substr(0, i);
		if (uri.length() >= 2048)
			this->set_status(414);
		const std::string allowedCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=";
		for (std::string::const_iterator it = uri.begin(); it != uri.end(); ++it) 
		{
			if (allowedCharacters.find(*it) == std::string::npos)
				return 1;
		}
		return 0;
	};

	std::string get_extention(std::string s)
	{
		int i = 0;
		std::string ext;
		s = s.erase(s.size() - 1);
		while (s[i] != '/')
			i++;
		ext = s.substr(i + 1);
		return (ext);
	};

	void ws::Request::parse_header(std::string body)
	{
		int j = 0;
		int first = 0;
		std::string key;
		std::string value;
		while (body[j] && body[j + 1] && body[j + 2] && body[j + 3])
		{
			if(body[j] == '\r' && body[j + 1] == '\n' && body[j + 2] == '\r' && body[j + 3] == '\n')
				break;
			while (body[j] != ' ' && body[j] != ':')
			{
				if (body[j] == '\n')
					j++;
				key.push_back(body[j]);
				j++;
			}
			if (first == 0)
			{
				if (!(key.compare("GET")))
					this->set_method(1);
				else if (!(key.compare("POST")))
					this->set_method(2);
				else if (!(key.compare("DELETE")))
					this->set_method(3);
				else
					this->set_status(405);
				first = 1;
			}
			while (body[j + 1] != '\r')
			{
				value.push_back(body[j + 2]);
				j++;
			}
			this->key = key;
			this->value = value;
			this->header_map[this->key] = this->value;
			while (key.size() > 0)
				key.pop_back();
			while (value.size() > 0)
				value.pop_back();
			j++;
		}
		this->delim = j;
		setStoprn(j);
		this->check_errors();
		query = "";
		if (location.find("?") != std::string::npos)
		{
			query = location.substr(location.find("?") + 1);
			location = location.substr(0, location.find("?"));
		}
	};

	int ws::Request::check_rn(std::string body)
	{
		if (body.find("\r\n\r\n") != std::string::npos)
		{
			stopRn = body.find("\r\n\r\n");
			return 1;
		}
		return 0;
	};

	void Request::check_errors()
	{
		if (this->get_method() == 1)
		{
			int i = 0;
			std::string location1 = this->header_map["GET"].c_str();
			while (location1[i] != ' ')
				i++;
			this->location = "/" + location1.substr(0, i);
			setlocation(this->location);
			if (this->not_allowed_char(this->header_map["GET"].c_str()))
			{
				this->set_status(400);
			}
		}
		else if (this->get_method() == 2)
		{
			int i = 0;
			std::string location1 = this->header_map["POST"].c_str();
			while (location1[i] != ' ')
				i++;
			this->location = "/" + location1.substr(0, i);
			setlocation(this->location);
			if (this->not_allowed_char(this->header_map["POST"].c_str()))
			{
				this->set_status(400);
			}
		}
		else if (this->get_method() == 3)
		{
			int i = 0;
			std::string location1 = this->header_map["DELETE"].c_str();
			while (location1[i] != ' ')
				i++;
			this->location = "/" + location1.substr(0, i);
			setlocation(this->location);
			if (this->not_allowed_char(this->header_map["DELETE"].c_str()))
			{
				this->set_status(400);
			}
		}
		if (this->header_map["\rContent-Type"].size() > 0)
		{
			this->extention = get_extention(this->header_map["\rContent-Type"].c_str());
			std::cout << std::endl;
			std::cout << "extention === " << this->extention << std::endl;
			std::cout << "Content-Type === " << this->header_map["\rContent-Type"] << std::endl;
			std::cout << std::endl;
			set_flag(1);
		}
		if (this->header_map["\rContent-Length"].size() > 0)
		{
			this->set_double_kind(1);
			this->set_body_kind(2);
			set_flag(1);
		}
		if (this->header_map["\rTransfer-Encoding"].size() > 0)
		{
			if (get_double_kind() == 1)
				this->set_status(404);
			else if (strcmp(this->header_map["\rTransfer-Encoding"].c_str(), "chunked\r") != 0)
			{
				this->set_status(501);
			}
			else
			{
				this->set_body_kind(1);
				set_flag(1);
			}
		}
	};

	int Request::wait_for_size(std::string body)
	{
		int j = this->delim;
		std::string body2 = body.substr(j);
		try
		{
			// std::cout << "Content length === " << (unsigned long)stoi(this->header_map["\rContent-Length"]) << std::endl;
			// std::cout << "Body length === " << body2.size() << std::endl;
			if (body2.length() >= (unsigned long)stoi(this->header_map["\rContent-Length"]))
				return 1;
		}
		catch(const std::exception& e)
		{
			std::cerr << "" << '\n';
		}
		return 0;
	};

	int Request::wait_for_zero(std::string body)
	{
		if (body.find("0\r\n\r\n") != std::string::npos)
			return 1;
		return 0;
	};

	int ws::Request::check_rn2(std::string body)
	{
		if(body.find("\r\n\r\n") != std::string::npos)
			return 1;
		return 0;
	};

	void	Request::set_inittt()
	{
		if (get_flag2() == 0)
		{
			if (check_rn(body))
			{
				parse_header(body);
				setStoprn(stopRn + 4);
				set_flag2(1);
				if (status != 200)
					set_init(-1);
			}
		}
		if (get_body_kind()== 1 && get_method() == 2)
		{
			usleep(200);
			if(wait_for_zero(body))
				set_init(1);
		}
		else if (get_body_kind() == 2 && get_method() == 2)
		{
			if (wait_for_size(body))
				set_init(1);
		}
		if (get_method() == 1 || this->get_method() == 3)
		{
			if (check_rn2(body))
				set_init(1);
		}
	};

	int the_end(std::string line)
	{
		int i = line.length();
		if (i != 0 && line[i] == '-' && line[i - 1] == '-')
			return 1;
		return 0;
	};

	int hexToDigit(const std::string& hexChar) {
		if (hexChar >= "0" && hexChar <= "9") {
			return hexChar[0] - '0';
		} else if (hexChar >= "A" && hexChar <= "F") {
			return hexChar[0] - 'A' + 10;
		} else if (hexChar >= "a" && hexChar <= "f") {
			return hexChar[0] - 'a' + 10;
		}
		return -1;
	};

	int hexToDecimal(std::string& hexStr)
	{
		if (!strcmp(hexStr.c_str(), "0\r\n\r\n"))
			return (0);
		int decimalValue = 0;
		int base = 1;

		for (int i = hexStr.length() - 1; i >= 0; i--) {
			char digit = hexStr[i];

			int digitValue;
			if (digit >= '0' && digit <= '9') {
				digitValue = digit - '0';
			} else if (digit >= 'A' && digit <= 'F') {
				digitValue = digit - 'A' + 10;
			} else if (digit >= 'a' && digit <= 'f') {
				digitValue = digit - 'a' + 10;
			} else {
				return 0;
			}
			decimalValue += digitValue * base;
			base *= 16;
		}
		return decimalValue;
	}

	std::string ws::Request::to_rn(std::string body)
	{
		int i = 0;
		std::string hexa;
		while (body[i] != '\r' && body[i + 1] && body[i + 1] != '\n')
		{
			hexa.push_back(body[i]);
			i++;
		}
		return hexa;
	};

	void ws::Request::upload1(std::string body)
	{
		int tmp_stop_rn, chunk_size, x;
		int i = 0;
		if (file_cr == 0)
		{ 
			stopRn = get_stopRn();
			std::string fileName = myLocation.getRoot() + generateName() + "." + extention;
			filee.open(myLocation.getRoot() + generateName() + "." + extention,std::ios::out | std::ios::app);
			cgiFile = fileName;
			file_cr++;
		}
		tmp_stop_rn = stopRn;
		while (true) {
			std::string hexa = to_rn(&body[stopRn]);
			x = stopRn;
			tmp_stop_rn += hexa.size() + 2;
			chunk_size = hexToDecimal(hexa);
			for (i = 0; i < chunk_size && (tmp_stop_rn + i) < (int)body.size();) {
				i++;
			}
			if (i != chunk_size) {
				body = body.substr(x, body.size()-x);
				break;
			}
			for (i = 0; i < chunk_size && (tmp_stop_rn + i) < (int)body.size(); i++) {
				filee << body[tmp_stop_rn + i];
			}
			if (!chunk_size) {
				break;
			}
			tmp_stop_rn += chunk_size + 2;
			setStoprn(tmp_stop_rn);
		}
		filee.close();
		return ;
	};

	void ws::Request::upload2(std::string body)
	{
		if (get_file_cr() == 0)
		{
			std::string fileName = myLocation.getRoot() + generateName() + "." + extention;
			filee.open(fileName, std::ios::out | std::ios::app );
			cgiFile = fileName;
			set_file_cr(1);
		}
		while ((size_t)stopRn < body.size())
		{
			filee << body[stopRn];
			stopRn++;
		}
		filee.close();
		setStoprn(stopRn);
	};

	void ws::Request::slash_n(std::string s)
	{
		int sl = 0;
		int i = 0;
		while (s[i])
		{
			if (s[i] == '/')
				sl++;
			i++;
		}
		setslash(sl);
	};

	std::string ws::Request::remove_slash(std::string s)
	{
		std::string new_str;
		int i = 0;
		int sl = 0;
		while (s[i])
		{
			if (s[i] == '/')
			{
				sl++;
				if (sl == getslash())
					break;
			}
			i++;
		}
		this->slash -= 1;
		new_str = s.substr(0, i);
		return (new_str);
	};


	std::string ws::Request::get_matched(std::string s)
	{
		// std::cout << "------------------------------------------------------" << std::endl;
		// std::cout << "  s ==  " << s << std::endl;
		// std::cout << "------------------------------------------------------" << std::endl;
		slash_n(s);
		const std::map<std::string, ws::LocationData>& locations = server.getLocations();
		for (std::map<std::string, ws::LocationData>::const_iterator it2 = locations.begin(); it2 != locations.end(); ++it2)
		{
			if (!strcmp(it2->first.c_str(), s.c_str()))
			{
				try
				{
					std::string check = location.substr(it2->first.size());
					if (check[0] == '/')
						this->final_path = it2->second.getRoot().substr(0, it2->second.getRoot().size()) + this->location.substr(it2->first.size() + 1);
					else
						this->final_path = it2->second.getRoot().substr(0, it2->second.getRoot().size()) + this->location.substr(it2->first.size());
					myLocation = it2->second;
					this->compareFlag = 1;
					return this->final_path;
				}
				catch(const std::exception& e)
				{
					std::cerr << "excep" << '\n';
				}
			}
		}
		// std::cout << "compareFlag ==  " << compareFlag << std::endl;
		if (compareFlag == 0)
		{
			std::string new_s = remove_slash(s);
			if (new_s.size() != 0)
			{
				if (get_matched(new_s) != "")
					return final_path;
			}
			else
			{
				if (get_matched("/") != "")
					return final_path;
			}

		}
		else
			return final_path;
		set_status(404);
		return std::string() ;
	};

	std::string ws::Request::generateName()
	{
		const std::string vowels = "aeiou";
		const std::string consonants = "bcdfghjklmnpqrstvwxyz";
		std::string name;
		srand(time(0));
		int nameLength = rand() % 5 + 1;
		for (int i = 0; i < nameLength; ++i)
		{
			if (i % 2 == 0) 
				name += vowels[rand() % vowels.size()];
			else
				name += consonants[rand() % consonants.size()];
		}
		return name;
	};
}