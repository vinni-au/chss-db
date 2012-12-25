#ifndef HEAPFILE_H
#define HEAPFILE_H
#include "../global.h"
//#include "buffermanager.h"
#include "record.h"

struct BufferManager;

struct HeapFile {
public:
    HeapFile(BufferManager* bm, uint32 table_id, Signature* signature);
    void create();
    uint32 get_size() const;
    Record* get(uint32 index) const;
    void add(Record* r);
private:
    static uint32 const HEADER_SIZE = sizeof(uint32);
    BufferManager* m_bm;
    uint32 m_table_id;
    Signature* m_signature;
    uint32 m_record_size;
};

#endif // HEAPFILE_H
