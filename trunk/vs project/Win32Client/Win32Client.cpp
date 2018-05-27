// Win32Client.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdlib.h>    
#include <string>  
#include <iostream>

#include "Win32Client.h"
#include "Common.h"
#include "Utils.h"
#include "tlhelp32.h"
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

//���������
#include "xcgui.h"
#pragma comment(lib,"XCGUI.lib")

HWINDOW hWindow = NULL;
NOTIFYICONDATAW tnd;
HWINDOW hWindowRegit = NULL;
HWINDOW hWindowFz = NULL;

#define  WM_SHOWTASK (WM_USER+100)
#define MAX_LOADSTRING 100
#define MAX_CLNT_SIZE  7
static int g_ClntPidMap[MAX_CLNT_SIZE] = { 0, 0, 0, 0, 0, 0, 0 };
static int g_CurClntCount = 0;

//////////////////////////////////////////////////////////////////////////////
static CString ServerAddr;
static char pServerAddr[64] = {0};
static CString ServerPort;
static int iServerPort = 0;
static CString GameVersion;
static CString GameStartFile;
static CString ClientUpdateTitile;
static CString ClientProtectTitile;
static CString ClientUpdateFile;
static CString ClientProtectFile;
static CString ClientCopyRight;
static CString ClientTitle;
static CString ClientFile;
static CString KeyHelpFile;
static CString Exprision;
static CString ServerUrl;
static CString RechargeUrl;
static CString MainWebUrl;
static CString DownLoadUrl;
static CString RechargeText;
static CString CarbonText;
static CString LeaderText;
static CString PropagateText;
static CString PropagatePayText;
static CString WarText;
static CString UpdateLogText;
//////////////////////////////////////////////////////////////////////////////
#include <WinSock2.h>
SOCKET sClient;
HELE hRichEditRegitAccount;
HELE hRichEditRegitPasswd;
HELE hRichEditRegitPasswd2;
HELE hRichEditRegitCode;
HXCGUI hPicCode;
CString m_bitmapCode;
HELE hRichEdit;
HELE hComboBoxKey;
CString ClntPid;

// ȫ�ֱ���: 
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

// �˴���ģ���а����ĺ�����ǰ������: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
//-- ���̴���
#if TRUE
#define WM_TRAY (WM_USER + 100)
#define WM_TASKBAR_CREATED RegisterWindowMessage(TEXT("TaskbarCreated"))

#define APP_NAME	TEXT("����ȺӢ��OL")
#define APP_TIP		TEXT("Win32 API ʵ��ϵͳ���̳���")

NOTIFYICONDATA nid;		//��������
HMENU hMenu;			//���̲˵�

//ʵ��������
void InitTray(HINSTANCE hInstance, HWND hWnd)
{
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = IDI_TRAY;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
	nid.uCallbackMessage = WM_TRAY;
	nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRAY));
	lstrcpy(nid.szTip, APP_NAME);

	//hMenu = CreatePopupMenu();//�������̲˵�
	//Ϊ���̲˵��������ѡ��
	//AppendMenu(hMenu, MF_STRING, ID_SHOW, TEXT("��ʾ"));
	//AppendMenu(hMenu, MF_STRING, ID_EXIT, TEXT("�˳�"));

	Shell_NotifyIcon(NIM_ADD, &nid);
}

//��ʾ������������
void ShowTrayMsg(CString msg)
{
	lstrcpy(nid.szInfoTitle, APP_NAME);
	lstrcpy(nid.szInfo, msg);
	nid.uTimeout = 1000;
	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_TRAY:
		switch (lParam)
		{
		case WM_RBUTTONDOWN:
		{
			//��ȡ�������
			POINT pt; GetCursorPos(&pt);

			//����ڲ˵��ⵥ������˵�����ʧ������
			SetForegroundWindow(hWnd);

			//ʹ�˵�ĳ����
			//EnableMenuItem(hMenu, ID_SHOW, MF_GRAYED);	

			//��ʾ����ȡѡ�еĲ˵�
			int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hWnd,
				NULL);
			/*if (cmd == ID_SHOW)
				MessageBox(hWnd, APP_TIP, APP_NAME, MB_OK);
			if (cmd == ID_EXIT)
				PostMessage(hWnd, WM_DESTROY, NULL, NULL);*/
		}
			break;
		case WM_LBUTTONDOWN:
			MessageBox(hWnd, APP_TIP, APP_NAME, MB_OK);
			break;
		case WM_LBUTTONDBLCLK:
			break;
		}
		break;
	case WM_DESTROY:
		//��������ʱɾ������
		Shell_NotifyIcon(NIM_DELETE, &nid);
		PostQuitMessage(0);
		break;
	case WM_TIMER:
		ShowTrayMsg("");
		KillTimer(hWnd, wParam);
		break;
	}
	if (uMsg == WM_TASKBAR_CREATED)
	{
		//ϵͳExplorer��������ʱ�����¼�������
		Shell_NotifyIcon(NIM_ADD, &nid);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

#endif

//--����ʵ�֣�ע�ᣬ�޸����룬��ȡ�汾��
#if TRUE
void CString2Char(CString str, char ch[])
{
	int i;
	char *tmpch;
	int wLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);//�õ�Char�ĳ���
	tmpch = new char[wLen + 1];                                             //��������ĵ�ַ��С
	WideCharToMultiByte(CP_ACP, 0, str, -1, tmpch, wLen, NULL, NULL);       //��CStringת����char*

	for (i = 0; tmpch[i] != '\0'; i++) ch[i] = tmpch[i];
	ch[i] = '\0';
}
void DoRegister(CString name, CString pwd)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	char serverip[64] = { 0 };
	if (Utils::DemainToIp(pServerAddr, serverip) != 0)
	{
		MessageBox(XWnd_GetHWND(hWindow), L"�޷����ӵ�Զ�̷�����", L"��ʾ", MB_OK);
		return;
	}

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);//�����׽��ֿ�
	if (err != 0)
	{
		return;
	}


	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return;
	}

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(serverip);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(iServerPort);

	sClient = socket(AF_INET, SOCK_STREAM, 0);
	if (sClient == INVALID_SOCKET)
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"�����׽���ʧ��", L"��ʾ", MB_OK);
		closesocket(sClient);
		WSACleanup();
		return;

	}
	//�������������������connect����
	if (connect(sClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"���ӷ�����ʧ��", L"��ʾ", MB_OK);
		closesocket(sClient);
		WSACleanup();
		return;
	}

	//����ע����Ϣ
	//SG_MSG msg;
	//memcpy(msg.start, START_FLAG, START_FLAG_LEN);
	//msg.type = REGISTER;
	//srand(time(0));
	//int nRand = rand() % 10 + 1;
	//msg.key = nRand;

	//�򵥼���
	//name = Common::GetInstance()->Encode(msg.key, name);
	//pwd = Common::GetInstance()->Encode(msg.key, pwd);

	//SG_MSG_REG reg_msg;
	//memset(&reg_msg, 0x0, sizeof(SG_MSG_REG));
	//CString2Char(name, reg_msg.name);
	//CString2Char(pwd, reg_msg.passwd);
	//memcpy(msg.buf, &reg_msg, sizeof(reg_msg));

	Rg_Info rg_info;
	Tran_Head tran_head;
	CString2Char(name, rg_info.name);
	CString2Char(pwd, rg_info.passwd);
	srand(time(0));
	int nRand = rand() % 10 + 1;
	CString str = "";
	//str.Format(_T("%d"), nRand);
	str.Format(_T("%d"), 110);

	CString2Char(str, rg_info.key);

	BYTE msg[2048] = { 0 };
	tran_head.cmd = 1;
	tran_head.length = sizeof(rg_info);
	memcpy(msg, &tran_head, sizeof(tran_head));
	memcpy(msg + sizeof(tran_head), &rg_info, sizeof(rg_info));

	//if (send(sClient, (char*)&msg, sizeof(SG_MSG), 0) == SOCKET_ERROR)
	if (send(sClient, (char*)&msg, 2048, 0) == SOCKET_ERROR)
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"����ʧ��", L"��ʾ", MB_OK);
		closesocket(sClient);
		WSACleanup();
		return;
	}

	char szBuff[400];
	memset(szBuff, 0, sizeof(szBuff));
	int ret;
	//���õȵ�ʱ��
	int nNetTimeout = 6000;//3��
	setsockopt(sClient, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(nNetTimeout));
	ret = recv(sClient, szBuff, sizeof(szBuff), 0);
	if (ret > 0)
	{
		CString retError = "";
		retError.Format(_T("%s"), szBuff);
		MessageBox(XWnd_GetHWND(hWindowRegit), retError, L"��ʾ", MB_OK);
	}
	else
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"��������Ӧ��ʱ�������ԣ�", L"��ʾ", MB_OK);
	}
	/*if (ret == sizeof(SG_MSG))
	{
		SG_MSG* msg = (SG_MSG*)szBuff;
		CString _msg = msg->buf;
		MessageBox(XWnd_GetHWND(hWindowRegit), _msg, L"��ʾ", MB_OK);
	}
	else
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"��������Ӧ��ʱ�������ԣ�", L"��ʾ", MB_OK);
	}*/
}
void DoAccMgr(CString name, CString pwd, CString pwd2)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	char serverip[64] = { 0 };
	if (Utils::DemainToIp(pServerAddr, serverip) != 0)
	{
		MessageBox(XWnd_GetHWND(hWindow), L"�޷����ӵ�Զ�̷�����", L"��ʾ", MB_OK);
		return;
	}

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);//�����׽��ֿ�
	if (err != 0)
	{
		return;
	}


	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return;
	}

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(serverip);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(iServerPort);

	sClient = socket(AF_INET, SOCK_STREAM, 0);
	if (sClient == INVALID_SOCKET)
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"�����׽���ʧ��", L"��ʾ", MB_OK);
		closesocket(sClient);
		WSACleanup();
		return;

	}
	//�������������������connect����
	if (connect(sClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"���ӷ�����ʧ��", L"��ʾ", MB_OK);
		closesocket(sClient);
		WSACleanup();
		return;
	}

	Mdfy_Pwd_Info _info;
	Tran_Head tran_head;
	CString2Char(name, _info.name);
	CString2Char(pwd, _info.oldpasswd);
	CString2Char(pwd2, _info.newpasswd);
	srand(time(0));
	int nRand = rand() % 10 + 1;
	CString str = "";
	//str.Format(_T("%d"), nRand);
	str.Format(_T("%d"), 110);

	CString2Char(str, _info.key);

	BYTE msg[2048] = { 0 };
	tran_head.cmd = 3;
	tran_head.length = sizeof(_info);
	memcpy(msg, &tran_head, sizeof(tran_head));
	memcpy(msg + sizeof(tran_head), &_info, sizeof(_info));

	//if (send(sClient, (char*)&msg, sizeof(SG_MSG), 0) == SOCKET_ERROR)
	if (send(sClient, (char*)&msg, 2048, 0) == SOCKET_ERROR)
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"����ʧ��", L"��ʾ", MB_OK);
		closesocket(sClient);
		WSACleanup();
		return;
	}

	char szBuff[400];
	memset(szBuff, 0, sizeof(szBuff));
	int ret;
	//���õȵ�ʱ��
	int nNetTimeout = 6000;//3��
	setsockopt(sClient, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(nNetTimeout));
	ret = recv(sClient, szBuff, sizeof(szBuff), 0);
	if (ret > 0)
	{
		CString retError = "";
		retError.Format(_T("%s"), szBuff);
		MessageBox(XWnd_GetHWND(hWindowRegit), retError, L"��ʾ", MB_OK);
	}
	else
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"��������Ӧ��ʱ�������ԣ�", L"��ʾ", MB_OK);
	}
	//if (ret == sizeof(SG_MSG))
	//{
	//	SG_MSG* msg = (SG_MSG*)szBuff;
	//	CString _msg = msg->buf;
	//	MessageBox(XWnd_GetHWND(hWindowRegit), _msg, L"��ʾ", MB_OK);
	//}
	//else
	//{
	//	MessageBox(XWnd_GetHWND(hWindowRegit), L"��������Ӧ��ʱ�������ԣ�", L"��ʾ", MB_OK);
	//}
}
BOOL DoCheckVersion(CString curVer, CString &newVer)
{
	char serverip[64] = { 0 };
	if (Utils::DemainToIp(pServerAddr, serverip) != 0)
	{
		MessageBox(XWnd_GetHWND(hWindow), L"�޷����ӵ�Զ�̷�����", L"��ʾ", MB_OK);
		return FALSE;
	}

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);//�����׽��ֿ�
	if (err != 0)
	{
		return FALSE;
	}


	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return FALSE;
	}

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(serverip);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(iServerPort);

	sClient = socket(AF_INET, SOCK_STREAM, 0);
	if (sClient == INVALID_SOCKET)
	{
		MessageBox(XWnd_GetHWND(hWindow), L"�����׽���ʧ��", L"��ʾ", MB_OK);
		closesocket(sClient);
		WSACleanup();
		return FALSE;

	}
	unsigned long ul = 1;
	ioctlsocket(sClient, FIONBIO, &ul); //����Ϊ������ģʽ
	//�������������������connect����
	if (connect(sClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		//MessageBox(XWnd_GetHWND(hWindow), L"���ӷ�����ʧ��", L"��ʾ", MB_OK);
		/*closesocket(sClient);
		WSACleanup();
		return FALSE;*/

		timeval tm;
		fd_set set;
		tm.tv_sec = 3;
		tm.tv_usec = 0;
		FD_ZERO(&set);
		FD_SET(sClient, &set);
		if (select(sClient + 1, NULL, &set, NULL, &tm) > 0)
		{
			int error = -1;
			int len = sizeof(int);
			getsockopt(sClient, SOL_SOCKET, SO_ERROR, (char *)&error, /*(socklen_t *)*/&len);
			if (error != 0)
			{
				closesocket(sClient);
				WSACleanup();
				return FALSE;
			}
		}
		else
		{
			closesocket(sClient);
			WSACleanup();
			return FALSE;
		}
	}

	Ver_Info _info;
	Tran_Head tran_head;
	CString2Char("get_version", _info.ver);
	srand(time(0));
	int nRand = rand() % 10 + 1;
	CString str = "";
	//str.Format(_T("%d"), nRand);
	str.Format(_T("%d"), 110);

	BYTE msg[2048] = { 0 };
	tran_head.cmd = 2;
	tran_head.length = sizeof(_info);
	memcpy(msg, &tran_head, sizeof(tran_head));
	memcpy(msg + sizeof(tran_head), &_info, sizeof(_info));

	ul = 0;
	ioctlsocket(sClient, FIONBIO, &ul); //����Ϊ����ģʽ
	//���õȵ�ʱ��
	int nNetTimeout = 3000;//3��
	setsockopt(sClient, SOL_SOCKET, SO_SNDTIMEO, (char*)&nNetTimeout, sizeof(nNetTimeout));
	//if (send(sClient, (char*)&msg, sizeof(SG_MSG), 0) == SOCKET_ERROR)
	if (send(sClient, (char*)&msg, 2048, 0) == SOCKET_ERROR)
	{
		MessageBox(XWnd_GetHWND(hWindow), L"����ʧ��", L"��ʾ", MB_OK);
		closesocket(sClient);
		WSACleanup();
		return FALSE;
	}

	char szBuff[400];
	memset(szBuff, 0, sizeof(szBuff));
	int ret;
	
	setsockopt(sClient, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(nNetTimeout));
	ret = recv(sClient, szBuff, 400, 0);
	if (ret > 0)
	{
		CString retError = "";
		retError.Format(_T("%s"), szBuff);

		CString Ver = retError.Left(retError.Find(_T("�汾111����")));
		if (Ver != curVer)
		{
			newVer = Ver;
			return TRUE;
		}
		else
		{
			newVer = Ver;
			return FALSE;
		}
		//MessageBox(XWnd_GetHWND(hWindow), retError, L"��ʾ", MB_OK);
	}
	else
	{
		return FALSE;
		//MessageBox(XWnd_GetHWND(hWindow), L"��������Ӧ��ʱ�������ԣ�", L"��ʾ", MB_OK);
	}
}
#endif 

//--������֤��
#if TRUE
BOOL  DoCreateBitmap(HDC hDC, HBITMAP hbitmap,
	PBITMAPFILEHEADER &outheadbuf, long *outheadsize,
	PBITMAPINFO &outinfobuf, long *outinfosize,
	LPBYTE &outdatabuf, long *outdatasize)	//���ɵ�ɫλͼ					 
{
	BITMAP bmp;
	WORD cClrBits;
	DWORD my_biClrUsed = 0;
	outinfobuf = NULL;
	outdatabuf = NULL;
	outheadbuf = NULL;

	if (!GetObject(hbitmap, sizeof(BITMAP), (LPSTR)&bmp))
		goto errout;
	bmp.bmPlanes = 1;
	bmp.bmBitsPixel = 1;  //ǿ�Ƹ�ֵת��������ÿ����BIT��
	cClrBits = 1;  //�õ�ÿ���ض���λ		
	*outinfosize = sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)* (1 << cClrBits);
	outinfobuf = (PBITMAPINFO)GlobalAlloc(GPTR, *outinfosize);
	outinfobuf->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); //��Ϣͷ��С��������ɫ�壩	
	outinfobuf->bmiHeader.biWidth = bmp.bmWidth;
	outinfobuf->bmiHeader.biHeight = bmp.bmHeight;
	outinfobuf->bmiHeader.biPlanes = bmp.bmPlanes;
	outinfobuf->bmiHeader.biBitCount = bmp.bmBitsPixel;
	my_biClrUsed = (1 << cClrBits);
	outinfobuf->bmiHeader.biClrUsed = my_biClrUsed;
	outinfobuf->bmiHeader.biCompression = BI_RGB;
	outinfobuf->bmiHeader.biSizeImage = ((outinfobuf->bmiHeader.biWidth * cClrBits + 31) & ~31)\

		/ 8 * outinfobuf->bmiHeader.biHeight;
	//ͼ���С	
	outinfobuf->bmiHeader.biClrImportant = 0;
	/////////////////////////////////�õ�λͼ����	
	// GlobalAlloc����λͼ���ݵ��ڴ�	
	// GetDIBits ����hDC ��HBITMAP�õ�λͼ���ݡ���ɫ������	
	*outdatasize = outinfobuf->bmiHeader.biSizeImage;
	outdatabuf = (LPBYTE)GlobalAlloc(GPTR, *outdatasize);  //����λͼ��С�����ڴ�	
	if (!outdatabuf)
		goto errout;
	if (!GetDIBits(//����DC��BITMAP�õ�λͼ����		
		hDC,
		hbitmap,
		0,
		(WORD)outinfobuf->bmiHeader.biHeight,
		outdatabuf,     // outdatabuf�еõ�λͼ����		
		outinfobuf,
		DIB_RGB_COLORS))

	{
		goto errout;
	}

	/////////////////////////////////�õ��ļ�ͷ	
	*outheadsize = sizeof(BITMAPFILEHEADER);
	outheadbuf = (PBITMAPFILEHEADER)GlobalAlloc(GPTR, *outheadsize);
	//����λͼ��С�����ڴ�	
	if (!outheadbuf)
		goto errout;
	outheadbuf->bfType = 0x4d42;
	outheadbuf->bfSize = (DWORD)(sizeof(BITMAPFILEHEADER)+
		outinfobuf->bmiHeader.biSize +
		my_biClrUsed * sizeof(RGBQUAD)+
		outinfobuf->bmiHeader.biSizeImage);
	outheadbuf->bfReserved1 = 0;
	outheadbuf->bfReserved2 = 0;

	outheadbuf->bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER)+
		outinfobuf->bmiHeader.biSize +
		my_biClrUsed * sizeof (RGBQUAD);
	return TRUE;

	//////////////////////������	
errout:
	if (outinfobuf) GlobalFree(outinfobuf);
	if (outdatabuf) GlobalFree(outdatabuf);
	if (outheadbuf) GlobalFree(outheadbuf);
	outinfobuf = NULL;
	outdatabuf = NULL;
	outheadbuf = NULL;
	*outheadsize = 0;
	*outinfosize = 0;
	*outdatasize = 0;
	return FALSE;
}
BOOL DoCreateCode(CString str, CFile& file)//���ɺ�����֤��
{
	ASSERT(0 == str.GetLength() % 2);

	CWnd* pDesk = CWnd::GetDesktopWindow();
	CDC* pDC = pDesk->GetDC();

	//ÿ���ַ���λ�����ƫ��4
	CRect r(0, 0, 0, 0);
	pDC->DrawText(str, &r, DT_CALCRECT);
	const int w = r.Width() + 4;
	const int h = r.Height() + 4;
	const int iCharWidth = w * 2 / str.GetLength();

	//�����ڴ�DC��λͼ����䱳��
	CBitmap bm;
	bm.CreateCompatibleBitmap(pDC, w, h);
	CDC memdc;
	memdc.CreateCompatibleDC(pDC);
	CBitmap*pOld = memdc.SelectObject(&bm);
	memdc.FillSolidRect(0, 0, w, h, RGB(255, 255, 255));

	::SetGraphicsMode(memdc.m_hDC, GM_ADVANCED);//Ϊ������б��׼��

	for (int i = 0; i < str.GetLength() / 2; i++)
	{
		//��������
		CFont* pFont = memdc.GetCurrentFont();
		LOGFONT logFont;
		pFont->GetLogFont(&logFont);
		logFont.lfOutPrecision = OUT_TT_ONLY_PRECIS;
		logFont.lfOrientation = rand() % 90;
		CFont font;
		font.CreateFontIndirect(&logFont);
		memdc.SelectObject(&font);

		int x = iCharWidth*i + rand() % 5;
		int y = rand() % 5;
		memdc.TextOut(x, y, str.Mid(i * 2, 2));
	}

	//�����ݴ浽�ļ�(CFile��CMemFile)
	PBITMAPFILEHEADER outheadbuf;
	PBITMAPINFO outinfobuf;
	LPBYTE outdatabuf;
	long outheadsize, outinfosize, outdatasize;
	if (!DoCreateBitmap(memdc.m_hDC, bm,
		outheadbuf, &outheadsize,
		outinfobuf, &outinfosize,
		outdatabuf, &outdatasize))
		return FALSE;


	file.Write(outheadbuf, outheadsize);
	file.Write(outinfobuf, outinfosize);
	file.Write(outdatabuf, outdatasize);

	memdc.SelectObject(pOld);
	bm.DeleteObject();
	memdc.DeleteDC();

	return TRUE;
}
void OnCreateCode()
{
	int nRand;
	srand((unsigned)time(NULL));
	nRand = rand() % 1000 + 1000;

	m_bitmapCode.Format(_T("%d"), nRand);

	CString strFileName = ".\\out.bmp";
	CFile file;
	file.Open(strFileName, CFile::modeCreate | CFile::modeWrite);
	DoCreateCode(m_bitmapCode, file);
	file.Close();
	
	HBITMAP bitmap = (HBITMAP)LoadImage(NULL, strFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	HIMAGE codeImg = XImage_LoadFileFromHBITMAP(bitmap);
	XShapePic_SetImage(hPicCode, codeImg);
	XWnd_RedrawWnd(hWindowRegit, TRUE);
}
#endif

//--�Ӵ��ڰ�ť�¼�����
#if TRUE 
int CALLBACK ComboBoxKey_EventChange(BOOL *pbHandled)
{
	//*pbHandled = TRUE;
	wchar_t key[1024] = {};
	XRichEdit_GetText(hComboBoxKey, key, 1024);
	CString _Key = key;
	MessageBox(NULL, _Key, L"��ʾ", MB_OK);
	return 0;
}
int CALLBACK StartFz_EventBtnClick(BOOL *pbHandled)
{
	*pbHandled = TRUE;
	MessageBox(NULL, L"���������У������ڴ���", L"��ʾ", MB_OK);
	return 0;
}
int CALLBACK DoCodeFlush_EventBtnClick(BOOL *pbHandled)
{
	OnCreateCode();

	*pbHandled = TRUE;
	return 0;
}
int CALLBACK DoRegist_EventBtnClick(BOOL *pbHandled)
{
	CString Account;
	CString Passwd;
	CString Passwd2;
	CString Code;
	wchar_t account[64];
	wchar_t passwd[64];
	wchar_t passwd2[64];
	wchar_t code[64];

	XRichEdit_GetText(hRichEditRegitAccount, account, 24);
	XRichEdit_GetText(hRichEditRegitPasswd, passwd, 24);
	XRichEdit_GetText(hRichEditRegitPasswd2, passwd2, 24);
	XRichEdit_GetText(hRichEditRegitCode, code, 24);
	Account = account;
	Passwd = passwd;
	Passwd2 = passwd2;
	Code = code;

	//���
	if (m_bitmapCode != Code)
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"��֤�����", L"��ʾ", MB_OK);
		XRichEdit_SetText(hRichEditRegitCode, L"");
		return -1;
	}
	if (Passwd != Passwd2)
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"�����������벻һ�£���˶ԣ�", L"��ʾ", MB_OK);
		XRichEdit_SetText(hRichEditRegitPasswd, L"");
		XRichEdit_SetText(hRichEditRegitPasswd2, L"");
		return -1;
	}
	//�ַ��Ϸ���
	if (Account.GetAllocLength() > 24 || Account.GetAllocLength() < 6)
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"�˺ű�����6-24λ���ȣ�", L"��ʾ", MB_OK);
		XRichEdit_SetText(hRichEditRegitAccount, L"");
		return -1;
	}

	if (Passwd.GetAllocLength() > 24 || Passwd.GetAllocLength() < 6)
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"���������6-24λ���ȣ�", L"��ʾ", MB_OK);
		XRichEdit_SetText(hRichEditRegitPasswd, L"");
		XRichEdit_SetText(hRichEditRegitPasswd2, L"");
		return -1;
	}

	if (!Common::GetInstance()->FullNumAndWord(Account))
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"�˺ű��������ĸ���������", L"��ʾ", MB_OK);
		XRichEdit_SetText(hRichEditRegitAccount, L"");
		return -1;
	}

	if (!Common::GetInstance()->FullNumAndWord(Passwd))
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"������������ĸ���������", L"��ʾ", MB_OK);
		XRichEdit_SetText(hRichEditRegitPasswd, L"");
		XRichEdit_SetText(hRichEditRegitPasswd2, L"");
		return -1;
	}

	DoRegister(Account, Passwd);
	OnCreateCode();
	//XWnd_RedrawWnd(hWindowRegit, TRUE);
	*pbHandled = TRUE;
	return 0;
}
int CALLBACK DoAccMgr_EventBtnClick(BOOL *pbHandled)
{
	CString Account;
	CString Passwd;
	CString Passwd2;
	CString Code;
	wchar_t account[64];
	wchar_t passwd[64];
	wchar_t passwd2[64];
	wchar_t code[64];

	XRichEdit_GetText(hRichEditRegitAccount, account, 24);
	XRichEdit_GetText(hRichEditRegitPasswd, passwd, 24);
	XRichEdit_GetText(hRichEditRegitPasswd2, passwd2, 24);
	XRichEdit_GetText(hRichEditRegitCode, code, 24);
	Account = account;
	Passwd = passwd;
	Passwd2 = passwd2;
	Code = code;

	//���
	if (m_bitmapCode != Code)
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"��֤�����", L"��ʾ", MB_OK);
		XRichEdit_SetText(hRichEditRegitCode, L"");
		return -1;
	}
	if (Passwd == Passwd2)
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"������������һ�£���˶ԣ�", L"��ʾ", MB_OK);
		return -1;
	}
	//�ַ��Ϸ���
	if (Account.GetAllocLength() > 24 || Account.GetAllocLength() < 6)
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"�˺ű�����6-24λ���ȣ�", L"��ʾ", MB_OK);
		XRichEdit_SetText(hRichEditRegitAccount, L"");
		return -1;
	}

	if (Passwd.GetAllocLength() > 24 || Passwd.GetAllocLength() < 6)
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"���������6-24λ���ȣ�", L"��ʾ", MB_OK);
		XRichEdit_SetText(hRichEditRegitPasswd, L"");
		XRichEdit_SetText(hRichEditRegitPasswd2, L"");
		return -1;
	}

	if (!Common::GetInstance()->FullNumAndWord(Account))
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"�˺ű��������ĸ���������", L"��ʾ", MB_OK);
		XRichEdit_SetText(hRichEditRegitAccount, L"");
		return -1;
	}

	if (!Common::GetInstance()->FullNumAndWord(Passwd))
	{
		MessageBox(XWnd_GetHWND(hWindowRegit), L"������������ĸ���������", L"��ʾ", MB_OK);
		XRichEdit_SetText(hRichEditRegitPasswd, L"");
		XRichEdit_SetText(hRichEditRegitPasswd2, L"");
		return -1;
	}

	DoAccMgr(Account, Passwd, Passwd2);
	OnCreateCode();
	//XWnd_RedrawWnd(hWindowRegit, TRUE);
	*pbHandled = TRUE;
	return 0;
}
#endif

//--�����ڰ�ť�¼�����
#if TRUE //�����ڰ�ť�¼�����
int LoadSrcToEdit(int SRCID)
{
	XRichEdit_SetText(hRichEdit, L"");
	HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(SRCID), _T("TXT"));
	DWORD len = SizeofResource(NULL, hRes);
	HGLOBAL hg = LoadResource(NULL, hRes);

	CString tmp = "";
	tmp = (CHAR*)LockResource(hg);
	XRichEdit_SetText(hRichEdit, tmp);
	FreeResource(hg);

	return 1;
}
int CALLBACK WinClose_EventBtnClick(BOOL *pbHandled)
{
	for (int i = 0; i < MAX_CLNT_SIZE; i++)
	{
		if (g_ClntPidMap[i] != 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, g_ClntPidMap[i]);
			if (hProcess != NULL)
			{
				if (TerminateProcess(hProcess, 0))
				{
					g_ClntPidMap[i] = 0;
					g_CurClntCount--;
				}
			}
		}
	}

	if (hWindow != NULL)
	{
		Shell_NotifyIcon(NIM_DELETE, &nid);//�����������ͼ��
		::SendMessage(XWnd_GetHWND(hWindow), WM_CLOSE, NULL, NULL);
	}
	*pbHandled = TRUE;
	return 0;
}
int CALLBACK WinMin_EventBtnClick(BOOL *pbHandled)
{
	ShowTrayMsg("��¼�����������");
	XWnd_ShowWindow(hWindow, SW_HIDE);//SW_MINIMIZE
	//ShowWindow(XWnd_GetHWND(hWindow), SW_HIDE);

	*pbHandled = TRUE;
	return 0;
}
int CALLBACK MainWinShow_EventBtnClick(WPARAM wParam, LPARAM IParam)//BOOL *pbHandled
{
	if ((IParam == WM_LBUTTONDOWN) || (IParam == WM_RBUTTONDOWN))
	{
		XWnd_ShowWindow(hWindow, SW_SHOW);
	}
	//*pbHandled = TRUE;
	return 0;
}
int CALLBACK Regist_EventBtnClick(BOOL *pbHandled)
{
	hWindowRegit = XModalWnd_Create(383, 230, L"�û�ע��", XWnd_GetHWND(hWindow));

	XShapeText_Create(12, 6, 0, 0, L"�û�ע��", hWindowRegit);
	XShapeText_Create(88, 54, 60, 20, L"ע���˻�:", hWindowRegit);
	XShapeText_Create(88, 82, 60, 20, L"ע������:", hWindowRegit);
	XShapeText_Create(88, 110, 60, 20, L"ȷ������:", hWindowRegit);
	XShapeText_Create(100, 139, 60, 20, L"��֤��:", hWindowRegit);

	hRichEditRegitAccount = XRichEdit_Create(167, 54, 135, 20, hWindowRegit);
	hRichEditRegitPasswd = XRichEdit_Create(167, 82, 135, 20, hWindowRegit);
	hRichEditRegitPasswd2 = XRichEdit_Create(167, 110, 135, 20, hWindowRegit);
	XRichEdit_EnablePassword(hRichEditRegitPasswd, TRUE);
	XRichEdit_EnablePassword(hRichEditRegitPasswd2, TRUE);

	hRichEditRegitCode = XRichEdit_Create(167, 139, 51, 20, hWindowRegit);
	hPicCode = XShapePic_Create(226, 139, 76, 20, hWindowRegit);

	HELE hBtnDoRegist = XBtn_Create(160, 170, 72, 29, L"ȷ��ע��", hWindowRegit);
	XEle_RegEventC(hBtnDoRegist, XE_BNCLICK, DoRegist_EventBtnClick);

	HELE hBtnCodeFlush = XBtn_Create(306, 140, 20, 20, L"", hWindowRegit);
	HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(IDB_PNG1), _T("PNG"));
	DWORD len = SizeofResource(NULL, hRes);
	HGLOBAL hg = LoadResource(NULL, hRes);
	LPVOID lp = (LPSTR)LockResource(hg);
	XBtn_AddBkImage(hBtnCodeFlush, button_state_stay, XImage_LoadMemory(lp, len, TRUE));
	XBtn_AddBkImage(hBtnCodeFlush, button_state_leave, XImage_LoadMemory(lp, len, TRUE));
	XBtn_AddBkImage(hBtnCodeFlush, button_state_down, XImage_LoadMemory(lp, len, TRUE));
	XBtn_AddBkImage(hBtnCodeFlush, button_state_check, XImage_LoadMemory(lp, len, TRUE));
	XBtn_SetStyle(hBtnCodeFlush, button_style_scrollbar_slider);
	XEle_RegEventC(hBtnCodeFlush, XE_BNCLICK, DoCodeFlush_EventBtnClick);

	HELE hBtnWinClose = XBtn_Create(357, 7, 15, 15, L"X", hWindowRegit);
	OnCreateCode();

	XBtn_SetType(hBtnWinClose, button_type_close);
	int nResult = XModalWnd_DoModal(hWindowRegit);

	*pbHandled = TRUE;
	return 0;
}
int CALLBACK AccMgr_EventBtnClick(BOOL *pbHandled)
{
	hWindowRegit = XModalWnd_Create(383, 230, L"�˻�����", XWnd_GetHWND(hWindow));

	XShapeText_Create(12, 6, 0, 0, L"�˻�����", hWindowRegit);
	XShapeText_Create(88, 54, 60, 20, L"ע���˻�:", hWindowRegit);
	XShapeText_Create(88, 82, 60, 20, L"ԭʼ����:", hWindowRegit);
	XShapeText_Create(88, 110, 60, 20, L"�� ����:", hWindowRegit);
	XShapeText_Create(100, 139, 60, 20, L"��֤��:", hWindowRegit);

	hRichEditRegitAccount = XRichEdit_Create(167, 54, 135, 20, hWindowRegit);
	hRichEditRegitPasswd = XRichEdit_Create(167, 82, 135, 20, hWindowRegit);
	hRichEditRegitPasswd2 = XRichEdit_Create(167, 110, 135, 20, hWindowRegit);
	XRichEdit_EnablePassword(hRichEditRegitPasswd, TRUE);
	XRichEdit_EnablePassword(hRichEditRegitPasswd2, TRUE);

	hRichEditRegitCode = XRichEdit_Create(167, 139, 51, 20, hWindowRegit);
	hPicCode = XShapePic_Create(226, 139, 76, 20, hWindowRegit);

	HELE hBtnDoRegist = XBtn_Create(160, 170, 72, 29, L"ȷ���޸�", hWindowRegit);
	XEle_RegEventC(hBtnDoRegist, XE_BNCLICK, DoAccMgr_EventBtnClick);

	HELE hBtnCodeFlush = XBtn_Create(306, 140, 20, 20, L"", hWindowRegit);
	HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(IDB_PNG1), _T("PNG"));
	DWORD len = SizeofResource(NULL, hRes);
	HGLOBAL hg = LoadResource(NULL, hRes);
	LPVOID lp = (LPSTR)LockResource(hg);
	XBtn_AddBkImage(hBtnCodeFlush, button_state_stay, XImage_LoadMemory(lp, len, TRUE));
	XBtn_AddBkImage(hBtnCodeFlush, button_state_leave, XImage_LoadMemory(lp, len, TRUE));
	XBtn_AddBkImage(hBtnCodeFlush, button_state_down, XImage_LoadMemory(lp, len, TRUE));
	XBtn_AddBkImage(hBtnCodeFlush, button_state_check, XImage_LoadMemory(lp, len, TRUE));
	XBtn_SetStyle(hBtnCodeFlush, button_style_scrollbar_slider);
	XEle_RegEventC(hBtnCodeFlush, XE_BNCLICK, DoCodeFlush_EventBtnClick);

	HELE hBtnWinClose = XBtn_Create(357, 7, 15, 15, L"X", hWindowRegit);
	OnCreateCode();

	XBtn_SetType(hBtnWinClose, button_type_close);
	int nResult = XModalWnd_DoModal(hWindowRegit);

	*pbHandled = TRUE;
	return 0;
}

BOOL CALLBACK MyEnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	if (hwnd != NULL) 
	{
		DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
		if ((dwStyle & WS_OVERLAPPEDWINDOW) && (dwStyle & WS_VISIBLE))
		{
			CString csWinName;
			CWnd* pWnd = CWnd::FromHandle(hwnd);
			// ���ڱ���
			pWnd->GetWindowText(csWinName);
			//if (csWinName.Find("����") >= 0 || csWinName.Find("online") >= 0 || csWinName.Find("loader") >= 0)
			if (csWinName.Find(L"����ȺӢ��OnLine") >= 0)
			{
				DWORD dwProcId;
				//��ȡ���ڽ���ID
				::GetWindowThreadProcessId(hwnd, &dwProcId);

				//���洰����
				CString szTest;
				szTest.Format(_T("����ȺӢ��OnLine - PID:%d"), dwProcId);

				SetWindowText(hwnd, szTest);
			}
		}
	}

	return TRUE;
}

///< ͨ������ID��ȡ���ھ��
HWND GetWindowHwndByPorcessID(DWORD dwProcessID)
{
	DWORD dwPID = 0;
	HWND hwndRet = NULL;
	// ȡ�õ�һ�����ھ��
	HWND hwndWindow = ::GetTopWindow(0);
	while (hwndWindow)
	{
		dwPID = 0;
		// ͨ�����ھ��ȡ�ý���ID
		DWORD dwTheardID = ::GetWindowThreadProcessId(hwndWindow, &dwPID);
		if (dwTheardID != 0)
		{
			// �жϺͲ�������Ľ���ID�Ƿ����
			if (dwPID == dwProcessID)
			{
				// ����ID��ȣ����¼���ھ��
				hwndRet = hwndWindow;
				break;
			}
		}
		// ȡ����һ�����ھ��
		hwndWindow = ::GetNextWindow(hwndWindow, GW_HWNDNEXT);
	}
	// ����ȡ�õĴ��ڣ���һ�������ϲ�Ĵ��ڣ���Ҫͨ��GetParent��ȡ��㴰��
	HWND hwndWindowParent = NULL;
	// ѭ�����Ҹ����ڣ��Ա㱣֤���صľ�������Ĵ��ھ��
	while (hwndRet != NULL)
	{
		hwndWindowParent = ::GetParent(hwndRet);
		if (hwndWindowParent == NULL)
		{
			break;
		}
		hwndRet = hwndWindowParent;
	}
	// ���ش��ھ��
	return hwndRet;
}

int CALLBACK StartGame_EventBtnClick(BOOL *pbHandled)
{
	CString nerVer = "";
	if (DoCheckVersion(GameVersion, nerVer))
	{//���ø���
		if (MessageBox(NULL, L"��⵽���°汾���Ƿ���������?", L"����", MB_YESNO) == IDYES)
		{
			CString StartUpdate = ClientUpdateFile;
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;
			// TODO:  �ڴ���ӿؼ�֪ͨ����������
			if (!CreateProcess(StartUpdate, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			{
				MessageBox(NULL, L"��������ʧ�ܣ�", L"��ʾ", MB_OK);
				return -1;
			}
			else{
				BOOL flag;
				WinClose_EventBtnClick(&flag);
			}
		}
	}

	//���map����Чpid
	for (int i = 0; i < MAX_CLNT_SIZE; i++)
	{
		//find pid
		if (g_ClntPidMap[i] != 0)
		{
			BOOL result = Utils::FindProcessId(g_ClntPidMap[i]);
			if (!result)
			{
				g_ClntPidMap[i] = 0;
				g_CurClntCount--;
			}
		}
	}
	if (g_CurClntCount >= MAX_CLNT_SIZE)
	{
		MessageBox(XWnd_GetHWND(hWindow), L"�������ޣ��޷�������Ϸ��", L"��ʾ", MB_OK);
		return -1;
	}
	CString StartFile = GameStartFile;
	CString g_CmdLine = Exprision;
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (!CreateProcess(StartFile, (LPWSTR)(LPCWSTR)g_CmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		MessageBox(XWnd_GetHWND(hWindow), L"������Ϸʧ�ܣ�", L"��ʾ", MB_OK);
		return -1;
	}

	// remember pid
	// ���ҵ�ǰ�Ƿ��п�λ
	for (int i = 0; i < MAX_CLNT_SIZE; i++)
	{
		if (g_ClntPidMap[i] == 0)
		{
			g_ClntPidMap[i] = pi.dwProcessId;
		}
	}

	*pbHandled = TRUE;
	return 0;
}
int CALLBACK UpLog_EventBtnClick(BOOL *pbHandled)
{
	XRichEdit_SetText(hRichEdit, UpdateLogText);
	*pbHandled = TRUE;
	return 0;
}
int CALLBACK NewLead_EventBtnClick(BOOL *pbHandled)
{
	XRichEdit_SetText(hRichEdit, LeaderText);
	*pbHandled = TRUE;
	return 0;
}
int CALLBACK BtnRecharge_EventBtnClick(BOOL *pbHandled)
{
	XRichEdit_SetText(hRichEdit, RechargeText);
	*pbHandled = TRUE;
	return 0;
}
int CALLBACK PropWd_EventBtnClick(BOOL *pbHandled)
{
	XRichEdit_SetText(hRichEdit, PropagateText);
	*pbHandled = TRUE;
	return 0;
}
int CALLBACK PropPay_EventBtnClick(BOOL *pbHandled)
{
	XRichEdit_SetText(hRichEdit, PropagatePayText);
	*pbHandled = TRUE;
	return 0;
}
int CALLBACK WarHis_EventBtnClick(BOOL *pbHandled)
{
	XRichEdit_SetText(hRichEdit, WarText);
	*pbHandled = TRUE;
	return 0;
}
int CALLBACK PlayHis_EventBtnClick(BOOL *pbHandled)
{
	XRichEdit_SetText(hRichEdit, CarbonText);
	*pbHandled = TRUE;
	return 0;
}
int CALLBACK KeyHelp_EventBtnClick(BOOL *pbHandled)
{
	wchar_t  buf[256] = { 0 };
	hWindowFz = XModalWnd_Create(320, 110, L"�Զ�����", XWnd_GetHWND(hWindow));

	XShapeText_Create(12, 6, 0, 0, L"�Զ�����", hWindowFz);
	XShapeText_Create(22, 36, 0, 0, L"����:", hWindowFz);

	HELE hComboBoxWin = XComboBox_Create(66, 36, 120, 20, hWindowFz);
	HXCGUI hAdapter = XAdapterTable_Create();
	XComboBox_BindApapter(hComboBoxWin, hAdapter);
	XAdapterTable_AddColumn(hAdapter, L"name");
	
	for (int i = 0; i<20; i++)
	{
		wsprintfW(buf, _T("%s���� - %d"), ClientCopyRight, i);
		XAdapterTable_AddItemText(hAdapter, buf);
	}
	XRichEdit_SetText(hComboBoxWin, L"��ѡ�񴰿�");
	XWnd_AdjustLayout(hWindowFz);

	XShapeText_Create(22, 66, 0, 0, L"����:", hWindowFz);

	hComboBoxKey = XComboBox_Create(66, 66, 120, 20, hWindowFz);
	HXCGUI hAdapter2 = XAdapterTable_Create();
	XComboBox_BindApapter(hComboBoxKey, hAdapter2);
	XAdapterTable_AddColumn(hAdapter2, L"name");
	for (int i = 1; i<=8; i++)
	{
		wsprintfW(buf, L" F%d", i);
		XAdapterTable_AddItemText(hAdapter2, buf);
	}
	wsprintfW(buf, L" TAB");
	XAdapterTable_AddItemText(hAdapter2, buf);

	XRichEdit_SetText(hComboBoxKey, L"��ѡ�񰴼�");

	XRichEdit_EnableEvent_XE_RICHEDIT_CHANGE(hComboBoxKey, TRUE);
	XEle_RegEventC(hComboBoxKey, XE_COMBOBOX_EXIT_LIST, ComboBoxKey_EventChange);
	XWnd_AdjustLayout(hWindowFz);

	HELE hBtnStartFz = XBtn_Create(222, 56, 73, 29, L"��ʼ", hWindowFz);
	XEle_RegEventC(hBtnStartFz, XE_BNCLICK, StartFz_EventBtnClick);

	HELE hBtnFzClose = XBtn_Create(290, 7, 15, 15, L"X", hWindowFz);
	XBtn_SetType(hBtnFzClose, button_type_close);

	//int nResult = XModalWnd_DoModal(hWindowRegit);
	XWnd_ShowWindow(hWindowFz, SW_SHOW);

	*pbHandled = TRUE;
	return 0;
}
int CALLBACK KeyMainWeb_EventBtnClick(BOOL *pbHandled)
{
	ShellExecute(0, NULL, MainWebUrl, NULL, NULL, SW_NORMAL);
	//Utils::OpenURL(url);
	*pbHandled = TRUE;
	return 0;
}
int CALLBACK KeyService_EventBtnClick(BOOL *pbHandled)
{
	ShellExecute(0, NULL, ServerUrl, NULL, NULL, SW_NORMAL);
	*pbHandled = TRUE;
	return 0;
}
int CALLBACK KeyNeedRecharge_EventBtnClick(BOOL *pbHandled)
{
	//CString url = RechargeUrl;
	//Utils::OpenURL(url);
	//ShellExecute(0, NULL, RechargeUrl, NULL, NULL, SW_NORMAL);
	MessageBox(NULL, L"�Զ���ֵ���������У������ֵ����ϵ�ͷ���", L"��ʾ", MB_OK);
	*pbHandled = TRUE;
	return 0;
}
#endif

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	//delete file and dir
	DeleteFile(L"SOLUpd3.pak");
	DeleteFile(L"SOLUpd4.pak");
	DeleteFile(L"Update.pak");
	DeleteFile(L"Update2.pak");
	DeleteFile(L"server.ini");
	DeleteFile(L"online.dat");

	TCHAR exeFullPath[256];
	GetCurrentDirectory(MAX_PATH, exeFullPath);
	CString tmp;
	tmp.Format(TEXT("%s\\data\\"), exeFullPath);
	Utils::DeleteDirectory(tmp);
	tmp.Format(TEXT("%s\\Shape\\"), exeFullPath);
	Utils::DeleteDirectory(tmp);
	tmp.Format(TEXT("%s\\SPL\\"), exeFullPath);
	Utils::DeleteDirectory(L"SPL\\");
	tmp.Format(TEXT("%s\\Script\\"), exeFullPath);
	Utils::DeleteDirectory(L"Script\\");

	//start protect
	CString StartFile = ClientProtectFile;
	//��ȡ��ǰ����pid
	int t_pid = _getpid();
	CString g_CmdLine = GameStartFile;
	g_CmdLine.Format(TEXT(" %s %d"), StartFile, t_pid);

	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi_pro;
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (!CreateProcess(StartFile, (LPWSTR)(LPCTSTR)g_CmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi_pro))
	{
		MessageBox(NULL, L"������Ϸʧ�ܣ�", L"��ʾ", MB_OK);
		return -1;
	}
	ClntPid.Format(TEXT("%d"), pi_pro.dwProcessId);

	CString str[] = { L"����κ����", L"��������", L"����С����" };
	INT pid[] = { _ttoi(ClntPid) };
	while (1)
	{
		try
		{
			BOOL result = Utils::GetProcessList(str, 3, pid, 1);
			if (result)
			{
				BOOL flag;
				WinClose_EventBtnClick(&flag);
			}

			/*for (int i = 0; i < MAX_CLNT_SIZE; i++)
			{
				if (g_ClntPidMap[i] != 0)
				{
					HWND hwnd = GetWindowHwndByPorcessID(g_ClntPidMap[i]);
					if (hwnd != NULL)
					{
						CString csTitle;
						csTitle.Format(_T("����ȺӢ��OnLine - %d"), g_ClntPidMap[i]);
						SetWindowText(hwnd, csTitle);
					}
				}
			}*/
			EnumWindows(MyEnumWindowsProc, (LPARAM)hWindow);//�������ڳ���
		}
		catch (CException& w)
		{
		}
		
		Sleep(5000);
	}
	return 0;
}

void LoadXmlInfos()
{
	try{
		tinyxml2::XMLDocument doc;
		XMLError ret = doc.LoadFile("main.xml");

		if (ret != XML_SUCCESS)
		{
			MessageBox(NULL, L"�����ļ��������ʵ", L"��ʾ", NULL);
			exit(1);
		}
		XMLElement* element;
		XMLElement* document = doc.RootElement();
		element = document->FirstChildElement("ServerAddr");
		ServerAddr = W(element->GetText());
		element = document->FirstChildElement("ServerPort");
		ServerPort = W(element->GetText());
		element = document->FirstChildElement("GameVersion");
		GameVersion = W(element->GetText());
		element = document->FirstChildElement("GameStartFile");
		GameStartFile = W(element->GetText());
		element = document->FirstChildElement("ClientUpdateTitile");
		ClientUpdateTitile = W(element->GetText());
		element = document->FirstChildElement("ClientProtectTitile");
		ClientProtectTitile = W(element->GetText());
		element = document->FirstChildElement("ClientUpdateFile");
		ClientUpdateFile = W(element->GetText());
		element = document->FirstChildElement("ClientProtectFile");
		ClientProtectFile = W(element->GetText());
		element = document->FirstChildElement("ClientCopyRight");
		ClientCopyRight = W(element->GetText());
		element = document->FirstChildElement("ClientTitle");
		ClientTitle = W(element->GetText());
		element = document->FirstChildElement("ClientFile");
		ClientFile = W(element->GetText());
		element = document->FirstChildElement("KeyHelpFile");
		KeyHelpFile = W(element->GetText());
		element = document->FirstChildElement("Exprision");
		Exprision = W(element->GetText());
		element = document->FirstChildElement("ServerUrl");
		ServerUrl = W(element->GetText());
		element = document->FirstChildElement("RechargeUrl");
		RechargeUrl = W(element->GetText());
		element = document->FirstChildElement("MainWebUrl");
		MainWebUrl = W(element->GetText());
		element = document->FirstChildElement("DownLoadUrl");
		DownLoadUrl = W(element->GetText());
		element = document->FirstChildElement("RechargeText");
		RechargeText = W(element->GetText());
		element = document->FirstChildElement("CarbonText");
		CarbonText = W(element->GetText());
		element = document->FirstChildElement("LeaderText");
		LeaderText = W(element->GetText());
		element = document->FirstChildElement("PropagateText");
		PropagateText = W(element->GetText());
		element = document->FirstChildElement("PropagatePayText");
		PropagatePayText = W(element->GetText());
		element = document->FirstChildElement("WarText");
		WarText = W(element->GetText());
		element = document->FirstChildElement("UpdateLogText");
		UpdateLogText = W(element->GetText());

		wchar_t *pWChar = ServerAddr.GetBuffer(); //��ȡstr�Ŀ��ַ������鱣��  
		ServerAddr.ReleaseBuffer();
		int nLen = ServerAddr.GetLength(); //��ȡstr���ַ���  
		char *addr = new char[nLen * 2 + 1];
		memset(addr, 0, nLen * 2 + 1);
		wcstombs(addr, pWChar, nLen * 2 + 1); //���ַ�ת��Ϊ���ֽ��ַ� 
		strcpy(pServerAddr, addr);
		delete[] addr;

		pWChar = ServerPort.GetBuffer(); //��ȡstr�Ŀ��ַ������鱣��  
		ServerPort.ReleaseBuffer();
		nLen = ServerPort.GetLength(); //��ȡstr���ַ���  
		char *port = new char[nLen * 2 + 1];
		memset(port, 0, nLen * 2 + 1);
		wcstombs(port, pWChar, nLen * 2 + 1); //���ַ�ת��Ϊ���ֽ��ַ� 
		iServerPort = atoi(port);
		delete[] port;
	}
	catch (exception ex){
		MessageBox(NULL, L"�����ļ��������ʵ", L"��ʾ", NULL);
		exit(1);
	}
}

int InitializeComponent()
{
	LoadXmlInfos();

	CString str = ClientTitle;
	LPCWSTR title = (LPCWSTR)str;

	hWindow = XWnd_Create(20, 20, 640, 333, title, NULL, xc_window_style_modal);//��������
	if (hWindow)
	{
		CString curVer = ClientCopyRight + "���� V" + GameVersion;
		XShapeText_Create(12, 6, 0, 0, curVer, hWindow);

		HELE hBtnWinClose = XBtn_Create(610, 7, 15, 15, L"X", hWindow);
		XEle_RegEventC(hBtnWinClose, XE_BNCLICK, WinClose_EventBtnClick);

		HELE hBtnWinMin = XBtn_Create(590, 7, 15, 15, L"-", hWindow);
		XEle_RegEventC(hBtnWinMin, XE_BNCLICK, WinMin_EventBtnClick);

		HELE hBtnStartGame = XBtn_Create(535, 281, 73, 29, L"��ʼ��Ϸ", hWindow);
		XEle_RegEventC(hBtnStartGame, XE_BNCLICK, StartGame_EventBtnClick);

		HELE hBtnRegist = XBtn_Create(34, 285, 60, 25, L"�û�ע��", hWindow);
		XEle_RegEventC(hBtnRegist, XE_BNCLICK, Regist_EventBtnClick);

		HELE hBtnAccMgr = XBtn_Create(110, 285, 60, 25, L"�˻�����", hWindow);
		XEle_RegEventC(hBtnAccMgr, XE_BNCLICK, AccMgr_EventBtnClick);

		HELE hBtnUpLog = XBtn_Create(187, 285, 60, 25, L"���¼�¼", hWindow);
		XEle_RegEventC(hBtnUpLog, XE_BNCLICK, UpLog_EventBtnClick);

		HELE hBtnNewLead = XBtn_Create(266, 285, 60, 25, L"����ָ��", hWindow);
		XEle_RegEventC(hBtnNewLead, XE_BNCLICK, NewLead_EventBtnClick);

		//HELE hBtnKeyHelp = XBtn_Create(345, 285, 60, 25, L"����", hWindow);
		//XEle_RegEventC(hBtnKeyHelp, XE_BNCLICK, KeyHelp_EventBtnClick);

		//==
		HELE hBtnRecharge = XBtn_Create(34, 41, 60, 25, L"��ֵ�趨", hWindow);
		XEle_RegEventC(hBtnRecharge, XE_BNCLICK, BtnRecharge_EventBtnClick);

		HELE hBtnPropWd = XBtn_Create(100, 41, 60, 25, L"������", hWindow);
		XEle_RegEventC(hBtnPropWd, XE_BNCLICK, PropWd_EventBtnClick);

		HELE hBtnPropPay = XBtn_Create(167, 41, 60, 25, L"������̯", hWindow);
		XEle_RegEventC(hBtnPropPay, XE_BNCLICK, PropPay_EventBtnClick);

		HELE hBtnWarHis = XBtn_Create(236, 41, 60, 25, L"��սս��", hWindow);
		XEle_RegEventC(hBtnWarHis, XE_BNCLICK, WarHis_EventBtnClick);

		HELE hBtnPlayHis = XBtn_Create(305, 41, 60, 25, L"����", hWindow);
		XEle_RegEventC(hBtnPlayHis, XE_BNCLICK, PlayHis_EventBtnClick);

		HELE hBtnKeyMainWeb = XBtn_Create(409, 41, 60, 25, L"�ٷ���ҳ", hWindow);
		XEle_RegEventC(hBtnKeyMainWeb, XE_BNCLICK, KeyMainWeb_EventBtnClick);

		HELE hBtnKeyService = XBtn_Create(479, 41, 60, 25, L"��ϵ�ͷ�", hWindow);
		XEle_RegEventC(hBtnKeyService, XE_BNCLICK, KeyService_EventBtnClick);

		HELE hBtnKeyNeedRecharge = XBtn_Create(549, 41, 60, 25, L"��Ҫ��ֵ", hWindow);
		XEle_RegEventC(hBtnKeyNeedRecharge, XE_BNCLICK, KeyNeedRecharge_EventBtnClick);

		hRichEdit = XRichEdit_Create(31, 75, 580, 193, hWindow);
		XRichEdit_EnableAutoWrap(hRichEdit,TRUE);
		XRichEdit_EnableReadOnly(hRichEdit, TRUE);

		XRichEdit_SetText(hRichEdit, UpdateLogText);

		InitTray(NULL, XWnd_GetHWND(hWindow));

		XWnd_RegEventC(hWindow, WM_SHOWTASK, MainWinShow_EventBtnClick);

		XWnd_ShowWindow(hWindow, SW_SHOW);
	}

	//start listen
	DWORD threadID;
	HANDLE hThread;
	hThread = CreateThread(NULL, 0, ThreadProc, NULL, 0, &threadID); // �����߳�
	if (hThread == NULL)
	{
		MessageBox(NULL, L"����ʧ�ܣ�", L"��ʾ", NULL);
		BOOL flag;
		WinClose_EventBtnClick(&flag);
		return -1;
	}

	HXCGUI UInerVer = NULL;
	CString newVer = "";
	BOOL updateVer = DoCheckVersion(GameVersion, newVer);
	if (updateVer)
	{
#if 0
		//���ø���
		CString StartUpdate = ClientUpdateFile;
		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi;
		// TODO:  �ڴ���ӿؼ�֪ͨ����������
		if (!CreateProcess(StartUpdate, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		{
			MessageBox(NULL, L"��������ʧ�ܣ�", L"��ʾ", MB_OK);
			return -1;
		}

		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pi_pro.dwProcessId);
		if (hProcess != NULL)
		{
			TerminateProcess(hProcess, 0);
		}
		BOOL flag;
		WinClose_EventBtnClick(&flag);
		return -1;
#else
		newVer = "->  V" + newVer;
		UInerVer = XShapeText_Create(132, 7, 0, 0, newVer, hWindow);
		XShapeText_SetTextColor(UInerVer, RGB(255, 0, 0), 128);
		if (newVer != GameVersion)
		{
			if (MessageBox(NULL, L"��⵽���°汾���Ƿ���������?", L"����", MB_YESNO) == IDYES)
			{
				CString StartUpdate = ClientUpdateFile;
				STARTUPINFO si = { sizeof(si) };
				PROCESS_INFORMATION pi;
				// TODO:  �ڴ���ӿؼ�֪ͨ����������
				if (!CreateProcess(StartUpdate, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
				{
					MessageBox(NULL, L"��������ʧ�ܣ�", L"��ʾ", MB_OK);
					return -1;
				}else{
					BOOL flag;
					WinClose_EventBtnClick(&flag);
				}
			}
		}
#endif
	}
	else
	{
		newVer = "���°汾";
		UInerVer = XShapeText_Create(132, 6, 0, 0, newVer, hWindow);
		XShapeText_SetTextColor(UInerVer, RGB(0, 0, 255), 128);
	}

	return 0;
}

//���庯����  
inline void EnableMemLeakCheck()
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
}

#define SCANSETTINGS_CLASSNAME      _T("ScanSettingsWindowClass")  
#define APPMUTEX                    _T("Global\\ScanSettings")  
HANDLE m_hMutex;

BOOL RestrictOneInstance(LPCTSTR className, LPCTSTR winName)
{
	SECURITY_DESCRIPTOR secutityDese;
	::InitializeSecurityDescriptor(&secutityDese, SECURITY_DESCRIPTOR_REVISION);
	::SetSecurityDescriptorDacl(&secutityDese, TRUE, NULL, FALSE);

	SECURITY_ATTRIBUTES securityAttr;
	securityAttr.nLength = sizeof SECURITY_ATTRIBUTES;
	securityAttr.bInheritHandle = FALSE;
	securityAttr.lpSecurityDescriptor = &secutityDese;

	m_hMutex = ::CreateMutex(&securityAttr, FALSE, APPMUTEX);
	BOOL bLaunched = (m_hMutex != NULL && ERROR_ALREADY_EXISTS == GetLastError());

	CWnd *pWndPrev = NULL;
	CWnd *pWndChild = NULL;

	if (pWndPrev == NULL)
	{
		if (className == NULL && winName == NULL)
		{
			return TRUE;
		}
		pWndPrev = CWnd::FindWindow(className, winName);
	}

	if (pWndPrev != NULL)
	{
		pWndPrev->ShowWindow(SW_SHOW);
		// If so, does it have any popups?  
		pWndChild = pWndPrev->GetLastActivePopup();

		// If iconic, restore the main window  
		if (pWndPrev->IsIconic())
		{
			pWndPrev->ShowWindow(SW_RESTORE);
		}

		// Bring the main window or its popup to  
		// the foreground  
		pWndChild->SetForegroundWindow();

		return FALSE;
	}

	return TRUE;
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	//if (!RestrictOneInstance(NULL, L"�����¼��"))
	//{
	//	return FALSE;
	//}

	// ����������
	CString str = ClientCopyRight + "��¼��";
	LPCWSTR title = (LPCWSTR)str;
	HANDLE hMutex = CreateMutex(NULL, FALSE, title);
	// ���������
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// ������л������������ͷž������λ������
		MessageBox(NULL, L"��¼���Ѿ�����,��ע���Ƿ����̣�", L"��ʾ", NULL);
		return FALSE;
	}

	XInitXCGUI();//��ʼ��
	XC_EnableDebugFile(FALSE);

	InitializeComponent();

	XRunXCGUI();
	XExitXCGUI();

	return 0;
}
