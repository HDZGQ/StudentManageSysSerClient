#include "MysqlMgr.h"
#include "LockTools.h"
#include "StringTool.h"


MysqlMgr::MysqlMgr() : 
		m_sUser("root"),
		m_sPswd("qq3197213"),
		m_sHost("localhost"),
		m_sDB("StudMangeSystem")
{
	m_IsConn = false;
	mysql_init(&m_MysqlCont);
	m_MysqlRes = NULL;

	InitializeCriticalSection(&_critical);
}

MysqlMgr::~MysqlMgr()
{
	if (!m_MysqlRes)
		mysql_free_result(m_MysqlRes); //释放结果资源

	if (m_IsConn)
		mysql_close(&m_MysqlCont); //断开连接

	DeleteCriticalSection(&_critical);
}

bool MysqlMgr::MysqlConn()
{
	if (m_IsConn)
	{
		printf("It has connected mysql！\n");
		return true;
	}

	bool bRes = false;
	if(mysql_real_connect(&m_MysqlCont, m_sHost.c_str(), m_sUser.c_str() ,m_sPswd.c_str(), m_sDB.c_str(), MysqlConnPort, NULL, CLIENT_MULTI_STATEMENTS)) //想要使用存储过程，最后一个参数必须要CLIENT_MULTI_STATEMENTS，而不能为0
	{
		printf("mysql connect succeed!\n");
		m_IsConn = true;
		int iRes = mysql_query(&m_MysqlCont, "SET NAMES GBK");  //设置编码格式,否则在cmd下无法显示中文
		if(!iRes)
		{
			bRes = true;
			printf("mysql encode set succeed！\n");
		}
		else
		{
			m_IsConn = false;
			mysql_close(&m_MysqlCont); //断开连接
			printf("mysql encode set failed, Disconnect the mysql！\n");
		}
	}
	else
	{
		cout<<"connect failed!"<<endl;
	}

	return bRes;
}

void MysqlMgr::InputMsgQueue(string strMysql, MysqlOper mysqlOper, Assist_ID AssistId, unsigned __int64 SocketId, string strRecord)
{
	if (strMysql.empty())
	{
		printf("数据库语句为空\n");
		return;
	}
	if (mysqlOper <= MYSQL_OPER_START || mysqlOper >= MYSQL_OPER_END)
	{
		printf("数据库操作类型错误[%d] error\n", mysqlOper);
		return;
	}
	if (AssistId <= ASSIST_ID_SPECIAL_START || (AssistId >= ASSIST_ID_SPECIAL_END && AssistId <= ASSIST_ID_START) || AssistId >= ASSIST_ID_END)
	{
		printf("Assistid类型错误[%d] error\n", AssistId);
		return;
	}

	//暂时不限制队列数量，如果要限制，只能把之前压入的先删除
	MysqlMsgData mysqlMsgData;
	mysqlMsgData.strMysql = strMysql;
	mysqlMsgData.mysqlOper = mysqlOper;
	mysqlMsgData.AssistId = AssistId;
	mysqlMsgData.SocketId = SocketId;
	mysqlMsgData.strRecord = strRecord;

	MysqlMsgLock uLock(_critical);
	//MysqlMsgLock::GetInstance()->Lock();
	m_MysqlMsgQueue.push(mysqlMsgData);
	//MysqlMsgLock::GetInstance()->Unlock();
}

void MysqlMgr::MsgQueueHandle()
{
	//每次处理一次
	if(!m_MysqlMsgQueue.empty())
	{
		MysqlMsgData recvMsgData = m_MysqlMsgQueue.front();

		int iRes=mysql_query(&m_MysqlCont, recvMsgData.strMysql.c_str());  //执行

		if (iRes)
			printf("Execute mysql failed : %s\n", recvMsgData.strMysql.c_str());

		if (recvMsgData.mysqlOper > MYSQL_OPER_START && recvMsgData.mysqlOper < MYSQL_OPER_END)
		{
			m_MysqlRes = NULL;
			if (MYSQL_OPER_SELECT == recvMsgData.mysqlOper)
			{
				m_MysqlRes=mysql_store_result(&m_MysqlCont);  //保存查询到的数据到m_MysqlRes，查询失败m_MysqlRes为NULL
			}

			string strRecord = StringTool::NumberToStr(iRes);
			if (!recvMsgData.strRecord.empty())
			{
				strRecord += "|" + recvMsgData.strRecord;
			}

			ProcMgr::GetInstance()->GetReplyHandleMoniter().DispatchEvent((Assist_ID)recvMsgData.AssistId, (SOCKET)recvMsgData.SocketId, m_MysqlRes, strRecord);
		}
		else
		{
			printf("mysql operator type error : [%d]\n", recvMsgData.mysqlOper);
		}


		MysqlMsgLock uLock(_critical);
		//MysqlMsgLock::GetInstance()->Lock();
		m_MysqlMsgQueue.pop();
		//MysqlMsgLock::GetInstance()->Unlock();
	}
}