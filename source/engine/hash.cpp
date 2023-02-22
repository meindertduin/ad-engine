#include "hash.h"
#include "xxhash64.h"
#include "xxhash32.h"

Hash64::Hash64(const std::string &value) {
    mHash = XXHash64::hash(value.c_str(), value.length(), 347183);
}

Hash32::Hash32(const uint32_t &value) {
    mHash = XXHash32::hash(&value, sizeof(value), 283991);
}
