#pragma once
#include <map>
#include <msgpack.hpp>
class CUnPack
{
public:
	CUnPack();
	~CUnPack();
	bool InitByBuffer(const char * buff, size_t buff_size);
	std::string GetStr(std::string & key);
	int GetInt(std::string & key);
private:
	msgpack::unpacked m_unpacked;
};

