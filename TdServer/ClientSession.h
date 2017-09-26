#pragma once

#include <memory>
#include <boost/asio.hpp>
#include "TransMsg.h"
using boost::asio::ip::tcp;

#include "ClientList.h"


class CClientSession : public std::enable_shared_from_this<CClientSession>
{
public:

	CClientSession(tcp::socket s, CClientList & client_list) : socket_(std::move(s)), client_list_(client_list){ std::cout << "Client ¹¹Ôì" << std::endl; }
	~CClientSession();
	void Start()
	{
		client_list_.Join(shared_from_this());
		DoReadHeader();
	}
protected:
	virtual void ProcessRequest(const char * szRequest, std::size_t nRequestSize, std::vector<char> & OutBuffer);
	CTransMsg msg_;
	void EncodeWriteData(BYTE * data, size_t nSize);
private:
	void DoReadHeader();
	void DoReadBody();
	void DoWrite(const char * szBuff,std::size_t nSize);
	tcp::socket socket_;
	CClientList & client_list_;
};


typedef std::shared_ptr<CClientSession> SessionPtr;