#include "../Includes/Client.hpp"

typedef  std::map<std::string, std::string>::iterator iter_map;

unsigned int Client::hexa_to_dec(const std::string& str)
{
    std::stringstream ss;
    ss << std::hex << str;
    unsigned int result;
    ss >> result;
    return result;
}

std::string& Client::ltrim(std::string& str)
{
    std::string::iterator it = str.begin();
    while (it != str.end() && std::isspace(*it)) {
        ++it;
    }
    str.erase(str.begin(), it);
    return str;
}

int Client::check_me()
{
    if(method != "POST" && method != "GET" && method != "DELETE")
        return 0;
    else
        return 1;
}

void    Client::parseRequest(const std::string& httpRequest)
{
    if (!headerSet)
        setReqStr(httpRequest);
    if (!headerSet && requestStr.find("\r\n\r\n", 0) != std::string::npos)
    {
        const std::string& startLine = requestStr.substr(0, requestStr.find("\r\n", 0));
        if(!parseHttpRequestStartLine(startLine))
        {
            setResponse(400,"400 Bad Request",true);
            return;
        }
        timeout = true;
        size_t pos = 0;
        size_t end = 0;
        size_t ch = requestStr.find("\r\n\r\n");
        if(ch != std::string::npos)
            ch = ch + 4;
        end = requestStr.find(' ');
        method = requestStr.substr(pos, end - pos);
        if (!check_me())
        {
            setResponse(501,"501 Not Implemented",true);
            return ;
        }
        pos = end + 1;
        end = requestStr.find(' ', pos);
        std::istringstream iss(requestStr.substr(pos, end - pos));
        std::getline(iss,path,'?');
        iss>>query;
        if (!path.empty())
        {
            if (path[path.size()-1] == '/' && path != "/")
                path.erase(path.size()-1);
        }
        pos = end + 1;
        end = requestStr.find("\r\n", pos);
        httpVersion = requestStr.substr(pos, end - pos);
        pos = end + 2;
        while ((end = requestStr.find("\r\n", pos)) != std::string::npos)
        {
            std::string line = requestStr.substr(pos, end - pos);
            std::istringstream iss(line);
            std::string headerName, headerValue;  
            std::getline(iss,headerName,':');
            std::getline(iss,headerValue);
            headers[headerName] = ltrim(headerValue);
            pos = end + 2;
            if(headerName == "Host")
                setPortHost(headerValue);
            
        }
        if(host.empty() || !parse_Host())
        {
            setResponse(400,"400 Bad Request",true);
            return;
        }
        else
        {
            if(host.substr(0,host.find(":")) == "localhost")
                host = "127.0.0.1" + host.substr(host.find(":"));
        }
        std::string hel = requestStr.substr(ch);
        chunks_size = hel.find("\r\n");
        if(chunks_size != std::string::npos)
        {
            chunks = hel.substr(0, chunks_size);
            hexSize = hexa_to_dec(chunks);
            store_hexSize = hexa_to_dec(chunks);
            size_cha = hexa_to_dec(chunks);
        }
        body = requestStr.substr(ch);

        std::map<std::string, std::string >::iterator it;
        it = headers.find("Transfer-Encoding");
        if( it != headers.end())
        {   
            transfer_encoding  = it->second;
            if(transfer_encoding == "chunked")
            {
                body = requestStr.substr(ch + chunks.size() + 2);
                ss = "\r\n" + chunks + "\r\n";

            }
        }
        headerSet = true;
    }
    else
        body = httpRequest;
    if (headerSet == true)
    {
        if(flag_hand_method == false)
        {
            update_servers();
            if(!matching_servers())
            {
                setResponse(404, "404 Not Found",true);
                return ;
            }
            if (!is_req_well_formed())
            {
                flag_in_out = true;
                return ;
            }
            parsePath();
            Location loc = servers[sindex].getLocations()[lindex];
            if (loc.isRedirUrlSet())
            {
                redirection = servers[sindex].getLocations()[lindex].getRedirUrl();
                setResponse(301,"301 Moved Permanently",true);
                return;
            }
            if(!checkMethod())
            {
                setResponse(405,  "405 Method Not Allowed",true);
                return ;
            }
            flag_hand_method = true;
        }
        handl_methodes();
    }
}

int Client::parse_Host()
{
    int nubp = 0;
    for(size_t i = 0 ; i < host.size() ; i++)
    {
        if(host[i] == ':')
            nubp++;
    }
    if(nubp == 1)
        return 1;
    return 0;
    
}
//////////////////////////////


void Client::update_servers()
{
    int i = (int)servers.size()-1;
    while (i>=0)
    {
        std::stringstream ss;
        ss << servers[i].getPort();
        std::stringstream ss1;
        ss1 << servers[sindex].getPort();
        std::string str = ss.str();
        std::string str1 = ss1.str();
        if(servers[sindex].getPort() == servers[i].getPort() && servers[sindex].getHost() == servers[i].getHost() && (host == servers[i].getServerNames()+":"+str1 || host == servers[i].getHost()+":"+str))
        {
            sindex = i;
            return;
        }
        i--;
    }
    if(i < 0)
    {
        i  = (int)servers.size()-1;
        while(i>=0)
        {
            if(servers[sindex].getPort() == servers[i].getPort() && servers[sindex].getHost() == servers[i].getHost())
            {
                sindex = i;
                return;
            }
            i--;
        } 
    }
}

int getMethodIndex(std::string method)
{
    if (method == "GET")
        return 0;
    else if (method == "POST")
        return 1;
    else if (method == "DELETE")
        return 2;
    return -1;
}

int Client::checkMethod()
{
    std::vector<int> methods = servers[sindex].getLocations()[lindex].getMethods();
    if(methods.size())
    {
        for (size_t i = 0; i < methods.size(); i++)
        {
            if (getMethodIndex(method) == methods[i])
                return 1;
        }
    }
    
    return 0;
}

int Client::is_req_well_formed()
{
    iter_map it = headers.find("Transfer-Encoding");
    if(it != headers.end() && method == "POST" && it->second != "chunked")
    {
        setResponse(501,"501 Not Implemented",false);
        return 0;  
    }
    iter_map it1 = headers.find("Content-Length");
    iter_map it2 = headers.find("Content-Type");
    if((method == "POST" && it == headers.end() && it1 == headers.end()) || (method == "POST" && it2 == headers.end()))
    {
        setResponse(400,"400 Bad Request",false);
        return 0;
    }
    if(method == "POST" && it2->second.find("form-data") != std::string::npos)
    {
        setResponse(501,"501 Not Implemented",false);
        return 0;   
    } 
    if(method == "POST" && it2->second.empty())
    {
        setResponse(400,"400 Bad Request",false);
        return 0;
    }

    const std::string allowedChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~:/?#[]@!$&'()*+,;=%";
    std::size_t found = path.find_first_not_of(allowedChars);
    std::size_t found1 = query.find_first_not_of(allowedChars);
    if((!path.empty() && found != std::string::npos) || (!query.empty() && found1 != std::string::npos))
    {
        setResponse(400,"400 Bad Request",false);
        return 0;
    }
    if(path.length() + query.length() > 2048)
    {
        setResponse(414,"414 Request-URL Too Long",false);
        return 0;
    }

    if(method == "POST" && it1 != headers.end())
    {
         if(it1->second.empty())
        {
            setResponse(400,"400 Bad Request",false);
            return 0;
        }
        size_t t = it1->second.find_first_not_of("0123456789");
        if (t != std::string::npos)
        {
            setResponse(400,"400 Bad Request",false);
            return 0;
        }
        content_len = strtoull(it1->second.c_str(), NULL, 10);
        if(content_len > servers[sindex].getMaxClientBodySize())
        {
            setResponse(413,"413 Request Entity Too Large",false);
            return 0;
        }
    }

    return 1; 
}


bool Client::parseHttpRequestStartLine(const std::string& startLine) 
{
    std::string method,  uri, httpVersion;
    std::istringstream iss(startLine);
    if (!(iss >> method >> uri >> httpVersion))
        return false;
    if(httpVersion != "HTTP/1.1")
        return false;
    return true;
}