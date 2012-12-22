#include "buffermanager.hpp"
#include <cstdio>
#include <cstring>

BufferManager::BufferManager(std::string dbfilename, uint32 pages)
    : m_pages(pages), m_disk_manager(new DiskManager(dbfilename))
{
    m_buffer = new char[PAGESIZE * m_pages];
}

BufferManager::~BufferManager() {
    delete[] m_buffer;
    delete m_disk_manager;
}

uint32 BufferManager::read(uint32 page, uint32 offset, char* buffer, uint32 size) {
    uint32 addr = get_address(page);
    size = std::min(size, PAGESIZE - offset);
    memcpy(buffer, m_buffer + addr * PAGESIZE + offset, size);
    return size;
}

uint32 BufferManager::write(uint32 page, uint32 offset, char* buffer, uint32 size) {
    uint32 addr = get_address(page);
    size = std::min(size, PAGESIZE - offset);
    memcpy(m_buffer + addr * PAGESIZE + offset, buffer, size);
    return size;
}

uint32 BufferManager::get_address(uint32 page) {
    if(!m_page_to_addr.count(page)) {
        std::map<uint32, uint32>::iterator sit = m_queue.begin();
        uint32 addr = sit->second;
        m_queue.erase(sit);
        uint32 oldpage = m_addr_to_page[addr];
        if(oldpage >= 0) {
            m_page_to_addr.erase(oldpage);
        }
        m_page_to_addr[page] = addr;
        m_addr_to_page[addr] = page;
    }
    uint32 addr = m_page_to_addr[page];
    m_queue.erase(m_priority[addr]);
    m_priority[addr] = ++max_priority;
    m_queue[max_priority] = addr;
    return addr;
}
