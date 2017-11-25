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
	//登录验证
	bool LoginUser(const std::wstring & id,const std::wstring & pwd,const std::wstring & machine_md5,std::wstring & msg);
	//设置角色ID
	reply_result SetGameRoleID(const std::wstring & role_name);
	//提交组队请求
	reply_result RequestJoinTeam( DWORD player_id, int caree_id, const std::wstring & fuben_name, int team_count, const std::wstring & caree_group);
	//申请跨服组队
	reply_result RequestJoinTeamCrossServer(DWORD house_id, int caree_id, const std::wstring & fuben_name, int team_count, const std::wstring  & caree_group);
	//申请创建队伍
	reply_result RequestCreateTeam(std::vector<CServerTeamMember> members, size_t * pTeamId = 0);
	//申请得到服务器队伍信息
	reply_result RequestGetTeamInfo(std::vector<CServerTeamMember> & members);
	//获取组队请求的结果
	reply_result GetJoinTeamReqResult(std::vector<JoinTeamResultMember> & team_req_result);
	//解散队伍
	reply_result DismissTeam();
	//客户端组队失败
	reply_result JoinTeamFailed();
	reply_result JoinTeamSucess();
	//取消申请组队
	reply_result RequestCancelJoinTeamReq();
	//设置队员信息
	reply_result RequestSetTeamMemberData(const std::string & key, int val);
	//设置队员信息列表
	reply_result RequestSetTeamMemberDataList(std::map<std::string,int> datas);
	//获取队员信息
	reply_result RequestGetTeamMemberDataList(std::map<std::string, int> &datas);
	//是否设置了角色
	bool IsRemoteGameRoleIDSet() const { return m_role_id_set; }
	//客户端心跳
	reply_result TouchServer(const CGameRoleID & m_game_role_id);
	//开启心跳线程
	void StartClientThread(std::function<void(void)> fn);
	//心跳回调用函数
	void OnHeartbeatTimer(const boost::system::error_code &);
private:
	bool m_role_id_set = false;
	reply_result CommonIdRequest(request_msg_id req_id);
	size_t m_team_id = 0;
	std::function<void(void)> m_heart_beat_fn;
	boost::asio::deadline_timer m_heartbeat_timer;
};

