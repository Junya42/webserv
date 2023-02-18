#ifndef ERROR_HPP
# define ERROR_HPP

#include <string>
#include <unistd.h>
#include "client.hpp"

void    expect_continue(int client); //100

// Client side errors
void  bad_request(int client); //400
void  unauthorized(int client); //401
void  forbidden(int client); //403
void  not_found(int client); //404
void  method_not_allowed(int client); //405
void  gone(int client); //410
void  length_required(int client); //411
void  content_too_large(int client); //413
void  uri_too_long(int client); //414
void  unsupported_media_type(int client); //415

// Server side errors
void  internal_server_error(int client); //500
void  not_implemented(int client); //501
void  http_version_not_supported(int client); //505
void  insufficient_storage(int client); //507

//  Switch error
void  send_error(int client, int code, Client &curr, std::string path);

// Redirect to error page
void  redirect_error(int client, int code);
#endif
