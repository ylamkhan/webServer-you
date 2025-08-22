#include "../Includes/Client.hpp"

void Client::remove_directory_file(const std::string& name)
{
    DIR *dir;
    struct dirent *entry;

    dir = opendir(name.c_str());

    if (dir != NULL)
    {
      
            while ((entry = readdir(dir)) != NULL)
            {
                std::string entry_name = entry->d_name;
                if (entry_name != "." && entry_name != "..")
                {
                std::string full_path = name + "/" + entry_name;
                DIR *sous_directory = opendir(full_path.c_str());
                    if (sous_directory != NULL) 
                    {
                        closedir(sous_directory);
                        remove_directory_file(full_path);
                    }
                    else
                    {
                        std::ifstream file(full_path.c_str());
                        if (file.is_open())
                        {
                            file.close();
                            struct stat  fileStat;
                            if(stat(full_path.c_str(), &fileStat) == 0) 
                            {
                                if(fileStat.st_mode & S_IWUSR)
                                {
                                    if (std::remove(full_path.c_str()) == 0)
                                        setResponse(204,"204 NO Content",false);
                                }
                                else
                                    setResponse(403, "403 Forbidden",false);
                            }
                        }
                    }
                }
            }

            closedir(dir);
            
            if (rmdir(name.c_str()) == 0)
            {
                setResponse(204,"204 NO Content",true);
                return;
            }
            else 
            {
                setResponse(500,"500 Internal Server Error",true);
                return; 
            }

    }
    
    else 
    {  
        std::ifstream file(name.c_str());
        size_t r = name.rfind("/");
        std::string parent;
        struct stat  fileStat;
        if(r != std::string::npos)
        {
            parent = name.substr(0, r);
            if(stat(parent.c_str(), &fileStat) == 0) 
            {

                if(!(fileStat.st_mode & S_IWUSR))
                {
                    setResponse(403,"403 Forbidden",true);
                    return;

                }
            }
        }
        if (file.is_open()) 
        {
            file.close();
            struct stat  fileStat;
            if(stat(name.c_str(), &fileStat) == 0) 
            {
                if(fileStat.st_mode & S_IWUSR)
                {
                    if (std::remove(name.c_str()) == 0)
                    {
                        setResponse(204,"204 NO Content",true);
                        return;
                    }
                }
                else 
                {
                    setResponse(403, "403 Forbiden",true);
                    return ;

                }
                
            }
        }
    }        
}

int Client::isPathAllowed(const char* path, const char* allowedDirectory) 
{
   
    Location loc = servers[sindex].getLocations()[lindex];
    std::string root =  loc.getRoot();
    if(root[root.size()-1])
        root.erase(root.size()-1);
    std::string aa(allowedDirectory);
    aa += root.substr(1);
    std::string paths(path);
    int isAllowed = paths.compare(0, aa.size(), aa, 0, aa.size());
    return isAllowed;
}

void Client::web_delete()
{ 
    char currentDir[PATH_MAX];
    if (getcwd(currentDir, sizeof(currentDir)) != NULL)
    {
         url = reqURL;
        char resolved_path[PATH_MAX];
        realpath(url.c_str(), resolved_path);
        if (isPathAllowed(resolved_path, currentDir) == 0 )
            url = resolved_path;
        else
        {
            setResponse(403, "403 Forbiden",true);
            return;
        }
        if (!can_open)
        {
            dir = opendir(url.c_str());

            if (dir != NULL)
            {
                can_open = true;
            }
            else
            {
                a_file.open(url.c_str(), std::ios::in | std::ios::binary);
                if (a_file.is_open())
                {
                    can_open = true;
                    a_file.close();
                }
            }
            if (!can_open)
            {
                setResponse(404, "404 Not Found",true);
                return ;
            }

        }
        struct stat fileStat;
        if(stat(url.c_str(), &fileStat)==0) 
        {
            if(!(fileStat.st_mode & S_IWUSR))
            {
                setResponse(403, "403 Forbidden",true);
                return;
            }
        }
        remove_directory_file(url);
    }
    else 
    {
        setResponse(500, "500 Internal Server Error", true);
        return;
    }
  
}