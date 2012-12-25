#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <map>
#include "query.hpp"

struct Parser
{
    enum Lexem {
        Lex_select,
        Lex_create,
        Lex_insert,
        Lex_into,
        Lex_table,
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
        Lex_intconst,
        Lex_doubleconst,
        Lex_stringconst,
        Lex_asc,
        Lex_desc,
        Lex_Star,       // *
        Lex_Lparen,     // (
        Lex_Rparen,     // )
        Lex_Semicolon,  // ;
        Lex_Comma,      // ,
        Lex_End,
        Lex_Unknown,
        Lex_Ident
    };

    struct ltable {
        std::string lexem;
        int code;
    };

    static ltable lexems[];

    std::map<std::string, int> keywordtable;

    Parser(std::string const& source);
    int searchKeyword(std::string const& word);

    Query* parse();

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
};

#endif // PARSER_HPP
