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
			strcpy(sendbuf.body,"showonline |");
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
		pthread_mutex_lock(&sendbuf_mutex);
		strcpy(sendbuf.body,temp.c_str());
		if(pthread_cond_signal(&not_empty)<0)
			cerr<<"signal not_empty failed"<<endl;
		pthread_mutex_unlock(&sendbuf_mutex);
		return 0;
	}
	return -1;
}

//解析接收到的数据
int parse_server(char *recvbuf)
{
	cmd="";
	arg="";	
	for(unsigned int i=0;i<strlen(recvbuf);i++)
	{
		if(recvbuf[i]==' ')
		{
			arg=string(recvbuf+i+1);
			break;
		}
		else
			cmd+=recvbuf[i];
	}
	return 0;
}

#endif
