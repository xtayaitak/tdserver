#include "stdafx.h"
#include "TradeSession.h"

#include "ServerTransaction.h"

#include "TradeStruct.h"

#include <msgpack.hpp>

CTradeSession::CTradeSession(tcp::socket s, CClientList & client_list) : CClientSession(std::move(s),client_list)
{
}


CTradeSession::~CTradeSession()
{
}

void CTradeSession::ProcessRequest(const char * szRequest, std::size_t nRequestSize, std::vector<char> & OutBuffer)
{
 	msgpack::object_handle oh =  msgpack::unpack(szRequest, nRequestSize);
	const msgpack::object & object = oh.get();
	if (object.is_nil())
		return;

	if (object.type == msgpack::type::MAP)
	{

	}


}