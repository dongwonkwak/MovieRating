cmake_minimum_required(VERSION 3.20.0)
project(MovieRating)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
#add_definitions(-DOPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl@3")
#set(CMAKE_EXE_LINKER_FLAGS "-L/opt/homebrew/opt/openssl@3/lib")
#set(CMAKE_CXX_FLAGS "-I/opt/homebrew/opt/openssl@3/include")

find_package(Git REQUIRED)

find_package(expected QUIET)
if (NOT expected_FOUND)
    message("expected not found, cloning git repository")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} clone
        https://github.com/TartanLlama/expected
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/extern
    )
    add_subdirectory(extern/expected)
endif()

add_subdirectory(metadata)
add_subdirectory(movie)
add_subdirectory(rating)