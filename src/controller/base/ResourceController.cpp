#include "controller/base/ResourceController.h"

#include <cpprest/json.h>

using namespace web;
using namespace web::http;

ResourceController::ResourceController(const utility::string_t& path)
    : _path(path) {
}

void ResourceController::registerRoutes(Router& router) {
    router.addRoute(methods::GET, _path,
                    [this](const http_request& request) { return handleGet(request); });
    router.addRoute(methods::POST, _path,
                    [this](const http_request& request) { return handlePost(request); });
    router.addRoute(methods::PUT, _path,
                    [this](const http_request& request) { return handlePut(request); });
    router.addRoute(methods::DEL, _path,
                    [this](const http_request& request) { return handleDelete(request); });
}

http_response ResourceController::handleGet(const http_request&) {
    return methodNotAllowed();
}

http_response ResourceController::handlePost(const http_request&) {
    return methodNotAllowed();
}

http_response ResourceController::handlePut(const http_request&) {
    return methodNotAllowed();
}

http_response ResourceController::handleDelete(const http_request&) {
    return methodNotAllowed();
}

http_response ResourceController::methodNotAllowed() {
    json::value body;
    body[U("error")] = json::value::string(U("method not allowed"));

    http_response response(status_codes::MethodNotAllowed);
    response.set_body(body);
    return response;
}
