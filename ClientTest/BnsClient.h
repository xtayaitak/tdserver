#pragma once

#include "TcpClient.h"
#include "TradeStruct.h"
#include "ClassInstance.h"
#include "ServerTeamMember.h"
#include "GameRoleID.h"
class CBnsClient : public CTcpClient
{
public:
	struct JoinTeamResultMember
	{
		DWORD player_id;
		DWORD house_id;
		std::wstring role_name;
	};
public:
	CBnsClient() : m_heartbeat_timer(m_io_service){}
	~CBnsClient();
	static std::shared_ptr<CBnsClient>  GetInstance()
	{
		return m_client_ptr;
	}
	static void CreateInstance()
	{
		m_client_ptr = std::make_shared<CBnsClient>();
	}
	static void FreeInstance()
	{
		m_client_ptr.reset();
	}
	static std::shared_ptr<CBnsClient> m_client_ptr;
	void StopIoService();
public:
	//��¼��֤
	bool LoginUser(const std::wstring & id,const std::wstring & pwd,const std::wstring & machine_md5,std::wstring & msg);
	//���ý�ɫID
	reply_result SetGameRoleID(const std::wstring & role_name);
	//�ύ�������
	reply_result RequestJoinTeam( DWORD player_id, int caree_id, const std::wstring & fuben_name, int team_count, const std::wstring & caree_group);
	//���������
	reply_result RequestJoinTeamCrossServer(DWORD house_id, int caree_id, const std::wstring & fuben_name, int team_count, const std::wstring  & caree_group);
	//���봴������
	reply_result RequestCreateTeam(std::vector<CServerTeamMember> members, size_t * pTeamId = 0);
	//����õ�������������Ϣ
	reply_result RequestGetTeamInfo(std::vector<CServerTeamMember> & members);
	//��ȡ�������Ľ��
	reply_result GetJoinTeamReqResult(std::vector<JoinTeamResultMember> & team_req_result);
	//��ɢ����
	reply_result DismissTeam();
	//�ͻ������ʧ��
	reply_result JoinTeamFailed();
	reply_result JoinTeamSucess();
	//ȡ���������
	reply_result RequestCancelJoinTeamReq();
	//���ö�Ա��Ϣ
	reply_result RequestSetTeamMemberData(const std::string & key, int val);
	//���ö�Ա��Ϣ�б�
	reply_result RequestSetTeamMemberDataList(std::map<std::string,int> datas);
	//��ȡ��Ա��Ϣ
	reply_result RequestGetTeamMemberDataList(std::map<std::string, int> &datas);
	//�Ƿ������˽�ɫ
	bool IsRemoteGameRoleIDSet() const { return m_role_id_set; }
	//�ͻ�������
	reply_result TouchServer(const CGameRoleID & m_game_role_id);
	//���������߳�
	void StartClientThread(std::function<void(void)> fn);
	//�����ص��ú���
	void OnHeartbeatTimer(const boost::system::error_code &);
private:
	bool m_role_id_set = false;
	reply_result CommonIdRequest(request_msg_id req_id);
	size_t m_team_id = 0;
	std::function<void(void)> m_heart_beat_fn;
	boost::asio::deadline_timer m_heartbeat_timer;
};

