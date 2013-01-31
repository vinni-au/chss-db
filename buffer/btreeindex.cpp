#include "btreeindex.h"
#include <cstring>

BTreeVertex::BTreeVertex(BufferManager* _bm, uint32 _u, const std::string& _file, DBDataType type) : bm(_bm), u(_u), file(_file), m_type(type) {
    char *buffer = new char[PAGESIZE];
    bm->read(file, PAGESIZE * u, buffer, PAGESIZE);
    root = *(int*)(buffer);
    total = *(int*)(buffer + sizeof(uint32));
    size = *(int*)(buffer + 2 * sizeof(uint32));
    isleaf = *(bool*)(buffer + 3 * sizeof(uint32));
    uint32 capacity = get_capacity(type);
    uint32 itemsize = sizeof(uint32) + type.get_size();
    data = new BTreeItem[capacity];
    for(uint32 i = 0; i < size; ++i) {
        uint32 value = *(uint32*)(buffer + BTreeindex::HEADER_SIZE + itemsize * i);
        if(type.get_type() == DBDataType::INT) {
            int key = *(int*)(buffer + BTreeindex::HEADER_SIZE + itemsize * i + sizeof(uint32));
            data[i] = BTreeItem(value, DBDataValue(key));
        } else if(type.get_type() == DBDataType::DOUBLE) {
            double key = *(double*)(buffer + BTreeindex::HEADER_SIZE + itemsize * i + sizeof(uint32));
            data[i] = BTreeItem(value, DBDataValue(key));
        } else if(type.get_type() == DBDataType::VARCHAR) {
            std::string key(buffer + BTreeindex::HEADER_SIZE + itemsize * i + sizeof(uint32), buffer + BTreeindex::HEADER_SIZE + itemsize * i + sizeof(uint32) + type.get_size());
            data[i] = BTreeItem(value, DBDataValue(key));
        }
//        std::cout << data[i].value << ' ' << data[i].key.intValue() << std::endl;

    }
    delete[] buffer;
}

uint32 BTreeVertex::get_capacity(DBDataType type) {
    return (PAGESIZE - BTreeindex::HEADER_SIZE) / (sizeof(uint32) + type.get_size());
}

BTreeVertex::~BTreeVertex() {
    uint32 capacity = get_capacity(m_type);
    char *buffer = new char[PAGESIZE];
    *(int*)(buffer) = root;
    *(int*)(buffer + sizeof(uint32)) = total;
    *(int*)(buffer + 2 * sizeof(uint32)) = size;
    *(bool*)(buffer + 3 * sizeof(uint32)) = isleaf;
    uint32 itemsize = sizeof(uint32) + m_type.get_size();
    for(uint32 i = 0; i < size; ++i) {
        *(uint32*)(buffer + BTreeindex::HEADER_SIZE + itemsize * i) = data[i].value;
        if(m_type.get_type() == DBDataType::INT) {
            *(int*)(buffer + BTreeindex::HEADER_SIZE + itemsize * i + sizeof(uint32)) = data[i].key.intValue();
        } else if(m_type.get_type() == DBDataType::DOUBLE) {
            *(double*)(buffer + BTreeindex::HEADER_SIZE + itemsize * i + sizeof(uint32)) = data[i].key.doubleValue();
        } else if(m_type.get_type() == DBDataType::VARCHAR) {
            memcpy(buffer + BTreeindex::HEADER_SIZE + itemsize * i + sizeof(uint32), data[i].key.stringValue().c_str(), m_type.get_size());
        }
    }
    bm->write(file, PAGESIZE * u, buffer, PAGESIZE);
    delete[] buffer;
    delete[] data;
}

Record* BTreeIterator::getNextRecord() {
    if(hasNextRecord()) {
        Record* result = current_record;
        current_record = 0;
        return result;
    }
    return 0;
}

bool BTreeIterator::hasNextRecord() {
    if(!current_record) {
        while(!path.empty()) {
            uint32& current_vertex = path[path.size() - 1].first;
            uint32& current_position = path[path.size() - 1].second;
            BTreeVertex cur(m_index->m_bm, current_vertex, ((BTreeindex*)m_index)->m_index_filename, m_key.type());
            if(cur.isleaf) {
//                std::cout << "Leaf (" << current_position << ", " << cur.size << ")" << std::endl;
                while(current_position < cur.size && cur.data[current_position].key != m_key) {
//                    std::cout << "> " << cur.data[current_position].key.intValue() << ' ' << m_key.intValue() << std::endl;
                    ++current_position;
                }
                if(current_position < cur.size) {
                    current_record = m_index->m_file->get(cur.data[current_position++].value);
                    break;
                }
                path.pop_back();
            } else {
//                std::cout << "Go (" << current_position << ", " << cur.size << ") " << std::endl;
                while(current_position < cur.size) {
                    if((current_position == 0 || cur.data[current_position - 1].key <= m_key) && (m_key <= cur.data[current_position].key || current_position == cur.size - 1)) {
                        ++current_position;
                        path.push_back(std::make_pair(cur.data[current_position - 1].value, 0));
                        break;
                    }
                    ++current_position;
                }
//                std::cout << current_vertex << ' ' << current_position << std::endl;
                if(current_position == cur.size) {
                    path.pop_back();
                }
            }
        }
    }
    return current_record != 0;
}

BTreeindex::BTreeindex(IndexFile* file, BufferManager* bm, Signature* signature, uint32 table_id, uint32 column) : Index(file, bm, signature, table_id, column), type(signature->get_field_type(column)) {
}

void BTreeindex::createIndex() {
    BTreeVertex* start = new BTreeVertex(m_bm, 0, m_index_filename, type);
    start->isleaf = true;
    start->total = 1;
    start->size = 0;
    start->root = 0;
    delete start;
    uint32 size = m_file->get_size();
    for(uint32 i = 0; i < size; ++i) {
        Record *cur = m_file->get(i);
//        std::cout << "Adding" << i << std::endl;
        addKey(cur->get(m_column), i);
        delete cur;
//        std::cout << "Added" << i << std::endl;
//        if(i == 1000)
//            break;
    }
//    start = new BTreeVertex(m_bm, 0, m_index_filename, type);
//    uint32 total = start->total;
//    delete start;
//    for(uint32 u = 0; u < total; ++u) {
//        BTreeVertex* cur = new BTreeVertex(m_bm, u, m_index_filename, type);
//        std::cout << "Vertex" << u << ": " << cur->size << std::endl;
//        for(int i = 0; i < cur->size; ++i) {
//            std::cout << cur->data[i].key.intValue() << ' ' << cur->data[i].value << std::endl;
//        }
//        delete cur;
//    }
}

void BTreeindex::addKey(DBDataValue key, uint32 index) {
//    std::cout << "addKey" << key.intValue() << ' ' << index << std::endl;
    BTreeItem item(index, key);

    BTreeVertex* start = new BTreeVertex(m_bm, 0, m_index_filename, type);
    uint32 root = start->root;
    delete start;

    BTreeVertex* vroot = new BTreeVertex(m_bm, root, m_index_filename, type);
    uint32 current_size = vroot->size;
    delete vroot;
    if(current_size == BTreeVertex::get_capacity(type)) {
//        std::cout << "FULL" << std::endl;

        BTreeVertex* start = new BTreeVertex(m_bm, 0, m_index_filename, type);
        uint32 s = start->total++;
        start->root = s;
        delete start;

        BTreeVertex* sv = new BTreeVertex(m_bm, s, m_index_filename, type);
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
    BTreeVertex* start = new BTreeVertex(m_bm, 0, m_index_filename, type);
    uint32 root = start->root;
    delete start;
    return new BTreeIterator(this, key, root);
}

void BTreeindex::deleteKey(DBDataValue key, uint32 position) {
    BTreeIterator* it = findKey(key);
    while(it->hasNextRecord()) {
        Record* record = it->getNextRecord();
        if(record->getPosition() == position) {
            uint32 current_vertex = it->path.back().first;
            uint32 current_position = it->path.back().second;
            BTreeVertex* vertex = new BTreeVertex(m_bm, current_vertex, m_index_filename, type);
            for(uint32 i = current_position; i < vertex->size; ++i) {
                vertex->data[i - 1] = vertex->data[i];
            }
            --vertex->size;
            delete vertex;
            delete record;
            break;
        }
        delete record;
    }
    delete it;
}

void BTreeindex::BTree_insert_nonfull(uint32 u, BTreeItem item) {
//    std::cout << "BTreeInsertNonfull" << u << std::endl;
    BTreeVertex* vertex = new BTreeVertex(m_bm, u, m_index_filename, type);
    int i = vertex->size - 1;
    if(vertex->isleaf) {
//        std::cout << u << std::endl;
        while (i >= 0 && item.key < vertex->data[i].key) {
            vertex->data[i + 1] = vertex->data[i];
            --i;
        }
//        std::cout << "item in " << i + 1 << std::endl;
        vertex->data[i + 1] = item;
        ++vertex->size;
        delete vertex;
    } else {
        --i;
        while (i >= 0 && item.key < vertex->data[i].key) {
            --i;
        }
        ++i;

        BTreeVertex* current_vertex = new BTreeVertex(m_bm, vertex->data[i].value, m_index_filename, type);
        uint32 current_size = current_vertex->size;
        delete current_vertex;

        if(current_size == BTreeVertex::get_capacity(type)) {
            delete vertex;
            BTree_split_child(u, i);
            vertex = new BTreeVertex(m_bm, u, m_index_filename, type);
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
//    std::cout << "BTree split child" << index << ' ' << std::endl;
    BTreeVertex vertex(m_bm, u, m_index_filename, type);

    BTreeVertex* start = new BTreeVertex(m_bm, 0, m_index_filename, type);
    uint32 z = start->total;
    ++start->total;
    delete start;

    uint32 y = vertex.data[index].value;
    BTreeVertex vy(m_bm, y, m_index_filename, type);
    BTreeVertex vz(m_bm, z, m_index_filename, type);
    vz.isleaf = vy.isleaf;

    uint32 t1 = BTreeVertex::get_capacity(type) / 2;
    uint32 t2 = BTreeVertex::get_capacity(type) - t1;

    for(int i = 0; i < t2; ++i) {
        vz.data[i] = vy.data[i + t1];
    }

    vy.size = t1;
    vz.size = t2;

    for(int i = vertex.size; i > index + 1; --i) {
        vertex.data[i] = vertex.data[i - 1];
    }
    vertex.data[index + 1].value = z;
//    std::cout << "in" << std::endl;

    for(int i = vertex.size - 1; i > index; --i) {
        vertex.data[i].key = vertex.data[i - 1].key;
    }
//    std::cout << "out" << std::endl;
    vertex.data[index].key = vy.data[t1].key;
    ++vertex.size;
}
