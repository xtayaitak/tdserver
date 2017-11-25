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
	reqid_dismiss_team,//��ɢ����
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
	repid_failed = 0, //ʧ��
	repid_ok = 1,     //�ɹ�
	repid_not_logined, //δ��¼
	repid_unknow_error,//δ֪����
	repid_client_error,//δ֪�ͻ��˴���
	repid_team_already_exsit,//�������ʱ�����Ѿ�����
	repid_not_set_client_role,//��δ���ý�ɫID,
	repid_unknow_msg,//δ���Ľ���ID
	repid_client_req_error,//�ͻ����������������
	repid_not_exist,//������
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

