#include "../Includes/webserver.hpp"

Location::Location() {
    upload_path = "";
    root = "";
    redirUrl = "";
    location_path = "";
    list = "OFF";
    upload = "OFF";
    auto_index = 0;
    rootSet = false;
    indexSet = false;
    methodsSet = false;
    redirUrlSet = false;
    autoIndexSet = false;
    cgiPathsSet = false;
    uploadPathSet = false;
    uploadSet = false;
    pathSet = false;
}

Location::Location(const Location &copy) {
    *this = copy;
}

Location &Location::operator=(const Location &other)
{
    if (this != &other)
    {
        index.clear();
        imethods.clear();
        methods.clear();
        cgi_paths.clear();

        root = other.root;
        location_path = other.location_path;
        auto_index = other.auto_index;
        upload = other.upload;
        methods = other.methods;
        imethods = other.imethods;
        cgi_paths = other.cgi_paths;
        redirUrl = other.redirUrl;
        index = other.index;
        upload_path = other.upload_path;

        rootSet = other.rootSet;
        indexSet = other.indexSet;
        methodsSet = other.methodsSet;
        redirUrlSet = other.redirUrlSet;
        autoIndexSet = other.autoIndexSet;
        uploadPathSet = other.uploadPathSet;
        uploadSet = other.uploadSet;
    }

    return (*this);
}

Location::~Location()
{
}

/****************GETTERS********************/

std::string Location::getLocationPath() const {
    return location_path;
}

vecSPair Location::getCgi() const {
    return cgi_paths;
}

std::string Location::getUpload() const {
    return upload;
}

std::string Location::getRoot() const {
    return root;
}

std::string Location::getUploadPath() const {
    return upload_path;
}

vecStr Location::getIndex() const {
    return index;
}

vecInt Location::getMethods() const {
    return imethods;
}

std::string Location::getRedirUrl() const {
    return redirUrl;
}

bool Location::getAutoIndex() const {
    return auto_index;
}

bool Location::isRootSet() const {
    return rootSet;
}

bool Location::isIndexSet() const {
    return indexSet;
}

bool Location::isMethodsSet() const {
    return methodsSet;
}

bool Location::isAutoIndexSet() const {
    return autoIndexSet;
}

bool Location::isCgiPathsSet() const {
    return cgiPathsSet;
}

bool Location::isUploadSet() const {
    return uploadSet;
}

bool Location::isPathSet() const {
    return pathSet;
}

bool Location::isUpPathSet() const {
    return uploadPathSet;
}

/*****************SETTERS********************/

void Location::setLocPath(std::string path) {
    if (!path.empty())
    {
        if (path[path.size()-1] == '/' && path != "/")
            path.erase(path.size()-1);
    }
    location_path = path;
}

void Location::setList(std::string l) {
    list = l;
}

void Location::setCgiPaths(pairStr p) {
    cgi_paths.push_back(p);
}

void Location::setRoot(std::string r) {
    root = r;
}

void Location::setIndex(std::string i) {
    index.push_back(i);
}

void Location::setMethods(std::string method) {
    methods.push_back(method);
}

void Location::setIMethods(int i) {
    imethods.push_back(i);
}

void Location::setUploadPath(std::string s)
{
    upload_path = s;
}

void Location::setAutoIndex(bool b) {
    auto_index = b;
}

void Location::setRedirUrl(std::string s) {
    redirUrl = s;
}

void Location::setUpload(std::string up) {
    upload = up;
}

void Location::setrootSet(bool b) {
    rootSet = b;
}

void Location::setUpPathSet(bool b) {
    uploadPathSet = b;
}

void Location::setindexSet(bool b) {
    indexSet = b;
}

void Location::setmethodsSet(bool b) {
    methodsSet = b;
}

void Location::setredirUrlSet(bool b) {
    redirUrlSet = b;
}

void Location::setautoIndexSet(bool b) {
    autoIndexSet = b;
}

void Location::setcgiPathsSet(bool b) {
    cgiPathsSet = b;
}

void Location::setUploadSet(bool b) {
    uploadSet = b;
}

void Location::setPathSet(bool b) {
    pathSet = b;
}

bool Location::isRedirUrlSet() const {
    return redirUrlSet;
}