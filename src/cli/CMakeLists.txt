# CLI CMake Configuration
# NLM Command Line Interface

cmake_minimum_required(VERSION 3.16)

# CLI source files
set(CLI_SOURCES
    src/cli/NLMCLI.cpp
)

# Create CLI executable
add_executable(nlm_cli ${CLI_SOURCES})

# Include directories
target_include_directories(nlm_cli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cli
)

# Link libraries
target_link_libraries(nlm_cli PRIVATE nlm_core)

# Set compiler features
target_compile_features(nlm_cli PRIVATE cxx_std_20)

# Install target (future)
install(TARGETS nlm_cli
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

# Add CLI to tests (optional)
if(BUILD_TESTS)
    enable_testing()
    add_test(NAME nlm_cli
        COMMAND nlm_cli --help
    )
endif()
