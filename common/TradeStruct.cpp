#include "stdafx.h"

#include "TradeStruct.h"


namespace trade_struct
{
	const wchar_t * GetErrCodeMsg(reply_result id)
	{
		switch (id)
		{
		case repid_failed:
			return L"����ʧ��";
			break;
		case repid_ok:
			return L"�����ɹ�";
			break;
		case repid_not_logined:
			return L"��δ��¼���������޷����д˲���";
			break;
		case repid_unknow_error:
			return L"δ֪����";
			break;
		case repid_client_error:
			return L"�ͻ��˳���";
			break;
		case repid_team_already_exsit:
			return L"�����Ѿ�����";
			break;
		case repid_not_set_client_role:
			return L"��δ���ý�ɫID�޷����д˲���";
			break;
		case repid_unknow_msg:
			return L"δ֪�Ľ���ID";
			break;
		case repid_client_req_error:
			return L"�ͻ������������д���";
		case repid_not_exist:
			return L"������";
		case repid_team_id_is_zero:
			return L"����IDΪ��";
		case repid_user_expired:
			return L"�˺��ѵ���";
		}

		return L"δ֪";
	}
}