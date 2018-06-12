#ifndef _PARSE_H_
#define _PARSE_H_

#include "init.h"
#include <iostream>
#include <string>
#include <cstring>
using namespace std;


//解析用户输入
int parse_command(string * cmdline)
{
	int type=0;
	cmd="";
	arg="";
	for(unsigned int i=0;i<(*cmdline).length();i++)
	{
		if((*cmdline)[i]==' '||(*cmdline)[i]=='\t')
			type=1;
		else
		{
			if(type==0)
				cmd+=(*cmdline)[i];
			else
				arg+=(*cmdline)[i];
		}
	}
	return 0;
}

//执行解析出的命令
int execute()
{
	if (cmd=="show")
	{
		if(arg=="command")
		{
			cout<<">>>show online"<<endl<<'\t';
			cout<<"you can browse user list online"<<endl<<endl;
			cout<<">>>sendto sb"<<endl<<'\t';
			cout<<"you can send message to someone online,if the arg is all,send the message to all online"<<endl;
			return 1;
		}
		else if(arg=="online")
		{	
			pthread_mutex_lock(&sendbuf_mutex);
			sendbuf.body[0]='1';
			strcpy(sendbuf.body+1,"showonline |");
			//对send_num加写锁
			pthread_rwlock_wrlock(&send_num_rwlock);
			send_num=strlen(sendbuf.body);
			sendbuf.length=htonl(send_num);
			pthread_rwlock_unlock(&send_num_rwlock);
			//唤醒发送进程
			if(pthread_cond_signal(&not_empty)<0)
				cerr<<"signal not_empty failed"<<endl;
			pthread_mutex_unlock(&sendbuf_mutex);
			return 0;
		}
	}
	if (cmd=="sendto")
	{
		cout<<"input the message to "<<arg<<">>>";
		cmdline="";
		getline(cin,cmdline);
		string temp="sendto "+arg+"|"+cmdline;
		//用来存储是这是本段第几次传输
		size_t count=0;
		do
		{
			//对sendbuf加锁
			pthread_mutex_lock(&sendbuf_mutex);
			if(temp.length()<=1023)
				sendbuf.body[0]='0';
			else
			{
				if(count==0)
					sendbuf.body[0]='1';
				else
					sendbuf.body[0]='2';
			}
			//对send_num 加写锁
			pthread_rwlock_wrlock(&send_num_rwlock);
			if(temp.length()>count*1023+1023)
				send_num=1024;
			else
			{
				send_num=temp.length()-count*1023+1;
				if(count!=0)
					sendbuf.body[0]='3';
			}
			strncpy(sendbuf.body+1,temp.c_str()+count*1023,send_num-1);
			count++;
			sendbuf.length=htonl(send_num);
			pthread_rwlock_unlock(&send_num_rwlock);
			//唤醒发送线程
			if(pthread_cond_signal(&not_empty)<0)
				cerr<<"signal not_empty failed"<<endl;
			pthread_mutex_unlock(&sendbuf_mutex);
		}while(temp.length()>count*1023);
		count=0;
		return 0;
	}
	if (cmd=="sendfileto")
	{
		cout<<"输入文件名>>>";
		getline(cin,cmdline);
		;
	}
	return -1;
}

//解析接收到的数据
int parse_server(char *recvbuf)
{
	char type=recvbuf[0];
	if(type=='0')
	{
	}
	else if(type=='1')
	{
		cmd="";
		arg="";	
		for(unsigned int i=1;i<strlen(recvbuf);i++)
		{
			if(recvbuf[i]==' ')
			{
				arg=string(recvbuf+i+1);
				break;
			}
			else
				cmd+=recvbuf[i];
		}
	}
	return 0;
}

#endif
