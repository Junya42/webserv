#include "../includes/socket.hpp"

//const int PORT = 8080;
const int MAX_EVENTS = 100;

/*	Create a server socket
 *	Set the socket option SO_REUSEADDR
 *	Add informations to socket
 *	Bind the socket
 *	Start listening
 */
void	server_handler(Config &config, char **env) {

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
				//PRINT_WIN("EPOLLIN Client event");
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
					answer_client(clientlist[i], clientlist[i].request, config, env);
				}
				else if (status == 0 && clientlist[i].request.in_use == false) {
					PRINT_ERR("Client timed out or disconnected");
					//std::cout << "\033[1;31m" << clientlist[i] << "\033[0m" << std::endl;
					remove_client(client, clientlist, i, &curr_fd, &numclient, epoll_fd);
				}
				else if (status == -1) {
					PRINT_ERR("Error syscall read / write");
					remove_client(client, clientlist, i, &curr_fd, &numclient, epoll_fd);
				}
				reorganize_client_list(clientlist, i, &curr_fd, &numclient, epoll_fd);
				clientlist[i]._request_count++;
				std::cout << "Number of clients : " << numclient << std::endl;
				std::cout << "Number of fd : " << curr_fd << std::endl;
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
				//PRINT_LOG("Outside read");
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
	close(server);
}
