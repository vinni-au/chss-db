#ifndef QUERY_HPP
#define QUERY_HPP

#include "../global.h"

enum QueryType {
    Q_Query,
    Q_Create,
    Q_Select
};

struct Query
{
    Query() : m_type(Q_Query)
    {}

    static Query* parse(std::string query);

    QueryType type() const
    {   return m_type;  }

protected:
    QueryType m_type;
};

struct CreateQuery : Query
{
    CreateQuery()
    {
        m_type = Q_Create;
    }

protected:
    std::string m_tablename;
    std::vector<std::pair<std::string, int> > m_columns;
};

#endif // QUERY_HPP
