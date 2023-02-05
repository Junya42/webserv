#ifndef SOCKET_HPP
# define SOCKET_HPP

#include <asm-generic/socket.h>
#include <cstdint>
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

int init_server_socket(int port);
int init_epoll(std::vector<int> &server);

int add_client(int server, int epoll_fd, std::vector<Client> &clientlist, uint32_t *id, int *numclient, int *curr_fd, std::string &hostname, std::string &port);
int remove_client(int client, std::vector<Client> &clientlist, int i, int *curr_fd, int *numclient, int epoll_fd);
size_t	get_serv_from_client(Client &client, std::vector<Server> &serv);
void  answer_client(Client &client, Request &req, Config &config, char **env);

int find_client_in_vector(std::vector<Client> &clientlist, int client, int index, uint32_t id);
void reorganize_client_list(std::vector<Client> &clientlist, size_t index, int *curr_fd, int *numclient, int epoll);


#endif
