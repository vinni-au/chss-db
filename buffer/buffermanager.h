#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include "../global.h"
#include "../db.hpp"
#include "diskmanager.h"
#include <string>
#include <map>

struct BufferManager {
public:
    BufferManager(const std::string& directory, uint32 pages);
    ~BufferManager();
    uint32 read(const std::string& filename, uint32 offset, char* buffer, uint32 size);
    uint32 write(const std::string& filename, uint32 offset, char* buffer, uint32 size);
private:
    DiskManager* get_disk_manager(const std::string& filename);
    uint32 get_address(const std::string& filename, uint32 page);
    void read_page(const std::string& filename, uint32 page, uint32 address);
    void write_page(const std::string& filename, uint32 page, uint32 address);

private:
    std::string m_directory;
    uint32 m_pages;
    char* m_buffer;
    uint32 max_priority;
    std::map<std::string, DiskManager*> m_disk_managers;
    std::map<uint32, std::pair<std::string, uint32> > m_addr_to_page;
    std::map<std::pair<std::string, uint32>, uint32> m_page_to_addr;
    std::map<uint32, uint32> m_priority;
    std::map<uint32, uint32> m_queue;
};

#endif // BUFFERMANAGER_H
