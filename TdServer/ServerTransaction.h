#pragma once

#include "ClassInstance.h"

#include "DbConnctionPool.h"
#include <string>

class CServerTransaction : public CClassInstance<CServerTransaction>
{
public:
	CServerTransaction();
	~CServerTransaction();
	
	

	BOOL OnLoginRequest(const std::wstring & user_name,const std::wstring & machine_md5);

	CDbConnctionPool m_db_pool;
};

