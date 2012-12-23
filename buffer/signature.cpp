#include "signature.h"

Signature::Signature(char* str, uint32 size) {
    for(uint32 i = 0; i != size; ++i) {
        m_fields[i] = str[i];
    }
}


char Signature::getField(uint32 pos) {
    return m_fields[pos];
}

bool Signature::isInt(uint32 pos) {
}

bool Signature::isDouble(uint32 pos) {

}

bool Signature::isVarchar(uint32 pos) {

}
