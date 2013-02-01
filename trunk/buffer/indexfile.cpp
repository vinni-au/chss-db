#include "indexfile.h"
#include "../buffer/buffermanager.h"
#include "index.h"
#include "noindex.h"
#include "btreeindex.h"
#include "hashindex.h"
#include <cstdio>

IndexFile::IndexFile(BufferManager* bm, uint32 table_id, Signature* signature) : m_bm(bm),
    m_table_id(table_id), m_table_filename("table" + int_to_string(table_id) + ".db"), m_signature(signature), m_record_size(signature->get_size_in_bytes()) {
    for(uint32 i = 0; i < signature->get_size(); ++i) {
        Index* current_index;
        if(signature->get_index_type(i) == TREEINDEX) {
            current_index = new BTreeindex(this, m_bm, m_signature, m_table_id, i);
        } else if(signature->get_index_type(i) == HASHINDEX) {
            current_index = new HashIndex(this, m_bm, m_signature, m_table_id, i);
        } else {
            current_index = new Noindex(this, m_bm, m_signature, m_table_id, i);
        }
        indexes.push_back(current_index);
    }
}

IndexFile::~IndexFile() {
    for(uint32 i = 0; i < m_signature->get_size(); ++i) {
        delete indexes[i];
    }
    delete m_signature;
}

void IndexFile::create() {
    uint32 size = 0;
    m_bm->write(m_table_filename, 0, (char*)(&size), sizeof(uint32));
}

Record* IndexFile::get(uint32 index) const {
    Record* res = new Record(index, m_signature);
    m_bm->read(m_table_filename, TABLE_HEADER_SIZE + m_record_size * index, res->get_data_pointer(), m_record_size);
    return res;
}

void IndexFile::set(uint32 index, Record* record) const {
    m_bm->write(m_table_filename, TABLE_HEADER_SIZE + m_record_size * index, record->get_data_pointer(), m_record_size);
}

Signature* IndexFile::get_file_signature() const {
    return m_signature;
}

void IndexFile::pop_back() {
    uint32 size = get_size();
    if(size) {
        --size;
        m_bm->write(m_table_filename, 0, (char*)(&size), TABLE_HEADER_SIZE);
    }
}

void IndexFile::add(Record* r) {
    uint32 size = get_size();
    m_bm->write(m_table_filename, TABLE_HEADER_SIZE + m_record_size * size, r->get_data_pointer(), m_record_size);
    for(uint32 i = 0; i < m_signature->get_size(); ++i) {
        indexes[i]->addKey(r->get(i), size);
    }
    ++size;
    m_bm->write(m_table_filename, 0, (char*)(&size), TABLE_HEADER_SIZE);
}

uint32 IndexFile::get_size() const {
    uint32 size = 0;
    m_bm->read(m_table_filename, 0, (char*)(&size), sizeof(uint32));
    return size;
}

void IndexFile::createIndex(uint32 column, IndexType type) {
    delete indexes[column];
    Index* current_index;
    if(type == TREEINDEX) {
        current_index = new BTreeindex(this, m_bm, m_signature, m_table_id, column);
    } else if(type == HASHINDEX) {
        current_index = new HashIndex(this, m_bm, m_signature, m_table_id, column);
    } else {
        current_index = new Noindex(this, m_bm, m_signature, m_table_id, column);
    }
    indexes[column] = current_index;
    indexes[column]->createIndex();
}

IndexIterator* IndexFile::select(uint32 column, DBDataValue key) {
    IndexIterator* res = indexes[column]->findKey(key);
    return res;
}

void IndexFile::update(uint32 column, uint32 position, DBDataValue oldkey, DBDataValue newkey) {
    indexes[column]->deleteKey(oldkey, position);
    indexes[column]->addKey(newkey, position);
}

void IndexFile::remove(uint32 column, uint32 position, DBDataValue key) {
    indexes[column]->deleteKey(key, position);
}
