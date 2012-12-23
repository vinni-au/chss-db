#ifndef DISKMANAGER_H
#define DISKMANAGER_H
#include "../global.h"
#include <string>
#include <cstdio>
#include <cstdlib>


class DiskManager
{
public:
    DiskManager(std::string dbfilename);
    ~DiskManager();
    uint32 read(char*, uint32 size, uint64 file_offset);
    uint32 write(char*, uint32 size, uint64 file_offset);

private:
    FILE* m_file;
};

#endif // DISKMANAGER_H
