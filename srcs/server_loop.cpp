#include "../includes/socket.hpp"
#include "../includes/error.hpp"
#include <exception>
#include <signal.h>

const int MAX_EVENTS = 300;
int	signalcheck=0;
std::map<int, int> csocks;

void	sighandle(int sig) {
	remove("/tmp/.webserv_process/process_checker");

	switch (sig) {
		case 2:
			signalcheck = 1;
			signal(SIGINT, SIG_IGN);
			break;
		case 6:
			signalcheck = 2;
			signal(SIGABRT, SIG_DFL);
			break;
		case 7:
			signalcheck = 2;
			signal(SIGBUS, SIG_DFL);
			break;
		case 11:
			signalcheck = 2;
			signal(SIGSEGV, SIG_DFL);
			break;
		case 15:
			signalcheck = 2;
			signal(SIGTERM, SIG_DFL);
			break;
	}
}

void	signalhandler(void) {
	signal(SIGINT, sighandle);
	signal(SIGABRT, sighandle);
	signal(SIGBUS, sighandle);
	signal(SIGSEGV, sighandle);
	signal(SIGTERM, sighandle);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
}

std::vector<int> create_servers(std::vector<Server> &servconf) {
	std::vector<int> server(servconf.size());

	for (size_t i  = 0; i < servconf.size(); i++) {
		server[i] = init_server_socket(servconf[i]._port, servconf[i]._ip.c_str());
		if (server[i] == -1) {
			for (size_t j = 0; j < i; j++) {
				close(server[j]);
			}
			server.erase(server.begin(), server.end());
			return server;		
		}
		servconf[i]._sock = server[i];
		PRINT_LOG("Server" + servconf[i]._name + " socket : " + to_string(server[i]));
	}
	std::cout << std::endl << std::endl << "********************************************************" << std::endl;
	std::cout << "________________________________________________________" << std::endl << std::endl;
	return server;
}

int	check_server_event(int fd, std::vector<Server> &server) {
	int save = -1;

	struct	sockaddr_in my_addr;
	unsigned int host_port;
	char myIP[16];

	bzero(&my_addr, sizeof(my_addr));
	socklen_t	len = sizeof(my_addr);

	getsockname(fd, (struct sockaddr *)&my_addr, &len);
	inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));

	host_port = ntohs(my_addr.sin_port);

	std::string _host = to_string(myIP);
	std::string _port = to_string(host_port);
	for (size_t i = 0; i < server.size(); i++) {
		std::string tmpname = server[i]._ip;
		std::string	tmpport = server[i]._sport;
		erase(tmpport, " ");
		if (_host == tmpname && _port == tmpport) {
			return i;
		}
		if (fd == server[i]._sock) {
			if (save == -1)
				save = i;
		}
	}
	return save;
}

std::string &hostname(int server, Config &config) {
	for (std::vector<Server>::iterator it = config._serv.begin(); it != config._serv.end(); it++) {
		if (server == it->_sock) {
			it->_connection++;
			return it->_name;
		}
	}
	config._serv[0]._connection++;
	return config._serv[0]._name;
}

std::string &port(int server, Config &config) {
	for (std::vector<Server>::iterator it = config._serv.begin(); it != config._serv.end(); it++) {
		if (server == it->_sock)
			return it->_sport;
	}
	return config._serv[0]._sport;
}

void	clear_server(std::vector<int> &server, int epoll_fd, Config &config) {
	PRINT_LOG("Exiting server");
	PRINT_LOG("Remaining clients sockets: " + to_string(csocks.size()));
	PRINT_LOG("Server socket list size: " + to_string(server.size()));
	for (std::map<int, int>::iterator it = csocks.begin(); it != csocks.end(); it++)
		if (it->second != 0)
			close(it->second);
	for (size_t j = 0; j < server.size(); j++)
		close(server[j]);
	close(epoll_fd);

	std::cout << std::endl << " Total Requests count: \033[32m" << to_string(config.request_count)
			<< "\033[0m" << std::endl << " Total connection count: \033[32m"
			<< to_string(config.connection_count) << "\033[0m" << std::endl << std::endl;

	for (size_t x = 0; x < config._serv.size(); x++) {
		std::cout << "\t\033[1;37m" << config._serv[x]._host << "\033[0m" << std::endl
			<< "\t\t" << "Connection count: \033[32m" << to_string(config._serv[x]._connection) << std::endl
			<< "\t\t" << "\033[0mRequest count: \033[32m" << to_string(config._serv[x]._requests) << std::endl
			<< "\033[0m____________________________________________"
			<< std::endl << std::endl;
	}
}

void	server_handler(Config &config, char **env) {

(void)env;
	std::vector<int> server;
	int	index;
	int	client;
	int	epoll_fd;
	int status;
	size_t num_events;
	Client tmp;
	std::vector<Client> clientlist;

	clientlist.clear();
	signalhandler();
	server = create_servers(config._serv);
	if (server.empty())
		return ;
	epoll_fd = init_epoll(server);
	if (epoll_fd == -1)
		return clear_server(server, epoll_fd, config);
	struct epoll_event events[MAX_EVENTS];

	int curr_fd = 1;
	int	c_index = 0;
	int numclient = 0;
	uint32_t	id = 1;
	int save_index;
	tmp.clear();
	while (1) {
		//num_events = epoll_wait(epoll_fd, events, curr_fd, 100);
		num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, 100);
		if (signalcheck >= 1)
			break ;
		for (size_t i = 0; i < num_events; i++) {
			index = check_server_event(events[i].data.fd, config._serv);
			if (index != -1 && events[i].data.fd == server[index]) {
				config.connection_count++;
				add_client(server[index], epoll_fd, clientlist, &id, &numclient, &curr_fd, hostname(server[index], config), port(server[index], config));
			}
			else {				
				if (events[i].events & EPOLLOUT) {
					int tmp;

					tmp = events[i].data.fd;
					c_index = find_client_in_vector(clientlist, events[i].data.fd, i);
					clientlist[c_index]._ready = true;
					events[i].events = EPOLLIN;
					epoll_ctl(epoll_fd, EPOLL_CTL_MOD, tmp, &events[i]);
				}

				if (events[i].events & EPOLLIN) {
					config.request_count++;
					client = events[i].data.fd;
					save_index = i;
					i = find_client_in_vector(clientlist, client, i);
					try {
						status = clientlist[i].request.read_client(client, clientlist[i], tmp);
						clientlist[i]._request_count++;
					}
					catch (std::exception & e) {
						std::cerr << e.what() << std::endl;
						clientlist.clear();
						return clear_server(server, epoll_fd, config);
					}
					if (tmp._name.size()) {
						clientlist.push_back(tmp);
						tmp.clear();
					}
					if (status == 1) {
						try {
							answer_client(clientlist[i], clientlist[i].request, config, epoll_fd);
						}
						catch (std::exception & e) {
							std::cerr << e.what() << std::endl;
							clientlist.clear();
							return clear_server(server, epoll_fd, config);
						}
					}
					else if (status == 0 && clientlist[i].request.in_use == false) {
						(void)status;
						//clientlist.erase(clientlist.begin() + i);
						//remove_client(client, clientlist, i, &curr_fd, &numclient, epoll_fd);
					}
					else if (status == -1) {
						//clientlist.erase(clientlist.begin() + i);
						(void)status;
						//remove_client(client, clientlist, i, &curr_fd, &numclient, epoll_fd);
					}
					if (status == 1)
						reorganize_client_list(clientlist, i, &curr_fd, &numclient, epoll_fd);
					if (status == 1) {
						for (size_t x = 0; x < clientlist.size(); x++) {
							std::cout << clientlist[x] << std::endl;
							std::cout << "\033[1;34mPath info: " << clientlist[x]._path << "\033[0m" << std::endl;
							std::cout << "============" << std::endl;
						}
					}
					i = save_index;
				}
			}
		}
		for (size_t j = 0; j < clientlist.size(); j++)
		{
			status = 0;
			if (clientlist[j].request.in_use == true) {
				try {
				status = clientlist[j].request.read_client(clientlist[j]._sock, clientlist[j], tmp);
				}
				catch (std::exception & e) {
					std::cerr << e.what() << std::endl;
					clientlist.clear();
					return clear_server(server, epoll_fd, config);
				}
				if (tmp._name.size()) {
					clientlist.push_back(tmp);
					tmp.clear();
				}
				if (status == 0 && clientlist[j].request.in_use == false) {
					remove_client(clientlist[j]._sock, clientlist, j, &curr_fd, &numclient, epoll_fd);
				}
				else if (status == -1) {
					remove_client(clientlist[j]._sock, clientlist, j, &curr_fd, &numclient, epoll_fd);
				}
			}
			if (clientlist[j].request.in_response == true || status == 1) {
				try {
					answer_client(clientlist[j], clientlist[j].request, config, epoll_fd);
				}
				catch (std::exception & e) {
					std::cerr << e.what() << std::endl;
					clientlist.clear();
					return clear_server(server, epoll_fd, config);
				}
			}
		}
	}
	clear_server(server, epoll_fd, config);
}
