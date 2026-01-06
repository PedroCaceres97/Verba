cmake_minimum_required(VERSION 3.15)

project(verba LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(dependencies.cmake)

# -----------------------------
# Executable
# -----------------------------

add_executable(verba
    src/blop.c
    src/ai.c
    src/log.c
    src/mem.c
    src/tui.c
    src/main.c
    ${CJSON_LIBRARY_DIR}/cJSON.c
)

target_include_directories(verba PRIVATE
    src
    ${CURL_INCLUDE_DIR}
    ${CJSON_INCLUDE_DIR}
)

target_compile_options(verba PRIVATE
    -Wall
    -Wextra
    -Werror=implicit-function-declaration
)

target_link_directories(verba PRIVATE
    ${CURL_ROOT}/lib
)

target_link_libraries(verba PRIVATE
    curl
)

add_custom_command(TARGET verba POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:verba>
        ${CMAKE_SOURCE_DIR}/bin/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CURL_ROOT}/bin/libcurl-x64.dll
        ${CMAKE_SOURCE_DIR}/bin/
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
