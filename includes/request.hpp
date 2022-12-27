#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <map>

class Request {
  public:
    Request(void);
    ~Request(void);
    Request(std::string &request); //parse request using get_header and get_body

    void  clear(void); //clear all request variables

    void  get_header(void);
    void  get_body(void);

    char        buffer[4096]; //Reading buffer
    size_t      bytes; //bytes read

    std::string method; //method string
    std::string path; //path string
    std::string host; //host string
    std::string index; //targeted html file
    std::string type; //content-type

    size_t content_lenght; //size of body
    std::map<std::string, std::string> header; //header
    std::map<std::string, std::string> body; //body basic form (fname lname)

    std::string body_str; //body upload

    std::string answer; //answer buffer after request is parsed
    

    /*  Probably need to add:
     *
     *  Add a fd file
     *  Path to fd file
     */
};

#endif
