#ifndef _RECV_AND_SEND_H_
#define _RECV_AND_SEND_H_

#include "init.h"
#include "parse.h"
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
	int ret;
	string *username=new string;
	do
	{
		
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
		*username=recvbuf;
	}while(online_user_table.count(*username)==1);
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
		string temp=parse(recvbuf);
		cout<<recvbuf<<endl;
		send(online_user_table[temp],recvbuf,ret,0);
	}
	close(sock);
	return (void*)0;
}

#endif
