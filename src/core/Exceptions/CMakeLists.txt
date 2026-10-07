# Exceptions library
add_library(nlm_exceptions STATIC
    Exceptions/NLMMExceptions.cpp
)

target_include_directories(nlm_exceptions PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/core/Exceptions>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_exceptions PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)