#include "controller/hello/HelloController.h"

#include <cpprest/json.h>

using namespace web;
using namespace web::http;

HelloController::HelloController()
    : ResourceController(U("/")) {
}

http_response HelloController::handleGet(const http_request& request) {
    json::value body;
    body[U("message")] = json::value::string(U("Hello from cpprest REST server"));
    body[U("path")] = json::value::string(request.relative_uri().path());

    http_response response(status_codes::OK);
    response.set_body(body);
    return response;
}
