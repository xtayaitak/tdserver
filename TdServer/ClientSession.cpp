#include "stdafx.h"
#include "ClientSession.h"


CClientSession::~CClientSession()
{
	std::cout << "Client 析构" << std::endl;
}

void CClientSession::DoReadHeader()
{
	auto self(shared_from_this());
	boost::asio::async_read(socket_, boost::asio::buffer(msg_.Data(), CTransMsg::header_length), [this, self](boost::system::error_code ec, std::size_t nRecvSize)
	{
		if (!ec && msg_.decode_header())
		{
			DoReadBody();
		}
		else
		{
			//异常断开
			client_list_.Leave(self);
		}
	});
}




void CClientSession::DoReadBody()
{
	auto self(shared_from_this());
	boost::asio::async_read(socket_, boost::asio::buffer(msg_.Body(), msg_.BodyLength()), [this, self](boost::system::error_code ec, std::size_t)
	{
		if (!ec)
		{
			std::vector<char> response;
			ProcessRequest(msg_.Body(), msg_.BodyLength(), response);
			DoWrite(response.data(), response.size());
		}
		else
		{
			//异常断开
			client_list_.Leave(self);
		}
	});
}


void CClientSession::EncodeWriteData(BYTE * szBuff, size_t nSize)
{
	msg_.SetBodyLength(nSize);
	memcpy(msg_.Body(), szBuff, nSize);
	msg_.encode_header();

}

void CClientSession::DoWrite(const char * szBuff, std::size_t nSize)
{
	auto self(shared_from_this());
	boost::asio::async_write(socket_, boost::asio::buffer(msg_.Data(), msg_.Length()), [this, self](boost::system::error_code ec, std::size_t)
	{
		if (!ec)
			DoReadHeader();
		else
			client_list_.Leave(self);
	});
}




void CClientSession::ProcessRequest(const char * szRequest, std::size_t nRequestSize, std::vector<char> & OutBuffer)
{

}