#ifndef QUERYPROCESSOR_HPP
#define QUERYPROCESSOR_HPP

#include "../global.h"
#include "../db.hpp"
#include "query.hpp"

struct QueryProcessor
{
    QueryProcessor(DB* db);

    IDataReader* runQuery(Query* query);

private:
    DB* m_db;
};

#endif // QUERYPROCESSOR_HPP
