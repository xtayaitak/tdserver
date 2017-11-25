#pragma once
#include <map>
#include "ServerTeamMember.h"
#include "TempTeam.h"
#include "GameRoleID.h"
#include <memory>
class CServerTeam
{
public:
	CServerTeam(std::vector<CServerTeamMember> & member_list) : m_member_list(member_list) {}
	CServerTeam(size_t team_id, std::vector<CServerTeamMember> & member_list) :  m_member_list(member_list), m_id(team_id) {}
	const std::vector<CServerTeamMember> & GetMemberList()  { Touch(); return m_member_list; }
	size_t GetTeamID()  { if (m_id == 0) CalcHash(); return m_id; }
	bool MemberDo(const CGameRoleID & id, std::function<void(CServerTeamMember & s)> fn);
	bool IsExpired() const { return time(NULL) - m_touch_time > (10 * 60); }
	std::wstring ToString() const;
private:
	void CalcHash();
	void Touch();
	std::vector<CServerTeamMember> m_member_list;
	time_t m_touch_time;
	size_t m_id = 0;
};

typedef std::shared_ptr<CServerTeam> CTeamPtr;