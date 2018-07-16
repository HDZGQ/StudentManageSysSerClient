#ifndef __PUBLICDEF_H__
#define __PUBLICDEF_H__

//#define WIN32_LEAN_AND_MEAN
//#include <WinSock2.h>  


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



#endif