#ifndef __MYSQLDEF_H__
#define __MYSQLDEF_H__

//数据库队列最大数量值
#define MaxMysqlMsgCount    50

//数据库连接端口
#define MysqlConnPort       3306

//数据库操作类型
enum MysqlOper
{
	MYSQL_OPER_INVALID   = 0,
	MYSQL_OPER_START     = 0,  //有效值起始值
	MYSQL_OPER_INSERT    = 1,  //增
	MYSQL_OPER_DELETE    = 2,  //删
	MYSQL_OPER_SELECT    = 3,  //查
	MYSQL_OPER_UPDATE    = 4,  //改
	MYSQL_OPER_ALTER     = 5,  //更改表属性

	MYSQL_OPER_END             //有效值终止值
};

//数据库队列结构体
typedef struct 
{
	string strMysql;
	MysqlOper mysqlOper;
	int operPer; //权限操作类型
	unsigned __int64 SocketId;
} MysqlMsgData, *PtrMysqlMsgData;



#endif