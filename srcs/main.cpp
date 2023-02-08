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
	struct stat path;

	if (!str)
		config_file = "configs/base.config";
	else
		config_file = str;
	fd = -1;
	if (stat(config_file.c_str(), &path) == -1) {
		std::cerr << "Can't open configuration file" << std::endl;
		config_file.clear();
		return config_file;
	}
	if (S_ISREG(path.st_mode) != 0)
		fd = open(config_file.c_str(), O_RDONLY);
	if (fd < 0) {
		std::cerr << "Can't open configuration file" << std::endl;
		config_file.clear();
		return config_file;
	}
	int bytes = 1;
	std::string config_buff;
	while (bytes) {
		std::memset(&buffer, 0, sizeof(buffer));
		bytes = read(fd, buffer, sizeof(buffer));
		for (int i = 0; i < bytes; i++)
			config_buff += buffer[i];
	}
	close(fd);
	return config_buff;
}

int main(int ac, char **av, char **env) {
	Config	config;
	std::string	config_buff;

	switch (ac) {
		case 1:
			std::cout << "Running: Default configuration" << std::endl;
			config_buff = get_config(NULL);
			break;
		case 2:
			std::cout << "Running: " << av[1] << std::endl;
			config_buff = get_config(av[1]);
			break;
		default:
			std::cout << "Error: Too many arguments provided. Shutting down webserv." << std::endl << std::endl
				<< "Usage: ./webserv" << std::endl
				<< "       ./webserv [configuration file]" << std::endl;
			return 1;
	}
	if (config_buff.size() < 1)
		return 1;
	config.add_config(config_buff);
	std::cout << config << std::endl;
	int p_id;

	p_id = getpid();
	PRINT_WIN("Process ID: " + to_string(p_id));
	server_handler(config, env);
	return 0;
}
