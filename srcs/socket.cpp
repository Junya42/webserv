#include "../includes/socket.hpp"
#include "../includes/error.hpp"
#include <arpa/inet.h>
#include <cstdlib>

const int PORT = 8080;
const int MAX_EVENTS = 10;

int init_server_socket(int port) {
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
	server_addr.sin_port = htons(port);

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

int add_client(int server, int epoll_fd, std::vector<Client> &clientlist, int *id, int *numclient, int *curr_fd, std::string &hostname, std::string &port) {
	Client clientinfo;
	char	ip[INET_ADDRSTRLEN];
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
	clientinfo._sport = port;
	erase(clientinfo._sport, " ");

	inet_ntop(AF_INET, &clientinfo.addr.sin_addr, ip, sizeof(ip));
	clientinfo._ip = to_string(ip);
	clientinfo._host = hostname;
	erase(clientinfo._host, " ");
	PRINT_WIN("ip: " + clientinfo._ip);
	PRINT_WIN("port: " + clientinfo._sport);
	clientlist.push_back(clientinfo);
	*curr_fd = *curr_fd + 1;
	*numclient = *numclient + 1;
	*id = *id + 1;
	PRINT_FUNC();
	return 0;
}

int	remove_client(int client, std::vector<Client>& clientlist, int i, int *curr_fd, int *numclient, int epoll_fd) {
	//PRINT_FUNC();
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client, 0) < 0) {
		PRINT_ERR("Couldn't remove client socket from epoll instance");
		//return 0;
	}
	if (*curr_fd > 1 && *numclient > 0) {
		*curr_fd = *curr_fd - 1;
		*numclient = *numclient - 1;
	}
	if (clientlist[i]._name.size() < 1 || clientlist[i]._cookie.size() < 1) {
		clientlist.erase(clientlist.begin() + i);
	}
	close(client);
	return 1;
}

int	init_epoll(std::vector<int> &server) {
	int epoll_fd;
	struct epoll_event event;

	epoll_fd = epoll_create(MAX_EVENTS);
	if (epoll_fd == -1) {
		PRINT_ERR("Can't create Epoll instance");
		std::cout << std::strerror(errno) << std::endl;
		for (size_t i = 0; i < server.size(); i++) {
			close(server[i]);
		}
		exit(errno);
	}
	event.events = EPOLLIN;
	size_t check = 0;
	for (size_t i = 0; i < server.size(); i++) {
		event.data.fd = server[i];
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server[i], &event) == -1) {
			std::cout << "Error : EPOLL_CTL" << std::endl;
			std::cout << std::strerror(errno) << std::endl;
			close(epoll_fd);
			close(server[i]);
			exit(errno);
		}
		else
			check++;
	}
	if (check == 0) {
		close(epoll_fd);
		for (size_t i = 0; i < server.size(); i++)
			close(server[i]);
		PRINT_ERR("Could't add any server socket to epoll instance");
		exit(0);
	}
	return epoll_fd;
}

void	answer_client(Client &client, Request &req, Config &config, char **env) {
	if (req.in_response == false) {
		PRINT_FUNC();
	}
	req.in_response = true;

	//PRINT_FUNC();
	(void)env;
	if (comp(req.path, "download") == false && comp(req.path, "delete") == false) {
		if (req.using_cgi == true)
			req.get_cgi(client, config);
		else
			req.get_request(config._serv, client);
	}
	else
		req.download_delete_cgi(client, config._serv[2], "/home/junya/serv/www/cgi-bin/download.py", env);
	if (req.complete_file == true) {
		//if (req.cookie.size() > 1 && req.cookie[0] == 'e') {
		if (req.header_code != 0) {
			PRINT_ERR("Sending error to client");
			//std::string code(req.cookie, 6);
			//req.header_code = atoi(code.c_str());
			send_error(client._sock, req.header_code, client);
			PRINT_ERR("After sending error to client");
			client._log = false;
			PRINT_ERR("Sent error page to client " + to_string(client._sock) + " " + client._name);
		}
		else if (req.header_code == 0) {
			req.get_response(config._mime, client);
			std::cout << req << std::endl;
			std::cout << "ANSWER" << std::endl << req.answer << std::endl;
			write(client._sock, req.answer.c_str(), req.answer.size());
			PRINT_WIN("Successfully sent response to client " + to_string(client._id) + " " + client._name);
		}
		//else {
		//	redirect_error(client._sock, req.header_code);
		//}
		close(client._sock);
		req.clear();
		//std::cout << client << std::endl;
	}
	}

	int find_client_in_vector(std::vector<Client> &clientlist, int client, int index) {
		//PRINT_LOG("Current client socked: " + to_string(client));
		for (size_t i = 0; i < clientlist.size(); i++) {
			if (clientlist[i]._sock == client) {
				//PRINT_WIN("Found client in vector");
				//std::cout << "\033[1;32m" << clientlist[i] << "\033[0m" << std::endl;
				return i;
			}
		}
		//PRINT_ERR("Couldn't find client in vector");
		//std::cout << "\033[1;31m" << clientlist[index] << "\033[0m" << std::endl;
		return index;
	}

	void	reorganize_client_list(std::vector<Client> &clientlist, size_t index, int *curr_fd, int *numclient, int epoll) {
		PRINT_FUNC();
		std::string tmp_name;
		if (clientlist[index]._name.size() > 1) {
			tmp_name = clientlist[index]._name;
			for (size_t i = 0; i < clientlist.size(); i++) {
				if (i != index){
					if (comp(clientlist[i]._name, clientlist[index]._name) == true){
						//clientlist[index]._log = true;
						std::cout << "\033[1;45m////////////////////////////////\033[0m" << std::endl;
						std::cout << "new client: " << std::endl << clientlist[index] << std::endl;
						std::cout << "removed client: " << std::endl << clientlist[i] << std::endl;
						std::cout << "\033[1;45m////////////////////////////////\033[0m" << std::endl;
						clientlist[index]._name = clientlist[i]._name;
						clientlist[index]._lastname = clientlist[i]._lastname;
						clientlist[index]._request_count += clientlist[i]._request_count;
						clientlist[index]._files = clientlist[i]._files;
						clientlist[index]._cookie = clientlist[i]._cookie;
						PRINT_ERR("Trying to kick " + clientlist[i]._name + " from clientlist");
						clientlist.erase(clientlist.begin() + i);
						PRINT_ERR("Successfully kicked " + clientlist[i]._name + " from clientlist");
						break ;
					}
				}
			}
		}
		size_t i = 0;
		while (i < clientlist.size()) {
			if (clientlist[i]._sock < 1)
				clientlist[i]._log = false;

			if (clientlist[i]._cookie.empty() || clientlist[i]._name.empty()) {
				PRINT_ERR("Removing client from vector");
				if (*curr_fd > 1 && *numclient > 0) {
					*curr_fd = *curr_fd - 1;
					*numclient = *numclient - 1;
				}
				clientlist.erase(clientlist.begin() + i);
			}
			std::cout << std::endl;
			i++;
		}
		(void)numclient;
		(void)epoll;
		(void)curr_fd;
	}

