#include "dbshell.hpp"
#include "query/parser.hpp"
#include <stdarg.h>

DBShell::DBShell(DB *db) :
    m_db(db)
{
}

void DBShell::run() {
    while (true) {
        std::string query;
        std::string cur;
        do {
            std::getline(std::cin, cur);
            query.append(cur.c_str());
        } while (cur.size());
        if (query == "exit")
            return;
        Parser parser(query);
        Query* q = parser.parse();
        IDataReader* reader = m_db->queryProcessor()->runQuery(q);
        if (reader) {
            while (reader->hasNext()) {

            }
        } else {
//            postError("DBShell", "It seems to be wrong query");
        }
    }
}
