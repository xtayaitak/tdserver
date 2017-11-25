#pragma once
#include "Team.h"
#include <map>
#include <set>
#include "GameRoleID.h"
#include <mutex>
#include <ClassInstance.h>
class CTeamMgr : public CClassInstance<CTeamMgr>
{
public:
	CTeamMgr();
	~CTeamMgr();
	void AddTeam(CTeamPtr team_ptr);
	bool DelTeam(CTeamPtr team_ptr);
	CTeamPtr GetTeamPtrByRoleID(const CGameRoleID & role_id);
	CTeamPtr GetTeamPtrByTeamID(const size_t & id);
	void CheckExpire();
	void TeamListDo(std::function<void(CTeamPtr)> fn);
	int GetCount() const { return m_team_list.size(); }
private:
	bool FreeTeam(CTeamPtr team_ptr);
	std::map<size_t, CTeamPtr> m_role_id_team_map;
	std::map<size_t ,CTeamPtr> m_team_list;
	std::mutex m_mutex;
};

