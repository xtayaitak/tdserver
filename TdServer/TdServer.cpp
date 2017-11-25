// ServerBns.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>

#include "TcpServer.h"
#include "DbConnctionPool.h"
#include "ProcessTool.h"
#include "IniFile.h"
#include "BoostLog.h"
#include "FileTool.h"
#include "TeamRequestMgr.h"
#include "ClientSession.h"
#include "StringTool.h"
#include <memory>
#include "MatchSuccessTeamMgr.h"
#include "TeamMgr.h"
#include <fstream>
#include <locale>
#include <codecvt>
void DoMain(int argc, _TCHAR* argv[])
{
	try
	{
		boost_log::InitBoostLog((file_tools::GetCurrentAppPath() + L"TdServer" + L"_%Y%m.log").c_str());
		CIniFile inifile(L"TdServer.ini");
		if (!CDbConnctionPool::GetInstance().ConnctionDataBase(inifile.GetIniStr(L"IP", L"数据库", L"127.0.0.1").c_str(),
			inifile.GetIniStr(L"用户名", L"数据库", L"sa").c_str(),
			inifile.GetIniStr(L"密码", L"数据库", L"1111111").c_str()))
		{
			std::wcout << L"数据库连接失败！" << std::endl;
			return;
		}
		CJoinTeamRequestMgr::GetInstance().StartTeamService();
		boost::asio::io_service io;
		CTcpServer  server(io, tcp::endpoint(boost::asio::ip::address::from_string("0.0.0.0"), 10180));
		std::thread input_thread([&server](){
			while (true)
			{
				std::wstring cmd;
				std::wcin >> cmd;
				std::wstring file_text;
				if (cmd == L"show_client_list")
				{
					file_text = std::wstring(L"共:") + std::to_wstring(server.GetSessionList().GetClientCount()) + L"条\r\n";
					server.GetSessionList().ClientListDo([&file_text](SessionPtr & ptr){
						WCHAR buff[30];
						time_t touch_time = ptr->GetTouchTime();
						struct tm newtime;
						localtime_s(&newtime, &touch_time);
						wcsftime(buff, 30, L"%Y-%m-%d %H:%M:%S", &newtime);
						file_text += (L"CardID:" + ptr->GetClientID() + L" IP:" + string_tool::CharToWide(ptr->GetIp().c_str()) + L" RoleID:" + ptr->GetGameRoleID().ToString() + L" TM:" + buff + L"\r\n");
					});
				}
				else if (cmd == L"show_match_team_list")
				{
					file_text = std::wstring(L"共:") + std::to_wstring(CMatchSuccessTeamMgr::GetInstance().GetCount()) + L"条\r\n";
					CMatchSuccessTeamMgr::GetInstance().TeamListDo([&file_text](std::shared_ptr<CMatchSucessTeam> ptr)
					{
						file_text += (L"id:" + std::to_wstring(ptr->GetID()) + L" member:" + ptr->ToString() + L"\r\n");
					});
				}
				else if (cmd == L"show_team_list")
				{
					file_text = std::wstring(L"共:") + std::to_wstring(CTeamMgr::GetInstance().GetCount()) + L"条\r\n";
					CTeamMgr::GetInstance().TeamListDo([&file_text](CTeamPtr ptr) {
						file_text += (L"id:" + std::to_wstring(ptr->GetTeamID()) + L" member:" + ptr->ToString() + L"\r\n");
					});
				}
				file_tools::WriteUnicodeFile(L"debug_file.txt", file_text);
			}
		});
		io.run();;
	}
	catch (std::exception & e)
	{
		std::cerr << "Exception:" << e.what() << std::endl;
	}


}

#include <io.h>
#include <fcntl.h>
int _tmain(int argc, _TCHAR* argv[])
{
	_setmode(_fileno(stdout), _O_U16TEXT);
	DoMain(argc, argv);
	return 0;
}

