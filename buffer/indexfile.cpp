#include "indexfile.h"
#include "../buffer/buffermanager.h"
#include <cstdio>

IndexFile::IndexFile(BufferManager* bm, uint32 table_id, Signature* signature) : m_bm(bm),
    m_table_id(table_id), m_table_filename("table" + int_to_string(table_id) + ".db"), m_signature(signature), m_record_size(signature->get_size_in_bytes()) {}

void IndexFile::create() {
    uint32 size = 0;
    m_bm->write(m_table_filename, 0, (char*)(&size), sizeof(uint32));
}

Record* IndexFile::get(uint32 index) const {
    Record* res = new Record(m_signature);
    m_bm->read(m_table_filename, TABLE_HEADER_SIZE + m_record_size * index, res->get_data_pointer(), m_record_size);
//    std::cout << "Record size" << res->getInt(0) << std::endl;
    return res;
}

void IndexFile::add(Record* r) {
    r->get_data_pointer();
    uint32 size = get_size();
    m_bm->write(m_table_filename, TABLE_HEADER_SIZE + m_record_size * size, r->get_data_pointer(), m_record_size);
    ++size;
    m_bm->write(m_table_filename, 0, (char*)(&size), TABLE_HEADER_SIZE);
}

uint32 IndexFile::get_size() const {
    uint32 size = 0;
    m_bm->read(m_table_filename, 0, (char*)(&size), sizeof(uint32));
    return size;
}

std::string IndexFile::get_index_filename(uint32 column) const {
    return "index" + int_to_string(m_table_id) + "_" + int_to_string(column);
}

void IndexFile::createIndex(uint32 column) {
    std::string index_file = get_index_filename(column);
    BTreeVertex* start = new BTreeVertex(m_bm, 0, index_file);
    start->isleaf = true;
    start->total = 1;
    start->size = 0;
    start->root = 0;
    delete start;
    std::cout << "Ok2" << std::endl;
    uint32 size = get_size();
    std::cout << "Ok" << std::endl;
    std::cout << size << std::endl;
    for(uint32 i = 0; i < size; ++i) {
//        std::cout << "GO" << std::endl;
        Record *cur = get(i);
        BTreeItem item(cur->getInt(column), i);
//        std::cout << cur->getInt(0) << std::endl;
//        std::cout << "Adding" << i << std::endl;
//        std::cout << item.key << ' ' << item.value << std::endl;
        addKey(column, item, i);
        delete cur;
//        std::cout << "Added" << i << std::endl;
//        if(i == 9000)
//            break;
    }
    findKey(0);
}

void IndexFile::findKey(uint32 column) {
    std::string index_file = get_index_filename(column);
    std::cout << IndexFile::PAGE_CAPACITY << std::endl;

    BTreeVertex* start = new BTreeVertex(m_bm, 0, index_file);
    uint32 current_size = start->total;
    delete start;

    for(int u = 0; u < current_size; ++u) {
        std::cout << "V " << u << std::endl;
        BTreeVertex start(m_bm, u, index_file);
        //uint32 root = start.root;
        //BTreeVertex vroot(m_bm, root, index_file);
        std::cout << start.root << ' ' << start.total << ' ' << start.size <<  ' ' << start.isleaf << std::endl;
        for(int i = 0; i < start.size; ++i) {
            std::cout << start.data[i].key << ' ' << start.data[i].value << std::endl;
        }
    }
}

void IndexFile::addKey(uint32 column, BTreeItem& item, uint32 page) {
//    std::cout << "addKey" << column << ' ' << page << std::endl;
    std::string index_file = get_index_filename(column);

    BTreeVertex* start = new BTreeVertex(m_bm, 0, index_file);
    uint32 root = start->root;
    delete start;

    BTreeVertex* vroot = new BTreeVertex(m_bm, root, index_file);
    uint32 current_size = vroot->size;
    delete vroot;

    if(current_size == IndexFile::PAGE_CAPACITY) {
        std::cout << "FULL" << std::endl;

        BTreeVertex* start = new BTreeVertex(m_bm, 0, index_file);
        uint32 s = start->total++;
        start->root = s;
        delete start;

        BTreeVertex* sv = new BTreeVertex(m_bm, s, index_file);
        sv->isleaf = false;
        sv->size = 1;
        sv->data[0].value = root;
        delete sv;

        std::cout << "Ok" << std::endl;
        BTree_split_child(s, 0, column);
        std::cout << "Splitted" << std::endl;
        BTree_insert_nonfull(column, s, item);
    } else {
//        std::cout << "NONFULL" << std::endl;
        BTree_insert_nonfull(column, root, item);
    }
}

void IndexFile::BTree_insert_nonfull(uint32 column, uint32 u, BTreeItem item) {
//    std::cout << "BTreeInsertNonfull" << u << std::endl;
    std::string index_file = get_index_filename(column);
    BTreeVertex* vertex = new BTreeVertex(m_bm, u, index_file);
    int i = vertex->size - 1;
    if(vertex->isleaf) {
//        std::cout << u << std::endl;
        while (i >= 0 && item.key < vertex->data[i].key) {
            vertex->data[i + 1] = vertex->data[i];
            --i;
        }
        vertex->data[i + 1] = item;
        ++vertex->size;
        delete vertex;
    } else {
        --i;
        while (i >= 0 && item.key < vertex->data[i].key) {
            --i;
        }
        ++i;

        BTreeVertex* current_vertex = new BTreeVertex(m_bm, vertex->data[i].value, index_file);
        uint32 current_size = current_vertex->size;
        delete current_vertex;

        if(current_size == IndexFile::PAGE_CAPACITY) {
            delete vertex;
            BTree_split_child(u, i, column);
            vertex = new BTreeVertex(m_bm, u, get_index_filename(column));
            if(item.key > vertex->data[i].key) {
                ++i;
            }
        }
        uint32 v = vertex->data[i].value;
        delete vertex;
        BTree_insert_nonfull(column, v, item);
//        std::cout << "returned" << std::endl;
    }
//    std::cout << "exit" << std::endl;
}

void IndexFile::BTree_split_child(uint32 u, uint32 index, uint32 column) {
    std::cout << "BTree split child" << index << ' ' << column << std::endl;
    std::string index_file = get_index_filename(column);

    BTreeVertex vertex(m_bm, u, index_file);

    BTreeVertex* start = new BTreeVertex(m_bm, 0, index_file);
    uint32 z = start->total;
    ++start->total;
    delete start;

    uint32 y = vertex.data[index].value;
    BTreeVertex vy(m_bm, y, index_file);
    BTreeVertex vz(m_bm, z, index_file);
    vz.isleaf = vy.isleaf;

    uint32 t1 = IndexFile::PAGE_CAPACITY / 2;
    uint32 t2 = IndexFile::PAGE_CAPACITY - t1;

    for(int i = 0; i < t2; ++i) {
        vz.data[i] = vy.data[i + t1];
    }

    vy.size = t1;
    vz.size = t2;

    for(int i = vertex.size; i > index + 1; --i) {
        vertex.data[i] = vertex.data[i - 1];
    }
    vertex.data[index + 1].value = z;
    std::cout << "in" << std::endl;

//    for(int i = vertex.size - 1; i > index; --i) {
//        vertex.data[i].key = vertex.data[i - 1].key;
//    }
    std::cout << "out" << std::endl;
    vertex.data[index].key = vy.data[t1].key;
    ++vertex.size;
}

std::pair<uint32, uint32> IndexFile::BTree_search(uint32 column, uint32 page, uint32 pos, uint32 key) {
    BTreeVertex vertex(m_bm, page, m_table_filename);
    for(int i = pos; i < vertex.size; ++i) {
        if((i == 0 || vertex.data[i - 1].key <= key) && (i == vertex.size - 1 || key <= vertex.data[i + 1].key)) {
            if(vertex.isleaf) {
                return std::make_pair(page, i);
            } else {
                return BTree_search(column, vertex.data[i].value, 0, key);
            }
        }
    }
    return std::make_pair(-1, -1);
}

BTreeVertex::BTreeVertex(BufferManager* _bm, uint32 _u, const std::string& _file) : bm(_bm), u(_u), file(_file) {
    data = new BTreeItem[IndexFile::PAGE_CAPACITY * sizeof(BTreeItem)];
    //std::cout << "BTreeVertex" << data << ' ' << *((int*)data - 2) << std::endl;
    bm->read(file, PAGESIZE * u, (char*)(&root), sizeof(uint32));
    bm->read(file, PAGESIZE * u + sizeof(uint32), (char*)(&total), sizeof(uint32));
    bm->read(file, PAGESIZE * u + 2 * sizeof(uint32), (char*)(&size), sizeof(uint32));
    bm->read(file, PAGESIZE * u + 3 * sizeof(uint32), (char*)(&isleaf), sizeof(bool));
    bm->read(file, PAGESIZE * u + IndexFile::HEADER_SIZE, (char*)data, sizeof(BTreeItem) * IndexFile::PAGE_CAPACITY);
}
BTreeVertex::~BTreeVertex() {
    bm->write(file, PAGESIZE * u, (char*)(&root), sizeof(uint32));
    bm->write(file, PAGESIZE * u + sizeof(uint32), (char*)(&total), sizeof(uint32));
    bm->write(file, PAGESIZE * u + 2 * sizeof(uint32), (char*)(&size), sizeof(uint32));
    bm->write(file, PAGESIZE * u + 3 * sizeof(uint32), (char*)(&isleaf), sizeof(bool));
    bm->write(file, PAGESIZE * u + IndexFile::HEADER_SIZE, (char*)data, sizeof(BTreeItem) * IndexFile::PAGE_CAPACITY);
    //std::cout << "~BTreeVertex " << data << ' ' << *((int*)data - 2) << ' ' << u << ' ' << root << ' ' << total << ' ' << size << ' ' << isleaf << std::endl;
    //std::cout << "Clearing" << std::endl;
    delete[] data;
    //std::cout << "Cleared" << std::endl;
}
