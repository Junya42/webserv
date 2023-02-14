#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <map>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "string.hpp"
#include <arpa/inet.h>
#include <fstream>
#include <vector>
#include "body.hpp"
#include "macro.hpp"
#include "config.hpp"
#include "mime.hpp"
#include <algorithm>

class Client;

class Request {
  public:
    Request(void);
    ~Request(void);
    Request(std::string &request); //parse request using get_header and get_body
    //Request &operator=(const Request &req);
    void        clear(void); //clear all request variables
    int         read_client(int client, Client &parent, Client &tmp);
    void        get_header(std::string &request, Client &parent, Client &tmp);
    void        get_body_stream(std::istringstream &stream, Client &parent, Client &tmp);
    void        get_body(int client);

    std::string check_method(std::string &method);
    void        set_error(int code, const char *s1 = __builtin_FUNCTION(), int line = __builtin_LINE());
    int         parse_header(void);
    int         parse_body(Client &parent);

    void        get_request(std::vector<Server> &serv, Client &client, int index); //located at srcs/get_request.cpp
    void        get_file(std::vector<Server> &serv, Client &client, std::string &path_info, std::string &file_path, int index, size_t flag = 0);
    void        get_cgi_read(Client &client, std::string &cgi_path, std::string &cgi_executor, std::string &file_path, std::string &pwd, int flag = 0, size_t n = 0);
    void        get_cgi_answer(Client &client);
    void        get_cgi(Client &client, Config &config, int flag = 0);
    void        set_content_type(std::map<std::string, std::string> &_mime, size_t flag = 0);
    void        get_response(std::map<std::string, std::string> &_mime, Client &client);

    void        auto_file_name(Client &client);

    void        post_request(std::vector<Server> &serv); //located at srcs/post_request.cpp

    void        delete_request(std::vector<Server> &serv); //located at srcs/delete_request.cpp
    
    void        download_delete_cgi(Client &client, Server &serv, const char *path, char **env);

    void        state_func(std::istringstream &stream);
    int         state_0(std::istringstream &stream);
    int         state_1(std::istringstream &stream);
    int         state_2(std::istringstream &stream);

    void        state_func(int client);
    int        state_0(int client);
    int         state_1(int client);
    int         state_2(int client);

    bool  in_use;

    char                buffer[4096];
    static const int    buff_size = 16000;
    size_t              bytes;
    size_t              current_bytes;
    size_t              body_size;
    size_t              linecount;

    size_t              ncount;
    size_t              fpos;
    long long int       initial_lenght;

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
    
    std::string link;

    std::string path_info;
    std::string query;
    std::string cgi;
    std::string cgi_path;

    std::string name;

    std::string status;

  
    bool          complete_chunk;
    int           limit;
    int           chunk_left;
    int           chunk_size;
    int           state;
    int           to_skip;
    FILE          *cfile;
    std::string   cname;
    std::string   temp0;
    std::string   temp2;

    bool        complete_header;
    bool        complete_body;
    bool        parsed_header;
    bool        parsed_body;
    bool        has_body;
    bool        has_size;
    bool        auth;
    bool        in_response;
    bool        auto_index;
    int         auth_redirect;
    int         header_code;

    bool        using_cgi;
    bool        chunked;
    size_t      nread;
    size_t      cgi_size;

    size_t content_lenght; //size of body
    std::map<std::string, std::string> header; //header
    std::map<std::string, std::string> body; //body basic form (fname lname)

    std::string body_str; //body upload
    std::vector<Body> multi_body; //body multipart

    std::string answer; //answer buffer after request is parsed
    
    /***************************************************************/
    std::string         sline;
    std::string         content_type;
    std::string         file_content; //file buffer
    std::string         file_path;
    std::string         file_type;
    size_t              file_size; //file size in text format
    size_t              read_size; //current buffer size
    size_t              read_count;
    bool                complete_file; //set to true if read_size == file_size
    bool                found_user;
};

std::ostream &operator<<(std::ostream &n, Request &req);

class Client {
  public:
    Client(void);
    ~Client(void);

    void  reset(void);

    int addclient(int server, int epoll_fd, std::vector<Client> &clientlist);

    Client &operator=(Client const &client) {
      _fav = client._fav;
      _id = client._id + 1;
      _sock = client._sock;
      _log = true;
      _request_count = client._request_count;
      _port = client._port;
      _cookie = client._cookie;
      _addr = client._addr;
      _name = client._name;
      _lastname = client._lastname;
      _path = client._path;
      _files = client._files;
      _host = client._host;
      _ip = client._ip;
      _sport = client._sport;
      _hostport = client._hostport;
      _hostip = client._hostip;
      //request = client.request;
      request.clear();
      return *this;
    }

    void  clear(void) {
      _log = false;
      _fav = false;
      _ready = false;
      _index = -1;
      _id = 0;
      _sock = 0;
      _port = 0;
      _myport = 0;
      _request_count = 0;
      _cookie.clear();
      _addr.clear();
      _name.clear();
      _lastname.clear();
      _oldname.clear();
      _path.clear();
      _host.clear();
      _hostsport.clear();
      _hostip.clear();
      _ip.clear();
      _sport.clear();

      _files.clear();
      _info.clear();
      request.clear();
    }

    bool  _log;
    bool _fav;
    bool  _ready;
    int   _index;
    uint32_t _id;
    int _sock;
    unsigned short  _port;
    unsigned int    _myport;
    size_t  _request_count;
    
    std::string _cookie;
    std::string _addr;
    std::string _oldname;
    std::string _name;
    std::string _lastname;
    std::string _path;
    std::string _host;
    unsigned int _hostport;
    std::string _hostsport;
    std::string _hostip;
    std::string _ip;
    std::string _sport;
 //   std::map<std::string, std::vector<std::string>> _files;
    std::vector<std::string> _files;
    std::map<std::string, std::string> _info;
    sockaddr_in addr;
    socklen_t addr_len;
    Request request;
};

std::ostream &operator<<(std::ostream &nstream, Client &client);

#endif
