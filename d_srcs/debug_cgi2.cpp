#include "../includes/cgi.hpp"
#include "../includes/socket.hpp"
#include <cstdlib>
#include <set>

char **create_env(Client &client, std::string &pwd, std::string &script, std::string &path) {
    std::set<std::string> set_env;

    set_env.insert("PATH=" "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin");
    set_env.insert("PWD=" + pwd);

    set_env.insert("SERVER_SOFTWARE=" "HTTP/1.1");
    set_env.insert("SERVER_NAME=" + client._host);
    set_env.insert("GATEWAY_INTERFACE=" "CGI/1.1");

    set_env.insert("SERVER_PROTOCOL=" "Webserv/1");
    set_env.insert("SERVER_PORT=" + client._sport);
    set_env.insert("REQUEST_METHOD=" + client.request.method);
    set_env.insert("PATH_INFO=" + client.request.path_info);
    set_env.insert("PATH_TRANSLATED=" + path);
    set_env.insert("SCRIPT_NAME=" + script);
    if (client.request.query.size())
        set_env.insert("QUERY_STRING=" + client.request.query);
    set_env.insert("REMOTE_HOST=" "");
    set_env.insert("REMOTE_ADDR=" + client._ip);
    set_env.insert("AUTH_TYPE=" "");
    if (client._name.size() < 1)
        set_env.insert("REMOTE_USER=" "unknown");
    else
        set_env.insert("REMOTE_USER=" + client._name);
    set_env.insert("REMOTE_IDENT=" "");
    set_env.insert("CONTENT_TYPE=" + client.request.content_type);
    set_env.insert("CONTENT_LENGTH=" + to_string(client.request.initial_lenght));

    set_env.insert("HTTP_ACCEPT=" + client.request.header["Accept"]);
    set_env.insert("HTTP_ACCEPT_LANGUAGE=" + client.request.header["Accept-Language"]);
    set_env.insert("HTTP_USER_AGENT=" + client.request.header["User-Agent"]);
    set_env.insert("HTTP_COOKIE=" + client.request.header["Cookie"]);
    set_env.insert("HTTP_REFERER=" + client.request.header["Referer"]);

    char **env = new char*[set_env.size() + 1];
    if (!env)
        return NULL;
    size_t  i = 0;

    for (std::set<std::string>::iterator it = set_env.begin(); it != set_env.end(); it++) {
        env[i] = new char[(*it).size() + 1];
        env[i] = strcpy(env[i], (*it).c_str());
        i++;
    }
    env[i] = NULL;
    for (size_t x = 0; env[x]; x++)
        PRINT_LOG("ENV[" + to_string(x) + "] : " + env[x]);
    std::cout << std::endl;
    return env;
}

char **create_args(std::string &cgi_path, std::string &cgi_executor, int flag) {
    char **args;


    (void)cgi_executor;
    switch (flag) {
        case 1:
            args = new char*[4];
            if (!args)
                return NULL;
            if (cgi_executor.size()) {
                args[0] = strdup(cgi_executor.c_str());
                args[1] = strdup(cgi_path.c_str());
                args[2] = strdup("replace");
                args[3] = NULL;
            }
            else {
                args[0] = strdup(cgi_path.c_str());
                args[1] = strdup("replace");
                args[2] = NULL;
                args[3] = NULL;
            }
            break;
        default:
            args = new char*[3];
            if (!args)
                return NULL;
            if (cgi_executor.size()) {
                args[0] = strdup(cgi_executor.c_str());
                args[1] = strdup(cgi_path.c_str());
                args[2] = NULL;
            }
            else {
                args[0] = strdup(cgi_path.c_str());
                args[1] = NULL;
                args[2] = NULL;
            }
            break;
    }
    /*for (size_t i = 0; args[i]; i++)
        PRINT_LOG("ARGS[" + to_string(i) + "] : " + args[i]);*/
    std::cout << std::endl;
    return args;
}

void    Request::get_cgi_read(Client &client, std::string &cgi_path, std::string &cgi_executor, std::string &file_path, std::string &pwd, int flag, size_t n) {
    PRINT_FUNC();
    pid_t   pid;

    FILE    *inFile = tmpfile();
    FILE    *outFile = tmpfile();
    FILE    *errFile = tmpfile();
    int     inFd = fileno(inFile);
    int     outFd = fileno(outFile);
    int     errFd = fileno(errFile);

    int     status;
    int     ret;

    file_content.clear();
    pid = fork();

    if (pid < 0)
        return set_error(500);
    else if (pid == 0) {
        char **args = create_args(cgi_path, cgi_executor, flag);
        if (!args)
            exit(42);
        char **env = create_env(client, pwd, cgi_path, file_path);
        if (!env) {
            for (size_t i = 0; args[i]; i++)
                delete[] args[i];
            delete[] args;
            exit(42);
        }

        dup2(inFd, 0);
        dup2(outFd, 1);
        dup2(errFd, 2);
        close(inFd);
        close(outFd);
        close(errFd);

        execve(args[0], args, env);

        PRINT_ERR("After Execve");
        for(size_t i = 0; args[i]; i++)
            delete[] args[i];
        delete[] args;

        for (size_t i = 0; env[i]; i++)
            delete[] env[i];
        delete[] env;
        std::cerr << "500" << std::endl;
        exit(42);
    }
    else {
        if (waitpid(pid, &status, 0) == -1)
            set_error(500);
        if (WIFEXITED(status)) {
            ret = WEXITSTATUS(status);
            if (ret != 0) {
                int errBytes = 1;
                char errbuff[buff_size];
                
                std::string errString;
                while (errBytes) {
                    memset(errbuff, 0, buff_size);
                    errBytes = read(errFd, errbuff, buff_size - 1);
                    for (int i = 0; i < errBytes; i++)
                        errString += errbuff[i];
                }
                set_error(atoi(errString.c_str()));
            }
            else {
                int errBytes = 1;
                char errbuff[buff_size];
                
                std::string errString;
                while (errBytes) {
                    memset(errbuff, 0, buff_size);
                    errBytes = read(errFd, errbuff, buff_size - 1);
                    for (int i = 0; i < errBytes; i++)
                        errString += errbuff[i];
                }
                std::cerr << "Errors from CGI:" << std::endl
                << errString << std::endl << std::endl;
            }
        }
        if (n == 0) {
            (void)n;
        }
        lseek(outFd, 0, SEEK_SET);

        int     rbytes = 1;
        char    buff[buff_size];

        while (rbytes) {
            memset(buff, 0, buff_size);
            rbytes = read(outFd, buff, buff_size - 1);
            for (int i = 0; i < rbytes; i++)
                file_content += buff[i];
        }
    }
    fclose(inFile);
    fclose(outFile);
    fclose(errFile);
    std::cout << "end of cgi" << std::endl;
    if (comp(cgi_executor, "python") == true)
        std::cout << "\033[1;32m" << file_content << "\033[0m" << std::endl;
    complete_file = true;
}

void    Request::get_cgi_answer(Client &client) {
    bool    redirect = false;

    PRINT_FUNC();
    answer.clear();
    if (comp(path, "download") == false && comp(path, "delete") == false) {
        if (method == "POST")
            status = "HTTP/1.1 201 Created\n";
        if (comp(query, "disconnect=true") == true) {
            PRINT_ERR("Redirect 0");
            redirect = true;
            client._log = false;
            status = "HTTP/1.1 307 Temporary Redirect\nLocation: http://" + client._host + ":" + client._sport + "/html\n";
        }
        else if (auth_redirect == 1 && auth == true && client._cookie.size()) {
            PRINT_ERR("Redirect 1");
            status = "HTTP/1.1 307 Temporary Redirect\nLocation: http://" + client._host + ":" + client._sport + "/user\n";
        }
        else if (auth_redirect == 2) {
            PRINT_ERR("Redirect 2");
            status = "HTTP/1.1 307 Temporary Redirect\nLocation: http://" + client._host + ":" + client._sport + "/login\n";
        }
        answer = status;
        if (client._name.size() && comp(file_path, "/user/*.html") == true && client._fav == false) {
            PRINT_WIN("Adding cookie to: " + client._name);
            client._log = true;
            client._cookie = "log=" + client._name;
            answer += "Set-Cookie: ";
            answer += client._cookie;
            answer += "; Path=/; Expires= Fri, 5 Oct 2042 14:42:00 CMT;\n";
        }
        else if (redirect == true) {
            PRINT_ERR("Removing cookie from: " + client._name);
            std::string tmp = "log=" + client._name + "; Path=/; Expires=Fri, 5 Oct 2018 14:42:00 GMT;\n";
            answer += "Set-Cookie: ";
            answer += tmp;
        }
        answer += "Content-Type: " + content_type + "\n";
        if (comp(content_type, "html") == false)
            answer += "Content-Transfer-Encoding: binary\n";
        answer += file_content;
    }
    else {
        answer = "HTTP/1.1 200 OK\n";
        answer += file_content;
    }
}

void    get_executor(std::vector<Server> &serv, Request &req, std::string &cgi_executor) {
    for (size_t i = 0; i < serv.size(); i++)
        if (comp(req.host, serv[i]._host)) {
            cgi_executor = serv[i].cgi[cgi_executor];
        }
}

void    Request::get_cgi(Client &client, Config &config, int flag) {
    PRINT_FUNC();
    if (file_path.size() < 1) {
        int index = get_serv_from_client(client, config._serv);
        if (path_info.size()) {
            this->get_file(config._serv, client, path_info, file_path, index);
            this->set_content_type(config._mime, 1);
        }
        this->get_file(config._serv, client, path, cgi_path, index, 1);
        std::string tmp;

        tmp = config._pwd;
        tmp.erase(tmp.size() - 1, 1);
        cgi_path = tmp + cgi_path;
        PRINT_LOG("Cgi_path :" + cgi_path);
    }
    //if (complete_file == true)
        //return ;
    if (comp(client.request.header["Referer"], "download") == false || client.request.path_info == "/user")
        flag = 1;
    size_t extension_pos = cgi_path.find_last_of('.');
    std::string cgi_executor;
    if (extension_pos != std::string::npos) {
        cgi_executor = cgi_path.substr(extension_pos);
        get_executor(config._serv, client.request, cgi_executor);
    }
    get_cgi_read(client, cgi_path, cgi_executor, file_path, config._pwd, flag);
    get_cgi_answer(client);
}
