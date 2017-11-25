#pragma once

#include <windows.h>


enum request_msg_id
{
	id_login_account = 1000,
	id_heartbeat
};

enum reply_msg_id
{
	id_failed = 0,
	id_ok = 1,
	id_unknow_msg,
	id_unknow_error,
};




#define GAMEID_TD 3

