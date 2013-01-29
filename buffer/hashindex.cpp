#include "hashindex.h"

HashIndex::HashIndex(IndexFile* file, BufferManager* bm, Signature* signature, uint32 table_id, uint32 column) : Index(file, bm, signature, table_id, column), table_size(0) {}

HashIndexIterator::HashIndexIterator(Index *index, int key): IndexIterator(index, key) {}

void HashIndex::createIndex() {
    int32 offset = 0;
    for(int i=0;i<BUCKETS_CNT; ++i) {
        m_bm->write(m_file, offset, 0, sizeof(uint32));
        offset += sizeof(uint32);
        m_bm->write(m_file, offset, 0, sizeof(uint32));
        offset += sizeof(uint32);
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
    if (key.type()==DBDataType::INT) {
        hash = get_hash(key.stringValue());
    } else if (key.type()==DBDataType::VARCHAR) {
        hash = get_hash(key.intValue());
    } else if (key.type()==DBDataType::DOUBLE) {
        hash = get_hash(key.doubleValue());
    }
    hash%=mod;
    hash+=mod;
    hash%=mod;
    return hash;
}

static int get_bufsize(DBDataValue const &key) {
    int bufsize = 0;
    if (key.type()==DBDataType::INT) {
        bufsize = sizeof(int);
        *((int*)buffer) = key.intValue();
    } else if (key.type()==DBDataType::DOUBLE) {
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
    int hash = get_hash(key);
    char *buffer = new char[300];
    m_bm->read(m_file, hash*2*sizeof(uint32), buffer, 2*sizeof(uint32));
    uint32 start = *((uint32*)buffer);
    uint32 finish = *((uint32*)(buffer+sizeof(uint32));

    int bufsize = get_bufsize(key);
    uint32 finish = *((uint32*)(buffer+sizeof(uint32)));

    uint32 offset = 2*sizeof(uint32)*BUCKETS_CNT + table_size*(bufsize+sizeof(value)+sizeof(uint32)*2);

    uint32 item_size = bufsize + sizeof(value) + sizeof(uint32)*2;

    if (finish==0) {
        m_bm->write(m_file, offset, buffer, bufsize);
        m_bm->write(m_file, offset+bufsize, (char*)(&value), sizeof(value));
        // prev
        m_bm->write(m_file, offset+bufsize+sizeof(value), 0, sizeof(uint32));
        // next
        m_bm->write(m_file, offset+bufsize+sizeof(value)+sizeof(uint32), 0, sizeof(uint32));


        m_bm->write(m_file, hash*2*sizeof(uint32), offset, sizeof(uint32));
        m_bm->write(m_file, hash*2*sizeof(uint32)+sizeof(uint32), offset, sizeof(uint32));
    } else {
        m_bm->write(m_file, finish+bufsize+sizeof(value), (char*)(&offset), sizeof(uint32));
        m_bm->write(m_file, finish+bufsize+sizeof(value)+sizeof(uint32), (char*)(&offset), sizeof(uint32));

        m_bm->write(m_file, hash*2*sizeof(uint32)+sizeof(uint32), (char*)(&offset), sizeof(uint32));


        m_bm->write(m_file, offset, buffer, bufsize);
        m_bm->write(m_file, offset+bufsize, (char*)(&value), sizeof(value));
        // prev
        m_bm->write(m_file, offset+bufsize+sizeof(value), finish, sizeof(uint32));
        // next
        m_bm->write(m_file, offset+bufsize+sizeof(value)+sizeof(value), 0, sizeof(uint32));
    }
    ++table_size;
    delete[] buffer;
}

void HashIndex::deleteKey(DBDataValue key, uint32 value) {
    int hash = get_hash(key);
    char *buffer = new char[300];
    m_bm->read(m_file, hash*2*sizeof(uint32), buffer, 2*sizeof(uint32));
    uint32 start = *((uint32*)buffer);
    if (!start)
        return;
    int bufsize = get_bufsize(key);
    uint32 item_size = bufsize + sizeof(value) + sizeof(uint32)*2;
    for (;;) {
        if (!start) {
            break;
        }
        m_bm->read(m_file, start, buffer, bufsize);
        if (compare(key, buffer)) {
            if (*((uint32*)(buffer+bufsize)) == value) {
                // DO FUCKING DELETE HERE
            }
        }

        start = *((uint32*)(buffer+bufsize+sizeof(value)));
    }
    delete[] buffer;
}

IndexIterator* HashIndex::findKey(DBDataValue key) {
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
