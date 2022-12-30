#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <map>
#include <vector>
#include "body.hpp"

class Request {
  public:
    Request(void);
    ~Request(void);
    Request(std::string &request); //parse request using get_header and get_body

    void  clear(void); //clear all request variables
    int  read_client(int client);
    void  get_header(std::string &request);
    void  get_body(int client);

    int   parse_header(void);
    int   parse_body(void);

    char        buffer[4096]; //Reading buffer
    size_t      bytes; //bytes read
    size_t      current_bytes;
    size_t      body_size;

    std::string method; //method string
    std::string path; //path string
    std::string version;
    std::string host; //host string
    std::string index; //targeted html file
    std::string type; //content-type
    std::string key;
    std::string value;
    std::string boundary;
    bool        complete_header;
    bool        parsed_header;
    bool        parsed_body;
    bool        has_body;
    int         header_code;

    size_t content_lenght; //size of body
    std::map<std::string, std::string> header; //header
    std::map<std::string, std::string> body; //body basic form (fname lname)

    std::string body_str; //body upload
    std::vector<Body> multi_body;

    std::string answer; //answer buffer after request is parsed
    

    /*  Probably need to add:
     *
     *  Add a fd file
     *  Path to fd file
     */
};

std::ostream &operator<<(std::ostream &n, Request &req);

#endif
