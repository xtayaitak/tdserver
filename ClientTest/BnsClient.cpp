#include "stdafx.h"
#include "BnsClient.h"
#include "TradeStruct.h"

#include <msgpack.hpp>

#include "StringTool.h"

#include "MsgpackMap.h"
#include "DebugOutput.h"

#include "msgpack_wstring.h"

#include <boost/bind.hpp>

#include <thread>

std::shared_ptr<CBnsClient> CBnsClient::m_client_ptr = nullptr;
CBnsClient::~CBnsClient()
{
}

bool CBnsClient::LoginUser(const std::wstring & id,const std::wstring & pwd, const std::wstring & machine_md5,std::wstring & out_msg)
{
	try
	{
		msgpack::sbuffer sBuf;
		{
			msgpack::packer<msgpack::sbuffer> pker(&sBuf);
			pker.pack_map(4);
			pker.pack("MSG_ID"); pker.pack((int)reqid_login_account);
			pker.pack("CLIENT_ID"); pker.pack(id);
			pker.pack("PWD"); pker.pack(pwd);
			pker.pack("MC_INFO"); pker.pack(machine_md5);
		}
		if (!Send(sBuf.data(), sBuf.size()))
		{
			OutputDebugStr(L"client send failed!");
			return false;
		}
		msgpack::object_handle obj_handle;
		if (Recv(obj_handle))
		{
			const msgpack::object  &obj = obj_handle.get();
			CMsgpackMap msg_map(obj);
			int ret_code = msg_map.GetValue<int>("RET_CODE");
			out_msg = msg_map.GetValue<std::wstring>("MSG");
			return ret_code == repid_ok;
		}
		else
		{
			OutputDebugStr(L"Client read failed");
			return false;
		}
	}
	catch (std::exception & e)
	{
		OutputDebugStrA("login user failed:%s",e.what());
		return false;
	}
	
}

reply_result CBnsClient::RequestJoinTeamCrossServer(DWORD house_id, int caree_id, const std::wstring & fuben_name, int team_count,  const std::wstring & caree_group)
{
	try
	{
		msgpack::sbuffer sBuf;
		{
			msgpack::packer<msgpack::sbuffer> pker(&sBuf);
			pker.pack_map(6);
			pker.pack("MSG_ID"); pker.pack((int)reqid_request_team_cross_server);
			pker.pack("HOUSE_ID"); pker.pack(house_id);
			pker.pack("CAREE_ID"); pker.pack(caree_id);
			pker.pack("TEAM_COUNT"); pker.pack(team_count);
			pker.pack("CAREE_FLAG"); pker.pack(caree_group);
			pker.pack("FUBEN_NAME"); pker.pack(fuben_name);
		}
		msgpack::object_handle obj_handle;
		if (!SendAndRecv(sBuf.data(), sBuf.size(), obj_handle))
		{
			OutputDebugStr(L"client send failed!");
			return repid_client_error;
		}
		else
		{
			return (reply_result)obj_handle.get().via.map.ptr->val.as<int>();
		}
	}
	catch (std::exception & e)
	{
		OutputDebugStrA("login user failed:%s", e.what());
		return repid_client_error;
	}
}



reply_result CBnsClient::RequestJoinTeam(DWORD player_id, int caree_id, const std::wstring & fuben_name, int team_count, const std::wstring & caree_group)
{
	try
	{
		msgpack::sbuffer sBuf;
		{
			msgpack::packer<msgpack::sbuffer> pker(&sBuf);
			pker.pack_map(6);
			pker.pack("MSG_ID"); pker.pack((int)reqid_request_team);
			pker.pack("PLAYER_ID"); pker.pack(player_id);
			pker.pack("CAREE_ID"); pker.pack(caree_id);
			pker.pack("TEAM_COUNT"); pker.pack(team_count);
			pker.pack("CAREE_FLAG"); pker.pack(caree_group);
			pker.pack("FUBEN_NAME"); pker.pack(fuben_name);
		}
		msgpack::object_handle obj_handle;
		if (!SendAndRecv(sBuf.data(), sBuf.size(), obj_handle))
		{
			OutputDebugStr(L"client send failed!");
			return repid_client_error;
		}
		else
			return (reply_result)obj_handle.get().via.map.ptr->val.as<int>();
	}
	catch (std::exception & e)
	{
		OutputDebugStrA("login user failed:%s", e.what());
		return repid_client_error;
	}
}
reply_result CBnsClient::GetJoinTeamReqResult(std::vector<JoinTeamResultMember> & team_req_result)
{
	try
	{
		msgpack::sbuffer sBuf;
		{
			msgpack::packer<msgpack::sbuffer> pker(&sBuf);
			pker.pack_map(1);
			pker.pack("MSG_ID"); pker.pack((int)reqid_get_reqteam_result);
		}
		msgpack::object_handle obj_handle;
		if (!SendAndRecv(sBuf.data(), sBuf.size(), obj_handle))
		{
			OutputDebugStr(L"client send failed!");
			return repid_client_error;
		}
		else
		{
			const msgpack::object  &obj = obj_handle.get();
			if (obj.type != msgpack::type::MAP)
				throw msgpack::type_error();
		
			CMsgpackMap msg_map(obj);
			if (msg_map.GetValue<int>("RET_CODE") == repid_ok)
			{
				m_team_id = msg_map.GetValue<size_t>("TEAM_ID");
				const msgpack::object  & team_member_object = msg_map.GetMsgpackObject("TEAM_INFO");
				if (team_member_object.type != msgpack::type::ARRAY) 
					throw msgpack::type_error();
				const auto array_count = team_member_object.via.array.size;
				for (unsigned int i = 0; i < array_count; i++)
				{
					const msgpack::object & member_info = team_member_object.via.array.ptr[i];
					DWORD player_id = member_info.via.array.ptr[0].as<DWORD>();
					DWORD house_id = member_info.via.array.ptr[1].as<DWORD>();
					std::wstring role_name = member_info.via.array.ptr[2].as<std::wstring>();
					team_req_result.push_back({ player_id,house_id,role_name });
				}
			}

			return (reply_result)msg_map.GetValue<int>("RET_CODE");

		}
	}
	catch (std::exception & e)
	{
		OutputDebugStrA("login user failed:%s", e.what());
		return repid_client_error;
	}
}




reply_result CBnsClient::DismissTeam()
{
	try
	{
		msgpack::sbuffer sBuf;
		{
			msgpack::packer<msgpack::sbuffer> pker(&sBuf);
			pker.pack_map(2);
			pker.pack("MSG_ID"); pker.pack((int)reqid_dismiss_team);
			pker.pack("TEAM_ID"); pker.pack(m_team_id);
		}
		msgpack::object_handle obj_handle;
		if (!SendAndRecv(sBuf.data(), sBuf.size(), obj_handle))
		{
			OutputDebugStr(L"client send failed!");
			return repid_client_error;
		}
		else
		{
			auto repid = (reply_result)obj_handle.get().via.map.ptr->val.as<int>();
			if (repid_ok == repid)
				m_team_id = 0;
			return repid;
		}
	}
	catch (std::exception & e)
	{
		OutputDebugStrA("login user failed:%s", e.what());
		return repid_client_error;
	}
}

reply_result CBnsClient::JoinTeamFailed()
{
	try
	{
		msgpack::sbuffer sBuf;
		{
			msgpack::packer<msgpack::sbuffer> pker(&sBuf);
			pker.pack_map(2);
			pker.pack("MSG_ID"); pker.pack((int)reqid_client_join_team_failed);
			pker.pack("TEAM_ID"); pker.pack(m_team_id);
		}
		msgpack::object_handle obj_handle;
		if (!SendAndRecv(sBuf.data(), sBuf.size(), obj_handle))
		{
			OutputDebugStr(L"client send failed!");
			return repid_client_error;
		}
		else
		{
			auto repid = (reply_result)obj_handle.get().via.map.ptr->val.as<int>();
			return repid;
		}
	}
	catch (std::exception & e)
	{
		OutputDebugStrA("login user failed:%s", e.what());
		return repid_client_error;
	}
}

reply_result CBnsClient::SetGameRoleID(const std::wstring & role_name)
{
	if (role_name.empty())
		return repid_client_req_error;
	try
	{
		msgpack::sbuffer sBuf;
		{
			msgpack::packer<msgpack::sbuffer> pker(&sBuf);
			pker.pack_map(2);
			pker.pack("MSG_ID"); pker.pack((int)reqid_set_game_role_id);
			pker.pack("ROLE_NAME"); pker.pack(role_name);
		}
		msgpack::object_handle obj_handle;
		if (!SendAndRecv(sBuf.data(), sBuf.size(), obj_handle))
		{
			OutputDebugStr(L"client send failed!");
			return repid_client_error;
		}
		else
		{
			m_role_id_set = true;
			return repid_ok;
		}
	}
	catch (std::exception & e)
	{
		OutputDebugStrA("login user failed:%s", e.what());
		return repid_client_error;
	}
}

reply_result CBnsClient::RequestCreateTeam(std::vector<CServerTeamMember> members, size_t * pTeamId)
{
	try
	{
		msgpack::sbuffer sBuf;
		{
			msgpack::packer<msgpack::sbuffer> pker(&sBuf);
			pker.pack_map(2);
			pker.pack("MSG_ID"); pker.pack((int)reqid_create_team);
			pker.pack("TEAM_INFO"); pker.pack(members);
		}
		msgpack::object_handle obj_handle;
		if (!SendAndRecv(sBuf.data(), sBuf.size(), obj_handle))
		{
			OutputDebugStr(L"client send failed!");
			return repid_client_error;
		}
		else
		{
			CMsgpackMap msg_map(obj_handle.get());
			reply_result rep_ret = (reply_result)msg_map.GetValue<int>("RET_CODE");
			if (rep_ret == repid_ok)
				m_team_id = msg_map.GetValue<DWORD>("TEAM_ID");
			else
				m_team_id = 0;

			if (pTeamId) *pTeamId = m_team_id;
			return (reply_result)rep_ret;
		}
			
	}
	catch (std::exception & e)
	{
		OutputDebugStrA("login user failed:%s", e.what());
		return repid_client_error;
	}
}

reply_result CBnsClient::RequestGetTeamInfo(std::vector<CServerTeamMember> & members)
{
	try
	{
		msgpack::sbuffer sBuf;
		{
			msgpack::packer<msgpack::sbuffer> pker(&sBuf);
			pker.pack_map(2);
			pker.pack("MSG_ID"); pker.pack((int)reqid_get_team_info);
			pker.pack("TEAM_ID"); pker.pack(m_team_id);//可以为空，此时有可能是人物掉线了
		}
		msgpack::object_handle obj_handle;
		if (!SendAndRecv(sBuf.data(), sBuf.size(), obj_handle))
		{
			OutputDebugStr(L"client send failed!");
			return repid_client_error;
		}
		else
		{
			CMsgpackMap msg_map(obj_handle.get());
			reply_result rep_ret = (reply_result)msg_map.GetValue<int>("RET_CODE");
			if (rep_ret == repid_ok)
			{
				const msgpack::object & team_member_array = msg_map.GetMsgpackObject("TEAM_INFO");
				members = team_member_array.as<std::vector<CServerTeamMember> >();
				if (m_team_id == 0)
					m_team_id = msg_map.GetValue<decltype(m_team_id)>("TEAM_ID");
			}
			return rep_ret;
		}
	}
	catch (std::exception & e)
	{
		OutputDebugStrA("login user failed:%s", e.what());
		return repid_client_error;
	}
}


reply_result CBnsClient::RequestCancelJoinTeamReq()
{
	return CommonIdRequest(reqid_cancel_join_team_req);
}
reply_result CBnsClient::CommonIdRequest(request_msg_id req_id)
{
	try
	{
		msgpack::sbuffer sBuf;
		{
			msgpack::packer<msgpack::sbuffer> pker(&sBuf);
			pker.pack_map(1);
			pker.pack("MSG_ID"); pker.pack((int)req_id);
		}
		msgpack::object_handle obj_handle;
		if (!SendAndRecv(sBuf.data(), sBuf.size(), obj_handle))
		{
			OutputDebugStr(L"client send failed!");
			return repid_client_error;
		}
		else
		{
			return (reply_result)obj_handle.get().via.map.ptr->val.as<int>();
		}
	}
	catch (std::exception & e)
	{
		OutputDebugStrA("login user failed:%s", e.what());
		return repid_client_error;
	}
}

reply_result CBnsClient::RequestSetTeamMemberData(const std::string & key, int val)
{
	std::map<std::string, int> datas = { { key, val } };
	return RequestSetTeamMemberDataList(datas);
}

reply_result CBnsClient::RequestSetTeamMemberDataList(std::map<std::string, int> datas)
{
	try
	{
		if (m_team_id == 0)
			return repid_team_id_is_zero;
		msgpack::sbuffer sBuf;
		{
			msgpack::packer<msgpack::sbuffer> pker(&sBuf);
			pker.pack_map(3);
			pker.pack("MSG_ID"); pker.pack((int)reqid_set_member_data);
			pker.pack("MEMBER_DATA"); pker.pack(datas);
			pker.pack("TEAM_ID"); pker.pack(m_team_id);
		}
		msgpack::object_handle obj_handle;
		if (!SendAndRecv(sBuf.data(), sBuf.size(), obj_handle))
		{
			OutputDebugStr(L"client send failed!");
			return repid_client_error;
		}
		else
		{
			return (reply_result)obj_handle.get().via.map.ptr->val.as<int>();
		}
	}
	catch (std::exception & e)
	{
		OutputDebugStrA("login user failed:%s", e.what());
		return repid_client_error;
	}
}

reply_result CBnsClient::RequestGetTeamMemberDataList(std::map<std::string, int> & datas)
{
	try
	{
		if (m_team_id == 0)
			return repid_team_id_is_zero;
		msgpack::sbuffer sBuf;
		{
			msgpack::packer<msgpack::sbuffer> pker(&sBuf);
			pker.pack_map(3);
			pker.pack("MSG_ID"); pker.pack((int)reqid_get_member_data);
			pker.pack("MEMBER_DATA"); pker.pack(datas);
			pker.pack("TEAM_ID"); pker.pack(m_team_id);
		}
		msgpack::object_handle obj_handle;
		if (!SendAndRecv(sBuf.data(), sBuf.size(), obj_handle))
		{
			OutputDebugStr(L"client send failed!");
			return repid_client_error;
		}
		else
		{
			auto repid = (reply_result)obj_handle.get().via.map.ptr->val.as<int>();
			if (IS_OK(repid))
			{
				CMsgpackMap msg_map(obj_handle.get());
				datas = msg_map.GetMsgpackObject("MEMBER_DATA").as<std::map<std::string, int> >();
			}
			return repid;

		}
	}
	catch (std::exception & e)
	{
		OutputDebugStrA("login user failed:%s", e.what());
		return repid_client_error;
	}
}

reply_result CBnsClient::TouchServer(const CGameRoleID & game_role_id)
{
	try
	{
		msgpack::sbuffer sBuf;
		{
			msgpack::packer<msgpack::sbuffer> pker(&sBuf);
			pker.pack_map(2);
			pker.pack("MSG_ID"); pker.pack((int)reqid_client_touch_server);
			pker.pack("GAME_ROLE_ID"); pker.pack(game_role_id);
		}
		msgpack::object_handle obj_handle;
		if (!SendAndRecv(sBuf.data(), sBuf.size(), obj_handle))
		{
			OutputDebugStr(L"client send failed!");
			return repid_client_error;
		}
		else
		{
			return (reply_result)obj_handle.get().via.map.ptr->val.as<int>();
		}
	}
	catch (std::exception & e)
	{
		OutputDebugStrA("login user failed:%s", e.what());
		return repid_client_error;
	}
}
void CBnsClient::OnHeartbeatTimer(const boost::system::error_code &)
{
	try
	{
		if (m_heart_beat_fn)
			m_heart_beat_fn();
	}
	catch (std::exception & e)
	{
		OutputDebugStrA("Client HeartBeat Excpetion:%s", e.what());;
	}
	m_heartbeat_timer.expires_from_now(boost::posix_time::seconds(30));
	m_heartbeat_timer.async_wait(boost::bind(&CBnsClient::OnHeartbeatTimer, this, boost::asio::placeholders::error));
}
void CBnsClient::StartClientThread(std::function<void(void)> fn)
{
	if (m_heart_beat_fn)
	{
		OutputDebugStr(L"已经创建客户端线程！！");
		return;
	}

	m_heart_beat_fn = fn;
	m_heartbeat_timer.expires_from_now(boost::posix_time::seconds(30));
	m_heartbeat_timer.async_wait(boost::bind(&CBnsClient::OnHeartbeatTimer, this, boost::asio::placeholders::error));
	std::thread t([this](){
		try
		{
			m_io_service.run();
		}
		catch (boost::system::system_error & e)
		{
			OutputDebugStrA("client io service exception:%s", e.what());
		}
		OutputDebugStr(L"client thread exit");
	});
	t.detach();
	OutputDebugStr(L"client io_service exit");
}

void CBnsClient::StopIoService()
{
	m_io_service.stop();
}

reply_result CBnsClient::JoinTeamSucess()
{
	try
	{
		msgpack::sbuffer sBuf;
		{
			msgpack::packer<msgpack::sbuffer> pker(&sBuf);
			pker.pack_map(2);
			pker.pack("MSG_ID"); pker.pack((int)reqid_client_join_team_success);
			pker.pack("TEAM_ID"); pker.pack(m_team_id);
		}
		msgpack::object_handle obj_handle;
		if (!SendAndRecv(sBuf.data(), sBuf.size(), obj_handle))
		{
			OutputDebugStr(L"client send failed!");
			return repid_client_error;
		}
		else
		{
			return (reply_result)obj_handle.get().via.map.ptr->val.as<int>();
		}
	}
	catch (std::exception & e)
	{
		OutputDebugStrA("JoinTeamSucess failed:%s", e.what());
		return repid_client_error;
	}
}