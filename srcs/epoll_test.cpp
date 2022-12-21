#include <asm-generic/socket.h>
#include <iostream>
#include <netinet/in.h>
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
#include <unordered_map>
#include <sstream>
#include <stdio.h>

const int PORT = 8080;
const int MAX_EVENTS = 10;

void	parse_request(const std::string& request, std::string& method, std::string& path, std::unordered_map<std::string, std::string>& headers) {
  std::istringstream stream(request);

  stream >> method >> path;
  std::string line;

  while (std::getline(stream, line) && line != "\r") {
    std::istringstream line_stream(line);
    std::string key;
    std::string value;
    std::getline(line_stream, key, ':');
    std::getline(line_stream, value);

    headers[key] = value;
  }
}

std::string handle_request(std::string &request) {
	std::string answer;
	std::string method;
	std::string path;
	std::ostringstream s;
	std::unordered_map<std::string, std::string> header;
	int size = 0;
	char buffer[1024];

	parse_request(request, method, path, header);

	std::cout << "Method: " << method << std::endl;
	std::cout << "Path: " << path << std::endl;
	std::cout << "Header:" << std::endl;
	for (std::unordered_map<std::string, std::string>::iterator it = header.begin(); it != header.end(); it++) {
		std::cout << "  " << it->first << ": " << it->second << std::endl;
	}
	if (path.compare("/") == 0) {
		FILE *p_file = NULL;
		int fd;
		int pos = 0;

		p_file = fopen("../index.html", "rb");
		pos = ftell(p_file);
		fseek(p_file, 0, SEEK_END);
		size = ftell(p_file);
		fseek(p_file, pos, SEEK_SET);
		fclose(p_file);

		fd = open("../index.html", O_RDONLY);
		read(fd, buffer, size);
		close(fd);
		s << size;
	}
	answer = "HTTP/1.1 200 OK\n";
	answer += "Content-Type: text/html\n";
	answer += "Content-Length: ";
	answer += s.str();
	answer += "\n\n";
	answer += buffer;

	std::cout << std::endl << "Answer: " << answer << std::endl;
	return answer;
}

int main(void) {

	int my_bool = 1;
	int server;
	int	client;
	char buffer[1024];
	int	bytes;
	int	epoll_fd;
	int num_events;
	struct epoll_event event;
	struct sockaddr_in server_addr;

	std::cout << "initating socket" << std::endl;
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

	std::cout << "binding socket" << std::endl;
	if (bind(server, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)) == -1) {
		std::cout << "Error : BIND" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		close(server);
		exit(errno);
	}

	std::cout << "listening on socket" << std::endl;
	if (listen(server, 5) == -1) {
		std::cout << "Error : LISTEN" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		close(server);
		exit(errno);
	}

	std::cout << "Epoll create(5)" << std::endl;
	epoll_fd = epoll_create(5);
	if (epoll_fd == -1) {
		std::cout << "Error : EPOLL_CREATE" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		close(server);
		exit(errno);
	}
	event.events = EPOLLIN;
	event.data.fd = server;
	std::cout << "Epoll_ctl" << std::endl;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server, &event) == -1) {
		std::cout << "Error : EPOLL_CTL" << std::endl;
		std::cout << std::strerror(errno) << std::endl;
		close(epoll_fd);
		close(server);
		exit(errno);
	}

	std::cout << "While loop" << std::endl;
	while (1) {
		struct epoll_event events[MAX_EVENTS];

		std::cout << "Epoll_wait" << std::endl;
		num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		std::cout << "num_events: " << num_events << std::endl;
		if (num_events == -1) {
			std::cout << "Error : EPOLL_WAIT" << std::endl;
			std::cout << std::strerror(errno) << std::endl;
			close(epoll_fd);
			close(server);
			exit(errno);
		}
		for (int i = 0; i < num_events; i++) {
			if (events[i].data.fd == server) {
				std::cout << "Accepting client" << std::endl;
				client = accept(server, NULL, NULL);
				if (client == -1) {
					std::cout << "Error : ACCEPT" << std::endl;
					std::cout << std::strerror(errno) << std::endl;
					close(server);
					exit(errno);
				}
				event.events = EPOLLIN;
				event.data.fd = client;
				std::cout << "Epoll_ctl" << std::endl;
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client, &event);
			}
			else {
				bytes = read(client, buffer, sizeof(buffer));
				if (bytes > 0) {
					std::cout << "Successfully read from data" << std::endl;
					std::string request(buffer, bytes);
					std::string response = handle_request(request);
					write(client, response.c_str(), response.size());
				}
			}
			close (events[i].data.fd);
		}
	}
	close(server);
	return 0;
}
