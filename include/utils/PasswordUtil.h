#pragma once
#include <string>
#include <tuple>

namespace utils {

class PasswordUtil {
public:
    // 注意：这里全部显式写 std::，不在类里用 using
    using HashResult = std::tuple<unsigned int, std::string, std::string>;

    static HashResult encrypt(const std::string& plain_password);
    static bool verify(const std::string& plain_password,
                       unsigned int iterations,
                       const std::string& salt_hex,
                       const std::string& hash_hex);
    
    static std::string serialize(const HashResult& result);
    static HashResult deserialize(const std::string& stored_str);

private:
    static constexpr int SALT_LENGTH = 16;
    static constexpr int HASH_LENGTH = 32;
    static constexpr unsigned int ITERATIONS = 100000;

    static std::string generateSalt();
    static std::string bytesToHex(const unsigned char* data, size_t len);
    static std::basic_string<unsigned char> hexToBytes(const std::string& hex);
};

} // namespace utils