#include "queryprocessor.hpp"
#include "../metadata.h"
#include "../messagedatareader.hpp"

QueryProcessor::QueryProcessor(DB *db)
    : m_db(db)
{
}

IDataReader* QueryProcessor::runQuery(Query *query)
{
    if (query == 0)
        return 0;

    if (query->type() == Query::Create) {
        CreateQuery* q = static_cast<CreateQuery*>(query);
        Table t(q->tablename());
        for (size_t i = 0; i < q->columns().size(); ++i) {
            std::pair<std::string, DBDataType> cur = q->columns()[i];
            t.add_column(Column(cur.second, cur.first));
        }
        m_db->metadata().add_table(t);
        return new MessageDataReader(std::string("OK"));
    } else if (query->type() == Query::Insert) {

    } else if (query->type() == Query::Select) {

    }
    return 0;
}
