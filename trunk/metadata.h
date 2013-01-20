#ifndef METADATA_H
#define METADATA_H

#include "global.h"
#include "buffer/signature.h"
#include "buffer/heapfile.h"
#include <string>
#include <map>
#include <vector>
#include <fstream>


struct Column {
private:
    DBDataType m_type;
    std::string m_name;
public:
    Column(DBDataType type, std::string name): m_type(type), m_name(name) { }
    Column() { }
    DBDataType get_type() const {
        return m_type;
    }

    std::string get_name() const {
        return m_name;
    }

    void print(std::ofstream &out) const {
        m_type.print(out);
        int t = m_name.length() + 1;
        out.write((char*)&t, sizeof(int));
        out.write(m_name.c_str(), m_name.length() + 1);
    }

    void read(std::ifstream &in) {
        m_type.read(in);
        int t = 0;
        in.read((char*)(&t), sizeof(int));
        char *buf = new char[t];
        in.read(buf, t);
        m_name = std::string(buf);
        delete[] buf;
    }

};

struct Table {
private:
    std::string m_name;
    std::vector<Column> m_columns;
    HeapFile* m_file;
public:
    Table(std::string name, HeapFile* file): m_name(name), m_columns(std::vector<Column>()), m_file(file) {}
    Table() : m_file(0) {}
    std::string const &get_name() const {
        return m_name;
    }
    ~Table() {
        if(m_file) {
            delete m_file;
        }
    }

    void set_tablename(std::string tablename) {
        m_name = tablename;
    }

    int get_columns_count() const {
        return (int)m_columns.size();
    }

    Column &get_column(int idx) {
        return m_columns[idx];
    }

    HeapFile* get_file() const {
        return m_file;
    }

    void add_column(Column const &column) {
        m_columns.push_back(column);
    }

    void set_file(HeapFile* file) {
        m_file = file;
    }

    void print(std::ofstream &out) const {
        int t = m_name.length() + 1;
        out.write((char*)&t, sizeof(int));
        out.write(m_name.c_str(), m_name.length() + 1);
        t = (int)m_columns.size();
        out.write((char*)&t, sizeof(int));
        for(int i=0; i<(int)m_columns.size(); ++i) {
            m_columns[i].print(out);
        }
    }

    void read(std::ifstream &in) {
        int t = 0;
        in.read((char*)(&t), sizeof(int));
        char *buf = new char[t];
        in.read(buf, t);
        m_name = std::string(buf);
        in.read((char*)(&t), sizeof(int));
        m_columns = std::vector<Column>(t);
        for(int i=0; i<t; ++i)
            m_columns[i].read(in);
        delete []buf;
    }

    Signature* makeSignature() {
        std::vector<std::string> names;
        std::vector<DBDataType> types;
        for (int i = 0; i < (int)m_columns.size(); ++i) {
            names.push_back(m_columns[i].get_name());
            types.push_back(m_columns[i].get_type());
        }
        return new Signature(names, types);
    }
};

struct DBMetaData {
private:
    std::vector<Table> m_tables;
    std::map<std::string, uint32> m_table_id;

public:
    DBMetaData(): m_tables(std::vector<Table>()) {}

    int get_tables_count() const {
        return (int)m_tables.size();
    }

    Table &get_table(std::string tablename) {
        uint32 index = get_table_index(tablename);
        return m_tables[index];
    }

    Table &get_table(int index) {
        return m_tables[index];
    }

    uint32 get_table_index(std::string tablename) {
        uint32 index = m_table_id[tablename];
        return index;
    }

    void add_table(Table const &table) {
        m_table_id[table.get_name()] = m_tables.size();
        m_tables.push_back(table);
    }

    bool exist_table(std::string tablename) const {
        return m_table_id.count(tablename);
    }

    void print(std::ofstream &out) const {
        int t = m_tables.size();
        std::cout << m_tables.size() << std::endl;
        out.write((char*)(&t), sizeof(int));
        for(int i=0; i<(int)m_tables.size(); ++i) {
            m_tables[i].print(out);
        }
    }

    void read(std::ifstream &in) {
        int table_count = 0;
        in.read((char*)(&table_count), sizeof(int));
        m_tables = std::vector<Table>();
        for(int i=0; i<table_count; ++i) {
            Table t;
            t.read(in);
            add_table(t);
        }
    }
};


#endif // METADATA_H
