#pragma once

#include "controller/base/ResourceController.h"

/**
 * @brief 루트 인사 엔드포인트("/")를 처리한다. GET 만 지원한다.
 */
class HelloController final : public ResourceController {
public:
    HelloController();

protected:
    web::http::http_response handleGet(const web::http::http_request& request) override;
};
