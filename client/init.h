#ifndef _INIT_H_
#define _INIT_H_

#include <csignal>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
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

#endif
