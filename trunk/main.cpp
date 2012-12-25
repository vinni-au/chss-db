/*
 * Chss-db main program
 */ 
#include <iostream> 
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "db.hpp"
#include "dbshell.hpp"


int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cout << "usage: chss-db <dir> <pages in memory>" << std::endl;
        return 0;
    } else {
        std::string dirname(argv[1]);
        int pages = atoi(argv[2]);
        DB* db = new DB(dirname, pages);
        DBShell* shell = new DBShell(db);
        shell->run();
        delete db;
        delete shell;
    }
    return 0;
}