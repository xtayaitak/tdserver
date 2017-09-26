#include "stdafx.h"
#include "DbConnctionPool.h"

#include "DebugOutput.h"

CDbConnctionPool::CDbConnctionPool(void)
{
	for(int i = 0 ;i < CONNECTION_POOL_COUNT;i++)
		m_ConnStatus[i] = FALSE;
	for(int i = 0;i < CONNECTION_POOL_COUNT;i++)
		m_Connections[i] = NULL;
}


CDbConnctionPool::~CDbConnctionPool(void)
{
}


BOOL CDbConnctionPool::Init()
{
	CoInitializeEx( NULL, COINIT_MULTITHREADED );
	for(int i = 0;i < CONNECTION_POOL_COUNT;i++)
	{
		HRESULT hr = m_Connections[i].CreateInstance(__uuidof(ADODB::Connection));
		if(hr != S_OK)
		{
			OutputDebugStr( L"m_Connections[i].CreateInstance(__uuidof(Connection)); Failed");
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CDbConnctionPool::DeInit()
{
	CoUninitialize();
	return TRUE;
}
BOOL CDbConnctionPool::ConnctionDataBase(const wchar_t * szIP,const wchar_t * User,const wchar_t * szPassWord)
{
	wchar_t szConnectionBuffer[1024] = {0};
	_stprintf_s(szConnectionBuffer,L"Provider=SQLOLEDB.1;Password=%s;Persist Security Info=True; \
								 User ID=%s;Initial Catalog=SoftwareVerification;Data Source=%s",szPassWord,User,szIP);
	for(int i = 0;i < CONNECTION_POOL_COUNT;i++)
	{
		try
		{
			HRESULT hr = m_Connections[i]->Open(szConnectionBuffer, "", "", ADODB::adModeUnknown);
			if(hr != S_OK)
			{
				m_ConnStatus[i] = FALSE;
				return FALSE;
			}
			else
			{
				m_ConnStatus[i] = TRUE;
			}
		}
		catch(_com_error e)
		{
			m_ConnStatus[i] = FALSE;
			OutputDebugStr( L"ConnectionDataBase Exception Index:%d ErrorDescription:%s"), i, (const wchar_t *)e.Description();
		}
	}
	return TRUE;
}

VOID CDbConnctionPool::DisconnectionDataBase()
{
	for(int i = 0;i < CONNECTION_POOL_COUNT;i++)
	{
		try
		{
			m_Connections[i]->Close();
			m_ConnStatus[i] = FALSE;
		}
		catch(_com_error e)
		{
			OutputDebugStr(L"DisconnectionDataBase Exception Index:%d ErrorDescription:%s"), i, (const wchar_t *)e.Description();
		}
	}
}

ADODB::_ConnectionPtr & CDbConnctionPool::GetConnectionPtr()
{
	int nIndex = rand() % CONNECTION_POOL_COUNT;
	return m_Connections[nIndex];
}