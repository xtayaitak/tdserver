// TdServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "TcpServer.h"


int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		boost::asio::io_service io;
		CTcpServer  server(io, tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 10102));
		io.run();
	}
	catch (std::exception & e)
	{
		std::cerr << "Exception:" << e.what() << std::endl;
	}
	return 0;
}

