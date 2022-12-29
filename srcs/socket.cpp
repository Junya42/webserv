#include <asm-generic/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <ostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unordered_map>
#include <sstream>
#include <stdio.h>
#include "../includes/config.hpp"

const int PORT = 8080;
const int MAX_EVENTS = 10;

void	parse_request(const std::string& request, std::string& method, std::string& path, std::unordered_map<std::string, std::string>& headers, std::unordered_map<std::string, std::string>& body, int client) {
  std::istringstream stream(request);

  stream >> method >> path;
  std::string line;
  size_t len = 0;

  while (std::getline(stream, line) && line != "\r") {
    std::istringstream line_stream(line);
    std::string key;
    std::string value;
    std::getline(line_stream, key, ':');
    std::getline(line_stream, value);
	if (key.find("Content-Length") != std::string::npos){
		len = atoi(value.c_str());
		std::cout << "CONTENT LENGHT FOUND : " << len << std::endl;
	}
    headers[key] = value;
  }
  if (method.find("POST") != std::string::npos) {
	  std::cout << "_______________POST METHOD ENCOUNTER_______________" << std::endl;
	  while (std::getline(stream, line)) {
		  std::istringstream line_stream(line);
		  std::string key;
		  std::string value;
		  while(std::getline(line_stream, key, '=')) {
			  std::getline(line_stream, value, '&');
			  body[key] = value;
		  }
	  }
  }
  if (!body.size() && method.find("POST") != std::string::npos) {
	  std::cout << "bytes left = " << len << std::endl;
	  std::stringstream sbody;
	  char buffer[1024];
	  int bytes_read;
	  std::size_t bytes_left = len;

	  while (bytes_left > 0 && (bytes_read = recv(client, buffer, sizeof(buffer), 0)) > 0) {
		  sbody << std::string(buffer, bytes_read);
		  bytes_left -= bytes_read;
	  }
	  std::cout << "Maybe body: " << std::endl << std::endl
		  << sbody.str() << std::endl
		  << ">>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
  }
  if (!body.size() && method.find("POST") != std::string::npos) {
	  std::cout << "Couldn't get body from POST REQUEST" << std::endl << std::endl << std::endl;
  }
}

std::string handle_request(std::string &request, std::vector<Server> &serv, Client &client, int socket)
{
	std::string method;
	std::string path;
	std::string host;
	std::string host_save;
	std::string index;
	std::string s_buffer;
	std::string	type = "text/html\n";
	std::ostringstream s;
	std::unordered_map<std::string, std::string> header;
	std::unordered_map<std::string, std::string> body;
	size_t size = 0;
	char buffer[4096];

	parse_request(request, method, path, header, body, socket);
	std::string::iterator pathit = path.end() - 1;
	if (*pathit == '/' && path.size() > 1) {
		path.pop_back();
	}
	std::cout << "Method: " << method << std::endl;
	std::cout << "Path: " << path << std::endl;
	std::cout << "_____Header_____" << std::endl;
	for (std::unordered_map<std::string, std::string>::iterator it = header.begin(); it != header.end(); it++) {
		std::cout << "  " << it->first << ": " << it->second << std::endl;
		if (it->first == "Host") {
			host = it->second;
			host_save = host;
		}
	}
	std::cout << "________END OF HEADER__________" << std::endl;
	if (body.size()) {
		std::cout << "_____Body______" << std::endl;
		for (std::unordered_map<std::string, std::string>::iterator it = body.begin(); it != body.end(); it++) {
			std::cout << "  " << it->first << ": " << it->second << std::endl;
			if (it->first.find("fname") != std::string::npos)
				client._name = it->second;
			else if (it->first.find("lname") != std::string::npos)
				client._lastname = it->second;
		}
		std::cout << "________END OF BODY__________" << std::endl;
		if (client._name.size() && client._lastname.size())
			client._log = true;
	}
	if (!host.size())
		std::cout << "HOST PROBLEM" << std::endl;
	std::cout << "HEADER[HOST]:" << header["Host"] << std::endl;
	std::cout << "Parsing through all host in vector" << std::endl;
	for (unsigned long int i = 0; i < serv.size(); i++)
	{
		std::cout << "serv["<<i<<"]._host:"<<serv[i]._host << "| : " << serv[i]._host.size() <<std::endl;
		if (host.find(serv[i]._host) != std::string::npos)
		{
			std::cout << "Found matching host" << std::endl;
			if (path.compare("/") == 0)
			{
				std::cout << "Using server index" << std::endl;
				host = serv[i]._index;
			}
			else if (path.find(".ico") != std::string::npos)
			{
				std::cout << "Favicon request" << std::endl;
				host = "./favicon.ico";
				type = "image/x-icon\nContent-Transfer-Encoding: binary\n";
			}
			else
			{
				std::cout << "Trying location index with serv[" << i << "] : size: "
					<< serv[i]._loc.size() << std::endl;
				path.insert(0, 1, ' ');
				int found = 0;
				for (unsigned long int j = 0; j < serv[i]._loc.size(); j++)
				{
					std::cout << "Loc :" << serv[i]._loc[j]._path << " : Size: "
						<< serv[i]._loc[j]._path.size() << " # Path :" << path
						<< " : Size: " << path.size() << std::endl;
					if (path.compare(serv[i]._loc[j]._path) == 0 && found == 0)
					{
						found++;
						host = serv[i]._loc[j]._root + "index.html";
						std::cout << "Found location index" << std::endl;
						break ;
					}
				}
				if (host_save.compare(host) == 0)
				{
					std::cout << "Couldn't find location index" << std::endl;
					exit(1);
				}
			}
			break ;
		}
	} 
	while (host[0] == ' ' || host[0] == '\t')
		host.erase(host.begin());
	std::cout << "new host value:" << host << std::endl;
	FILE *p_file = NULL;
	int pos = 0;

	std::cout << "index path:" << host << std::endl;
	p_file = fopen(host.c_str(), "rb");
	if (!p_file) {
		std::cerr << "Invalid fopen index" << std::endl;
		exit (1);
	}
	pos = ftell(p_file);
	fseek(p_file, 0, SEEK_END);
	size = ftell(p_file);
	fseek(p_file, pos, SEEK_SET);
	size_t bytes = 0;
	size_t read_bytes = 0;
	(void)buffer;
	char *buf = (char *)malloc(sizeof(char) * size);
	int check = 0;
	if (buf != NULL) {
		(void)read_bytes;
		bytes = fread(buf, 1, size, p_file);
		std::stringstream out;
		for (size_t i = 0; i < size; i++) {
			if (buf[i] == '$' && client._log == true) {
				check = 1;
				if (buf[i + 1] == 'f') {
					for (size_t j = 0; j < client._name.size(); j++)
						out << client._name[j];
				}
				else if (buf[i + 1] == 'l') {
					for (size_t j = 0; j < client._lastname.size(); j++)
						out << client._lastname[j];
				}
				while (buf[i] != ' ')
					i++;
			}
			out << buf[i];
		}
		std::string copy = out.str();
		s_buffer = copy;
		free(buf);
		(void)bytes;
	}
	fclose(p_file);
	s << size;

	std::string answer;
	answer = "HTTP/1.1 200 OK\n";
	answer += "Content-Type: ";
	answer += type;
	answer += "Content-Length: ";
	answer += s.str();
	answer += "\r\n\r\n";
	answer += s_buffer;

	if (check) {
		std::cout << std::endl << "Answer: " << std::endl << answer << std::endl
			<< "__________________________________________" << std::endl;
	}
	return answer;
}

int init_server_socket(void) {
	int server;
	int my_bool = 1;
	struct sockaddr_in server_addr;

	server = socket(AF_INET, SOCK_STREAM, 0);
	if (server == -1) {
		std::cout << "Error : SOCKET" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		exit(errno);
	}
	if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &my_bool, sizeof(my_bool)) == -1) {

		std::cout << "Error : SETSOCKOPT" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		close(server);
		exit(errno);
	}

	std::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_addr.s_addr = INADDR_ANY; //allows connections to all interfaces, not just localhost
	server_addr.sin_family = AF_INET; //allows connection to IPV4
	server_addr.sin_port = htons(PORT); //converting host port to network valid port

	if (bind(server, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)) == -1) {
		std::cout << "Error : BIND" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		close(server);
		exit(errno);
	}

	if (listen(server, 5) == -1) {
		std::cout << "Error : LISTEN" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		close(server);
		exit(errno);
	}
	return server;
}

int add_client(int server, int epoll_fd, std::vector<Client> &clientlist, int i) {
	Client clientinfo;
	struct epoll_event client_event;
	int	client;

	client = accept(server, reinterpret_cast<sockaddr*>(&clientinfo.addr), &clientinfo.addr_len);
	if (client == -1) {
		std::cout << "Error : ACCEPT" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		close(server);
		exit(errno);
	}
	client_event.events = EPOLLIN | EPOLLOUT; //set client available for read operations
	client_event.data.fd = client; //link to client socket
								   //
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client, &client_event) < 0) {
		std::cout << "Error: EPOLL_CTL Client" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		close(client);
		close(server);
		exit(errno);
	}
	clientinfo._port = clientinfo.addr.sin_port;
	clientinfo._id = i;
	clientinfo._sock = client;
	clientlist.push_back(clientinfo);
	return 0;
}

//remove_client(client, vec_clients, i, &curr_fd, &numclient, events[i].data.fd);
int	remove_client(int client, std::vector<Client>& clientlist, int i, int *curr_fd, int *numclient, int eventfd, int epoll_fd) {
	std::cout << "Error: Reading data from client | Removing client" << std::endl;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client, nullptr) < 0) {
		std::cout << "Error removing client socket from epoll instance" << std::endl;
		return -1;
	}
	*curr_fd = *curr_fd - 1;
	*numclient = *numclient - 1;
	clientlist.erase(clientlist.begin() + i);
	close(eventfd);
	return 0;
}

int	init_epoll(int server) {
	int epoll_fd;
	struct epoll_event event;

	epoll_fd = epoll_create(MAX_EVENTS);
	if (epoll_fd == -1) {
		std::cout << "Error : EPOLL_CREATE" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		close(server);
		exit(errno);
	}
	event.events = EPOLLIN;
	event.data.fd = server;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server, &event) == -1) {
		std::cout << "Error : EPOLL_CTL" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		close(epoll_fd);
		close(server);
		exit(errno);
	}
	return epoll_fd;
}

void	server_handler(Config &config) {

	int server; //server socket fd
	int	client; //client socket fd
	int	epoll_fd; //epoll instance 
	int num_events; //number of events occuring in epoll wait
	struct epoll_event event; //epoll event specifier
	std::vector<Client> vec_clients;

	server = init_server_socket();
	epoll_fd = init_epoll(server);

	struct epoll_event events[MAX_EVENTS];

	int curr_fd = 1; // Number of actual clients fd + server fd;
	int numclient = 0;
	while (1) {
		num_events = epoll_wait(epoll_fd, events, curr_fd, -1);
		if (num_events < 0) {
			std::cout << "Error : EPOLL_WAIT" << std::endl;
			std::cout << std::strerror(errno) << std::endl;
			close(epoll_fd);
			close(server);
			exit(errno);
		}
		for (int i = 0; i < num_events; i++) {
			if (events[i].data.fd == server) {
				add_client(server, epoll_fd, vec_clients, i);
				numclient++;
				curr_fd++;
			}
			else {
				client = events[i].data.fd;
				if (vec_clients[i].request.read_client(client) == 1) {
					//answer client
				}
				if (bytes > 0) {
					std::cout << "Successfully read " << bytes << " bytes from data" << std::endl;
				}
				else {
					remove_client(client, vec_clients, i, &curr_fd, &numclient, events[i].data.fd, epoll_fd);
				}
			}
		}
	}
	close(server);
}
