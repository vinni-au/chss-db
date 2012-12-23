#include "buffermanager.hpp"
#include <cstdio>
#include <cstring>
#include <algorithm>

BufferManager::BufferManager(std::string dbfilename, uint32 pages)
    : m_pages(std::max(pages, (uint32)MIN_PAGES_IN_MEMORY)), m_buffer(new char[PAGESIZE * m_pages]),
      m_disk_manager(new DiskManager(dbfilename)), max_priority(0) {
    memset(m_buffer, 0, m_pages * PAGESIZE);
    for(size_t i = 0; i != m_pages; ++i) {
        m_queue[++max_priority] = i;
    }
}

BufferManager::~BufferManager() {
    for(size_t i = 0; i != m_pages; ++i) if(m_addr_to_page.count(i)) {
        write_page(m_addr_to_page[i], i);
    }
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
        if(m_addr_to_page.count(addr)) {
            write_page(m_addr_to_page[addr], addr);
            uint32 oldpage = m_addr_to_page[addr];
            m_page_to_addr.erase(oldpage);
        }
        m_page_to_addr[page] = addr;
        m_addr_to_page[addr] = page;
        read_page(page, addr);
    }
    uint32 addr = m_page_to_addr[page];
    m_queue.erase(m_priority[addr]);
    m_priority[addr] = ++max_priority;
    m_queue[max_priority] = addr;
    return addr;
}


void BufferManager::read_page(uint32 page, uint32 address) {
    m_disk_manager->read(m_buffer + PAGESIZE * address, PAGESIZE, PAGESIZE * page);
}

void BufferManager::write_page(uint32 page, uint32 address) {
    m_disk_manager->write(m_buffer + PAGESIZE * address, PAGESIZE, PAGESIZE * page);
}
