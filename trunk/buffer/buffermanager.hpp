#ifndef BUFFERMANAGER_HPP
#define BUFFERMANAGER_HPP

#include "../global.h"
#include "diskmanager.h"
#include <string>
#include <map>

class BufferManager
{
public:
    BufferManager(std::string dbfilename, uint32 pages);
    ~BufferManager();
    uint32 read(uint32 page, uint32 offset, char* buffer, uint32 size);
    uint32 write(uint32 page, uint32 offset, char* buffer, uint32 size);
private:
    uint32 get_address(uint32 page);
    void read_page(uint32 page, uint32 address);
    void write_page(uint32 page, uint32 address);
private:
    uint32 m_pages;
    char* m_buffer;
    DiskManager* m_disk_manager;
    uint32 max_priority;
    std::map<uint32, uint32> m_addr_to_page;
    std::map<uint32, uint32> m_page_to_addr;
    std::map<uint32, uint32> m_priority;
    std::map<uint32, uint32> m_queue;
};

#include "../db.hpp"

#endif // BUFFERMANAGER_HPP
