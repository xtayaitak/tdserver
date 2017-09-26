#pragma once
#include "ClientSession.h"
class CTradeSession :
	public CClientSession
{
public:
	CTradeSession(tcp::socket s, CClientList & client_list);
	~CTradeSession();

	virtual void ProcessRequest(const char * szRequest, std::size_t nRequestSize, std::vector<char> & OutBuffer);

protected:
	std::wstring m_user_id;
	BOOL m_logined = FALSE;
};

