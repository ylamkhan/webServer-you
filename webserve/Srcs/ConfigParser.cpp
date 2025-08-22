#include "../Includes/ConfigParser.hpp"
#include <climits>

ConfigParser::ConfigParser() {}

ConfigParser::ConfigParser(std::string file)
{
    parseConfig(file);
}

ConfigParser &ConfigParser::operator=(const ConfigParser &c)
{
    servers = c.servers;
    return (*this);
}

ConfigParser::~ConfigParser()
{
}

void ConfigParser::parseConfig(std::string filename)
{
    std::ifstream   file(filename.c_str());
    std::vector<std::string> lines;
    int i = 0;

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(1);
    }

    std::string line;
    while(std::getline(file, line))
    {
        size_t pos = line.find_first_not_of(" \t\n");
        if (pos != std::string::npos && line[pos] != '#')
            lines.push_back(line);
        i++;
    }
    if (i == 0 || !lines.size()) {
        std::cerr << "Error: empty file " << filename << std::endl;
        exit(1);
    }

    fillServers(lines);
    file.close();
}

int ConfigParser::duplicate(std::string s)
{
    int count = 0;
    for (size_t i = 0; i < servers.size(); i++)
    {
        if (servers[i].getSline() == s)
            count++;
    }
    if (count > 1)
        return 1;
    return 0;
}

void ConfigParser::checkservers()
{
    for (size_t i = 0; i < servers.size(); i++)
    {
        if (duplicate(servers[i].getSline()))
        {
            std::cerr << "Error: Invalid Config File." << std::endl;
            exit(1);
        }
    }
}

void ConfigParser::fillServers(std::vector<std::string> lines)
{
    size_t i = 0;
    
    while (i < lines.size())
    {
        if (lines[i] == "servers:")
        {
            i++;
            if (lines[i] == "  - listen:")
                fillServer(lines, &i);
            else
            {
                std::cerr << "Invalid Config File!!!" << std::endl;
                exit(1);
            }
        }
        else
        {
            std::cerr << "Invalid Config File!!!" << std::endl;
            exit(1);
        }
        i++;
    }
    checkservers();
}

std::string tof_string(int value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

void ConfigParser::fillServer(std::vector<std::string> &lines, size_t *i)
{
    Server server;

    (*i)++;
    while((*i) < lines.size())
    {
        if (lines[*i] == "    locations:")
            fillLocations(lines, i, server);
        else if (lines[*i] == "  - listen:")
            fillServer(lines, i);
        else
            fillOther(lines, i, server);
        (*i)++;
    }
    if (!server.isPortSet() || !server.isHostSet() || !server.isRootSet())
    {
        std::cerr << "Missing Port, Host or Root value." << std::endl;
        exit(1);
    }
    server.setSline(server.getHost() + tof_string(server.getPort()) + server.getServerNames());
    servers.push_back(server);
}

void ConfigParser::fillLocations(std::vector<std::string> &lines, size_t *i, Server &server)
{
    if (!server.isLocationsSet())
    {
        (*i)++;
        server.setLocationsSet(true);
        while((*i) < lines.size())
        {
            if (lines[*i].substr(0, 13) == "      - path:")
                fillLocation(lines, i, server);
            else
            {
                std::cerr << "Invalid Config Filee" << std::endl;
                exit(1);
            }
            (*i)++;
        }
    }
    else {
        std::cerr << "Error: Multiple locations entry." << std::endl;
        exit(1);
    }
}

void ConfigParser::fillLocation(std::vector<std::string> lines, size_t *i, Server &server)
{
    Location location;

    fillLocPath(location, lines, i);
    (*i)++;
    while((*i) < lines.size())
    {
        if (lines[*i].compare(0, 13, "        root:") == 0)
            saveLocRoot(location, lines, i);
        else if (lines[*i].compare(0, 15, "        upload:") == 0)
            saveUpload(location, lines, i);
        else if (lines[*i].compare(0, 20, "        upload_path:") == 0)
            saveUploadPath(location, lines, i);
        else if (lines[*i].compare(0, 18, "        autoindex:") == 0)
            saveAutoindex(location, lines, i);
        else if (lines[*i].compare(0, 16, "        methods:") == 0)
            saveMethods(location, lines, i);
        else if (lines[*i].compare(0, 14, "        index:") == 0)
            saveIndex(location, lines, i);
        else if (lines[*i].compare(0, lines[*i].size(), "        cgi_paths:") == 0)
            saveCgiPaths(location, lines, i);
        else if (lines[*i].compare(0, 15, "        return:") == 0)
            saveRedirUrl(location, lines, i);
        else if (lines[*i].compare(0, 13, "      - path:") == 0)
            fillLocation(lines, i, server);
        else if (lines[*i].compare(0, lines[*i].size(), "  - listen:") == 0)
            fillServer(lines, i);
        else if (lines[*i] == "    locations:")
            fillLocations(lines, i, server);
        else
        {
            std::cerr << "Invalid Config File$" << std::endl;
            exit(1);
        }
        (*i)++;
    }
    if (!location.isRootSet())
    {
        location.setRoot(server.getRoot());
        location.setrootSet(true);
    }
    if (!location.isPathSet() || !location.isMethodsSet())
    {
        std::cerr << "Missing Path or Methods value." << std::endl;
        exit(1);
    }
    server.setLocation(location);
}

bool isDirectory(const char* path)
{
    struct stat pathStat;
    if (stat(path, &pathStat) == 0)
        return S_ISDIR(pathStat.st_mode);
    else
        return false; 
}

void ConfigParser::saveUploadPath(Location &location, std::vector<std::string> lines, size_t *i)
{
    if (!location.isUpPathSet())
    {
        std::string prefix = "        upload_path:";
        std::string line = lines[*i];
        std::string value;
        value = line.substr(prefix.length());
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        if (value.empty())
        {
            std::cerr << "Error: Invalid uplod_path  value." << std::endl;
            exit(1);
        }
        if (value.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890./_-") != std::string::npos) {
            std::cerr << "Error: Invalid characters detected after the upload_path directory path!" << std::endl;
            exit(1);
        }

        if (value[0] != '.' || (value[0] == '.' && value[1] != '/'))
        {
            std::cerr << "Error: Invalid uplod_path value." << std::endl;
            exit(1);
        }
        if (isDirectory(value.c_str()))
        {
            location.setUploadPath(value);
            location.setUpPathSet(true);
        }
    }
    else {
        std::cerr << "Error: Multiple uplod_path values specified!!!." << std::endl;
        exit(1);
    }
}

void ConfigParser::saveRedirUrl(Location &location, std::vector<std::string> lines, size_t *i) {
    if (!location.isRedirUrlSet())
    {
        std::string prefix = "        return:";
        std::string line = lines[*i];
        std::string value = line.substr(prefix.length());
        
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        if (value.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890./_-:") != std::string::npos) {
            std::cerr << "Error: Invalid characters detected after the reurn url!" << std::endl;
            exit(1);
        }
        location.setRedirUrl(value);
        location.setredirUrlSet(true);
    }
    else {
        std::cerr << "Error: Multiple redirUrl specified." << std::endl;
        exit(1);
    }
}

void ConfigParser::fillLocPath(Location &location, std::vector<std::string> &lines, size_t *i) {
    if (!location.isPathSet())
    {
        std::string prefix = "      - path:";
        std::string line = lines[*i];    

        std::string path = line.substr(prefix.length());

        path.erase(0, path.find_first_not_of(" \t"));
        path.erase(path.find_last_not_of(" \t") + 1);

        if (path.empty()) {
            std::cerr << "Error: Invalid path value." << std::endl;
            exit(1);
        }

        size_t lastQuotePos = path.find_last_of('"');
        size_t lastSpacePos = path.find_last_of(" \t");

        if (lastQuotePos != std::string::npos && lastQuotePos != path.length() - 1) {
            std::cerr << "Error: Additional characters detected after the path." << std::endl;
            exit(1);
        }

        if (lastSpacePos != std::string::npos && lastSpacePos != path.length() - 1) {
            std::cerr << "Error: Additional characters detected after the path." << std::endl;
            exit(1);
        }
        if (path[0] != '/')
            path = "/" + path;
        location.setLocPath(path);
        location.setPathSet(true);
    }
    else {
        std::cerr << "Error: Multiple path entry." << std::endl;
        exit(1);
    }
}



void ConfigParser::saveLocRoot(Location &location, std::vector<std::string> &lines, size_t *i)
{
    if (!location.isRootSet())
    {
        std::string prefix = "        root:";
        std::string line = lines[*i];
        std::string value;
        value = line.substr(prefix.length());
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        if (value.empty())
        {
            std::cerr << "Error: Invalid root value." << std::endl;
            exit(1);
        }
        if (value.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890./_-") != std::string::npos) {
            std::cerr << "Error: Invalid characters detected after the root directory path!" << std::endl;
            exit(1);
        }

        if (value[0] != '.' || (value[0] == '.' && value[1] != '/'))
        {
            std::cerr << "Error: Invalid root value." << std::endl;
            exit(1);
        }
        location.setRoot(value);
        location.setrootSet(true);
    }
    else {
        std::cerr << "Error: Multiple root values specified!!!." << std::endl;
        exit(1);
    }
}

void ConfigParser::saveUpload(Location &location, std::vector<std::string> &lines, size_t *i)
{
    if (!location.isUploadSet())
    {
        std::string prefix = "        upload: ";
        std::string line = lines[*i];

        std::string upload = line.substr(prefix.length());
        std::transform(upload.begin(), upload.end(), upload.begin(), ::toupper);
        if (upload != "ON" && upload != "OFF") {
            std::cerr << "Error: Invalid value for upload. It should be either 'ON' or 'OFF'." << std::endl;
            exit(1);
        }
        location.setUpload(upload);
        location.setUploadSet(true);
    }
    else {
        std::cerr << "Error: Multiple upload entry." << std::endl;
        exit(1);
    }
}

void ConfigParser::saveAutoindex(Location &location, std::vector<std::string> &lines, size_t *i)
{
    if (!location.isAutoIndexSet())
    {
        std::string prefix = "        autoindex:";
        std::string line = lines[*i];
        std::string s;

        s = line.substr(prefix.length());
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        s.erase(0, s.find_first_not_of(" \t"));
        s.erase(s.find_last_not_of(" \t") + 1);
        if (s != "ON" && s != "OFF") {
            std::cerr << "Error: Invalid value for autoindex. It should be either 'ON' or 'OFF'." << std::endl;
            exit(1);
        }
        else if (s == "ON")
        {
            location.setAutoIndex(true);
            location.setautoIndexSet(true);
        }
        else if (s == "OFF")
        {
            location.setAutoIndex(false);
            location.setautoIndexSet(true);
        }
    }
    else {
        std::cerr << "Error: Multiple autoindex specified." << std::endl;
        exit(1);
    }
}

int duplicated(const std::vector<int>& m)
{
    std::map<int, int> countMap;

    for (std::vector<int>::const_iterator it = m.begin(); it != m.end(); ++it)
    {
        countMap[*it]++;
    }

    int c = 0;
    for (std::map<int, int>::const_iterator it = countMap.begin(); it != countMap.end(); ++it)
    {
        if (it->second > 1)
        {
            c++;
        }
    }

    return c;
}

void ConfigParser::saveMethods(Location &location, std::vector<std::string> &lines, size_t *i) {
    if (!location.isMethodsSet())
    {
        std::string prefix = "        methods: [";
        std::string line = lines[*i];
        
        if (line.find(prefix) == 0) {
            size_t closingBracketPos = line.find_last_of("]");
            
            if (closingBracketPos != std::string::npos) {
                std::string methods = line.substr(prefix.length(), closingBracketPos - prefix.length());
                std::string rest = line.substr(closingBracketPos );
                if (!rest.empty() && rest != "]")
                {
                    std::cerr << "Error: Invalid methods entry." << std::endl;
                    exit(1);
                }
                std::istringstream iss(methods);
                std::string method;
                while (std::getline(iss, method, ',')) {
                    method.erase(0, method.find_first_not_of(" \t"));
                    method.erase(method.find_last_not_of(" \t") + 1);

                    std::transform(method.begin(), method.end(), method.begin(), ::toupper);

                    if (method != "POST" && method != "GET" && method != "DELETE") {
                        std::cerr << "Error: Invalid method detected! Methods should be POST, GET, or DELETE." << std::endl;
                        exit(1);
                    }

                    location.setMethods(method);
                    if (method == "GET")
                        location.setIMethods(0);
                    else if (method == "POST")
                        location.setIMethods(1);
                    else if (method == "DELETE")
                        location.setIMethods(2);
                    else {
                        std::cerr << "Error: Invalid method." << std::endl;
                        exit(1);
                    }
                }
                if (location.getMethods().size() > 3 || duplicated(location.getMethods()))
                {
                    std::cerr << "Error: Invalid methods entry." << std::endl;
                    exit(1);
                }
            } else {
                std::cerr << "Error: Missing closing bracket in methods entry." << std::endl;
                exit(1);
            }
        } else {
            std::cerr << "Error: Incorrect format for methods entry." << std::endl;
            exit(1);
        }
        location.setmethodsSet(true);
    }
    else {
        std::cerr << "Error: Multiple methods entry." << std::endl;
        exit(1);
    }
}

void ConfigParser::saveIndex(Location &location, std::vector<std::string> &lines, size_t *i) {
    if (!location.isIndexSet())
    {
        std::string prefix = "        index: [";
        std::string line = lines[*i];
        
        if (line.find(prefix) == 0) {
            size_t closingBracketPos = line.find_last_of("]");
            
            if (closingBracketPos != std::string::npos) {
                std::string indexString = line.substr(prefix.length(), closingBracketPos - prefix.length());
                std::string rest = line.substr(closingBracketPos);
                if (!rest.empty() && rest != "]")
                {
                    std::cerr << "Error: Invalid index entry." << std::endl;
                    exit(1);
                }
                std::istringstream iss(indexString);
                std::string index;
                while (std::getline(iss, index, ',')) {
                    index.erase(0, index.find_first_not_of(" \t"));
                    index.erase(index.find_last_not_of(" \t") + 1);

                    if (index.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./_-") != std::string::npos) {
                        std::cerr << "Error: Invalid characters detected in index file name." << std::endl;
                        exit(1);
                    }

                    location.setIndex(index);
                }
            } else {
                std::cerr << "Error: Missing closing bracket in index entry." << std::endl;
                exit(1);
            }
        } else {
            std::cerr << "Error: Incorrect format for index entry." << std::endl;
            exit(1);
        }
        location.setindexSet(true);
    }
    else {
        std::cerr << "Error: Multiple index entry." << std::endl;
        exit(1);
    }
}

void ConfigParser::saveCgiPaths(Location &location, std::vector<std::string> &lines, size_t *i)
{
    if (!location.isCgiPathsSet())
    {
        std::string prefix = "          - [";
        std::string line = lines[*i];

        (*i)++;
        line = lines[*i];

        while (line.find(prefix) == 0) {
            size_t openingBracketPos = line.find("[");
            size_t closingBracketPos = line.find("]", openingBracketPos);

            if (openingBracketPos != std::string::npos && closingBracketPos != std::string::npos && closingBracketPos > openingBracketPos) {
                std::istringstream iss(line.substr(openingBracketPos + 1, closingBracketPos - openingBracketPos - 1));
                std::string cgiPath, executable;
                char delimiter;
                iss >> delimiter;

                std::getline(iss, cgiPath, ',');
                std::getline(iss, executable, ']');

                cgiPath.erase(0, cgiPath.find_first_not_of(" \t"));
                cgiPath.erase(cgiPath.find_last_not_of(" \t") + 1);
                executable.erase(0, executable.find_first_not_of(" \t"));
                executable.erase(executable.find_last_not_of(" \t") + 1);

                if (cgiPath.empty() || executable.empty() ||
                    cgiPath.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/_-") != std::string::npos ||
                    executable.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/_-") != std::string::npos) {
                    std::cerr << "Error: Invalid characters detected in cgi_path or executable." << std::endl;
                    exit(1);
                }
                
                location.setCgiPaths(std::make_pair(cgiPath, executable));

                size_t afterClosingBracketPos = line.find_first_not_of(" \t", closingBracketPos + 1);
                if (afterClosingBracketPos != std::string::npos) {
                    std::cerr << "Error: Characters found after the closing bracket." << std::endl;
                    exit(1);
                }
                (*i)++;
                if (*i >= lines.size())
                    break;
                line = lines[*i];
                
            } else {
                std::cerr << "Error: Missing or mismatched opening/closing brackets." << std::endl;
                exit(1);
            }
        }
        location.setcgiPathsSet(true);
        (*i)--;
    }
    else {
        std::cerr << "Error: Multiple cgi entry." << std::endl;
        exit(1);
    }
}

void ConfigParser::fillOther(std::vector<std::string> &lines, size_t *i, Server &server)
{
    if (lines[*i].compare(0, 16, "    server_name:") == 0)
        saveName(server, lines, i);
    else if (lines[*i].compare(0, 9, "    port:") == 0)
        savePort(server, lines, i);
    else if (lines[*i].compare(0, 9, "    host:") == 0)
        saveHost(server, lines, i);
    else if (lines[*i].compare(0, 9, "    root:") == 0)
        saveRoot(server, lines, i);
    else if (lines[*i].compare(0, lines[*i].size(), "    error_pages:") == 0)
        saveErrorPages(server, lines, i);
    else if (lines[*i].compare(0, 25, "    client_max_body_size:") == 0)
        saveBodySize(server, lines, i);
    else
    {
        std::cerr << "Invalid Config File." << std::endl;
        exit(1);
    }
}

void ConfigParser::saveName(Server &server, std::vector<std::string> &lines, size_t *i)
{
    if (!server.isServerNameSet())
    {
        std::string prefix = "    server_name:";
        std::string line = lines[*i];

        std::string value = line.substr(prefix.length());
        
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        if (!value.empty() && value.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./_-") != std::string::npos) {
            std::cerr << "Error: Invalid server_name value." << std::endl;
            exit(1);
        }

        server.setServerName(value);
        server.setServerNameSet(true);
    }
    else {
        std::cerr << "Error: Multiple server name values specified." << std::endl;
        exit(1);
    }
}

void ConfigParser::saveBodySize(Server &server, std::vector<std::string> &lines, size_t *i) {
    if (!server.isMaxClientBodySet())
    {
        std::string prefix = "    client_max_body_size:";
        std::string line = lines[*i];

        std::string value = line.substr(prefix.length());

        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        size_t multiplier = 0;
        char suffix = value[value.size()-1];
        if (suffix == 'g' || suffix == 'G') {
            multiplier = 1024 * 1024 * 1024;
        } else if (suffix == 'm' || suffix == 'M') {
            multiplier = 1024 * 1024;
        } else if (suffix == 'k' || suffix == 'K') {
            multiplier = 1024;
        } else if (suffix == 'b' || suffix == 'B') {
            multiplier = 1;
        } else {
            std::cerr << "Error: Invalid suffix in client_max_body_size value." << std::endl;
            exit(1);
        }

        if (multiplier != 0) {
            if (!value.empty()) {
                value.erase(value.size() - 1);
            }
        }

        if (value.find_first_not_of("0123456789") != std::string::npos) {
            std::cerr << "Error: Invalid characters detected in client_max_body_size value." << std::endl;
            exit(1);
        }
        if (strtoull(value.c_str(), NULL, 10) * multiplier > ULLONG_MAX) {
            std::cerr << "Error: Value exceeds the limits of unsigned long long." << std::endl;
            exit(1);
        }
        unsigned long long sizeInBytes = strtoull(value.c_str(), NULL, 10) * multiplier;

        server.setMaxClientBodySize(sizeInBytes);
        server.setmaxClientBodySizeSet(true);
    }
    else {
        std::cerr << "Error: Multiple MaxClientBodySize specified." << std::endl;
        exit(1);
    }
}


void ConfigParser::savePort(Server &server, std::vector<std::string> &lines, size_t *i)
{
    if (!server.isPortSet())
    {
        std::string prefix = "    port:";
        std::string line = lines[*i];

        std::string value = line.substr(prefix.length());
        
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        if (value.empty() || value.find_first_not_of("0123456789") != std::string::npos) {
            std::cerr << "Error: Invalid port value." << std::endl;
            exit(1);
        }

        int p = std::atoi(value.c_str());
        if (p > USHRT_MAX)
        {
            std::cerr << "Value exceeds the limits of unsigned short!" << std::endl;
            exit(1);
        }
        server.setPort(p);
        server.setportSet(true);
    }
    else
    {
            std::cerr << "Error: Multiple port values specified." << std::endl;
            exit(1);
    }
}

void ConfigParser::saveHost(Server &server, std::vector<std::string> &lines, size_t *i)
{
    if (!server.isHostSet())
    {
        std::string prefix = "    host:";
        std::string line = lines[*i];
        std::string value = line.substr(prefix.length());
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        std::transform(value.begin(), value.end(), value.begin(), ::toupper);
        if (value == "LOCALHOST")
            value = "127.0.0.1";
        if (value.empty() || value[value.size()-1] == '.') {
            std::cerr << "Error: Invalid host value." << std::endl;
            exit(1);
        }

        std::istringstream iss(value);
        std::string segment;
        std::vector<std::string> segments;
        while (std::getline(iss, segment, '.')) {
            segments.push_back(segment);
        }

        if (segments.size() != 4) {
            std::cerr << "Error: Invalid host value. Must contain 4 segments separated by dots or localhost." << std::endl;
            exit(1);
        }

        size_t ip_address = 0;
        for (size_t i = 0; i < segments.size(); ++i) {
            const std::string& segment = segments[i];
            if (segment.empty() || segment.find_first_not_of("0123456789") != std::string::npos) {
                std::cerr << "Error: Invalid host value. Each segment must be a number." << std::endl;
                exit(1);
            }
            int num = std::atoi(segment.c_str());
            if (num < 0 || num > 255) {
                std::cerr << "Error: Invalid host value. Each segment must be between 0 and 255." << std::endl;
                exit(1);
            }
            ip_address = (ip_address * 256) + num;
        }

        server.setHost(value);
        server.sethostSet(true);
    }
    else {
        std::cerr << "Error: Multiple host values specified." << std::endl;
        exit(1);
    }
}

void ConfigParser::saveRoot(Server &server, std::vector<std::string> &lines, size_t *i)
{
    if (!server.isRootSet())
    {
        std::string prefix = "    root:";
        std::string line = lines[*i];
        std::string value;
        value = line.substr(prefix.length());
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        if (value.empty())
        {
            std::cerr << "Error: Invalid root value." << std::endl;
            exit(1);
        }
        if (value.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890./_-") != std::string::npos) {
            std::cerr << "Error: Invalid characters detected after the root directory path!" << std::endl;
            exit(1);
        }
        server.setRoot(value);
        server.setrootSet(true);
    }
    else {
        std::cerr << "Error: Multiple root values specified!!!." << std::endl;
        exit(1);
    }
}

void ConfigParser::saveErrorPages(Server &server, std::vector<std::string> &lines, size_t *i) {
    if (!server.isErrorPagesSet())
    {
        std::string prefix = "    error_pages:";
        std::string line = lines[*i];
            
        (*i)++;
        line = lines[*i];

        while (line.find("      -") == 0) {
            std::istringstream iss(line.substr(7));
            std::string code;
            int errorCode;
            std::string pagePath;

            std::getline(iss, code, ':');
            code.erase(0, code.find_first_not_of(" \t"));
            code.erase(code.find_last_not_of(" \t") + 1);
            std::getline(iss, pagePath);
            pagePath.erase(0, pagePath.find_first_not_of(" \t"));
            pagePath.erase(pagePath.find_last_not_of(" \t") + 1);
            if (code.length() != 3 || code.find_first_not_of("0123456789") != std::string::npos)
            {
                std::cerr << "Error: Invalid error code!!" << std::endl;
                exit(1);
            }
            if (pagePath.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890./_") != std::string::npos)
            {
                std::cerr << "Error: Invalid characters detected in the error page path!" << std::endl;
                exit(1);
            }

            errorCode = std::atoi(code.c_str());
            if (errorCode < 100 || errorCode > 999 || pagePath.empty()) {
                std::cerr << "Error: Invalid format for error_pages entry." << std::endl;
                exit(1);
            }
            server.setErrorPages(errorCode, pagePath);
            (*i)++;
            line = lines[*i];
        }
        (*i)--;
        if (server.getErrorPages().size() != 0)
            server.seterrorPagesSet(true);
    }
    else {
        std::cerr << "Error: Multiple error pages specified." << std::endl;
        exit(1);
    }
}

std::vector<Server> &ConfigParser::getServers()
{
    return servers;
}