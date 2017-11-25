#pragma once

#include "CJoinTeamReqCharacteristic.h"

#include <windows.h>
#include <time.h>
#include "GameRoleID.h"


//组队请求结构
class CJoinTeamRequest
{
public:
	CJoinTeamRequest(const CGameRoleID & req_id_, DWORD playerid_, DWORD house_id_, int caree, const CJoinTeamReqCharacteristic & team_feature)
		: req_id(req_id_), player_id(playerid_), house_id(house_id_), caree_type(caree), team_characteristic(team_feature){
		touch_time = time(NULL);}

	CGameRoleID req_id;
	DWORD player_id;
	DWORD house_id;
	int caree_type;
	CJoinTeamReqCharacteristic team_characteristic;
	time_t touch_time;
	void Touch()
	{
		touch_time = time(NULL);
	}
	bool IsTimeOut() const
	{
		return (time(NULL) - touch_time) > 60;
	}
	size_t GetHash() const
	{
		return req_id.GetHash();
	}
};