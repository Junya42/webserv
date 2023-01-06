#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <map>
#include <netinet/in.h>
#include <cstring>
#include "string.hpp"
#include <arpa/inet.h>
#include <fstream>
#include <vector>
#include "body.hpp"
#include "macro.hpp"
#include "config.hpp"
#include "mime.hpp"

class Client;

class Request {
  public:
    Request(void);
    ~Request(void);
    Request(std::string &request); //parse request using get_header and get_body

    void  clear(void); //clear all request variables
    int  read_client(int client, Client &parent);
    void  get_header(std::string &request, Client &parent);
    void  get_body_stream(std::istringstream &stream, Client &parent);
    void  get_body(int client);

    int   parse_header(void);
    int   parse_body(void);

    void        get_request(std::vector<Server> &serv); //located at srcs/get_request.cpp
    void        get_file(std::vector<Server> &serv);
    void        set_content_type(std::map<std::string, std::string> &_mime);
    void        get_response(std::map<std::string, std::string> &_mime, Client &client);

    void        post_request(std::vector<Server> &serv); //located at srcs/post_request.cpp

    void        delete_request(std::vector<Server> &serv); //located at srcs/delete_request.cpp

    bool  in_use;

    //Default 4096
    char                buffer[4096]; //Reading buffer
    static const int    buff_size = 16000;
    size_t              bytes; //bytes read
    size_t              current_bytes;
    size_t              body_size;

    std::string method; //method string
    std::string path; //path string
    std::string version;
    std::string host; //host string
    std::string index; //targeted html file
    std::string cookie;
    std::string URI;
    std::string transfer_encoding;
    std::string type; //content-type
    std::string key;
    std::string value;
    std::string boundary;

    std::string status;

    bool        complete_header;
    bool        parsed_header;
    bool        parsed_body;
    bool        has_body;
    bool        has_size;
    bool        auth;
    bool        in_response;
    int         header_code;

    size_t content_lenght; //size of body
    std::map<std::string, std::string> header; //header
    std::map<std::string, std::string> body; //body basic form (fname lname)

    std::string body_str; //body upload
    std::vector<Body> multi_body; //body multipart

    std::string answer; //answer buffer after request is parsed
    
    /***************************************************************/

    std::string         content_type;
    std::string         file_content; //file buffer
    std::string         file_path;
    std::string         file_type;
    size_t              file_size; //file size in text format
    size_t              read_size; //current buffer size
    size_t              read_count;
    bool                complete_file; //set to true if read_size == file_size
};

std::ostream &operator<<(std::ostream &n, Request &req);

class Client {
  public:
    Client(void);
    ~Client(void);

    void  reset(void);

    int addclient(int server, int epoll_fd, std::vector<Client> &clientlist);

    bool  _log;
    bool _fav;
    int _id;
    int _sock;
    unsigned short  _port;
    size_t  _request_count;
    std::string _cookie;
    std::string _addr;
    std::string _name;
    std::string _lastname;
    std::vector<std::string> _files;
    std::map<std::string, std::string> _info;
    sockaddr_in addr;
    socklen_t addr_len;
    Request request;
};

std::ostream &operator<<(std::ostream &nstream, Client &client);

#endif
