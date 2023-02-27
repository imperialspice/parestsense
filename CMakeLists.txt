cmake_minimum_required(VERSION 3.24)
project(parestSense)

set(CMAKE_CXX_STANDARD 17)
include_directories(deps/toml11)
#include_directories(deps/tomlplusplus/include)
include_directories(deps/sciplot)


add_executable(parestSense main.cpp)
add_executable(parest parest.cpp)
add_executable(test readOutput.cpp)
add_executable(generateGraphs generateGraphs.cpp)

message(${CMAKE_SOURCE_DIR})


## include uuid
include_directories(parestSense PUBLIC deps/stduuid/include/)
include_directories(parestSense PUBLIC deps/stduuid)



target_link_libraries(parestSense uuid)
target_link_libraries(generateGraphs PRIVATE stdc++fs)


set_target_properties(parestSense PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

set_target_properties(parest PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
        )

set_target_properties(test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
        )

set_target_properties(generateGraphs PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
        )