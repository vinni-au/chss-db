#ifndef DB_HPP
#define DB_HPP

#include <iostream>
#include "global.h"
#include "buffer/buffermanager.hpp"

struct DB
{
    DB(std::string dbname);

    BufferManager* buffer() const
    {   return m_buffer;    }

private:
    BufferManager* m_buffer;
};

#endif // DB_HPP
