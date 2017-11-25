#pragma once

#include <set>
#include <memory>

class CSocketSession;

typedef std::shared_ptr<CSocketSession> SessionPtr;

class CSessionList
{
public:
	CSessionList();
	~CSessionList();
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

