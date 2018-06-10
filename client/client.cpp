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
	while(true){
		pthread_mutex_lock(&sendbuf_mutex);
		if(strlen(sendbuf.body)==0)
		{
			cout<<"wait for not_empty"<<endl;
			if(pthread_cond_wait(&not_empty,&sendbuf_mutex)<0)
				cerr<<"wait not_empty failed"<<endl;
		}
	
		n=strlen(sendbuf.body);
		sendbuf.length=htonl(n);
		sendn(sock,&sendbuf,4+n,0);
		memset(&sendbuf,0,sizeof(sendbuf));
		pthread_mutex_unlock(&sendbuf_mutex);
	}
	close(sock);
	exit(EXIT_SUCCESS);
	return (void *)0;
}

void * read_from_local(void *args){
	while(getline(cin,cmdline))
	{
		if(is_username)
		{
			is_username=false;
			pthread_mutex_lock(&sendbuf_mutex);
			strcpy(sendbuf.body,cmdline.c_str());
			if(pthread_cond_signal(&not_empty)<0)
				cerr<<"signal not_empty failed"<<endl;
			pthread_mutex_unlock(&sendbuf_mutex);
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
		cmdline="";
	}
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

	pthread_once(&once_control,init_routine);

	//主线程接收来在服务器的数据
	//send线程向服务器发送数据，read线程从本地读取要发送的数据
	pthread_t send_tid,read_tid;
	if(pthread_create(&send_tid,NULL,send_to_service,NULL)<0)
		cerr<<"create send thread failed"<<endl;
	if(pthread_create(&read_tid,NULL,read_from_local,NULL)<0)
		cerr<<"create read thread failed"<<endl;
	

	recv_from_service();

	int *rval;
	pthread_join(send_tid,(void **)&rval);
	pthread_join(read_tid,(void **)&rval);

	pthread_cond_destroy(&not_empty);
	pthread_mutex_destroy(&sendbuf_mutex);
	
	return 0;
}
