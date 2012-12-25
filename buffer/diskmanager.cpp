#include "diskmanager.h"

DiskManager::DiskManager(std::string dbfilename) : m_file(fopen(dbfilename.c_str(), "rb+")) {
    if(!m_file) {
        FILE* tmp = fopen(dbfilename.c_str(), "w");
        uint32 size = 0;
        fwrite(&size, 1, sizeof(uint32), tmp);
        fclose(tmp);
        m_file = fopen(dbfilename.c_str(), "rb+");
    }
}

DiskManager::~DiskManager() {
    if(m_file) {
        fclose(m_file);
    }
}

uint32 DiskManager::read(char* buffer, uint32 size, uint64 file_offset) {
    fseek(m_file, file_offset, SEEK_SET);
    return fread(buffer, 1, size, m_file);
}

uint32 DiskManager::write(char* buffer, uint32 size, uint64 file_offset) {
    fseek(m_file, file_offset, SEEK_SET);
    return fwrite(buffer, 1, size, m_file);
}
