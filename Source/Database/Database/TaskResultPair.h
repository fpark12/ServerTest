#pragma once

class PreparedStatement;
class ResultSet;

class QueryResultPair
{
public:
	QueryResultPair();
	~QueryResultPair();

private:
	PreparedStatement* Query;
	ResultSet* Result;
};