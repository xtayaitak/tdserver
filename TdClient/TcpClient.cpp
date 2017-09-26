#include "stdafx.h"
#include "TcpClient.h"
#include "DebugOutput.h"

#include "TransMsg.h"

CTcpClient::CTcpClient(const std::string & ip, short port) : ip_(ip), port_(port), socket_(io_service_)
{
}


CTcpClient::~CTcpClient()
{
}


bool CTcpClient::Connect()
{
	try
	{
		socket_.connect(
			tcp::endpoint(boost::asio::ip::address::from_string(ip_), port_)
			);
		return true;
	}
	catch (boost::system::system_error  &e)
	{
		OutputDebugStrA("connect tcp:%s:%d failed :%s", ip_.c_str(), port_,e.what());
		return false;
	}
}

bool CTcpClient::Send(const char * szBuff, std::size_t nSize)
{
	//Encode Msg
	
	trans_msg_.SetBodyLength(nSize);
	memcpy(trans_msg_.Body(), szBuff, trans_msg_.BodyLength());
	trans_msg_.encode_header();

	//Send
	return RawSend(trans_msg_.Data(), trans_msg_.Length());
}

bool CTcpClient::RawSend(const char * szData, std::size_t nSize, bool with_connect)
{
	try
	{
		boost::asio::write(socket_, boost::asio::buffer(szData, nSize));
		return true;
	}
	catch (boost::system::system_error & e)
	{
		if (e.code().value() == 10009)
		{
			if (with_connect && Connect())
				return RawSend(szData, nSize, false);
		}

		if (e.code().value() == 10054)
		{
			//∂œœﬂ÷ÿ¡∑
			if (with_connect)
			{
				socket_.close();
				if (Connect())
					return RawSend(szData, nSize, false);
			}			
		}
		OutputDebugStrA("CTcpClient::Send exception:%s", e.what());
		return false;	
	}
}
size_t CTcpClient::Recv()
{
	try
	{
		auto head_size = boost::asio::read(socket_, boost::asio::buffer(trans_msg_.Data(), CTransMsg::header_length));
		if (head_size != CTransMsg::header_length)
		{
			OutputDebugStrA("!!!Error:CTcpClient::Recv Head != ReadHead ");
			Close();
			return 0;
		}

		if (!trans_msg_.decode_header())
		{
			OutputDebugStrA("!!!CTcpClient::Recv trans_msg_.decode_header() failed!");
			Close();
			return 0;
		}


		auto body_size = boost::asio::read(socket_, boost::asio::buffer(trans_msg_.Body(), trans_msg_.BodyLength()));
		if (body_size != trans_msg_.BodyLength())
		{
			OutputDebugStrA("!!!CTcpClient::Recv body_size != trans_msg_.BodyLength() ");
			Close();
			return 0;
		}
		return body_size;
	}
	catch (boost::system::system_error & e)
	{
		OutputDebugStrA("CTcpClient::Send exception:%s", e.what());
		Close();
		return 0;
	}
}

const char * CTcpClient::GetData() const
{
	return trans_msg_.Body();
}

void CTcpClient::Close()
{
	//socket_.shutdown(tcp::socket::shutdown_both);
	socket_.close();
}