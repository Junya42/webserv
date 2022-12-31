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

/*	Create a server socket
 *	Set the socket option SO_REUSEADDR
 *	Add informations to socket
 *	Bind the socket
 *	Start listening
 */
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
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);

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

/* Create a class Client instance
 * Create a client socket
 * Make the socket non blocking using fcntl
 * Set the client event to EPOLLIN (Read)
 * Add the client to the Epoll instance
 * Add the Client instance to my clientlist
 */
int add_client(int server, int epoll_fd, std::vector<Client> &clientlist, int *id, int *numclient, int *curr_fd) {
	Client clientinfo;
	struct epoll_event client_event;
	int	client;

	clientinfo.reset();
	client = accept(server, reinterpret_cast<sockaddr*>(&clientinfo.addr), &clientinfo.addr_len);
	if (client == -1) {
		std::cout << "Error : ACCEPT" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		close(server);
		exit(errno);
	}
	fcntl(client, F_SETFL, O_NONBLOCK);
	client_event.events = EPOLLIN; 
	client_event.data.fd = client;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client, &client_event) < 0) {
		std::cout << "Error: EPOLL_CTL Client" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		close(client);
		close(server);
		exit(errno);
	}
	clientinfo._port = clientinfo.addr.sin_port;
	clientinfo._id = *id;
	clientinfo._sock = client;
	clientlist.push_back(clientinfo);
	*curr_fd = *curr_fd + 1;
	*numclient = *numclient + 1;
	*id = *id + 1;
	PRINT_FUNC();
	return 0;
}

/*	Remove the client from the epoll instance
 *	Update current number of sockets (servers + clients)
 *	Update current number of clients
 *	remove class Client form clientlist
 *	close client socket
 */
int	remove_client(int client, std::vector<Client>& clientlist, int i, int *curr_fd, int *numclient, int epoll_fd) {
	PRINT_FUNC();
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client, nullptr) < 0) {
		PRINT_ERR("Couldn't remove client socket from epoll instance");
		return -1;
	}
	*curr_fd = *curr_fd - 1;
	*numclient = *numclient - 1;
	clientlist.erase(clientlist.begin() + i);
	close(client);
	PRINT_LOG("Updating number of clients");
	PRINT_LOG(*numclient);
	return 0;
}

/*	Create epoll instance
 *	Add server socket to epoll instance
 */
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

void	answer_client(int client, Request &req) {
	std::string answer;

	answer = "HTTP/1.1 200 OK\n";
	answer += "Content-Type: text/html\n";
	answer += "Content-Length: ";

	FILE *fd = NULL;
	int pos = 0;
	size_t size = 0;
	fd = fopen("/home/junya/serv/default/button.html", "rb");
	if (!fd) {
		PRINT_ERR("Invalid file descriptor");
		return ;
	}
	pos = ftell(fd);
	fseek(fd, 0, SEEK_END);
	size = ftell(fd);
	fseek(fd, pos, SEEK_SET);

	size_t bytes = 0;
	char *buf = (char *)malloc(sizeof(char) * size);
	if (!buf) {
		PRINT_ERR("Couldn't allocate memory for read buffer");
		fclose(fd);
		return ;
	}
	bytes = fread(buf, 1, size, fd);
	if (bytes < 1) {
		PRINT_ERR("Couldn't read from file");
		free(buf);
		fclose(fd);
		return ;
	}
	std::stringstream out;

	for (size_t i = 0; i < size; i++)
		out << buf[i];
	std::string cpy = out.str();
	free(buf);
	fclose(fd);
	std::ostringstream s;
	s << size;

	answer += s.str();
	answer += "\r\n\r\n";
	answer += cpy;

	write(client, answer.c_str(), answer.size());
	req.clear();
	return ;
}

void	server_handler(Config &config) {

	int server; //server socket fd
	int	client; //client socket fd
	int	epoll_fd; //epoll instance 
	int num_events; //number of events occuring in epoll wait
	int status; //request status
	std::vector<Client> clientlist;

	(void)config;
	server = init_server_socket();
	epoll_fd = init_epoll(server);

	struct epoll_event events[MAX_EVENTS];

	int curr_fd = 1; // Number of actual clients fd + server fd;
	int numclient = 0; //Number of clients;
	int	id = 1; //unique client id;
	while (1) {
		num_events = epoll_wait(epoll_fd, events, curr_fd, -1);
		/*if (num_events < 0) {
			PRINT_ERR("Exiting server");
			for (int i = 0; i < curr_fd; i++)
				remove_client(events[i].data.fd, clientlist, i, &curr_fd, &numclient, epoll_fd);
			close(epoll_fd);
			exit(0);
		}*/
		for (int i = 0; i < num_events; i++) {
			if (events[i].data.fd == server) {
				PRINT_WIN("Server event"); 
				add_client(server, epoll_fd, clientlist, &id, &numclient, &curr_fd);
			}
			else if (events[i].events & EPOLLIN) {
				PRINT_WIN("EPOLLIN Client event");
				client = events[i].data.fd;
				status = clientlist[i].request.read_client(client); //read and parse client request data

				/* status 1 for success, no error during read and parsing
				 * status 0 for empty read since sockets are non blocking
				 * status -1 for read errors
				 */
				if (status == 1) {
					PRINT_LOG("Status: 1");
					std::cout << clientlist[i].request << std::endl; //priting the request data
					answer_client(client, clientlist[i].request);
					/*
					std::string res = std::string("HTTP/1.1 200 OK\n\
							Content-Length: 92\n\
							Content-Type: text/html\n\
							Connection: keep-alive\n\
							\r\n\r\n\
							<html>\n\
							<body>\n\
							<h1>Hello, World!</h1>\n\
							</body>\n\
							</html>");
							
					if (write(client, res.c_str(), res.size()) > 0) {//sending default answer to client
						PRINT_LOG("Writing to client");
						clientlist[i].request.clear();
					}*/
				}
				else if (status == 0 && clientlist[i].request.in_use == false) {
					PRINT_ERR("Client timed out or disconnected");
					remove_client(client, clientlist, i, &curr_fd, &numclient, epoll_fd);
				}
				else if (status == -1) {
					PRINT_ERR("Error syscall read / write");
					remove_client(client, clientlist, i, &curr_fd, &numclient, epoll_fd);
				}
				sleep(1);
			}
			else if (events[i].events & EPOLLOUT){
				PRINT_WIN("EPOLLOUT Client event");
			}
		}
	}
	close(server);
}
