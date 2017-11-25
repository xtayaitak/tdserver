#pragma once

#include <memory>
#include <boost/asio.hpp>
#include "TransMsg.h"
using boost::asio::ip::tcp;

#include "ClientList.h"
#include "msgpack.hpp"

class CSocketSession : public std::enable_shared_from_this<CSocketSession>
{
public:
	CSocketSession(tcp::socket s, CSessionList & client_list);
	~CSocketSession();
	void Start()
	{
		std::string ip = socket_.remote_endpoint().address().to_string();
		client_list_.Join(shared_from_this());
		DoRead();
	}
	virtual bool ProcessPackObj(const msgpack::object & obj, msgpack::sbuffer * psbuff);
	const tcp::socket & GetSocket() const{ return socket_; }
protected:
	void DoRead();
	void DoWrite(const char * szBuf, std::size_t s);
private:
	tcp::socket socket_;
	CSessionList & client_list_;
	msgpack::unpacker m_unpacker;
};


typedef std::shared_ptr<CSocketSession> SessionPtr;