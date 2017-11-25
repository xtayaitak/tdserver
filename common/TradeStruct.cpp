#include "stdafx.h"

#include "TradeStruct.h"


namespace trade_struct
{
	const wchar_t * GetErrCodeMsg(reply_result id)
	{
		switch (id)
		{
		case repid_failed:
			return L"操作失败";
			break;
		case repid_ok:
			return L"操作成功";
			break;
		case repid_not_logined:
			return L"还未登录服务器，无法进行此操作";
			break;
		case repid_unknow_error:
			return L"未知错误";
			break;
		case repid_client_error:
			return L"客户端出错";
			break;
		case repid_team_already_exsit:
			return L"队伍已经存在";
			break;
		case repid_not_set_client_role:
			return L"还未设置角色ID无法进行此操作";
			break;
		case repid_unknow_msg:
			return L"未知的交易ID";
			break;
		case repid_client_req_error:
			return L"客户端请求数据有错误";
		case repid_not_exist:
			return L"不存在";
		case repid_team_id_is_zero:
			return L"队伍ID为零";
		case repid_user_expired:
			return L"账号已到期";
		}

		return L"未知";
	}
}