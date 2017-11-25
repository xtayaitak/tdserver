#include "stdafx.h"
#include "MatchSuccessTeamMgr.h"
#include <algorithm>
#include "DebugOutput.h"
#include "TeamMgr.h"
#include "BoostLog.h"
CMatchSuccessTeamMgr::CMatchSuccessTeamMgr()
{
}


CMatchSuccessTeamMgr::~CMatchSuccessTeamMgr()
{
}


std::shared_ptr<CMatchSucessTeam> CMatchSuccessTeamMgr::AddTeam(const CTempTeam & temp_team)
{
	std::shared_ptr<CMatchSucessTeam> pTeam = std::make_shared<CMatchSucessTeam>(temp_team);
	std::lock_guard<std::mutex> l(m_team_mutex);
	m_team_list.insert(std::pair<size_t, std::shared_ptr<CMatchSucessTeam> >(pTeam->GetID(), pTeam));

	const std::vector<CMatchSuccessTeamMember>  & team_member_list = pTeam->GetMembers();
	for (auto & iter : team_member_list)
		m_role_team_map.insert(std::pair < size_t, std::shared_ptr<CMatchSucessTeam>>(iter.m_id.GetHash(), pTeam));
	return pTeam;
}

void CMatchSuccessTeamMgr::DelRoleTeamMap(std::shared_ptr<CMatchSucessTeam> t)
{
	for (auto iter = m_role_team_map.begin(); iter != m_role_team_map.end();)
	{
		if (iter->second == t)
			iter = m_role_team_map.erase(iter);
		else
			++iter;
	}
}

void CMatchSuccessTeamMgr::DelTeam(std::shared_ptr<CMatchSucessTeam> t)
{
	DelRoleTeamMap(t);
	auto delete_iter = m_team_list.find(t->GetID());
	if (delete_iter != m_team_list.end())
		m_team_list.erase(delete_iter);
}

void CMatchSuccessTeamMgr::FreeTeam(std::shared_ptr<CMatchSucessTeam> t)
{
	std::lock_guard<std::mutex> l(m_team_mutex);
	DelTeam(t);
}


std::shared_ptr<CMatchSucessTeam> CMatchSuccessTeamMgr::GetTeamByGameRoleID(const CGameRoleID & game_role_id)
{
	std::lock_guard<std::mutex> l(m_team_mutex);
	auto iter = m_role_team_map.find(game_role_id.GetHash());
	if (iter != m_role_team_map.end())
		return iter->second;
	else
		return nullptr;
}

void CMatchSuccessTeamMgr::CheckExpired()
{
	std::lock_guard<std::mutex> l(m_team_mutex);
	for (auto iter = m_team_list.begin(); iter != m_team_list.end();)
	{
		if (iter->second->IsExpired())
		{
			OutputDebugStr(L"MatchSuccessTeamMgr Expire");
			DelRoleTeamMap(iter->second);
			iter = m_team_list.erase(iter);
		}
		else
			++iter;
	}
}
void CMatchSuccessTeamMgr::OnJoinTeamSucess(size_t team_id)
{
	std::lock_guard<std::mutex> l(m_team_mutex);
	std::shared_ptr<CMatchSucessTeam> match_team_ptr = nullptr;
	auto iter = m_team_list.find(team_id);
	if (iter != m_team_list.end())
		match_team_ptr = iter->second;
	else
	{
		boost_log::LogFmtW(boost_log::error, L"!!OnJoinTeamSucess Can't find teamid:%lu", (unsigned long)team_id);
		return;
	}


	std::vector<CServerTeamMember> member_list;
	for (auto iter2 : match_team_ptr->GetMembers())
		member_list.push_back({ iter2.m_id.GetRoleName(), iter2.m_id, iter2.m_house_id });
	CTeamPtr team_ptr = std::make_shared<CServerTeam>(team_id, member_list);
	CTeamMgr::GetInstance().AddTeam(team_ptr);	
	DelTeam(match_team_ptr);


}

void CMatchSuccessTeamMgr::TeamListDo(std::function<void(std::shared_ptr<CMatchSucessTeam>)> fn)
{
	std::lock_guard<std::mutex> l(m_team_mutex);
	for (auto iter : m_team_list)
	{
		fn(iter.second);
	}
}

void CMatchSuccessTeamMgr::OnJoinTeamFail(size_t team_id)
{
	std::lock_guard<std::mutex> l(m_team_mutex);
	auto iter = m_team_list.find(team_id);
	if (iter != m_team_list.end())
		DelTeam(iter->second);
}

CMatchSucessTeam::CMatchSucessTeam(const CTempTeam & temp_team)
{
	const std::vector<CJoinTeamRequest> & team_request_list = temp_team.GetTeam();
	m_team_id = 0;
	for (auto & iter : team_request_list)
	{
		m_members.push_back(CMatchSuccessTeamMember(iter.req_id, iter.player_id, iter.house_id));
		public_tool::hash_combine(m_team_id, iter.req_id.GetRoleName());
	}
	m_create_time = time(NULL);
}



std::wstring CMatchSucessTeam::ToString() const//Êä³ö¶ÓÎé
{
	std::wstring str = L"[";
	for (auto & iter : m_members)
		str += iter.m_id.GetRoleName() + L",";
	str += L"]";
	return str;
}