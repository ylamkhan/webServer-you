#include "../Includes/Client.hpp"
#include<signal.h>
#include <sys/wait.h>

void Client::parse_header(std::string headersCgi)
{
    std::istringstream iss(headersCgi);
    std::string line;
    while (std::getline(iss, line))
    {
        if (!line.empty())
        {
            if (line.substr(0, line.find_first_of(" \t")) == "Set-Cookie:")
            {
                cookies = line;
                cookies += "\n";
            }
            else if (line.substr(0, line.find_first_of(" \t")) == "Content-Type:")
                content_type = line.substr(line.find_first_of(" \t") + 1, line.rfind(";")-(line.find_first_of(" \t") + 1));
            else if (line.substr(0, line.find_first_of(" \t")) == "Status:")
            {
                statCgi = line.substr(line.find_first_of(" \t") + 1, 3);
                message = line.substr(line.find_first_of(" \t") + 1);
            }
        }
    }
}

void    Client::sendGet()
{
    signal(SIGPIPE,SIG_IGN);

    if (listing)
    {
        if (!openread)
        {
            listDir();
            a_file.open("./Srcs/list.html", std::ios::in | std::ios::binary);
            openread = true;
        }
        if (!readPage("./Srcs/list.html"))
            return;
    }
    else
    {
        if (cgiflag)
        {
            isCgi = true;
            a_file.open(result_file.c_str(), std::ios::in | std::ios::binary);
            cgiflag = false;
        }
        if (Opened)
        {
            if (!readPage(getUrl))
                return;
        }
        else
        {
            close(sockfd);
            closed = true;
            return;
        }
    }
}

void Client::sendPost()
{
    std::string response;
    response = "HTTP/1.1 201 Created\r\nContent-Type: text/html\r\n\r\n";
    int a = write(sockfd, response.c_str(), response.size());
    if(a<=0)
    {
        close(sockfd);
        closed = true;
        return;
    }
    close(sockfd);
    closed = true;
}

void Client::sendDelete()
{
    std::string response;
    response = "HTTP/1.1 204\r\nContent-Type: text/html\r\n\r\n";
    int a= write(sockfd, response.c_str(), response.size());
    if(a<=0)
    {
        close(sockfd);
        closed = true;
        return;
    }
    close(sockfd);
    closed = true;
}

void    Client::readDefault()
{
    std::string responseBody = "<html><head><style>body { display: flex; justify-content: center; align-items: center; height: 100vh; }</style></head><body><div><h1>" + message + "</h1></div></body></html>";
    std::string response = "HTTP/1.1 " + message + "\r\nContent-Type: text/html\r\n\r\n" + responseBody;
    int a = write(sockfd, response.c_str(), response.size());
    if(a<=0)
    {
        close(sockfd);
        closed = true;
        return;
    }
    close(sockfd);
    closed = true;
}

int    Client::readPage(std::string p)
{
    signal(SIGPIPE,SIG_IGN);
    size_t t = p.rfind(".");
    std::string typa;
    if(t != std::string::npos)
        typa = p.substr(t);
    
    store_type1();
    std::string Content_typa;
    std::map<std::string, std::string>::iterator it = mime_type1.find(typa);
    if (it != mime_type1.end())
        Content_typa = mime_type1[typa];
    else
        Content_typa = "text/html";
    char buffer[1024] = {0};
    a_file.read(buffer, sizeof(buffer) - 1);
    std::string s(buffer, a_file.gcount());
    if (isCgi && s.find("\r\n\r\n") != std::string::npos)
    {
        headersCgi = s.substr(0, s.find("\r\n\r\n"));
        s = s.substr(s.find("\r\n\r\n")+4);
        parse_header(headersCgi);
        Content_typa = content_type;
    }
    if (isCgi && !statCgi.empty() && statCgi != "200" && !flagResponse)
    {
        std::string newpath("./error_pages/" + statCgi +".html");
        if (servers.empty() || sindex >= servers.size()) {
            if(fcgi == true)
            {
                int status2;
                kill(pid, SIGKILL);
                waitpid(pid, &status2, 0);
            }
            close(sockfd);
            closed = true;
            return 0;
        }
        const mapErr& er = servers[sindex].getErrorPages();

        if (servers[sindex].isErrorPagesSet()) {
            mapErr::const_iterator it = er.find(std::atoi(statCgi.c_str()));
            if (it != er.end())
            {
                newpath = it->second;
                message = "200 OK";
                size_t t = newpath.rfind(".");
                std::string typa;
                if(t != std::string::npos)
                    typa = newpath.substr(t);
                store_type1();
                std::map<std::string, std::string>::iterator it = mime_type1.find(typa);
                if (it != mime_type1.end())
                    Content_typa = mime_type1[typa];
                else
                    Content_typa = "text/html";
            }
        }
        signal(SIGPIPE,SIG_IGN);
        a_file.close();
        a_file.open(newpath.c_str(), std::ios::in | std::ios::binary);
        std::string response;
        response = "HTTP/1.1 "+ message +"\r\n"
                    "Content-Type: "+ Content_typa+"\r\n\r\n";
        flagResponse = true;
        if (write(sockfd, response.c_str(), response.size()) <= 0)
        {
            close(sockfd);
            closed = true;
            a_file.close();
            return 0;
        }
        return 0;
    }
    if (!flagResponse)
    {
        std::string re;
        std::string response;
        response = "HTTP/1.1 " + message + "\r\n"
                        + cookies +
                        "Content-Type: "+ Content_typa+"\r\n\r\n";
        flagResponse = true;
        re = response + s;
        if (write(sockfd, re.c_str(), re.size()) <= 0)
        {
            close(sockfd);
            closed = true;
            a_file.close();
            return 0;
        }
        return 0;
    }
    if (write(sockfd, s.c_str(), s.size()) <= 0)
    {
        if (listing)
            remove("./Srcs/list.html");
        if (fcgi)
            remove(result_file.c_str());
        close(sockfd);
        closed = true;
        a_file.close();
        return 0;
    }
    
    if(a_file.eof() || a_file.fail())
    {
        if (listing)
            remove("./Srcs/list.html");
        if (fcgi)
            remove(result_file.c_str());
        close(sockfd);
        a_file.close();
        closed = true;
        return 0;
    }
    return 0;
}

void    Client::ft_open(std::string opath)
{
    if (opath.empty())
    {
        setResponse(500,"500 Internal Server Error",true);
        return;
    }
    size_t t = opath.rfind(".");
    std::string typa;
    if(t != std::string::npos)
        typa = opath.substr(t);
    
    store_type1();
    std::map<std::string, std::string>::iterator it = mime_type1.find(typa);
    if (it != mime_type1.end())
        content_type = mime_type1[typa];
    else
        content_type = "text/html";
    if (!openread)
    {
        a_file.open(opath.c_str(), std::ios::in | std::ios::binary);
        if (a_file.is_open())
            openread = true;
    }
    if (openread)
    {
        if (!readPage(opath))
            return;
    }
    else
        readDefault();

}

void Client::ft_read(int error_code, std::string ppppath) {
    if (servers.empty() || sindex >= servers.size()) {
        if(fcgi == true)
        {
            int status2;
            kill(pid, SIGKILL);
            waitpid(pid, &status2, 0);
        }
        close(sockfd);
        closed = true;
        return;
    }
    const mapErr& er = servers[sindex].getErrorPages();

    if (servers[sindex].isErrorPagesSet()) {
        mapErr::const_iterator it = er.find(error_code);
        if (it != er.end()) {
            ft_open(it->second);
        } else {
            ft_open(ppppath);
        }
    } else {
        ft_open(ppppath);
    }
}


void Client::send_client()
{
    if (status == 200)
        sendGet();
    else if (status == 201)
        sendPost();
    else if (status == 204)
        sendDelete();
    else if (status == 301)
    {
        std::string newLocation = "Location: " + redirection + "\r\n";
        std::string response = "HTTP/1.1 301 Moved Permanently\r\n" + newLocation + "Content-Length: 0\r\n\r\n";
        int a = write(sockfd, response.c_str(), response.size());
        if(a<=0)
        {
            close(sockfd);
            closed = true;
            return;
        }   
        close(sockfd);
    }
    else if (status != 500)
    {
        ft_read(status, "./error_pages/" + to_string(status) +".html");
    }
    else
        ft_read(500, "./error_pages/500.html");

}