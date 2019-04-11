/*
 * HttpRequest.cpp
 *
 * COPYRIGHT (C) 2019 PokemonGO TEAM
 * pokemongo.icu
 */

#include "HttpRequest.h"

using std::string;

void HttpRequest::m_parse_url(char *m_url, string &server_name, string &filepath, string &filename)
{
	string::size_type n;
	string url = m_url;

	if (url.substr(0, 7) == "http://")
		url.erase(0, 7);

	if (url.substr(0, 8) == "https://")
		url.erase(0, 8);

	n = url.find('/');
	if (n != string::npos)
	{
		server_name = url.substr(0, n);
		filepath = url.substr(n);
		n = filepath.rfind('/');
		filename = filepath.substr(n + 1);
	}

	else
	{
		server_name = url;
		filepath = "/";
		filename = "";
	}
}

SOCKET HttpRequest::connect_to_server(char *sz_server_name, WORD port_num)
{
	struct hostent *hp;
	unsigned int addr;
	struct sockaddr_in server;

	SOCKET conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (conn == INVALID_SOCKET)
		return NULL;

	if (inet_addr(sz_server_name) == INADDR_NONE)
	{
		hp = gethostbyname(sz_server_name);
	}
	else
	{
		addr = inet_addr(sz_server_name);
		hp = gethostbyaddr(reinterpret_cast<char*>(&addr), sizeof(addr), AF_INET);
	}

	if (hp == nullptr)
	{
		closesocket(conn);
		return NULL;
	}

	server.sin_addr.s_addr = *reinterpret_cast<unsigned long*>(hp->h_addr);
	server.sin_family = AF_INET;
	server.sin_port = htons(port_num);
	if (connect(conn, reinterpret_cast<struct sockaddr*>(&server), sizeof(server)))
	{
		closesocket(conn);
		return NULL;
	}
	return conn;
}

size_t HttpRequest::get_header_length(char *content)
{
	const auto srch_str1 = "\r\n\r\n";
	const auto srchStr2 = "\n\r\n\r";
	size_t ofset = -1;

	auto find_pos = strstr(content, srch_str1);
	if (find_pos != nullptr)
	{
		ofset = find_pos - content;
		ofset += strlen(srch_str1);
	}

	else
	{
		find_pos = strstr(content, srchStr2);
		if (find_pos != nullptr)
		{
			ofset = find_pos - content;
			ofset += strlen(srchStr2);
		}
	}
	return ofset;
}

char* HttpRequest::read_url(char *sz_url)
{
	const int bufSize = 512;
	char readBuffer[bufSize], sendBuffer[bufSize], tmpBuffer[bufSize];
	char *tmpResult = nullptr;
	SOCKET conn;
	string server, filepath, filename;

	m_parse_url(sz_url, server, filepath, filename);

	///////////// step 1, connect //////////////////////
	conn = connect_to_server((char*)server.c_str(), 80);

	if(conn == NULL)
	{
		return nullptr;
	}

	///////////// step 2, send GET request /////////////
	sprintf(tmpBuffer, "GET %s HTTP/1.0", filepath.c_str());
	strcpy(sendBuffer, tmpBuffer);
	strcat(sendBuffer, "\r\n");
	sprintf(tmpBuffer, "Host: %s", server.c_str());
	strcat(sendBuffer, tmpBuffer);
	strcat(sendBuffer, "\r\n");
	strcat(sendBuffer, "\r\n");
	send(conn, sendBuffer, static_cast<int>(strlen(sendBuffer)), 0);

	//    SetWindowText(edit3Hwnd, sendBuffer);
	printf("Buffer being sent:\n%s", sendBuffer);

	///////////// step 3 - get received bytes ////////////////
	// Receive until the peer closes the connection
	long total_bytes_read = 0;
	while (1)
	{
		memset(readBuffer, 0, bufSize);
		const long this_read_size = recv(conn, readBuffer, bufSize, 0);

		if (this_read_size <= 0)
			break;

		tmpResult = static_cast<char*>(realloc(tmpResult, this_read_size + total_bytes_read));

		memcpy(tmpResult + total_bytes_read, readBuffer, this_read_size);
		total_bytes_read += this_read_size;
	}

	const auto header_len = get_header_length(tmpResult);
	const auto conten_len = total_bytes_read - header_len;
	const auto result = new char[conten_len + 1];
	memcpy(result, tmpResult + header_len, conten_len);
	result[conten_len] = 0x0;

	closesocket(conn);
	return(result);
}