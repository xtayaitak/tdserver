#pragma once

#include "TcpClient.h"

class CTdClient : public CTcpClient
{
public:
	CTdClient(const std::string & ip, short port);
	~CTdClient();

	bool LoginUser(const std::wstring & id,const std::wstring & pwd,const std::wstring & machine_md5);
};

