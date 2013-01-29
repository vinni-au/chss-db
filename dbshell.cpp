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
            for (int i = 0; i < s->get_size(); ++i) {
                if (i != 0)
                    std::cout << ",";
                DBDataType type = s->get_field_type(i);
                std::cout << s->get_name(i) << "(";
                std::cout << type.name();
                std::cout << ")";
            }
            std::cout << std::endl;
            while (reader->hasNextRecord()) {
                Record* current = reader->getNextRecord();
                for (int i = 0; i < s->get_size(); ++i) {
                    if (i != 0)
                        std::cout << ",";
                    switch(s->get_field_type(i).get_type()) {
                    case DBDataType::INT:
                        std::cout << current->getInt(i);
                        break;
                    case DBDataType::DOUBLE:
                        std::cout << current->getDouble(i);
                        break;
                    case DBDataType::VARCHAR:
                        std::cout << current->getVarchar(i);
                        break;
                    }
                }
                std::cout << std::endl;
            }
            delete reader;
        } else {
//            postError("DBShell", "It seems to be wrong query");
        }
    }
}
