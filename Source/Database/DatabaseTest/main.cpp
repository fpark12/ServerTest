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
	MYSQL* mysql = mysql_init(NULL);

	MYSQL_STMT* stmt = mysql_stmt_init(mysql);
	if (!stmt)
	{
		exit(EXIT_FAILURE);
	}

	if (!mysql_real_connect(mysql, "localhost", "root", "Keathalin21", "testserver", 0, NULL, 0))
	{
		fprintf(stderr, "No connection could be made to the database\n");
		exit(EXIT_FAILURE);
	}

	char* sql = "SELECT `sex`, `name` FROM `user` WHERE `id` = ?";

	if (mysql_stmt_prepare(stmt, sql, strlen(sql)))
	{
		exit(EXIT_FAILURE);
	}
	// error checking
	DatabaseOperation Op;
	Op.SetStatement(stmt);
	Op.SetParamInt32(0, 1);
	Op.BindParam();

	MYSQL_BIND result[2];
	char result_data1[128];
	char result_data2[128];
	unsigned long data_length1;
	unsigned long data_length2;

	memset(result, 0, sizeof(result) * 2);
	result[0].buffer_type = MYSQL_TYPE_LONG;
	result[0].buffer = result_data1;
	result[0].buffer_length = 32;
	result[0].is_null = 0;
	result[0].length = &data_length1;
	result[0].is_unsigned = false;

	result[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	result[1].buffer = result_data2;
	result[1].buffer_length = 128;
	result[1].is_null = 0;
	result[1].length = &data_length2;

	if (mysql_stmt_execute(stmt))
	{
		fprintf(stderr, " mysql_stmt_execute(), failed\n");
		fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
		exit(EXIT_FAILURE);
	}

	int a = mysql_stmt_store_result(stmt);

	MYSQL_RES* res = mysql_stmt_result_metadata(stmt);
	MYSQL_FIELD* field = mysql_fetch_fields(res);
	uint64 row = (uint64)mysql_stmt_num_rows(stmt);
	uint32 fields = (uint32)mysql_stmt_field_count(stmt);

	/*
	MYSQL_ROWS* Rows = stmt->result.data;
	MYSQL_ROW Row = Rows->data;
	unsigned int num_fields = mysql_stmt_field_count(stmt);
	unsigned int i;

	unsigned long *lengths;
	Row = mysql_fetch_row(res);
	lengths = mysql_fetch_lengths(res);
	for (i = 0; i < num_fields; i++)
	{
		printf("[%.*s] ", (int)lengths[i],
			Row[i] ? Row[i] : "NULL");
	}
	printf("\n");
	*/

	if (mysql_stmt_bind_result(stmt, result) != 0)
	{
		fprintf(stderr, " mysql_stmt_bind_result() failed\n");
		fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
		exit(EXIT_FAILURE);
	}

	/*
	if (mysql_stmt_fetch(stmt) == 0)
	{
		printf("%s %s\n", result_data1, result_data2);
	}
	else
	{
		printf("No results found!\n");
	}
	*/

	if (mysql_stmt_close(stmt))
	{
		fprintf(stderr, " failed while closing the statement\n");
		fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
		exit(EXIT_FAILURE);
	}

}