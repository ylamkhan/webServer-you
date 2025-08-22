#include "../Includes/Client.hpp"

std::string to_string_clock(clock_t value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}
int Client::open_file()
{
    std::string name_file = "file_" + to_string_clock(clock());
    Location loc = servers[sindex].getLocations()[lindex];
    if (loc.isUpPathSet())
    {
        dir = opendir(loc.getUploadPath().c_str());
        if(dir != NULL)
        {
            struct stat  fileStat;
            if(stat(loc.getUploadPath().c_str(), &fileStat) == 0) 
            {
                if(!(fileStat.st_mode & S_IWUSR))
                {
                    setResponse(403, "403 Forbiden",true);
                    closedir(dir);
                    return 0;
                }
            }
            closedir(dir);
        }
        else 
        {
            setResponse(500,"500 Internal Server Error",true);
            return 0;
        }
    }
    else
    {
        setResponse(500,"500 Internal Server Error",true);
        return 0;
    }
    url = reqURL;
    if (!handleRealPath(reqURL))
        return 0;
    if (!Opened)
    {
        dir = opendir(url.c_str());
        if (dir != NULL)
        {
            struct stat  fileStat;
            if(stat(url.c_str(), &fileStat) == 0) 
            {
                if(fileStat.st_mode & S_IWUSR)
                {
                    isDir = true;
                    Opened = true;
                    closedir(dir);
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
    }
    std::map<std::string, std::string >::iterator it;
    it = headers.find("Content-Type");
    if( it != headers.end())
    {
        value_type  = it->second;
    }
    store_type();
    std::string extension;
    std::map<std::string, std::string>::iterator at;
    at = mime_type.find(value_type);
    if( at != mime_type.end())
    {
        extension = at->second;
    }
    if (!flag_open)
    {
        std::string up = loc.getUploadPath();
        if (up[up.size()-1] != '/')
            up += "/";
        name_up_file = up + name_file + extension;
        file.open(name_up_file.c_str(), std::ios::app);
        if (file.is_open())
        {
            flag_open = true;
        }
    }
    return 1;
}

void Client::ft_chunked()
{
    if(store_hexSize > body.size())
    {
        if(save.size() <= hexSize)
        {
            save += body;
        }
        if(save.size() > hexSize)
        {
            
            store = save.substr(0, hexSize);
            if (!file.write(store.c_str(), store.size()))
            {
                setResponse(500,"500 Internal Server Error", true);
                return;
            }
            std::string sub2 = save.substr(hexSize);
            size_t posa1 = sub2.find("\r\n");
            size_t posa2;
            if(posa1 != std::string::npos)
            {
                posa2 = sub2.find("\r\n", posa1 + 2 );
                if(posa2 != std::string::npos)
                {
                    ich = 1;
                    shinka = sub2.substr(posa1 + 2, posa2 - posa1 - 2);
                    hexSize = hexa_to_dec(shinka);
                }
                sab = sub2.substr(posa2 + 2);
                save.clear();
                save += sab;
            }
            else
            {
                save.clear();
                save += sub2;
            }
            
        }
        size_cha -= (int)body.size();
        if(size_cha <= 0 && ich == 0)
        {
            std::string put = save.substr(0,save.find("\r\n0\r\n")); 
            if (!file.write(put.c_str(), put.size()))
            {
                setResponse(500,"500 Internal Server Error", true);
                return;
            }
            file.close();
            terminated = true;
        }
       
        else if( hexSize == 0 && ich == 1)
        {
            std::string put = save.substr(0,save.find("\r\n0\r\n")); 
            if (!file.write(put.c_str(), put.size()))
            {
                setResponse(500,"500 Internal Server Error", true);
                return;
            }
            file.close();
            terminated = true;
            ich = 0;
        }
       
    }
    else
    {
        size_t f = body.find("\r\n0\r\n") ;
        if( f != std::string::npos)
        {
            std::string put = body.substr(0,body.find("\r\n0\r\n"));
            body.clear();
            body = put;
            if (!file.write(body.c_str(), body.size()))
            {
                setResponse(500,"500 Internal Server Error", true);
                return;
            }
            file.close();
            terminated = true;
        }
        else
        {
            if (!file.write(body.c_str(), body.size()))
            {
                setResponse(500,"500 Internal Server Error", true);
                return;
            }
            file.close();
            terminated = true;
        }
            
    }
}

void Client::ft_binary()
{
    size_t size = body.size();
    bodySize += size;

    if (!file.write(body.c_str(), size))
    {
        setResponse(500,"500 Internal Server Error", true);
        return;
    }
    if(bodySize >= content_len)
    {
        file.close();
        terminated = true;
        return;
    }
}

bool Client::isDirectory(const char* path)
{
    struct stat pathStat;
    if (stat(path, &pathStat) == 0)
        return S_ISDIR(pathStat.st_mode);
    else
        return false; 
}

void Client::setResponse(size_t s, std::string m, bool b)
{
    status = s;
    message = m;
    flag_in_out = b;
}

void Client::handleFile()
{
    tmpfile.open((url).c_str());
    if (!tmpfile.is_open())
    {
        if (remove((name_up_file).c_str()) != 0)
        {
            setResponse(500, "500 Internal Server Error", true);
            return ;
        }
        setResponse(404, "404 Not Found", true);
        return;
    }
    tmpfile.close();
    Location loc = servers[sindex].getLocations()[lindex];
    size_t t = reqURL.rfind(".");
    if(t != std::string::npos)
        type = reqURL.substr(t);
    if (checkforCgi(type))
    {
        if (loc.getCgi().size())
        {
            if (!handleRealPath(reqURL))
                return;
            cgiUrl = url;
            getUrl = url;
            cgi(cgiUrl);
            fcgi = true;
            isCgi = true;
            getUrl = url;
            cgiflag = true;
            Opened = true;
            return ;
        }
        else
        {
            if (remove((name_up_file).c_str()) != 0)
            {
                setResponse(500, "500 Internal Server Error", true);
                return;
            }
            setResponse(403, "403 Forbiden", true);
            return;
        }
    }
    else 
    {
        if (loc.getUpload() == "ON")
            setResponse(201, "201 Created", true);
        else
        {
            if (remove((name_up_file).c_str()) != 0)
            {
                setResponse(500, "500 Internal Server Error", true);
                return;
            }
            setResponse(403, "403 Forbiden", true);
        }
        return ;
    }

}

void Client::post()
{
    if (!flag_open)
    {
        if(open_file() == 0)
            return ;
    }
    std::map<std::string, std::string >::iterator it;
    it = headers.find("Transfer-Encoding");
    if( it != headers.end())
    {  
        transfer_encoding  = it->second;
    }
    if(transfer_encoding == "chunked")
        ft_chunked();
    else 
        ft_binary();
    
    Location loc = servers[sindex].getLocations()[lindex];
    if (terminated == true)
    {
        if (isDirectory((url).c_str()))
        {
            if (loc.getUpload() == "ON")
            {
                setResponse(201, "201 Created", true);
                return ;
            }
            else
            {
                handleDirOff();
                return ;
            }

        }
        else
        {
            handleFile();
            return;
        }
    }
}

void Client::handleDirOff()
{
    Location loc = servers[sindex].getLocations()[lindex];
    if (loc.isIndexSet() && loc.getAutoIndex() == true)
    {
        for (size_t i = 0; i < loc.getIndex().size(); i++)
        {
            getUrl = reqURL;
            if (reqURL[reqURL.size()-1] != '/')
                getUrl += "/";
            getUrl += loc.getIndex()[i];
            if (!handleRealPath(getUrl))
                return;
            size_t t = getUrl.rfind(".");
            if(t != std::string::npos)
                type = getUrl.substr(t);
            tmpfile.open((getUrl).c_str());
            if (!tmpfile.is_open())
                continue;
            tmpfile.close();
            if (checkforCgi(type))
            {
                if (loc.getCgi().size())
                {
                    cgiUrl = getUrl;
                    cgi(getUrl);
                    cgiflag = true;
                    fcgi = true;
                    isCgi = true;
                    Opened = true;
                    return ;
                }
                else
                {
                    if (remove((name_up_file).c_str()) != 0)
                    {
                        setResponse(500, "500 Internal Server Error", true);
                        return;
                    }
                    setResponse(403, "403 Forbiden", true);
                    return;
                }
            }
            else 
            {
                remove((name_up_file).c_str());
                setResponse(500, "500 Internal Server Error", true);
                return;
            }
        }
        remove((name_up_file).c_str());
        setResponse(500, "500 Internal Server Error", true);
        return;
            
    }
    else
    {
        if (remove((name_up_file).c_str()) != 0)
        {
            setResponse(500, "500 Internal Server Error", true);
            return;
        }
        setResponse(403, "403 Forbiden", true);
        return;
    }
}