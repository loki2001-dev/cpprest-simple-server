CMAKE_MINIMUM_REQUIRED(VERSION 3.15)

IF(POLICY CMP0091)
    CMAKE_POLICY(SET CMP0091 NEW)
ENDIF()

PROJECT(cpprest_simple_rest_server)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

# Build date (YYYYMMDD)
STRING(TIMESTAMP BUILD_DATE "%Y%m%d")
ADD_COMPILE_DEFINITIONS(CPPREST_REST_SERVER_VERSION_DATE="${BUILD_DATE}")
MESSAGE(STATUS "Build date: ${BUILD_DATE}")

MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "Project: ${PROJECT_NAME}")

# 3rdparty (cpprest) — produced by 3rdparty/cpprest/build.sh
SET(CPPREST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/cpprest)
SET(CPPREST_INCLUDE_DIR ${CPPREST_DIR}/include)
SET(CPPREST_LIB_DIR ${CPPREST_DIR}/lib)

IF(NOT EXISTS "${CPPREST_INCLUDE_DIR}/cpprest/http_listener.h")
    MESSAGE(FATAL_ERROR
        "cpprest headers not found at ${CPPREST_INCLUDE_DIR}.\n"
        "Build the dependency first: (cd 3rdparty/cpprest && ./build.sh)")
ENDIF()

# cpprest dependencies (Boost / OpenSSL / Threads / zlib)
FIND_PACKAGE(Boost REQUIRED COMPONENTS system thread chrono)
FIND_PACKAGE(OpenSSL REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(Threads REQUIRED)

# SOURCE
SET(SOURCES
        src/main.cpp
        # Application layer
        src/app/Application.cpp
        # Config layer
        src/config/ServerConfig.cpp
        # Router layer
        src/router/Router.cpp
        # Controller layer
        src/controller/base/ResourceController.cpp
        src/controller/hello/HelloController.cpp
)

# EXECUTABLE
ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES})

# INCLUDE
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE
        src
        src/app
        src/config
        src/router
        src/controller
        src/controller/interface
        src/controller/base
        src/controller/hello
        ${CPPREST_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${OPENSSL_INCLUDE_DIR}
)

# LINK
TARGET_LINK_DIRECTORIES(${PROJECT_NAME} PRIVATE
        ${CPPREST_LIB_DIR}
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE
        # 3rdparty (cpprest)
        cpprest
        # cpprest dependencies
        Boost::system
        Boost::thread
        Boost::chrono
        OpenSSL::SSL
        OpenSSL::Crypto
        ZLIB::ZLIB
        Threads::Threads
        # system
        dl
)

# RPATH: resolve libcpprest.so from the executable directory ($ORIGIN)
# and from the 3rdparty cpprest lib dir.
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
        BUILD_RPATH "$ORIGIN:${CPPREST_LIB_DIR}"
        INSTALL_RPATH "$ORIGIN:${CPPREST_LIB_DIR}"
        BUILD_WITH_INSTALL_RPATH FALSE
        INSTALL_RPATH_USE_LINK_PATH TRUE
)

# POST BUILD: copy cpprest shared libraries next to the executable
FILE(GLOB CPPREST_SO_FILES "${CPPREST_LIB_DIR}/libcpprest*.so*")
FOREACH(so ${CPPREST_SO_FILES})
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${so}"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying ${so} to output directory"
    )
ENDFOREACH()

MESSAGE(STATUS "cpprest library directory: ${CPPREST_LIB_DIR}")
