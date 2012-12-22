#include "diskmanager.h"

DiskManager::DiskManager(std::string dbfilename) : m_file(fopen(dbfilename.c_str(), "rw")) {
}

DiskManager::~DiskManager() {
    if(m_file) {
        fclose(m_file);
    }
}

void DiskManager::read(char* buffer, size_t size, size_t file_offset) {

}

void DiskManager::write(char* buffer, size_t size, size_t file_offset) {

}
