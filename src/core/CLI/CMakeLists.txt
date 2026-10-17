# CLI CMakeLists.txt

# NLM CLI executable
add_executable(nlm_cli
    core/CLI/NLMCLI.cpp
)

target_link_libraries(nlm_cli PUBLIC nlm_core)

# Add dependencies for CLI-specific functionality
target_include_directories(nlm_cli PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Install CLI executable
install(TARGETS nlm_cli
    RUNTIME DESTINATION bin
)

# Create alias for backward compatibility
add_custom_target(nlm_alias ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink nlm_cli $<TARGET_FILE_NAME:nlm_cli>
    DEPENDS nlm_cli
)

# Add CLI configuration files
target_sources(nlm_cli PRIVATE
    configs/default.cfg
    configs/cli.cfg
)
