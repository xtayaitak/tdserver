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
	msgpack::object & GetMsgpackObject(const std::string & key);

	template<typename T>
	T GetValue(const std::string & key)
	{
		if (!m_parsed)
			Parse();
		auto iter = m_save_parse_string_key.find(key);
		if (iter != m_save_parse_string_key.end())
			return iter->second.as<T>();
		else
			throw std::exception((std::string("no key:") + key).c_str());
	}
protected:
	void Parse();
private:
	const msgpack::object & m_obj;
	std::map<std::string, msgpack::object > m_save_parse_string_key;
	bool m_parsed = false;
};

