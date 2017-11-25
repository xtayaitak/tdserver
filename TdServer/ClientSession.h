#pragma once
#include "SocketSession.h"
#include <string>
#include <ctime>
#include <boost/date_time/posix_time/posix_time.hpp>


class CClientSession : public CSocketSession
{
public:
	CClientSession(tcp::socket s, CSessionList & client_list);
	~CClientSession();

	virtual bool CClientSession::ProcessPackObj(const msgpack::object & obj, msgpack::sbuffer * psbuff);

	bool OnLoginRequest(const std::wstring & user, const std::wstring & pwd,const std::wstring & mc_info,std::wstring & msg);

	bool IsLogined() const { return m_logined; }
private:
	bool m_logined = false;
	boost::posix_time::ptime m_expite_time;

};

