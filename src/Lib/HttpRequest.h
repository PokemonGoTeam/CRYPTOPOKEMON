#pragma once
#include <windows.h>
#include <string>
#pragma comment(lib,"ws2_32.lib")

class HttpRequest
{
private:
	static void m_parse_url(char *m_url, std::string &server_name, std::string &filepath, std::string &filename);
	static SOCKET connect_to_server(char *sz_server_name, WORD port_num);
	static size_t get_header_length(char *content);
public:
	static char* read_url(char *sz_url);
};

