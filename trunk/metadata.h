#ifndef METADATA_H
#define METADATA_H

#include "global.h"
#include <string>
#include <vector>
#include <fstream>

struct Column {
private:
    DBDataType m_type;
    std::string m_name;
public:
    Column(DBDataType type, std::string name): m_type(type), m_name(name) { }

    DBDataType get_type() const {
        return m_type;
    }

    std::string get_name() const {
        return m_name;
    }

    void print(std::ofstream &out) const {
        m_type.print(out);
        out.write(m_name.c_str(), m_name.length());
    }
};

struct Table {
private:
    std::string m_name;
    std::vector<Column> m_columns;
public:
    Table(std::string name): m_name(name), m_columns(std::vector<Column>()) {}

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

    void print(std::ofstream const &out) const {
        out.write(m_name.c_str(), m_name.length());
        int t = m_name.length();
        out.write((char*)&t, sizeof(int));
        for(int i=0; i<(int)m_columns.size(); ++i) {
            m_columns[i].print(out);
        }
    }
};

struct DBMetaData {
private:
    std::string m_name;
    std::vector<Table> tables;

public:
    DBMetaData(std::string db_name): m_name(db_name), tables(std::vector<Table>()) {}

    std::string const& get_name() const {
        return m_name;
    }

    int get_tables_count() const {
        return (int)tables.size();
    }

    Table &get_table(int idx) {
        return tables[idx];
    }

    void add_table(Table const &table) {
        tables.push_back(table);
    }

    void print(std::ofstream &out) const {
        out.write(m_name.c_str(), m_name.length());
        int t = tables.size();
        out.write((char*)(&t), sizeof(int));
        for(int i=0; i<(int)tables.size(); ++i) {
            tables[i].print(out);
        }
    }
};



#endif // METADATA_H
