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
        std::cout << &pages << std::endl;
        DB* db = new DB(dirname, pages);
        DBShell* shell = new DBShell(db);
        shell->run();
        delete shell;
        delete db;
    }
    return 0;
}
