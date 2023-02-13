#include "../includes/socket.hpp"
#include "../includes/error.hpp"
#include <arpa/inet.h>
#include <cstdlib>
#include <fcntl.h>

const int MAX_EVENTS = 100;
extern std::map<int, int> csocks;

int init_server_socket(int port, const char *ip) {
	int server;
	int my_bool = 1;
	struct sockaddr_in server_addr;

	server = socket(AF_INET, SOCK_STREAM, 0);
	if (server == -1) {
		std::cout << "Error : SOCKET" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		return -1;
	}
	if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &my_bool, sizeof(my_bool)) == -1) {

		std::cout << "Error : SETSOCKOPT" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		close(server);
		return -1;
	}
	if (setsockopt(server, SOL_SOCKET, SO_REUSEPORT, &my_bool, sizeof(my_bool)) == -1) {

		std::cout << "Error : SETSOCKOPT" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		close(server);
		return -1;
	}
	std::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);

	if (bind(server, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)) == -1) {
		std::cout << "Error : BIND" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		close(server);
		return -1;
	}

	if (listen(server, 5) == -1) {
		std::cout << "Error : LISTEN" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		close (server);
		return -1;
	}
	return server;
}

int add_client(int server, int epoll_fd, std::vector<Client> &clientlist, uint32_t *id, int *numclient, int *curr_fd, std::string &hostname, std::string &port) {
	Client clientinfo;
	//char	ip[INET_ADDRSTRLEN];
	struct epoll_event client_event;
	int	client;


	clientinfo.reset();
	client = accept(server, reinterpret_cast<sockaddr*>(&clientinfo.addr), &clientinfo.addr_len);
	if (client == -1) {
		return -1;
	}
	fcntl(client, F_SETFL, O_NONBLOCK);
	std::memset(&client_event, 0, sizeof(client_event));
	client_event.events = EPOLLIN | EPOLLOUT; 
	client_event.data.fd = client;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client, &client_event) < 0) {
		close(client);
		return -1;
	}
	csocks[client] = client;
	(void)clientlist;
	(void)id;
	(void)port;
	(void)hostname;
	//clientinfo._port = clientinfo.addr.sin_port;

	/*********************************************/
	/*struct	sockaddr_in my_addr;
	unsigned int host_port;
	char myIP[16];

	bzero(&my_addr, sizeof(my_addr));
	socklen_t	len = sizeof(my_addr);

	getsockname(client, (struct sockaddr *)&my_addr, &len);
	inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));

	host_port = ntohs(my_addr.sin_port);
	*/
	/*********************************************/

	/*
	clientinfo._id = *id;
	clientinfo._sock = client;
	clientinfo._sport = port;
	erase(clientinfo._sport, " ");

	clientinfo._hostip = to_string(myIP);
	clientinfo._hostport = host_port;
	clientinfo._hostsport = to_string(host_port);
	inet_ntop(AF_INET, &clientinfo.addr.sin_addr, ip, sizeof(ip));
	clientinfo._ip = to_string(ip);
	clientinfo._host = hostname;
	erase(clientinfo._host, " ");
	clientlist.push_back(clientinfo);
	*/
	*curr_fd = *curr_fd + 1;
	*numclient = *numclient + 1;
	*id = *id + 1;
	return 0;
}

int	remove_client(int client, std::vector<Client>& clientlist, int i, int *curr_fd, int *numclient, int epoll_fd) {

	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client, 0);
	if (*curr_fd > 1 && *numclient > 0) {
		*curr_fd = *curr_fd - 1;
		*numclient = *numclient - 1;
	}
	if ((clientlist[i]._name.size() < 1 || clientlist[i]._cookie.size() < 1) && clientlist.size()) {
		clientlist.erase(clientlist.begin() + i);
	}
	csocks.erase(client);
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
		return -1;
	}
	std::memset(&event, 0, sizeof(event));
	event.events = EPOLLIN;
	size_t check = 0;
	for (size_t i = 0; i < server.size(); i++) {
		event.data.fd = server[i];
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server[i], &event) == -1) {
			std::cout << "Error : EPOLL_CTL" << std::endl;
			std::cout << std::strerror(errno) << std::endl;
			return -1;
		}
		else
			check++;
	}
	if (check == 0) {
		PRINT_ERR("Could't add any server socket to epoll instance");
		return -1;
	}
	return epoll_fd;
}

size_t	get_serv_from_client(Client &client, std::vector<Server> &serv) {
	for (size_t i = 0; i < serv.size(); i++) {
		if (client.request.host == serv[i]._host) {
			serv[i]._requests++;
			return i;
		}
	}
	return 0;
}

std::string	error_path(Server &serv, int code) {
	std::string	path;

	try {
		std::string tmp;
		tmp = serv.errors.at(code);
		path = tmp;
	}
	catch (std::exception &e) {
	}
	return path;
}

void	delete_file(Server &serv, Client &client) {
	PRINT_LOG(client.request.path_info);
	size_t j = 0;
	for (j = 0; j < serv._loc.size(); j++) {
      if (comp(client.request.path_info, serv._loc[j]._path) == true) {
		break;
	  }
	}
	if (j >= serv._loc.size()) {
		client.request.set_error(404);
		client.request.complete_file = true;
		return ;
	}
	if (serv._loc[j].method["DELETE"] == false) {
		client.request.set_error(405);
		client.request.complete_file = true;
		return ;
	}
	if (serv._login) {
		if (client._name.empty() || client._log == false) {
			client.request.set_error(401);
			client.request.complete_file = true;
			return ;
		}
		else {
			size_t skip = find(client.request.path_info, '/', 4);
			if (skip < client.request.path_info.size() - 1 && client.request.path_info[skip] == '/') {
				skip++;
				std::string folder_owner = client.request.path_info.substr(skip);
				folder_owner = folder_owner.substr(0, folder_owner.find('/'));
				if (client._name != folder_owner) {
					client.request.set_error(403);
					client.request.complete_file = true;
					return ;
				}
			}
			else {
				client.request.set_error(400);
				client.request.complete_file = true;
				return ;
			}
		}
	}
	if (client.request.path_info.size()) {
		struct stat st;

    	if (stat(client.request.path_info.c_str(), &st) == 0) {
        	if (S_ISREG(st.st_mode) != 0) {
				int del = remove(client.request.path_info.c_str());
				if (del == 0) {
					client.request.file_content = "<html><head><title>Webserv</title></head>\n";
					client.request.file_content += "<body><h1>The requested file has been deleted</h1></body></html>\n";
				}
				else {
					client.request.set_error(400);
				}
        	}
        	else {
				client.request.set_error(400);
			}
      	}
      	else {
			client.request.set_error(404);
	  	}
    }
	client.request.complete_file = true;
}

void	answer_client(Client &client, Request &req, Config &config, int epoll_fd) {

	if (client._index == -1)
		client._index = get_serv_from_client(client, config._serv);
	if (req.complete_file == false) {
		req.in_response = true;

		if (req.using_cgi == true)
			req.get_cgi(client, config);
		else if (comp(req.method, "delete") == false)
			req.get_request(config._serv, client, client._index);
		else
			delete_file(config._serv[client._index], client);
	}
	if (req.complete_file == true && client._ready == true) {
		std::cout << std::endl;
		if (req.header_code != 0) {
			send_error(client._sock, req.header_code, client, error_path(config._serv[client._index], req.header_code));
			PRINT_ERR("Sending error");
			client._log = false;
		}
		else if (req.header_code == 0) {
			if (req.using_cgi == false)
				req.get_response(config._mime, client);
			if (req.answer.size() > 100000 && config._serv[client._index]._large == true) {
				int val = fcntl(client._sock, F_GETFL, 0);
				int flags = O_NONBLOCK;
				val &= ~flags;
				fcntl(client._sock, F_SETFL, val);
			}
			write(client._sock, req.answer.c_str(), req.answer.size());
			PRINT_LOG("Sending answer");
		}
		std::cout << std::endl;
		client._path = req.path_info;
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client._sock, NULL);
		csocks.erase(client._sock);
		close(client._sock);
		req.clear();
	}
}

void set_ready_client(std::vector<Client> &clientlist, int client, int index) {
	struct	sockaddr_in my_addr;
	unsigned int host_port;
	char myIP[16];

	bzero(&my_addr, sizeof(my_addr));
	socklen_t	len = sizeof(my_addr);

	getsockname(client, (struct sockaddr *)&my_addr, &len);
	inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));

	host_port = ntohs(my_addr.sin_port);

	int	x = -1;
	for (size_t i = 0; i < clientlist.size(); i++) {
		if (clientlist[i]._sock == client && clientlist[i]._hostport == host_port && clientlist[i]._hostip == to_string(myIP)) {
			clientlist[i]._ready = true;
			return ;
		}
		else if (clientlist[i]._sock == client) {
			if (x == -1)
				x = i;
		}
	}
	if (x != -1) {
		clientlist[x]._ready = true;
		return ;
	}
	size_t sdex = index;
	if (sdex < clientlist.size()) {
		clientlist[sdex]._ready = true;
	}
}

int find_client_in_vector(std::vector<Client> &clientlist, int client, int index) {
	struct	sockaddr_in my_addr;
	unsigned int host_port;
	char myIP[16];

	bzero(&my_addr, sizeof(my_addr));
	socklen_t	len = sizeof(my_addr);

	getsockname(client, (struct sockaddr *)&my_addr, &len);
	inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));

	host_port = ntohs(my_addr.sin_port);

	(void)index;
	for (size_t i = 0; i < clientlist.size(); i++) {
		if (clientlist[i]._sock == client && clientlist[i]._hostport == host_port && clientlist[i]._hostip == to_string(myIP)) {
			return i;
		}
	}
	Client	newclient;

	newclient._sock = client;
	newclient._hostport = host_port;
	newclient._hostip = to_string(myIP);
	clientlist.push_back(newclient);
	return (clientlist.size() - 1);
}

void	reorganize_client_list(std::vector<Client> &clientlist, size_t index, int *curr_fd, int *numclient, int epoll) {
	std::string tmp_name;
	if (index < clientlist.size() && clientlist[index]._name.size() > 1) {
		tmp_name = clientlist[index]._name;
		for (size_t i = 0; i < clientlist.size(); i++) {
			if (i != index){
				if (comp(clientlist[i]._name, clientlist[index]._name) == true && comp(clientlist[i]._host, clientlist[index]._host) == true){
					clientlist[index]._name = clientlist[i]._name;
					clientlist[index]._lastname = clientlist[i]._lastname;
					clientlist[index]._request_count += clientlist[i]._request_count;
					clientlist[index]._files = clientlist[i]._files;
					clientlist[index]._cookie = clientlist[i]._cookie;
					clientlist.erase(clientlist.begin() + i);
					break ;
				}
			}
		}
	}

	size_t i = 0;

	while (i < clientlist.size()) {
		if (clientlist[i]._sock < 1)
			clientlist[i]._log = false;

		if (clientlist[i]._request_count != 0 && (clientlist[i].request.in_use == false && clientlist[i].request.in_response == false) && (clientlist[i]._cookie.empty() || clientlist[i]._name.empty())) {
			if (*curr_fd > 1 && *numclient > 0) {
				*curr_fd = *curr_fd - 1;
				*numclient = *numclient - 1;
			}
			clientlist.erase(clientlist.begin() + i);
		}
		else {
			i++;
		}
	}
	(void)numclient;
	(void)epoll;
	(void)curr_fd;
}

