#ifndef _INIT_H_
#define _INIT_H_

#include <map>
#include <string>
using namespace std;

struct user_table{
	string username;
	string password;
};

map<string,int> online_user_table;

#endif
