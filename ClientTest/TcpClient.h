#pragma once
#include <string>

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

#include <vector>

#include "msgpack.hpp"

#include <mutex>



//不可以多线程调用此类的成员函数
class CTcpClient 
{
public:
	CTcpClient();
	CTcpClient(const std::string & ip,short port);
	virtual ~CTcpClient();
	void SetServerIp(const std::string & ip, short port);
	const time_t GetLastTrans() const { return m_last_trans; }
protected:
	bool Send(const char * szBuff, std::size_t nSize);
	bool Recv(msgpack::object_handle & result);
	bool SendAndRecv(const char * buff, std::size_t bytes, msgpack::object_handle & result);
	void Close();
	boost::asio::io_service m_io_service;
private:
	bool Connect();
	//发送固定数目的数据
	bool RawSend(const char * szData, std::size_t nSize, bool with_connect = true);
	tcp::socket m_socket;
	std::string m_ip;
	short m_port = 0;
	msgpack::unpacker m_unpacker;
	std::mutex m_session_mutex;
	time_t m_last_trans = 0;
	bool m_can_reconnect = false;
	bool m_connected = false;
};

