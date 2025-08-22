#include "../Includes/Client.hpp"

void	Client::store_type(void)
{
	mime_type["text/css"] = ".css";
	mime_type["text/xml"] = ".rss";
	mime_type["image/gif"] = ".gif";
	mime_type["image/jpeg"] = ".jpeg";
	mime_type["text/plain"] = ".txt";
	mime_type["text/html"] = ".html";
	mime_type["image/png"] = ".png";
	mime_type["image/x-icon"] = ".ico";
	mime_type["image/x-jng"] = ".jng";
	mime_type["application/pdf"] = ".pdf";	
	mime_type["audio/mpeg"] = ".mp3";
	mime_type["video/mp4"] = ".mp4";
	mime_type["application/octet-stream"] = ".py";
}

void Client::store_type1(void) 
{
    mime_type1[".css"] = "text/css";
    mime_type1[".rss"] = "text/xml";
    mime_type1[".gif"] = "image/gif";
    mime_type1[".jpeg"] = "image/jpeg";
	mime_type1[".jpg"] = "image/jpeg";
    mime_type1[".txt"] = "text/plain";
    mime_type1[".html"] = "text/html";
    mime_type1[".png"] = "image/png";
    mime_type1[".ico"] = "image/x-icon";
    mime_type1[".jng"] = "image/x-jng";
    mime_type1[".pdf"] = "application/pdf";    
    mime_type1[".mp3"] = "audio/mpeg";
    mime_type1[".mp4"] = "video/mp4";
}
