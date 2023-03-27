cmake_minimum_required(VERSION 3.5)

project(errorsystem-cmake LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(errorsystem_test "errorsystem test example" ON)

function(errorsystemMain)
    add_library(thirdparty_errorsystem_static src/status.cpp)
    target_include_directories(thirdparty_errorsystem_static PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)

    add_library(thirdparty_errorsystem INTERFACE)
    target_link_libraries(thirdparty_errorsystem PUBLIC INTERFACE thirdparty_errorsystem_static)
    target_include_directories(thirdparty_errorsystem PUBLIC INTERFACE SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/include/)
endfunction()

function(errorsystemTestMain)
    add_executable(errorsystem_test test/main.cpp)
    target_link_libraries(errorsystem_test thirdparty_errorsystem)
endfunction()

errorsystemMain()

if(errorsystem_test)
    errorsystemTestMain()
endif()
