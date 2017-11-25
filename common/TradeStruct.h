#pragma once

#include <windows.h>


enum request_msg_id
{
	reqid_login_account = 1000,
	reqid_heartbeat,
	reqid_request_team,
	reqid_request_team_cross_server,
	reqid_get_reqteam_result,
	reqid_get_cross_server_reqteam_result,
	reqid_dismiss_team,//解散队伍
	reqid_client_join_team_failed,
	reqid_client_join_team_success,
	reqid_set_game_role_id,
	reqid_create_team,
	reqid_get_team_info,
	reqid_cancel_join_team_req,
	reqid_set_member_data,
	reqid_get_member_data,
	reqid_client_touch_server,
};






enum reply_result
{
	repid_failed = 0, //失败
	repid_ok = 1,     //成功
	repid_not_logined, //未登录
	repid_unknow_error,//未知错误
	repid_client_error,//未知客户端错误
	repid_team_already_exsit,//请求组队时队伍已经存在
	repid_not_set_client_role,//还未设置角色ID,
	repid_unknow_msg,//未各的交易ID
	repid_client_req_error,//客户端请求的数据有误
	repid_not_exist,//不存在
	repid_team_id_is_zero,
	repid_user_expired,
};

#define IS_OK(x) (x == repid_ok)
#define IS_FAIL(x) (x != repid_ok)
namespace trade_struct
{
	const wchar_t * GetErrCodeMsg(reply_result id);
}

#define GAMEID  3

