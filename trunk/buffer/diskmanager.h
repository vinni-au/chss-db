#ifndef DISKMANAGER_H
#define DISKMANAGER_H
#include <string>
#include <cstdio>
#include <cstdlib>

class DiskManager
{
public:
    DiskManager(std::string dbfilename);
    ~DiskManager();
    void read(char*, size_t size, size_t offset);
    void write(char*, size_t size, size_t offset);
private:
    FILE* m_file;
};

#endif // DISKMANAGER_H
