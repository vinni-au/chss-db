#include "db.hpp"
#include <fstream>

DB::DB(std::string dir, uint32 pages)
    : m_buffer(new BufferManager(dir, pages)),
      m_queryProcessor(new QueryProcessor(this)),
      m_dir(dir),
      m_metadata(new DBMetaData())
{
    if (m_dir.at(m_dir.length() - 1) != '/')
        m_dir.append("/");

    //read metadata
    std::ifstream m_mdfile((m_dir + "db.meta").c_str(), std::fstream::binary);
    if (m_mdfile.is_open()) {
        m_metadata->read(m_mdfile);
        m_mdfile.close();
    } else {
        FILE* f = fopen((m_dir + "db.meta").c_str(), "w");
        fclose(f);
    }
}

DB::~DB()
{
    //write metadata
    std::ofstream m_mdfile((m_dir + "db.meta").c_str(), std::fstream::binary);
    assert(m_mdfile.is_open() == true);
    m_metadata->print(m_mdfile);
    m_mdfile.close();
    delete m_buffer;
    delete m_queryProcessor;
    delete m_metadata;
}
