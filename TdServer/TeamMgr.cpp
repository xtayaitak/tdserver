#include "stdafx.h"
#include "TeamMgr.h"
#include "DebugOutput.h"

CTeamMgr::CTeamMgr()
{
}


CTeamMgr::~CTeamMgr()
{
}
std::map<size_t ,CTeamPtr> m_team_list;

void CTeamMgr::AddTeam(CTeamPtr team_ptr)
{
	std::lock_guard<std::mutex> l(m_mutex);
	if (m_team_list.find(team_ptr->GetTeamID()) == m_team_list.end())
	{
		m_team_list.insert(std::pair<size_t, CTeamPtr>(team_ptr->GetTeamID(), team_ptr));
		const std::vector<CServerTeamMember>  & team_member_list = team_ptr->GetMemberList();
		for (auto & iter : team_member_list)
			m_role_id_team_map.insert(std::pair < size_t, std::shared_ptr<CServerTeam> >(iter.GetID().GetHash(), team_ptr));
	}
}
bool CTeamMgr::DelTeam(CTeamPtr team_ptr)
{
	if (!team_ptr)
		return false;
	std::lock_guard<std::mutex> l(m_mutex);
	return FreeTeam(team_ptr);
}

bool CTeamMgr::FreeTeam(CTeamPtr team_ptr)
{
	//遍历队员 删除队员与Team映射
	for (auto iter : team_ptr->GetMemberList())
	{
		auto iter2 = m_role_id_team_map.find(iter.GetID().GetHash());
		if (iter2 != m_role_id_team_map.end())
			m_role_id_team_map.erase(iter2);
	}
	auto iter_delete = m_team_list.find(team_ptr->GetTeamID());
	if (iter_delete != m_team_list.end())
	{
		m_team_list.erase(iter_delete);
		return true;
	}
	else
		return false;
}
CTeamPtr CTeamMgr::GetTeamPtrByRoleID(const CGameRoleID & role_id)
{
	std::lock_guard<std::mutex> l(m_mutex);
	auto iter = m_role_id_team_map.find(role_id.GetHash());
	if (iter != m_role_id_team_map.end())
		return iter->second;
	else
		return nullptr;
}

CTeamPtr CTeamMgr::GetTeamPtrByTeamID(const size_t & id)
{
	if (id == 0)
		return nullptr;
	std::lock_guard<std::mutex> l(m_mutex);
	auto iter = m_team_list.find(id);
	if (iter != m_team_list.end())
		return iter->second;
	else
		return nullptr;
}

void CTeamMgr::CheckExpire()
{
	std::lock_guard<std::mutex> l(m_mutex);
	std::vector<CTeamPtr> del_list;
	for (auto iter : m_team_list)
	{
		if (iter.second->IsExpired())
		{
			OutputDebugStr(L"Team Expired:%lu", (unsigned long)iter.second->GetTeamID());
			del_list.push_back(iter.second);
		}
	}
	for (auto iter : del_list)
		FreeTeam(iter);
}

void CTeamMgr::TeamListDo(std::function<void(CTeamPtr)> fn)
{
	std::lock_guard<std::mutex> l(m_mutex);
	for (auto & iter : m_team_list)
		fn(iter.second);
}