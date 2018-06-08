#ifndef _RECV_AND_SEND_H_
#define _RECV_AND_SEND_H_

#include <errno.h>
#include "init.h"
#include "parse.h"
#include <string>
#include <sys/socket.h>
#include <pthread.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
using namespace std;

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



void * send_and_recv(void * arg){
	int sock=*((int *)arg);
	char recvbuf[1024]={0};
	char sendbuf[1024];
	int ret;
	string *username=new string("");
	string *cmd=new string("");
	string *args=new string("");
	string *data=new string("");
	map<string,int>::iterator *iter=new map<string,int>::iterator;	
	
	memset(sendbuf,0,sizeof(sendbuf));
	memset(recvbuf,0,sizeof(recvbuf));

	do
	{
		strcpy(sendbuf,"请输入用户名");
		sendn(sock,sendbuf,sizeof(sendbuf),0);
		cout<<"sendn函数已执行完"<<endl;
		ret=recvn(sock,recvbuf,sizeof(recvbuf),0);
		cout<<"用户输入用户名>>>"<<recvbuf<<endl;
		if(ret==0){
			cout<<"connection break when recv the username "<<sock<<endl;
			close(sock);
			pthread_exit((void *)(-1));
		}
		if (ret==-1){
			cerr<<"recv buf failed when recv the username "<<*((int *)arg)<<endl;
			close(sock);
			pthread_exit((void *)(-1));
		}
		*username=recvbuf;
	}while(online_user_table.count(*username)==1);
	online_user_table.insert(pair<string,int>(*username,sock));
	while (true){
		memset(recvbuf,0,sizeof(recvbuf));
		memset(recvbuf,0,sizeof(sendbuf));
		ret=recvn(sock,recvbuf,sizeof(recvbuf),0);
		if(ret==0)
		{
			cout<<"connection break  "<<*((int *)arg)<<endl;
			break;
		}
		else if (ret==-1)
		{
			cerr<<"recv buf failed  "<<*((int *)arg)<<endl;
			break;
		}
		     else 
			cout<<"everything is ok  "<<*((int *)arg)<<endl;
		parse_command(recvbuf,cmd,args,data);
		cout<<"收到的命令是"<<*cmd<<endl;
		cout<<"参数为"<<*args<<endl;
		cout<<"数据为"<<*data<<endl;
		if(*cmd=="showonline"){
			for(*iter=online_user_table.begin();*iter!=online_user_table.end();(*iter)++)
				*data+=(*iter)->first;
			*args=*username;
			*data="online "+*data;
		}
		if(*cmd=="sendto"){
			if(*args=="all"){
				string temp;
				for(*iter=online_user_table.begin();*iter!=online_user_table.end();(*iter)++)
				{
					temp=*data;
					temp=*username+" "+temp;
					strcpy(sendbuf,temp.c_str());
					sendn((*iter)->second,sendbuf,sizeof(sendbuf),0);
				}
				continue;
			}
			else if(online_user_table.count(*args)==0){
				*data="用户不存在";
				*args=*username;
			}
			else{
				*data=*username+" "+*data;	
			}
		}
		cout<<"发出的数据是"<<*data<<endl;
		strcpy(sendbuf,data->c_str());
		cout<<recvbuf<<endl;
		sendn(online_user_table[*args],sendbuf,sizeof(sendbuf),0);
	}
	close(sock);
	return (void*)0;
}
#endif
