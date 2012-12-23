#include "queryprocessor.hpp"
#include "../metadata.h"

QueryProcessor::QueryProcessor(DB *db)
    : m_db(db)
{
}

IDataReader* QueryProcessor::runQuery(Query *query)
{
    if (query->type() == Q_Create) {
        CreateQuery* q = static_cast<CreateQuery*>(query);
        Table t(q->tablename());
        for (int i = 0; i < q->columns().size(); ++i) {
            std::pair<std::string, int> cur = q->columns()[i];
            t.add_column(Column(cur.second, cur.first));
        }
    } else if (query->type() == Q_Select) {

    }
    return 0;
}
