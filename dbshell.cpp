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
            cur.push_back(' ');
            query.append(cur.c_str());
        } while (cur.size() > 1);
        if (query == "exit  ")
            return;
        Parser parser(query);
        Query* q = parser.parse();
        IDataReader* reader = m_db->queryProcessor()->runQuery(q);
        delete q;
        if (reader) {
            Signature* signature = reader->getSignature();
            while (reader->hasNextRecord()) {
                Record* record = reader->getNextRecord();
                for(uint32 i = 0; i < signature->get_size(); ++i) {
                    if(i > 0)
                        std::cout << ",";
                    switch(signature->get_field_type(i).get_type()) {
                        case 1:
                            std::cout << record->getInt(i);
                            break;
                        case 2:
                            std::cout << record->getDouble(i);
                            break;
                        case 0:
                            std::cout << "\"" << record->getVarchar(i) << "\"";
                            break;
                    }
                    std::cout << std::endl;
                }
                delete record;
            }
            delete reader;
        } else {
//            postError("DBShell", "It seems to be wrong query");
        }
    }
}
