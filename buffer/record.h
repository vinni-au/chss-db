#ifndef RECORD_H
#define RECORD_H
#include "signature.h"

struct Record {
public:
    Record(uint32 position, Signature* signature);
    void setInt(uint32 pos, int value);
    void setDouble(uint32 pos, double value);
    void setVarchar(uint32 pos, std::string value);
    void set(uint32 pos, DBDataValue value);
    char* get_data_pointer() const;
    int getInt(uint32 pos) const;
    double getDouble(uint32 pos) const;
    uint32 getPosition() const;
    DBDataValue get(uint32 pos) const;
    std::string getVarchar(uint32 pos) const;
private:
    uint32 m_position;
    Signature* m_signature;
    char* m_data;
};

#endif // RECORD_H
