cmake_minimum_required(VERSION 3.16)

# Test framework
add_library(nlm_test_framework STATIC
    TestFramework.cpp
)

target_include_directories(nlm_test_framework PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_test_framework PUBLIC nlm_core)

target_compile_features(nlm_test_framework PRIVATE cxx_std_20)

# Install test framework
install(TARGETS nlm_test_framework
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY . DESTINATION include/nlm_test)
