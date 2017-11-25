#pragma once


#include <vector>
#include "TeamRequestMgr.h"
#include "CJoinTeamReqCharacteristic.h"

class CTempTeam
{
public:
	CTempTeam(const CJoinTeamReqCharacteristic & characteristic) : m_characteristic(characteristic), m_caree_flag(characteristic.caree_group){  }
	bool TryJoin(const CJoinTeamRequest & team_req)
	{
		if (team_req.team_characteristic == m_characteristic && JoinCaree(team_req.caree_type))
		{
			m_team.push_back(team_req);
			return true;
		}
		else
			return false;
	}

	bool InTeam(const CJoinTeamRequest & req)
	{
		for (auto & iter : m_team)
		{
			if (req.GetHash() == iter.GetHash())
				return true;
		}
		return false;
	}

	//本组是否组队满
	bool IsTeamJoinFinished() const
	{
		return m_team.size() == m_characteristic.member_count;
	}
	const std::vector<CJoinTeamRequest> & GetTeam() const
	{
		return m_team;
	}
	std::vector<CJoinTeamReqCharacteristic>::size_type GetCurrentCount() const
	{
		return m_team.size();
	}
	const CJoinTeamReqCharacteristic & GetCharacteristic() const
	{
		return m_characteristic;
	}
private:
	bool JoinCaree(const char c)
	{
		for (auto & iter : m_caree_flag)
		{
			if (iter == c)
			{
				//增加这个职业
				iter = 0;
				return true;
			}
		}
		return false;
	}
	const CJoinTeamReqCharacteristic m_characteristic;
	std::vector<int> m_caree_flag;
	std::vector<CJoinTeamRequest> m_team;
};
