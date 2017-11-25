
// ClientTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ClientTest.h"
#include "ClientTestDlg.h"
#include "afxdialogex.h"
#include <vector>
#include "ProcessTool.h"
#include <boost/assert.hpp>
#include "StringTool.h"

#include "BnsClient.h"
#include <thread>




#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TimerID_GetTeamInfo 1000
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CClientTestDlg �Ի���




CClientTestDlg::CClientTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_listbox);
}

BEGIN_MESSAGE_MAP(CClientTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CClientTestDlg::OnBnClickedButtonLogin)
	ON_BN_CLICKED(IDC_BUTTON_SET_ROLE_ID, &CClientTestDlg::OnBnClickedButtonSetRoleId)
	ON_BN_CLICKED(IDC_BUTTON_REQ_JON_TEAM, &CClientTestDlg::OnBnClickedButtonReqJonTeam)
	ON_BN_CLICKED(IDC_BUTTON_REQ_CROSS_SERVER_JOIN_TEAM, &CClientTestDlg::OnBnClickedButtonReqCrossServerJoinTeam)
	ON_BN_CLICKED(IDC_BUTTON_GET_TEAM_INFO, &CClientTestDlg::OnBnClickedButtonGetTeamInfo)
	ON_BN_CLICKED(IDC_BUTTON_TEAM_DISMISS, &CClientTestDlg::OnBnClickedButtonTeamDismiss)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CREATE_SERVER_TEAM, &CClientTestDlg::OnBnClickedButtonCreateServerTeam)
	ON_BN_CLICKED(IDC_BUTTON_GET_SERVER_TEAM, &CClientTestDlg::OnBnClickedButtonGetServerTeam)
	ON_BN_CLICKED(IDC_BUTTON_SET_REMOTE_TEAM_MEMBER_DATA, &CClientTestDlg::OnBnClickedButtonSetRemoteTeamMemberData)
	ON_BN_CLICKED(IDC_BUTTON_GET_TEAM_MEMBER_DATA, &CClientTestDlg::OnBnClickedButtonGetTeamMemberData)
	ON_BN_CLICKED(IDC_BUTTON3, &CClientTestDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON_SET_IP, &CClientTestDlg::OnBnClickedButtonSetIp)
END_MESSAGE_MAP()


// CClientTestDlg ��Ϣ�������

BOOL CClientTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	CBnsClient::CreateInstance();

	SetDlgItemText(IDC_EDIT_IP, L"120.25.82.241");
	CBnsClient::GetInstance()->SetServerIp("120.25.82.241", 10102);


	const std::vector<std::wstring> g_cards =
	{
		L"31AB5C3B8BF153CC387D123564845491",
		L"6BE00C6C51BE2BD30EC2B98F8DCEEE55",
		L"2535609348101C8D10705D43C6ABF95F",
		L"010A66D6A945698FFF390CCA1AA98F50",
		L"8652D4E3A791527C093E2C5762F48974",
		L"8A254D317CD93C3E2A8A8690FBBCACEA",
	};

	auto pids = process_tool::GetPidsFromExeName(L"ClientTest.exe");
	BOOST_ASSERT_MSG(pids.size() <= g_cards.size(),L"���Ų���");
	std::wstring card_id = g_cards[pids.size() - 1];

	SetDlgItemText(IDC_EDIT_USER_ID, card_id.c_str());
	SetDlgItemText(IDC_EDIT_GAME_ROLEID, std::to_wstring(GetCurrentProcessId()).c_str());
	SetDlgItemInt(IDC_EDIT_ROLE_ID, GetCurrentProcessId());
	SetDlgItemText(IDC_EDIT_FUBEN_NAME, L"���Ը���");
	SetDlgItemInt(IDC_EDIT_TEAM_COUNT, 6);
	SetDlgItemText(IDC_EDIT_TEAM_CAREE_FLAG, L"1,5,7,3,8,9");


	//SetDlgItemText(IDC_EDIT_SERVER_TEAM_INFO, server_info.c_str());




	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CClientTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CClientTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CClientTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CClientTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent == TimerID_GetTeamInfo)
	{
		GetJoinedTeamInfo(true);
	}
	CDialogEx::OnTimer(nIDEvent);
}


bool CClientTestDlg::GetJoinedTeamInfo(bool timer_do)
{
	std::vector<CBnsClient::JoinTeamResultMember>  team_req_result;
	auto ret_id = CBnsClient::GetInstance()->GetJoinTeamReqResult(team_req_result);
	if (ret_id == repid_ok)
	{
		std::wstring team_result;
		for (auto iter : team_req_result)
		{
			std::wstring member_str = std::wstring(L"[") + std::to_wstring(iter.player_id) + L"," + std::to_wstring(iter.house_id) + L"," + iter.role_name + L"],";
			team_result += member_str;
		}
		SetDlgItemText(IDC_EDIT_TEAM_INFO, team_result.c_str());
	}
	if (timer_do)
	{
		if (ret_id == repid_ok)
		{
			KillTimer(TimerID_GetTeamInfo);
			AddLog(trade_struct::GetErrCodeMsg(ret_id));
		}
		else
			;
	}
	else
		AddLog(trade_struct::GetErrCodeMsg(ret_id));
	return ret_id == repid_ok;

}


void CClientTestDlg::OnBnClickedButtonLogin()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString use_id;
	GetDlgItemText(IDC_EDIT_USER_ID, use_id);
	std::wstring out_msg;
	CBnsClient::GetInstance()->LoginUser((LPCTSTR)use_id, L"", L"232323", out_msg);
	AddLog(out_msg.c_str());
}


void CClientTestDlg::OnBnClickedButtonSetRoleId()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	
	CString role_name;
	GetDlgItemText(IDC_EDIT_GAME_ROLEID, role_name);
	auto id = CBnsClient::GetInstance()->SetGameRoleID((LPCTSTR)role_name);
	AddLog(trade_struct::GetErrCodeMsg(id));
}


void CClientTestDlg::OnBnClickedButtonReqJonTeam()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	DWORD role_id = GetDlgItemInt(IDC_EDIT_ROLE_ID);
	CString fuben_name, temp_caree_flag;
	char caree =  GetDlgItemInt(IDC_EDIT_ROLE_CAREE);
	int team_count = GetDlgItemInt(IDC_EDIT_TEAM_COUNT);
	GetDlgItemText(IDC_EDIT_FUBEN_NAME, fuben_name);
	GetDlgItemText(IDC_EDIT_TEAM_CAREE_FLAG, temp_caree_flag);

	auto rep_id = CBnsClient::GetInstance()->RequestJoinTeam(role_id, caree, (LPCTSTR)fuben_name, team_count, (LPCTSTR)temp_caree_flag);
	if (rep_id == repid_ok)
	{
		SetTimer(TimerID_GetTeamInfo, 5000, NULL);
	}
	AddLog(trade_struct::GetErrCodeMsg(rep_id));
}


void CClientTestDlg::OnBnClickedButtonReqCrossServerJoinTeam()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	DWORD house_id = GetDlgItemInt(IDC_EDIT_HOUSE_ID);
	CString fuben_name, temp_caree_flag;
	char caree = GetDlgItemInt(IDC_EDIT_ROLE_CAREE);
	int team_count = GetDlgItemInt(IDC_EDIT_TEAM_COUNT);
	GetDlgItemText(IDC_EDIT_FUBEN_NAME, fuben_name);
	GetDlgItemText(IDC_EDIT_TEAM_CAREE_FLAG, temp_caree_flag);

	auto rep_id = CBnsClient::GetInstance()->RequestJoinTeamCrossServer(house_id, caree, (LPCTSTR)fuben_name, team_count, (LPCTSTR)temp_caree_flag);
	AddLog(trade_struct::GetErrCodeMsg(rep_id));
}
void CClientTestDlg::OnBnClickedButtonGetTeamInfo()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	GetJoinedTeamInfo();
}

void CClientTestDlg::OnBnClickedButtonTeamDismiss()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	AddLog(trade_struct::GetErrCodeMsg(CBnsClient::GetInstance()->DismissTeam()));
}


void CClientTestDlg::AddLog(const CString & str)
{
	int index = m_listbox.AddString(str);
	m_listbox.SetCurSel(index);
}



std::vector<CServerTeamMember> GetServerTeamByString(const std::wstring & team_info_str)
{
	std::vector<CServerTeamMember> member_list;
	auto team_member_str_list = string_tool::SplitStrByFlag<std::wstring>(team_info_str, L"|");
	for (auto member_str : team_member_str_list)
	{
		if (member_str.empty())
			continue;
		auto member_item_list = string_tool::SplitStrByFlag<std::wstring>(member_str, L",");
		CServerTeamMember member_obj;
		BOOST_ASSERT_MSG(member_item_list.size() >= 3, L"������Ϣ�Ƿ�");
		for (unsigned i = 0; i < member_item_list.size(); i++)
		{
			if (i == 0)
				member_obj.SetRoleName(member_item_list[i]);
			else if (i == 1)
				member_obj.SetPlayerID(std::stoi(member_item_list[i]));
			else if (i == 2)
				member_obj.SetHouseID(std::stoi(member_item_list[i]));
		}
		member_list.push_back(member_obj);
	}

	return member_list;
}

std::wstring GetStrByServerTeam(std::vector<CServerTeamMember> & members)
{
	std::wstring line;
	for (auto iter : members)
	{
		std::wstring member_line = iter.GetRoleName() + L"," + std::to_wstring(iter.GetPlayerID()) + L"," + std::to_wstring(iter.GetHouseID());
		line += member_line + L"|";
	}
	return line;
}



void CClientTestDlg::OnBnClickedButtonCreateServerTeam()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString team_info_str;
	GetDlgItemText(IDC_EDIT_SERVER_TEAM_INFO, team_info_str);
	//std::wstring server_info = L"role_name, playerid, house_id| role_name,playerid, house_id";
	auto member_list = GetServerTeamByString((LPCTSTR)team_info_str);
	BOOST_ASSERT_MSG(member_list.size() != 0, L"������Ϣ�Ƿ�");
	auto rep_id = CBnsClient::GetInstance()->RequestCreateTeam(member_list);
	AddLog(trade_struct::GetErrCodeMsg(rep_id));
}


void CClientTestDlg::OnBnClickedButtonGetServerTeam()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	std::vector<CServerTeamMember> member_list;
	auto rep_id = CBnsClient::GetInstance()->RequestGetTeamInfo(member_list);
	AddLog(trade_struct::GetErrCodeMsg(rep_id));
	if (rep_id == repid_ok)
	{
		SetDlgItemText(IDC_EDIT_SERVER_TEAM_INFO, GetStrByServerTeam(member_list).c_str());
	}

}


void CClientTestDlg::OnBnClickedButtonSetRemoteTeamMemberData()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString key;
	GetDlgItemText(IDC_EDIT_TEAM_MEMBER_NAME, key);
	int val = GetDlgItemInt(IDC_EDIT_TEAM_MEMBER_DATA);
	auto rep_id =CBnsClient::GetInstance()->RequestSetTeamMemberData(string_tool::WideToChar(key), val);
	AddLog(trade_struct::GetErrCodeMsg(rep_id));
}


void CClientTestDlg::OnBnClickedButtonGetTeamMemberData()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString keyTemp;
	GetDlgItemText(IDC_EDIT_TEAM_MEMBER_NAME, keyTemp);
	std::string key = string_tool::WideToChar(keyTemp);
	std::map<std::string, int> datas = { { key, 0 } };
	auto rep_id = CBnsClient::GetInstance()->RequestGetTeamMemberDataList(datas);
	AddLog(trade_struct::GetErrCodeMsg(rep_id));
	if (IS_OK(rep_id))
		SetDlgItemInt(IDC_EDIT_TEAM_MEMBER_DATA, datas.begin()->second);
}


void CClientTestDlg::OnBnClickedButton3()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	std::thread t([this](){
		std::map<std::string, int> key_val = { { "324124", 12 }, { "vvv", 134 }, { "zz", 12 } };
		for (int i = 0; i < 10000; i++)
		{
			auto rep_id = CBnsClient::GetInstance()->RequestSetTeamMemberDataList(key_val);
			AddLog(trade_struct::GetErrCodeMsg(rep_id));
		}
	});

	t.detach();
}


void CClientTestDlg::OnBnClickedButtonSetIp()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString ip;
	GetDlgItemText(IDC_EDIT_IP, ip);
	CBnsClient::GetInstance()->SetServerIp(string_tool::WideToChar(ip), 10102);
}
