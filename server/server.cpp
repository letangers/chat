#include "recv_and_send.h"
#include <string>
#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstdlib>
#include <unistd.h>
#include <pthread.h>
using namespace std;


int main(void){
	int listenfd;
	if((listenfd=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
		cerr<<"something wrong when creating socket "<<endl;
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(12138);
	servaddr.sin_addr.s_addr=inet_addr("0.0.0.0");
	
	int on=1;
	//开启地址重复利用
	if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))<0)
		cerr<<"开启地址重复利用时出错"<<endl;

        if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
		cerr<<"something wrong when bind"<<endl;
	if(listen(listenfd,SOMAXCONN)<0)
		cerr<<"something wrong when listen"<<endl;
	
	struct sockaddr_in peeraddr;
	socklen_t peerlen=sizeof(peeraddr);

	while(true){
	int *conn=new int;
 	if((*conn=accept(listenfd,(struct sockaddr*)&peeraddr,&peerlen))<0)
		cerr<<"something wrong when accept"<<endl;
	cout<<"ip:"<<inet_ntoa(peeraddr.sin_addr)<<"  port:"<<ntohs(peeraddr.sin_port)<<endl;
	pthread_t tid;
	if(pthread_create(&tid,NULL,send_and_recv,conn)<0)
		cerr<<"create thread failed"<<endl;
	}
	close(listenfd);
	return 0;
}
