# Phase 7: Advanced Command-Line Tools

# Build all advanced CLI tools for NLM
# Tools provide enhanced functionality for advanced users

# Brain Configuration Tool
add_executable(nlm-config
    tools/brain_config/main.cpp
)
target_link_libraries(nlm-config PUBLIC nlm_core)

# Batch Simulation Tool
add_executable(nlm-batch
    tools/batch_sim/main.cpp
)
target_link_libraries(nlm-batch PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Analysis Tool
add_executable(nlm-analysis
    tools/analysis/main.cpp
)
target_link_libraries(nlm-analysis PUBLIC nlm_core nlm_agent nlm_world nlm_phase6)

# Visualization Tool
add_executable(nlm-visualize
    tools/visualization/main.cpp
)
target_link_libraries(nlm-visualize PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase5)

# Checkpoint Management Tool
add_executable(nlm-checkpoint
    tools/checkpoint/main.cpp
)
target_link_libraries(nlm-checkpoint PUBLIC nlm_core nlm_agent nlm_world)

# Performance Profiler Tool
add_executable(nlm-profile
    tools/profiler/main.cpp
)
target_link_libraries(nlm-profile PUBLIC nlm_core)

# Install tools
install(TARGETS nlm-config nlm-batch nlm-analysis nlm-visualize nlm-checkpoint nlm-profile
    RUNTIME DESTINATION bin
)