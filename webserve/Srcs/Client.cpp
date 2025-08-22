#include <iostream>
#include <string>
#include <cctype>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include "../Includes/Client.hpp"
#include<signal.h>


Client::Client(){}
std::string Client::to_string(int value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}
void Client::setIsCgi(bool a){
    isCgi = a;
}

Client::Client(std::vector<Server> &servers):servers(servers)
{
    startTimeReq = clock();
    openread = false;
    redirection = "";
    flag_hand_method = false;
    terminated = false;
    timeout = false;
    headerSet = false;
    sindex = 0;
    lindex = 0;
    Permissions = 0;
    flag_in_out = false;
    isCgi = false;
    cgiflag = false;
    flag_open = false;
    Opened = false;
    isDir = false;
    isFile = false;
    startRead = false;
    bodySize = 0;
    flagResponse = false;
    listing = false;
    closed = false;
    executable = "";
    query = "";
    message = "";
    type = "";
    cgiUrl = "";
    getUrl = "";
    reqURL = "";
    url = "";
    requestStr = "";
    requesta = "";
    body = "";
    path = "";
    status = 0;
    can_open = false;
    fcgi = false;
    name_up_file = "";
    result_file = "";
    content_type = "text/html";
    cookies = "";
    statCgi = "";
    path_info = "";
    size_cha = 0;

}

Client::~Client()
{
}

Client &Client::operator=(Client const &other)
{
    startTimeCgi = other.startTimeCgi;
    size_cha = other.size_cha;
    openread = other.openread;
    path_info = other.path_info;
    redirection = other.redirection;
    statCgi = other.statCgi;
    result_file = other.result_file;
    flag_hand_method = other.flag_hand_method;
    terminated = other.terminated;
    fcgi = other.fcgi;
    name_up_file = other.name_up_file;
    startTimeReq = other.startTimeReq;
    timeout = other.timeout;
    content_type = other.content_type;
    cookies = other.cookies;
    reqURL = other.reqURL;
    isCgi = other.isCgi;
    closed = other.closed;
    listing = other.listing;
    flagResponse = other.flagResponse;
    getUrl = other.getUrl;
    cgiUrl = other.cgiUrl;
    cgiflag = other.cgiflag;
    type = other.type;
    startRead = other.startRead;
    isFile = other.isFile;
    isDir = other.isDir;
    Opened = other.Opened;
    query = other.query;
    executable = other.executable;
    flag_open = other.flag_open;
    sockfd = other.sockfd;
    flag_in_out = other.flag_in_out;
    name_path = other.name_path;
    requesta = other.requesta;
    method = other.method;
    path = other.path;
    httpVersion = other.httpVersion;
    headers = other.headers; 
    mime_type = other.mime_type; 
    body = other.body;
    requestStr = other.requestStr;
    headerSet = other.headerSet;
    content_len = other.content_len;
    bodySize = other.bodySize;
    servers = other.servers;
    value_type = other.value_type;
    transfer_encoding = other.transfer_encoding;
    store = other.store; 
    save = other.save;
    shinka = other.shinka;
    sab = other.sab;
    ss = other.ss;
    sindex = other.sindex;
    lindex = other.lindex;
    message = other.message;
    status = other.status;
    Permissions = other.Permissions;
    pid = other.pid;
    can_open = other.can_open;
    return *this;
}

bool Client::RisCgi()
{
    return isCgi;
}

Client::Client(Client const &other)
{
    *this = other;
}

int Client::getSocket() const {
    return sockfd;
}

bool Client::getfcgi() const
{
    return fcgi;
}

std::string Client::get_file_name() const 
{
    return name_path;
}

void Client::set_request_client(std::string requ)
{
    parseRequest(requ);
}

void Client::set_flag_in_out(bool in_out)
{
    flag_in_out = in_out;
}

void Client::setPortHost(std::string headerValue)
{
    host = headerValue;
}

int Client::mattching(std::string url, std::string pathloc)
{
    if (pathloc[0] != '/')
        pathloc = "/" + pathloc;
    if (url == pathloc)
        return 1;
    return 0;
}

void   Client::handl_methodes()
{
    if (method == "GET")
        get();
    else if (method == "POST")
        post();
    else if (method == "DELETE")
        web_delete();
    else
    {
        setResponse(501,"501 Not Implemented",true);
        return ;
    }
}

void Client::setFd(int fd) {
    sockfd = fd;
}

void    Client::setSindex(size_t i) {
    sindex = i;
}

bool Client::get_flag_in_out() const{
    return flag_in_out;
}

std::string Client::getMethod() const { 
    return method;
}

std::string Client::getPath() const {
    return path;
}

std::string Client::getHTTPVersion() const {
    return httpVersion;
}

std::map<std::string, std::string> Client::getHeaders() const {
    return headers;
}

std::string Client::getBody() const {
    return body;
}


std::string Client::getReqStr() const {
    return requestStr;
}

void Client::setReqStr(std::string s) {
    requestStr += s;
}

std::string Client::getMessage() const {
    return message;
}

size_t Client::getStatus() const {
    return status;
}

bool Client::getflagResponse() 
{
    return flagResponse;
}
std::ifstream &Client::get_a_file()
{
    return a_file;
}

bool Client::getClosed() const {
    return closed;
}

pid_t Client::getpid() const
{
    return pid;
}

std::string Client::getCgiUrl() const {
    return cgiUrl;
}

void Client::setflagResponse(bool t)
{
    flagResponse = t;
}

void Client::setstartTimeReq(clock_t t)
{
    startTimeReq = t;
}

clock_t Client::getstartTimeReq() const
{
    return startTimeReq;
}


void Client::setStatus(size_t t)
{
    status = t;
}

void Client::setmessage(std::string me)
{
    message = me;
}
