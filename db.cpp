#include "db.hpp"
#include "metadata.h"

DB::DB(std::string dir)
    : m_buffer(0),
      m_queryProcessor(new QueryProcessor(this)),
      m_dir(dir)
{
}
