#include "Request.hpp"

void ws::Request::error()
{
	std::cout << "Error" << this->status << std::endl;
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
	while (s[i] != '/')
		i++;
	ext = s.substr(i + 1, s.size());
	return (ext);
};

void ws::Request::parse_header(std::string body)
{
	int j = 0;
	int first = 0;
	std::string key;
	std::string value;
	while (body[j])
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
	this->check_errors();
};

int ws::Request::check_rn(std::string body)
{
	int j = 0;
	while (body[j])
	{
		if(body[j] == '\r' && body[j + 1] == '\n' && body[j + 2] == '\r' && body[j + 3] == '\n')
			return 1;
		j++;
	}
	return 0;
};

void ws::Request::check_errors()
{
	if (this->get_method() == 1)
	{
		int i = 0;
		std::string location1 = this->header_map["GET"].c_str();
		while (location1[i] != ' ')
			i++;
		this->location = "/" + location1.substr(0, i);
		if (this->not_allowed_char(this->header_map["GET"].c_str()))
		{
			this->set_status(400);
			return ;
		}
	}
	else if (this->get_method() == 2)
	{
		int i = 0;
		std::string location1 = this->header_map["POST"].c_str();
		while (location1[i] != ' ')
			i++;
		this->location = "/" + location1.substr(0, i);
		if (this->not_allowed_char(this->header_map["POST"].c_str()))
		{
			this->set_status(400);
			return ;
		}

	}
	else if (this->get_method() == 3)
	{
		int i = 0;
		std::string location1 = this->header_map["DELETE"].c_str();
		while (location1[i] != ' ')
			i++;
		this->location = "/" + location1.substr(0, i);
		if (this->not_allowed_char(this->header_map["DELETE"].c_str()))
		{
			this->set_status(400);
			return ;
		}
	}
	if (this->header_map["\rContent-Length"].size() > 0)
	{
		this->set_body_kind(2);
		this->flag = 1;
		return ;
	}
	else if (this->header_map["\rTransfer-Encoding"].size() > 0)
	{
		if (strcmp(this->header_map["\rTransfer-Encoding"].c_str(), "chunked\r") != 0)
		{
			this->set_status(501);
			return;
		}
		else
		{
			this->set_body_kind(1);
			this->flag = 1;
			return;
		}
	}
	if (this->header_map["\rContent-Type"].size() > 0)
	{
		this->extention = get_extention(this->header_map["\rContent-Type"].c_str());
		this->flag = 1;
		return ;
	}
	if (this->flag != 1)
	{
		this->set_status(404);
		return ;
	}
};

int ws::Request::wait_for_size(std::string body)
{
	int j = this->delim;
	std::string body2 = body.substr(j);
	try
	{
		if (body2.length() >= (unsigned long)stoi(this->header_map["\rContent-Length"]))
			return 1;
	}
	catch(const std::exception& e)
	{
		std::cerr << "" << '\n';
	}
	return 0;
};

int ws::Request::wait_for_zero(std::string body)
{
    if (body.find("0\r\n\r\n") != std::string::npos)
        return 1;
	return 0;
};

void	ws::Request::set_inittt()
{
	int k = 0;
	if (this->check_rn(this->body))
	{
		if (k == 0)
		{
			this->parse_header(this->body);
			k = 1;
			if (this->status != 200)
				this->set_init(-1);
		}
	}
	if (this->get_body_kind()== 1 && this->init != -1)
	{
		usleep(200);
		if(this->wait_for_zero(this->body))
			this->set_init(1);
	}
	else if (this->get_body_kind() == 2)
	{
		if (this->wait_for_size(this->body))
			this->set_init(1);
	}
};

int the_end(std::string line)
{
	int i = line.length();
	if (line[i] == '-' && line[i - 1] == '-')
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

int hexToDecimal(std::string& hexString) {
    int decimal = 0;
    int power = 0;

    for (int i = hexString.length() - 2; i >= 0; i--) {
        char currentChar = hexString[i];
    	int digitValue;

        if (currentChar >= '0' && currentChar <= '9') {
            digitValue = currentChar - '0';
        }
        else if (currentChar >= 'A' && currentChar <= 'F') {
            digitValue = currentChar - 'A' + 10;
        }
        else if (currentChar >= 'a' && currentChar <= 'f') {
            digitValue = currentChar - 'a' + 10;
        }
        else {
            std::cerr << "Invalid hexadecimal digit: " << currentChar << std::endl;
            return 0;
        }
        decimal += digitValue * static_cast<int>(std::pow(16, power));
        power++;
    }
    return decimal;
}

void ws::Request::upload(std::fstream& file)
{
	int i = 0;
	std::string output;
	std::string body;
	if (!file.is_open())
		this->error();
	file.seekg(0);
	while(i < (int)this->header_map.size())
	{
		getline(file, output);
		i++;
	}
	if (this->get_body_kind() == 1)
	{
		int length = 1;
		char c;
		std::string output;
		std::ofstream filee;
		std::string kk = this->generateName() + ".";
		kk += this->extention;
		filee.open("../uploads/" + kk);
		while(length != 0)
		{
			try
			{
				getline(file, output);
				int length = hexToDecimal(output);
				if (length == 0)
					break;
				for(int i = 0; i < length; i++)
				{
					file.get(c);
					this->last_body.push_back(c);
				}
				file.get(c);
				file.get(c);
			}
			catch(const std::exception& e)
			{
				std::cerr << "" << '\n';
			}
			
		}
		file.close();
		for (size_t i = 0; i < this->last_body.size(); ++i)
			filee << this->last_body[i];
		filee.close();
		this->set_status(201);
	}
	if (this->get_body_kind()== 2)
	{
		try
		{
			getline(file, output);
			size_t length = stoi(this->header_map["\rContent-Length"]);
			std::ofstream filee;
			std::string kk = this->generateName() + ".";
			kk += this->extention;
			filee.open("../uploads/" + kk);
			char c;
			while (file.get(c))
				this->last_body.push_back(c);
			file.close();
			for (size_t i = 0; i < length; ++i) {
					filee << this->last_body[i];
				}
			filee.close();
			this->set_status(201);
		}
		catch(const std::exception& e)
		{
			std::cerr << "" << '\n';
		}
	}
}

std::string ws::Request::compare(std::string s)
{
	if (this->location.size() == 1)
	{
		if (s == this->location)
			return this->location;
	}
	int i = s.size();
	if (this->location[i] != '\0' && this->location[i] != '/')
		return std::string();
	if (i != 1)
	{
		int o = 0;
		o = s.substr(0, i).compare(this->location.substr(0, i));
		if (o == 0)
		{
			std::string path = this->location.substr(i);
			this->compairFlag = 1;
			return path;
		}
	}
	return std::string();
}

void ws::Request::get_matched()
{
	const std::map<std::string, ws::LocationData>& locations = server.getLocations();
	for (std::map<std::string, ws::LocationData>::const_iterator it2 = locations.begin(); it2 != locations.end(); ++it2)
	{
		compare(it2->first).size();
		if (this->compairFlag == 1)
		{
			this->final_path = it2->second.getRoot() + compare(it2->first);
			myLocation = it2->second;
			return ;
		}
	}
	if (this->final_path.size() == 0)
	{
		std::cout << "not found 404\n";
	}
};

std::string ws::Request::generateName() {
    const std::string vowels = "aeiou";
    const std::string consonants = "bcdfghjklmnpqrstvwxyz";
    std::string name;
    
    srand(time(0));  // Initialize random seed
    
    // Generate random name with a maximum size of 5 characters
    int nameLength = rand() % 5 + 1;
    
    for (int i = 0; i < nameLength; ++i) {
        if (i % 2 == 0) {  // Generate a vowel
            name += vowels[rand() % vowels.size()];
        } else {  // Generate a consonant
            name += consonants[rand() % consonants.size()];
        }
    }
    
    return name;
};
