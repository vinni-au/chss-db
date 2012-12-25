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
      hasErrors(false),
      negative(false)
{
    if (keywordtable.size() == 0) {
        for (int i = 0; i < sizeof(lexems)/sizeof(ltable); ++i)
            keywordtable[lexems[i].lexem] = lexems[i].code;
    }
}

void Parser::nextch(bool ignoreCase /* = true */) {
    if (ignoreCase)
        curchar = tolower(source[curpos++]);
    else
        curchar = source[curpos++];

    if (curpos >= source.size())
        curchar = -1;
}

void Parser::nextsym() {
    if (curchar == -1)
        symbol = Lex_End;

    while (curchar == ' ' || curchar == '\t')
        nextch();

    if (isalpha(curchar)) {
        ident.clear();
        do {
            ident.push_back(curchar);
            nextch();
        } while (isalnum(curchar) || curchar == '_');

        symbol = searchKeyword(ident);

        return;
    }

    switch (curchar) {
    case '(':
        symbol = Lex_Lparen;
        nextch();
        break;
    case ')':
        symbol = Lex_Rparen;
        nextch();
        break;
    case ';':
        symbol = Lex_Semicolon;
        nextch();
        break;
    case '*':
        symbol = Lex_Star;
        nextch();
        break;
    case ',':
        symbol = Lex_Comma;
        nextch();
        break;
    case '\"':
    case '\'':
    {
        stringconst.clear();
        char closing = curchar;
        nextch(false);
        while (curchar != closing && curchar != -1) {
            stringconst.push_back(curchar);
            nextch(false);
        }
        nextch();
        symbol = Lex_stringconst;
    }   break;
    case '-':
        negative = true;
        nextch();
    default:
        if (isdigit(curchar)) {
            intconst = 0;
            do {
                intconst *= 10;
                intconst += curchar - '0';
                nextch();
            } while (isdigit(curchar));
            if (curchar == '.') {
                doubleconst = intconst;
                nextch();
                double m = 10.0;
                char curdigit = 0;
                while (isdigit(curchar)) {
                    curdigit = curchar - '0';
                    doubleconst += curdigit / m;
                    nextch();
                    m *= 10;
                }
                symbol = Lex_doubleconst;
            } else {
                symbol = Lex_intconst;
            }
            if (negative) {
                intconst = -intconst;
                doubleconst = -doubleconst;
                negative = false;
            }
        } else symbol = Lex_Unknown;
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
    if (symbol == Lex_create) {
        nextsym();
        CreateQuery* result = new CreateQuery;

        if (symbol == Lex_table) {
            accept(Lex_table);
            result->m_tablename = ident;

            accept(Lex_Ident);
            accept(Lex_Lparen);

            while (symbol == Lex_Ident &&
                   symbol != Lex_Rparen && symbol != Lex_End) {
                std::pair<std::string, DBDataType> col = make_pair(ident, DBDataType());
                accept(Lex_Ident);
                switch (symbol) {
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
                if (symbol == Lex_Comma)
                    nextsym();
                else break;
            }

            accept(Lex_Rparen);

            if (hasErrors) {
                delete result;
                result = 0;
            }
            return result;
        }
    } else if (symbol == Lex_select) {
        accept(Lex_select);


        if (symbol == Lex_Star) {
            accept(Lex_Star);
            accept(Lex_from);

        }
    } else if (symbol == Lex_insert) {
        accept(Lex_insert);
        accept(Lex_into);
        InsertQuery* result = new InsertQuery;
        result->m_tablename = ident;
        accept(Lex_Ident);
        accept(Lex_Lparen);
        std::vector<std::string> cols;
        while (symbol == Lex_Ident && symbol != Lex_Rparen && symbol != Lex_End) {
            cols.push_back(ident);
            accept(Lex_Ident);
            if (symbol == Lex_Comma)
                accept(Lex_Comma);
            else break;
        }
        accept(Lex_Rparen);

        accept(Lex_values);
        accept(Lex_Lparen);
        std::vector<DBDataValue> values;
        while ((symbol == Lex_intconst || symbol == Lex_doubleconst || symbol == Lex_stringconst)
               && symbol != Lex_End && symbol != Lex_Rparen) {
            switch(symbol) {
            case Lex_intconst:
                values.push_back(DBDataValue(intconst));
                accept(Lex_intconst);
                break;
            case Lex_doubleconst:
                values.push_back(DBDataValue(doubleconst));
                accept(Lex_doubleconst);
                break;
            case Lex_stringconst:
                values.push_back(DBDataValue(stringconst));
                accept(Lex_stringconst);
                break;
            }

            if (symbol == Lex_Comma)
                nextsym();
            else break;
        }
        accept(Lex_Rparen);

        if (cols.size() != values.size())
            hasErrors = true;
        for (uint32 i = 0; i < cols.size(); ++i)
            result->m_values.push_back(make_pair(cols[i], values[i]));

        if (hasErrors) {
            delete result;
            result = 0;
        }
        return result;
    }
    return 0;
}

bool Parser::accept(int symbolexpected) {
    if (symbol == symbolexpected) {
        nextsym();
        return true;
    }
    hasErrors = true;
    return false;
}