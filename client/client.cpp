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

typedef struct PACKAGE{
	int length;
	string recvbuf;
};



string sendbuf="";
string cmdline="";
string username="";

ssize_t sendn(int sockfd,const void *buf,size_t len,int flags)
{
	size_t nleft=len;
	ssize_t nsend;

	char * bufp=(char*)buf;

	while(nleft>0)
	{
		if((nsend=send(sockfd,bufp,nleft,flags))<0)
		{
			if(errno==EINTR)
				continue;
			return -1;
		}
		else if (nsend==0)
			continue;
		bufp+=nsend;
		nleft-=nsend;
	}
	return len;
}



ssize_t recvn(int sockfd,void*buf,size_t len,int flags)
{
	size_t nleft=len;
	ssize_t nrecv;

	char *bufp=(char *)buf;

	while(nleft>0)
	{
		if((nrecv=recv(sockfd,bufp,nleft,flags))<0)
			{
				if(errno==EINTR)
					continue;
				return -1;
			}
		else if(nrecv==0)
			return len-nleft;
		bufp+=nrecv;
		nleft-=nrecv;
	}
	return len;

}







int execute_showcommand(){
	cout<<">>>show online"<<endl<<'\t';
	cout<<"you can browse user list online"<<endl<<endl;
	cout<<">>>sendto sb"<<endl<<'\t';
	cout<<"you can send message to someone online,if the arg is all,send the message to all online"<<endl;
	
	return 0;
}

int execute(){
	if (cmd=="show"){
		if(arg=="command"){
			execute_showcommand();
			return 1;
		}
		else if(arg=="online"){
			sendbuf="showonline |";
			return 0;
		}
	}
	if (cmd=="sendto")
	{
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
		parse_command(&cmdline);
		sendbuf="";
		cout<<"--"<<cmd<<"--"<<arg<<"--"<<endl;
		if (cmd=="exit"){
			cout<<"exit"<<endl;
			break;
		}
		int execute_code=execute();
		if (execute_code==1){
			cout<<">>>";
			continue;
		}
		if (execute_code==-1){
			cout<<"command wrong"<<endl;
			cout<<">>>";
			continue;
		}
		
		sendn(sock,sendbuf.c_str(),sendbuf.length(),0);
		cmdline="";
		cout<<">>>";

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
		int ret=recvn(sock,recvbuf,sizeof(recvbuf),0);
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
		if(cmd=="online"){
			cout<<"there are online user==="<<arg<<"==="<<endl;
		}
		else{
			cout<<endl<<"[receive from "<<cmd<<"] "<<arg<<endl;
		}
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

	char recvbuf[1024]={0};
	int ret=recvn(sock,recvbuf,/*sizeof(recvbuf)*/18,0);
	parse_server(recvbuf);
	if(cmd=="请输入用户名"){
		cout<<cmd<<">>>";
		getline(cin,username);
		sendn(sock,username.c_str(),username.length(),0);
	}
	pthread_t tid;
	if(pthread_create(&tid,NULL,send_to_service,NULL)<0)
		cerr<<"create thread failed"<<endl;
	
	recv_from_service();
	return 0;
}
