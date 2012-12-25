#include "queryprocessor.hpp"
#include "../metadata.h"
#include "../messagedatareader.hpp"
#include "../buffer/heapfile.h"

QueryProcessor::QueryProcessor(DB *db)
    : m_db(db)
{
}

IDataReader* QueryProcessor::runQuery(Query *query)
{
    if (query == 0)
        return 0;

    if (query->type() == Query::Create) {
        CreateTableQuery* q = static_cast<CreateTableQuery*>(query);
        Table t(q->tablename());
        for (size_t i = 0; i < q->columns().size(); ++i) {
            std::pair<std::string, DBDataType> cur = q->columns()[i];
            t.add_column(Column(cur.second, cur.first));
        }
        HeapFile* file = new HeapFile(m_db->buffer(), 0, t.makeSignature());
        t.set_file(file);
        m_db->metadata().add_table(t);
        file->create();
        return new MessageDataReader(std::string("OK"));
    } else if (query->type() == Query::Insert) {
        //m_db->metadata.get_table()
    } else if (query->type() == Query::Select) {

    }
    return 0;
}
