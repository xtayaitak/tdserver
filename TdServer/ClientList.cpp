#include "stdafx.h"
#include "ClientList.h"
#include "SocketSession.h"
#include "DebugOutput.h"
#include <StringTool.h>
#include <iostream>



CSessionList::CSessionList()
{
}


CSessionList::~CSessionList()
{
}

bool CSessionList::CanJoin(const std::wstring & id) 
{
	std::lock_guard<std::mutex> l(m_client_list_mutex);
	return (m_client_list.find(id) == m_client_list.end());
}

bool CSessionList::Join(SessionPtr client_ptr)
{
	std::lock_guard<std::mutex> l(m_client_list_mutex);
	auto iter = m_client_list.find(client_ptr->GetClientID());
	if (iter == m_client_list.end())
	{
		OutputDebugStr(L"client joined:%s", client_ptr->GetClientID().c_str());
		m_client_list.insert(std::pair<std::wstring, SessionPtr>(client_ptr->GetClientID(), client_ptr));
		return true;
	}
	else
		return false;

}
void CSessionList::Leave(SessionPtr client_ptr)
{
	std::lock_guard<std::mutex> l(m_client_list_mutex);
	auto iter = m_client_list.find(client_ptr->GetClientID());
	if (iter != m_client_list.end())
	{
		OutputDebugStr(L"client Leave:%s", client_ptr->GetClientID().c_str());
		m_client_list.erase(iter);
	}

}

void CSessionList::CheckSessionTimeOut()
{
	std::lock_guard<std::mutex> l(m_client_list_mutex);
	for (auto iter = m_client_list.begin(); iter != m_client_list.end(); ++iter)
	{
		if (iter->second->IsTimeOut())
		{
			iter->second->Close();
			std::wcout << L"client:" << iter->second->GetClientID() << L"³¬Ê±!" << std::endl;
		}
	}
}

void CSessionList::ClientListDo(std::function<void(SessionPtr & ptr)> fn)
{
	std::lock_guard<std::mutex> l(m_client_list_mutex);
	for (auto & iter : m_client_list)
	{
		fn(iter.second);
	}
}