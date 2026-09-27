# NLM Experiment Layer - Experimental systems and simulations

# Experiment
add_library(nlm_experiment STATIC
    src/experiments/Experiment.cpp
)

target_include_directories(nlm_experiment PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/experiments>
    $<INSTALL_INTERFACE:include/experiment>
)

# Experiment runner
add_library(nlm_experiment_runner STATIC
    src/experiments/ExperimentRunner.cpp
)

target_include_directories(nlm_experiment_runner PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/experiments>
    $<INSTALL_INTERFACE:include/experiment>
)

# Metrics
add_library(nlm_metrics STATIC
    src/experiments/Metrics.cpp
)

target_include_directories(nlm_metrics PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/experiments>
    $<INSTALL_INTERFACE:include/experiment>
)

# Phase 3 experiment
add_library(nlm_phase3_experiment STATIC
    src/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3_experiment PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/experiments>
    $<INSTALL_INTERFACE:include/experiment>
)

# Phase 3 demo
add_library(nlm_phase3_demo_lib STATIC
    src/experiments/Phase3Demo.cpp
)

ttarget_include_directories(nlm_phase3_demo_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/experiments>
    $<INSTALL_INTERFACE:include/experiment>
)

# Phase 4 experiment
add_library(nlm_phase4_experiment STATIC
    src/experiments/Phase4Experiment.cpp
)

ttarget_include_directories(nlm_phase4_experiment PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/experiments>
    $<INSTALL_INTERFACE:include/experiment>
)

# Phase 4 demo
add_library(nlm_phase4_demo_lib STATIC
    src/experiments/Phase4Demo.cpp
)

ttarget_include_directories(nlm_phase4_demo_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/experiments>
    $<INSTALL_INTERFACE:include/experiment>
)

# Phase 5 experiment
add_library(nlm_phase5_experiment STATIC
    src/experiments/Phase5Experiment.cpp
)

target_include_directories(nlm_phase5_experiment PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/experiments>
    $<INSTALL_INTERFACE:include/experiment>
)

# Phase 6 integrated experiment
add_library(nlm_phase6_integrated_experiment STATIC
    src/experiments/Phase6IntegratedExperiment.cpp
)

target_include_directories(nlm_phase6_integrated_experiment PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/experiments>
    $<INSTALL_INTERFACE:include/experiment>
)

# Phase 6 demo
add_library(nlm_phase6_demo_lib STATIC
    src/experiments/Phase6Demo.cpp
)

target_include_directories(nlm_phase6_demo_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/experiments>
    $<INSTALL_INTERFACE:include/experiment>
)

# Experiment layer interface library
add_library(nlm_experiment INTERFACE)
target_link_libraries(nlm_experiment INTERFACE
    nlm_experiment
    nlm_experiment_runner
    nlm_metrics
    nlm_phase3_experiment
    nlm_phase3_demo_lib
    nlm_phase4_experiment
    nlm_phase4_demo_lib
    nlm_phase5_experiment
    nlm_phase6_integrated_experiment
    nlm_phase6_demo_lib
)

# Experiment layer installation
install(TARGETS nlm_experiment nlm_experiment_runner nlm_metrics nlm_phase3_experiment nlm_phase3_demo_lib nlm_phase4_experiment nlm_phase4_demo_lib nlm_phase5_experiment nlm_phase6_integrated_experiment nlm_phase6_demo_lib
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/experiments
    DESTINATION include/experiment
    FILES_MATCHING PATTERN "*.hpp"
)

# Experiment interface header
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/experiments/include/nlm_experiment.hpp
    DESTINATION include/experiment
)