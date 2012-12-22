#include "queryprocessor.hpp"

QueryProcessor::QueryProcessor(DB *db)
    : m_db(db)
{
}

IDataReader* QueryProcessor::runQuery(Query *query)
{
    return 0;
}
