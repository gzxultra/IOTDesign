#include "stdafx.h"
#include "Serial.h"

CSerial::CSerial()
{
	m_hDev = INVALID_HANDLE_VALUE;
}

CSerial::~CSerial()
{
	if(IsOpen())
	{
		Close();
	}
}

BOOL CSerial::Open(UINT nPort, DWORD dwBaudRate)
{
	TCHAR szName[MAX_PATH];

	if(IsOpen())
	{
		TRACE(TEXT("Serial::Open() error not open\r\n") );
		return FALSE;
	}

	wsprintf(szName, _T("COM%d:"), nPort);
	m_hDev = CreateFile(szName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0);
	if(!IsOpen())
	{
		TRACE(TEXT("Serial::Open() error CreateFile()\r\n") );
		return FALSE;
	}

	Init(dwBaudRate);
	return TRUE;
}

BOOL CSerial::Close(void)
{
	BOOL ret;
	if(!IsOpen())
	{
		return FALSE;
	}

	PurgeComm(m_hDev, PURGE_TXABORT
		|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	if(ret = CloseHandle(m_hDev))
		m_hDev = INVALID_HANDLE_VALUE;
	if(!ret)
		TRACE(TEXT("Serial::Close() ret = FALSE\r\n") );
	return ret;
}

BOOL CSerial::SetBaudRate(DWORD dwBaudRate)
{
	DCB dcb;

	if(!IsOpen())
	{
		//RETAILMSG(DBGMSG, (L"Serial::SetBaudRate() error not open\r\n") );
		return FALSE;
	}

	dcb.DCBlength = sizeof(dcb);
	if(!GetCommState(m_hDev, &dcb))
	{
		//RETAILMSG(DBGMSG, (L"Serial::SetBaudRate() error GetCommState()\r\n") );
		return FALSE;
	}

	dcb.BaudRate = dwBaudRate;
	if(!SetCommState(m_hDev, &dcb))
	{
		//RETAILMSG(DBGMSG, (L"Serial::SetBaudRate() error SetCommState()\r\n") );
		return FALSE;
	}

	return TRUE;
}

BOOL CSerial::Init(DWORD dwBaudRate)
{
	DCB dcb;

	if(!GetCommState(m_hDev, &dcb))
	{
		//RETAILMSG(DBGMSG, (L"Serial::Init() error GetCommState()\r\n") );
		return FALSE;
	}

	dcb.DCBlength = sizeof(dcb);
    dcb.BaudRate = dwBaudRate;
    dcb.fParity = FALSE;
    dcb.fNull = FALSE;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;
    dcb.ByteSize = 8;
//	dcb.EvtChar = '@';		//注册一个事件字符


	if(!SetCommState(m_hDev, &dcb))
	{
		//RETAILMSG(DBGMSG, (L"Serial::Init() error SetCommState()\r\n") );
		return FALSE;
	}

	if(!SetupComm(m_hDev, 4096, 4096))
	{
		//RETAILMSG(DBGMSG, (L"Serial::Init() error SetupComm()\r\n") );
		return FALSE;
	}

	SetReadTimeout(100,100);

	return TRUE;
}

BOOL CSerial::SetReadTimeout(DWORD dwIntervalTimeout, DWORD dwTimeout)
{
	COMMTIMEOUTS time;

	if(!IsOpen())
	{
		//RETAILMSG(DBGMSG, (L"Serial::SetReadTimeout() error not open\r\n") );
		return FALSE;
	}

	if(GetCommTimeouts(m_hDev, &time))
	{
		time.ReadIntervalTimeout = dwIntervalTimeout;
		time.ReadTotalTimeoutConstant = dwTimeout;
		time.ReadTotalTimeoutMultiplier = dwIntervalTimeout;
		time.WriteTotalTimeoutConstant = 0;
		time.WriteTotalTimeoutMultiplier = 0;
		return SetCommTimeouts(m_hDev, &time);
	}
	return FALSE;
}


DWORD CSerial::Read(LPVOID lpBuffer, DWORD dwBufLen)
{
	DWORD dwBytes;

	if(!IsOpen())
	{
		//RETAILMSG(DBGMSG, (L"Serial::Read() error not open\r\n") );
		return 0;
	}
	
	if(!ReadFile(m_hDev, lpBuffer, dwBufLen, &dwBytes, NULL))
	{
		int i=GetLastError();
		TRACE("\nReadFile Err %d \n",i);
		//RETAILMSG(DBGMSG, (L"Serial::Read() error ReadFile()\r\n") );
		return 0;
	}

	return dwBytes;
}

DWORD CSerial::Write(LPVOID lpBuffer, DWORD dwBufLen)
{
	DWORD dwBytes;

	if(!IsOpen())
	{
		//RETAILMSG(DBGMSG, (L"Serial::Write() error not open\r\n") );
		return 0;
	}

	if(!WriteFile(m_hDev, lpBuffer, dwBufLen, &dwBytes, NULL))
	{
		//RETAILMSG(DBGMSG, (L"Serial::Write() error WriteFile()\r\n") );
		return 0;
	}

	return dwBytes;
}


