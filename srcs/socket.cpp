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

	clientinfo.reset();
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
	std::vector<Client> clientlist;

	(void)config;
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
				std::cout << "ADD CLIENT" << std::endl;
				add_client(server, epoll_fd, clientlist, i);
				numclient++;
				curr_fd++;
			}
			else {
				client = events[i].data.fd;
				std::cout << "HANDLE CLIENT" << std::endl;
				if (clientlist[i].request.read_client(client) == 1) {
					std::cout << "SUCCESS CLIENT REQUEST" << std::endl;
					std::cout << clientlist[i].request << std::endl;//answer client
				}
				else {
					std::cout << "FAILURE CLIENT REQUEST" << std::endl;
					std::cout << clientlist[i].request << std::endl;//answer client
				}
				sleep(1);
				if (clientlist[i].request.current_bytes > 0) {
					std::cout << "Successfully read " << clientlist[i].request.current_bytes << " bytes from data" << std::endl;
				}
				else {
					remove_client(client, clientlist, i, &curr_fd, &numclient, events[i].data.fd, epoll_fd);
				}
			}
		}
	}
	close(server);
}
