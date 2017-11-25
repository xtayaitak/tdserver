#include "stdafx.h"
#include "SocketSession.h"
#include "DebugOutput.h"


#include <iostream>


#define TRY_READ_SIZE 10240



CSocketSession::CSocketSession(tcp::socket s, CSessionList & client_list) : socket_(std::move(s)), client_list_(client_list)
{ 
	std::wcout << L"Client 构造" << std::endl; 
	Touch();
}


CSocketSession::~CSocketSession()
{
	std::wcout << L"Client 析构" << std::endl;
}
void CSocketSession::DoRead()
{
	if (m_stop) return;
	auto self(shared_from_this());
	//m_unpacker.reset();
	m_unpacker.reserve_buffer(TRY_READ_SIZE);
	socket_.async_read_some(boost::asio::buffer(m_unpacker.buffer(), TRY_READ_SIZE),
		[this, self](const boost::system::error_code & ec, std::size_t nRecvSize)
	{
		if (!ec && !m_stop)
		{
			m_unpacker.buffer_consumed(nRecvSize);
			msgpack::object_handle oh;
			if (m_unpacker.next(oh))
			{
				msgpack::sbuffer sBuf;
				ProcessPackObj(oh.get(),&sBuf,self);
				DoWrite(sBuf.data(), sBuf.size());
				Touch();
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
			OnClientDisconnect();
			OutputDebugStrA("Server On Read Error:%s", ec.message().c_str());			
			client_list_.Leave(self);
		}
	});
}



void CSocketSession::DoWrite(const char * szBuf, std::size_t s)
{
	auto self(shared_from_this());
	boost::asio::async_write(socket_, boost::asio::buffer(szBuf, s), [this, self](const boost::system::error_code& error, std::size_t bytes_transferred)
	{
		if (!error && !m_stop)
		{
			CSocketSession::DoRead();
		}
		else
		{
			OutputDebugStr(L"ServerOnWrite Error:%s", error.message().c_str());
			OnClientDisconnect();
			client_list_.Leave(self);
		}
	});
}
void CSocketSession::Close()
{
	socket_.close();
}

void CSocketSession::Touch()
{
	m_touch_time = time(NULL);
}