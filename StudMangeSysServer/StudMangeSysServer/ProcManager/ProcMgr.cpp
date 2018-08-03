#include "ProcMgr.h"

ProcMgr::ProcMgr()
{
}

ProcMgr::~ProcMgr()
{

}

bool ProcMgr::InitMoniter()
{
	/*
	* 接收消息处理监控器
	*/
	m_RecvHandleMoniter.MonitorEvent(ASSIST_ID_LOGIN_REQ, &m_EnterSysProc, &EnterSysProc::LoginRecvHandle);
	m_RecvHandleMoniter.MonitorEvent(ASSIST_ID_REGISTER_REQ, &m_EnterSysProc, &EnterSysProc::RegisterRecvHandle);
	//m_RecvHandleMoniter.MonitorEvent(ASSIST_ID_EXIT_SYS_REQ, &m_EnterSysProc, &EnterSysProc::ExitSysRecvHandle);
	m_RecvHandleMoniter.MonitorEvent(ASSIST_ID_GET_SUBJECTS_REQ, &m_ScoreProc, &ScoreProc::CheckCanAlterSubjectRecvHandle);
	m_RecvHandleMoniter.MonitorEvent(ASSIST_ID_ALTER_SUBJECTS_REQ, &m_ScoreProc, &ScoreProc::AlterSubjectRecvHandle);
	m_RecvHandleMoniter.MonitorEvent(ASSIST_ID_ADD_SINGLE_SCORE_REQ, &m_ScoreProc, &ScoreProc::AddSingleScoreRecvHandle);
	m_RecvHandleMoniter.MonitorEvent(ASSIST_ID_ADD_BATCH_SCORE_REQ, &m_ScoreProc, &ScoreProc::AddBatchScoreRecvHandle);
	m_RecvHandleMoniter.MonitorEvent(ASSIST_ID_SELECT_SINGLE_SCORE_REQ, &m_ScoreProc, &ScoreProc::SelectSingleScoreRecvHandle);
	m_RecvHandleMoniter.MonitorEvent(ASSIST_ID_SELECT_BATCH_SCORE_REQ, &m_ScoreProc, &ScoreProc::SelectBatchScoreRecvHandle);
	m_RecvHandleMoniter.MonitorEvent(ASSIST_ID_UPDATE_SINGLE_SCORE_REQ, &m_ScoreProc, &ScoreProc::UpdateSingleScoreRecvHandle);
	m_RecvHandleMoniter.MonitorEvent(ASSIST_ID_DELETE_SCORE_REQ, &m_ScoreProc, &ScoreProc::DeleteScoreRecvHandle);
	m_RecvHandleMoniter.MonitorEvent(ASSIST_ID_GET_AUTHORITY_REQ, &m_AuthorityProc, &AuthorityProc::GetAuthoritRecvHandle);
	m_RecvHandleMoniter.MonitorEvent(ASSIST_ID_EDIT_AUTHORITY_REQ, &m_AuthorityProc, &AuthorityProc::EditAuthoritRecvHandle);
	m_RecvHandleMoniter.MonitorEvent(ASSIST_ID_ADD_SINGLE_USERINFO_REQ, &m_UserInfoProc, &UserInfoProc::AddSingleUserInfoRecvHandle);
	m_RecvHandleMoniter.MonitorEvent(ASSIST_ID_ADD_BATCH_USERINFO_REQ, &m_UserInfoProc, &UserInfoProc::AddBatchUserInfoRecvHandle);
	m_RecvHandleMoniter.MonitorEvent(ASSIST_ID_SELECT_SINGLE_USERINFO_REQ, &m_UserInfoProc, &UserInfoProc::SelectSingleUserInfoRecvHandle);
	m_RecvHandleMoniter.MonitorEvent(ASSIST_ID_UPDATE_SINGLE_USERINFO_REQ, &m_UserInfoProc, &UserInfoProc::UpdateSingleUserInfoRecvHandle);


	/*
	* 回复消息处理监控器
	*/
	m_ReplyHandleMoniter.MonitorEvent(ASSIST_ID_SPECIAL_GET_EXIAT_SUBJECTS, SubjectsMgr::GetInstance(), &SubjectsMgr::GetExistSubjectReplyHandle);
	m_ReplyHandleMoniter.MonitorEvent(ASSIST_ID_LOGIN_ACK, &m_EnterSysProc, &EnterSysProc::LoginReplyHandle);
	m_ReplyHandleMoniter.MonitorEvent(ASSIST_ID_REGISTER_ACK, &m_EnterSysProc, &EnterSysProc::RegisterReplyHandle);
	m_ReplyHandleMoniter.MonitorEvent(ASSIST_ID_ALTER_SUBJECTS_ACK, &m_ScoreProc, &ScoreProc::AlterSubjectReplyHandle);
	m_ReplyHandleMoniter.MonitorEvent(ASSIST_ID_ADD_SINGLE_SCORE_ACK, &m_ScoreProc, &ScoreProc::AddSingleScoreReplyHandle);
	m_ReplyHandleMoniter.MonitorEvent(ASSIST_ID_ADD_BATCH_SCORE_ACK, &m_ScoreProc, &ScoreProc::AddBatchScoreReplyHandle);
	m_ReplyHandleMoniter.MonitorEvent(ASSIST_ID_SELECT_SINGLE_SCORE_ACK, &m_ScoreProc, &ScoreProc::SelectSingleScoreReplyHandle);
	m_ReplyHandleMoniter.MonitorEvent(ASSIST_ID_SELECT_BATCH_SCORE_ACK, &m_ScoreProc, &ScoreProc::SelectBatchScoreReplyHandle);
	m_ReplyHandleMoniter.MonitorEvent(ASSIST_ID_UPDATE_SINGLE_SCORE_ACK, &m_ScoreProc, &ScoreProc::UpdateSingleScoreReplyHandle);
	m_ReplyHandleMoniter.MonitorEvent(ASSIST_ID_DELETE_SCORE_ACK, &m_ScoreProc, &ScoreProc::DeleteScoreReplyHandle);
	m_ReplyHandleMoniter.MonitorEvent(ASSIST_ID_GET_AUTHORITY_ACK, &m_AuthorityProc, &AuthorityProc::GetAuthoritReplyHandle);
	m_ReplyHandleMoniter.MonitorEvent(ASSIST_ID_EDIT_AUTHORITY_ACK, &m_AuthorityProc, &AuthorityProc::EditAuthoritReplyHandle);
	m_ReplyHandleMoniter.MonitorEvent(ASSIST_ID_ADD_SINGLE_USERINFO_ACK, &m_UserInfoProc, &UserInfoProc::AddSingleUserInfoReplyHandle);
	m_ReplyHandleMoniter.MonitorEvent(ASSIST_ID_ADD_BATCH_USERINFO_ACK, &m_UserInfoProc, &UserInfoProc::AddBatchUserInfoReplyHandle);
	m_ReplyHandleMoniter.MonitorEvent(ASSIST_ID_SELECT_SINGLE_USERINFO_ACK, &m_UserInfoProc, &UserInfoProc::SelectSingleUserInfoReplyHandle);
	m_ReplyHandleMoniter.MonitorEvent(ASSIST_ID_UPDATE_SINGLE_USERINFO_ACK, &m_UserInfoProc, &UserInfoProc::UpdateSingleUserInfoReplyHandle);

	return true;
}

CEventProxy<Assist_ID, SOCKET, void*, unsigned int>& ProcMgr::GetRecvHandleMoniter()
{
	return m_RecvHandleMoniter;
}

CEventProxy<Assist_ID, SOCKET, MYSQL_RES*, string>& ProcMgr::GetReplyHandleMoniter()
{
	return m_ReplyHandleMoniter;
}