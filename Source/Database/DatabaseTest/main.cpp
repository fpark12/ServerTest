/*

int main()
{
	bool param1;
	double param2;
	int field1;
	double field2;
	DatabaseQuery query;
// 	preparedBind.bind(0, param1);
// 	preparedBind.bind(1, param2);
	/ *query.initInput(param1, param2);* /
	//query.initOutput(field1, field2);
// 	resultBind.bind(0, field1);
// 	resultBind.bind(1, field2);
	query.setStmt(stmt);
	query.setConnection(connection);
/ *	query.run();* /
	query.getResult();
	query.getResultAsync();


	DatabaseQuery query1(stmt);
	query1.SetParam(0, param1);
	query1.SetParam(1, param1);
	query1.setConnection(connection);
	ErrorCodeOrSuccess = query1.Query(DatabaseQueryResult& result);
	DatabaseQueryResult = query1.Execute();

	PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_CREATE_INFO);
	stmt->setUInt32(0, GetAccountId());
	stmt->setUInt32(1, (skipCinematics == 1 || createInfo->Class == CLASS_DEATH_KNIGHT) ? 10 : 1);
	_charCreateCallback.SetFutureResult(CharacterDatabase.AsyncQuery(stmt));
	_charCreateCallback.NextStage();




	// init mysql_stmt
	stmt = mysql_stmt_init(mysql);
	mysql_stmt_prepare(stmt, sql, strlen(sql));
	init mysql_bind;
	mysql_stmt_bind_param(msql_STMT, msql_BIND);
	mysql_stmt_execute(msql_STMT);
	m_mStmt->ClearParameters();
	*pResult = mysql_stmt_result_metadata(msql_STMT);
	*pRowCount = mysql_stmt_num_rows(msql_STMT);
	*pFieldCount = mysql_stmt_field_count(msql_STMT);

	"Select * from user where name = Frank"
	"Select * from user where name = ?"

	void Battlenet::Session::AccountInfo::LoadResult(DatabaseOperation dbOp)
	{
		ResultSet& = dbOp.Result;
		Id = ResultSet.GetField(0);
		// ba.id, ba.email, ba.locked, ba.lock_country, ba.last_ip, ba.failed_logins, bab.unbandate > UNIX_TIMESTAMP() OR bab.unbandate = bab.bandate, bab.unbandate = bab.bandate FROM battlenet_accounts ba LEFT JOIN battlenet_account_bans bab WHERE email = ?
		Field* fields = result->Fetch();
		Id = fields[0].GetUInt32();
		Login = fields[1].GetString();
		IsLockedToIP = fields[2].GetBool();
		LockCountry = fields[3].GetString();
		LastIP = fields[4].GetString();
		FailedLogins = fields[5].GetUInt32();
		IsBanned = fields[6].GetUInt64() != 0;
		IsPermanenetlyBanned = fields[7].GetUInt64() != 0;

}

	pipeline.setparam()
	pipeline.getfield()

		union PreparedStatementDataUnion
		{
		bool boolean;
		uint8 ui8;
		int8 i8;
		uint16 ui16;
		int16 i16;
		uint32 ui32;
		int32 i32;
		uint64 ui64;
		int64 i64;
		float f;
		double d;
		const char* s;
		};*/
#include "DatabaseOperation.h"
int main()
{
	DatabaseOperation Op;

}