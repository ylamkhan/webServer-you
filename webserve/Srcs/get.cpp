#include "../Includes/Client.hpp"


int Client::checkforCgi(std::string type)
{
    vecSPair cgis = servers[sindex].getLocations()[lindex].getCgi();

    for (size_t i = 0; i < cgis.size(); i++)
    {
        if ("." + cgis[i].first == type)
        {
            executable = cgis[i].second;
            return 1;
        }
    }

    return 0;
}

int Client::handleRealPath(std::string s)
{
    char currentDir[PATH_MAX];
    if (getcwd(currentDir, sizeof(currentDir)) != NULL)
    {
        std::string url = s;
        char resolved_path[PATH_MAX];
        realpath(url.c_str(), resolved_path);
        if (isPathAllowed(resolved_path, currentDir) == 0)
        {
            url = resolved_path;
        }
        else
        {
            setResponse(403, "403 Forbiden",true);
            return 0;
        }
    }
    else 
    {
        setResponse(500, "500 Internal Server Error", true);
        return 0;
    }
    return 1;
}

int Client::get_min()
{
    dir = opendir(url.c_str());
    if (dir != NULL)
    {
        struct stat  fileStat;
        if(stat(url.c_str(), &fileStat) == 0) 
        {
            if(fileStat.st_mode & S_IWUSR)
            {
                closedir(dir);
                isDir = true;
                Opened = true;
            }
            else 
            {
                closedir(dir);
                setResponse(403, "403 Forbiden",true);
                return 0;
            }
        }
        
    }
    else
    {
        a_file.open(url.c_str(), std::ios::in | std::ios::binary);
        if (a_file.is_open())
        {
            struct stat  fileStat;
            if(stat(url.c_str(), &fileStat) == 0) 
            {
                if(fileStat.st_mode & S_IWUSR)
                {
                    isFile = true;
                    Opened = true;
                }
                else 
                {
                    setResponse(403, "403 Forbiden",true);
                    a_file.close();
                    return 0;
                }
            }
            a_file.close();
        }
    }
    if (!Opened)
    {
        setResponse(404, "404 Not Found",true);
        return 0;
    }
    return 1;
}

int Client::get_min1(Location &loc)
{
    for (size_t i = 0; i < loc.getIndex().size(); i++)
    {
        std::string index = loc.getIndex()[i];
        getUrl = url;
        if (url[url.size()-1] != '/')
            getUrl += "/";
        if (index[0] == '/')
            index.erase(0);
        getUrl += index;
        if (!handleRealPath(getUrl))
            return 0;
        if (isDirectory(getUrl.c_str()))
        {
            redirection = index;
            setResponse(301,"301 Moved Permanently",true);
            return 0;
        }
        tmpfile.open((getUrl).c_str());
        if (!tmpfile.is_open())
            continue;
        tmpfile.close();
        size_t t = getUrl.rfind(".");
        if(t != std::string::npos)
            type = getUrl.substr(t);
        if (loc.getCgi().size())
        {
            if (checkforCgi(type) && !cgiflag)
            {
                cgiflag = true;
                cgiUrl = getUrl;
                cgi(getUrl);
                fcgi = true;
                isCgi = true;
                return 0;
            }
        }
        a_file.open(getUrl.c_str(), std::ios::in | std::ios::binary);
        if (a_file.is_open())
        {
            setResponse(200,"200 OK",true);
            return 0;
        }
    }
    if(loc.getAutoIndex() == true)
    {
        listing = true;
        setResponse(200,"200 OK",true);
        return 0;
    }
    else
    {
        setResponse(403, "403 Forbiden",true);
        return 0;
    }
    return 1;
}
int Client::get_min2(Location &loc)
{
    size_t t = url.rfind(".");
    if(t != std::string::npos)
        type = url.substr(t);
    if (loc.getCgi().size())
    {
        if (checkforCgi(type) && !cgiflag)
        {
            cgiflag = true;
            cgiUrl = url;
            isCgi = true;
            cgi(url);
            fcgi = true;
        }
    }
    if (!cgiflag)
    {
        a_file.open(url.c_str(), std::ios::in | std::ios::binary);
        getUrl = url;
        setResponse(200,"200 OK",true);
    }
    return 0;
}

void Client::get()
{
    if (!handleRealPath(reqURL))
        return;
    url = reqURL;
    Location loc = servers[sindex].getLocations()[lindex];
    if (!Opened && !get_min())
        return;
    if (isDir)
    {
        if (loc.isIndexSet())
        {
            if (!get_min1(loc))
                return;
        }
        else
        {
            if(loc.getAutoIndex() == true)
            {
                listing = true;
                setResponse(200,"200 OK",true);
                return ;
            }
            else
            {
                setResponse(403, "403 Forbiden",true);
                return ;
            }
        }
    }
    else if (isFile)
        get_min2(loc);
}

void    Client::listDir()
{
    dir = opendir(url.c_str());
    struct dirent *entry;
    std::string response;
    listfile.open("./Srcs/list.html", std::ios::out | std::ios::binary);
    while ((entry = readdir(dir)) != NULL) 
    {
        std::string entry_name = entry->d_name;
        std::string name = entry_name;
        if(entry_name != "." && entry_name != "..")
        {
            if (path[path.size()-1] == '/')
            {
                entry_name = path +  entry_name;
                
            }
            else
                entry_name = path + "/" + entry_name;   
            response = "<li><a href=\"" + entry_name + "\">" + name + "</a></li>";
            listfile.write(response.c_str(), response.size());
        }
    }
    closedir(dir);
    listfile.close();
}


