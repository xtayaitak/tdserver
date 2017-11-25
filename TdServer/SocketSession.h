#pragma once

#include <memory>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;

#include "ClientList.h"
#include "msgpack.hpp"
#include "GameRoleID.h"
class CSocketSession : public std::enable_shared_from_this<CSocketSession>
{
public:
	CSocketSession(tcp::socket s, CSessionList & client_list);
	~CSocketSession();
	void Start()
	{
		m_ip = socket_.remote_endpoint().address().to_string() + ":" + std::to_string(socket_.remote_endpoint().port());
		DoRead();
	}
	virtual void ProcessPackObj(const msgpack::object & obj, msgpack::sbuffer * psbuff, SessionPtr sess_ptr) = 0;
	const tcp::socket & GetSocket() const{ return socket_; }
	virtual const CGameRoleID & GetGameRoleID() const = 0;
	virtual const std::wstring & GetClientID() const = 0;
	bool IsTimeOut() const { return time(NULL) - m_touch_time > 300; }
	std::string GetIp() const { return m_ip; }
	void Close();
	const time_t GetTouchTime() const { return m_touch_time; }
protected:
	void DoRead();
	void DoWrite(const char * szBuf, std::size_t s);
	virtual void OnClientDisconnect() = 0;
	CSessionList & client_list_;
	bool m_stop = false;
private:
	void Touch();
	tcp::socket socket_;
	msgpack::unpacker m_unpacker;
	time_t m_touch_time;//上次通信时间;
	std::string m_ip;
};


typedef std::shared_ptr<CSocketSession> SessionPtr;