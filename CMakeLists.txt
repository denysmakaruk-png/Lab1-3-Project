cmake_minimum_required(VERSION 3.14)
project(Lab1_3_Project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

add_library(core_structures INTERFACE)
target_include_directories(core_structures INTERFACE include)
target_link_libraries(core_structures INTERFACE nlohmann_json::nlohmann_json)

add_executable(project_app
    src/main.cpp
)
target_link_libraries(project_app PRIVATE core_structures)

if(WIN32)
    add_executable(gui_app WIN32
        src/gui_main.cpp
    )
    target_link_libraries(gui_app PRIVATE core_structures comctl32)
    target_compile_definitions(gui_app PRIVATE UNICODE _UNICODE)
endif()

enable_testing()
add_executable(unit_tests
    tests/test_suite.cpp
)
target_link_libraries(unit_tests PRIVATE core_structures)
add_test(NAME AllTests COMMAND unit_tests)