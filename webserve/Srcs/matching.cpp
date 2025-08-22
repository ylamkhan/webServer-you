#include "../Includes/Client.hpp"

bool ft_isFile(const std::string& path)
{
    struct stat fileInfo;
    if (stat(path.c_str(), &fileInfo) != 0)
        return false;
    return S_ISREG(fileInfo.st_mode);
}

void Client::parsePath()
{
    std::vector<std::string> segments;
    std::istringstream iss(reqURL);
    std::string segment;
    while (std::getline(iss, segment, '/')) {
        segments.push_back(segment);
    }

    std::string currentPath = segments[0];
    for (std::vector<std::string>::iterator it = segments.begin()+1; it != segments.end(); ++it) {
        currentPath += "/" + *it;
        if (ft_isFile(currentPath))
        {
            reqURL = currentPath;
            if (currentPath.size() < path.size())
                path_info = path.substr(currentPath.size());
            return;
        }
    }
}

int Client::matching_servers()
{
    std::string tmp = path;
    int matched = 0;
    
    if (tmp[tmp.size()-1] == '/')
        tmp.erase(tmp.size()-1);
    if (path.empty())
        tmp = "/";
    if (servers[sindex].getLocations().size() == 0)
        return 0;
    while (!matched)
    {
        for (size_t j = 0; j < servers[sindex].getLocations().size(); j++)
        {
            std::string p = servers[sindex].getLocations()[j].getLocationPath();
            if (p == tmp)
            {
                std::string root = servers[sindex].getLocations()[j].getRoot();
                if (root[root.size()-1] == '/')
                    root.erase(root.size()-1);
                if (tmp == "/")
                    tmp = "";
                reqURL = root + path.substr(tmp.size());
                parsePath();
                matched = 1;
                lindex = j;
                return 1;
            }
            if (tmp == "/" && j == servers[sindex].getLocations().size() - 1)
                matched = 1;
        }
        tmp = tmp.substr(0, tmp.find_last_of("/"));
        if (tmp == "")
            tmp = "/";
    }
    return 0;
}

