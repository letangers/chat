#ifndef _PARSE_H_
#define _PARSE_H_

#include <iostream>
#include <string>
#include <cstring>
using namespace std;

//sendto sb;
//show online;
//showcommand;

string cmd="";
string arg="";

int parse_command(string * cmdline){
	int type=0;
	cmd="";
	arg="";
	for(unsigned int i=0;i<(*cmdline).length();i++){
		if((*cmdline)[i]==' '||(*cmdline)[i]=='\t')
			type=1;
		else{
			if(type==1)
				cmd+=(*cmdline)[i];
			else
				arg+=(*cmdline)[i];
		}
	}
	return 0;
}

int parse_server(char *recvbuf){
	cmd="";
	arg="";	
	for(unsigned int i=0;i<strlen(recvbuf);i++){
		if(recvbuf[i]==' ')
			arg=string(recvbuf+i+1);
		else
			cmd+=recvbuf[i];
	}
	return 0;
}

#endif
