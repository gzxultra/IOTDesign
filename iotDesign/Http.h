#pragma once  
#include <windows.h>
#include <winsock2.h>
class CHttp  
{  
public:  
     CHttp(void);  
    ~CHttp(void);  
    virtual bool put(CString inbuff,CString *outbuff,CString type);  
    virtual bool get(CString url,CString *result);  
    virtual bool get(CString ip,CString port,CString url,CString *result);  
    virtual bool post(CString url, CString pstr, CString* result);  
    virtual bool post(CString ip,CString port,CString url,CString pstr, CString *result);  
  
    virtual bool getimage(CString url,CString path);  
  
    virtual bool m_connect(unsigned long ip,CString port);  
    virtual void BulidCookie(CString urlsc);  
    virtual CString fakeip(void);  
  
    CString cookie;  
    CString pryIp;  
    CString pryPort;  
	CString referer;  
  
  
protected:  
    DWORD dwTimeOut;  
    SOCKET MySocket;  
    bool m_bConnect;  
   
    CString host;  
    CString nfakeip;  
  
    int m_nContent;  
    bool chunked;  
public:
	virtual int Init(void);
	virtual int UInit(void);
};  