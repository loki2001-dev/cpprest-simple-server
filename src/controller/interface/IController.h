#pragma once

#include "router/Router.h"

/**
 * @brief 컨트롤러 인터페이스: 자신의 라우트를 등록하는 도메인 컴포넌트.
 */
class IController {
public:
    virtual ~IController() = default;

    virtual void registerRoutes(Router& router) = 0;
};
