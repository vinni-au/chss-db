#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <map>
#include "query.hpp"

struct Parser
{
    Parser(std::string const& source);

    Query* parse();

    std::string const& error()
    {   return errormsg;    }

private:
    enum Lexem {
        Lex_select,
        Lex_create,
        Lex_insert,
        Lex_into,
        Lex_table,
        Lex_delete,
        Lex_index,
        Lex_values,
        Lex_from,
        Lex_int,
        Lex_double,
        Lex_varchar,
        Lex_using,
        Lex_btree,
        Lex_hash,
        Lex_update,
        Lex_where,
        Lex_set,
        Lex_on,
        Lex_intconst,
        Lex_doubleconst,
        Lex_stringconst,
        Lex_unique,
        Lex_asc,
        Lex_desc,
        Lex_Star,       // *
        Lex_Lparen,     // (
        Lex_Rparen,     // )
        Lex_Semicolon,  // ;
        Lex_Comma,      // ,
        Lex_eq,         // =
        Lex_End,
        Lex_Unknown,
        Lex_Ident
    };

    struct ltable {
        std::string lexem;
        int code;
    };

    std::string errormsg;

    static ltable lexems[];

    std::map<std::string, int> keywordtable;
    std::map<int, std::string> keywordtable_r;

    int searchKeyword(std::string const& word);

    void nextch(bool ingoreCase = true);
    void nextsym();
    bool accept(int symbolexpected);

    char curchar;
    bool negative;
    int symbol;
    uint32 curpos;
    std::string ident;
    int intconst;
    bool hasErrors;
    double doubleconst;
    std::string stringconst;
    std::string source;

    Query* p_create();
    InsertQuery* p_insert();
    SelectQuery* p_select();
    UpdateQuery* p_update();
    DeleteQuery* p_delete();

    std::vector< std::pair<std::string, DBDataValue> > p_valueslist();
    DBDataValue p_value();
    DBDataType p_type();
};

#endif // PARSER_HPP
