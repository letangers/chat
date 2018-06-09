#ifndef _INIT_H_
#define _INIT_H_

#include <csignal>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

//收发包的结构体
typedef struct PACKAGE{
	int length;
	char body[1024];
}package;

//套接字
int sock;

//判断当前输入是否为用户名
bool is_username=true;
//保存本地输入的用户名
string username="";

//发送缓存
package sendbuf;
//接受缓存
package recvbuf;

//保存用户输入内容
string cmdline="";
//保存解析出来的命令
string cmd="";
//保存解析出来的参数
string arg="";


//处理Ctrl+C
void handle(int sig){
	close(sock);
	cout<<"recv signal SIGINT"<<endl;
	exit(EXIT_SUCCESS);
}

//发送函数
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

//接收函数
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
#endif
