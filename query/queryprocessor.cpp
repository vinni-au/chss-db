#include "queryprocessor.hpp"
#include "../metadata.h"
#include "../messagedatareader.hpp"
#include "../buffer/index.h"
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
//        std::cerr << "Create Table" << std::endl;
        CreateTableQuery* q = static_cast<CreateTableQuery*>(query);
        if(meta->exist_table(q->tablename())) {
            return new MessageDataReader("ERROR: Table is already exist");
        }
        uint32 tables_count = meta->get_tables_count();
        Table* t = new Table();
        t->set_tablename(q->tablename());
        for (size_t i = 0; i < q->columns().size(); ++i) {
            std::pair<std::string, DBDataType> cur = q->columns()[i];
            t->add_column(Column(cur.second, cur.first));
        }
        //FIXME: а куда потом девается file? ТЕЧЁТ БЛЯТЬ!!!!
        IndexFile* file = new IndexFile(m_db->buffer(), tables_count, t->makeSignature());
        t->set_file(file);
        meta->add_table(t);
        file->create();
        return new MessageDataReader(std::string("OK"));
    } else if (query->type() == Query::Select) {
//        std::cout << "Select" << std::endl;
        SelectQuery* q = static_cast<SelectQuery*>(query);
        std::string tablename = q->tablename();
        if(!meta->exist_table(tablename)) {
            return new MessageDataReader("ERROR: Table doesn't exist");
        }
        uint32 index = meta->get_table_index(tablename);
        Table* t = meta->get_table(index);
        Signature* signature = t->makeSignature();
        if(t->get_file() == 0) {
            t->set_file(new IndexFile(m_db->buffer(), index, t->makeSignature()));
        }
        if(!q->condition().first.empty()) {
            std::pair< std::string, DBDataValue> cond = q->condition();
            uint32 column = signature->get_index(cond.first);
            if(column == -1) {
                return new MessageDataReader("ERROR: Field doesn't exist");
            }
            cond.second.type() = signature->get_field_type(column);
            return t->get_file()->select(column, cond.second);
        } else {
            return new FileIterator(t->get_file());
        }
    } else if(query->type() == Query::CreateIndex) {
//        std::cout << "CreateIndex" << std::endl;
        CreateIndexQuery* q = static_cast<CreateIndexQuery*>(query);
        std::string tablename = q->tablename();
        if(!meta->exist_table(tablename)) {
            return new MessageDataReader("ERROR: Table doesn't exist");
        }
        uint32 index = meta->get_table_index(tablename);
        Table* t = meta->get_table(index);
        if(t->get_file() == 0) {
            t->set_file(new IndexFile(m_db->buffer(), index, t->makeSignature()));
        }
        Signature* signature = t->makeSignature();
        uint32 column = signature->get_index(q->cols().at(0).first);
        if(column == -1) {
            return new MessageDataReader("ERROR: Field doesn't exist");
        }
        t->get_column(column).indextype = (IndexType)q->indextype();
        t->get_column(column).unique_index = q->is_unique();
        t->get_file()->createIndex(column, (IndexType)q->indextype());
        return new MessageDataReader("OK");
    } else if (query->type() == Query::Insert) {
        InsertQuery* q = static_cast<InsertQuery*>(query);
        std::string tablename = q->tablename();
        if(!meta->exist_table(tablename)) {
            return new MessageDataReader("ERROR: Table doesn't exist");
        }
        uint32 index = meta->get_table_index(tablename);
        Table* t = meta->get_table(index);
        Signature* signature = t->makeSignature();
        Record record(-1, signature);
        for (size_t i = 0; i < q->values().size(); ++i) {
            std::pair<std::string, DBDataValue> cur = q->values()[i];
            uint32 index = signature->get_index(cur.first);
            if(index == -1) {
                return new MessageDataReader("ERROR: Field doesn't exist");
            }
            if(cur.second.type().get_type() != signature->get_field_type(index).get_type()) {
                return new MessageDataReader("ERROR: Data type error");
            }
            record.set(index, cur.second);
        }
        if(t->get_file() == 0) {
            t->set_file(new IndexFile(m_db->buffer(), index, t->makeSignature()));
        }
        IndexFile* file = t->get_file();
        file->add(&record);
        delete signature;
        return new MessageDataReader("OK");
    } else if (query->type() == Query::Delete) {
        DeleteQuery* q = static_cast<DeleteQuery*>(query);
        std::string tablename = q->tablename();
        if(!meta->exist_table(tablename)) {
            return new MessageDataReader("Error: Table doesn't exist");
        }
        uint32 index = meta->get_table_index(tablename);
        Table* t = meta->get_table(index);
        Signature* signature = t->makeSignature();
        if(t->get_file() == 0) {
            t->set_file(new IndexFile(m_db->buffer(), index, t->makeSignature()));
        }
        uint32 cnt = 0;
        std::pair< std::string, DBDataValue> cond = q->condition();
        while(true) {
            IDataReader* data;
            if(!cond.first.empty()) {
                uint32 column = signature->get_index(cond.first);
                data = t->get_file()->select(column, cond.second);
            } else {
                data = new FileIterator(t->get_file());
            }
            if(!data->hasNextRecord()) {
                delete data;
                break;
            }
            Record* record = data->getNextRecord();
            uint32 position = record->getPosition();
            for(uint32 i = 0; i < signature->get_size(); ++i) {
                t->get_file()->remove(i, position, record->get(i));
            }
            delete record;

            uint32 filesize = t->get_file()->get_size();
            Record* tail = t->get_file()->get(filesize - 1);
            t->get_file()->set(position, tail);
            for(uint32 i = 0; i < signature->get_size(); ++i) {
                t->get_file()->update_position(i, filesize - 1, position, tail->get(i));
            }
            delete tail;

            delete data;
            t->get_file()->pop_back();
            ++cnt;
        }
        delete signature;
        return new MessageDataReader("OK Affected rows: " + int_to_string(cnt));
    } else if (query->type() == Query::Update) {
//        std::cout << "Update" << std::endl;
        UpdateQuery* q = static_cast<UpdateQuery*>(query);
        std::string tablename = q->tablename();
        if(!meta->exist_table(tablename)) {
            return new MessageDataReader("ERROR: Table doesn't exist");
        }
        uint32 index = meta->get_table_index(tablename);
        Table* t = meta->get_table(index);
        Signature* signature = t->makeSignature();
        if(t->get_file() == 0) {
            t->set_file(new IndexFile(m_db->buffer(), index, t->makeSignature()));
        }
        uint32 cnt = 0;
        IDataReader* data = 0;
        while(true) {
            uint32 column = -1;
            if(!data) {
                if(!q->condition().first.empty()) {
                    std::pair< std::string, DBDataValue> cond = q->condition();
                    column = signature->get_index(cond.first);
                    if(column == -1) {
                        return new MessageDataReader("ERROR: Field doesn't exist");
                    }
                    data = t->get_file()->select(column, cond.second);
                } else {
                    data = new FileIterator(t->get_file());
                }
            }
            if(!data->hasNextRecord()) {
                delete data;
                break;
            }
            Record* record = data->getNextRecord();
            uint32 position = record->getPosition();
            for(uint32 i = 0; i < q->values().size(); ++i) {
                uint32 curcolumn = signature->get_index(q->values()[i].first);
                if(curcolumn == -1) {
                    return new MessageDataReader("ERROR: Field doesn't exist");
                }
                t->get_file()->update(curcolumn, position, record->get(curcolumn), q->values()[i].second);
                record->set(curcolumn, q->values()[i].second);
                if(curcolumn == column) {
                    delete data;
                    data = 0;
                }
            }
            t->get_file()->set(position, record);
            delete record;
            ++cnt;
        }
        delete signature;
        return new MessageDataReader("OK Affected rows: " + int_to_string(cnt));
    }
    return 0;
}
