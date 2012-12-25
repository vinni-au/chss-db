#ifndef QUERY_HPP
#define QUERY_HPP

#include "../global.h"


struct Query {
    enum QueryType {
        Other,
        Create,
        Select,
        Insert
    };

    Query() : m_type(Other) { }

    static Query* parse(std::string const &query);

    QueryType type() const {
        return m_type;
    }

protected:
    QueryType m_type;
};

struct CreateQuery : Query
{
    CreateQuery()
    {   m_type = Create;    }

    const std::string& tablename() const
    {   return m_tablename; }

    const std::vector<std::pair<std::string, DBDataType> >& columns() const
    {   return m_columns;   }

protected:
    std::string m_tablename;
    std::vector<std::pair<std::string, DBDataType> > m_columns;

    friend struct Parser;
};

struct InsertQuery : Query
{
    InsertQuery()
    {   m_type = Insert;    }

    const std::string& tablename() const
    {   return m_tablename; }

    std::vector<std::pair<std::string, DBDataValue> > const& values() const
    {   return m_values;    }

protected:
    std::string m_tablename;
    std::vector<std::pair<std::string, DBDataValue> > m_values;

    friend struct Parser;
};


struct SelectQuety : Query
{
protected:
    std::string m_tablename;
    std::vector<std::string> m_cols;
};

#endif // QUERY_HPP
