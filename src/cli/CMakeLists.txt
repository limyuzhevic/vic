# CMake configuration for NLM Command Line Interface

# CLI library targets
add_library(nlm_cli STATIC
    NLMCLI.cpp
    NLMCLI_impl.cpp
)

target_include_directories(nlm_cli PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_cli PUBLIC
    nlm_core
    nlm_agent
    nlm_world
)

# CLI executable
add_executable(nlm_master_cli
    main_cli.cpp
)

target_link_libraries(nlm_master_cli PUBLIC
    nlm_cli
)

# Installation targets
install(TARGETS nlm_master_cli
    RUNTIME DESTINATION bin
)

install(TARGETS nlm_cli
    ARCHIVE DESTINATION lib
)
