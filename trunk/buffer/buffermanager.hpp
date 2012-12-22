#ifndef BUFFERMANAGER_HPP
#define BUFFERMANAGER_HPP

#include "../global.h"
#include <algorithm>
struct DB;

struct BufferManager
{
    BufferManager(DB* db);

    void setPagesInMemory(uint32 pages) {
        m_pages = std::min((uint32)MIN_PAGES_IN_MEMORY, pages);
    }

private:
    DB* m_db;
    uint32 m_pages;
};

#include "../db.hpp"

#endif // BUFFERMANAGER_HPP
