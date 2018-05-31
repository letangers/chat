#include "init.cpp"
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


void *send_to_service(void *arg){
	if(signal(SIGINT,handle)<0)
		cerr<<"something wrong when installing sub signal"<<endl;
	if(signal(SIGUSR1,endprocess)<0)
		cerr<<"something wrong when installing sub SIGUSR1"<<endl;;
	char sendbuf[1024]={0};
	while(cin.getline(sendbuf,1024)){
		if(strcmp(sendbuf,"exit")==0){
			cout<<"exit"<<endl;
			break;
		}
		send(sock,sendbuf,strlen(sendbuf),0);
		memset(sendbuf,0,sizeof(sendbuf));
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
		if(ret==0)
		{
			cout<<"服务端断开了连接"<<endl;
			break;
		}
		cout<<"[receive from server] "<<recvbuf<<endl;
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
