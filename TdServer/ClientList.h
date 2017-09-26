#pragma once

#include <set>
#include <memory>

class CClientSession;

typedef std::shared_ptr<CClientSession> SessionPtr;

class CClientList
{
public:
	CClientList();
	~CClientList();
	void Join(SessionPtr client_ptr)
	{
		client_list.insert(client_ptr);
	}
	void Leave(SessionPtr client_ptr)
	{
		client_list.erase(client_ptr);
	}

private:
	std::set<SessionPtr> client_list;
};

