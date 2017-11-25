#include "stdafx.h"
#include "SocketSession.h"
#include "DebugOutput.h"

#ifdef _DEBUG 
#define TRY_READ_SIZE 100
#else
#define TRY_READ_SIZE 10240
#endif


CSocketSession::CSocketSession(tcp::socket s, CSessionList & client_list) : socket_(std::move(s)), client_list_(client_list)
{ 
	std::cout << "Client 构造" << std::endl; 
}


CSocketSession::~CSocketSession()
{
	std::cout << "Client 析构" << std::endl;
}
void CSocketSession::DoRead()
{
	OutputDebugStr(L"Server Post Read");
	auto self(shared_from_this());
	//m_unpacker.reset();
	m_unpacker.reserve_buffer(TRY_READ_SIZE);
	socket_.async_read_some(boost::asio::buffer(m_unpacker.buffer(), TRY_READ_SIZE),
		[this, self](const boost::system::error_code & ec, std::size_t nRecvSize)
	{
		if (!ec)
		{
			OutputDebugStr(L"server read :%d", nRecvSize);
			m_unpacker.buffer_consumed(nRecvSize);
			msgpack::object_handle oh;
			if (m_unpacker.next(oh))
			{
				msgpack::sbuffer sBuf;
				ProcessPackObj(oh.get(),&sBuf);
				DoWrite(sBuf.data(), sBuf.size());
			}
			else
			{
				OutputDebugStr(L"半包");
				DoRead();
			}
		}
		else
		{
			//异常断开
			client_list_.Leave(self);
		}
	});
}


void CSocketSession::DoWrite(const char * szBuf, std::size_t s)
{
	OutputDebugStr(L"Server Post Write");
	auto self(shared_from_this());
	boost::asio::async_write(socket_, boost::asio::buffer(szBuf, s), [this, self](const boost::system::error_code& error, std::size_t bytes_transferred)
	{
		if (!error)
		{
			OutputDebugStr(L"OnServerWrite:%d", bytes_transferred);
			CSocketSession::DoRead();
		}
		else
		{
			OutputDebugStr(L"ServerOnWrite Error:%s", error.message().c_str());
			client_list_.Leave(self);
		}
	});
}

bool CSocketSession::ProcessPackObj(const msgpack::object & obj, msgpack::sbuffer * psbuff)
{
	//解包

	return true;
}
