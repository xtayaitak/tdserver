#include "stdafx.h"
#include "ServerTeamMember.h"




CServerTeamMember::~CServerTeamMember()
{

}


void CServerTeamMember::SetData(const std::string & key, int val)
{
	if (key == "m_step")
		m_step = val;
	else if (key == "m_step_finished")
		m_step_finished = (val > 0);
	else
		m_values[key] = val;
}

int CServerTeamMember::GetData(const std::string & key)
{
	if (key == "m_step")
		return m_step;
	else if (key == "m_step_finished")
		return m_step_finished ? 1 : 0;
	else
		return m_values[key];
}