#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "../Includes/webserver.hpp"

typedef std::vector<std::string>					vecStr;
typedef std::map<std::string, std::string>			mapStr;
typedef std::map<int, std::string>					mapErr;
typedef	std::vector<int>							vecInt;
typedef	std::map<int, int>							mapSock;
typedef std::vector< std::pair<std::string, int> >	vecFiles;
typedef std::pair<std::string, std::string> 		pairStr;
typedef std::vector<std::pair<std::string, std::string> > vecSPair;

class Location {
        private:
                std::string location_path;
                bool auto_index;
                vecStr index;
                std::string root;
                std::string upload;
                std::string list;
                vecStr methods;
                vecInt imethods;
                vecSPair cgi_paths;
                std::string redirUrl;
                std::string upload_path;

                bool rootSet;
                bool indexSet;
                bool methodsSet;
                bool redirUrlSet;
                bool autoIndexSet;
                bool cgiPathsSet;
                bool uploadPathSet;
                bool uploadSet;
                bool pathSet;
                
        public:
                Location();
                Location(const Location &copy);
                Location &operator=(const Location &other);
                ~Location();

                std::string getRoot() const;
                vecStr getIndex() const;
                size_t getMaxClientBodySize() const;
                vecInt getMethods() const;
                std::string getRedirUrl() const;
                bool getAutoIndex() const;
                std::string getUpload() const;
                vecSPair getCgi() const;
                std::string getLocationPath() const;
                std::string getUploadPath() const;

                void setCgiPaths(pairStr p);
                void setRoot(std::string r);
                void setIndex(std::string i);
                void setMethods(std::string method);
                void setIMethods(int i);
                void setAutoIndex(bool b);
                void setRedirUrl(std::string s);
                void setUpload(std::string up);
                void setLocPath(std::string path);
                void setList(std::string l);
                void setUploadPath(std::string s);

                void setrootSet(bool b);
                void setindexSet(bool b);
                void setmethodsSet(bool b);
                void setredirUrlSet(bool b);
                void setautoIndexSet(bool b);
                void setcgiPathsSet(bool b);
                void setUploadSet(bool b);
                void setPathSet(bool b);
                void setUpPathSet(bool b);

                bool isRootSet() const;
                bool isIndexSet() const;
                bool isMethodsSet() const;
                bool isRedirUrlSet() const;
                bool isAutoIndexSet() const;
                bool isCgiPathsSet() const;
                bool isUploadSet() const;
                bool isPathSet() const;
                bool isUpPathSet() const;    
};

#endif