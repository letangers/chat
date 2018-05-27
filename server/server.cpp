#include <iostream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
using namespace std;

int main(void){
	int listenfd;
	if((listenfd=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP)))
		cerr<<"something wrong when creating socket "<<endl;
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(12138);
	servaddr.sin_addr.s_addr=inet_addr("0.0.0.0");
	if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
		cerr<<"something wrong when bind"<<endl;
	if(listen(listenfd,SOMAXCONN)<0)
		cerr<<"something wrong when listen"<<endl;
	
	struct sockaddr_in peeraddr;
	socklen_t peerlen=sizeof(peeraddr);
	int conn;
	if((conn=accept(listenfd,(struct sockaddr*)&peeraddr,&peerlen))<0)
		cerr<<"something wrong when accept"<<endl;
	char recvbuf[1024];
	while (true){
		memset(recvbuf,0,sizeof(recvbuf));
		int ret=read(conn,recvbuf,sizeof(recvbuf));
		cout<<recvbuf<<endl;
		write(conn,recvbuf,ret);
	}
	close(conn);
	close(listenfd);
	return 0;
}
