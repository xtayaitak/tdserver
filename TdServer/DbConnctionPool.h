#pragma once

#include "ClassInstance.h"

#import "C:\Program Files\Common Files\System\ado\msado15.dll" rename("EOF", "adoEOF") rename("BOF", "adoBOF")


#ifdef _DEBUG
#define CONNECTION_POOL_COUNT 1
#else
#define CONNECTION_POOL_COUNT 10
#endif
class CDbConnctionPool : public CClassInstance<CDbConnctionPool>
{
public:
	CDbConnctionPool(void);
	~CDbConnctionPool(void);
	BOOL Init();
	BOOL ConnctionDataBase(const wchar_t * szIP,const wchar_t * User,const wchar_t * szPassWord);
	VOID DisconnectionDataBase();
	BOOL DeInit();
	ADODB::_ConnectionPtr & GetConnectionPtr();
private:
	ADODB::_ConnectionPtr m_Connections[CONNECTION_POOL_COUNT];
	BOOL m_ConnStatus[CONNECTION_POOL_COUNT];
};

