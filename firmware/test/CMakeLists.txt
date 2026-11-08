# Host unit tests for the firmware: the hardware-free modules (Core/Src) compiled with the PC's own compiler
# and tested with Unity (https://github.com/ThrowTheSwitch/Unity), a C test framework made for embedded code.
#
#   cmake -S firmware/test -B build-test
#   cmake --build build-test
#   ctest --test-dir build-test --output-on-failure --output-junit test-results.xml
cmake_minimum_required(VERSION 3.22)
project(CITest_FW_Tests C)

include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.5.2
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(unity)

enable_testing()

set(FW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

# One test executable per module: add_fw_test(<name> <test source> <firmware sources>...)
function(add_fw_test name test_source)
    add_executable(${name} ${test_source} ${ARGN})
    target_include_directories(${name} PRIVATE ${FW_DIR}/Core/Inc)
    target_compile_options(${name} PRIVATE -std=gnu11 -Wall -Wextra -Werror)
    target_link_libraries(${name} PRIVATE unity)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_fw_test(test_blink   test_blink.c   ${FW_DIR}/Core/Src/blink.c)
add_fw_test(test_version test_version.c)
