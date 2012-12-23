#ifndef BUFFERMANAGER_HPP
#define BUFFERMANAGER_HPP

#include "../global.h"
#include "../db.hpp"
#include "diskmanager.h"
#include <string>
#include <map>

#define forn(i, n) for(int i = 0; i < n; ++i)
#define make_pair mp

struct BufferManager {
public:
    BufferManager(DB* db, uint32 pages);
    ~BufferManager();
    uint32 read(uint32 table_id, uint32 offset, char* buffer, uint32 size);
    uint32 write(uint32 table_id, uint32 offset, char* buffer, uint32 size);
private:
    uint32 get_address(uint32 page);
    void read_page(uint32 page, uint32 address);
    void write_page(uint32 page, uint32 address);

private:
    DB* m_db;
    uint32 m_pages;
    char* m_buffer;
    uint32 max_priority;
    std::map<uint32, DiskManager*> tableid;
    std::map<std::string, DiskManager*> m_disk_managers;
//    std::map<std::pair<std::string, uint32>, uint32>;
    std::map<uint32, uint32> m_addr_to_page;
    std::map<uint32, uint32> m_page_to_addr;
    std::map<uint32, uint32> m_priority;
    std::map<uint32, uint32> m_queue;
};

#endif // BUFFERMANAGER_HPP
