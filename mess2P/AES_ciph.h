#ifndef AES_CIPH_H
#define AES_CIPH_H

#include <string>

std::string get_key(void);

int decrypt(std::string ciphertext, const unsigned char *key,
            std::string &plaintext);

int encrypt(std::string plaintext, const unsigned char *key,
            std::string &ciphertext);

#endif // AES_CIPH_H
