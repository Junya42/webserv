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
#include <map>
#include <sstream>
#include <stdio.h>
#include "../includes/config.hpp"
#include "../includes/client.hpp"
#include "../includes/macro.hpp"

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
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client, 0) < 0) {
		PRINT_ERR("Couldn't remove client socket from epoll instance");
		//return -1;
	}
	*curr_fd = *curr_fd - 1;
	*numclient = *numclient - 1;
	clientlist[i]._log = false;
	if (clientlist[i]._name.size() < 1 && clientlist[i]._cookie.size() < 1) {
		PRINT_ERR("Deleting elem from clientlist");
		clientlist.erase(clientlist.begin() + i);
		PRINT_ERR(clientlist.size());
	}
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

void	answer_client(Client &client, Request &req, Config &config) {
	req.in_response = true;

	req.get_request(config._serv);
	if (req.complete_file == true) {
		req.get_response(config._mime, client);
		/*std::cout << "ANSWER" << std::endl << req.answer << std::endl;*/
		write(client._sock, req.answer.c_str(), req.answer.size());
		req.clear();
		PRINT_WIN("Successfully sent response to client");
		PRINT_WIN(client._id);
	}
}

int find_client_in_vector(std::vector<Client> &clientlist, int client, int index) {
	for (size_t i = 0; i < clientlist.size(); i++) {
		if (clientlist[i]._sock == client) {
			PRINT_WIN("Found client in vector");
			//std::cout << "\033[1;32m" << clientlist[i] << "\033[0m" << std::endl;
			return i;
		}
	}
	PRINT_ERR("Couldn't find client in vector");
	//std::cout << "\033[1;31m" << clientlist[index] << "\033[0m" << std::endl;
	return index;
}

void	reorganize_client_list(std::vector<Client> &clientlist, size_t index, int *curr_fd, int *numclient, int epoll) {
	std::string tmp_name;
	if (clientlist[index]._cookie.size() > 1) {
		tmp_name = clientlist[index]._name;
		clientlist[index]._log = true;
		PRINT_LOG(clientlist[index]._cookie);
		for (size_t i = 0; i < clientlist.size(); i++) {
			if (i != index){
				if (comp(clientlist[i]._name, clientlist[index]._name) == true){
					clientlist[index]._name = clientlist[i]._name;
					clientlist[index]._lastname = clientlist[i]._lastname;
					clientlist[index]._request_count += clientlist[i]._request_count;
					clientlist[index]._files = clientlist[i]._files;
					PRINT_ERR("Successfully kicked " + clientlist[i]._name + " from clientlist");
					clientlist.erase(clientlist.begin() + i);
					break ;
				}
			}
		}
	}
	size_t i = 0;
	while (i < clientlist.size()) {
		if (clientlist[i]._cookie.empty()) {
			remove_client(clientlist[i]._sock, clientlist, i, curr_fd, numclient, epoll);
			continue ;
		}
		if (clientlist[i]._name != tmp_name)
			clientlist[i]._log = false;
		i++;
	}
}

void	server_handler(Config &config) {

	int server; //server socket fd
	int	client; //client socket fd
	int	epoll_fd; //epoll instance 
	int status; //request status
	size_t num_events; //number of events occuring in epoll wait
	Client tmp;
	std::vector<Client> clientlist;

	server = init_server_socket();
	epoll_fd = init_epoll(server);

	struct epoll_event events[MAX_EVENTS];

	int curr_fd = 1; // Number of actual clients fd + server fd;
	int numclient = 0; //Number of clients;
	int	id = 1; //unique client id;
	int save_index;
	tmp.clear();
	while (1) {
		num_events = epoll_wait(epoll_fd, events, curr_fd, 100);
		for (size_t i = 0; i < num_events; i++) {
			if (events[i].data.fd == server) {
				PRINT_WIN("Server event"); 
				add_client(server, epoll_fd, clientlist, &id, &numclient, &curr_fd);
			}
			else if (events[i].events & EPOLLIN) {
				PRINT_WIN("EPOLLIN Client event");
				client = events[i].data.fd;
				save_index = i;
				i = find_client_in_vector(clientlist, client, i);
				status = clientlist[i].request.read_client(client, clientlist[i], tmp); //read and parse client request data
				if (tmp._name.size()) {
					clientlist.push_back(tmp);
					tmp.clear();
				}
				/* status 1 for success, no error during read and parsing
				 * status 0 for empty read since sockets are non blocking
				 * status -1 for read errors
				 */
				if (status == 1) {
					PRINT_LOG("Status: 1");
					//std::cout << clientlist[i].request << std::endl; //priting the request data
					answer_client(clientlist[i], clientlist[i].request, config);
				}
				else if (status == 0 && clientlist[i].request.in_use == false) {
					PRINT_ERR("Client timed out or disconnected");
					std::cout << "\033[1;31m" << clientlist[i] << "\033[0m" << std::endl;
					remove_client(client, clientlist, i, &curr_fd, &numclient, epoll_fd);
				}
				else if (status == -1) {
					PRINT_ERR("Error syscall read / write");
					remove_client(client, clientlist, i, &curr_fd, &numclient, epoll_fd);
				}
				reorganize_client_list(clientlist, i, &curr_fd, &numclient, epoll_fd);
				clientlist[i]._request_count++;
				/*for (size_t x = 0; x < clientlist.size(); x++) {
					std::cout << clientlist[x] << std::endl << "path: " << clientlist[x]._path << std::endl;
					if (x == i)
						std::cout << "\033[1;34m##############################\033[0m" << std::endl;
					else
						std::cout << "##############################" << std::endl;
				}*/
				i = save_index;
				std::cout << std::endl;
			}
			else if (events[i].events & EPOLLOUT){
				PRINT_WIN("EPOLLOUT Client event");
			}
		}
		for (size_t j = 0; j < clientlist.size(); j++)
		{
			status = 0;
			if (clientlist[j].request.in_use == true) {
				status = clientlist[j].request.read_client(clientlist[j]._sock, clientlist[j], tmp);
				if (tmp._name.size()) {
					clientlist.push_back(tmp);
					tmp.clear();
				}
			}
			if (clientlist[j].request.in_response == true || status == 1)
				answer_client(clientlist[j], clientlist[j].request, config);
		}
	}
	close(server);
}