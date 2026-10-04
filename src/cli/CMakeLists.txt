# Find the NLM core library
find_package(Threads REQUIRED)

# Create the CLI library
add_library(nlm_cli src/cli/main.cpp src/cli/CommandLineInterface.cpp)

# Link against NLM core library
add_dependencies(nlm_cli nlm_core)
target_link_libraries(nlm_cli PUBLIC nlm_core)

# Set target properties
set_target_properties(nlm_cli PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    POSITION_INDEPENDENT_CODE ON
)

# Install the CLI library
install(TARGETS nlm_cli
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install the CLI executable
install(TARGETS nlm
    RUNTIME DESTINATION bin
)

# Create a convenience target for users
add_custom_target(nlm_cli_install ALL
    DEPENDS nlm_cli nlm
    COMMENT "Install NLM command line interface"
)
