#ifndef _INIT_H_
#define _INIT_H_

#include <csignal>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

// 收发包的结构体
typedef struct PACKAGE{
	int length;
	char body[1024];
}package;
/* 规则
 * length表示body的长度
 * body的第一位为标志位
 * 若为‘0’，表示数据只需要发送一次
 * 若为‘1’，表示数据需要多次发送，且当前为多次发送的第一个
 * 若为‘2’，表示当前为多次发送的中间的包
 * 若为‘3’，表示当前为多次发送的最后一个包
 */

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
//保存发送的本地字节序的头部长度
int send_num=0;

//保存用户输入内容
string cmdline="";
//保存解析出来的命令
string cmd="";
//保存解析出来的参数
string arg="";


//一次性初始化变量
pthread_once_t once_control=PTHREAD_ONCE_INIT;

//read线程和send线程对sendbuf访问要进行同步
pthread_mutex_t sendbuf_mutex;
//send线程要在sendbuf非空时才执行
pthread_cond_t not_empty;
//send_num的读写锁
pthread_rwlock_t send_num_rwlock;

//处理Ctrl+C
void handle(int sig){
	close(sock);
	cout<<"recv signal SIGINT"<<endl;
	exit(EXIT_SUCCESS);
}

//一次性初始化函数
void init_routine(){
	if(pthread_mutex_init(&sendbuf_mutex,NULL)<0)
		cerr<<"init mutex failed"<<endl;
	
	if(pthread_cond_init(&not_empty,NULL)<0)
		cerr<<"init not empty cond failed"<<endl;

	if(pthread_rwlock_init(&send_num_rwlock,NULL)<0)
		cerr<<"init rwlock failed"<<endl;

}


//发送函数
/* 不封装send函数
ssize_t sendn(int sockfd,const void *buf,size_t len,int flags)
{
	len=len>1024?1028:len;
	if(send(sockfd,buf,1028,flags)<0)
	{
		cerr<<"something wrong when send"<<endl;
		return -1;
	}

	return len;
}
*/
/*
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
