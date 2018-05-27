#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;


int main(void){
	int sock;
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
		write(sock,sendbuf,strlen(sendbuf));
		read(sock,recvbuf,sizeof(recvbuf));
		cout<<"[receive from server]"<<recvbuf;
		memset(sendbuf,0,sizeof(sendbuf));
		memset(recvbuf,0,sizeof(recvbuf));
	}
	close(sock);
	return 0;
}
