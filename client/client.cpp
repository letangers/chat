#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <csignal>
using namespace std;

int sock;

void handle(int sig){
	close(sock);
	cout<<"exit by ctrl c"<<endl;
	exit(EXIT_SUCCESS);
}

int main(void){
	if(signal(SIGINT,handle)<0)
		cerr<<"something wrong when installing signal"<<endl;

	if((sock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		cout<<"something wrong when creating socket"<<endl;
	struct sockaddr_in cliaddr;
	memset(&cliaddr,0,sizeof(cliaddr));
	cliaddr.sin_family=AF_INET;
	cliaddr.sin_port=htons(12138);
	cliaddr.sin_addr.s_addr=inet_addr("123.207.139.132");
	if(connect(sock,(struct sockaddr*)&cliaddr,sizeof(cliaddr))<0)
		cout<<"something wrong when connect"<<endl;
	char sendbuf[1024]={0};
	char recvbuf[1024]={0};
	while(cin.getline(sendbuf,1024)){
		if(strcmp(sendbuf,"exit")==0){
			cout<<"exit"<<endl;
			break;
		}
		write(sock,sendbuf,strlen(sendbuf));
		int ret=read(sock,recvbuf,sizeof(recvbuf));
		if(ret==0)
		{
			cout<<"服务端断开了连接"<<endl;
			break;
		}
		cout<<"[receive from server]"<<recvbuf<<endl;
		memset(sendbuf,0,sizeof(sendbuf));
		memset(recvbuf,0,sizeof(recvbuf));
		cin.clear();
	}
	close(sock);
	return 0;
}
