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
	while(true){
		pthread_mutex_lock(&sendbuf_mutex);
		//等待sendbuf不为空
		if(strlen(sendbuf.body)==0)
		{
			cout<<"wait for not_empty"<<endl;
			if(pthread_cond_wait(&not_empty,&sendbuf_mutex)<0)
				cerr<<"wait not_empty failed"<<endl;
		}
		//对send_num加读锁
		pthread_rwlock_rdlock(&send_num_rwlock);
		send(sock,&sendbuf,4+send_num,0);
		pthread_rwlock_unlock(&send_num_rwlock);
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
			while(cmdline.length()>20)
			{
				cout<<"用户名不能超过二十个字节"<<endl;
				getline(cin,cmdline);
			}
			username=cmdline;
			//对sendbuf加锁
			pthread_mutex_lock(&sendbuf_mutex);
			strcpy(sendbuf.body,cmdline.c_str());
			//对send_num 加写锁
			pthread_rwlock_wrlock(&send_num_rwlock);
			send_num=strlen(sendbuf.body);
			sendbuf.length=htonl(send_num);
			pthread_rwlock_unlock(&send_num_rwlock);
			//唤醒发送线程
			if(pthread_cond_signal(&not_empty)<0)
				cerr<<"signal not_empty failed"<<endl;
			pthread_mutex_unlock(&sendbuf_mutex);
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
	size_t len;
	while(true){
		cout<<endl;
		memset(&recvbuf,0,sizeof(recvbuf));
		
		len=recvn(sock,recvbuf.body,&len,0);
		if(len==-1){
			cout<<"接收失败"<<endl;
			break;
		}
		if(len==0)
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
