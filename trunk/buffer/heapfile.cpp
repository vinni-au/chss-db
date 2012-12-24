#include "heapfile.h"

HeapFile::HeapFile(BufferManager* bm, uint32 table_id, Signature* signature) : m_bm(bm), m_table_id(table_id), m_signature(signature) {
}

void HeapFile::create() {
    std::string filename = get_dbfilename(m_table_id);
    FILE* f = fopen(filename.c_str(), "w");
    fclose(f);
}

Record HeapFile::get(uint32 index) const {
    return 0;
}

void HeapFile::add(Record const& r) {
}
