#ifndef BUFFERMANAGER_HPP
#define BUFFERMANAGER_HPP

#include "../global.h"

struct DB;

struct BufferManager
{
    BufferManager(DB* db);

    void setPagesInMemory(uint32 pages) {
        m_pages = pages < MIN_PAGES_IN_MEMORY ? MIN_PAGES_IN_MEMORY : pages;
    }

private:
    DB* m_db;
    uint32 m_pages;
};

#include "../db.hpp"

#endif // BUFFERMANAGER_HPP
