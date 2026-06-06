#include "app/Application.h"

#include <condition_variable>
#include <csignal>
#include <iostream>
#include <mutex>

#include "controller/hello/HelloController.h"

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

namespace {
    std::condition_variable g_stopCv;
    std::mutex g_stopMutex;
    bool g_stopRequested = false;

    void handleSignal(int) {
        std::lock_guard<std::mutex> lock(g_stopMutex);
        g_stopRequested = true;
        g_stopCv.notify_all();
    }
}

Application::Application(const ServerConfig& config)
    : _config(config)
    , _listener(config.address()) {
}

void Application::registerControllers() {
    _controllers.push_back(std::make_unique<HelloController>());

    for (const auto& controller : _controllers) {
        controller->registerRoutes(_router);
    }
}

int Application::run() {
    registerControllers();
    _router.attachTo(_listener);

    try {
        _listener.open().wait();
    } catch (const std::exception& e) {
        std::cerr << "Error: failed to open listener: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "REST server listening on "
              << utility::conversions::to_utf8string(_listener.uri().to_string())
              << "\nPress Ctrl+C to stop." << std::endl;

    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);
    std::signal(SIGHUP, handleSignal);

    {
        std::unique_lock<std::mutex> lock(g_stopMutex);
        g_stopCv.wait(lock, [] { return g_stopRequested; });
    }

    try {
        _listener.close().wait();
    } catch (const std::exception& e) {
        std::cerr << "Error: failed to close listener: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\nServer stopped." << std::endl;
    return 0;
}
