#include "db.hpp"
#include "metadata.h"

DB::DB(std::string dbname)
    : m_buffer(new BufferManager(this)),
      m_queryProcessor(new QueryProcessor(this)),
      m_name(dbname)
{
}
