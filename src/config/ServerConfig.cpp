// 자신의 헤더 (가장 먼저)
#include "config/ServerConfig.h"

// 외부 라이브러리 헤더
#include <cpprest/asyncrt_utils.h>

namespace {
    // argv 로 주소가 주어지지 않았을 때 사용하는 기본 리스닝 주소.
    const utility::string_t DEFAULT_LISTEN_ADDRESS = U("http://0.0.0.0:9000/");
}

ServerConfig::ServerConfig()
    : _address(DEFAULT_LISTEN_ADDRESS) {
}

ServerConfig::ServerConfig(const utility::string_t& address)
    : _address(address) {
}

ServerConfig ServerConfig::fromArgs(int argc, char* argv[]) {
    if (argc > 1) {
        return ServerConfig(utility::conversions::to_string_t(argv[1]));
    }
    return ServerConfig();
}

const utility::string_t& ServerConfig::address() const {
    return _address;
}
