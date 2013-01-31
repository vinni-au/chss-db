#include "parser.hpp"
#include "../buffer/signature.h"
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
    {"where",   Parser::Lex_where},
    {"delete",  Parser::Lex_delete},
    {"index",   Parser::Lex_index},
    {"set",     Parser::Lex_set},
    {"on",      Parser::Lex_on},
    {"asc",     Parser::Lex_asc},
    {"desc",    Parser::Lex_desc},
    {"unique",  Parser::Lex_unique}
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
    case '=':
        symbol = Lex_eq;
        nextch();
        break;
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

    if (symbol == Lex_create)
        return p_create();

    if (symbol == Lex_select)
        return p_select();

    if (symbol == Lex_insert)
        return p_insert();

    if (symbol == Lex_update)
        return p_update();

    if (symbol == Lex_delete)
        return p_delete();

    return 0;
}

InsertQuery* Parser::p_insert() {
    InsertQuery* result = new InsertQuery;
    std::vector<std::string> cols;
    std::vector<DBDataValue> values;

    accept(Lex_insert);
    accept(Lex_into);
    result->m_tablename = ident;
    accept(Lex_Ident);
    accept(Lex_Lparen);

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
    while (symbol == Lex_intconst || symbol == Lex_doubleconst || symbol == Lex_stringconst) {
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

SelectQuery* Parser::p_select() {
    SelectQuery* result = new SelectQuery;

    accept(Lex_select);

    if (symbol == Lex_Star) {
        result->m_allCols = true;
        accept(Lex_Star);
    } else {
        while (symbol == Lex_Ident) {
            result->m_cols.push_back(ident);
            accept(Lex_Ident);
            if (symbol == Lex_Comma)
                accept(Lex_Comma);
            else break;
        }
    }

    accept(Lex_from);
    result->m_tablename = ident;
    accept(Lex_Ident);

    if (symbol == Lex_where) {
        accept(Lex_where);
        result->m_cond.first = ident;
        accept(Lex_Ident);
        accept(Lex_eq);
        result->m_cond.second = p_value();
    }

    if (hasErrors) {
        delete result;
        result = 0;
    }
    return result;
}

DBDataValue Parser::p_value() {
    if (symbol == Lex_intconst) {
        DBDataValue result(intconst);
        accept(Lex_intconst);
        return result;
    }

    if (symbol == Lex_doubleconst) {
        DBDataValue result(doubleconst);
        accept(Lex_doubleconst);
        return result;
    }

    if (symbol == Lex_stringconst) {
        DBDataValue result(stringconst);
        accept(Lex_stringconst);
        return result;
    }

    return DBDataValue();
}

DBDataType Parser::p_type() {
    if (symbol == Lex_int) {
        DBDataType result(DBDataType::INT);
        nextsym();
        return result;
    }

    if (symbol == Lex_double) {
        DBDataType result(DBDataType::DOUBLE);
        nextsym();
        return result;
    }

    if (symbol == Lex_varchar) {
        accept(Lex_varchar);
        accept(Lex_Lparen);
        if (intconst != 0) {
            DBDataType result(DBDataType::VARCHAR, intconst);
            accept(Lex_intconst);
            accept(Lex_Rparen);
            return result;
        }
    }

    return DBDataType();
}

Query* Parser::p_create() {
    accept(Lex_create);

    if (symbol == Lex_table) {
        CreateTableQuery* result = new CreateTableQuery;
        accept(Lex_table);
        result->m_tablename = ident;

        accept(Lex_Ident);
        accept(Lex_Lparen);

        while (symbol == Lex_Ident) {
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
    } else if (symbol == Lex_index || symbol == Lex_unique) {
        bool needUnique = false;
        if (symbol == Lex_unique) {
            accept(Lex_unique);
            needUnique = true;
        }
        accept(Lex_index);
        CreateIndexQuery* result = new CreateIndexQuery;
        result->m_name = ident;
        accept(Lex_Ident);
        accept(Lex_on);
        result->m_tablename = ident;
        accept(Lex_Ident);
        accept(Lex_Lparen);
        while (symbol == Lex_Ident) {
            std::string col = ident;
            accept(Lex_Ident);
            if (symbol == Lex_asc) {
                accept(Lex_asc);
                result->m_cols.push_back(make_pair(col, true));
            } else if (symbol == Lex_desc) {
                accept(Lex_desc);
                result->m_cols.push_back(make_pair(col, false));
            } else
                result->m_cols.push_back(make_pair(col, true));

            if (symbol == Lex_Comma)
                nextsym();
            else break;
        }
        accept(Lex_Rparen);
        accept(Lex_using);
        if (symbol == Lex_btree) {
            accept(Lex_btree);
            result->m_indextype = TREEINDEX;
        } else if (symbol == Lex_hash) {
            accept(Lex_hash);
            result->m_indextype = HASHINDEX;
        }
        result->m_unique = needUnique;

        if (hasErrors) {
            delete result;
            result = 0;
        }
        return result;
    }
    return 0;
}

UpdateQuery* Parser::p_update() {
    UpdateQuery* result = new UpdateQuery;
    accept(Lex_update);

    if (symbol == Lex_table)
        accept(Lex_table);
    result->m_tablename = ident;
    accept(Lex_Ident);
    accept(Lex_set);
    result->m_values = p_valueslist();
    if (symbol == Lex_where) {
        accept(Lex_where);
        if (symbol == Lex_Ident) {
            accept(Lex_Ident);
            result->m_cond.first = ident;
            accept(Lex_eq);
            result->m_cond.second = p_value();
        }
    }
    if (hasErrors) {
        delete result;
        result = 0;
    }
    return result;
}

DeleteQuery* Parser::p_delete() {
    DeleteQuery* result = new DeleteQuery;
    accept(Lex_delete);
    accept(Lex_from);
    result->m_tablename = ident;
    accept(Lex_Ident);
    accept(Lex_where);
    result->m_cond.first = ident;
    accept(Lex_Ident);
    accept(Lex_eq);
    result->m_cond.second = p_value();
    if (hasErrors) {
        delete result;
        result = 0;
    }
    return result;
}

std::vector< std::pair<std::string, DBDataValue> > Parser::p_valueslist() {
    std::vector< std::pair<std::string, DBDataValue> > result;
    while (symbol == Lex_Ident) {
        accept(Lex_Ident);
        std::string name = ident;
        accept(Lex_eq);
        result.push_back(make_pair(name, p_value()));

        if (symbol == Lex_Comma)
            nextsym();
        else break;
    }
    return result;
}

bool Parser::accept(int symbolexpected) {
    if (symbol == symbolexpected) {
        nextsym();
        return true;
    }
    hasErrors = true;
    return false;
}
