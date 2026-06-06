#pragma once

#include <cpprest/details/basic_types.h>

/**
 * @brief 설정 레이어: 서버 설정(리스닝 주소)을 보관한다.
 */
class ServerConfig {
public:
    ServerConfig();
    explicit ServerConfig(const utility::string_t& address);

    // 커맨드라인 인자로부터 설정을 생성한다(argv[1] = 주소).
    static ServerConfig fromArgs(int argc, char* argv[]);

    const utility::string_t& address() const;

private:
    utility::string_t _address;
};
