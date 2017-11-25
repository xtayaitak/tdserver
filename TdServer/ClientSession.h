#pragma once
#include "SocketSession.h"
#include <string>
#include <ctime>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Team.h"

#include <memory>
#include "GameRoleID.h"
#include "TradeStruct.h"

class CClientSession : public CSocketSession
{
public:
	CClientSession(tcp::socket s, CSessionList & client_list);
	~CClientSession();

	virtual void ProcessPackObj(const msgpack::object & obj, msgpack::sbuffer * psbuff,SessionPtr sess_ptr);
	//��¼����
	bool OnRequestLogin(const std::wstring & user, const std::wstring & pwd,const std::wstring & mc_info,std::wstring & msg);
	//�ͻ����������
	reply_result OnRequestJoinTeam(DWORD player_id, const std::wstring & fuben_name, int member_count, int caree_id, const std::wstring & team_flag);
	//���������
	reply_result OnRequestJoinTeamCrossServer(DWORD house_id, const std::wstring & fuben_name, int member_count, int caree_id, const std::wstring & team_flag);
	//�Ƿ�����
	bool IsLogined() const { return m_logined; }
	//�ͻ������ʧ��
	void OnClientTeamJoinFailed();
public:
	const CGameRoleID & GetGameRoleID() const { return m_game_role_id; }
	const std::wstring & GetClientID() const { return m_client_id; }
protected:
	virtual void OnClientDisconnect();
	bool HasTime() const { return m_expire_time > boost::posix_time::second_clock::local_time(); }
private:
	bool m_logined = false;
	boost::posix_time::ptime m_expire_time;
	CGameRoleID m_game_role_id;
	std::wstring m_client_id;

};

typedef std::shared_ptr<CClientSession> ClientSessionPtr;