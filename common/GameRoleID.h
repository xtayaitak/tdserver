#pragma once
#include <string>
#include <msgpack.hpp>
#include "msgpack_wstring.h"
class CGameRoleID
{
public:
	CGameRoleID(const std::wstring & s_role_name);
	CGameRoleID();
	~CGameRoleID();
	bool operator == (const CGameRoleID & role) const;
	std::size_t GetHash() const;
	const std::wstring & GetRoleName() const;
	const std::wstring ToString() const { return m_role_name; }
	operator bool() const { return m_hash > 0; }

	MSGPACK_DEFINE(m_role_name, m_hash);
private:
	std::wstring m_role_name;
	std::size_t m_hash = 0;
};

