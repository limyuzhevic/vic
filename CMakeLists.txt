cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Core library
add_library(nlm_core STATIC
    # Core
    core/Types/Types.cpp
    core/Config/Config.cpp
    core/Random/Random.cpp
    core/Logger/Logger.cpp
    core/SimulationClock/SimulationClock.cpp
    # Advanced Configuration
    advanced_config/core/ConfigSchema.cpp
    advanced_config/core/ConfigValidator.cpp
    advanced_config/core/ConfigGroup.cpp
    advanced_config/core/ConfigTemplate.cpp
    advanced_config/core/ConfigProfile.cpp
    advanced_config/core/ConfigSource.cpp
    advanced_config/core/ConfigCallbacks.cpp
    advanced_config/core/ConfigManager.cpp
    # Brain
    brain/Neuron.cpp
    brain/Synapse.cpp
    brain/NeuralPopulation.cpp
    brain/NeuralRegion.cpp
    brain/Brain.cpp
    # Dynamics
    dynamics/NeuralDynamics.cpp
    dynamics/SpikeSystem.cpp
    # Plasticity
    plasticity/PlasticityRule.cpp
    plasticity/Hebbian.cpp
    plasticity/STDP.cpp
    plasticity/StructuralPlasticity.cpp
    # Development
    development/DevelopmentSystem.cpp
    development/Synaptogenesis.cpp
    development/Pruning.cpp
    development/Maturation.cpp
    # Neuromodulation
    neuromodulation/Neuromodulator.cpp
    neuromodulation/Reward.cpp
    neuromodulation/PredictionError.cpp
    neuromodulation/Novelty.cpp
    neuromodulation/Curiosity.cpp
    # Memory
    memory/WorkingMemory.cpp
    memory/EpisodicMemory.cpp
    memory/SemanticMemory.cpp
    memory/ProceduralMemory.cpp
    memory/AssociativeMemory.cpp
    memory/NeuralWorkingMemory.cpp
    memory/NeuralEpisodicMemory.cpp
    # Cognition
    cognition/ConceptFormation.cpp
    cognition/NeuralPlanner.cpp
    # Sensory
    sensory/SensoryInput.cpp
    sensory/Vision.cpp
    sensory/Audio.cpp
    sensory/InternalSignals.cpp
    # Prediction
    prediction/PredictionSystem.cpp
    prediction/NeuralPrediction.cpp
    # Motor
    motor/MotorSystem.cpp
    # Environment
    environment/Environment.cpp
    environment/Observation.cpp
    environment/Action.cpp
    # Experiments
    experiments/Experiment.cpp
    experiments/ExperimentRunner.cpp
    experiments/Metrics.cpp
    # Experiment Management (Phase 2 Enhanced)
    experiments/IExperiment.cpp
    experiments/ExperimentResult.cpp
    experiments/ExperimentRegistry.cpp
    experiments/ExperimentFactory.cpp
    experiments/ExperimentScheduler.cpp
    experiments/ExperimentComparison.cpp
    experiments/ExperimentManager.cpp
    # Phase 2 Enhanced Experiments
    experiments/BasicConnectivityExperiment.cpp
    experiments/PlasticityExperiment.cpp
    experiments/STDPVerificationExperiment.cpp
    # Phase 6 Integrated Experiments
    experiments/IntegratedBrainExperiment.cpp
    experiments/MemorySystemExperiment.cpp
    experiments/PredictionSystemExperiment.cpp
    experiments/CognitiveEngineExperiment.cpp
    experiments/NeuromodulationExperiment.cpp
    experiments/DevelopmentExperiment.cpp
    experiments/StructuralPlasticityExperiment.cpp
    # Visualization
    visualization/VisualizationInterface.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/advanced_config>
    $<INSTALL_INTERFACE:include>
)

# Visualization library
add_library(nlm_visualization STATIC
    # Visualization components
    visualization/NeuralVisualizer.cpp
    visualization/NetworkTopologyVisualizer.cpp
    visualization/3DNeuralVisualizer.cpp
    visualization/ActivityHeatMapVisualizer.cpp
    visualization/StatisticalChartVisualizer.cpp
    visualization/TimeSeriesVisualizer.cpp
    visualization/ScientificVisualizer.cpp
    visualization/DashboardVisualizer.cpp
)
target_include_directories(nlm_visualization PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/visualization>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/visualization>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(nlm_visualization PUBLIC nlm_core)

# Monitoring library
add_library(nlm_monitoring STATIC
    # Monitoring components
    monitoring/SystemMonitor.cpp
    monitoring/PerformanceMonitor.cpp
    monitoring/ExperimentMonitor.cpp
    monitoring/ResourceMonitor.cpp
    monitoring/NetworkMonitor.cpp
    monitoring/AnomalyDetector.cpp
    monitoring/AlertManager.cpp
    monitoring/MetricCollector.cpp
)
target_include_directories(nlm_monitoring PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/monitoring>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/monitoring>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(nlm_monitoring PUBLIC nlm_core)

# User interface library
add_library(nlm_ui STATIC
    # User interface components
    ui/VisualizationWindow.cpp
    ui/ControlPanel.cpp
    ui/ViewManager.cpp
    ui/LayoutManager.cpp
    ui/ViewExporter.cpp
    ui/InteractionManager.cpp
)
target_include_directories(nlm_ui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/ui>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/ui>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(nlm_ui PUBLIC nlm_core)

# Integration library
add_library(nlm_integration STATIC
    # Integration components
    integration/BrainStateVisualizer.cpp
    integration/ExperimentProgressMonitor.cpp
    integration/ResultVisualizer.cpp
    integration/ComparisonVisualizer.cpp
    integration/ParameterImpactVisualizer.cpp
)
target_include_directories(nlm_integration PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/integration>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/integration>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(nlm_integration PUBLIC nlm_core)

# Agent system (body, sensory percept, agent-brain interface)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    experiments/Phase3Experiment.cpp
)
target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    experiments/Phase4Experiment.cpp
)
target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 5 experiments
add_library(nlm_phase5 STATIC
    experiments/Phase5Experiment.cpp
)
target_include_directories(nlm_phase5 PUBLIC src)
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable
add_executable(nlm
    main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core nlm_visualization nlm_monitoring nlm_ui nlm_integration)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Phase 6 demo executable
add_executable(nlm_phase6_demo
    experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5)

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase3_demo nlm_phase4_demo nlm_phase6_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)