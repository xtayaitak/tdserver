#pragma once
#include <string>
#include <windows.h>
#include <msgpack.hpp>
#include "msgpack_wstring.h"
#include "GameRoleID.h"
class CServerTeamMember
{
public:
public:
	CServerTeamMember(){}
	~CServerTeamMember();
	CServerTeamMember(const std::wstring & role_name, DWORD playerid,DWORD house_id) : 
		m_role_name(role_name), m_player_id(playerid), m_house_id(house_id), m_game_role_id(role_name){}
	bool operator==(const CServerTeamMember & o) const { return o.GetRoleName() == m_role_name; }


	DWORD GetPlayerID() const { return m_player_id; }
	DWORD GetHouseID() const { return m_house_id; }
	const CGameRoleID & GetID() const { return m_game_role_id; }
	const std::wstring & GetRoleName() const { return m_role_name; }
	void SetRoleName(const std::wstring & name) { m_role_name = name; m_game_role_id = CGameRoleID(name); }
	void SetPlayerID(const DWORD id) { m_player_id = id; }
	void SetHouseID(const DWORD id) { m_house_id = id; }
	void SetMemberIntValue(const std::string & key, int val);
	void SetData(const std::string & key, int vale);
	int GetData(const std::string & key) ;
	int GetStep() const { return m_step; }
	bool GetStepFinished() const { return m_step_finished; }

	MSGPACK_DEFINE(m_role_name, m_player_id, m_house_id, m_game_role_id, m_step, m_step_finished);
private:
	std::wstring m_role_name;
	DWORD m_player_id;
	DWORD m_house_id;
	CGameRoleID m_game_role_id;
	std::map<std::string, int> m_values;
	int m_step = 0;
	bool m_step_finished = false;
};

