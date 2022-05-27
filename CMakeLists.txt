cmake_minimum_required(VERSION 3.5)

project(errorsystem-cmake LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(errorsystem_test "errorsystem test example" ON)

function(errorsystemMain)

    if (NOT HAS_THIRDPARTY_BOOST)
        message(FATAL_ERROR "you need import 'bool-cmake' project from github, cmd `git clone xxx`")
    endif()

    add_library(thirdparty_errorsystem_static src/status.cpp)
    target_include_directories(thirdparty_errorsystem_static PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
    target_link_libraries(thirdparty_errorsystem_static PRIVATE thirdparty_boost)

    add_library(thirdparty_errorsystem INTERFACE)
    target_link_libraries(thirdparty_errorsystem PUBLIC INTERFACE thirdparty_errorsystem_static)
    target_include_directories(thirdparty_errorsystem PUBLIC INTERFACE SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/include/)

endfunction()

function(errorsystemTestMain)
    add_executable(errorsystem_test test/main.cpp)
    target_link_libraries(errorsystem_test thirdparty_errorsystem thirdparty_boost)
endfunction()

errorsystemMain()

if (errorsystem_test)
    errorsystemTestMain()
endif()
