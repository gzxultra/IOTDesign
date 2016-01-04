class CSerial  
{
public:
	CSerial();
	virtual ~CSerial();
public:
	HANDLE m_hDev;
	
public:
	BOOL Open(UINT nPort, DWORD dwBaudRate=115200);
	BOOL Close(void);

	BOOL IsOpen(void) {return (m_hDev != INVALID_HANDLE_VALUE);}
	BOOL SetBaudRate(DWORD dwBaudRate);
protected:
	virtual BOOL Init(DWORD dwBaudRate);
public:
	BOOL SetReadTimeout(DWORD dwIntervalTimeout, DWORD dwTimeout);
	DWORD Read(LPVOID lpBuffer, DWORD dwBufLen);
	DWORD Write(LPVOID lpBuffer, DWORD dwBufLen);
};