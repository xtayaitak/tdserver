#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <functional>
class CSocketSession;
typedef std::shared_ptr<CSocketSession> SessionPtr;

#include "GameRoleID.h"

class CSessionList
{
public:
	CSessionList();
	~CSessionList();
	bool CanJoin(const std::wstring & id) ;
	bool Join(SessionPtr client_ptr);
	void Leave(SessionPtr client_ptr);
	size_t GetClientCount() const { return m_client_list.size(); }
	void CheckSessionTimeOut();
	void ClientListDo(std::function<void(SessionPtr & ptr)> fn);
private:
	std::mutex m_client_list_mutex;
	std::map<std::wstring, SessionPtr> m_client_list;
};

