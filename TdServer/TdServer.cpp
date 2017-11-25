// TdServer.cpp : �������̨Ӧ�ó������ڵ㡣
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
		CIniFile inifile(L"����.ini");
		if (!CDbConnctionPool::GetInstance().ConnctionDataBase(inifile.GetIniStr(L"IP", L"���ݿ�", L"127.0.0.1").c_str(),
			inifile.GetIniStr(L"�û���", L"���ݿ�", L"sa").c_str(),
			inifile.GetIniStr(L"����", L"���ݿ�", L"1111111").c_str()))
		{
			std::cout << "���ݿ�����ʧ�ܣ�" << std::endl;
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

