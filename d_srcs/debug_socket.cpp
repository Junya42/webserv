#include "../includes/socket.hpp"
#include "../includes/error.hpp"
#include <arpa/inet.h>
#include <cstdlib>

const int MAX_EVENTS = 100;
extern std::map<int, int> csocks;

int init_server_socket(int port, const char *ip) {
	int server;
	int my_bool = 1;
	struct sockaddr_in server_addr;

	PRINT_LOG(to_string(ip) + " : " + to_string(port));
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
	char	ip[INET_ADDRSTRLEN];
	struct epoll_event client_event;
	int	client;


	PRINT_LOG("Server socket: " + to_string(server));
	clientinfo.reset();
	client = accept(server, reinterpret_cast<sockaddr*>(&clientinfo.addr), &clientinfo.addr_len);
	if (client == -1) {
		std::cout << "Error : ACCEPT" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		return -1;
	}
	fcntl(client, F_SETFL, O_NONBLOCK);
	std::memset(&client_event, 0, sizeof(client_event));
	client_event.events = EPOLLIN | EPOLLOUT; 
	client_event.data.fd = client;
	//client_event.data.u32 = *id;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client, &client_event) < 0) {
		std::cout << "Error: EPOLL_CTL Client" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		close(client);
		return -1;
	}
	csocks[client] = client;
	clientinfo._port = clientinfo.addr.sin_port;

	/*********************************************/
	struct	sockaddr_in my_addr;
	unsigned int host_port;
	char myIP[16];

	bzero(&my_addr, sizeof(my_addr));
	socklen_t	len = sizeof(my_addr);

	getsockname(client, (struct sockaddr *)&my_addr, &len);
	inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));

	host_port = ntohs(my_addr.sin_port);
	/*********************************************/

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
	PRINT_WIN("host: " + clientinfo._host);
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

	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client, 0) < 0) {
		PRINT_ERR("Couldn't remove client socket from epoll instance");
	}
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
		std::string tmpname = serv[i]._name;
		std::string	tmpport = serv[i]._sport;
		erase(tmpname, " ");
		erase(tmpport, " ");
		if (client._host == tmpname && client._hostsport == tmpport) {
			PRINT_WIN(tmpname + ":" + client._hostsport);
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
		PRINT_ERR("No default error pages for error " + to_string(code));
	}
	return path;
}

void	delete_file(Server &serv, Client &client) {
	(void)serv;
	PRINT_FUNC();
	if (client.request.path_info.size()) {
		struct stat st;

    	if (stat(client.request.path_info.c_str(), &st) == 0) {
        	if (S_ISREG(st.st_mode) != 0) {
				int del = remove(client.request.path_info.c_str());
				if (!del) {
					PRINT_WIN("Deleting file");
					client.request.file_content = "<html><head><title>Webserv</title></head><body><h1>The requested file has been deleted</h1></body></html>";
				}
				else {
					PRINT_ERR("Couldnt delete file");
					client.request.set_error(400);
				}
        	}
        	else {
				client.request.set_error(400);
        		PRINT_ERR("The requested file is a directory");
			}
      	}
      	else {
			client.request.set_error(404);
        	PRINT_ERR("The requested file does not exist");
	  	}
    }
	client.request.complete_file = true;
}

void	answer_client(Client &client, Request &req, Config &config, int epoll_fd) {

	if (client._index == -1)
		client._index = get_serv_from_client(client, config._serv);
	if (req.complete_file == false) {
		if (req.in_response == false)
			PRINT_FUNC();
		req.in_response = true;

		PRINT_WIN("Using cgi = " + to_string(req.using_cgi));
		if (req.using_cgi == true)
			req.get_cgi(client, config);
		else if (comp(req.method, "delete") == false)
			req.get_request(config._serv, client, client._index);
		else
			delete_file(config._serv[client._index], client);
	}
	if (req.complete_file == true && client._ready == true) {
		if (req.header_code != 0) {
			send_error(client._sock, req.header_code, client, error_path(config._serv[client._index], req.header_code));
			client._log = false;
			PRINT_ERR("Sent error page to client " + to_string(client._sock) + " " + client._name);
		}
		else if (req.header_code == 0) {
			if (req.using_cgi == false)
				req.get_response(config._mime, client);
			//std::cout << req << std::endl;
			//std::cout << "ANSWER" << std::endl << req.answer << std::endl;
			write(client._sock, req.answer.c_str(), req.answer.size());
			PRINT_WIN("Successfully sent response to client " + to_string(client._id) + " " + client._name);
		}
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client._sock, NULL);
		csocks.erase(client._sock);
		close(client._sock);
		req.clear();
	}
}

void set_ready_client(std::vector<Client> &clientlist, int client, int index) {
	PRINT_LOG("Current client socket: " + to_string(client));

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
			PRINT_WIN("Perfect match: " + clientlist[i]._hostip + ":" + to_string(clientlist[i]._hostport));
			clientlist[i]._ready = true;
			return ;
		}
		else if (clientlist[i]._sock == client) {
			if (x == -1)
				x = i;
		}
	}
	if (x != -1) {
		PRINT_WIN("Only sockets matches: " + clientlist[x]._hostip + ":" + to_string(clientlist[x]._hostport));
		clientlist[x]._ready = true;
		return ;
	}
	size_t sdex = index;
	if (sdex < clientlist.size()) {
		PRINT_WIN("Index: " + clientlist[sdex]._hostip + ":" + to_string(clientlist[sdex]._hostport));
		clientlist[sdex]._ready = true;
	}
	else {
		PRINT_ERR("Index: " + to_string(index));
	}
}

int find_client_in_vector(std::vector<Client> &clientlist, int client, int index) {
	PRINT_LOG("Current client socket: " + to_string(client));

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
			PRINT_WIN("Perfect match: " + clientlist[i]._hostip + ":" + to_string(clientlist[i]._hostport));
			return i;
		}
		else if (clientlist[i]._sock == client) {
			if (x == -1)
				x = i;
		}
	}
	if (x != -1) {
		PRINT_WIN("Only sockets matches: " + clientlist[x]._hostip + ":" + to_string(clientlist[x]._hostport));
		return x;
	}
	size_t sdex = index;
	if (sdex < clientlist.size()) {
		PRINT_WIN("Index: " + clientlist[sdex]._hostip + ":" + to_string(clientlist[sdex]._hostport));
	}
	else {
		std::cout << "\033[1;41mBIIIIG problem\033[0m" << std::endl;
		PRINT_ERR("Current socket(" + to_string(client) + ") host = " + to_string(myIP) + ":" + to_string(host_port));
		PRINT_ERR("Clientlist size: " + to_string(clientlist.size()));
		for (size_t i = 0; i < clientlist.size(); i++) {
			PRINT_ERR("Clientlist[" + to_string(i) + "].socket :" + to_string(clientlist[i]._sock));
		}
		PRINT_ERR("Index: " + to_string(index));
	}
	return index;
}

void	reorganize_client_list(std::vector<Client> &clientlist, size_t index, int *curr_fd, int *numclient, int epoll) {
	PRINT_FUNC();
	std::string tmp_name;
	if (clientlist[index]._name.size() > 1) {
		tmp_name = clientlist[index]._name;
		for (size_t i = 0; i < clientlist.size(); i++) {
			if (i != index){
				if (comp(clientlist[i]._name, clientlist[index]._name) == true && comp(clientlist[i]._host, clientlist[index]._host) == true){
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

		if (clientlist[i]._request_count != 0 && (clientlist[i].request.in_use == false && clientlist[i].request.in_response == false) && (clientlist[i]._cookie.empty() || clientlist[i]._name.empty())) {
			//std::cerr << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl << clientlist[i] << std::endl << "~~~~~~~~~~~~~~~~~~~~" << std::endl;
			PRINT_ERR("Removing client from vector");
			if (*curr_fd > 1 && *numclient > 0) {
				*curr_fd = *curr_fd - 1;
				*numclient = *numclient - 1;
			}
			clientlist.erase(clientlist.begin() + i);
		}
		else {
			i++;
		}
		std::cout << std::endl;
	}
	(void)numclient;
	(void)epoll;
	(void)curr_fd;
}
