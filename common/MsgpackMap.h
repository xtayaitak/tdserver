#pragma once
#include <string>
#include <msgpack.hpp>

#include "NonCopyable.h"

//处理必须是key为string
class CMsgpackMap : public CNonCopyable
{
public:
	CMsgpackMap(const msgpack::object & obj);
	~CMsgpackMap();
	int GetValueInt(const std::string & key);
	std::wstring GetValueStr(const std::string & key);
	msgpack::object & GetValue(const std::string & key);
protected:
	void Parse();
private:
	const msgpack::object & m_obj;
	std::map<std::string, msgpack::object > m_save_parse_string_key;
	bool m_parsed = false;
};

