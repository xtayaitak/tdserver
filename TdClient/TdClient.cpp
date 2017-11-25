#include "stdafx.h"
#include "TdClient.h"
#include "TradeStruct.h"

#include <msgpack.hpp>

#include "StringTool.h"

#include "MsgpackMap.h"
#include "DebugOutput.h"
CTdClient::CTdClient(const std::string & ip, short port) : CTcpClient(ip,port)
{

}


CTdClient::~CTdClient()
{
}


bool CTdClient::LoginUser(const std::wstring & id,const std::wstring & pwd, const std::wstring & machine_md5)
{
	try
	{
		msgpack::sbuffer sBuf;
		{
			msgpack::packer<msgpack::sbuffer> pker(&sBuf);
			pker.pack_map(4);
			pker.pack("ID"); pker.pack((int)id_login_account);
			pker.pack("CLIENT_ID"); pker.pack(string_tool::wstring_to_utf8(id));
			pker.pack("PWD"); pker.pack(string_tool::wstring_to_utf8(pwd));
			pker.pack("MC_INFO"); pker.pack(string_tool::wstring_to_utf8(machine_md5));
		}
		if (!Send(sBuf.data(), sBuf.size()))
		{
			OutputDebugStr(L"client send failed!");
			return false;
		}
		msgpack::object_handle obj_handle;
		if (Recv(obj_handle))
		{
			const msgpack::object  &obj = obj_handle.get();
			CMsgpackMap msg_map(obj);
			int ret_code = msg_map.GetValueInt("RET_CODE");
			return ret_code == id_ok;
		}
		else
		{
			OutputDebugStr(L"Client read failed");
			return false;
		}
	}
	catch (std::exception & e)
	{
		OutputDebugStrA("login user failed:%s",e.what());
		return false;
	}
	
}
