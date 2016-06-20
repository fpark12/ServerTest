/*
* Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

// #include "DatabaseEnv.h"
#include "DatabaseWorker.h"
#include "SQLOperation.h"
#include "MySQLConnection.h"
// #include "MySQLThreading.h"
#include "ProducerConsumerQueue.h"

DatabaseWorker::DatabaseWorker(ProducerConsumerQueue<SQLOperation*>* newQueue, MySQLConnection* connection)
{
	conn = connection;
	queue = newQueue;
	cancelationToken = false;
	_workerThread = std::thread(&DatabaseWorker::WorkerThread, this);
}

DatabaseWorker::~DatabaseWorker()
{
	cancelationToken = true;

	queue->Cancel();

	_workerThread.join();
}

void DatabaseWorker::WorkerThread()
{
	if (!queue)
		return;

	for (;;)
	{
		SQLOperation* operation = nullptr;

		queue->WaitAndPop(operation);

		if (cancelationToken || !operation)
			return;

		operation->SetConnection(conn);
		operation->call();

		delete operation;
	}
}