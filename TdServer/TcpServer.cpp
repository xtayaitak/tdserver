#include "stdafx.h"
#include "TcpServer.h"
#include "ClientSession.h"
#include <boost/bind.hpp>

#define HEART_BEAT_TIME 5

CTcpServer::CTcpServer(boost::asio::io_service & io, const tcp::endpoint & endpoint) : acceptor_(io, endpoint, false), socket_(io), m_heartbeat_timer(io)
{
	DoAccept();
	m_heartbeat_timer.expires_from_now(boost::posix_time::seconds(HEART_BEAT_TIME));
	m_heartbeat_timer.async_wait(boost::bind(&CTcpServer::OnHeartbeatTimer, this, boost::asio::placeholders::error));
}

void CTcpServer::OnHeartbeatTimer(const boost::system::error_code & err)
{
	client_list_.CheckSessionTimeOut();

	m_heartbeat_timer.expires_from_now(boost::posix_time::seconds(HEART_BEAT_TIME));
	m_heartbeat_timer.async_wait(boost::bind(&CTcpServer::OnHeartbeatTimer, this, boost::asio::placeholders::error));
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
