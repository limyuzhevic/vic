# Demo help system CMakeLists.txt

# Add the demo help library
add_library(demo_help STATIC
    demo/Help.cpp
)

target_include_directories(demo_help PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_options(demo_help PRIVATE
    -Wall -Wextra -Wpedantic
)