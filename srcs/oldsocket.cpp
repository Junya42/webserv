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
	  // std::cout << stream.str() << std::endl;
	  //std::cout << "_______________END OF STREAM_______________________" << std::endl;
  }
  if (!body.size() && method.find("POST") != std::string::npos) {
	  std::cout << "bytes left = " << len << std::endl;
	  std::stringstream sbody;
	  char buffer[1024];
	  int bytes_read;
	  std::size_t bytes_left = len;

	  // Read from the socket until we have read the entire body
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
	  //std::cout << request << std::endl;
	  //exit (0);
  }
}

std::string handle_request(std::string &request, std::vector<Server> &serv, Client &client, int socket) {
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
				//type = "HTTP/1.1 200 OK\n";
				//return type;

				//type = "application/octet-stream\n";
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
					//if (serv[i]._loc[j]._path.find(path) != std::string::npos) {
					//if (path.find(serv[i]._loc[j]._path) != std::string::npos && found == 0)
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
		//int fd;
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
		/*while (bytes < size) {
		  read_bytes = fread(buffer, 1, sizeof(buffer), p_file);
		  bytes += read_bytes;
		  s_buffer += buffer;
		  if (read_bytes < sizeof(buffer)) {
		  break ;
		  }
		  }*/
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
		//std::cout << "Creating answer" << std::endl;
		answer = "HTTP/1.1 200 OK\n";
		//std::cout << "Http status" << std::endl;
		answer += "Content-Type: ";
		answer += type;
		//std::cout << "Content type" << std::endl;
		answer += "Content-Length: ";
		//std::cout << "Content Lenght" << std::endl;
		//std::cout << "Before adding str size to answer" << std::endl;
		answer += s.str();
		//std::cout << "After adding str size to answer" << std::endl;
		answer += "\r\n\r\n";
		//std::cout << "Before adding buffer to answer" << std::endl;
		//std::cout << "s_buffer: " << s_buffer << std::endl;
		answer += s_buffer;
		//std::cout << "After adding buffer to answer" << std::endl;

		if (check) {
			std::cout << std::endl << "Answer: " << std::endl << answer << std::endl
				<< "__________________________________________" << std::endl;
		}
		return answer;
	}

	void	server_handler(Config &config) {

		int my_bool = 1;
		int server; //server socket fd
		int	client; //client socket fd
		char buffer[1024]; // buffer to store client request
		int	bytes; //return of read(client, buffer, sizeof(buffer))
		int	epoll_fd; //epoll instance 
		int num_events; //number of events occuring in epoll wait
		struct epoll_event event; //epoll event specifier
		struct sockaddr_in server_addr; //socket informations
		std::vector<Client> vec_clients;
		Client	clientinfo;

		clientinfo.reset();
		std::cout << "initating socket" << std::endl;
		/* creating socket for IPV4 using SOCK_STREAM connection based protocol between two parties */
		server = socket(AF_INET, SOCK_STREAM, 0); 
		if (server == -1) {
			std::cout << "Error : SOCKET" << std::endl;
			std::cout << std::strerror(errno) << std::endl;
			exit(errno);
		}

		/* setsockopt = set socket options
		 *
		 * first param is the socket
		 *
		 * second param is the target, SOL_SOCKET = this->socket
		 *
		 * third param is the option, SO_REUSEADDR is useful because if we close the server
		 * and try to launch it again, we won't be allowed because the address and port will still be in use until
		 * we manually kill the previous server process.
		 * SO_REUSEADDR allows us to keep using the same address and port without conflict
		 *
		 * fourth param is a flag, SO_REUSEADDR needs a boolean value true or false (1 or 0), to enable SO_REUSEADDR
		 * or to disable it, we are sending 1
		 *
		 * fifth param is the size of the flag */

		if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &my_bool, sizeof(my_bool)) == -1) {

			std::cout << "Error : SETSOCKOPT" << std::endl;
			std::cout << std::strerror(errno) << std::endl;
			close(server);
			exit(errno);
		}

		/*	Clearing socketaddr_in before use just in case */
		std::memset(&server_addr, 0, sizeof(server_addr));

		/* Setting socket informations */
		server_addr.sin_addr.s_addr = INADDR_ANY; //allows connections to all interfaces, not just localhost
		server_addr.sin_family = AF_INET; //allows connection to IPV4
		server_addr.sin_port = htons(PORT); //converting host port to network valid port

		std::cout << "binding socket" << std::endl;
		/* Binding informations to server socket */
		if (bind(server, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)) == -1) {
			std::cout << "Error : BIND" << std::endl;
			std::cout << std::strerror(errno) << std::endl;
			close(server);
			exit(errno);
		}

		std::cout << "listening on socket" << std::endl;
		/*	Make the server listen, and accept a queue of 5 pendings connection */
		if (listen(server, 5) == -1) {
			std::cout << "Error : LISTEN" << std::endl;
			std::cout << std::strerror(errno) << std::endl;
			close(server);
			exit(errno);
		}

		std::cout << "Epoll create(5)" << std::endl;
		/* Creating epoll API and allows MAX_EVENTS events to happen simultaneously */
		epoll_fd = epoll_create(MAX_EVENTS);
		if (epoll_fd == -1) {
			std::cout << "Error : EPOLL_CREATE" << std::endl;
			std::cout << std::strerror(errno) << std::endl;
			close(server);
			exit(errno);
		}
		/* Telling epoll how to handle the server socket */
		event.events = EPOLLIN; //Set server available for read operations
		event.data.fd = server; //link to server socket

		std::cout << "Epoll_ctl" << std::endl;
		/* Epoll_ctl allows us to control the epoll API
		 *
		 * first argument is the Epoll API instance
		 *
		 * second argument is the option, in this case we are using EPOLL_CTL_ADD, to add our socket to EPOLL
		 *
		 * third argument is the targeted socket
		 *
		 * fourth argument are the informations on how to handle the socket */
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server, &event) == -1) {
			std::cout << "Error : EPOLL_CTL" << std::endl;
			std::cout << std::strerror(errno) << std::endl;
			close(epoll_fd);
			close(server);
			exit(errno);
		}

		/* Same as how we handled the server socket and ported him to the Epoll API, we also need
		 * to port our clients socket, so we're making a client socket epoll data array */
		struct epoll_event events[MAX_EVENTS];

		int curr_fd = 1; // Number of actual clients fd + 1;

		std::cout << "While loop" << std::endl;
		/* Server loop, it will keep running until receiving a signal or an error happens */
		std::vector<sockaddr_in> client_info;
		int numclient = 0;
		while (1) {

			std::cout << "Epoll_wait" << std::endl;
			//waiting for an event
			num_events = epoll_wait(epoll_fd, events, curr_fd, -1);
			std::cout << "num_events: " << num_events << std::endl;
			if (num_events < 0) {
				std::cout << "Error : EPOLL_WAIT" << std::endl;
				std::cout << std::strerror(errno) << std::endl;
				close(epoll_fd);
				close(server);
				exit(errno);
			}
			/* Looping through all happening events */
			for (int i = 0; i < num_events; i++) {
				if (events[i].data.fd == server) {

					sockaddr_in client_addr; //creating client address buffer
					socklen_t client_addr_len = sizeof(client_addr); //size of client address buffer

					numclient++;
					std::cout << "Accepting client: " << numclient << std::endl;
					/* Extract the first connection request on pending connections for the listening socket server
					 * and creates a new fd related to the socket server */
					client = accept(server, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_len);
					if (client == -1) {
						std::cout << "Error : ACCEPT" << std::endl;
						std::cout << std::strerror(errno) << std::endl;
						close(server);
						exit(errno);
					}
					client_info.push_back(client_addr);
					char buffer_addr[INET_ADDRSTRLEN];
					std::cout << "Client: " << inet_ntop(AF_INET, &client_addr.sin_addr, buffer_addr, sizeof(buffer_addr)) << " | " << client_addr.sin_port << std::endl;
					std::cout << "Test buffer: " << buffer_addr;
					struct epoll_event client_event;

					client_event.events = EPOLLIN | EPOLLOUT; //set client available for read operations
					client_event.data.fd = client; //link to client socket

					std::cout << "Epoll_ctl" << std::endl;
					/* Adding client to epoll API */
					if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client, &client_event) < 0) {
						std::cout << "Error: EPOLL_CTL Client" << std::endl;
						std::cout << std::strerror(errno) << std::endl;
						close(client);
						close(server);
						exit(errno);
					}
					clientinfo._port = client_addr.sin_port;
					clientinfo._id = i;
					clientinfo._sock = client;
					vec_clients.push_back(clientinfo);
					clientinfo.reset();

					for (std::vector<Client>::iterator it = vec_clients.begin(); it != vec_clients.end(); it++) {
						std::cout << *it << std::endl;
					}
					curr_fd++;
				}
				else {
					client = events[i].data.fd;
					if (vec_clients[i]._sock == client) {
						std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << std::endl;
						std::cout << "Find client in vector" << std::endl;
						std::cout << vec_clients[i] << std::endl;
						std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << std::endl;
					}
					else {
						std::cout << "......................................." << std::endl;
						std::cout << "Couldnt find client in vec_clients" << std::endl;
						std::cout << "......................................." << std::endl;
					}
					char buffer_addr[INET_ADDRSTRLEN];
					//bytes = read(client, buffer, sizeof(buffer));
					size_t current_bytes = 0;
					bytes = 0;
					std::stringstream readbuff;
					while ((current_bytes = recv(client, buffer, sizeof(buffer), 0)) > 0) {
						bytes += current_bytes;
						readbuff << std::string(buffer, current_bytes);
						if (readbuff.str().find("\r\n\r\n") != std::string::npos) {
							break;
						}
					}
					std::cout << "READBUFFER size: " << bytes << std::endl << std::endl;
					std::cout << readbuff.str() << std::endl
						<< "oooooooooooooooooooooooooooooooooooooooooooooo" << std::endl;
					std::cout << std::endl << std::endl << "BUFFER = " << std::endl << std::endl;
					std::cout << readbuff.str() << std::endl
						<< "oooooooooooooooooooooooooooooooooooooooooooooo" << std::endl;
					if (bytes > 0) {
						std::cout << "Successfully read " << bytes << " bytes from data" << std::endl;
						std::cout << "Number of client: " << client_info.size() << std::endl;
						for (size_t x = 0; x < client_info.size(); x++) {
							std::cout << "Retrieving client [" << x << "] from vector :" << std::endl;
							std::cout << inet_ntop(AF_INET, &client_info[x].sin_addr, buffer_addr, sizeof(buffer_addr)) << " | " << client_info[x].sin_port << std::endl;
						}
						std::cout << "Current Client " << i << ": "<< inet_ntop(AF_INET, &client_info[i].sin_addr, buffer_addr, sizeof(buffer_addr)) << " | " << client_info[i].sin_port << std::endl;
						std::string request(buffer, bytes);
						std::string response = handle_request(request, config._serv, vec_clients[i], client);
						write(client, response.c_str(), response.size());
					}
					else {
						std::cout << "Error: Reading data from client | Removing client" << std::endl;
						/* Removing client socket from epoll instance */
						if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client, nullptr) < 0) {
							std::cout << "Error removing client socket from epoll instance" << std::endl;
						}
						curr_fd--;
						numclient--;
						vec_clients.erase(vec_clients.begin() + i);
						client_info.erase(client_info.begin() + i);
						close(events[i].data.fd);
					}
					//close(client);
				}
				//close (events[i].data.fd);
			}
		}
		close(server);
	}
