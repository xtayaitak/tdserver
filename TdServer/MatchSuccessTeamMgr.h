#pragma once

#include "TempTeam.h"
#include <set>
#include "ClassInstance.h"
#include <mutex>
#include <memory>
typedef size_t TeamID;

//本队成功队伍列表

class CMatchSuccessTeamMember
{
public:
	CMatchSuccessTeamMember(const CGameRoleID & game_role_id, DWORD player_id, DWORD house_id) : m_id(game_role_id), m_player_id(player_id), m_house_id(house_id) {}
	CGameRoleID m_id;
	DWORD m_player_id;
	DWORD m_house_id;
};

class CMatchSucessTeam
{
public:
	CMatchSucessTeam::CMatchSucessTeam(const CTempTeam & temp_team);
	const std::vector<CMatchSuccessTeamMember> & GetMembers() const { return m_members; }
	bool IsExpired() const { return time(NULL) - m_create_time > (60 * 10); }
	size_t GetID() const { return m_team_id; }
	std::wstring ToString() const;//输出队伍
private:
	std::vector<CMatchSuccessTeamMember> m_members;
	time_t m_create_time = 0;
	size_t m_team_id = 0;
};


class CMatchSuccessTeamMgr : public CClassInstance<CMatchSuccessTeamMgr>
{
public:
	CMatchSuccessTeamMgr();
	~CMatchSuccessTeamMgr();
	std::shared_ptr<CMatchSucessTeam> AddTeam(const CTempTeam & temp); //由组队线程调用
	void FreeTeam(std::shared_ptr<CMatchSucessTeam> t); //由通信线程调用
	std::shared_ptr<CMatchSucessTeam> GetTeamByGameRoleID(const CGameRoleID & game_role_id);
	void CheckExpired();
	void OnJoinTeamSucess(size_t team_id);
	void OnJoinTeamFail(size_t team_id);
	void TeamListDo(std::function<void(std::shared_ptr<CMatchSucessTeam>)> fn);
	int GetCount() const { return m_team_list.size(); }
protected:
private:
	void DelTeam(std::shared_ptr<CMatchSucessTeam> t);
	void DelRoleTeamMap(std::shared_ptr<CMatchSucessTeam> t);
	std::mutex m_team_mutex;
	std::map<size_t, std::shared_ptr<CMatchSucessTeam> > m_team_list;
	std::map<size_t, std::shared_ptr<CMatchSucessTeam> > m_role_team_map;
};

