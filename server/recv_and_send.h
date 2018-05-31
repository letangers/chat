#ifndef _RECV_AND_SEND_H_
#define _RECV_AND_SEND_H_

#include "init.h"
#include <string>
#include <sys/socket.h>
#include <pthread.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
using namespace std;

void * send_and_recv(void * arg){
	int sock=*((int *)arg);
	char recvbuf[1024]={0};
	char sendbuf[1024]="请输入用户名";
	send(sock,sendbuf,strlen(sendbuf),0);
	int ret;
	ret=recv(sock,recvbuf,sizeof(recvbuf),0);
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
	string *username=new string;
	*username=recvbuf;
	online_user_table.insert(pair<string,int>(*username,sock));
	while (true){
		memset(recvbuf,0,sizeof(recvbuf));
		ret=recv(sock,recvbuf,sizeof(recvbuf),0);
		if(ret==0){
			cout<<"connection break  "<<*((int *)arg)<<endl;
			break;
		}
		else if (ret==-1){
			cerr<<"recv buf failed  "<<*((int *)arg)<<endl;
			break;
		}
		     else 
			cout<<"everything is ok  "<<*((int *)arg)<<endl;
		cout<<recvbuf<<endl;
		if(recvbuf[0]=='1'){
			send(online_user_table["1"],recvbuf,ret,0);
		}
		if(recvbuf[0]=='2'){
			send(online_user_table["2"],recvbuf,ret,0);
		}
	}
	close(sock);
	return (void*)0;
}

#endif
