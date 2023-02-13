#include "../includes/client.hpp"
#include <cstdlib>
#include <sstream>
#include <string>
#include <unistd.h>

std::string auto_begin = "<html><head><title>Webserv</title><style>.glass-panel {color: #fff;margin: 40px auto;background-color: rgba(255,255,255,0.06);border: 1px solid rgba(255,255,255,0.1);width: 100%;border-radius: 15px;padding: 32px;backdrop-filter: blur(10px);}.glass-button-user {display: inline-block;padding: 24px 32px;border: 0;position: absolute;top: 50px; right: 30px;text-decoration: none;border-radius: 15px;background-color: rgba(255,255,255,0.1);border: 1px solid rgba(255,255,255,0.1);backdrop-filter: blur(30px);color: rgba(255,255,255,0.8);font-size: 14px;letter-spacing: 2px;cursor: pointer;text-transform: uppercase;transition: 1s;}.glass-button-default {display: inline-block;padding: 24px 32px;border: 0;text-decoration: none;border-radius: 15px;background-color: rgba(255,255,255,0.1);border: 1px solid rgba(255,255,255,0.1);backdrop-filter: blur(30px);color: rgba(255,255,255,0.8);font-size: 14px;letter-spacing: 2px;cursor: pointer;text-transform: uppercase;transition: 1s;}.glass-button-default:hover {transform: scale(1.1);background-color: rgba(255,255,255,0.3);}.glass-button-user:hover {transform: scale(1.1);background-color: rgba(255,255,255,0.3);}.glass-button-return {display: inline-block;padding: 24px 32px;border: 0;position: absolute;top: 150px; right: 30px;text-decoration: none;border-radius: 15px;background-color: rgba(255,255,255,0.1);border: 1px solid rgba(255,255,255,0.1);backdrop-filter: blur(30px);color: rgba(255,255,255,0.8);font-size: 14px;letter-spacing: 2px;cursor: pointer;text-transform: uppercase;transition: 1s;}.glass-button-return:hover {transform: scale(1.1);background-color: rgba(255,255,255,0.3);}@media (max-width: 800px) {.glass-button-user {transition: 0s;margin: 20%;}.glass-button-return {transition: 0s;margin: 20%;}}.glass-button-folder {display: inline-block;padding: 24px 40px;margin: 13px;border: 0;text-decoration: none;border-radius: 15px;background-color: rgba(255,255,255,0.3);border: 1px solid rgba(255,255,255,0.3);backdrop-filter: blur(30px);color: rgba(255,255,255,0.8);font-size: 18px;letter-spacing: 2px;cursor: pointer;text-transform: uppercase;transition: 1s;}.glass-button-folder:hover {transform: scale(1.1);background-color: rgba(255,255,255,0.4);}.glass-button {display: inline-block;padding: 24px 32px;border: 0;text-decoration: none;border-radius: 15px;background-color: rgba(255,255,255,0.1);border: 1px solid rgba(255,255,255,0.1);backdrop-filter: blur(30px);color: rgba(255,255,255,0.8);font-size: 14px;letter-spacing: 2px;cursor: pointer;text-transform: uppercase;transition: 1s;}.glass-button:hover {transform: scale(1.1);background-color: rgba(255,255,255,0.3);}html, body {margin: 0;height: 100%;}body {background: linear-gradient(45deg, #ee7752, #e73c7e, #23a6d5, #23d5ab);background-size: 400% 400%;animation: gradient 10s ease infinite;}@keyframes gradient {0% {background-position: 0% 50%;}50% {background-position: 100% 50%;}100% {    background-position: 0% 50%;}}.glass-panel {  max-width: 600px;}.glass-button {margin: 15px;margin-top: 40px;}h1, h1 a {min-height: 120px;width: 90%;max-width: 700px;vertical-align: middle;text-align: center;margin: 0 auto;text-decoration: none;color: #fff;padding-top: 60px;color: rgba(255,255,255,0.8);}p {width: 80%;margin: 0 auto;padding-bottom: 32px;color: rgba(255,255,255,0.6);}ul {list-style-type: none;}</style></head><body><h1>WEBSERV</h1>";

std::string auto_middle = "<div class=\"glass-panel\"><div class=\"glass-toolbar\">";

std::string auto_end = "</div></div></body></html>";

std::string create_folder_button(std::string &host, const std::string &name)
{
  std::string ret;

    ret = "<a href=\"http://";
    ret += host + "/";
    ret += name;
    ret += "\" target=\"_self\" class=\"glass-button-folder\">/" + name + "</a>\n";
    return ret;
}

std::string create_file_button(std::string &host, const std::string &name)
{
    std::string ret;

    ret = "<a href=\"http://";
    ret += host + "/";
    ret += name;
    ret += "\" target=\"_self\" class=\"glass-button\">" + name + "</a>\n";
    return ret;
}

void  Request::auto_file_name(Client &client) 
{
    (void)client;
    file_content = auto_begin;
    //file_content += client._name;
    file_content += auto_middle;

    DIR *dir;
    struct dirent *entry;

    std::vector<std::string> dirnames;
    std::vector<std::string> filenames;

    dir = opendir(file_path.c_str());
    if (dir) {
        while (dir) {
            entry = readdir(dir);
            if (!entry)
                break;
            if (entry->d_type == DT_DIR) {
                if (!(entry->d_name[0] == '.' && strlen(entry->d_name) == 1))
                    dirnames.push_back(entry->d_name);
            }
            else if (entry->d_type == DT_REG)
                filenames.push_back(entry->d_name);
        }
        closedir(dir);
    }
    else {
        set_error(404);
        return ;
    }
    for (std::vector<std::string>::iterator it = dirnames.begin(); it != dirnames.end(); it++) {
        file_content += create_folder_button(host, *it);
    }

    file_content += "</div><div class=\"glass-toolbar\">";

    for (std::vector<std::string>::iterator it = filenames.begin(); it != filenames.end(); it++) {
        file_content += create_file_button(host, *it);
    }

    file_content += auto_end;
}
