#pragma once
#include <string>

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

#include <vector>

#include "TransMsg.h"


//不可以多线程调用此类的成员函数
class CTcpClient
{
public:
	CTcpClient(const std::string & ip,short port);
	virtual ~CTcpClient();
	bool Send(const char * szBuff,std::size_t nSize);

	//读取固定数目的数据
	size_t Recv();
	const char * GetData() const;
protected:
	void Close();
private:
	bool Connect();
	//发送固定数目的数据
	bool RawSend(const char * szData, std::size_t nSize, bool with_connect = true);
	boost::asio::io_service io_service_;
	tcp::socket socket_;
	std::string ip_;
	short port_ = 0;
	CTransMsg trans_msg_;

};

