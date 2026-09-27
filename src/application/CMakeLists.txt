# NLM Application Layer - Executables and demos

# Main application
add_executable(nlm_main
    src/application/main.cpp
)

target_link_libraries(nlm_main PRIVATE
    nlm_core
    nlm_neural
    nlm_plasticity
    nlm_memory
    nlm_neuromodulation
    nlm_development
    nlm_cognition
    nlm_sensory
    nlm_prediction
    nlm_motor
    nlm_environment
    nlm_experiment
    nlm_agent
)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/application/Phase3Demo.cpp
)

target_link_libraries(nlm_phase3_demo PRIVATE
    nlm_core
    nlm_neural
    nlm_plasticity
    nlm_memory
    nlm_neuromodulation
    nlm_development
    nlm_cognition
    nlm_sensory
    nlm_prediction
    nlm_motor
    nlm_environment
    nlm_experiment
    nlm_agent
    nlm_phase3_experiment
)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/application/Phase4Demo.cpp
)

target_link_libraries(nlm_phase4_demo PRIVATE
    nlm_core
    nlm_neural
    nlm_plasticity
    nlm_memory
    nlm_neuromodulation
    nlm_development
    nlm_cognition
    nlm_sensory
    nlm_prediction
    nlm_motor
    nlm_environment
    nlm_experiment
    nlm_agent
    nlm_phase3_experiment
    nlm_phase4_experiment
)

# Phase 5 demo executable
add_executable(nlm_phase5_demo
    src/application/Phase5Demo.cpp
)

target_link_libraries(nlm_phase5_demo PRIVATE
    nlm_core
    nlm_neural
    nlm_plasticity
    nlm_memory
    nlm_neuromodulation
    nlm_development
    nlm_cognition
    nlm_sensory
    nlm_prediction
    nlm_motor
    nlm_environment
    nlm_experiment
    nlm_agent
    nlm_phase3_experiment
    nlm_phase4_experiment
    nlm_phase5_experiment
)

# Phase 6 demo executable
add_executable(nlm_phase6_demo
    src/application/Phase6Demo.cpp
)

target_link_libraries(nlm_phase6_demo PRIVATE
    nlm_core
    nlm_neural
    nlm_plasticity
    nlm_memory
    nlm_neuromodulation
    nlm_development
    nlm_cognition
    nlm_sensory
    nlm_prediction
    nlm_motor
    nlm_environment
    nlm_experiment
    nlm_agent
    nlm_phase3_experiment
    nlm_phase4_experiment
    nlm_phase5_experiment
    nlm_phase6_integrated_experiment
)

# Benchmarking executable
add_executable(nlm_benchmark
    src/application/benchmark.cpp
)

ttarget_link_libraries(nlm_benchmark PRIVATE
    nlm_core
    nlm_neural
    nlm_plasticity
    nlm_memory
)

# Application installation
install(TARGETS nlm_main nlm_phase3_demo nlm_phase4_demo nlm_phase5_demo nlm_phase6_demo nlm_benchmark
    RUNTIME DESTINATION bin
)

# Application headers
install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/application
    DESTINATION include/application
    FILES_MATCHING PATTERN "*.hpp"
)

# Application interface header
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/application/include/nlm_application.hpp
    DESTINATION include/application
)