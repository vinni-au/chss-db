#include "indexfile.h"
#include "../buffer/buffermanager.h"
#include "index.h"
#include "noindex.h"
#include "btreeindex.h"
#include <cstdio>

IndexFile::IndexFile(BufferManager* bm, uint32 table_id, Signature* signature) : m_bm(bm),
    m_table_id(table_id), m_table_filename("table" + int_to_string(table_id) + ".db"), m_signature(signature), m_record_size(signature->get_size_in_bytes()) {}

void IndexFile::create() {
    uint32 size = 0;
    m_bm->write(m_table_filename, 0, (char*)(&size), sizeof(uint32));
}

Record* IndexFile::get(uint32 index) const {
    Record* res = new Record(m_signature);
    m_bm->read(m_table_filename, TABLE_HEADER_SIZE + m_record_size * index, res->get_data_pointer(), m_record_size);
    return res;
}

void IndexFile::set(uint32 index, Record* record) const {
    m_bm->write(m_table_filename, TABLE_HEADER_SIZE + m_record_size * index, record->get_data_pointer(), m_record_size);
}

Signature* IndexFile::get_file_signature() const {
    return m_signature;
}

void IndexFile::add(Record* r) {
    r->get_data_pointer();
    uint32 size = get_size();
    m_bm->write(m_table_filename, TABLE_HEADER_SIZE + m_record_size * size, r->get_data_pointer(), m_record_size);
    ++size;
    m_bm->write(m_table_filename, 0, (char*)(&size), TABLE_HEADER_SIZE);
}

uint32 IndexFile::get_size() const {
    uint32 size = 0;
    m_bm->read(m_table_filename, 0, (char*)(&size), sizeof(uint32));
    return size;
}

void IndexFile::createIndex(uint32 column, uint32 index_type) {
    Index* current_index = 0;
    if(index_type == 1) {
        current_index = new BTreeindex(this, m_bm, m_signature, m_table_id, column);
    } else {
        current_index = new Noindex(this, m_bm, m_signature, m_table_id, column);
    }
    current_index->createIndex();
}

IndexIterator* IndexFile::select(uint32 column, DBDataValue key, uint32 index_type) {
    Index* current_index = 0;
    if(index_type == 1) {
        current_index = new BTreeindex(this, m_bm, m_signature, m_table_id, column);
    } else {
        current_index = new Noindex(this, m_bm, m_signature, m_table_id, column);
    }
    return current_index->findKey(key);
}
