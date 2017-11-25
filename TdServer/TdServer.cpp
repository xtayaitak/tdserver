// TdServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "TcpServer.h"
#include "DbConnctionPool.h"
#include "ProcessTool.h"
#include "IniFile.h"
#include "BoostLog.h"
#include "FileTool.h"

void DoMain(int argc, _TCHAR* argv[])
{
	try
	{
		boost_log::InitBoostLog((file_tools::GetCurrentAppPath() + L"TdServer" + L"_%Y%m.log").c_str());

		CDbConnctionPool::GetInstance().Init();
		CIniFile inifile(L"设置.ini");
		if (!CDbConnctionPool::GetInstance().ConnctionDataBase(inifile.GetIniStr(L"IP", L"数据库", L"127.0.0.1").c_str(),
			inifile.GetIniStr(L"用户名", L"数据库", L"sa").c_str(),
			inifile.GetIniStr(L"密码", L"数据库", L"1111111").c_str()))
		{
			std::cout << "数据库连接失败！" << std::endl;
			return;
		}
		boost::asio::io_service io;
		CTcpServer  server(io, tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 10102));
		io.run();
	}
	catch (std::exception & e)
	{
		std::cerr << "Exception:" << e.what() << std::endl;
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	DoMain(argc, argv);
	CDbConnctionPool::GetInstance().DeInit();
	system("pause");
	return 0;
}

