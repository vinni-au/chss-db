#ifndef METADATA_H
#define METADATA_H

#include "global.h"
#include <string>
#include <vector>


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
};

struct DBMetaData {
private:
    std::string m_name;
    std::vector<Table> tables;

public:
    DBMetaData(std::string db_name): m_name(db_name), tables(vector<Table>()) {}

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
        return table;
    }
};



#endif // METADATA_H
