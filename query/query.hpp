#ifndef QUERY_HPP
#define QUERY_HPP

#include "../global.h"


struct Query {
    enum QueryType {
        Other,
        Create,
        Select,
        Insert,
        Update,
        Delete,
        CreateIndex
    };

    Query() : m_type(Other) { }

    QueryType type() const {
        return m_type;
    }

protected:
    QueryType m_type;
};

struct CreateTableQuery : Query
{
    CreateTableQuery()
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


struct SelectQuery : Query
{
    SelectQuery()
    {   m_type = Select;    }

    std::string const& tablename() const
    {   return m_tablename; }

    std::vector<std::string> const& columns() const
    {   return m_cols;  }

    std::pair< std::string, DBDataValue > const& condition() const
    {   return m_cond;  }

    bool allColunms() const
    {   return m_allCols;   }

protected:
    std::string m_tablename;
    std::vector<std::string> m_cols;
    std::pair< std::string, DBDataValue > m_cond;
    bool m_allCols;

    friend struct Parser;
};

struct CreateIndexQuery : Query
{
    enum Type {
        BTREE = 0,
        HASH
    };

    CreateIndexQuery() : m_indextype(-1)
    {   m_type = CreateIndex; }

    std::string const& indexname() const
    {   return m_name;  }

    std::string const& tablename() const
    {   return m_tablename; }

    int indextype() const
    {   return m_indextype; }

    bool is_unique() const
    {   return m_unique;    }

    std::vector< std::pair<std::string, bool> > const& cols() const
    {   return m_cols;  }

protected:
    friend struct Parser;

    std::string m_name;
    std::string m_tablename;
    bool m_unique;
    int m_indextype;
    std::vector< std::pair<std::string, bool> > m_cols;
};

struct UpdateQuery : Query
{
    UpdateQuery()
    {   m_type = Update;    }

    std::string const& tablename() const
    {   return m_tablename; }

    std::vector< std::pair<std::string, DBDataValue > > const& values() const
    {   return m_values;    }

    std::pair< std::string, DBDataValue > const& condition() const
    {   return m_cond;  }

protected:
    std::string m_tablename;
    std::vector< std::pair<std::string, DBDataValue > > m_values;
    std::pair< std::string, DBDataValue > m_cond;
    friend struct Parser;
};

struct DeleteQuery : Query
{
    DeleteQuery()
    {   m_type = Delete;    }

    std::string const& tablename() const
    {   return m_tablename; }

    std::pair< std::string, DBDataValue > const& condition() const
    {   return m_cond;  }

protected:
    std::string m_tablename;
    std::pair< std::string, DBDataValue > m_cond;
    friend struct Parser;
};

#endif // QUERY_HPP
