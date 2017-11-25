#pragma once

#include <string>
#include <map>
#include <mutex>
#include <thread>
#include "ClassInstance.h"
#include "PublicTool.h"


#include "JoinTeamRequest.h"


class CJoinTeamRequestMgr : public CClassInstance<CJoinTeamRequestMgr>
{
public:
	CJoinTeamRequestMgr();
	~CJoinTeamRequestMgr();
	void PushRequest(const CJoinTeamRequest & team_req);
	bool DeleteRequest(const CGameRoleID &id);
	void TouchReq(const CGameRoleID  & id);
	void TestTimeOutReq();
	void MatchTeam();//∂”ŒÈ±æ∂”
	void StartTeamService();
private:
	std::map<std::size_t, CJoinTeamRequest> m_request_list;
	std::mutex m_team_req_mutex;
	std::thread m_deal_thread;
	bool m_thread_run = false;
};

