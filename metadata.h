#ifndef METADATA_H
#define METADATA_H

#include "global.h"
#include "buffer/signature.h"
#include <string>
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
public:
    Table(std::string name): m_name(name), m_columns(std::vector<Column>()) {}
    Table() {}
    std::string const &get_name() const {
        return m_name;
    }

    int get_columns_count() const {
        return (int)m_columns.size();
    }

    Column &get_column(int idx) {
        return m_columns[idx];
    }

    void add_column(Column const &column) {
        m_columns.push_back(column);
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

public:
    DBMetaData(): m_tables(std::vector<Table>()) {}

    int get_tables_count() const {
        return (int)m_tables.size();
    }

    Table &get_table(int idx) {
        return m_tables[idx];
    }

    void add_table(Table const &table) {
        m_tables.push_back(table);
    }

    void print(std::ofstream &out) const {
        int t = m_tables.size();
        out.write((char*)(&t), sizeof(int));
        for(int i=0; i<(int)m_tables.size(); ++i) {
            m_tables[i].print(out);
        }
    }

    void read(std::ifstream &in) {
        int t = 0;
        in.read((char*)(&t), sizeof(int));
        m_tables = std::vector<Table>(t);
        for(int i=0; i<t; ++i)
            m_tables[i].read(in);
    }
};


#endif // METADATA_H
