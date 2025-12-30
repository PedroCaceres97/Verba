cmake_minimum_required(VERSION 3.15)

project(verba LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(dependencies.cmake)

# -----------------------------
# Executable
# -----------------------------

add_executable(verba
    src/ai.c
    src/log.c
    src/mem.c
    src/tui.c
    src/blop.c
    src/main.c
    ${CJSON_LIBRARY_DIR}/cJSON.c
)

target_include_directories(verba PRIVATE
    src
    ${BLOP_INCLUDE_DIR}
    ${CURL_INCLUDE_DIR}
    ${CJSON_INCLUDE_DIR}
)

target_link_directories(verba PRIVATE
    ${CURL_ROOT}/lib
)

target_link_libraries(verba PRIVATE
    curl
)

# -----------------------------
# Windows system libs
# -----------------------------

if (WIN32)
    target_link_libraries(verba PRIVATE
        ws2_32
        crypt32
    )
endif()

install(TARGETS verba DESTINATION bin)
