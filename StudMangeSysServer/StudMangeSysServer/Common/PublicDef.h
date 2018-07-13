#ifndef __PUBLICDEF_H__
#define __PUBLICDEF_H__

#include <string>
using namespace std;

//操作权限宏 与服务端对应  登录和注册操作权限，每个玩家都拥有
enum OperPermission
{
	OPER_PER_INVALID        =    0,
	OPER_PER_START	        =    0, //有效值起始值
	OPER_PER_LOGIN          =    1, 
	OPER_PER_REGISTER       =    2,

	OPER_PER_END                    //有效值终止值
};

enum IdentType
{
	IDENT_TYPE_INVALID      =    0,
	IDENT_TYPE_START	    =    0, //有效值起始值
	IDENT_TYPE_STUDENT      =    1, 
	IDENT_TYPE_TEACHER      =    2,
	IDENT_TYPE_ADMIN        =    3,

	IDENT_TYPE_END                    //有效值终止值
};

struct UserInfo
{
	//用户常记录
	unsigned int iUserId;
	string strAccount;
	string strIP;
	unsigned short sPort;
	string strAuthority;


	//用户动态记录
	short sRegNeedCount;  //注册需时候有3次数据库操作： 插入客户端传来的信息  查找userid和身份标识  根据身份生成权限再插入数据库

	UserInfo()
	{
		iUserId = 0;
		strAccount = "";
		strIP = "";
		sPort = 0;
		strAuthority = "";


		sRegNeedCount = 0;
	}
};


#endif