// TestTdServer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "TdClient.h"
#include <iostream>


int _tmain(int argc, _TCHAR* argv[])
{
	CTdClient tcp_client("127.0.0.1", 10102);

	//tcp_client.LoginUser(L"1CC1ED1DBA3A6C5D2B458410A0C0A919",L"", L"AFDSALKFJASKDFJFF21");
	std::string temp = "�Ŷ���12345�Ŷ���";
	tcp_client.Send(temp.c_str(), temp.length());
	msgpack::object_handle obj_handle;
	tcp_client.Recv(obj_handle);


	system("pause");
	return 0;
}

