#ifndef _INIT_H_
#define _INIT_H_

#include <sys/socket.h>
#include <errno.h>
#include <map>
#include <arpa/inet.h>
#include <string>
using namespace std;

typedef struct PACKAGE{
	int length;
	char body[1024];
}package;

map<string,int> online_user_table;


/*不再封装sendn函数
ssize_t sendn(int sockfd,const void *buf,size_t len,int flags)
{
	size_t nleft=len;
	ssize_t nsend;

	char * bufp=(char*)buf;

	while(nleft>0)
	{
		if((nsend=send(sockfd,bufp,nleft,flags))<0)
		{
			if(errno==EINTR)
				continue;
			return -1;
		}
		else if (nsend==0)
			continue;
		bufp+=nsend;
		nleft-=nsend;
	}
	return len;
}
*/
//接收函数
//返回接收到的body长度
ssize_t recvn(int sockfd,void *sbuf,size_t *len,int flags)
{
	package *buf=(package *)sbuf;
	*len=recv(sockfd,&(buf->length),4,flags);
	if(*len>0)
	{
		*len=ntohl(buf->length);
		*len=recv(sockfd,buf->body,*len,flags);
	}
	return *len;
}
/*
ssize_t recvn(int sockfd,void*buf,size_t len,int flags)
{
	size_t nleft=len;
	ssize_t nrecv;

	char *bufp=(char *)buf;

	while(nleft>0)
	{
		if((nrecv=recv(sockfd,bufp,nleft,flags))<0)
			{
				if(errno==EINTR)
					continue;
				return -1;
			}
		else if(nrecv==0)
			return len-nleft;
		bufp+=nrecv;
		nleft-=nrecv;
	}
	return len;
}
*/
#endif
