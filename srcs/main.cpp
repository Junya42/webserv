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
#include "../includes/config.hpp"
#include "../includes/header.hpp"

std::string	get_config(char *str) {
	std::string config_file;
	char buffer[1024];
	int	fd;

	if (!str)
		config_file = "configs/config";
	else
		config_file = str;
	fd = open(config_file.c_str(), O_RDONLY);
	if (fd < 0) {
		std::cerr << "Can't open configuration file" << std::endl;
		exit(1);
	}
	int bytes = read(fd, buffer, sizeof(buffer));
	std::string	config_buff(buffer, bytes);

	return config_buff;
}

int main(int ac, char **av) {

	std::string test;
	Config	config;
	std::string	config_buff;

	switch (ac) {
		case 1:
			test = "Default configuration";
			std::cout << "test: " << test << std::endl;
			config_buff = get_config(NULL);
			break;
		case 2:
			test = av[1];
			std::cout << "test: " << test << std::endl;
			config_buff = get_config(av[1]);
			break;
		default:
			std::cout << "Error: Too many arguments provided. Shutting down webserv." << std::endl << std::endl
				<< "Usage: ./webserv" << std::endl
				<< "       ./webserv [configuration file]" << std::endl;
			return 1;
	}
	std::cout << "add config" << std::endl;
	config.add_config(config_buff);
	std::cout << config << std::endl;
	server_handler(config);
	return 0;
}
