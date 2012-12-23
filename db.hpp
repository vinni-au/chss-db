#ifndef DB_HPP
#define DB_HPP

#include <iostream>
#include "global.h"

struct QueryProcessor;
class BufferManager;

struct DB
{
    DB(std::string dir);

    BufferManager* buffer() const {
        return m_buffer;
    }

    QueryProcessor* queryProcessor() const {
        return m_queryProcessor;
    }

private:
    BufferManager* m_buffer;
    QueryProcessor* m_queryProcessor;
    std::string m_dir;
};

#include "query/queryprocessor.hpp"
#include "buffer/buffermanager.h"

#endif // DB_HPP
