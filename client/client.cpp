#include "init.h"
#include "parse.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <csignal>

#include <pthread.h>
using namespace std;

string sendbuf;
string cmdline;

int execute_showcommand(){
	cout<<">>>show online"<<endl<<'\t';
	cout<<"you can browse user list online"<<endl<<endl;
	cout<<">>>sendto sb"<<endl<<'\t';
	cout<<"you can send message to someone online,if the arg is all,send the message to all online"<<endl;
	
	return 0;
}

int execute(){
	if (cmd=="show"){
		sendbuf="showonline |";
		return 0;
	}
	if (cmd=="sendto")
		if(arg=="all"){
			sendbuf="sendto all|";
			return 0;
		}
		else{
			cout<<"input the message to "<<arg<<">>>";
			cmdline="";
			getline(cin,cmdline);
			sendbuf="sendto "+arg+"|"+cmdline;
			return 0;
		}
	return -1;
}


void *send_to_service(void *args){
	if(signal(SIGINT,handle)<0)
		cerr<<"something wrong when installing sub signal"<<endl;
	if(signal(SIGUSR1,endprocess)<0)
		cerr<<"something wrong when installing sub SIGUSR1"<<endl;;
	while(getline(cin,cmdline)){
		sendbuf="";
		cmdline="";
		parse_command(&cmdline);
		if (cmd=="exit"){
			cout<<"exit"<<endl;
			break;
		}
		if (cmd=="showcommand"){
			execute_showcommand();
			continue;
		}
		if (execute()<0){
			cout<<"command wrong"<<endl;
			continue;
		}
		
		send(sock,sendbuf.c_str(),sendbuf.length(),0);
	}
	close(sock);
	exit(EXIT_SUCCESS);
	return (void *)0;
}
void recv_from_service(){
	if(signal(SIGINT,handle)<0)
		cerr<<"something wrong when installing signal"<<endl;
	if(signal(SIGUSR1,endprocess)<0)
		cerr<<"something wrong when installing SIGUSR1"<<endl;
	char recvbuf[1024]={0};
	while(true){
		int ret=recv(sock,recvbuf,sizeof(recvbuf),0);
		if(ret==-1){
			cout<<"接收失败"<<endl;
			break;
		}
		if(ret==0)
		{
			cout<<"服务端断开了连接"<<endl;
			break;
		}
		parse_server(recvbuf);
		cout<<"[receive from "<<cmd<<"] "<<arg<<endl;
		memset(recvbuf,0,sizeof(recvbuf));
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

	pthread_t tid;
	if(pthread_create(&tid,NULL,send_to_service,NULL)<0)
		cerr<<"create thread failed"<<endl;
	
	recv_from_service();
	return 0;
}
