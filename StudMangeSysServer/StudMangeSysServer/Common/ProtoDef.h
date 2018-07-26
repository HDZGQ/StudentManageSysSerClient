
#ifndef __PROTODEF_H__
#define __PROTODEF_H__

//最大科目数
#define MAXSUBJECTSCOUNT 20
//批量查询单次最大返回记录数
#define MAXBATCHSELECTACKCOUNT 10

#if 0
//协议主ID
enum Main_ID
{
	MAIN_ID_INVALID          =   0,
	MAIN_ID_START            =   0, //有效值起始值
	MAIN_ID_LOGINREGISTER    =   1,
	MAIN_ID_SCORE            =   2,
	MAIN_ID_USERINFO         =   3,

	MAIN_ID_END                     //有效值终止值
};
#endif

//协议辅助ID
enum Assist_ID
{
	/*
	* 服务端自己使用
	*/
	ASSIST_ID_INVALID								 =   0,
	ASSIST_ID_SPECIAL_START                          =   0,     //特殊有效值起始值
	ASSIST_ID_SPECIAL_GET_EXIAT_SUBJECTS             =   1,     //获取存在的科目


	ASSIST_ID_SPECIAL_END                                 ,     //特殊有效值终止值


	/*------------------------------------------------------------------------------*/

	/*
	* 与客户端交互
	*/
	ASSIST_ID_START									 =   10000, //有效值起始值
	ASSIST_ID_LOGIN_REQ								 =   10001, //登录请求
	ASSIST_ID_LOGIN_ACK								 =   10002, //登录回复
	ASSIST_ID_REGISTER_REQ							 =   10003, //注册请求
	ASSIST_ID_REGISTER_ACK							 =   10004, //注册回复
	ASSIST_ID_EXIT_SYS_REQ							 =   10005, //退出系统
	ASSIST_ID_GET_SUBJECTS_REQ						 =   10006, //获取剩余和现有科目请求
	ASSIST_ID_GET_SUBJECTS_ACK						 =   10007, //获取剩余和现有科目回复
	ASSIST_ID_ALTER_SUBJECTS_REQ					 =   10008, //增减科目请求
	ASSIST_ID_ALTER_SUBJECTS_ACK					 =   10009, //增减科目回复
	ASSIST_ID_ADD_SINGLE_SCORE_REQ					 =   10010, //单条添加成绩请求
	ASSIST_ID_ADD_SINGLE_SCORE_ACK					 =   10011, //单条添加成绩回复
	ASSIST_ID_SELECT_SINGLE_SCORE_REQ			     =   10012, //单条查询成绩请求
	ASSIST_ID_SELECT_SINGLE_SCORE_ACK			     =   10013, //单条查询成绩回复
	ASSIST_ID_SELECT_BATCH_SCORE_REQ			     =   10014, //批量查询成绩请求
	ASSIST_ID_SELECT_BATCH_SCORE_ACK			     =   10015, //批量查询成绩回复
	ASSIST_ID_UPDATE_SINGLE_SCORE_REQ			     =   10016, //单条更新成绩请求
	ASSIST_ID_UPDATE_SINGLE_SCORE_ACK			     =   10017, //单条更新成绩回复
	ASSIST_ID_DELETE_SCORE_REQ					     =   10018, //删除成绩请求
	ASSIST_ID_DELETE_SCORE_ACK					     =   10019, //删除成绩回复


	ASSIST_ID_END												//有效值终止值
};


//协议头定义
struct HeadData
{
//	int MainID;
	int AssistID;
	int Len;

	HeadData()
	{
		memset(this, 0, sizeof(HeadData));
	}
};

//登录请求   assist[10001]
struct SC_MSG_LOGIN_REQ
{
	char cAccount[31];
	char cPWD[31];
//	int  OperPerId;


	SC_MSG_LOGIN_REQ()
	{
		memset(this, 0, sizeof(SC_MSG_LOGIN_REQ));
	}
};

//登录回复   assist[10002]
struct CS_MSG_LOGIN_ACK
{
	bool bSucceed;
	unsigned int iUserId;
	char cAccount[31];
	char cName[31];
	short sIdent;
	short sSex;
	char cOperPer[150];

	CS_MSG_LOGIN_ACK()
	{
		memset(this, 0, sizeof(SC_MSG_LOGIN_REQ));
	}
};

//注册请求   assist[10003]
struct SC_MSG_REGISTER_REQ
{
	char cName[31];
	char cAccount[31];
	char cPWD[31];
	char cSex[3];
	char cIdent[3]; //只能注册学生和老师。 1学生 2老师
//	int  OperPerId;

	SC_MSG_REGISTER_REQ()
	{
		memset(this, 0, sizeof(SC_MSG_REGISTER_REQ));
	}
};

//注册回复   assist[10004]
struct CS_MSG_REGISTER_ACK
{
	bool bSucceed;
	unsigned int iUserId;
	char cAccount[31];
	char cName[31];
	short sIdent;
	short sSex;
	char cOperPer[150];

	CS_MSG_REGISTER_ACK()
	{
		memset(this, 0, sizeof(CS_MSG_REGISTER_ACK));
	}
};

//退出系统   assist[10005]
struct CS_MSG_EXIT_SYS_REQ
{
	bool bExit;
	CS_MSG_EXIT_SYS_REQ()
	{
		memset(this, 0, sizeof(CS_MSG_EXIT_SYS_REQ));
	}
};

//获取剩余和现有科目请求   assist[10006]
struct CS_MSG_GET_SUBJECTS_REQ
{
	short sGetType; //1剩余科目（未增加）  2现有科目
	CS_MSG_GET_SUBJECTS_REQ()
	{
		memset(this, 0, sizeof(CS_MSG_GET_SUBJECTS_REQ));
	}
};

//获取剩余和现有科目回复   assist[10007]
struct CS_MSG_GET_SUBJECTS_ACK
{
	bool bSucceed;
	short sGetType; //1剩余科目（未添加）  2现有科目
	char cCanAlterSubjects[60];
	CS_MSG_GET_SUBJECTS_ACK()
	{
		memset(this, 0, sizeof(CS_MSG_GET_SUBJECTS_ACK));
	}
};

//增减科目请求   assist[10008]
struct CS_MSG_ALTER_SUBJECTS_REQ
{
	short sGetType; //1增加科目  2删除科目
	short sSubjectId;
	CS_MSG_ALTER_SUBJECTS_REQ()
	{
		memset(this, 0, sizeof(CS_MSG_ALTER_SUBJECTS_REQ));
	}
};

//增减科目回复   assist[10009]
struct CS_MSG_ALTER_SUBJECTS_ACK
{
	bool bSucceed;
	short sGetType;
	short sSubjectId;
	CS_MSG_ALTER_SUBJECTS_ACK()
	{
		memset(this, 0, sizeof(CS_MSG_ALTER_SUBJECTS_ACK));
	}
};

//单条添加成绩请求   assist[10010]
struct CS_MSG_ADD_SINGLE_SCORE_REQ
{
	short sType; //1单科单条增加成绩 2现有所有科目单条增加成绩
	char cAccount[31]; //通过账号添加成绩  如果数据库没有这个账号，就得创建新的账号。所以不能通过userid，userid是自动增加的
	char sSubjectId[60]; //每个ID使用分隔符|隔开
	char sScore[90]; //每科分数使用分隔符|隔开
	CS_MSG_ADD_SINGLE_SCORE_REQ()
	{
		memset(this, 0, sizeof(CS_MSG_ADD_SINGLE_SCORE_REQ));
	}
};

//单条添加成绩回复   assist[10011]
struct CS_MSG_ADD_SINGLE_SCORE_ACK
{
	bool bSucceed;
	short sType; //1单科单条增加成绩 2现有所有科目单条增加成绩
	char cAccount[31];
	CS_MSG_ADD_SINGLE_SCORE_ACK()
	{
		memset(this, 0, sizeof(CS_MSG_ADD_SINGLE_SCORE_ACK));
	}
};

//单条查询成绩请求   assist[10012]
struct CS_MSG_SELECT_SINGLE_SCORE_REQ
{
	short sType; //1单科单条查询成绩 2现有所有科目单条查询成绩
	char cAccount[31]; //通过账号查询成绩  userid是自动增加的，所以不能用userid
	unsigned char sSubjectId[MAXSUBJECTSCOUNT]; //每个科目ID
	unsigned char bSubjectCount; //科目数
	CS_MSG_SELECT_SINGLE_SCORE_REQ()
	{
		memset(this, 0, sizeof(CS_MSG_SELECT_SINGLE_SCORE_REQ));
	}
};

//单条查询成绩回复   assist[10013]
struct CS_MSG_SELECT_SINGLE_SCORE_ACK
{
	short sType; //1单科单条查询成绩 2现有所有科目单条查询成绩
	unsigned int uUserid; //用户id
	char cName[31]; //姓名
	char cAccount[31]; //账号
	char cGrade[31]; //班级
	unsigned char bSubjectId[MAXSUBJECTSCOUNT]; //每个科目ID
	unsigned char bScore[MAXSUBJECTSCOUNT]; //每科分数
	unsigned char bSubjectCount; //科目数
	unsigned char bResCode; //0成功 1失败 2数据库没有账号信息或者没有成绩信息 3其他异常
	CS_MSG_SELECT_SINGLE_SCORE_ACK()
	{
		memset(this, 0, sizeof(CS_MSG_SELECT_SINGLE_SCORE_ACK));
	}
};

//批量查询成绩请求   assist[10014]
struct CS_MSG_SELECT_BATCH_SCORE_REQ
{
	short sType; //1单科批量查询成绩 2现有所有科目批量查询成绩
	char cGrade[31]; //班级 -- 为空则查询所有班级
	unsigned char sSubjectId[MAXSUBJECTSCOUNT]; //每个科目ID
	unsigned char cCondition[5]; //查询条件，每一个单元是一个条件  -- 单科批量只有排序和分数范围查询这两种条件
	unsigned char bRankFlag; //0没有排序 1升序 2降序 全科根据总分（没选总分这条件也是这样）排序，单科根据这科目排序
	unsigned char bScoreRange[3]; //分数范围，第三个元素为0数据没设置，为1设置数据了
	CS_MSG_SELECT_BATCH_SCORE_REQ()
	{
		memset(this, 0, sizeof(CS_MSG_SELECT_BATCH_SCORE_REQ));
	}
};

//批量查询成绩回复 -- 每次10组数据   assist[10015]
struct CS_MSG_SELECT_BATCH_SCORE_ACK
{
	short sType; //1单科批量查询成绩 2现有所有科目批量查询成绩
	unsigned char cCondition[5]; //查询条件，每一个单元是一个条件  -- 单科批量只有排序和分数范围查询这两种条件
	unsigned char bRankFlag; //0没有排序 1升序 2降序 全科根据总分（没选总分这条件也是这样）排序，单科根据这科目排序
	unsigned int uUserid[MAXBATCHSELECTACKCOUNT]; //用户id
	char cName[MAXBATCHSELECTACKCOUNT][31]; //姓名
	char cAccount[MAXBATCHSELECTACKCOUNT][31]; //账号 
	char cGrade[MAXBATCHSELECTACKCOUNT][31]; //班级 
	short sRank[MAXBATCHSELECTACKCOUNT]; //排名
	unsigned short bSum[MAXBATCHSELECTACKCOUNT]; //总分
	unsigned char bAverage[MAXBATCHSELECTACKCOUNT]; //平均分
	unsigned char bSubjectId[MAXBATCHSELECTACKCOUNT][MAXSUBJECTSCOUNT]; //每个科目ID
	unsigned char bScore[MAXBATCHSELECTACKCOUNT][MAXSUBJECTSCOUNT]; //每科分数
	unsigned char bSubjectCount; //科目数
	unsigned char bEndFalg; //发送完毕标志。0没有完毕 1完毕
	CS_MSG_SELECT_BATCH_SCORE_ACK()
	{
		memset(this, 0, sizeof(CS_MSG_SELECT_BATCH_SCORE_ACK));
	}
};

//单条更新成绩请求   assist[10016]
struct CS_MSG_UPDATE_SINGLE_SCORE_REQ
{
	short sType; //1单科单条更新成绩 2现有所有科目单条更新成绩
	char cAccount[31]; //通过账号更改成绩  userid是自动增加的，所以不能用userid
	unsigned char sSubjectId[MAXSUBJECTSCOUNT]; //每个科目ID
	unsigned char bScore[MAXSUBJECTSCOUNT]; //每科分数
	unsigned char bSubjectCount; //科目数
	CS_MSG_UPDATE_SINGLE_SCORE_REQ()
	{
		memset(this, 0, sizeof(CS_MSG_UPDATE_SINGLE_SCORE_REQ));
	}
};

//单条更新成绩回复   assist[10017]
struct CS_MSG_UPDATE_SINGLE_SCORE_ACK
{
	bool bSucceed;
	short sType; //1单科单条更新成绩 2现有所有科目单条更新成绩
	char cAccount[31]; //通过账号更改成绩 
	CS_MSG_UPDATE_SINGLE_SCORE_ACK()
	{
		memset(this, 0, sizeof(CS_MSG_UPDATE_SINGLE_SCORE_ACK));
	}
};

//删除成绩请求   assist[10018]
struct CS_MSG_DELETE_SCORE_REQ
{
	short sType; //1单条删除成绩 2批量删除成绩
	char cAccount[31]; //单条通过账号删除成绩  userid是自动增加的，所以不能用userid
	unsigned int uUserid[3]; //用户id范围，用于批量删除，第三个元素为0数据没设置，为1设置数据了
	CS_MSG_DELETE_SCORE_REQ()
	{
		memset(this, 0, sizeof(CS_MSG_DELETE_SCORE_REQ));
	}
};

//删除成绩回复   assist[10019]
struct CS_MSG_DELETE_SCORE_ACK
{
	bool bSucceed;
	short sType; //1单条删除成绩 2批量删除成绩
	CS_MSG_DELETE_SCORE_ACK()
	{
		memset(this, 0, sizeof(CS_MSG_DELETE_SCORE_ACK));
	}
};


#endif