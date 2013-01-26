#include "buffermanager.h"
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <stdexcept>


BufferManager::BufferManager(const std::string& directory, uint32 pages) : m_directory(directory), m_pages(std::max(pages, (uint32)MIN_PAGES_IN_MEMORY)),
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
    for(std::map<std::string, DiskManager*>::iterator it = m_disk_managers.begin(); it != m_disk_managers.end(); ++it) {
        delete it->second;
    }
}

uint32 BufferManager::read(const std::string& filename, uint32 offset, char* buffer, uint32 size) {
    uint32 page = offset / PAGESIZE;
    offset -= PAGESIZE * page;
    uint32 addr = get_address(filename, page);
    size = std::min(size, PAGESIZE - offset);
    memcpy(buffer, m_buffer + addr * PAGESIZE + offset, size);
    return size;
}

uint32 BufferManager::write(const std::string& filename, uint32 offset, char* buffer, uint32 size) {
    uint32 page = offset / PAGESIZE;
    offset -= PAGESIZE * page;
    uint32 addr = get_address(filename, page);
    size = std::min(size, PAGESIZE - offset);
    assert(addr * PAGESIZE + offset + size <= PAGESIZE * m_pages);
    assert(addr * PAGESIZE + offset + size >= 0);
    memcpy(m_buffer + addr * PAGESIZE + offset, buffer, size);
    return size;
}

uint32 BufferManager::get_address(const std::string& filename, uint32 page) {
    if(!m_page_to_addr.count(std::make_pair(filename, page))) {
        std::map<uint32, uint32>::iterator sit = m_queue.begin();
        uint32 addr = sit->second;
        m_queue.erase(sit);
        if(m_addr_to_page.count(addr)) {
            std::pair<std::string, uint32> tablepage = m_addr_to_page[addr];
            write_page(tablepage.first, tablepage.second, addr);
            m_page_to_addr.erase(tablepage);
        }
        m_page_to_addr[std::make_pair(filename, page)] = addr;
        m_addr_to_page[addr] = std::make_pair(filename, page);
        read_page(filename, page, addr);
    }
    uint32 addr = m_page_to_addr[std::make_pair(filename, page)];
    m_queue.erase(m_priority[addr]);
    m_priority[addr] = ++max_priority;
    m_queue[max_priority] = addr;
    return addr;
}

DiskManager* BufferManager::get_disk_manager(const std::string& filename) {
    if(!m_disk_managers.count(filename)) {
        return m_disk_managers[filename] = new DiskManager(m_directory + "/" + filename);
    }
    return m_disk_managers[filename];
}

void BufferManager::read_page(const std::string& filename, uint32 page, uint32 address) {
    DiskManager *diskmanager = get_disk_manager(filename);
    diskmanager->read(m_buffer + PAGESIZE * address, PAGESIZE, PAGESIZE * page);
}

void BufferManager::write_page(const std::string& filename, uint32 page, uint32 address) {
    DiskManager *diskmanager = get_disk_manager(filename);
    diskmanager->write(m_buffer + PAGESIZE * address, PAGESIZE, PAGESIZE * page);
}
