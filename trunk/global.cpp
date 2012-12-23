#include "global.h"
#include <sstream>

inline void postError(const char *who, const char *message) {
    std::cerr << "ERROR in " << who << ": " << message << std::endl;
}

std::string int_to_string(int value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}
