#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#define TIMECGI 10
#define TIMEREC 12
#include <map>
#include <iostream>
#include <vector>
#include <fstream>
#include <sys/stat.h>
#include <map>
#include <fstream>
#include <string>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include<dirent.h>
#include <filesystem>
#include "../Includes/Server_Data.hpp"
#include "../Includes/ConfigParser.hpp"
#include <sys/stat.h>
#include <ctime>



static int ich = 0;
class Socket;

class Client {
    private:
        int sockfd;
        std::ofstream file;
        std::ifstream tmpfile;
        std::ofstream listfile;
        std::string tmpfilename;
        std::ifstream a_file;
        std::string url;
        DIR *dir;


        std::vector<Server> servers;
        std::string cookies;
        std::string redirection;

        bool flag_open;
        bool flag_in_out;
        bool headerSet;
        bool Opened;
        bool isDir;
        bool isFile;
        bool startRead;
        bool cgiflag;
        bool flagResponse;
        bool timeout;
        bool flag_hand_method;
        bool openread;

        std::string name_path;
        std::string result_file;
        std::string cgiUrl;
        std::string getUrl;
        std::string type;
        std::string requesta;
        std::string method;
        std::string path;
        std::string query;
        std::string httpVersion;
        std::map<std::string, std::string>headers;
        std::map<std::string, std::string>mime_type;
        std::map<std::string, std::string>mime_type1;
        std::string body;
        std::string requestStr;
        unsigned long long content_len;
        unsigned long long bodySize;
        size_t hexSize;
        size_t store_hexSize;
        size_t sindex;
        size_t lindex;
        std::string executable;
        size_t status;
        clock_t startTimeReq;
        clock_t startTimeCgi;
        pid_t pid;

        int port;
        std::string host;
        std::string URL;
        std::string value_type;
        size_t chunks_size;
        std::string chunks;
        std::string transfer_encoding;
        std::string ss;
        std::string save;
        std::string store;
        std::string shinka;
        std::string sab;
        std::string reqURL;
        std::string message;
        std::string statCgi;
        bool listing;
        bool closed;
        bool can_open;
        bool isCgi;
        std::string headersCgi;
        int Permissions;
        std::string content_type;
        bool fcgi;
        std::string name_up_file;
        bool terminated;
        std::string path_info;
        int size_cha;


    public:
        Client();
        Client(std::vector<Server> &servers);
        ~Client();
        Client &operator=(Client const &other);
        Client(Client const &other);
        
        void ft_chunked();
        void parsePath();
        int handleRealPath(std::string s);
        std::string to_string(int value);
        void handleFile();
        void handleDirOff();
        void    sendGet();
        void    ft_binary();
        void    cgiOpen();
        void    setResponse(size_t s, std::string m, bool b);
        int     check_me();
        void    ft_read(int error_code, std::string ppppath);
        void    readDefault();
        int    readPage(std::string p);
        void    sendPost();
        void    sendDelete();
        bool    get_flag_in_out() const;
        void    cgi(std::string u);
        int     getSocket() const;
        void    setFd(int fd);
        int    open_file();
        void    setstartTimeReq(clock_t t);
        clock_t getstartTimeReq() const;
        std::string get_file_name() const ;
        void    parseRequest(const std::string& httpRequest);
        void    set_request_client(std::string requ);
        std::string getMethod() const;
        std::string getPath() const;
        std::string getHTTPVersion() const;
        std::map<std::string, std::string> getHeaders() const;
        std::string getBody() const;
        std::string getReqStr() const;
        std::string getMessage() const;
        size_t getStatus() const;
        std::string getCgiUrl() const;
        void update_servers();
        void set_flag_in_out(bool in_out);
        bool getfcgi() const;
        
        void    setReqStr(std::string s);
        void    handl_methodes();
        int     matching_servers();
        void    setPortHost(std::string headerValue);
        void	store_type(void);
        void    setSindex(size_t i);
        int     mattching(std::string url, std::string pathloc);
        unsigned int hexa_to_dec(const std::string& str);
        std::string& ltrim(std::string& str);
        int checkforCgi(std::string type);


        void post();
        void store_type1();
        void get();
        int get_min();
        int get_min1(Location &loc);
        int get_min2(Location &loc);

        void web_delete();
        void setStatus(size_t t);
        void setmessage(std::string me);
        std::string toString();
        int checkMethod();
        int is_req_well_formed();
        bool RisCgi();

        void    listDir();
        bool getflagResponse();
        void setflagResponse(bool t);
        std::ifstream &get_a_file();
        void send_client();
        bool getClosed() const;
        void readFile(std::string file);
        void remove_directory_file(const std::string& name);
        std::string parseFile(std::string f);
        bool isDirectory(const char* path);
        void parse_header(std::string headersCgi);
        void    ft_open(std::string opath);
       bool parseHttpRequestStartLine(const std::string& startLine);
        void setIsCgi(bool a);
        pid_t getpid() const;
        int isPathAllowed(const char* path, const char* allowedDirectory);
        int parse_Host();

};

#endif
