
// ClientTestDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

// CClientTestDlg 对话框
class CClientTestDlg : public CDialogEx
{
// 构造
public:
	CClientTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CLIENTTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listbox;
	afx_msg void OnBnClickedButtonLogin();
	afx_msg void OnBnClickedButtonSetRoleId();
	afx_msg void OnBnClickedButtonReqJonTeam();
	afx_msg void OnBnClickedButtonReqCrossServerJoinTeam();
	afx_msg void OnBnClickedButtonGetTeamInfo();
	afx_msg void OnBnClickedButtonTeamDismiss();
	void AddLog(const CString & str);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	bool GetJoinedTeamInfo(bool timer_do = false);
	afx_msg void OnBnClickedButtonCreateServerTeam();
	afx_msg void OnBnClickedButtonGetServerTeam();
	afx_msg void OnBnClickedButtonSetRemoteTeamMemberData();
	afx_msg void OnBnClickedButtonGetTeamMemberData();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButtonSetIp();
};
