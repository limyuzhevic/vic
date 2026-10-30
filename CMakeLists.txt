# CLI Framework CMakeLists.txt

# Add CLI framework
add_library(nlm_cli
    src/core/CLI/CLI.cpp
    src/core/CLI/CLI.hpp
)

# Add enhanced Phase 3 demo
add_executable(nlm_phase3_enhanced_demo
    src/experiments/Phase3EnhancedDemo.cpp
)

target_link_libraries(nlm_phase3_enhanced_demo
    nlm_cli
    nlm_core
    nlm_agent
    nlm_world
)

# Install targets
install(TARGETS nlm_phase3_enhanced_demo
    RUNTIME DESTINATION bin
    NAMELINK_COMPONENT_NAME nlm-cli
)

install(TARGETS nlm_cli
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)