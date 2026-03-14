#include "utils/PasswordUtil.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <iomanip>

namespace utils {

// 这里可以用 using namespace std; 简化代码（只在 .cc 文件里用）
using namespace std;

string PasswordUtil::generateSalt() {
    basic_string<unsigned char> salt(SALT_LENGTH, 0);
    if (RAND_bytes(salt.data(), SALT_LENGTH) != 1) {
        throw runtime_error("Failed to generate random salt");
    }
    return string(salt.begin(), salt.end());
}

string PasswordUtil::bytesToHex(const unsigned char* data, size_t len) {
    stringstream ss;
    ss << hex << setfill('0');
    for (size_t i = 0; i < len; ++i) {
        ss << setw(2) << static_cast<int>(data[i]);
    }
    return ss.str();
}

basic_string<unsigned char> PasswordUtil::hexToBytes(const string& hex) {
    basic_string<unsigned char> bytes;
    bytes.reserve(hex.length() / 2);
    for (size_t i = 0; i < hex.length(); i += 2) {
        string byte_str = hex.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(stoi(byte_str, nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

PasswordUtil::HashResult PasswordUtil::encrypt(const string& plain_password) {
    string salt = generateSalt();
    basic_string<unsigned char> hash(HASH_LENGTH, 0);

    if (PKCS5_PBKDF2_HMAC(plain_password.c_str(), plain_password.length(),
                          reinterpret_cast<const unsigned char*>(salt.c_str()), SALT_LENGTH,
                          ITERATIONS,
                          EVP_sha256(),
                          HASH_LENGTH, hash.data()) != 1) {
        throw runtime_error("PBKDF2 encryption failed");
    }

    return {
        ITERATIONS,
        bytesToHex(reinterpret_cast<const unsigned char*>(salt.c_str()), SALT_LENGTH),
        bytesToHex(hash.data(), HASH_LENGTH)
    };
}

bool PasswordUtil::verify(const string& plain_password,
                          unsigned int iterations,
                          const string& salt_hex,
                          const string& hash_hex) {
    auto salt_bytes = hexToBytes(salt_hex);
    auto stored_hash = hexToBytes(hash_hex);
    basic_string<unsigned char> computed_hash(HASH_LENGTH, 0);

    if (PKCS5_PBKDF2_HMAC(plain_password.c_str(), plain_password.length(),
                          salt_bytes.data(), salt_bytes.size(),
                          iterations,
                          EVP_sha256(),
                          HASH_LENGTH, computed_hash.data()) != 1) {
        throw runtime_error("PBKDF2 verification failed");
    }

    return CRYPTO_memcmp(computed_hash.data(), stored_hash.data(), HASH_LENGTH) == 0;
}

string PasswordUtil::serialize(const HashResult& result) {
    return to_string(get<0>(result)) + "$" + get<1>(result) + "$" + get<2>(result);
}

auto PasswordUtil::deserialize(const string& stored_str) -> HashResult {
    vector<string> parts;
    stringstream ss(stored_str);
    string part;

    while (getline(ss, part, '$')) {
        parts.push_back(part);
    }

    if (parts.size() != 3) {
        throw runtime_error("Invalid stored password format");
    }

    return {stoul(parts[0]), parts[1], parts[2]};
}

} // namespace utils