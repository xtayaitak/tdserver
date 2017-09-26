#include "stdafx.h"
#include "TcpServer.h"
#include "ClientSession.h"


CTcpServer::CTcpServer(boost::asio::io_service & io, const tcp::endpoint & endpoint) : acceptor_(io, endpoint), socket_(io)
{
	DoAccept();
}


CTcpServer::~CTcpServer()
{
}

void CTcpServer::DoAccept()
{
	acceptor_.async_accept(socket_, [this](boost::system::error_code ec)
	{
 		if (!ec)
 			std::make_shared<CClientSession>(std::move(socket_),client_list_)->Start();
		DoAccept();
	});
}
