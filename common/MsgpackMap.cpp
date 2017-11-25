#include "stdafx.h"
#include "MsgpackMap.h"
#include "DebugOutput.h"
#include "StringTool.h"
#include "StringTool.h"
CMsgpackMap::CMsgpackMap(const msgpack::object & obj) : m_obj(obj)
{

}

CMsgpackMap::~CMsgpackMap()
{
}


void CMsgpackMap::Parse()
{
	if (m_obj.type == msgpack::type::MAP)
	{
		if (m_obj.via.map.size > 0)
		{
			msgpack::object_kv * pkv = m_obj.via.map.ptr;
			msgpack::object_kv * pkv_end = m_obj.via.map.ptr + m_obj.via.map.size;
			do
			{
				const msgpack::object & key = pkv->key;
				const msgpack::object & value = pkv->val;

				if (key.type == msgpack::type::STR)
					m_save_parse_string_key[key.as<std::string>()] = value;
				else
					OutputDebugStr(L"Unknow key %d", (int)key.type);
				++pkv;
			} while (pkv < pkv_end);
		}
	}

	m_parsed = true;
}

int CMsgpackMap::GetValueInt(const std::string & key)
{
	if (!m_parsed)
		Parse();
	auto iter = m_save_parse_string_key.find(key);
	if (iter != m_save_parse_string_key.end())
		return iter->second.as<int>();
	else
		throw std::exception((std::string("no key:") + key).c_str());

}
std::wstring CMsgpackMap::GetValueStr(const std::string & key)
{
	if (!m_parsed)
		Parse();
	auto iter = m_save_parse_string_key.find(key);
	if (iter != m_save_parse_string_key.end())
		return string_tool::utf8_to_wstring(iter->second.as<std::string>());
	else
		throw std::exception((std::string("no key:") + key).c_str());
}

msgpack::object & CMsgpackMap::GetValue(const std::string & key)
{
	if (!m_parsed)
		Parse();
	auto iter = m_save_parse_string_key.find(key);
	if (iter != m_save_parse_string_key.end())
		return iter->second;
	else
		throw std::exception((std::string("no key:") + key).c_str());
}