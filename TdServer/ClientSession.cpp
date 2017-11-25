#include "stdafx.h"
#include "ClientSession.h"
#include "MsgpackMap.h"
#include "DebugOutput.h"
#include "TradeStruct.h"
#include "DbConnctionPool.h"
#include "StringTool.h"
#include <chrono>
#include "BoostLog.h"
CClientSession::CClientSession(tcp::socket s, CSessionList & client_list) : CSocketSession(std::move(s), client_list)
{
}


CClientSession::~CClientSession()
{
}
bool CClientSession::ProcessPackObj(const msgpack::object & obj, msgpack::sbuffer * psbuff)
{
	msgpack::packer<msgpack::sbuffer> pker(psbuff);
	try
	{
		if (obj.is_nil())
		{
			OutputDebugStr(L"recv bag is nil");
			pker.pack_map(1);
			pker.pack("RET_CODE"); pker.pack((int)id_unknow_error);
		}
		else if (obj.type == msgpack::type::MAP)
		{
			CMsgpackMap msg_map(obj);
			int id = msg_map.GetValueInt("ID");
			if (id == id_login_account)
			{
				std::wstring client_id = msg_map.GetValueStr("CLIENT_ID");
				std::wstring pwd = msg_map.GetValueStr("PWD");
				std::wstring mc_info = msg_map.GetValueStr("MC_INFO");
				std::wstring out_msg;
				if (OnLoginRequest(client_id, pwd, mc_info, out_msg))
				{
					pker.pack_map(2);
					pker.pack("RET_CODE"); pker.pack((int)id_ok);
					pker.pack("MSG"); pker.pack(string_tool::wstring_to_utf8(out_msg));
				}
				else
				{
					pker.pack_map(2);
					pker.pack("RET_CODE"); pker.pack((int)id_failed);
					pker.pack("MSG"); pker.pack(string_tool::wstring_to_utf8(out_msg));
				}
			}
			else
			{
				pker.pack_map(2);
				pker.pack("RET_CODE"); pker.pack((int)id_unknow_msg);
				pker.pack("MSG"); pker.pack("unknow msg");
			}
		}
		return true;
	}
	catch (std::exception & e)
	{
		OutputDebugStrA("process exceptoin:%s", e.what());
		pker.pack_map(2);
		pker.pack("RET_CODE"); pker.pack((int)id_unknow_error);
		pker.pack("MSG"); pker.pack("excpetino");
		return false;
	}
}

bool CClientSession::OnLoginRequest(const std::wstring & user, const std::wstring & pwd, const std::wstring & mc_info,std::wstring & msg)
{
	if (IsLogined())
	{
		msg = L"已经登录不用重新登录！";
		return true;
	}


	try
	{
		ADODB::_CommandPtr cmmd;
		HRESULT hr = cmmd.CreateInstance(__uuidof(ADODB::Command));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t(L"Return"), ADODB::adInteger, ADODB::adParamReturnValue, 4));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t(L"strUserName"), ADODB::adVarChar, ADODB::adParamInput, 50, string_tool::WideToChar(user.c_str()).c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t(L"strPassWord"), ADODB::adVarChar, ADODB::adParamInput, 50, string_tool::WideToChar(pwd.c_str()).c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t(L"nSoftWareID"), ADODB::adInteger, ADODB::adParamInput, 4, GAMEID_TD));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t(L"strMachineCode"), ADODB::adVarChar, ADODB::adParamInput, 50, string_tool::WideToChar(mc_info.c_str()).c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t(L"strIP"), ADODB::adVarChar, ADODB::adParamInput, 20, GetSocket().remote_endpoint().address().to_string().c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t(L"strMsg"), ADODB::adVarWChar, ADODB::adParamOutput, 100));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t(L"strExpireDate"), ADODB::adVarChar, ADODB::adParamOutput, 50));

		cmmd->CommandText = _bstr_t("UserLogin");
		cmmd->ActiveConnection = CDbConnctionPool::GetInstance().GetConnectionPtr();
		cmmd->CommandType = ADODB::adCmdStoredProc;
		cmmd->Execute(NULL, NULL, ADODB::adCmdStoredProc);
		int dblogin_ret = cmmd->Parameters->GetItem("Return")->GetValue();
		msg = (const wchar_t*)(_bstr_t)cmmd->Parameters->GetItem("strMsg")->GetValue();
		std::string str_expire_time  = (const char*)(_bstr_t)cmmd->Parameters->GetItem("strExpireDate")->GetValue();//2017-02-07 00:49:57
		if (dblogin_ret > 0)
		{
			m_expite_time = boost::posix_time::time_from_string(str_expire_time);
			m_logined = true;
			return true;
		}
		else
			return false;
	}
	catch (_com_error e)
	{
		LOGW(error) << L"数据库UsrLogin异常:" << (const wchar_t*)e.Description();
		return false;
	}
	catch (std::exception & e)
	{
		LOGW(error) << L"exception UsrLogin异常:" << string_tool::CharToWide(e.what()).c_str();
		return false;
	}
	catch (boost::exception & e)
	{
		LOGW(error) << L"boost UsrLogin异常:";
		return false;
	}

}