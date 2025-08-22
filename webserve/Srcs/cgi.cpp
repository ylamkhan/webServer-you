#include "../Includes/Client.hpp"

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>

void Client::cgi(std::string u)
{
    if (u.empty())
    {
        remove((name_up_file).c_str());
        setResponse(500,"500 Internal Server Error",true);
        return;
    }
    int stat;
    if (!fcgi)
    {
        fcgi = true;
        std::string a = "CONTENT_LENGTH=" + headers["Content-Length"];
        std::string b = "CONTENT_TYPE=" + headers["Content-Type"];
        std::string c = "PATH_TRANSLATED=" + u.substr(u.rfind('/')+1);
        std::string d = "REQUEST_METHOD=" + method;
        std::string e = "QUERY_STRING=" + query;
        std::string f = "REDIRECT_STATUS=CGI";
        std::string g = "HTTP_COOKIE=" + headers["Cookie"];
        std::string h = "PATH_INFO=" + path_info;

        char *env[]= {
            (char *)a.c_str(),
            (char *)b.c_str(),
            (char *)c.c_str(),
            (char *)d.c_str(),
            (char *)e.c_str(),
            (char *)f.c_str(),
            (char *)g.c_str(),
            (char *)h.c_str(),
            NULL
        };

        startTimeCgi = clock();
        result_file = u.substr(0,u.find_last_of('/')+1) + "result.txt";
        pid = fork();
        if (pid < 0)
        {
            if (method == "POST")
                remove((name_up_file).c_str());
            setResponse(500,"500 Internal Server Error",true);
            return;
        }
        else if (pid == 0)
        {
            if (method == "POST")
            {
                int fd2;
                fd2 = open((name_up_file).c_str(), O_RDONLY);
                if(fd2<0)
                {
                    remove((name_up_file).c_str());
                    setResponse(500,"500 Internal Server Error",true);
                    return;
                }           
                if(dup2(fd2, 0)<0)
                {
                    remove((name_up_file).c_str());
                    setResponse(500,"500 Internal Server Error",true);
                    return;
                }
                close(fd2);
            }
            int fd_result = open(result_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            if (fd_result < 0)
            {
                if (method == "POST")
                    remove((name_up_file).c_str());
                setResponse(500,"500 Internal Server Error",true);
                return;
            }
            if (dup2(fd_result, STDOUT_FILENO) == -1)
            {
                if (method == "POST")
                    remove((name_up_file).c_str());
                setResponse(500,"500 Internal Server Error",true);
                return;
            }
            close(fd_result);
            std::string s = (u.substr(u.rfind('/')+1));
            char *arg[] = {(char *)executable.c_str(), (char *)(s).c_str(), NULL};
            if(u.find("/") != std::string::npos){
                std::string newD = u.substr(0, u.rfind("/"));
                chdir(newD.c_str());
            }
            if (execve(executable.c_str(), arg, env) < 0)
            {
                kill(getpid(), SIGINT);
                return;
            }
        }
    }
    else if (double(static_cast<double>(clock() - startTimeCgi) / CLOCKS_PER_SEC) > TIMECGI)
    {
        int status1;
        kill(pid, SIGKILL);
        waitpid(pid, &status1, 0);
        if (method == "POST")
            remove((name_up_file).c_str());
        remove(result_file.c_str());
        setResponse(504,"504 Gateway Timeout",true);
        isCgi = false;
        return;
    }
    else if (waitpid(pid, &stat, WNOHANG) == 0) {
        return ;
    }
    else
    {
        if (WIFEXITED(stat))
        {
            int exit_status = WEXITSTATUS(stat);
            if (exit_status == 0)
            {
                setResponse(200,"200 OK",true);
                if (method == "POST")
                    remove((name_up_file).c_str());
                return;
            }
            else
            {
                if (method == "POST")
                    remove((name_up_file).c_str());
                setResponse(500,"500 Internal Server Error",true);
                return;
            }
        }
        if(WIFSIGNALED(stat))
        {
            if (method == "POST")
                remove((name_up_file).c_str());
            setResponse(500,"500 Internal Server Error",true);
            return;
        }
    }

}