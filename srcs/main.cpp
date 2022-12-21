#include <iostream>
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

int main(int ac, char **av) {

	std::string test;

	switch (ac) {
		case 1:
			test = "Default configuration";
			std::cout << "test: " << test << std::endl;
			break;
		case 2:
			test = av[1];
			std::cout << "test: " << test << std::endl;
			break;
		default:
			std::cout << "Error: Too many arguments provided. Shutting down webserv." << std::endl << std::endl
				<< "Usage: ./webserv" << std::endl
				<< "       ./webserv [configuration file]" << std::endl;
			return 1;
	}
	return 0;
}
