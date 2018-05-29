#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <csignal>
using namespace std;

int sock;

void handle(int sig){
	close(sock);
	cout<<"recv signal SIGINT"<<endl;
	exit(EXIT_SUCCESS);
}

void endprocess(int sig){
	close(sock);
	cout<<"recv signal SIGUSR1"<<endl;
	exit(EXIT_SUCCESS);

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
	
	
	pid_t pid;
	pid=fork();
	if (pid==-1){
		cout<<"create sub process faile"<<endl;
		exit(EXIT_SUCCESS);
	}
	
	if(pid==0){
		if(signal(SIGINT,handle)<0)
			cerr<<"something wrong when installing sub signal"<<endl;
		if(signal(SIGUSR1,endprocess)<0)
			cerr<<"something wrong when installing sub SIGUSR1"<<endl;;
		char sendbuf[1024]={0};
		while(cin.getline(sendbuf,1024)){
			if(strcmp(sendbuf,"exit")==0){
				cout<<"exit"<<endl;
				kill(getppid(),SIGUSR1);
				break;
			}
			send(sock,sendbuf,strlen(sendbuf),0);
			memset(sendbuf,0,sizeof(sendbuf));
		
		}
		kill(getppid(),SIGUSR1);
		close(sock);
	}
	else{
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
				kill(pid,SIGUSR1);
				break;
			}
			cout<<"[receive from server] "<<recvbuf<<endl;
			memset(recvbuf,0,sizeof(recvbuf));
		}
		close(sock);
	}
	return 0;
}
