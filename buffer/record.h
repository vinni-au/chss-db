#ifndef RECORD_H
#define RECORD_H
#include "signature.h"

class Record {
public:
    Record(Signature* signature);
    int getInt(uint32 pos);
    double getDouble(uint32 pos);
    std::string getVarchar(uint32 pos);
private:
    Signature* m_signature;
};

#endif // RECORD_H
