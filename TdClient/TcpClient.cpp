#include "stdafx.h"
#include "TcpClient.h"
#include "DebugOutput.h"

#include "TransMsg.h"

#define MAX_TRY_RECV_BUFF 1024



CTcpClient::CTcpClient(const std::string & ip, short port) : m_ip(ip), m_port(port), m_socket(m_io_service)
{
	
}


CTcpClient::~CTcpClient()
{
}


bool CTcpClient::Connect()
{
	try
	{
		m_socket.connect(
			tcp::endpoint(boost::asio::ip::address::from_string(m_ip), m_port)
			);
		return true;
	}
	catch (boost::system::system_error  &e)
	{
		OutputDebugStrA("connect tcp:%s:%d failed :%s", m_ip.c_str(), m_port,e.what());
		return false;
	}
}

bool CTcpClient::Send(const char * szBuff, std::size_t nSize)
{
	return RawSend(szBuff, nSize);
}


bool CTcpClient::RawSend(const char * szData, std::size_t nSize, bool with_connect)
{
	try
	{
		boost::asio::write(m_socket, boost::asio::buffer(szData, nSize));
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
				m_socket.close();
				if (Connect())
					return RawSend(szData, nSize, false);
			}			
		}
		OutputDebugStrA("CTcpClient::Send exception:%s", e.what());
		return false;	
	}
}
bool CTcpClient::Recv(msgpack::object_handle & result)
{
	try
	{
		unpacker.reserve_buffer(MAX_TRY_RECV_BUFF);
		while (true)
		{
			std::size_t actual_read_size = m_socket.read_some(boost::asio::buffer(unpacker.buffer(), MAX_TRY_RECV_BUFF));
			unpacker.buffer_consumed(actual_read_size);
			if (unpacker.next(result))
			{
				if (unpacker.nonparsed_size() == 0)
					return true;
				else
				{
					OutputDebugStr(L"muti bag?");
					return false;
				}
			}
		}
	}
	catch (boost::system::system_error & e)
	{
		OutputDebugStrA("CTcpClient::Send exception:%s", e.what());
		Close();
		return false;
	}
}


void CTcpClient::Close()
{
	//socket_.shutdown(tcp::socket::shutdown_both);
	m_socket.close();
}