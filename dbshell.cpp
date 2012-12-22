#include "dbshell.hpp"
#include <stdarg.h>

DBShell::DBShell(DB *db) :
    m_db(db)
{
}

void DBShell::run()
{
    while (true) {
        std::string query;
        std::string cur;
        do {
            std::getline(std::cin, cur);
            query.append(cur.c_str());
        } while (cur.size());
        Query* q = Query::parse(query);
        IDataReader* reader = m_db->queryProcessor()->runQuery(q);
        if (reader) {
            while (reader->hasNext()) {

            }
        } else {
//            postError("DBShell", "It seems to be wrong query");
        }
    }
}
