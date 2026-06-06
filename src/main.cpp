// 프로젝트 헤더
#include "app/Application.h"
#include "config/ServerConfig.h"

int main(int argc, char* argv[]) {
    const ServerConfig config = ServerConfig::fromArgs(argc, argv);

    Application app(config);
    return app.run();
}
