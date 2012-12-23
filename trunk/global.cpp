#include "global.h"

inline void postError(const char *who, const char *message) {
    std::cerr << "ERROR in " << who << ": " << message << std::endl;
}

