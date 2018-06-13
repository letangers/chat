#ifndef _RECV_AND_SEND_H_
#define _RECV_AND_SEND_H_

#include "init.h"
#include "parse.h"

#include <errno.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <pthread.h>

#include <cstring>
#include <string>
#include <iostream>
using namespace std;


void * send_and_recv(void * arg){
	//参数传入套接字，保存在sock中
	int sock=*((int *)arg);
	//接收缓冲区
	package recvbuf;
	//发送缓冲区
	package sendbuf;
	memset(&sendbuf,0,sizeof(sendbuf));
	memset(&recvbuf,0,sizeof(recvbuf));
	size_t ret;
	//保存结构体首部
	int n;
	//保存对应套接字用户名
	string *username=new string("");
	//保存解析出来的命令
	string *cmd=new string("");
	//保存解析出来的参数
	string *args=new string("");
	//保存解析出来的数据
	string *data=new string("");
	//用户列表迭代器
	map<string,int>::iterator *iter=new map<string,int>::iterator;	
	
	//将用户名和sock建立联系
	do
	{
		sendbuf.body[0]='0';
		strcpy(sendbuf.body+1,"请输入用户名");
		n=strlen(sendbuf.body);
		sendbuf.length=htonl(n);
		send(sock,&sendbuf,n+4,0);
		ret=recvn(sock,&recvbuf,&ret,0);
		cout<<"用户输入用户名>>>"<<recvbuf.body<<endl;
		if(ret==0)
		{
			cout<<"connection break when recv username"<<sock<<endl;
			close(sock);
			pthread_exit((void *)(-1));
		}
		if (ret==-1){
			cerr<<"recv username failed"<<sock<<endl;
			close(sock);
			pthread_exit((void *)(-1));
		}
		*username=recvbuf.body;
	}while(online_user_table.count(*username)==1);
	online_user_table.insert(pair<string,int>(*username,sock));

	//服务器转发
	while (true){
		memset(&recvbuf,0,sizeof(recvbuf));
		memset(&sendbuf,0,sizeof(sendbuf));
		
		//接收来源命令
		ret=recvn(sock,&recvbuf,&ret,0);
		if(ret==0)
		{
			cout<<"connection break  "<<sock<<endl;
			break;
		}
		if (ret==-1)
		{
			cerr<<"recv buf failed  "<<sock<<endl;
			break;
		}
		
		//解析命令
		parse_command(recvbuf.body,cmd,args,data);
		cout<<"命令:"<<*cmd<<" |参数:"<<*args<<" |数据:"<<*data<<endl;
		if(*cmd=="showonline"){
			for(*iter=online_user_table.begin();*iter!=online_user_table.end();(*iter)++)
			{
				*data+=(*iter)->first;
				*data+='|';
			}
			*args=*username;
			*data="server online|"+*data;
		}
		if(*cmd=="sendto"){
			if(*args=="all"){
				*data=*username+" messagetoall|"+*data;
				for(*iter=online_user_table.begin();*iter!=online_user_table.end();(*iter)++)
				{
					sendbuf.body[0]='0';
					strcpy(sendbuf.body+1,data->c_str());
					n=strlen(sendbuf.body);
					sendbuf.length=htonl(n);
					send((*iter)->second,&sendbuf,n+4,0);
				}
				continue;
			}
			else if(online_user_table.count(*args)==0){
				*data="server tips|用户不存在";
				*args=*username;
			}
			else{
				*data=*username+" message|"+*data;	
			}
		}

		//发送数据
		cout<<"发出的数据是"<<*data<<endl;
		sendbuf.body[0]='0';
		strcpy(sendbuf.body+1,data->c_str());
		n=strlen(sendbuf.body);
		sendbuf.length=htonl(n);
		send(online_user_table[*args],&sendbuf,n+4,0);
	}
	online_user_table.erase(*username);
	delete username;
	delete cmd;
	delete args;
	delete data;
	delete iter;
	close(sock);
	return (void*)0;
}
#endif
