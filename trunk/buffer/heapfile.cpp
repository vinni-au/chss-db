#include "heapfile.h"

HeapFile::HeapFile(BufferManager* bm, uint32 table_id) : m_bm(bm), m_table_id(table_id) {
}

void HeapFile::create() {

}

Record HeapFile::get(uint32 index) {
    return 0;
}

void HeapFile::add(Record const& r) {
}

