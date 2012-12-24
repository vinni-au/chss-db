#include "buffermanager.h"
#include <cstdio>
#include <cstring>
#include <algorithm>

BufferManager::BufferManager(DB* db, uint32 pages) : m_db(db), m_pages(std::max(pages, (uint32)MIN_PAGES_IN_MEMORY)),
        m_buffer(new char[PAGESIZE * m_pages]), max_priority(0) {
    memset(m_buffer, 0, m_pages * PAGESIZE);
    for(size_t i = 0; i != m_pages; ++i) {
        m_queue[++max_priority] = i;
    }
}

BufferManager::~BufferManager() {
    for(size_t i = 0; i != m_pages; ++i) if(m_addr_to_page.count(i)) {
        write_page(m_addr_to_page[i].first, m_addr_to_page[i].second, i);
    }
    delete[] m_buffer;
}

uint32 BufferManager::read(uint32 table_id, uint32 offset, char* buffer, uint32 size) {
    uint32 page = offset / PAGESIZE;
    uint32 addr = get_address(table_id, page);
    size = std::min(size, PAGESIZE - offset);
    memcpy(buffer, m_buffer + addr * PAGESIZE + offset, size);
    return size;
}

uint32 BufferManager::write(uint32 table_id, uint32 offset, char* buffer, uint32 size) {
    uint32 page = offset / PAGESIZE;
    uint32 addr = get_address(table_id, page);
    size = std::min(size, PAGESIZE - offset);
    memcpy(m_buffer + addr * PAGESIZE + offset, buffer, size);
    return size;
}

uint32 BufferManager::get_address(uint32 table_id, uint32 page) {
    if(!m_page_to_addr.count(std::make_pair(table_id, page))) {
        std::map<uint32, uint32>::iterator sit = m_queue.begin();
        uint32 addr = sit->second;
        m_queue.erase(sit);
        if(m_addr_to_page.count(addr)) {
            std::pair<uint32, uint32> tablepage = m_addr_to_page[addr];
            write_page(tablepage.first, tablepage.second, addr);
            m_page_to_addr.erase(tablepage);
        }
        m_page_to_addr[std::make_pair(table_id, page)] = addr;
        m_addr_to_page[addr] = std::make_pair(table_id, page);
        read_page(table_id, page, addr);
    }
    uint32 addr = m_page_to_addr[std::make_pair(table_id, page)];
    m_queue.erase(m_priority[addr]);
    m_priority[addr] = ++max_priority;
    m_queue[max_priority] = addr;
    return addr;
}

DiskManager* BufferManager::get_disk_manager(uint32 table_id) {
    if(m_disk_managers.count(table_id)) {
        return m_disk_managers[table_id] = new DiskManager(get_dbfilename(table_id));
    }
    return m_disk_managers[table_id];
}

void BufferManager::read_page(uint32 table_id, uint32 page, uint32 address) {
    DiskManager *diskmanager = get_disk_manager(table_id);
    diskmanager->read(m_buffer + PAGESIZE * address, PAGESIZE, PAGESIZE * page);
}

void BufferManager::write_page(uint32 table_id, uint32 page, uint32 address) {
    DiskManager *diskmanager = get_disk_manager(table_id);
    diskmanager->write(m_buffer + PAGESIZE * address, PAGESIZE, PAGESIZE * page);
}
