#include "stdafx.h"
#include "TcpClient.h"
#include "DebugOutput.h"



#define MAX_TRY_RECV_BUFF 1024


CTcpClient::CTcpClient() : m_socket(m_io_service)
{
	
}
CTcpClient::CTcpClient(const std::string & ip, short port) : m_ip(ip), m_port(port), m_socket(m_io_service)
{
	
}


CTcpClient::~CTcpClient()
{
}

void CTcpClient::SetServerIp(const std::string & ip, short port)
{
	m_ip = ip;
	m_port = port;
}
bool CTcpClient::Connect()
{
	try
	{
		m_socket.connect(
			tcp::endpoint(boost::asio::ip::address::from_string(m_ip), m_port)
			);
		m_last_trans = time(NULL);
		m_connected = true;
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
	if (!m_connected && !Connect())
		return false;
	return RawSend(szBuff, nSize,m_can_reconnect);
}

bool CTcpClient::RawSend(const char * szData, std::size_t nSize, bool with_connect)
{
	try
	{
		boost::asio::write(m_socket, boost::asio::buffer(szData, nSize));
		m_last_trans = time(NULL);
		return true;
	}
	catch (boost::system::system_error & e)
	{
		if (with_connect)
		{
			const int socket_error_code = e.code().value();
			if (socket_error_code == 10009 || socket_error_code == 10054 || socket_error_code == 10057)
				if(Connect())
					return RawSend(szData, nSize, false);

		}
		OutputDebugStrA("CTcpClient::Send exception:%s", e.what());
		return false;	
	}
}
bool CTcpClient::Recv(msgpack::object_handle & result)
{
	if (!m_connected && !Connect())
		return false;
	try
	{
		m_unpacker.reserve_buffer(MAX_TRY_RECV_BUFF);
		while (true)
		{
			std::size_t actual_read_size = m_socket.read_some(boost::asio::buffer(m_unpacker.buffer(), MAX_TRY_RECV_BUFF));
			if (actual_read_size > 0) m_last_trans = time(NULL);
			m_unpacker.buffer_consumed(actual_read_size);
			if (m_unpacker.next(result))
			{
				if (m_unpacker.nonparsed_size() == 0)
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


bool CTcpClient::SendAndRecv(const char * buff, std::size_t bytes, msgpack::object_handle & result)
{
	std::lock_guard<std::mutex> l(m_session_mutex);
	return (Send(buff, bytes) && Recv(result));
}