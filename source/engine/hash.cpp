#include "hash.h"
#include "xxhash64.h"

Hash64::Hash64(const std::string &value) {
    mHash = XXHash64::hash(value.c_str(), value.length(), 347183);
}


