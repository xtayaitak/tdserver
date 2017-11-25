
// ClientTestDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"

// CClientTestDlg �Ի���
class CClientTestDlg : public CDialogEx
{
// ����
public:
	CClientTestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CLIENTTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
