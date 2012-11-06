#ifndef DBSHELL_HPP
#define DBSHELL_HPP

#include "db.hpp"

struct DBShell
{
    DBShell(DB* db);
    void run();

private:
    DB* m_db;
};

#endif // DBSHELL_HPP
