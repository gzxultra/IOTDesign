
#include "StdAfx.h"  
#include "Http.h"  
  
  
CHttp::CHttp(void)  
    : m_bConnect(false),dwTimeOut(15000),chunked(FALSE)  
{  
  
      
    referer="";  
    cookie="";  
  
}  
  
//http底层提交  
bool CHttp::put(CString inbuff,CString *outbuff,CString type)  
{  
    chunked=FALSE;  
    char *len=new char[15];  
    char *content;  
	int nrecv=0; 
	
    if (MySocket==INVALID_SOCKET) return FALSE;  
    if (m_bConnect==FALSE) return FALSE;  
    if (type.Compare("img")!=0)  
    *outbuff="";  
    int retval=::send(MySocket,(LPCTSTR)inbuff,inbuff.GetLength(),0);  
  
    if( SOCKET_ERROR== retval )  return FALSE;  
       
    char *urls=new char[1024*1024*2];
	ZeroMemory(urls,1024*1024*2); //urls存储总下载源码

	char *buffer=new char[1024];  
      

    WSAEVENT rwE = ::WSACreateEvent();  
    WSAEventSelect(MySocket, rwE, FD_READ|FD_CLOSE|FD_WRITE);  
    while(1)  
    {  
        int ret=::WSAWaitForMultipleEvents(1, &rwE, FALSE,dwTimeOut, FALSE);  
       
        WSANETWORKEVENTS event1;   
        ::WSAEnumNetworkEvents(MySocket, rwE, &event1);  
        if(event1.lNetworkEvents & FD_READ)  
        {  
				ZeroMemory(buffer,1024);  
				retval= recv(MySocket ,buffer,1023,0);  				
				if (retval>0)
				{
					memcpy(urls+nrecv,buffer,retval);
					nrecv+=retval;
				}
                      
         }  
         else if(event1.lNetworkEvents & FD_OOB_BIT)  
         {  
				ZeroMemory(buffer,1024);  
				retval= recv(MySocket ,buffer,1023,0);
				if (retval>0)
				{
					memcpy(urls+nrecv,buffer,retval);
					nrecv+=retval;
				}
         }  
         else if(event1.lNetworkEvents & FD_CLOSE)  
         {  
				break;  
         }  
         else  
         {  
                break;  
         }  
         if (strstr(urls,"Transfer-Encoding: chunked")!=NULL)  
         {  
				chunked=TRUE;  
         }  
         else if(strstr(urls,"Content-Length")!=NULL)  
         {  
              
				strncpy(len,strstr(urls,"Content-Length")+16,10); 
				char *end;
                end=strstr(len,"\r\n");
				int nlen=end-len;
				len[nlen]='\0';

                m_nContent=atoi(len);  
				if (m_nContent>1024*1024*2)
				{
					*outbuff="";
					goto label2;
				}
                content=strstr(urls,"\r\n\r\n")+4;
				int nheader=content-urls;
                if (nrecv-nheader==m_nContent)   
                break;  
          }  
  
          if(chunked==TRUE )  
          {  
                if(strstr(urls,"0\r\n\r\n")>0)  
                break;  
          }  
          
       }  //while end
   

	if (type.Compare("img")==0)  
    {  
        CFile file;  
        file.Open(outbuff->GetString(),CFile::modeCreate|CFile::modeReadWrite| CFile::typeBinary);
		file.SeekToEnd();  
        file.Write(content,m_nContent); 
		file.Close(); 
	
	}

	*outbuff+=urls;
    BulidCookie(*outbuff);

label2:
	WSACloseEvent(rwE);
	delete [] urls;
	delete [] buffer;
	delete [] len; 

    return TRUE;  
}  
  
CHttp::~CHttp(void)  
{  
      
}  
  
//直接get方式访问  
bool CHttp::get(CString url,CString *result)  
{     
    CString refer; 
	CString y;
    refer=url;  
    url.Replace("http://","");  
    host=url.Left(url.Find("/"));  

    LPHOSTENT ip= gethostbyname(host);  
	if (ip==NULL )return false;
    if (!m_connect(((LPIN_ADDR)ip->h_addr)->s_addr,"80"))   
    {     
        *result="连接错误";  
        return FALSE;  
    }  
    //构造http头  
    url.Replace(host,""); 
    CString head=   CString("GET ")+url+  
    CString(" HTTP/1.1\r\nAccept: text/html, application/xml;q=0.9, application/xhtml+xml, */*\r\n")+  
    CString("Accept-Language: zh-cn\r\n")+  
    CString("User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; );\r\n")+  
    CString("Host: ")+host+CString("\r\n")+  
    CString("Connection: Keep-Alive\r\n");  
  
    if (nfakeip.Compare("")!=0)  
    {  
        head+=(CString("X-Forwarded-For: ")+nfakeip+CString("\r\n"));  
    }  
  
    if (referer.Compare("")!=0)  
    {  
        head+=(CString("Referer: ")+referer+CString("\r\n"));  
    }  
    if (cookie.Compare("")!=0)  
    {  
        head+=(CString("Cookie: ")+cookie+CString("\r\n"));  
    }  
  
    head+=(CString("\r\n"));  
    //提交http包，结果返回到result指针。  
    if (!put(head,result,""))  
    {  
        *result="提交数据错误";  
        return FALSE;  
    }  
    referer=refer;  
  
      
    closesocket(MySocket);  
    m_bConnect=FALSE;  
    return TRUE;  
}  
  
bool CHttp::get(CString ip,CString port,CString url,CString *result)  
{     
    CString refer;  
    refer=url;  
    url.Replace("http://","");  
    host=url.Left(url.Find("/"));  
  
    if (!m_connect(inet_addr(ip),port))   
    {     
        *result="连接错误";  
        return FALSE;  
    }  
    //构造http头  
    url.Replace(host,"");  
    CString head=   CString("GET ")+refer+  
    CString(" HTTP/1.1\r\nAccept: text/html, application/xml;q=0.9, application/xhtml+xml, */*\r\n")+  
    CString("Accept-Language: zh-cn\r\n")+  
    CString("User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; );\r\n")+  
    CString("Host: ")+host+CString("\r\n")+  
    CString("Connection: Keep-Alive\r\n");  
  
    if (nfakeip.Compare("")!=0)  
    {  
        head+=(CString("X-Forwarded-For: ")+nfakeip+CString("\r\n"));  
    }  
  
    if (referer.Compare("")!=0)  
    {  
        head+=(CString("Referer: ")+referer+CString("\r\n"));  
    }  
    if (cookie.Compare("")!=0)  
    {  
        head+=(CString("Cookie: ")+cookie+CString("\r\n"));  
    }  
  
    head+=(CString("\r\n"));  
    //提交http包，结果返回到result指针。  
    if (!put(head,result,""))  
    {  
        *result="提交数据错误";  
        return FALSE;  
    }  
    referer=refer;  
  
      
    closesocket(MySocket);  
    m_bConnect=FALSE;  
    return TRUE;  
}  
  
  
bool CHttp::m_connect(unsigned long ip,CString port)  
{  
  
    MySocket =::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);  
    if (MySocket==INVALID_SOCKET) return false;  
  
    //设置超时  
	int nTimeOut= dwTimeOut;  
    ::setsockopt(MySocket,SOL_SOCKET,SO_RCVTIMEO,(char *)&nTimeOut,sizeof(int));  
  
    sockaddr_in RemoteAddr;  
    RemoteAddr.sin_family =AF_INET;  
    RemoteAddr.sin_port = htons(atoi(port));  
    RemoteAddr.sin_addr.S_un.S_addr = ip;  
  
    if (connect(MySocket,(SOCKADDR*)&RemoteAddr,sizeof(SOCKADDR))==SOCKET_ERROR)  
    {     
        m_bConnect=FALSE; 
		closesocket(MySocket);  
        return false;  
    }  
    else  
    {  
        m_bConnect=TRUE;  
    }  
    return true;  
}  
  
//处理cookie的函数  
void CHttp::BulidCookie(CString urlsc)  
{  
  
    while (urlsc.Find("Set-Cookie: ")!=-1)  
    {  
        urlsc=urlsc.Mid(urlsc.Find("Set-Cookie: ")+12);  
        CString ts,cvalue;  
        ts=urlsc.Left(urlsc.Find(";")+1);  
        if (cookie.Find(ts.Left(ts.Find("=")))!=-1)  
        {  
            cvalue=cookie.Mid(cookie.Find(ts.Left(ts.Find("="))));  
            cvalue=cookie.Mid(cookie.Find("=")+1);  
            cvalue=cvalue.Left(cvalue.Find(";")+1);  
            cookie.Replace(cvalue,ts.Mid(ts.Find("=")+1));  
        }  
        else  
        {  
            cookie+=(ts+"");      
        }  
        cvalue.Empty();  
        cvalue.FreeExtra();  
        ts.Empty();  
        ts.FreeExtra();  
    }  
  
}  
  
  
bool CHttp::post(CString url, CString pstr, CString* result)  
{  
    CString refer=url;  
    url.Replace("http://","");  
    host=url.Left(url.Find("/"));  
     
  
    LPHOSTENT ip= gethostbyname(host);  
	if (ip==NULL )return false;
    if (!m_connect(((LPIN_ADDR)ip->h_addr)->s_addr,"80"))   
    {     
        *result="连接错误";  
        return FALSE;  
    }  
    //构造http头  
    int len;  
    len=pstr.GetLength();  
    CString slen;  
    slen.Format("%d\r\n",len);  
   
    url.Replace(host,"");  
    CString head=   CString("POST ")+url+  
    CString(" HTTP/1.1\r\nAccept: text/html, application/xml;q=0.9, application/xhtml+xml, */*\r\n")+  
    CString("Content-Type: application/x-www-form-urlencoded\r\n")+  
    CString("Accept-Language: zh-cn\r\n")+  
    CString("User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; );\r\n")+  
    CString("Host: ")+host+CString("\r\n")+  
    CString("Content-Length: ")+slen+  
    CString("Connection: Keep-Alive\r\n");  
  
    if (nfakeip.Compare("")!=0)  
    {  
        head+=(CString("X-Forwarded-For: ")+nfakeip+CString("\r\n"));  
    }  
  
    if (referer.Compare("")!=0)  
    {  
        head+=(CString("Referer: ")+referer+CString("\r\n"));  
    }  
    if (cookie.Compare("")!=0)  
    {  
        head+=(CString("Cookie: ")+cookie+CString("\r\n"));  
    }  
  
    head+=(CString("\r\n"));  
  
    head+=pstr;  
  
    //提交http包，结果返回到result指针。  
    if (!put(head,result,""))  
    {  
        *result="提交数据错误";  
        return FALSE;  
    }  
      
    referer=refer;  
    closesocket(MySocket);  
    m_bConnect=FALSE;  
    return TRUE;  
}  
  
bool CHttp::post(CString ip,CString port,CString url, CString pstr, CString* result)  
{  
      
    CString refer;  
    if (url.Find("ttp://")>0)  
    {  
        refer=url;  
    }  
    else  
    {  
        refer=url.Mid(url.Find("/"));  
    }  
    url.Replace("http://","");  
    host=url.Left(url.Find("/"));  
  
    if (!m_connect(inet_addr(ip),port))   
    {     
        *result="连接错误";  
        return FALSE;  
    }  
    //构造http头  
    int len;  
    len=pstr.GetLength();  
    CString slen;  
    slen.Format("%d\r\n",len);  
   
    url.Replace(host,"");  
    CString head=   CString("POST ")+refer+  
    CString(" HTTP/1.1\r\nAccept: text/html, application/xml;q=0.9, application/xhtml+xml, */*\r\n")+  
    CString("Content-Type: application/x-www-form-rulencoded\r\n")+  
    CString("Accept-Language: zh-cn\r\n")+  
    CString("User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; );\r\n")+  
    CString("Host: ")+host+CString("\r\n")+  
    CString("Content-Length: ")+slen+  
    CString("Connection: Keep-Alive\r\n");  
  
    if (nfakeip.Compare("")!=0)  
    {  
        head+=(CString("X-Forwarded-For: ")+nfakeip+CString("\r\n"));  
    }  
  
    if (referer.Compare("")!=0)  
    {  
        head+=(CString("Referer: ")+referer+CString("\r\n"));  
    }  
    if (cookie.Compare("")!=0)  
    {  
        head+=(CString("Cookie: ")+cookie+CString("\r\n"));  
    }  
  
    head+=(CString("\r\n"));  
  
    head+=pstr;  
  
    //提交http包，结果返回到result指针。  
    if (!put(head,result,""))  
    {  
        *result="提交数据错误";  
        return FALSE;  
    }  
    if (refer.Find("ttp://")>0)  
    referer=refer;  
    closesocket(MySocket);  
    m_bConnect=FALSE;  
    return TRUE;  
}  
  
  
CString CHttp::fakeip(void)  
{  
    srand( (unsigned)time( NULL ) );  
    CString ip1,ip2,ip3,ip4;  
      
    ip1.Format("%d.",rand()%253+1);  
    ip2.Format("%d.",rand()%253+1);  
    ip3.Format("%d.",rand()%253+1);  
    ip4.Format("%d",rand()%253+1);  
    ip1=ip1+ip2+ip3+ip4;  
    nfakeip=ip1;  
    return ip1;  
}  
  
  
bool CHttp::getimage(CString url,CString path)  
{     
    CString refer;  
    refer=url;  
    url.Replace("http://","");  
    host=url.Left(url.Find("/"));  
  
    
  
  
    LPHOSTENT ip= ::gethostbyname(host);  
    if (!m_connect(((LPIN_ADDR)ip->h_addr)->s_addr,"80"))   
    {         
        return FALSE;  
    }  
      
    //构造http头  
    url.Replace(host,"");  
    CString head=   CString("GET ")+url+  
    CString(" HTTP/1.1\r\nAccept: text/html, application/xml;q=0.9, application/xhtml+xml, */*\r\n")+  
    CString("Accept-Language: zh-cn\r\n")+  
    CString("User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; );\r\n")+  
    CString("Host: ")+host+CString("\r\n")+  
    CString("Connection: Keep-Alive\r\n");  
  
    if (nfakeip.Compare("")!=0)  
    {  
        head+=(CString("X-Forwarded-For: ")+nfakeip+CString("\r\n"));  
    }  
  
    if (referer.Compare("")!=0)  
    {  
        head+=(CString("Referer: ")+referer+CString("\r\n"));  
    }  
    if (cookie.Compare("")!=0)  
    {  
        head+=(CString("Cookie: ")+cookie+CString("\r\n"));  
    }  
  
    head+=(CString("\r\n"));  
    //提交http包，结果返回到result指针。  
    if (!put(head,&path,"img"))  
    {  
          
        return FALSE;  
    }  
  
      
    closesocket(MySocket);  
    
    m_bConnect=FALSE;  
    return TRUE;  
}  

int CHttp::Init(void)
{
	WSADATA wsaDATA;  
    
	if (WSAStartup(MAKEWORD(2,2),&wsaDATA) != NO_ERROR)  
    {   
        ::WSACleanup();  
        return FALSE;         
    }  

	return 0;
}

int CHttp::UInit(void)
{
	
	WSACleanup(); 
	return 0;
}
