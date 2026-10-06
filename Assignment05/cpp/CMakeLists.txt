cmake_minimum_required(VERSION 3.16)
project(Assignment05_CPP CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(store_lib src/store.cpp src/transaction.cpp)

add_executable(kv_store src/main.cpp)
target_link_libraries(kv_store PRIVATE store_lib)

enable_testing()
add_executable(store_tests tests/store_tests.cpp)
target_link_libraries(store_tests PRIVATE store_lib)
add_test(NAME store_tests COMMAND store_tests)