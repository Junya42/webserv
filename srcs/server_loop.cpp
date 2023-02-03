#include "../includes/socket.hpp"

//const int PORT = 8080;
const int MAX_EVENTS = 100;


std::vector<int> create_servers(std::vector<Server> &servconf) {
	std::vector<int> server;

	for (size_t i  = 0; i < servconf.size(); i++) {
		server.push_back(init_server_socket(servconf[i]._port));
		servconf[i]._sock = server[i];
	}
	return server;
}

int	check_server_event(int fd, std::vector<int> &server) {
	for (size_t i = 0; i < server.size(); i++) {
		if (fd == server[i])
			return i;
	}
	return -1;
}

std::string &hostname(int server, Config &config) {
	for (std::vector<Server>::iterator it = config._serv.begin(); it != config._serv.end(); it++) {
		if (server == it->_sock)
			return it->_name;
	}
	return config._serv[0]._name;
}

std::string &port(int server, Config &config) {
	for (std::vector<Server>::iterator it = config._serv.begin(); it != config._serv.end(); it++) {
		if (server == it->_sock)
			return it->_sport;
	}
	return config._serv[0]._sport;
}

void	server_handler(Config &config, char **env) {

	std::vector<int> server;
	int	index;
	int	client;
	int	epoll_fd;
	int status;
	size_t num_events;
	Client tmp;
	std::vector<Client> clientlist;

	server = create_servers(config._serv);
	epoll_fd = init_epoll(server);

	struct epoll_event events[MAX_EVENTS];

	int curr_fd = 1;
	int numclient = 0;
	int	id = 1;
	int save_index;
	tmp.clear();
	while (1) {
		num_events = epoll_wait(epoll_fd, events, curr_fd, 100);
		for (size_t i = 0; i < num_events; i++) {
			index = check_server_event(events[i].data.fd, server);
			if (index != -1 && events[i].data.fd == server[index]) {
				PRINT_WIN("Server event"); 
				add_client(server[index], epoll_fd, clientlist, &id, &numclient, &curr_fd, hostname(server[index], config), port(server[index], config));
			}
			else if (events[i].events & EPOLLIN) {
				client = events[i].data.fd;
				save_index = i;
				i = find_client_in_vector(clientlist, client, i);
				status = clientlist[i].request.read_client(client, clientlist[i], tmp);
				if (tmp._name.size()) {
					clientlist.push_back(tmp);
					tmp.clear();
				}
				if (status == 1) {
					PRINT_LOG("Status: 1");
					answer_client(clientlist[i], clientlist[i].request, config, env);
				}
				else if (status == 0 && clientlist[i].request.in_use == false) {
					PRINT_ERR("Client timed out or disconnected");
					remove_client(client, clientlist, i, &curr_fd, &numclient, epoll_fd);
				}
				else if (status == -1) {
					PRINT_ERR("Error syscall read / write");
					remove_client(client, clientlist, i, &curr_fd, &numclient, epoll_fd);
				}
				reorganize_client_list(clientlist, i, &curr_fd, &numclient, epoll_fd);
				clientlist[i]._request_count++;
				for (size_t x = 0; x < clientlist.size(); x++) {
					std::cout << clientlist[x] << std::endl << "path: " << clientlist[x]._path << std::endl;
					if (x == i)
						std::cout << "\033[1;34m##############################\033[0m" << std::endl;
					else
						std::cout << "##############################" << std::endl;
				}
				i = save_index;
				//std::cout << std::endl;
			}
			else if (events[i].events & EPOLLOUT){
				PRINT_WIN("EPOLLOUT Client event");
			}
		}
		for (size_t j = 0; j < clientlist.size(); j++)
		{
			status = 0;
			if (clientlist[j].request.in_use == true) {
				//PRINT_LOG("Outside read : " + clientlist[j]._name);
				status = clientlist[j].request.read_client(clientlist[j]._sock, clientlist[j], tmp);
				if (tmp._name.size()) {
					clientlist.push_back(tmp);
					tmp.clear();
				}
			}
			if (clientlist[j].request.in_response == true || status == 1) {
				//PRINT_LOG("Outside answer");
				answer_client(clientlist[j], clientlist[j].request, config, env);
			}
		}
	}
	//close(server);
}
