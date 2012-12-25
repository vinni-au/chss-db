#include "parser.hpp"
#include "../metadata.h"

Parser::ltable Parser::lexems[] = {
    {"select",  Parser::Lex_select},
    {"create",  Parser::Lex_create},
    {"insert",  Parser::Lex_insert},
    {"into",    Parser::Lex_into},
    {"table",   Parser::Lex_table},
    {"values",  Parser::Lex_values},
    {"from",    Parser::Lex_from},
    {"int",     Parser::Lex_int},
    {"double",  Parser::Lex_double},
    {"varchar", Parser::Lex_varchar},
    {"using",   Parser::Lex_using},
    {"btree",   Parser::Lex_btree},
    {"hash",    Parser::Lex_hash},
    {"update",  Parser::Lex_update},
    {"where",   Parser::Lex_where}
};

Parser::Parser(const std::string &source)
    : curpos(0),
      curchar(0),
      intconst(0),
      doubleconst(0.0),
      source(source),
      hasErrors(false)
{
    if (keywordtable.size() == 0) {
        for (int i = 0; i < sizeof(lexems)/sizeof(ltable); ++i)
            keywordtable[lexems[i].lexem] = lexems[i].code;
    }
}

void Parser::nextch() {
    curchar = tolower(source[curpos++]);

    if (curpos >= source.size())
        curchar = -1;
}

void Parser::nextsym() {
    if (curchar == -1)
        cursym = Lex_End;

    while (curchar == ' ' || curchar == '\t')
        nextch();

    if (isalpha(curchar)) {
        ident.clear();
        do {
            ident.push_back(curchar);
            nextch();
        } while (isalnum(curchar));

        cursym = searchKeyword(ident);

        return;
    }

    switch (curchar) {
    case '(':
        cursym = Lex_Lparen;
        nextch();
        break;
    case ')':
        cursym = Lex_Rparen;
        nextch();
        break;
    case ';':
        cursym = Lex_Semicolon;
        nextch();
        break;
    case '*':
        cursym = Lex_Star;
        nextch();
        break;
    case ',':
        cursym = Lex_Comma;
        nextch();
        break;
    default:
        if (isdigit(curchar)) {
            int m = 1;
            do {
                intconst *= m;
                intconst += curchar - '0';
                m *= 10;
                nextch();
            } while (isdigit(curchar));
            cursym = Lex_intconst;
        }
    }

}

int Parser::searchKeyword(const std::string &word) {
    std::map<std::string, int>::iterator it = keywordtable.find(word);

    if (it != keywordtable.end())
        return (*it).second;

    return Lex_Ident;
}

Query* Parser::parse() {
    nextch();
    nextsym();
    if (cursym == Lex_create) {
        nextsym();
        CreateQuery* result = new CreateQuery;

        if (cursym == Lex_table) {
            accept(Lex_table);
            result->m_tablename = ident;

            accept(Lex_Ident);
            accept(Lex_Lparen);

            while (cursym == Lex_Ident &&
                   cursym != Lex_Rparen && cursym != Lex_End) {
                std::pair<std::string, DBDataType> col = make_pair(ident, DBDataType());
                accept(Lex_Ident);
                switch (cursym) {
                case Lex_int:
                    col.second = DBDataType(DBDataType::INT);
                    nextsym();
                    break;
                case Lex_double:
                    col.second = DBDataType(DBDataType::DOUBLE);
                    nextsym();
                    break;
                case Lex_varchar:
                    nextsym();
                    accept(Lex_Lparen);
                    if (intconst != 0)
                        col.second = DBDataType(DBDataType::VARCHAR, intconst);
                    accept(Lex_intconst);
                    accept(Lex_Rparen);
                    break;
                }
                result->m_columns.push_back(col);
                if (cursym == Lex_Comma)
                    nextsym();
            }

            if (hasErrors) {
                delete result;
                result = 0;
            }
            return result;
        }
    } else if (cursym == Lex_select) {
        accept(Lex_select);


        if (cursym == Lex_Star) {
            accept(Lex_Star);
            accept(Lex_from);

        }
    } else if (cursym == Lex_insert) {
        accept(Lex_insert);
        accept(Lex_into);
    }
    return 0;
}

bool Parser::accept(int symbolexpected) {
    if (cursym == symbolexpected) {
        nextsym();
        return true;
    }
    hasErrors = true;
    return false;
}
