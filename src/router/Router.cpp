#include "router/Router.h"

#include <utility>

#include <cpprest/json.h>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

namespace {
    // CORS preflight
    void addCorsHeaders(http_response& response) {
        http_headers& headers = response.headers();
        headers.add(U("Access-Control-Allow-Origin"), U("*"));
        headers.add(U("Access-Control-Allow-Methods"), U("GET, POST, PUT, DELETE, OPTIONS"));
        headers.add(U("Access-Control-Allow-Headers"), U("Content-Type, Authorization"));
        headers.add(U("Access-Control-Max-Age"), U("86400"));
    }
}

void Router::addRoute(const method& httpMethod, const utility::string_t& path, route_handler_t handler) {
    _routes[httpMethod][path] = std::move(handler);
}

void Router::attachTo(http_listener& listener) {
    // CORS preflight: 모든 OPTIONS 요청에 허용 헤더로 응답한다.
    listener.support(methods::OPTIONS, [](http_request request) {
        http_response response(status_codes::NoContent);
        addCorsHeaders(response);
        request.reply(response);
    });

    for (const auto& methodEntry : _routes) {
        const method httpMethod = methodEntry.first;
        if (httpMethod == methods::OPTIONS) {
            continue;
        }
        listener.support(httpMethod, [this, httpMethod](http_request request) {
            http_response response = dispatch(httpMethod, request);
            addCorsHeaders(response);
            request.reply(response);
        });
    }
}

http_response Router::dispatch(const method& httpMethod, const http_request& request) {
    const utility::string_t path = request.relative_uri().path();

    const auto methodIt = _routes.find(httpMethod);
    if (methodIt != _routes.end()) {
        const auto pathIt = methodIt->second.find(path);
        if (pathIt != methodIt->second.end()) {
            return pathIt->second(request);
        }
    }

    // 매칭되는 라우트 없음: 404 ^^ㅗㅗ
    json::value body;
    body[U("error")] = json::value::string(U("not found"));
    body[U("path")] = json::value::string(path);

    http_response response(status_codes::NotFound);
    response.set_body(body);
    return response;
}

