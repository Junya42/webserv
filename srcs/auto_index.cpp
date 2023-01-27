#include "../includes/client.hpp"
#include <cstdlib>
#include <sstream>
#include <string>
#include <unistd.h>


std::string auto_begin = "<html><head><title>Webserv</title><style>.glass-panel {color: #fff;margin: 40px auto;background-color: rgba(255,255,255,0.06);border: 1px solid rgba(255,255,255,0.1);width: 100%;border-radius: 15px;padding: 32px;backdrop-filter: blur(10px);}.glass-button-user {display: inline-block;padding: 24px 32px;border: 0;position: absolute;top: 50px; right: 30px;text-decoration: none;border-radius: 15px;background-color: rgba(255,255,255,0.1);border: 1px solid rgba(255,255,255,0.1);backdrop-filter: blur(30px);color: rgba(255,255,255,0.8);font-size: 14px;letter-spacing: 2px;cursor: pointer;text-transform: uppercase;transition: 1s;}.glass-button-default {display: inline-block;padding: 24px 32px;border: 0;text-decoration: none;border-radius: 15px;background-color: rgba(255,255,255,0.1);border: 1px solid rgba(255,255,255,0.1);backdrop-filter: blur(30px);color: rgba(255,255,255,0.8);font-size: 14px;letter-spacing: 2px;cursor: pointer;text-transform: uppercase;transition: 1s;}.glass-button-default:hover {transform: scale(1.1);background-color: rgba(255,255,255,0.3);}.glass-button-user:hover {transform: scale(1.1);background-color: rgba(255,255,255,0.3);}.glass-button-return {display: inline-block;padding: 24px 32px;border: 0;position: absolute;top: 150px; right: 30px;text-decoration: none;border-radius: 15px;background-color: rgba(255,255,255,0.1);border: 1px solid rgba(255,255,255,0.1);backdrop-filter: blur(30px);color: rgba(255,255,255,0.8);font-size: 14px;letter-spacing: 2px;cursor: pointer;text-transform: uppercase;transition: 1s;}.glass-button-return:hover {transform: scale(1.1);background-color: rgba(255,255,255,0.3);}@media (max-width: 800px) {.glass-button-user {transition: 0s;margin: 20%;}.glass-button-return {transition: 0s;margin: 20%;}}.glass-button-folder {display: inline-block;padding: 24px 40px;margin: 13px;border: 0;text-decoration: none;border-radius: 15px;background-color: rgba(255,255,255,0.3);border: 1px solid rgba(255,255,255,0.3);backdrop-filter: blur(30px);color: rgba(255,255,255,0.8);font-size: 18px;letter-spacing: 2px;cursor: pointer;text-transform: uppercase;transition: 1s;}.glass-button-folder:hover {transform: scale(1.1);background-color: rgba(255,255,255,0.4);}.glass-button {display: inline-block;padding: 24px 32px;border: 0;text-decoration: none;border-radius: 15px;background-color: rgba(255,255,255,0.1);border: 1px solid rgba(255,255,255,0.1);backdrop-filter: blur(30px);color: rgba(255,255,255,0.8);font-size: 14px;letter-spacing: 2px;cursor: pointer;text-transform: uppercase;transition: 1s;}.glass-button:hover {transform: scale(1.1);background-color: rgba(255,255,255,0.3);}html, body {margin: 0;height: 100%;}body {background: linear-gradient(45deg, #ee7752, #e73c7e, #23a6d5, #23d5ab);background-size: 400% 400%;animation: gradient 10s ease infinite;}@keyframes gradient {0% {background-position: 0% 50%;}50% {background-position: 100% 50%;}100% {    background-position: 0% 50%;}}.glass-panel {  max-width: 600px;}.glass-button {margin: 15px;margin-top: 40px;}h1, h1 a {min-height: 120px;width: 90%;max-width: 700px;vertical-align: middle;text-align: center;margin: 0 auto;text-decoration: none;color: #fff;padding-top: 60px;color: rgba(255,255,255,0.8);}p {width: 80%;margin: 0 auto;padding-bottom: 32px;color: rgba(255,255,255,0.6);}ul {list-style-type: none;}</style></head><body><h1>WEBSERV</h1><a href =\"http://localhost:8080/html?disconnect=true\" target=\"_self\" class=\"glass-button-user\"> Disconnect ";

std::string auto_middle = "</a><a href =\"http://localhost:8080/user\" target=\"_self\" class=\"glass-button-return\"> Return</a><div class=\"glass-panel\"><div class=\"glass-toolbar\">";

std::string auto_end = "</div></div></body></html>";

std::string create_folder_button(const std::string & name)
{
    std::string ret;

    ret += "<button class=\"glass-button-folder\">";
    ret += name;
    ret += "</button>";

    return ret;
}

std::string create_file_button(const std::string & name)
{
    std::string ret;

    ret += "<button class=\"glass-button\">";
    ret += name;
    ret += "</button>";

    return ret;
}

void  Request::auto_file_name(std::vector<Server> &serv, Client &client) 
{(void)serv; (void)client;

    PRINT_FUNC();
    file_content = auto_begin;
    file_content += client._name;
    file_content += auto_middle;
    file_content += create_folder_button(".");
    file_content += create_folder_button("..");

    file_content += "</div><div class=\"glass-toolbar\">";

    for ( std::vector<std::string>::iterator it = client._files.begin() ; it != client._files.end(); it++ )
    {
        std::string its = *it;
        //its.pop_back();
        PRINT_LOG(its);
        file_content += create_file_button(its);
    }

    file_content += auto_end;
    PRINT_WIN("Success");
}