#include "index.h"
#include "noindex.h"

NoindexIterator::NoindexIterator(Index* index, int key) : IndexIterator(index, key), m_current_position(0), current_record(0) {
}

Record* NoindexIterator::getNextRecord() {
    if(hasNextRecord()) {
        Record* result_record = current_record;
        current_record = 0;
        ++m_current_position;
        return result_record;
    }
    return 0;
}

bool NoindexIterator::hasNextRecord() {
    if(!current_record) {
        uint32 size = m_index->m_file->get_size();
        while(m_current_position < size) {
            current_record = m_index->m_file->get(m_current_position);
            if(current_record->getInt(m_index->m_column) == m_key)
                break;
            delete current_record;
            current_record = 0;
            ++m_current_position;
        }
    }
    return current_record != 0;
}

void Noindex::createIndex() {
}

void Noindex::addKey(int key, uint32 page) {
}

NoindexIterator* Noindex::findKey(int key) {
    return new NoindexIterator(this, key);
}

void Noindex::deleteKey(int key, uint32 page) {
}
