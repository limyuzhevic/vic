# CMake configuration for NLM CLI

# CLI source files
set(CLI_SOURCES
    src/cli/main.cpp
)

# Add CLI executable
add_executable(nlm_cli ${CLI_SOURCES})
target_link_libraries(nlm_cli PUBLIC nlm_core nlm_agent nlm_world)

# Install CLI
install(TARGETS nlm_cli DESTINATION bin)

# Documentation target for CLI
add_custom_target(cli_docs
    COMMAND echo "Generating CLI documentation..."
    COMMAND nlm help > docs/CLI_REFERENCE.md
    COMMENT "Generate CLI reference documentation"
)
