#include "stdafx.h"
#include "TdClient.h"
#include "TradeStruct.h"
#include "UnPack.h"

#include <msgpack.hpp>

#include "StringTool.h"

CTdClient::CTdClient(const std::string & ip, short port) : CTcpClient(ip,port)
{

}


CTdClient::~CTdClient()
{
}


bool CTdClient::LoginUser(const std::wstring & id, const std::wstring & machine_md5)
{

	msgpack::sbuffer sBuf;
	{
		msgpack::packer<msgpack::sbuffer> pker(&sBuf);
		pker.pack_map(3);
		pker.pack("ID"); pker.pack(id_login_account);
		pker.pack("CLIENT_ID"); pker.pack(string_tool::wstring_to_utf8(id));
		pker.pack("MC_INFO"); pker.pack(string_tool::wstring_to_utf8(machine_md5));
	}

	if (!Send(sBuf.data(), sBuf.size()))
		return false;









// 	if (!Send(Obj.Data(), Obj.Size()))
// 		return false;
// 
// 	CBagObj RecvObj;
// 	size_t nRecvSize = Recv();
// 	if (nRecvSize > 0)
// 	{
// 		if (RecvObj.InitByBuffer(GetData(), nRecvSize))
// 		{
// 			if (RecvObj["ret"] == id_ok)
// 			{
// 				return true;
// 			}
// 		}
// 	}

	
}