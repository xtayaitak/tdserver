#include "stdafx.h"
#include "GameRoleID.h"
#include "PublicTool.h"

CGameRoleID::CGameRoleID(const std::wstring & srole_name) : m_role_name(srole_name)
{
	if (!srole_name.empty())
	{
		std::hash<std::wstring> hasher;
		m_hash = hasher(srole_name);
	}

}
CGameRoleID::CGameRoleID()
{
	m_hash = 0;
}

CGameRoleID::~CGameRoleID()
{
}


bool CGameRoleID::operator == (const CGameRoleID & other) const
{
	return m_hash == other.GetHash();
}


std::size_t CGameRoleID::GetHash() const
{
	return m_hash;
}

const std::wstring & CGameRoleID::GetRoleName() const
{
	return m_role_name;
}