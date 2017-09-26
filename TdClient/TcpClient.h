#pragma once
#include <string>

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

#include <vector>

#include "TransMsg.h"


//�����Զ��̵߳��ô���ĳ�Ա����
class CTcpClient
{
public:
	CTcpClient(const std::string & ip,short port);
	virtual ~CTcpClient();
	bool Send(const char * szBuff,std::size_t nSize);

	//��ȡ�̶���Ŀ������
	size_t Recv();
	const char * GetData() const;
protected:
	void Close();
private:
	bool Connect();
	//���͹̶���Ŀ������
	bool RawSend(const char * szData, std::size_t nSize, bool with_connect = true);
	boost::asio::io_service io_service_;
	tcp::socket socket_;
	std::string ip_;
	short port_ = 0;
	CTransMsg trans_msg_;

};

