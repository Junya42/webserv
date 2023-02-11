#include "../includes/client.hpp"
#include <cstdio>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

char **create_args(std::string &name, std::vector<std::string> &files, const char *path) {
    char **args;

    (void)path;
    args = (char **)malloc(sizeof(char *) * files.size() + 4);
    //args[0] = strdup("/usr/bin/python3");
    args[0] = strdup(path);
    //args[0] = strdup("/home/junya/serv/www/cgi-bin/c.cgi");
    args[1] = strdup(name.c_str());
    for (size_t i = 0; i < files.size(); i++)
        args[i + 2] = strdup(files[i].c_str());
    args[files.size() + 2] = 0;
    return args;
}

void    Request::download_delete_cgi(Client &client, Server &serv, const char *path, char **env) {

    PRINT_FUNC();
    (void)serv;
    pid_t   pid;

   FILE    *inFile = tmpfile();
   FILE    *outFile = tmpfile();

   int     inFd = fileno(inFile);
   int     outFd = fileno(outFile);

    int     status;
    int     ret;

    file_content.clear();
    pid = fork();

    if (pid < 0) {
        //PRINT_ERR("Error fork");
        return set_error(500);
    }
    if (pid == 0) {
        char **args = create_args(client._name, client._files, path);

        dup2(inFd, 0);
        dup2(outFd, 1);
        close(inFd);
        close(outFd);

        //for (size_t i = 0; args[i]; i++)
            //PRINT_ERR(args[i]);
        execve(args[0], args, env);

        PRINT_ERR("Couldn't execute execve");
        for (size_t i = 0; args[i]; i++) 
            delete[] args[i];
        delete[] args;
        exit(42);
    }
    else {
        if (waitpid(pid, &status, 0) == -1)
            set_error(500);
        if (WIFEXITED(status)) {
            ret = WEXITSTATUS(status);
            if (ret == 42) {
                PRINT_ERR("SERVER ERROR");
                set_error(500);
            }
        }
        lseek(outFd, 0, SEEK_SET);

        size_t  rbytes = 1;
        char buff[buff_size];

        while (rbytes) {
            memset(buff, 0, buff_size);
            rbytes = read(outFd, buff, buff_size - 1);
            file_content += buff;
        }
    }
    fclose(inFile);
    fclose(outFile);

    complete_file = true;
}
