#include "btreeindex.h"

BTreeVertex::BTreeVertex(BufferManager* _bm, uint32 _u, const std::string& _file) : bm(_bm), u(_u), file(_file) {
    data = new BTreeItem[BTreeindex::PAGE_CAPACITY * sizeof(BTreeItem)];
    bm->read(file, PAGESIZE * u, (char*)(&root), sizeof(uint32));
    bm->read(file, PAGESIZE * u + sizeof(uint32), (char*)(&total), sizeof(uint32));
    bm->read(file, PAGESIZE * u + 2 * sizeof(uint32), (char*)(&size), sizeof(uint32));
    bm->read(file, PAGESIZE * u + 3 * sizeof(uint32), (char*)(&isleaf), sizeof(bool));
    bm->read(file, PAGESIZE * u + BTreeindex::HEADER_SIZE, (char*)data, sizeof(BTreeItem) * BTreeindex::PAGE_CAPACITY);
}

BTreeVertex::~BTreeVertex() {
    bm->write(file, PAGESIZE * u, (char*)(&root), sizeof(uint32));
    bm->write(file, PAGESIZE * u + sizeof(uint32), (char*)(&total), sizeof(uint32));
    bm->write(file, PAGESIZE * u + 2 * sizeof(uint32), (char*)(&size), sizeof(uint32));
    bm->write(file, PAGESIZE * u + 3 * sizeof(uint32), (char*)(&isleaf), sizeof(bool));
    bm->write(file, PAGESIZE * u + BTreeindex::HEADER_SIZE, (char*)data, sizeof(BTreeItem) * BTreeindex::PAGE_CAPACITY);
    delete[] data;
}

Record* BTreeIterator::getNextRecord() {

}

bool BTreeIterator::hasNextRecord() {

}

BTreeindex::BTreeindex(IndexFile* file, BufferManager* bm, Signature* signature, uint32 table_id, uint32 column) : Index(file, bm, signature, table_id, column) {
}

void BTreeindex::createIndex() {
    BTreeVertex* start = new BTreeVertex(m_bm, 0, m_btree_file);
    start->isleaf = true;
    start->total = 1;
    start->size = 0;
    start->root = 0;
    delete start;
    uint32 size = m_file->get_size();
    for(uint32 i = 0; i < size; ++i) {
//        std::cout << "GO" << std::endl;
        Record *cur = m_file->get(i);
//        std::cout << "Adding" << i << std::endl;
        addKey(cur->get(m_column), i);
        delete cur;
//        std::cout << "Added" << i << std::endl;
//        if(i == 9000)
//            break;
    }
}

void BTreeindex::addKey(DBDataValue key, uint32 index) {
//    std::cout << "addKey" << column << ' ' << page << std::endl;
    BTreeItem item(key, index);

    BTreeVertex* start = new BTreeVertex(m_bm, 0, m_btree_file);
    uint32 root = start->root;
    delete start;

    BTreeVertex* vroot = new BTreeVertex(m_bm, root, m_btree_file);
    uint32 current_size = vroot->size;
    delete vroot;

    if(current_size == PAGE_CAPACITY) {
        std::cout << "FULL" << std::endl;

        BTreeVertex* start = new BTreeVertex(m_bm, 0, m_btree_file);
        uint32 s = start->total++;
        start->root = s;
        delete start;

        BTreeVertex* sv = new BTreeVertex(m_bm, s, m_btree_file);
        sv->isleaf = false;
        sv->size = 1;
        sv->data[0].value = root;
        delete sv;

        BTree_split_child(s, 0);
        BTree_insert_nonfull(s, item);
    } else {
        BTree_insert_nonfull(root, item);
    }

}

BTreeIterator* BTreeindex::findKey(DBDataValue key) {
    BTreeVertex* start = new BTreeVertex(m_bm, 0, m_btree_file);
    uint32 root = start->root;
    delete start;
    return new BTreeIterator(this, key, root);
}

void BTreeindex::deleteKey(DBDataValue key, uint32 index) {
}

void BTreeindex::BTree_insert_nonfull(uint32 u, BTreeItem item) {
//    std::cout << "BTreeInsertNonfull" << u << std::endl;
    BTreeVertex* vertex = new BTreeVertex(m_bm, u, m_btree_file);
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

        BTreeVertex* current_vertex = new BTreeVertex(m_bm, vertex->data[i].value, m_btree_file);
        uint32 current_size = current_vertex->size;
        delete current_vertex;

        if(current_size == PAGE_CAPACITY) {
            delete vertex;
            BTree_split_child(u, i);
            vertex = new BTreeVertex(m_bm, u, m_btree_file);
            if(item.key > vertex->data[i].key) {
                ++i;
            }
        }
        uint32 v = vertex->data[i].value;
        delete vertex;
        BTree_insert_nonfull(v, item);
//        std::cout << "returned" << std::endl;
    }
//    std::cout << "exit" << std::endl;
}

void BTreeindex::BTree_split_child(uint32 u, uint32 index) {
    std::cout << "BTree split child" << index << ' ' << std::endl;
    BTreeVertex vertex(m_bm, u, m_btree_file);

    BTreeVertex* start = new BTreeVertex(m_bm, 0, m_btree_file);
    uint32 z = start->total;
    ++start->total;
    delete start;

    uint32 y = vertex.data[index].value;
    BTreeVertex vy(m_bm, y, m_btree_file);
    BTreeVertex vz(m_bm, z, m_btree_file);
    vz.isleaf = vy.isleaf;

    uint32 t1 = PAGE_CAPACITY / 2;
    uint32 t2 = PAGE_CAPACITY - t1;

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

std::pair<uint32, uint32> BTreeindex::BTree_search(uint32 page, uint32 pos, uint32 key) {
    BTreeVertex vertex(m_bm, page, m_btree_file);
    for(int i = pos; i < vertex.size; ++i) {
        if((i == 0 || vertex.data[i - 1].key < key) && (i == vertex.size - 1 || key < vertex.data[i + 1].key)) {
            if(vertex.isleaf) {
                return std::make_pair(page, i);
            } else {
                return BTree_search(vertex.data[i].value, 0, key);
            }
        }
    }
    return std::make_pair(-1, -1);
}

