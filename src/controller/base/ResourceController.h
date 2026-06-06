#pragma once

#include <cpprest/http_listener.h>

#include "controller/interface/IController.h"

/**
 * @brief 단일 리소스 경로에 묶이는 베이스 컨트롤러.
 *
 * 해당 경로에 GET / POST / PUT / DELETE 를 등록하고 각각을 가상 핸들러로
 * 디스패치한다. 파생 컨트롤러는 지원하는 verb 만 override 하며, override 하지
 * 않은 verb 는 기본적으로 405 Method Not Allowed 를 반환한다.
 */
class ResourceController : public IController {
public:
    explicit ResourceController(const utility::string_t& path);
    ~ResourceController() override = default;

    void registerRoutes(Router& router) override;

protected:
    virtual web::http::http_response handleGet(const web::http::http_request& request);
    virtual web::http::http_response handlePost(const web::http::http_request& request);
    virtual web::http::http_response handlePut(const web::http::http_request& request);
    virtual web::http::http_response handleDelete(const web::http::http_request& request);

    // 헬퍼: 405 Method Not Allowed JSON 응답을 생성한다.
    static web::http::http_response methodNotAllowed();

private:
    utility::string_t _path;
};
