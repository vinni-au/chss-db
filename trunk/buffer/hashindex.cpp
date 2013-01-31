#include "hashindex.h"

HashIndex::HashIndex(IndexFile* file, BufferManager* bm, Signature* signature, uint32 table_id, uint32 column) : Index(file, bm, signature, table_id, column), table_size(0) {}

HashIndexIterator::HashIndexIterator(Index *index, DBDataValue key): IndexIterator(index, key) {}

void HashIndex::createIndex() {
    int32 offset = 0;
    uint32 t = 0;
    m_bm->write(m_index_filename, 0, (char*)&t, sizeof(uint32));
    for(int i=0;i<BUCKETS_CNT;++i) {
        m_bm->write(m_index_filename, PAGESIZE*(i+1), (char*)&t, sizeof(uint32));
        m_bm->write(m_index_filename, PAGESIZE*(i+2)-sizeof(uint32), (char*)&t, sizeof(uint32));
    }
}

static int32 get_hash(int n) {
    return n;
}

static int32 get_hash(std::string const &s) {
    int p = 31;
    int hash = 0;
    for(int i=0;i<s.length();++i) {
        hash = hash*p + s[i]+1;
    }
    return hash;
}

static int32 get_hash(double d) {
    return *((int32*)&d);
}

static int32 get_hash(DBDataValue const &key, int mod) {
    int32 hash = 0;
    if (key.type().get_type()==DBDataType::INT) {
//        hash = get_hash(key.stringValue());
    } else if (key.type().get_type()==DBDataType::VARCHAR) {
//        hash = get_hash(key.intValue());
    } else if (key.type().get_type()==DBDataType::DOUBLE) {
//        hash = get_hash(key.doubleValue());
    }
    hash%=mod;
    hash+=mod;
    hash%=mod;
    return hash;
}

static int get_bufsize(DBDataValue const &key, char *buffer) {
    int bufsize = 0;
    if (key.type().get_type()==DBDataType::INT) {
        bufsize = sizeof(int);
        *((int*)buffer) = key.intValue();
    } else if (key.type().get_type()==DBDataType::DOUBLE) {
        bufsize = sizeof(double);
        *((double*)buffer) = key.doubleValue();
    } else {
        bufsize = key.type().m_len;
        memcpy(buffer, key.stringValue().c_str(), key.type().m_len);
    }
    return bufsize;
}

static bool compare(DBDataValue const &value, char *data) {
    if (value.type().get_type() == DBDataType::INT) {
        return value.intValue() == *((int*)data);
    } else if (value.type().get_type() == DBDataType::DOUBLE) {
        return value.doubleValue() == *((double*)data);
    } else {
        return memcmp(data, value.stringValue().c_str(), value.type().m_len) == 0;
    }
}

void HashIndex::addKey(DBDataValue key, uint32 value) {
    int hash = get_hash(key, BUCKETS_CNT);
    char *buff = new char[300];
    int key_size = get_bufsize(key, buff);
    int record_size = key_size+sizeof(uint32);
    int records_per_page = (PAGESIZE-sizeof(uint32))/record_size;
    int page = 0;
    m_bm->read(m_index_filename, PAGESIZE*(hash+1), buff, sizeof(uint32));
    int last = *((int*)buff);
    int last_page = last/PAGESIZE;
    int bufsize = get_bufsize(key, buff);
    if ((last+record_size+sizeof(uint32))%PAGESIZE==0) {
        m_bm->write(m_index_filename, last+record_size, buff, key_size);
        m_bm->write(m_index_filename, last+record_size+key_size, (char*)&value, sizeof(uint32));
        uint32 t = last+record_size+sizeof(value);
        m_bm->write(m_index_filename, PAGESIZE*(hash+1), (char*)&t, sizeof(uint32));
    } else {
        m_bm->read(m_index_filename, 0, buff, sizeof(uint32));
        int tt = *((uint32*)buff);
        ++tt;
        m_bm->write(m_index_filename, 0, (char*)&tt, sizeof(uint32));
        uint32 new_page = *((uint32*)buff)+BUCKETS_CNT+1;
        tt = *((uint32*)buff)+1;
        m_bm->write(m_index_filename, (last_page+1)*PAGESIZE-sizeof(uint32), (char*)&tt, sizeof(uint32));
        uint32 t = 0;
        m_bm->write(m_index_filename, (new_page+1)*PAGESIZE-sizeof(uint32), (char*)&t, sizeof(uint32));

        m_bm->write(m_index_filename, (new_page)*PAGESIZE, buff, key_size);
        m_bm->write(m_index_filename, (new_page)*PAGESIZE+key_size, (char*)&value, sizeof(uint32));
    }

    delete[]buff;

}

static uint32 find_keyvalue(BufferManager *bm, DBDataValue *key, uint32 value, std::string const &filename) {
}

static uint32 find_key(BufferManager *bm, DBDataValue *key, std::string const &filename) {
}

void HashIndex::deleteKey(DBDataValue key, uint32 value) {
//    uint32 start = find_keyvalue(m_bm, &key, value, m_index_filename);
//    if (!start) {
//        return;
//    }
//    char *buffer = new char[300];
//    uint32 item_size = bufsize + sizeof(value) + sizeof(uint32)*2;
//    m_bm->read(m_index_filename, start, buffer, item_size);
}

IndexIterator* HashIndex::findKey(DBDataValue key) {
    uint32 start = find_key(m_bm, &key, m_index_filename);
    if (!start) {
        // return kind of fake iterator
        return (IndexIterator*)0;
    }
    // return real iterator
    return (IndexIterator*)0;
}


Record* HashIndexIterator::getNextRecord() {
    return (Record*)0;
}

bool HashIndexIterator::hasNextRecord() {

}

//struct HashIndex: Index {
//    HashIndex(IndexFile* file, BufferManager* bm, Signature* signature, uint32 table_id, uint32 column) :
//        m_file(file), m_bm(bm), m_signature(signature), m_table_id(table_id), m_column(column) {}
//    virtual void createIndex() = 0;
//    virtual void addKey(int key, uint32 page) = 0;
//    virtual HashIndexIterator* findKey(int key) = 0;
//    virtual void deleteKey(int key, uint32 page) = 0;

//    IndexFile* m_file;
//    BufferManager* m_bm;
//    Signature* m_signature;
//    uint32 m_table_id;
//    uint32 m_column;
//};

//struct HashIndexIterator : IndexIterator {
//    IndexIterator(Index* index, int key) : m_index(index), m_key(key) {}
//    Record* getNextRecord() = 0;
//    bool hasNextRecord() = 0;
//protected:
//    Index* m_index;
//    int m_key;
//};
