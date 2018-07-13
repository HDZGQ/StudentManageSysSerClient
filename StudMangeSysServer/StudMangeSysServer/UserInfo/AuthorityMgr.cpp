#include "AuthorityMgr.h"

AuthorityMgr::AuthorityMgr()
{

}

AuthorityMgr::~AuthorityMgr()
{

}


void AuthorityMgr::GetDefaultAuthorityByIdent(IdentType Ident, vector<OperPermission>& vecOper)
{
	vecOper.clear();
	if (IDENT_TYPE_STUDENT == Ident)
	{
		vecOper.push_back(OPER_PER_LOGIN);
		vecOper.push_back(OPER_PER_REGISTER);
	}
	else if (IDENT_TYPE_TEACHER == Ident)
	{
		vecOper.push_back(OPER_PER_LOGIN);
		vecOper.push_back(OPER_PER_REGISTER);
	}
	else if (IDENT_TYPE_ADMIN == Ident)
	{
		vecOper.push_back(OPER_PER_LOGIN);
		vecOper.push_back(OPER_PER_REGISTER);
	}
	else
	{
		printf("%s  身份标识错误Ident[%d]\n", __FUNCTION__, Ident);
	}
}
