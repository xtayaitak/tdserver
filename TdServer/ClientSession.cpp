#include "stdafx.h"
#include "ClientSession.h"
#include "MsgpackMap.h"
#include "DebugOutput.h"
#include "TradeStruct.h"
#include "StringTool.h"
#include <chrono>
#include "BoostLog.h"
#include "DbConnctionPool.h"
#include "TeamRequestMgr.h"
#include "MatchSuccessTeamMgr.h"
#include "msgpack_wstring.h"
#include "TeamMgr.h"
#include "BoostLog.h"
CClientSession::CClientSession(tcp::socket s, CSessionList & client_list) : CSocketSession(std::move(s), client_list)
{
}

CClientSession::~CClientSession()
{
}
void CClientSession::ProcessPackObj(const msgpack::object & obj, msgpack::sbuffer * psbuff, SessionPtr sess_ptr)
{
	msgpack::packer<msgpack::sbuffer> pker(psbuff);
	try
	{
		if (obj.is_nil() || obj.type != msgpack::type::MAP)
		{
			OutputDebugStr(L"recv bag is nil or is not map");
			pker.pack_map(1);
			pker.pack("RET_CODE"); pker.pack((int)repid_unknow_error);
			return;
		}

		CMsgpackMap msg_map(obj);
		int id = msg_map.GetValue<int>("MSG_ID");
		if (id == reqid_login_account)
		{
			std::wstring client_id = msg_map.GetValue<std::wstring>("CLIENT_ID");
			std::wstring pwd = msg_map.GetValue<std::wstring>("PWD");
			std::wstring mc_info = msg_map.GetValue<std::wstring>("MC_INFO");
			std::wstring out_msg;
			if (client_list_.CanJoin(client_id))
			{
				if (OnRequestLogin(client_id, pwd, mc_info, out_msg))
				{
					client_list_.Join(sess_ptr);
					pker.pack_map(2);
					pker.pack("RET_CODE"); pker.pack((int)repid_ok);
					pker.pack("MSG"); pker.pack(out_msg);
				}
				else
				{
					pker.pack_map(2);
					pker.pack("RET_CODE"); pker.pack((int)repid_failed);
					pker.pack("MSG"); pker.pack(out_msg);
				}
			}
			else
			{
				pker.pack_map(2);
				pker.pack("RET_CODE"); pker.pack((int)repid_failed);
				pker.pack("MSG"); pker.pack(std::wstring(L"卡账号已经在线，无法登录"));
			}
		}
		else
		{
			if (!IsLogined())
			{
				pker.pack_map(1);
				pker.pack("RET_CODE"); pker.pack((int)repid_not_logined);
				return ;
			}

			if (!HasTime())
			{
				pker.pack_map(1);
				pker.pack("RET_CODE"); pker.pack((int)repid_not_logined);
				m_stop = true;
				return;
			}

			//设置角色
			if (id == reqid_set_game_role_id)
			{
				std::wstring role_name = msg_map.GetValue<std::wstring>("ROLE_NAME");
				auto id_ret = role_name.empty() ? repid_failed : repid_ok;
				m_game_role_id = CGameRoleID(role_name);
				pker.pack_map(1);
				pker.pack("RET_CODE"); pker.pack((int)id_ret);
				return ;
			}

			//客户端心跳
			if (id == reqid_client_touch_server)
			{
				CGameRoleID role_id = msg_map.GetMsgpackObject("GAME_ROLE_ID").as<CGameRoleID>();
				if (role_id && m_game_role_id != role_id)
					m_game_role_id = role_id;
				pker.pack_map(1);
				pker.pack("RET_CODE"); pker.pack((int)repid_ok);
				return ;
			}

			if (!m_game_role_id)
			{
				pker.pack_map(1);
				pker.pack("RET_CODE"); pker.pack((int)repid_not_set_client_role);
				return ;
			}

			//请求组队
			if (id == reqid_request_team)
			{
				DWORD player_id = msg_map.GetValue<DWORD>("PLAYER_ID");
				int caree_id = msg_map.GetValue<int>("CAREE_ID");
				int team_count = msg_map.GetValue<int>("TEAM_COUNT");
				std::wstring caree_flag = msg_map.GetValue<std::wstring>("CAREE_FLAG");
				std::wstring fuben_name = msg_map.GetValue<std::wstring>("FUBEN_NAME");
				reply_result ret_id = OnRequestJoinTeam(player_id, fuben_name, team_count, caree_id, caree_flag);
				pker.pack_map(1);
				pker.pack("RET_CODE"); pker.pack((int)ret_id);
				return ;
			}

			//请求跨服组队
			if (id == reqid_request_team_cross_server)
			{
				DWORD house_id = msg_map.GetValue<DWORD>("HOUSE_ID");
				int caree_id = msg_map.GetValue<int>("CAREE_ID");
				int team_count = msg_map.GetValue<int>("TEAM_COUNT");
				std::wstring caree_flag = msg_map.GetValue<std::wstring>("CAREE_FLAG");
				std::wstring fuben_name = msg_map.GetValue<std::wstring>("FUBEN_NAME");
				reply_result ret_id = OnRequestJoinTeamCrossServer(house_id, fuben_name, team_count, caree_id, caree_flag);
				pker.pack_map(1);
				pker.pack("RET_CODE"); pker.pack((int)ret_id);
				return ;
			}

			//取消组队申请
			if (id == reqid_cancel_join_team_req)
			{
				reply_result rep_id = CJoinTeamRequestMgr::GetInstance().DeleteRequest(m_game_role_id) ? repid_ok : repid_not_exist;
				pker.pack_map(1);
				pker.pack("RET_CODE"); pker.pack((int)rep_id);
				return;
			}

			//获取组队结果
			if (id == reqid_get_reqteam_result)
			{
				CJoinTeamRequestMgr::GetInstance().TouchReq(GetGameRoleID());
				auto team_ptr = CMatchSuccessTeamMgr::GetInstance().GetTeamByGameRoleID(m_game_role_id);
				if (team_ptr)
				{
					pker.pack_map(3);
					pker.pack("RET_CODE"); pker.pack((int)repid_ok);
					pker.pack("TEAM_ID"); pker.pack(team_ptr->GetID());
					pker.pack("TEAM_INFO"); pker.pack_array(team_ptr->GetMembers().size());
					const std::vector<CMatchSuccessTeamMember> &member = team_ptr->GetMembers();
					for (auto & iter : member)
					{
						pker.pack_array(3);
						pker.pack(iter.m_player_id);
						pker.pack(iter.m_house_id);
						pker.pack(iter.m_id.GetRoleName());
					}
				}
				else
				{
					pker.pack_map(1);
					pker.pack("RET_CODE"); pker.pack((int)repid_failed);
				}
				return ;
			}

			//客户端组队成功
			if (id == reqid_client_join_team_success)
			{
				size_t team_id = msg_map.GetValue<size_t>("TEAM_ID");
				CMatchSuccessTeamMgr::GetInstance().OnJoinTeamSucess(team_id);
				pker.pack_map(1);
				pker.pack("RET_CODE"); pker.pack((int)repid_ok);
				return;
			}

			//客户端组队失败
			if (id == reqid_client_join_team_failed)
			{
				size_t team_id = msg_map.GetValue<size_t>("TEAM_ID");
				reply_result rep_id = repid_ok;
				auto team_ptr = CMatchSuccessTeamMgr::GetInstance().GetTeamByGameRoleID(m_game_role_id);
				if (team_ptr)
					CMatchSuccessTeamMgr::GetInstance().FreeTeam(team_ptr);
				else
					rep_id = repid_not_exist;
					
				pker.pack_map(1);
				pker.pack("RET_CODE"); pker.pack((int)rep_id);
				return ;
			}
		
			
			//解散队伍
			if (id == reqid_dismiss_team)
			{
				size_t team_id = msg_map.GetValue<size_t>("TEAM_ID");
				auto repid = repid_ok;
				auto team_ptr = CTeamMgr::GetInstance().GetTeamPtrByTeamID(team_id);
				if (!team_ptr)
					team_ptr = CTeamMgr::GetInstance().GetTeamPtrByRoleID(m_game_role_id);
				if (!team_ptr)
					repid = repid_not_exist;
				
				CTeamMgr::GetInstance().DelTeam(team_ptr);
				pker.pack_map(1);
				pker.pack("RET_CODE"); pker.pack((int)repid);
				return ;
			}

			//主动创建队伍
			if (id == reqid_create_team)
			{
				auto team_info  = msg_map.GetMsgpackObject("TEAM_INFO");
				std::vector<CServerTeamMember> member_list = team_info.as<std::vector<CServerTeamMember> >();
				if (member_list.size() > 1)
				{
					auto my_team_ptr = CTeamMgr::GetInstance().GetTeamPtrByRoleID(m_game_role_id);
					if (my_team_ptr)
					{
						boost_log::LogW(boost_log::error, L"已经存在队伍，不可以再次创建！");
						pker.pack_map(2);
						pker.pack("RET_CODE"); pker.pack((int)repid_team_already_exsit);
						pker.pack("TEAM_ID"); pker.pack(my_team_ptr->GetTeamID());
					}
					else
					{

						CTeamPtr team_ptr = std::make_shared<CServerTeam>(member_list);
						CTeamMgr::GetInstance().AddTeam(team_ptr);
						pker.pack("RET_CODE"); pker.pack((int)repid_ok);
						pker.pack("TEAM_ID"); pker.pack(team_ptr->GetTeamID());
					}
				}
				else
				{
					pker.pack_map(1);
					pker.pack("RET_CODE"); pker.pack((int)repid_client_req_error);
				}
				return ;
			}
			//得到队伍信息
			if (id == reqid_get_team_info)
			{
				size_t team_id = msg_map.GetValue<size_t>("TEAM_ID");
				CTeamPtr team_ptr = nullptr;
				if (team_id != 0)
					team_ptr = CTeamMgr::GetInstance().GetTeamPtrByTeamID(team_id);
				else
					team_ptr = CTeamMgr::GetInstance().GetTeamPtrByRoleID(m_game_role_id);
				if (team_ptr)
				{
					pker.pack_map(3);
					pker.pack("RET_CODE"); pker.pack((int)repid_ok);
					pker.pack("TEAM_INFO"); pker.pack(team_ptr->GetMemberList());
					pker.pack("TEAM_ID"); pker.pack(team_ptr->GetTeamID());
				}
				else
				{
					pker.pack_map(1);
					pker.pack("RET_CODE"); pker.pack((int)repid_not_exist);
				}
				return ;
			}

			//设置队员信息
			if (id == reqid_set_member_data)
			{
				std::map<std::string, int> datas = msg_map.GetMsgpackObject("MEMBER_DATA").as<std::map<std::string, int> >();
				size_t team_id = msg_map.GetValue<size_t>("TEAM_ID");
				auto team_ptr = CTeamMgr::GetInstance().GetTeamPtrByTeamID(team_id);
				if (!team_ptr)
				{
					pker.pack_map(1);
					pker.pack("RET_CODE"); pker.pack((int)repid_not_exist);
				}
				else
				{
					if (team_ptr->MemberDo(m_game_role_id, [&datas](CServerTeamMember & member) {
						for (auto & iter : datas)
							member.SetData(iter.first, iter.second);
					}))
					{
						pker.pack_map(1);
						pker.pack("RET_CODE"); pker.pack((int)repid_ok);
					}
					else
					{
						pker.pack_map(1);
						pker.pack("RET_CODE"); pker.pack((int)repid_not_exist);
					}
				}
				return ;
			}
			//得到队员信息
			if (id == reqid_get_member_data)
			{
				std::map<std::string, int> datas = msg_map.GetMsgpackObject("MEMBER_DATA").as<std::map<std::string, int> >();
				size_t team_id = msg_map.GetValue<size_t>("TEAM_ID");
				auto team_ptr = CTeamMgr::GetInstance().GetTeamPtrByTeamID(team_id);
				if (!team_ptr)
				{
					pker.pack_map(1);
					pker.pack("RET_CODE"); pker.pack((int)repid_not_exist);
				}
				else
				{
					if (team_ptr->MemberDo(m_game_role_id, [&datas](CServerTeamMember & member) {
						for (auto & iter : datas)
							iter.second = member.GetData(iter.first);
					}))
					{
						pker.pack_map(2);
						pker.pack("RET_CODE"); pker.pack((int)repid_ok);
						pker.pack("MEMBER_DATA"); pker.pack(datas);
					}
					else
					{
						pker.pack_map(1);
						pker.pack("RET_CODE"); pker.pack((int)repid_not_exist);
					}
				}
				return ;
			}


			
			//最后没有此交易
			pker.pack_map(2);
			pker.pack("RET_CODE"); pker.pack((int)repid_unknow_msg);
			pker.pack("MSG"); pker.pack(std::wstring(L"unknow msg"));
			
		}
	}
	catch (std::exception & e)
	{
		OutputDebugStrA("process exceptoin:%s", e.what());
		pker.pack_map(2);
		pker.pack("RET_CODE"); pker.pack((int)repid_unknow_error);
		pker.pack("MSG"); pker.pack(std::wstring(L"excpetino"));
		return ;
	}
}

bool CClientSession::OnRequestLogin(const std::wstring & user, const std::wstring & pwd, const std::wstring & mc_info,std::wstring & msg)
{
	if (IsLogined())
	{
		msg = L"已经登录不用重新登录！";
		return true;
	}
	try
	{
		ADODB::_CommandPtr cmmd;
		HRESULT hr = cmmd.CreateInstance(__uuidof(ADODB::Command));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t(L"Return"), ADODB::adInteger, ADODB::adParamReturnValue, 4));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t(L"strUserName"), ADODB::adVarChar, ADODB::adParamInput, 50, string_tool::WideToChar(user.c_str()).c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t(L"strPassWord"), ADODB::adVarChar, ADODB::adParamInput, 50, string_tool::WideToChar(pwd.c_str()).c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t(L"nSoftWareID"), ADODB::adInteger, ADODB::adParamInput, 4, GAMEID));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t(L"strMachineCode"), ADODB::adVarChar, ADODB::adParamInput, 50, string_tool::WideToChar(mc_info.c_str()).c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t(L"strIP"), ADODB::adVarChar, ADODB::adParamInput, 20, GetSocket().remote_endpoint().address().to_string().c_str()));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t(L"strMsg"), ADODB::adVarWChar, ADODB::adParamOutput, 100));
		cmmd->Parameters->Append(cmmd->CreateParameter(_bstr_t(L"strExpireDate"), ADODB::adVarChar, ADODB::adParamOutput, 50));

		cmmd->CommandText = _bstr_t("UserLogin");
		cmmd->ActiveConnection = CDbConnctionPool::GetInstance().GetConnectionPtr();
		cmmd->CommandType = ADODB::adCmdStoredProc;
		cmmd->Execute(NULL, NULL, ADODB::adCmdStoredProc);
		int dblogin_ret = cmmd->Parameters->GetItem("Return")->GetValue();
		msg = (const wchar_t*)(_bstr_t)cmmd->Parameters->GetItem("strMsg")->GetValue();
		std::string str_expire_time  = (const char*)(_bstr_t)cmmd->Parameters->GetItem("strExpireDate")->GetValue();//2017-02-07 00:49:57
		if (dblogin_ret > 0)
		{
			m_expire_time = boost::posix_time::time_from_string(str_expire_time);
			m_logined = true;
			m_client_id = user;
			return true;
		}
		else
			return false;
	}
	catch (_com_error &e)
	{
		LOGW(error) << L"数据库UsrLogin异常:" << (const wchar_t*)e.Description();
		return false;
	}
	catch (std::exception & e)
	{
		LOGW(error) << L"exception UsrLogin异常:" << string_tool::CharToWide(e.what()).c_str();
		return false;
	}
	catch (...)
	{
		LOGW(error) << L"未知异常";
		return false;
	}
}



reply_result CClientSession::OnRequestJoinTeam(DWORD player_id, const std::wstring & fuben_name, int member_count, int caree_id, const std::wstring & team_flag)
{
	if (m_game_role_id.GetHash() == 0)
	{
		OutputDebugStr(L"!!!OnRequestJoinTeam Failed game_role_id is empty!");
		return repid_not_set_client_role;
	}

	if (player_id == 0 || member_count <= 0 || (caree_id <= 0 || caree_id > 20) || string_tool::SplitStrByFlag<std::wstring>(team_flag,L",").size() < member_count )
		return repid_client_req_error;

	auto team_ptr = CMatchSuccessTeamMgr::GetInstance().GetTeamByGameRoleID(m_game_role_id);
	if (team_ptr)
		return repid_team_already_exsit;

		
	CJoinTeamRequestMgr::GetInstance().PushRequest({ { m_game_role_id.GetRoleName() }, player_id, (DWORD)0, caree_id, { member_count, fuben_name, team_flag, false } });
	return repid_ok;
}

reply_result CClientSession::OnRequestJoinTeamCrossServer(DWORD house_id, const std::wstring & fuben_name, int member_count, int caree_id, const std::wstring & team_flag)
{
	if (m_game_role_id.GetHash() == 0)
	{
		OutputDebugStr(L"!!!OnRequestJoinTeamCrossServer Failed game_role_id is empty!");
		return repid_not_set_client_role;
	}

	if (house_id == 0 || member_count <= 0 || (caree_id <= 0 || caree_id > 20) || string_tool::SplitStrByFlag<std::wstring>(team_flag, L",").size() < member_count)
		return repid_client_req_error;

	auto team_ptr = CMatchSuccessTeamMgr::GetInstance().GetTeamByGameRoleID(m_game_role_id);
	if (team_ptr)
		return repid_team_already_exsit;
	CJoinTeamRequestMgr::GetInstance().PushRequest({ { m_game_role_id.GetRoleName() }, (DWORD)0, house_id,caree_id, { member_count, fuben_name, team_flag, true } });
	return repid_ok;
}







//通信线程调用
void CClientSession::OnClientDisconnect()
{
	if (!m_game_role_id)
	{
		OutputDebugStr(L"!!!OnClientDisconnect Failed game_role_id is empty!");
		return;
	}
	CJoinTeamRequestMgr::GetInstance().DeleteRequest(m_game_role_id);
}

