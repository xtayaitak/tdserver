#include "stdafx.h"
#include "Team.h"
#include <algorithm>
#include "PublicTool.h"
// CServerTeam::CServerTeam()
// {
// }





void CServerTeam::CalcHash()
{
	m_id = 0;
	for (auto & iter : m_member_list)
		public_tool::hash_combine(m_id,iter.GetID().GetRoleName());
	m_touch_time = time(NULL);
}


void CServerTeam::Touch()
{
	m_touch_time = time(NULL);
}

bool CServerTeam::MemberDo(const CGameRoleID & id, std::function<void(CServerTeamMember & s)> fn)
{
	Touch();
	for (auto & iter : m_member_list)
	{
		if (id == iter.GetID())
		{
			fn(iter);
			return true;
		}
	}
	return false;
}


std::wstring CServerTeam::ToString() const
{
	std::wstring str = L"[";
	for (auto & iter : m_member_list)
		str += iter.GetRoleName() + L",";
	str += L"]";
	return str;
}