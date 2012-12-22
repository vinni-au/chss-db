#ifndef QUERY_HPP
#define QUERY_HPP

#include <string>

struct Query
{
    Query();

    static Query* parse(std::string query);

private:
};

#endif // QUERY_HPP
