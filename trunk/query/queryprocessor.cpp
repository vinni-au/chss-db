#include "queryprocessor.hpp"
#include "../metadata.h"
#include "../messagedatareader.hpp"
#include "../buffer/heapfile.h"
#include "../buffer/indexfile.h"

struct FileIterator : IDataReader {
    FileIterator(IndexFile* file) : m_file(file), m_current_position(0), m_size(file->get_size()) {}

    bool hasNextRecord() {
        return m_current_position < m_size;
    }

    Signature* getSignature() {
        m_file->get_file_signature();
    }

    Record* getNextRecord() {
        return m_file->get(m_current_position++);
    }
private:
    IndexFile* m_file;
    uint32 m_current_position;
    uint32 m_size;
};

QueryProcessor::QueryProcessor(DB *db)
    : m_db(db)
{
}

IDataReader* QueryProcessor::runQuery(Query *query) {
    if (query == 0) {
        return 0;
    }
    DBMetaData* meta = m_db->metadata();
    if (query->type() == Query::Create) {
        std::cout << "Create Table" << std::endl;
        CreateTableQuery* q = static_cast<CreateTableQuery*>(query);
        if(meta->exist_table(q->tablename())) {
            return new MessageDataReader("Table is already exist");
        }
        uint32 tables_count = meta->get_tables_count();
        Table* t = new Table();
        t->set_tablename(q->tablename());
        for (size_t i = 0; i < q->columns().size(); ++i) {
            std::pair<std::string, DBDataType> cur = q->columns()[i];
            t->add_column(Column(cur.second, cur.first));
        }
        IndexFile* file = new IndexFile(m_db->buffer(), tables_count, t->makeSignature());
        t->set_file(file);
        meta->add_table(t);
        file->create();
        return new MessageDataReader(std::string("OK"));
    } else if(query->type() == Query::CreateIndex) {
        std::cout << "CreateIndex" << std::endl;
        CreateIndexQuery* q = static_cast<CreateIndexQuery*>(query);
        std::string tablename = q->tablename();
        if(!meta->exist_table(tablename)) {
            return new MessageDataReader("Table doesn't exist");
        }
        uint32 index = meta->get_table_index(tablename);
        Table* t = meta->get_table(index);
        Signature* signature = t->makeSignature();
        signature->get_index(q->indexname());
        IndexFile indexfile(m_db->buffer(), index, t->makeSignature());
        //indexfile.findKey(0);
        indexfile.createIndex(0, 1);
    } else if (query->type() == Query::Insert) {
        std::cout << "Insert" << std::endl;
        InsertQuery* q = static_cast<InsertQuery*>(query);
        std::string tablename = q->tablename();
        if(!meta->exist_table(tablename)) {
                    std::cout << "Error" << std::endl;
            return new MessageDataReader("Table doesn't exist");
        }
        uint32 index = meta->get_table_index(tablename);
        Table* t = meta->get_table(index);
        Signature* signature = t->makeSignature();
        Record record(signature);
        for (size_t i = 0; i < q->values().size(); ++i) {
            std::pair<std::string, DBDataValue> cur = q->values()[i];
            uint32 index = signature->get_index(cur.first);
            switch(signature->get_field_type(index).get_type()) {
                case 1:
                    record.setInt(index, cur.second.intValue());
                    break;
                case 2:
                    record.setDouble(index, cur.second.doubleValue());
                    break;
                case 0:
                    record.setVarchar(index, cur.second.stringValue());
                    break;
            }
        }
        if(t->get_file() == 0) {
            std::cout << "BM" << m_db->buffer() << std::endl;
            t->set_file(new IndexFile(m_db->buffer(), index, t->makeSignature()));
        }
        IndexFile* file = t->get_file();
        file->add(&record);
        delete signature;
    } else if (query->type() == Query::Select) {
        SelectQuery* q = static_cast<SelectQuery*>(query);
        std::string tablename = q->tablename();
        if(!meta->exist_table(tablename)) {
            return new MessageDataReader("Table doesn't exist");
        }
        uint32 index = meta->get_table_index(tablename);
        Table* t = meta->get_table(index);
        Signature* signature = t->makeSignature();
        if(t->get_file() == 0) {
            t->set_file(new IndexFile(m_db->buffer(), index, t->makeSignature()));
        }
//        if(q->condition()) {
            IndexFile indexfile(m_db->buffer(), index, t->makeSignature());
            std::pair< std::string, DBDataValue > cond = q->condition();
            uint32 column = signature->get_index(cond.first);
//            return indexfile.select(column, cond.second, 1);
            return new FileIterator(&indexfile);
//        }
    }
    return 0;
}
