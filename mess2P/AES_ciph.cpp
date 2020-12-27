#include <string.h>
#include <ctime>
#include <openssl/aes.h>

#include "AES_ciph.h"

std::string get_key(void)
{
    std::string key;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    key = "";
    for (int i = 0; i < 32; ++i)
        key += alphanum[rand() % (sizeof(alphanum) - 1)];
    return key;
}

int decrypt(std::string ciphertext, const unsigned char *key,
            std::string &plaintext)
{
    AES_KEY aeskey;
    int plaintext_len = 0;

    AES_set_decrypt_key(key, 256, &aeskey);

    plaintext = "";
    while (ciphertext.length() != 0)
    {
        size_t end_pos = (ciphertext.length() >= 16) ? 16U : ciphertext.length();
        std::string tmp_s = ciphertext.substr(0, end_pos);
        char pl[17];
        memset(pl, 0, 17);
        if (end_pos == ciphertext.length())
            ciphertext = "";
        else
            ciphertext = ciphertext.substr(16, ciphertext.length() - 16);

        AES_decrypt((const unsigned char *)(tmp_s.c_str()), (unsigned char *)pl, &aeskey);
        tmp_s = pl;
        plaintext += pl;
        plaintext_len += 16;
    }
    return plaintext_len;
}

int encrypt(std::string plaintext, const unsigned char *key,
            std::string &ciphertext)
{
    AES_KEY aeskey;
    int plaintext_len = 0;

    AES_set_encrypt_key(key, 256, &aeskey);

    ciphertext = "";
    while (plaintext.length() != 0)
    {
        size_t end_pos = (plaintext.length() >= 16) ? 16U : plaintext.length();
        std::string tmp_s = plaintext.substr(0, end_pos);
        char pl[17];
        memset(pl, 0, 17);
        if (end_pos == plaintext.length())
            plaintext = "";
        else
            plaintext = plaintext.substr(16, plaintext.length() - 16);

        AES_encrypt((const unsigned char *)(tmp_s.c_str()), (unsigned char *)pl, &aeskey);
        tmp_s = pl;
        ciphertext += pl;
        plaintext_len += 16;
    }
    return plaintext_len;
}
