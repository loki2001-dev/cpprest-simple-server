#pragma once

#include <functional>
#include <map>

#include <cpprest/http_listener.h>

using route_handler_t = std::function<web::http::http_response(const web::http::http_request&)>;

/**
 * @brief 라우팅 레이어: (HTTP 메서드, 경로) 쌍을 핸들러에 매핑하고, 들어온
 *        요청을 디스패치하며, cross-cutting 관심사(CORS)를 한 곳에서 적용한다.
 */
class Router {
public:
    void addRoute(const web::http::method& httpMethod, const utility::string_t& path, route_handler_t handler);

    void attachTo(web::http::experimental::listener::http_listener& listener);

private:
    web::http::http_response dispatch(const web::http::method& httpMethod, const web::http::http_request& request);

    std::map<utility::string_t, std::map<utility::string_t, route_handler_t>> _routes;
};
