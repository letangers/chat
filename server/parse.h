#ifndef _PARSE_H_
#define _PAESE_H_
#include <string>
using namespace std;

#define MAXARG 10


int parse_command(const string *recvline,string *cmd,string *arg){
	int type=0;
	*cmd="";
	*arg="";
	for(unsigned int i=0;i<recvline->length();i++){
		//通过客户端保证命令之后是空格，参数之后是 | 
		if((*recvline)[i]=='|')
			break;
		if((*recvline)[i]==' ')
			type=1;
		else{
			if(type==0)
				*cmd+=(*recvline)[i];
			else
				*arg+=(*recvline)[i];
		}
	}
	return 0;
}


#endif
