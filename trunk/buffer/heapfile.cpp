#include "heapfile.h"
#include "../buffer/buffermanager.h"
#include <cstdio>

HeapFile::HeapFile(BufferManager* bm, uint32 table_id, Signature* signature) : m_bm(bm),
        m_table_id(table_id), m_signature(signature), m_record_size(signature->get_size_in_bytes()) {}

void HeapFile::create() {
    uint32 size = 0;
    m_bm->write(m_table_id, 0, (char*)(&size), sizeof(uint32));
}

Record* HeapFile::get(uint32 index) const {
    Record* res = new Record(m_signature);
    m_bm->read(m_table_id, HEADER_SIZE + m_record_size * index, res->get_data_pointer(), m_record_size);
    return res;
}

void HeapFile::add(Record* r) {
    m_bm->write(m_table_id, HEADER_SIZE + m_record_size * get_size(), r->get_data_pointer(), m_record_size);
}

uint32 HeapFile::get_size() const {
    uint32 size = 0;
    m_bm->write(m_table_id, 0, (char*)(&size), HEADER_SIZE);
    return size;
}
