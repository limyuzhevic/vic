cmake_minimum_required(VERSION 3.16)

# Testing
enable_testing()

# Add tests directory
add_subdirectory(demos)

# Add new CLI tests
add_executable(test_cli test_cli.cpp)
target_link_libraries(test_cli nlm_core nlm_agent nlm_world)

add_test(NAME test_cli COMMAND test_cli)
