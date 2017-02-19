#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Public/Detail/SQLOperations/SQLStatement.h"
#include "Public/Detail/SQLOperations/SQLTransaction.h"

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