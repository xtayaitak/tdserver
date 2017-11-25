#include "stdafx.h"
#include "CJoinTeamReqCharacteristic.h"

#include "StringTool.h"


CJoinTeamReqCharacteristic::CJoinTeamReqCharacteristic(int member_count_, const std::wstring & fuben_name_, const std::wstring & caree_group_, bool is_cross_server)
	: member_count(member_count_), fuben_name(fuben_name_), is_cross_team(is_cross_server), caree_group_str(caree_group_)
{
	std::vector<std::wstring> caree_item = string_tool::SplitStrByFlag<std::wstring>(caree_group_, L",");
	caree_group.clear();
	for (auto iter : caree_item)
	{
		int caree = std::stoi(iter);
		caree_group.push_back(caree);
	}
	//¼ÆËãÒ»¸öhash
	hash = 0;
	public_tool::hash_combine(hash, member_count);
	public_tool::hash_combine(hash, fuben_name);
	public_tool::hash_combine(hash, caree_group_str);
	public_tool::hash_combine(hash, is_cross_team);
}

bool CJoinTeamReqCharacteristic::TryJoin(int caree)
{
	for (auto & iter : caree_group)
	{
		if (iter == caree)
		{
			iter = '0';
			return true;
		}
	}
	return false;
}