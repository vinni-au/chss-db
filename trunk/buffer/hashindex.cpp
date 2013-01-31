#include "hashindex.h"

HashIndex::HashIndex(IndexFile* file, BufferManager* bm, Signature* signature, uint32 table_id, uint32 column) : Index(file, bm, signature, table_id, column), table_size(0) {}

HashIndexIterator::HashIndexIterator(Index *index, DBDataValue key): IndexIterator(index, key) {}

static void create_page(BufferManager *bm, std::string const &filename, int pagenumber) {
    char *buff = new char[PAGESIZE];
    memset(buff, 0, PAGESIZE);
    bm->write(filename, pagenumber*PAGESIZE, buff, PAGESIZE);
}

void HashIndex::createIndex() {
    for(int i=0;i<=BUCKETS_CNT;++i) {
        create_page(m_bm, m_index_filename, i);
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

static void get_buff(DBDataValue const &key, int value, char *buffer, int &len) {
    int bufsize = 0;
    if (key.type().get_type()==DBDataType::INT) {
        bufsize = sizeof(int)+sizeof(value);
        *((int*)buffer) = key.intValue();
        *((int*)(buffer+sizeof(int))) = value;
    } else if (key.type().get_type()==DBDataType::DOUBLE) {
        bufsize = sizeof(double)+sizeof(value);
        *((double*)buffer) = key.doubleValue();
        *((int*)(buffer+sizeof(double))) = value;
    } else {
        bufsize = key.type().m_len+sizeof(value);
        memcpy(buffer, key.stringValue().c_str(), key.type().m_len);
        *((int*)(buffer+key.type().m_len)) = value;
    }
    len = bufsize;
}

static bool compare(DBDataValue const &key, char *data) {
    if (key.type().get_type() == DBDataType::INT) {
        return key.intValue() == *((int*)data);
    } else if (key.type().get_type() == DBDataType::DOUBLE) {
        return key.doubleValue() == *((double*)data);
    } else {
        return memcmp(data, key.stringValue().c_str(), key.type().m_len) == 0;
    }
}

static inline int get_offset(int hash, int idx, int record_len, int records_per_first_page, int records_per_page){
    if (idx<records_per_first_page) {
        int offset = (hash+1)*PAGESIZE+sizeof(uint32)+idx*record_len;
        return offset;
    } else {
        if ((list_len-records_per_first_page)%records_per_page==0) {
            int offset = hash+(list_len-records_per_first_page)/records_per_page*PAGESIZE*HashIndex::BUCKETS_CNT;
            return offset;
        } else {
            int offset = list_len-records_per_first_page;
            int cur_page = hash+BUCKETS_CNT+1;
            while (offset>=records_per_page) {
                offset-=records_per_page;
                cur_page+=BUCKETS_CNT;
            }
            return cur_page*PAGESIZE+offset*record_len;
        }
    }
}

void HashIndex::addKey(DBDataValue key, uint32 value) {
    int hash = get_hash(key, BUCKETS_CNT);
    char *buff = new char[300];
    m_bm->read(m_index_filename, (hash+1)*PAGESIZE, buff, sizeof(uint32));
    int list_len = *((uint32*)buff);
    char *record = new char[300];
    int record_len =0 ;
    get_buff(key, value, record, record_len);
    int records_per_first_page = (PAGESIZE-sizeof(uint32))/record_len;
    int records_per_page = (PAGESIZE)/record_len;
    int offset = get_offset(hash, list_len, record_size, records_per_first_page, records_per_page);
    m_bm->write(m_index_filename, offset, record, record_len);
    delete[] record;
    delete[] buff;
}

void HashIndex::deleteKey(DBDataValue key, uint32 value) {
    int hash = get_hash(key, BUCKETS_CNT);
    char *buff = new char[300];

    int record_len =0 ;
    get_buff(key, value, record, record_len);

    m_bm->read(m_index_filename, (hash+1)*PAGESIZE, buff, sizeof(uint32));
    int list_len = *((uint32*)buff);
    if (!list_len) {
        delete[]buff;
        return;
    }
    int records_per_first_page = (PAGESIZE-sizeof(uint32))/record_len;
    int records_per_page = (PAGESIZE)/record_len;

    int records = records_per_first_page;
    int cur_offset = (hash+1)*PAGESIZE + sizeof(uint32);
    int cur_page = hash+1;
    int rest_list = list_len;
    for(;;) {
        int t = records;
        while (t--) {
            m_bm->read(m_index_filename, cur_offset, buff, record_len);
            if (compare(key, buff)) {
                int cur_value = *((uint32*)(buff+record_len-sizeof(value)));
                if (value==cur_value) {
                    if (list_len==1) {
                        --list_len;
                    } else {
                        int offset = get_offset(hash, list_len-1, record_len, records_per_first_page, records_per_page);
                        m_bm->read(m_index_filename, offset, buff, record_len);
                        m_bm->write(m_index_filename, cur_offset, buff, record_len);
                    }
                    delete[] buff;
                    return;
                }
            }
            cur_offset += record_len + sizeof(value);
            --rest_list;
            if (!rest_list)
                break;
        }
        if (!rest_list)
            break;
        cur_page += BUCKETS_CNT;
        cur_offset = PAGESIZE*BUCKETS_CNT;
        records = records_per_page;
    }

    delete[]buff;
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
