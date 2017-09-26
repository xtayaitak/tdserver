#pragma once


#include "TransMsg.h"
#include <memory>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;

#include "ClientList.h"


class CTcpServer
{
public:
	CTcpServer(boost::asio::io_service & io,const tcp::endpoint & endpoint);
	~CTcpServer();
private:
	void CTcpServer::DoAccept();
	tcp::acceptor acceptor_;
	tcp::socket socket_;
	CClientList client_list_;
};

