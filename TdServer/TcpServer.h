#pragma once



#include <memory>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

#include "ClientList.h"


class CTcpServer : public boost::noncopyable
{
public:
	CTcpServer(boost::asio::io_service & io,const tcp::endpoint & endpoint);
	~CTcpServer();
	CSessionList & GetSessionList()  { return client_list_; }
protected:
	void OnHeartbeatTimer(const boost::system::error_code &);
	CTcpServer(const CTcpServer&) = delete;
	CTcpServer& operator=(const CTcpServer&) = delete;
private:
	void CTcpServer::DoAccept();
	tcp::acceptor acceptor_;
	tcp::socket socket_;
	CSessionList client_list_;
	boost::asio::deadline_timer m_heartbeat_timer;


};

