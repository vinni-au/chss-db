#include "db.hpp"

DB::DB(std::string dbname)
    : m_buffer(new BufferManager(this))
{
}
