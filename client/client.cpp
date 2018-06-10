#include "init.h"
#include "parse.h"

#include <unistd.h>
#include <cstdlib>

#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <csignal>
#include <pthread.h>

#include <iostream>
using namespace std;


void *send_to_service(void *args)
{
	if(signal(SIGINT,handle)<0)
		cerr<<"something wrong when installing sub signal"<<endl;
	//保存发送时首部数据，即body长度
	int n;
	while(getline(cin,cmdline))
	{
		memset(&sendbuf,0,sizeof(sendbuf));
		if(is_username)
		{
			is_username=false;
			strcpy(sendbuf.body,cmdline.c_str());
			username=cmdline;
		}
		else
		{
			parse_command(&cmdline);
			cout<<"--"<<cmd<<"--"<<arg<<"--"<<endl;
			if (cmd=="exit")
			{
				cout<<"exit"<<endl;
				break;
			}
			int execute_code=execute();
			if (execute_code==1)
			{
				continue;
			}
			if (execute_code==-1)
			{
				cout<<"command wrong"<<endl;
				continue;
			}
		}
		n=strlen(sendbuf.body);
		sendbuf.length=htonl(n);
		sendn(sock,&sendbuf,4+n,0);
		cmdline="";

	}
	close(sock);
	exit(EXIT_SUCCESS);
	return (void *)0;
}
void recv_from_service(){
	if(signal(SIGINT,handle)<0)
		cerr<<"something wrong when installing signal"<<endl;
	//保存接收到的首部内容，即将要接收的body长度
	int n;
	while(true){
		cout<<endl;
		memset(&recvbuf,0,sizeof(recvbuf));
		int ret=recvn(sock,&recvbuf.length,4,0);
		if(ret==-1){
			cerr<<"接收首部四个字节时出错"<<endl;
			break;
		}
		else if(ret<4){
			cerr<<"服务端关闭"<<endl;
			break;
		}
		n=ntohl(recvbuf.length);

		ret=recvn(sock,recvbuf.body,n,0);
		if(ret==-1){
			cout<<"接收失败"<<endl;
			break;
		}
		if(ret==0)
		{
			cout<<"服务端断开了连接"<<endl;
			break;
		}
		parse_server(recvbuf.body);
		if(cmd=="请输入用户名"){
			if(is_username)
				cout<<cmd<<">>>";
			else
				cout<<"用户名已存在，请重新输入>>>";
			is_username=true;
			continue;
		}
		if(cmd=="online"){
			cout<<"there are online user |"<<arg<<endl;
		}
		else
		{
			if (cmd=="用户不存在")
				cout<<cmd<<endl;
			else
				cout<<endl<<"[receive from "<<cmd<<"] "<<arg<<endl;
		}
	}
	close(sock);
}

int main(void){

	if((sock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		cout<<"something wrong when creating socket"<<endl;
	struct sockaddr_in cliaddr;
	memset(&cliaddr,0,sizeof(cliaddr));
	cliaddr.sin_family=AF_INET;
	cliaddr.sin_port=htons(12138);
	cliaddr.sin_addr.s_addr=inet_addr("123.207.139.132");
	if(connect(sock,(struct sockaddr*)&cliaddr,sizeof(cliaddr))<0)
		cout<<"something wrong when connect"<<endl;

	memset(&sendbuf,0,sizeof(sendbuf));
	memset(&recvbuf,0,sizeof(recvbuf));
/*
	int n;
	int ret=recvn(sock,&recvbuf.length,4,0);
	n=ntohl(recvbuf.length);
	ret=recvn(sock,recvbuf.body,n,0);
	parse_server(recvbuf.body);
	if(cmd=="请输入用户名")
	{
		getline(cin,username);
		strcpy(sendbuf.body,username.c_str());
		n=strlen(sendbuf.body);
		sendbuf.length=htonl(n);
		sendn(sock,&sendbuf,n+4,0);
	}
*/
	pthread_t tid;
	if(pthread_create(&tid,NULL,send_to_service,NULL)<0)
		cerr<<"create thread failed"<<endl;
	
	recv_from_service();
	return 0;
}
