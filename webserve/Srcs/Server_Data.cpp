#include "../Includes/Server_Data.hpp"

Server::Server() {
    root = "";
    host = "";
    server_name = "";
    client_max_body_size = 2147483648;
    rootSet = false;
    maxClientBodySizeSet = false;
    errorPagesSet = false;
    portSet = false;
    serverNameSet = false;
    hostSet = false;
    locationsSet = false;
    sline = "";
}

Server::Server(const Server &copy) {
    *this = copy;
}

Server &Server::operator=(const Server &other)
{
    if (this != &other)
    {
        error_pages.clear();

        root = other.root;
        port_as_int = other.port_as_int;
        error_pages = other.error_pages;
        client_max_body_size = other.client_max_body_size;
        host = other.host;
        server_name = other.server_name;
        sline = other.sline;
        locations = other.locations;

        rootSet = other.rootSet;
        portSet = other.portSet;
        maxClientBodySizeSet = other.maxClientBodySizeSet;
        errorPagesSet = other.errorPagesSet;
        serverNameSet = other.serverNameSet;
        hostSet = other.hostSet;
        maploc = other.maploc;
    }

    return (*this);
}

Server::~Server()
{
}

/****************GETTERS********************/

vecLoc Server::getLocations() const {
    return locations;
}

std::string Server::getHost() const {
    return host;
}

int Server::getPort() const {
    return port_as_int;
}

std::string Server::getServerNames() const {
    return server_name;
}

std::string Server::getRoot() const {
    return root;
}

unsigned long long Server::getMaxClientBodySize() const {
    return client_max_body_size;
}

mapErr Server::getErrorPages() const {
    return error_pages;
}

bool Server::isRootSet() const {
    return rootSet;
}

bool Server::isPortSet() const {
    return portSet;
}

bool Server::isMaxClientBodySet() const {
    return maxClientBodySizeSet;
}

bool Server::isErrorPagesSet() const {
    return errorPagesSet;
}

bool Server::isHostSet() const {
    return hostSet;
}

bool Server::isServerNameSet() const {
    return serverNameSet;
}

bool Server::isLocationsSet() const {
    return locationsSet;
}
/*****************SETTERS********************/

void Server::setLocation(Location loc)
{
    maploc[loc.getLocationPath()] = 1;
    locations.push_back(loc);
}

void Server::setRoot(std::string r) {
    root = r;
}

void Server::setPort(int p) {
    port_as_int = p;
}

void Server::setHost(std::string h) {
    host = h;
}

void Server::setServerName(std::string s) {
    server_name = s;
}

void Server::setMaxClientBodySize(unsigned long long m) {
    client_max_body_size = m;
}

void Server::setErrorPages(int code, std::string page) {
    error_pages[code] = page;
}

void Server::setrootSet(bool b) {
    rootSet = b;
}

void Server::setportSet(bool b) {
    portSet = b;
}

void Server::setmaxClientBodySizeSet(bool b) {
    maxClientBodySizeSet = b;
}

void Server::seterrorPagesSet(bool b) {
    errorPagesSet = b;
}

void Server::sethostSet(bool b) {
    hostSet = b;
}

void Server::setServerNameSet(bool b) {
    serverNameSet = b;
}

void Server::setLocationsSet(bool b) {
    locationsSet = b;
}

std::string Server::getSline() const
{
    return sline;
}

void Server::setSline(std::string s)
{
    if (maploc.size() < locations.size())
    {
        std::cerr << "Error: Duplicated locations specified" << std::endl;
        exit(1);
    }
    sline = s;
}