#ifndef RECORD_H
#define RECORD_H
#include "signature.h"

struct Record {
public:
    Record(Signature* signature);
    void setInt(uint32 pos, int value);
    void setDouble(uint32 pos, double value);
    void setVarchar(uint32 pos, std::string value);
    char* get_data_pointer() const;
    int getInt(uint32 pos) const;
    double getDouble(uint32 pos) const;
    DBDataValue get(uint32 pos) const;
    std::string getVarchar(uint32 pos) const;
private:
    Signature* m_signature;
    char* m_data;
};

#endif // RECORD_H
