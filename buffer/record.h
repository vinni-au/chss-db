#ifndef RECORD_H
#define RECORD_H
#include "signature.h"

struct Record {
public:
    Record(Signature* signature);
    void set_data(char* data, uint32 size);
    void setInt(uint32 pos, int value);
    void setDouble(uint32 pos, double value);
    void setVarchar(uint32 pos, std::string value);
    int getInt(uint32 pos);
    double getDouble(uint32 pos);
    std::string getVarchar(uint32 pos);
private:
    char* m_data;
    Signature* m_signature;
};

#endif // RECORD_H
