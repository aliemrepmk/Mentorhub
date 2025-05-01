#include "SodiumPasswordHasher.hpp"
#include <sodium.h>
#include <stdexcept>

std::string SodiumPasswordHasher::hash(const std::string& password) {
    char hashed[crypto_pwhash_STRBYTES];
    if (crypto_pwhash_str(
            hashed, password.c_str(), password.length(),
            crypto_pwhash_OPSLIMIT_MODERATE,
            crypto_pwhash_MEMLIMIT_MODERATE) != 0) {
        throw std::runtime_error("Out of memory hashing password");
    }
    return std::string(hashed);
}

bool SodiumPasswordHasher::verify(const std::string& hash, const std::string& password) {
    return crypto_pwhash_str_verify(hash.c_str(), password.c_str(), password.length()) == 0;
}