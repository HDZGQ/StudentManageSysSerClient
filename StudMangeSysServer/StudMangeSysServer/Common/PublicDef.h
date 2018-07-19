#ifndef __PUBLICDEF_H__
#define __PUBLICDEF_H__

//#define WIN32_LEAN_AND_MEAN
//#include <WinSock2.h>  
#include <string>


//操作权限宏 与服务端对应  登录和注册操作权限，每个玩家都拥有
enum OperPermission
{
	OPER_PER_INVALID                          =    0,
	OPER_PER_START	                          =    0, //有效值起始值
	OPER_PER_LOGIN                            =    1, 
	OPER_PER_REGISTER                         =    2,

	OPER_PER_ADDBATCHSCOREBYONESUBJECT        =    3, //单科批量增加成绩 -- 不是班级性也可以 （管理员和老师）
	OPER_PER_ADDBATCHSCOREBYSUBJECTS          =    4, //现有所有科目批量增加成绩 -- 不是班级性也可以 （管理员和老师）
	OPER_PER_ADDSINGLESCOREBYONESUBJECT       =    5, //单科单条增加成绩 （管理员和老师）
	OPER_PER_ADDSINGLESCOREBYSUBJECTS         =    6, //现有所有科目单条增加成绩 （管理员和老师）
	OPER_PER_UPDATEBATCHSCOREBYONESUBJECT     =    7, //单科批量更改成绩 -- 班级性，可选哪个班级；操作前，还要先把班级列出来 （管理员和老师）
	OPER_PER_UPDATEBATCHSCOREBYSUBJECTS       =    8, //现有所有科目批量更改成绩 -- 班级性，可选哪个班级 （管理员和老师）
	OPER_PER_UPDATESINGLESCOREBYONESUBJECT    =    9, //单科单条更改成绩 （管理员和老师）
	OPER_PER_UPDATESINGLESCOREBYSUBJECTS      =    10,//现有所有科目单条更改成绩 （管理员和老师）
	OPER_PER_SELECTBATCHSCOREBYONESUBJECT     =    11,//单科批量查询成绩 -- 班级性，可选哪个班级 （管理员和老师）
	OPER_PER_SELECTBATCHSCOREBYSUBJECTS       =    12,//现有所有科目批量查询成绩 -- 班级性，可选哪个班级 （管理员和老师）
	OPER_PER_SELECTSINGLESCOREBYONESUBJECT    =    13,//单科单条查询成绩 （管理员和老师； 学生：学生自己）
	OPER_PER_SELECTSINGLESCOREBYSUBJECTS      =    14,//现有所有科目单条查询成绩 （管理员和老师； 学生：学生自己）
	OPER_PER_DELETEBATCHSCORE			      =    15,//批量删除成绩 -- 班级性，可选哪个班级 （管理员和老师）
	OPER_PER_DELETESINGLESCORE		          =    16,//单条删除成绩 （管理员和老师）
	OPER_PER_ALTERADDONESCORESUBJECT		  =    17,//增加成绩科目 （管理员和老师）
	OPER_PER_ALTERDELETEONESCORESUBJECT		  =    18,//删除成绩科目 （管理员和老师）

	OPER_PER_ADDBATCHUSERINFO	              =    19,//批量增加用户信息 -- 不是班级性也可以，固定字段增加 （管理员和老师）
	OPER_PER_ADDSINGLEUSERINFO		          =    20,//单条增加用户信息 -- 固定字段增加 （管理员和老师）
	OPER_PER_UPDATEBATCHUSERINFOBYONE		  =    21,//单个字段批量更改用户信息 -- 班级性，可选哪个班级 （管理员和老师）
	OPER_PER_UPDATEBATCHUSERINFOBYMORE        =    22,//部分固定字段批量更改用户信息 -- 班级性，可选哪个班级；固定字段更改 （管理员和老师）
	OPER_PER_UPDATESINGLEUSERINFOEBYONE       =    23,//单个字段单条更改用户信息 （管理员：学生和老师 ；老师：老师自己，和学生； 学生：学生自己）
	OPER_PER_UPDATESINGLEUSERINFOBYMORE       =    24,//部分固定字段单条更改用户信息 -- 固定字段更改 （管理员：学生和老师； 老师：老师自己，和学生； 学生：学生自己）
	OPER_PER_SELECTBATCHUSERINFO	          =    25,//批量查询用户信息 -- 班级性，可选哪个班级；查询固定字段 （管理员和老师）
	OPER_PER_SELECTSINGLEUSERINFO		      =    26,//单条查询用户信息 -- 查询固定字段 （管理员和老师，学生自己）
	OPER_PER_DELETEBATCHUSERINFO			  =    27,//批量删除用户信息 -- 班级性，可选哪个班级 （管理员和老师）
	OPER_PER_DELETESINGLEUSERINFO		      =    28,//单条删除用户信息 （管理员和老师）

	OPER_PER_ADDAUTHORITBYONE		          =    29,//增加某用户一种或者多种权限 （管理员）
	OPER_PER_DELETEAUTHORITBYONE	          =    30,//删除某用户一种或者多种权限 （管理员）

	OPER_PER_END									  //有效值终止值
};


//批量查询成绩可用条件
enum SelectScoreCondition
{
	SELECT_SCORE_INVALID                      =    0,
	SELECT_SCORE_START	                      =    0, //有效值起始值
	SELECT_SCORE_RANK     	                  =    1, //排序 -- 分为升序和降序
	SELECT_SCORE_RANGE    	                  =    2, //分数范围 -- 起始分数结束分数
	SELECT_SCORE_TOTAL    	                  =    3, //总分查询
	SELECT_SCORE_AVERAGE    	              =    4, //平均分查询

	SELECT_SCORE_END								  //有效值终止值
};

//排序查询成绩升降序条件
enum SelectScoreRank
{
	SELECT_SCORE_RANK_INVALID                  =    0,
	SELECT_SCORE_RANK_START	                   =    0, //有效值起始值
	SELECT_SCORE_RANK_ASC	                   =    1, //升序
	SELECT_SCORE_RANK_DESC					   =    2, //降序

	SELECT_SCORE_RANK_END						       //有效值终止值
};

//批量查询用户信息可用条件
enum SelectUserInfoCondition
{
	SELECT_USERINFO_INVALID                    =    0,
	SELECT_USERINFO_START	                   =    0, //有效值起始值
	SELECT_USERINFO_RANGE    	               =    1, //用户id范围 -- 起始id结束id
	SELECT_USERINFO_GRADE    	               =    2, //班级查询
	SELECT_USERINFO_PART_NAME    	           =    3, //姓名模糊查询
	SELECT_USERINFO_SEX         	           =    4, //性别查询
	SELECT_USERINFO_COUNT         	           =    5, //统计查询 -- 查询当前的总记录数


	SELECT_USERINFO_END						          //有效值终止值
};

//学科
enum SubjectsType
{
	SUBJECTS_TYPE_INVALID                    =    0,
	SUBJECTS_TYPE_START	                     =    0, //有效值起始值
	SUBJECTS_TYPE_CHINESE                    =    1, //语文
	SUBJECTS_TYPE_MATH                       =    2, //数学
	SUBJECTS_TYPE_ENGLISH                    =    3, //英语
	SUBJECTS_TYPE_PHYSICS                    =    4, //物理
	SUBJECTS_TYPE_CHEMISTRY                  =    5, //化学
	SUBJECTS_TYPE_BIOLOGY                    =    6, //生物
	SUBJECTS_TYPE_HISTORY                    =    7, //历史
	SUBJECTS_TYPE_POLITICS                   =    8, //政治
	SUBJECTS_TYPE_GEOGRAPHY                  =    9, //地理

	SUBJECTS_TYPE_END						         //有效值终止值
};

//用户身份标识
enum IdentType
{
	IDENT_TYPE_INVALID      =    0,
	IDENT_TYPE_START	    =    0, //有效值起始值
	IDENT_TYPE_STUDENT      =    1, 
	IDENT_TYPE_TEACHER      =    2,
	IDENT_TYPE_ADMIN        =    3,

	IDENT_TYPE_END                    //有效值终止值
};


struct SubjectsData
{
	std::string strEnglishName;
	std::string strChineseName;
	SubjectsData()
	{
		strEnglishName = "";
		strChineseName = "";
	}
	SubjectsData(std::string strEName, std::string strCName)
	{
		strEnglishName = strEName;
		strChineseName = strCName;
	}
};


#endif