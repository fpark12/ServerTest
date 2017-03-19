#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Public/Detail/SQLOperations/SQLStatement.h"
#include "Public/Detail/SQLOperations/SQLTransaction.h"

#include <sqlpp11/alias_provider.h>
#include <sqlpp11/functions.h>
#include <sqlpp11/insert.h>
#include <sqlpp11/multi_column.h>
#include <sqlpp11/mysql/connection.h>
#include <sqlpp11/remove.h>
#include <sqlpp11/select.h>
#include <sqlpp11/transaction.h>
#include <sqlpp11/update.h>

#include "SampleTable.h"

std::mutex m;
std::condition_variable cv;
std::string data;
bool ready = false;
bool processed = false;
MYSQL* MySqlHandle1;
MYSQL* MySqlHandle2;

void worker_thread()
{
	// Wait until main() sends data
	std::unique_lock<std::mutex> lk(m);
	cv.wait(lk, [] { return ready; });

	// after the wait, we own the lock.
	std::cout << "Worker thread is processing data\n";
	data += " after processing";

	// Send data back to main()
	processed = true;
	std::cout << "Worker thread signals data processing completed\n";

	// Manual unlocking is done before notifying, to avoid waking up
	// the waiting thread only to block again (see notify_one for details)
	lk.unlock();
	cv.notify_one();
}


namespace sql = sqlpp::mysql;
const auto tab = SampleTable{};
const auto t = TabBar{};
const auto Users = SQLTable::Users{};

SQLPP_ALIAS_PROVIDER(left);
SQLPP_ALIAS_PROVIDER(cheese);

void testSelectAll(sql::connection& db, int expectedRowCount)
{
	std::cerr << "--------------------------------------" << std::endl;
	int i = 0;
	for (const auto& row : db(sqlpp::select(all_of(tab)).from(tab).unconditionally()))
	{
		++i;
		std::cerr << ">>> row.alpha: " << row.alpha << ", row.beta: " << row.beta << ", row.gamma: " << row.gamma
			<< std::endl;
		assert(i == row.alpha);
	};
	assert(i == expectedRowCount);

	auto preparedSelectAll = db.prepare(sqlpp::select(all_of(tab)).from(tab).unconditionally());
	i = 0;
	for (const auto& row : db(preparedSelectAll))
	{
		++i;
		std::cerr << ">>> row.alpha: " << row.alpha << ", row.beta: " << row.beta << ", row.gamma: " << row.gamma
			<< std::endl;
		assert(i == row.alpha);
	};
	assert(i == expectedRowCount);
	std::cerr << "--------------------------------------" << std::endl;
}

int main()
{
	Status Result = Status::OK;
	// Initialize Database Connection
	SQLConnectionPoolInfo connPoolInfo;
	Result = GConfig.Load("DatabaseTest.ini");
	Result = GConfig.GetString("DatabaseTest.LoginDatabase.Hostname", connPoolInfo.Hostname);
	Result = GConfig.GetUInt("DatabaseTest.LoginDatabase.Port", connPoolInfo.Port);
	Result = GConfig.GetString("DatabaseTest.LoginDatabase.Username", connPoolInfo.Username);
	Result = GConfig.GetString("DatabaseTest.LoginDatabase.Password", connPoolInfo.Password);
	Result = GConfig.GetString("DatabaseTest.LoginDatabase.Schema", connPoolInfo.Schema);
	Result = GConfig.GetUInt("DatabaseTest.LoginDatabase.ConnectionCount", connPoolInfo.ConnectionCount);
	GDatabase.AddSchema(0, connPoolInfo);
	GDatabase.SpawnSQLConnections();
	SQLConnectionPool* connPool = GDatabase.GetSQLConnectionPool(0);


	// sqlpp11 test

	auto config = std::make_shared<sql::connection_config>();
	config->user     = "root";
	config->password = "password";
	config->database = "sqlpp_mysql";
	config->debug    = true;
	try
	{
		sql::connection db(config);
	}
	catch (const sqlpp::exception& e)
	{
		std::cerr << "For testing, you'll need to create a database sqlpp_mysql for user root (no password)" << std::endl;
		std::cerr << e.what() << std::endl;
		return 1;
	}

	try
	{
		sql::connection db(config);
		db.execute(R"(DROP TABLE IF EXISTS tab_sample)");
		db.execute(R"(CREATE TABLE tab_sample (
			alpha bigint(20) AUTO_INCREMENT,
			beta varchar(255) DEFAULT NULL,
			gamma bool DEFAULT NULL,
			PRIMARY KEY (alpha)
			))");
		db.execute(R"(DROP TABLE IF EXISTS tab_foo)");
		db.execute(R"(CREATE TABLE tab_foo (
			omega bigint(20) DEFAULT NULL
			))");

		db.execute(R"(DROP TABLE IF EXISTS Users)");
		db.execute(R"(CREATE TABLE Users (
			ID bigint AUTO_INCREMENT,
			AccountName varchar(255) NOT NULL,
			Password varchar(255) NOT NULL,
			EmailAddress varchar(255) NOT NULL,
			PRIMARY KEY (ID)
			))");
		
		/*
		auto s = select(all_of(t)).from(t).where((t.beta.like(parameter(t.beta)) and t.alpha == parameter(t.alpha)) or
			t.gamma != parameter(t.gamma));
		auto p = db.prepare(s);
		p.params.alpha = 7;
		p.params.alpha = sqlpp::tvin(0);
		*/

		// Unprepared insert
		auto s1 = insert_into(Users).set(
			Users.AccountName  = "fpark12",
			Users.Password     = "password",
			Users.EmailAddress = "fpark12@icloud.com");
		db.run(s1);

		// Prepared insert
		auto p1 = db.prepare(
			insert_into(Users).set(
				Users.AccountName  = parameter(Users.AccountName),
				Users.Password     = parameter(Users.Password),
				Users.EmailAddress = parameter(Users.EmailAddress)
			));

		struct UserInfo
		{
			std::string AccountName, Password, EmailAddress;
		};

		UserInfo input_values[] =
		{
			{"yellowtail", "loveSush1"  , "jellyfish3311@gmail.com"},
			{"tuna"      , "loveSash1m1", "paperfish@gmail.com"    },
			{"salmon"    , "deliciousme", "peppercorn11@gmail.com" },
		};

		for (const auto& input : input_values)
		{
			//prepared_insert.params.alpha = input.first;
			p1.params.AccountName  = input.AccountName;
			p1.params.Password     = input.Password;
			p1.params.EmailAddress = input.EmailAddress;
			db.run(p1);
		}

		testSelectAll(db, 0);
		db(insert_into(tab).default_values());
		testSelectAll(db, 1);
		db(insert_into(tab).set(tab.gamma = true, tab.beta = "cheesecake"));
		testSelectAll(db, 2);
		db(insert_into(tab).set(tab.gamma = true, tab.beta = "cheesecake"));
		testSelectAll(db, 3);

		// selecting two multicolumns
		for (const auto& row :
			db(select(multi_column(tab.alpha, tab.beta, tab.gamma).as(left), multi_column(all_of(tab)).as(tab))
				.from(tab)
				.unconditionally()))
		{
			std::cerr << "row.left.alpha: " << row.left.alpha << ", row.left.beta: " << row.left.beta
				<< ", row.left.gamma: " << row.left.gamma << std::endl;
			std::cerr << "row.tabSample.alpha: " << row.tabSample.alpha << ", row.tabSample.beta: " << row.tabSample.beta
				<< ", row.tabSample.gamma: " << row.tabSample.gamma << std::endl;
		};

		// test functions and operators
		db(select(all_of(tab)).from(tab).where(tab.alpha.is_null()));
		db(select(all_of(tab)).from(tab).where(tab.alpha.is_not_null()));
		db(select(all_of(tab)).from(tab).where(tab.alpha.in(1, 2, 3)));
		db(select(all_of(tab)).from(tab).where(tab.alpha.in(sqlpp::value_list(std::vector<int>{1, 2, 3, 4}))));
		db(select(all_of(tab)).from(tab).where(tab.alpha.not_in(1, 2, 3)));
		db(select(all_of(tab)).from(tab).where(tab.alpha.not_in(sqlpp::value_list(std::vector<int>{1, 2, 3, 4}))));
		db(select(count(tab.alpha)).from(tab).unconditionally());
		db(select(avg(tab.alpha)).from(tab).unconditionally());
		db(select(max(tab.alpha)).from(tab).unconditionally());
		db(select(min(tab.alpha)).from(tab).unconditionally());
		db(select(exists(select(tab.alpha).from(tab).where(tab.alpha > 7))).from(tab).unconditionally());
		db(select(all_of(tab)).from(tab).where(tab.alpha == any(select(tab.alpha).from(tab).where(tab.alpha < 3))));
		db(select(all_of(tab)).from(tab).where(tab.alpha == some(select(tab.alpha).from(tab).where(tab.alpha < 3))));

		db(select(all_of(tab)).from(tab).where(tab.alpha + tab.alpha > 3));
		db(select(all_of(tab)).from(tab).where((tab.beta + tab.beta) == ""));
		db(select(all_of(tab)).from(tab).where((tab.beta + tab.beta).like("%'\"%")));

		// insert
		db(insert_into(tab).set(tab.gamma = true));

		// update
		db(update(tab).set(tab.gamma = false).where(tab.alpha.in(1)));
		db(update(tab).set(tab.gamma = false).where(tab.alpha.in(sqlpp::value_list(std::vector<int>{1, 2, 3, 4}))));

		// remove
		db(remove_from(tab).where(tab.alpha == tab.alpha + 3));

		auto result = db(select(all_of(tab)).from(tab).unconditionally());
		std::cerr << "Accessing a field directly from the result (using the current row): " << result.begin()->alpha
			<< std::endl;
		std::cerr << "Can do that again, no problem: " << result.begin()->alpha << std::endl;

		auto tx = start_transaction(db);
		if (const auto& row =
			*db(select(all_of(tab), select(max(tab.alpha)).from(tab)).from(tab).unconditionally()).begin())
		{
			int a = row.alpha;
			int m = row.max;
			std::cerr << "-----------------------------" << a << ", " << m << std::endl;
		}
		tx.commit();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}








	// Prepared Statement


	MYSQL* initMysql1 = mysql_init(NULL);
	MYSQL* initMysql2 = mysql_init(NULL);
	if (!initMysql1)
	{
		// TODO Error Log: not enough memory to spawn a new mysql connection handle
		return 1;
	}

	MySqlHandle1 = mysql_real_connect(initMysql1, "127.0.0.1",
		"root", "password",	"test", 3306, NULL, 0);

	if (!MySqlHandle1)
	{
		return 1;
	}

	MySqlHandle2 = mysql_real_connect(initMysql2, "127.0.0.1",
		"root", "password", "test", 3306, NULL, 0);

	if (!MySqlHandle2)
	{
		return 1;
	}

	std::thread worker(worker_thread);

	int retValue = 0;
	retValue = mysql_query(MySqlHandle1, "START TRANSACTION");

	retValue = mysql_query(MySqlHandle1, "CREATE TABLE `test`.`new_table` (`idnew_table` INT NOT NULL,	PRIMARY KEY(`idnew_table`))");
	retValue = mysql_query(MySqlHandle1, "INSERT INTO `test`.`new_table` (`idnew_table`) VALUES('1')");
	retValue = mysql_query(MySqlHandle1, "INSERT INTO `test`.`new_table` (`idnew_table`) VALUES('2')");

	data = "Example data";
	// send data to the worker thread
	{
		std::lock_guard<std::mutex> lk(m);
		ready = true;
		std::cout << "main() signals data ready for processing\n";
	}
	cv.notify_one();

	// wait for the worker
	{
		std::unique_lock<std::mutex> lk(m);
		cv.wait(lk, [] { return processed; });
	}
	std::cout << "Back in main(), data = " << data << '\n';

	worker.join();
}