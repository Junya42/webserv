#include "../includes/socket.hpp"
#include "../includes/error.hpp"

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
		//return 0;
	}
	if (*curr_fd > 1 && *numclient > 0) {
		*curr_fd = *curr_fd - 1;
		*numclient = *numclient - 1;
	}
	//clientlist[i]._log = false;
	if (clientlist[i]._name.size() < 1 || clientlist[i]._cookie.size() < 1) {
		PRINT_ERR("Deleting elem from clientlist");
		clientlist.erase(clientlist.begin() + i);
		PRINT_ERR(clientlist.size());
	}
	close(client);
	PRINT_LOG("Updating number of clients");
	PRINT_LOG(*numclient);
	return 1;
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

	PRINT_FUNC();
	req.get_request(config._serv, client);
	if (req.complete_file == true) {
		if (req.header_code == 0) {
			req.get_response(config._mime, client);
			std::cout << req << std::endl;
			//std::cout << "ANSWER" << std::endl << req.answer << std::endl;
			write(client._sock, req.answer.c_str(), req.answer.size());
			PRINT_WIN("Successfully sent response to client");
			PRINT_WIN(client._id);
		}
		else {
			send_error(client._sock, req.header_code);
			client._log = false;
			PRINT_ERR("Sent error page to client");
			PRINT_ERR(client._id);
		}
		req.clear();
	}
}

int find_client_in_vector(std::vector<Client> &clientlist, int client, int index) {
	PRINT_LOG("Current client socked: " + to_string(client));
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
			if (remove_client(clientlist[i]._sock, clientlist, i, curr_fd, numclient, epoll))
				continue ;
		}
		//if (clientlist[i]._name != tmp_name)
		i++;
	}
	PRINT_LOG("End of reorganize_client_list");
}

