#include <asm-generic/socket.h>
#include <iostream>
#include <netinet/in.h>
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

const int PORT = 8080;
const int MAX_EVENTS = 10;

void	parse_request(const std::string& request, std::string& method, std::string& path, std::unordered_map<std::string, std::string>& headers) {
  std::istringstream stream(request);

  stream >> method >> path;
  std::string line;

  while (std::getline(stream, line) && line != "\r") {
    std::istringstream line_stream(line);
    std::string key;
    std::string value;
    std::getline(line_stream, key, ':');
    std::getline(line_stream, value);

    headers[key] = value;
  }
}

std::string handle_request(std::string &request) {
	std::string answer;
	std::string method;
	std::string path;
	std::ostringstream s;
	std::unordered_map<std::string, std::string> header;
	int size = 0;
	char buffer[1024];

	parse_request(request, method, path, header);

	std::cout << "Method: " << method << std::endl;
	std::cout << "Path: " << path << std::endl;
	std::cout << "Header:" << std::endl;
	for (std::unordered_map<std::string, std::string>::iterator it = header.begin(); it != header.end(); it++) {
		std::cout << "  " << it->first << ": " << it->second << std::endl;
	}
	if (path.compare("/") == 0) {
		FILE *p_file = NULL;
		int fd;
		int pos = 0;

		p_file = fopen("../index.html", "rb");
		pos = ftell(p_file);
		fseek(p_file, 0, SEEK_END);
		size = ftell(p_file);
		fseek(p_file, pos, SEEK_SET);
		fclose(p_file);

		fd = open("../index.html", O_RDONLY);
		read(fd, buffer, size);
		close(fd);
		s << size;
	}
	answer = "HTTP/1.1 200 OK\n";
	answer += "Content-Type: text/html\n";
	answer += "Content-Length: ";
	answer += s.str();
	answer += "\n\n";
	answer += buffer;

	std::cout << std::endl << "Answer: " << answer << std::endl;
	return answer;
}

int main(void) {

	int my_bool = 1;
	int server; //server socket fd
	int	client; //client socket fd
	char buffer[1024]; // buffer to store client request
	int	bytes; //return of read(client, buffer, sizeof(buffer))
	int	epoll_fd; //epoll instance 
	int num_events; //number of events occuring in epoll wait
	struct epoll_event event; //epoll event specifier
	struct sockaddr_in server_addr; //socket informations

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

	std::cout << "While loop" << std::endl;
	/* Server loop, it will keep running until receiving a signal or an error happens */
	while (1) {

		std::cout << "Epoll_wait" << std::endl;
		//waiting for an event
		num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
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

				std::cout << "Accepting client" << std::endl;
				/* Extract the first connection request on pending connections for the listening socket server
				 * and creates a new fd related to the socket server */
				client = accept(server, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_len);
				if (client == -1) {
					std::cout << "Error : ACCEPT" << std::endl;
					std::cout << std::strerror(errno) << std::endl;
					close(server);
					exit(errno);
				}
				else {
					char buffer_addr[INET_ADDRSTRLEN];
					std::cout << "Client: " << inet_ntop(AF_INET, &client_addr.sin_addr, buffer_addr, sizeof(buffer_addr)) << " | " << client_addr.sin_port << std::endl;
				}
				struct epoll_event client_event;

				client_event.events = EPOLLIN; //set client available for read operations
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
			}
			else {
				client = events[i].data.fd;
				bytes = read(client, buffer, sizeof(buffer));
				if (bytes > 0) {
					std::cout << "Successfully read from data" << std::endl;
					std::string request(buffer, bytes);
					std::string response = handle_request(request);
					write(client, response.c_str(), response.size());
				}
				else {
					std::cout << "Error: Reading data from client" << std::endl;
					/* Removing client socket from epoll instance */
					if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client, nullptr) < 0) {
						std::cout << "Error removing client socket from epoll instance" << std::endl;
					}
				}
				close(client);
			}
			close (events[i].data.fd);
		}
	}
	close(server);
	return 0;
}
