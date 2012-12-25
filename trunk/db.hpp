#ifndef DB_HPP
#define DB_HPP

#include <iostream>
#include <vector>
#include "global.h"
#include "metadata.h"

struct QueryProcessor;
struct BufferManager;

struct DB
{
    DB(std::string dir);
    virtual ~DB();

    BufferManager* buffer() const {
        return m_buffer;
    }

    QueryProcessor* queryProcessor() const {
        return m_queryProcessor;
    }

    DBMetaData& metadata()
    {   return m_metadata;    }

private:
    BufferManager* m_buffer;
    QueryProcessor* m_queryProcessor;
    std::string m_dir;
    DBMetaData m_metadata;
};

#include "query/queryprocessor.hpp"
#include "buffer/buffermanager.h"

#endif // DB_HPP
