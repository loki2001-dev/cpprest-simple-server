#pragma once

#include <memory>
#include <vector>

#include <cpprest/http_listener.h>

#include "config/ServerConfig.h"
#include "controller/interface/IController.h"
#include "router/Router.h"

/**
 * @brief 애플리케이션 레이어: config, router, controller 를 조립하고
 *        HTTP 리스너의 수명주기(open / wait / close)를 소유한다.
 *
 * 리스너를 열고, 종료 신호(SIGINT/SIGTERM)가 올 때까지 대기한 뒤 닫는다.
 * main.cpp 는 이 객체를 생성하고 실행하는 역할만 한다.
 */
class Application {
public:
    explicit Application(const ServerConfig& config);

    int run();

private:
    void registerControllers();

    ServerConfig _config;
    Router _router;
    std::vector<std::unique_ptr<IController>> _controllers;
    web::http::experimental::listener::http_listener _listener;
};
