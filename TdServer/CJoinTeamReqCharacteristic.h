#pragma once

#include "PublicTool.h"



struct CJoinTeamReqCharacteristic
{
	CJoinTeamReqCharacteristic(int member_count_, const std::wstring & fuben_name_, const std::wstring & caree_group_, bool is_cross_server);
	CJoinTeamReqCharacteristic(){}

	bool operator ==(const CJoinTeamReqCharacteristic & o) const { return hash == o.GetHash(); }
	bool TryJoin(int caree);
	std::size_t GetHash() const { return hash; }

	int member_count = 0;
	std::wstring fuben_name;
	std::vector<int> caree_group;
	std::wstring caree_group_str;
	bool is_cross_team = false;
private:
	std::size_t hash = 0;
};