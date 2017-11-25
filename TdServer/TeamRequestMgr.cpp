#include "stdafx.h"
#include "TcpServer.h"

#include "TeamRequestMgr.h"
#include "DebugOutput.h"

#include "TempTeam.h"
#include "MatchSuccessTeamMgr.h"
#include "ClientSession.h"
#include "TeamMgr.h"

extern CTcpServer * g_TcpServer;

CJoinTeamRequestMgr::CJoinTeamRequestMgr()
{
}


CJoinTeamRequestMgr::~CJoinTeamRequestMgr()
{
	m_thread_run = false;
	if (m_deal_thread.joinable())
		m_deal_thread.join();
}

//通信线程调用
void CJoinTeamRequestMgr::PushRequest(const CJoinTeamRequest & team_req)
{
	std::lock_guard<std::mutex> l(m_team_req_mutex);
	auto iter = m_request_list.find(team_req.GetHash());
	if (iter == m_request_list.end())
		m_request_list.insert(std::pair<size_t, CJoinTeamRequest>(team_req.GetHash(), team_req));
	else
		iter->second = team_req;
}
//通信线程调用
bool CJoinTeamRequestMgr::DeleteRequest(const CGameRoleID & team_req_id)
{
	std::lock_guard<std::mutex> l(m_team_req_mutex);
	auto iter = m_request_list.find(team_req_id.GetHash());
	if (iter != m_request_list.end())
	{
		m_request_list.erase(iter);
		return true;
	}
	else
		return false;
}

//通信线程调用
void CJoinTeamRequestMgr::TouchReq(const CGameRoleID & team_req_id)
{
	std::lock_guard<std::mutex> l(m_team_req_mutex);
	auto iter = m_request_list.find(team_req_id.GetHash());
	if (iter != m_request_list.end())
	{
		//OutputDebugStr(L"TeamReq Touch:%s", team_req_id.GetRoleName().c_str());
		iter->second.Touch();
	}
		
}
//本队线程调用
void CJoinTeamRequestMgr::TestTimeOutReq()
{
	std::lock_guard<std::mutex> l(m_team_req_mutex);
	for (auto iter = m_request_list.begin(); iter != m_request_list.end();)
	{
		if (iter->second.IsTimeOut())
		{
#ifdef _DEBUG
			OutputDebugStr(L"TeamReq TimeOut::%s", iter->second.req_id.GetRoleName().c_str());
#endif
			iter = m_request_list.erase(iter);
		}
		else
			++iter;
	}
}

//本队线程调用
void CJoinTeamRequestMgr::MatchTeam()
{
	std::lock_guard<std::mutex> l(m_team_req_mutex);
#ifdef _DEBUG
	//OutputDebugStr(L"申请组队数量:%d", (int)m_request_list.size());
#endif // _DEBUG

	std::set<size_t> tried_characteristics;
	for (auto iter = m_request_list.begin(); iter != m_request_list.end(); iter++)
	{
		if (tried_characteristics.find(iter->second.team_characteristic.GetHash()) != tried_characteristics.end())
			continue;

		CTempTeam temp_team(iter->second.team_characteristic);
		temp_team.TryJoin(iter->second);
		tried_characteristics.insert(iter->second.team_characteristic.GetHash());
		for (auto iter2 = std::next(iter); iter2 != m_request_list.end(); iter2++)
		{
			if (temp_team.TryJoin(iter2->second))
			{
				if (temp_team.IsTeamJoinFinished())
					break; //组队完成
				else
					;//人数不够
			}
			else
				;//无法加入
		}

		if (temp_team.IsTeamJoinFinished())
		{
			OutputDebugStr(L"组队配队成功");
			std::shared_ptr < CMatchSucessTeam> team_ptr = CMatchSuccessTeamMgr::GetInstance().AddTeam(temp_team);
			for (auto iter_delete = m_request_list.begin(); iter_delete != m_request_list.end();)
			{
				if (temp_team.InTeam(iter_delete->second))
					iter_delete = m_request_list.erase(iter_delete);
				else
					++iter_delete;
			}
			break;
		}
		else
			OutputDebugStr(L"组队本队未成功，本队了%d个终止", (int)temp_team.GetCurrentCount());
	}
}
void CJoinTeamRequestMgr::StartTeamService()
{
	m_thread_run = true;
	m_deal_thread = std::thread( [this]()
	{
		int last_check_match_team_expired_time = 0;
		int last_check_team_expire_time = 0;
		while (m_thread_run)
		{
			Sleep(1000);
			MatchTeam();
			TestTimeOutReq();
			if (time(NULL) - last_check_match_team_expired_time > 10)
				CMatchSuccessTeamMgr::GetInstance().CheckExpired();
			if (time(NULL) - last_check_team_expire_time > 10)
				CTeamMgr::GetInstance().CheckExpire();
		}
	});
}