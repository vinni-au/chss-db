#include "index.h"

IndexIterator::IndexIterator(Index* index, int key) : m_index(index), m_current_position(0), m_key(key), current_record(0) {}

Record* IndexIterator::getNextRecord() {
    if(current_record)
        return current_record;
}

bool IndexIterator::hasNextRecord() {

}


Index::Index(BufferManager* bm, Signature* signature, uint32 table_id, uint32 column) :
    m_bm(bm), m_signature(signature), m_table_id(table_id), m_column(column) {
}

void Index::createIndex() {}

void Index::addKey(int key, uint32 page) {
}

IndexIterator* Index::findKey(int key) {
}

void Index::deleteKey(int key, uint32 page) {

}
