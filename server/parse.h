#ifndef _PARSE_H_
#define _PAESE_H_
#include <string>
#include <cstring>
using namespace std;

#define MAXARG 10

//解析接收到的命令
int parse_command(char *recvline,string *cmd,string *arg,string * data){
	int type=0;
	*cmd="";
	*arg="";
	*data="";
	for(unsigned int i=0;i<strlen(recvline);i++){
		//通过客户端保证命令之后是空格，参数之后是 | 
		if(recvline[i]=='|'){
			*data=string(recvline+i+1);
			break;
		}
		if(recvline[i]==' ')
			type=1;
		else{
			if(type==0)
				*cmd+=recvline[i];
			else
				*arg+=recvline[i];
		}
	}
	return 0;
}


#endif
