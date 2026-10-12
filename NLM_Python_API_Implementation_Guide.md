# NLM Python API - Practical Implementation Guide

## Complete Implementation Blueprint

This document provides a step-by-step implementation guide for enhancing the NLM Python API in `bindings.cpp`. It shows exactly what code needs to be added to address all the missing areas identified in the analysis.

## Section 1: Configuration Builder

### What to Add:
- Fluent configuration builder class
- Configuration profile system
- Quick configuration helpers

### Implementation Code:

```cpp
// Add this after the existing Config class binding (around line 165)

// Configuration Builder Class
py::class_<ConfigBuilder> configBuilder(m, "ConfigBuilder");
configBuilder
    .def(py::init<>())
    .def("with_neural_params", &ConfigBuilder::withNeuralParams,
         py::arg("gain"), py::arg("threshold") = 0.5f,
         "Set neural parameters (gain and threshold)")
    .def("with_plasticity", &ConfigBuilder::withPlasticity,
         py::arg("learning_rate"), py::arg("hebbian_strength") = 0.5f,
         "Set plasticity parameters (STDP and Hebbian)")
    .def("with_development", &ConfigBuilder::withDevelopment,
         py::arg("stage"), py::arg("plasticity_rate") = 1.0f,
         "Set developmental stage and plasticity rate")
    .def("with_memory", &ConfigBuilder::withMemory,
         py::arg("working_memory_size"), py::arg("episodic_memory_size") = 1000,
         "Configure memory systems")
    .def("with_sensory", &ConfigBuilder::withSensory,
         py::arg("vision_width"), py::arg("vision_height"),
         py::arg("audio_samples") = 44100,
         "Configure sensory systems")
    .def("build", &ConfigBuilder::build, "Build and return the configuration");

// Configuration Profile Functions
m.def("createConfigProfile", [](const std::string& profile_name) {
    return std::make_shared<Config>(createProfileConfig(profile_name));
}, py::arg("profile_name"), "Create configuration from predefined profile");

m.def("listConfigProfiles", []() {
    return getAvailableConfigProfiles();
}, "List available configuration profiles");

m.def("validateConfigForTask", [](const std::string& task_type, 
                                 const std::shared_ptr<Config>& config) {
    return validateConfigurationForTask(task_type, config);
}, py::arg("task_type"), py::arg("config"),
          "Validate configuration for specific task type");

m.def("suggestConfigForScenario", [](const std::string& scenario) {
    return getSuggestedConfigForScenario(scenario);
}, py::arg("scenario"), "Get suggested configuration for scenario");

// Quick Configuration Helpers
m.def("createResearchConfig", [](int width, int height, int vision_size) {
    auto config = std::make_shared<Config>(createProfileConfig("research"));
    config->set("world.width", width);
    config->set("world.height", height);
    config->set("sensory.vision.width", vision_size);
    config->set("sensory.vision.height", vision_size);
    return config;
}, py::arg("width") = 10, py::arg("height") = 10,
          py::arg("vision_size") = 8,
          "Create a configuration optimized for research tasks");

m.def("createDevelopmentConfig", [](DevelopmentalStage stage) {
    auto config = std::make_shared<Config>(createProfileConfig("development"));
    config->set("brain.development.stage", stage);
    config->set("brain.development.plasticity_rate", 2.0f);
    return config;
}, py::arg("stage"), "Create a configuration optimized for development");

m.def("createReinforcementLearningConfig", [](float learning_rate,
                                              float exploration_rate) {
    auto config = std::make_shared<Config>(createProfileConfig("rl"));
    config->set("neuromodulation.dopamine.learning_rate", learning_rate);
    config->set("neuromodulation.curiosity.exploration_rate", exploration_rate);
    return config;
}, py::arg("learning_rate") = 0.01, py::arg("exploration_rate") = 0.1,
          "Create a configuration optimized for reinforcement learning");
```

## Section 2: Context Managers

### What to Add:
- Brain context manager for automatic cleanup
- Agent context manager for agent-world interaction

### Implementation Code:

```cpp
// Brain Context Manager
py::class_<BrainContextManager> brainContextManager(m, "BrainContextManager");
brainContextManager
    .def(py::init<std::shared_ptr<Config>>())
    .def("__enter__", &BrainContextManager::enter)
    .def("__exit__", &BrainContextManager::exit)
    .def("brain", &BrainContextManager::getBrain)
    .def("config", &BrainContextManager::getConfig)
    .def("is_initialized", &BrainContextManager::isInitialized);

// Agent Context Manager
py::class_<AgentContextManager> agentContextManager(m, "AgentContextManager");
agentContextManager
    .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>())
    .def("__enter__", &AgentContextManager::enter)
    .def("__exit__", &AgentContextManager::exit)
    .def("agent", &AgentContextManager::getAgent)
    .def("world", &AgentContextManager::getWorld)
    .def("brain", &AgentContextManager::getBrain);

// Helper Functions
m.def("brain_context", [](std::shared_ptr<Config> config) {
    return BrainContextManager(config);
}, py::arg("config") = nullptr, "Create a brain context manager for automatic cleanup");

m.def("agent_context", [](std::shared_ptr<Brain> brain, 
                         std::shared_ptr<SimpleWorld> world) {
    return AgentContextManager(brain, world);
}, py::arg("brain"), py::arg("world"), 
          "Create an agent context manager for agent-world interaction");
```

## Section 3: Batch Processing

### What to Add:
- Batch processor for efficient multiple operations
- Quick simulation helper
- Batch action executor

### Implementation Code:

```cpp
// Brain Batch Processor
py::class_<BrainBatchProcessor> brainBatchProcessor(m, "BrainBatchProcessor");
brainBatchProcessor
    .def(py::init<std::shared_ptr<Brain>>())
    .def("step_batch", &BrainBatchProcessor::stepBatch,
         py::arg("steps"), py::arg("parallel") = false,
         "Execute multiple simulation steps efficiently")
    .def("process_sensory_batch", &BrainBatchProcessor::processSensoryBatch,
         py::arg("percepts"), "Process multiple sensory inputs")
    .def("execute_action_batch", &BrainBatchProcessor::executeActionBatch,
         py::arg("actions"), py::arg("timestamps"),
         "Execute multiple actions in sequence")
    .def("reset_batch", &BrainBatchProcessor::resetBatch,
         py::arg("regions") = std::vector<RegionId>(),
         "Reset brain regions efficiently")
    .def("get_batch_stats", &BrainBatchProcessor::getBatchStats,
         "Get statistics for batch operations");

// Factory Functions
m.def("create_batch_processor", [](std::shared_ptr<Brain> brain) {
    return std::make_shared<BrainBatchProcessor>(brain);
}, py::arg("brain"), "Create a batch processor for efficient brain operations");

// Quick Simulation Helper
m.def("run_simulation", [](std::shared_ptr<SimpleWorld> world,
                         std::shared_ptr<Brain> brain,
                         std::shared_ptr<AgentBrain> agent,
                         int steps, double timestep) {
    return runCompleteSimulation(world, brain, agent, steps, timestep);
}, py::arg("world"), py::arg("brain"), py::arg("agent"),
          py::arg("steps"), py::arg("timestep") = 0.1,
          "Run a complete simulation with world, brain, and agent");

m.def("run_episode", [](std::shared_ptr<AgentBrain> agent,
                       std::shared_ptr<SimpleWorld> world,
                       int max_steps, double timestep) {
    return runAgentEpisode(agent, world, max_steps, timestep);
}, py::arg("agent"), py::arg("world"),
          py::arg("max_steps"), py::arg("timestep") = 0.1,
          "Run a complete agent episode");

// Batch Action Builder
py::class_<ActionBatchBuilder> actionBatchBuilder(m, "ActionBatchBuilder");
actionBatchBuilder
    .def(py::init<>())
    .def("add_action", &ActionBatchBuilder::addAction,
         py::arg("action"), "Add action to batch")
    .def("add_move_action", &ActionBatchBuilder::addMoveAction,
         py::arg("dx"), py::arg("dy"), "Add move action")
    .def("add_look_action", &ActionBatchBuilder::addLookAction,
         py::arg("angle"), "Add look action")
    .def("execute", &ActionBatchBuilder::execute,
         py::arg("world"), "Execute all actions in batch")
    .def("clear", &ActionBatchBuilder::clear, "Clear batch");

m.def("create_action_batch", []() {
    return std::make_shared<ActionBatchBuilder>();
}, "Create an action batch builder");
```

## Section 4: Action Builder

### What to Add:
- Fluent action builder for easy action construction
- Quick action creation helpers

### Implementation Code:

```cpp
// Action Builder Class
py::class_<ActionBuilder> actionBuilder(m, "ActionBuilder");
actionBuilder
    .def(py::init<ActionType>())
    .def("with_parameter", &ActionBuilder::withParameter,
         py::arg("name"), py::arg("value"),
         "Add a parameter to the action")
    .def("with_parameters", &ActionBuilder::withParameters,
         py::arg("parameters"), "Add multiple parameters")
    .def("with_duration", &ActionBuilder::withDuration,
         py::arg("duration"), "Set action duration")
    .def("with_velocity", &ActionBuilder::withVelocity,
         py::arg("velocity"), "Set action velocity")
    .def("build", &ActionBuilder::build, "Build and return the action");

// Action Factory Functions
m.def("action_builder", [](ActionType type) {
    return ActionBuilder(type);
}, py::arg("type"), "Create an action builder for the specified action type");

// Quick Action Creators
m.def("create_move_action", [](float x, float y) {
    auto action = ActionBuilder(ActionType.MoveForward);
    action.with_parameter("dx", x)
          .with_parameter("dy", y);
    return action.build();
}, py::arg("x"), py::arg("y"), "Create a move action with specified coordinates");

m.def("create_look_action", [](float angle) {
    auto action = ActionBuilder(ActionType.Look);
    action.with_parameter("angle", angle);
    return action.build();
}, py::arg("angle"), "Create a look action with specified angle");

m.def("create_interact_action", [](const std::string& target_type) {
    auto action = ActionBuilder(ActionType.Interact);
    action.with_parameter("target_type", target_type);
    return action.build();
}, py::arg("target_type"), "Create an interact action for specified target type");

m.def("create_custom_action", [](const std::string& name, 
                                 const py::dict& parameters) {
    auto action = ActionBuilder(ActionType.Custom);
    action.with_parameter("name", name);
    for (auto item : parameters) {
        std::string key = item.first.cast<std::string>();
        action.with_parameter(key, item.second);
    }
    return action.build();
}, py::arg("name"), py::arg("parameters"),
          "Create a custom action with parameters");
```

## Section 5: Enhanced Error Handling

### What to Add:
- Custom exception types specific to NLM
- Error recovery helpers
- Validation utilities

### Implementation Code:

```cpp
// Custom Exception Types
py::register_exception<BrainInitializationError>(m, "BrainInitializationError");
py::register_exception<MemoryError>(m, "MemoryError");
py::register_exception<ConfigurationError>(m, "ConfigurationError");
py::register_exception<SimulationError>(m, "SimulationError");
py::register_exception<ValidationError>(m, "ValidationError");

// Error Recovery Helpers
m.def("safe_brain_operation", [](std::shared_ptr<Brain> brain,
                                std::function<void(std::shared_ptr<Brain>)> operation,
                                int max_retries = 3) {
    return safeBrainOperation(brain, operation, max_retries);
}, py::arg("brain"), py::arg("operation"), py::arg("max_retries") = 3,
          "Safely execute brain operation with automatic error recovery");

m.def("validate_brain_integrity", [](std::shared_ptr<Brain> brain) {
    return validateBrainIntegrity(brain);
}, py::arg("brain"), "Validate brain integrity and return health status");

m.def("assert_brain_healthy", [](std::shared_ptr<Brain> brain) {
    if (!brain->isHealthy()) {
        throw BrainInitializationError("Brain is not healthy");
    }
}, py::arg("brain"), "Assert brain health (throws if unhealthy)");

// Configuration Validation
m.def("validate_config_for_task", [](const std::string& task_type,
                                     const std::shared_ptr<Config>& config) {
    return validateConfigurationForTask(task_type, config);
}, py::arg("task_type"), py::arg("config"),
          "Validate configuration for specific task type");

m.def("get_config_issues", [](const std::shared_ptr<Config>& config) {
    return getConfigurationIssues(config);
}, py::arg("config"), "Get configuration issues and suggestions");

// Simulation Validation
m.def("validate_simulation_setup", [](const std::shared_ptr<SimpleWorld>& world,
                                      const std::shared_ptr<Brain>& brain,
                                      const std::shared_ptr<AgentBrain>& agent) {
    return validateSimulationSetup(world, brain, agent);
}, py::arg("world"), py::arg("brain"), py::arg("agent"),
          "Validate simulation setup");
```

## Section 6: Performance Monitoring

### What to Add:
- Performance monitor for tracking execution metrics
- Profiler for detailed performance analysis
- Resource usage tracking

### Implementation Code:

```cpp
// Performance Monitor Class
py::class_<PerformanceMonitor> performanceMonitor(m, "PerformanceMonitor");
performanceMonitor
    .def(py::init<>())
    .def("start", &PerformanceMonitor::start, "Start performance monitoring")
    .def("stop", &PerformanceMonitor::stop,
         "Stop monitoring and return performance statistics")
    .def("get_stats", &PerformanceMonitor::getStats,
         "Get current performance statistics")
    .def("reset", &PerformanceMonitor::reset, "Reset performance statistics")
    .def("is_running", &PerformanceMonitor::isRunning,
         "Check if monitoring is active");

// Profiler Class
py::class_<Profiler> profiler(m, "Profiler");
profiler
    .def(py::init<>())
    .def("trace", &Profiler::trace,
         py::arg("name"), py::arg("duration_seconds") = 0.0,
         "Trace a function execution time")
    .def("start", &Profiler::start, "Start profiling")
    .def("stop", &Profiler::stop, "Stop profiling")
    .def("get_profile", &Profiler::getProfile,
         "Get current profiling data")
    .def("get_function_profile", &Profiler::getFunctionProfile,
         py::arg("name"), "Get profile for specific function")
    .def("reset", &Profiler::reset, "Reset profiler data")
    .def("is_active", &Profiler::isActive, "Check if profiler is active");

// Factory Functions
m.def("create_performance_monitor", []() {
    return std::make_shared<PerformanceMonitor>();
}, "Create a performance monitor");

m.def("create_profiler", []() {
    return std::make_shared<Profiler>();
}, "Create a profiler");

// Utility Functions
m.def("get_memory_usage", &getMemoryUsage, 
      "Get current memory usage in megabytes");
m.def("get_brain_statistics", &getBrainStatistics,
     py::arg("brain"), "Get comprehensive brain statistics");
m.def("get_simulation_stats", &getSimulationStatistics,
     py::arg("world"), "Get simulation statistics");
m.def("get_system_info", &getSystemInfo, "Get system information");

// Context Managers for Performance
py::class_<PerformanceScope> performanceScope(m, "PerformanceScope");
performanceScope
    .def(py::init<std::shared_ptr<PerformanceMonitor>, std::string>())
    .def("__enter__", &PerformanceScope::enter)
    .def("__exit__", &PerformanceScope::exit);

m.def("performance_scope", [](std::shared_ptr<PerformanceMonitor> monitor,
                              const std::string& name) {
    return PerformanceScope(monitor, name);
}, py::arg("monitor"), py::arg("name"),
          "Create a performance scope for timing a code block");
```

## Section 7: Quick Setup Functions

### What to Add:
- Quick setup functions for common scenarios
- Factory functions for specialized brains
- Helper functions for common patterns

### Implementation Code:

```cpp
// Quick Setup Functions
m.def("create_agent_world_setup", [](int width, int height,
                                    int vision_width, int vision_height,
                                    bool enable_learning,
                                    bool enable_plasticity) {
    auto config = std::make_shared<Config>(createProfileConfig("agent_world"));
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width, height, vision_width, vision_height);
    world->reset();
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    if (enable_learning) {
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
    }
    if (enable_plasticity) {
        agent->enableStructuralPlasticity(true);
    }
    
    return std::make_tuple(brain, world, agent);
}, py::arg("width") = 10, py::arg("height") = 10,
          py::arg("vision_width") = 8, py::arg("vision_height") = 8,
          py::arg("enable_learning") = true,
          py::arg("enable_plasticity") = false,
          "Quick setup of complete agent-world-brain system");

// Specialized Brain Factories
m.def("create_research_brain", [](int width, int height, int vision_size) {
    auto config = std::make_shared<Config>(createProfileConfig("research"));
    config->set("world.width", width);
    config->set("world.height", height);
    config->set("sensory.vision.width", vision_size);
    config->set("sensory.vision.height", vision_size);
    config->set("brain.development.stage", DevelopmentalStage::Adult);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    return brain;
}, py::arg("width") = 10, py::arg("height") = 10,
          py::arg("vision_size") = 8,
          "Create a brain optimized for research and experimentation");

m.def("create_development_brain", [](DevelopmentalStage stage,
                                    bool enable_plasticity) {
    auto config = std::make_shared<Config>(createProfileConfig("development"));
    config->set("brain.development.stage", stage);
    config->set("brain.development.plasticity_rate", 2.0f);
    config->set("plasticity.structural.enabled", enable_plasticity);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    return brain;
}, py::arg("stage"), py::arg("enable_plasticity") = true,
          "Create a brain configured for developmental studies");

m.def("create_rl_agent", [](float learning_rate, float exploration_rate) {
    auto config = std::make_shared<Config>(createProfileConfig("rl"));
    config->set("neuromodulation.dopamine.learning_rate", learning_rate);
    config->set("neuromodulation.curiosity.exploration_rate", exploration_rate);
    config->set("neuromodulation.dopamine.enabled", true);
    config->set("neuromodulation.curiosity.enabled", true);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    
    return std::make_tuple(brain, agent);
}, py::arg("learning_rate") = 0.01, py::arg("exploration_rate") = 0.1,
          "Create a reinforcement learning agent");

// Helper Functions
m.def("quick_test_simulation", []() {
    return runQuickTestSimulation();
}, "Run a quick test simulation with default parameters");

m.def("benchmark_brain", [](std::shared_ptr<Brain> brain, int steps) {
    return benchmarkBrainPerformance(brain, steps);
}, py::arg("brain"), py::arg("steps"),
          "Benchmark brain performance over specified steps");
```

## Section 8: Command-Line Interface

### What to Add:
- CLI argument parsing
- Configuration file helpers
- Command-line execution functions

### Implementation Code:

```cpp
// CLI Argument Parsing
m.def("parse_cli_args", &parseCLIArgs,
      "Parse command line arguments for NLM");

m.def("create_config_from_cli", &createConfigFromCLI,
      py::arg("argc"), py::arg("argv"),
      "Create configuration from command line arguments");

// Configuration File Helpers
m.def("load_config_from_file", &loadConfigFromFile,
      py::arg("filepath"), "Load configuration from file");

m.def("save_config_to_file", &saveConfigToFile,
      py::arg("config"), py::arg("filepath"),
      "Save configuration to file");

m.def("export_config_for_analysis", &exportConfigForAnalysis,
      py::arg("config"), py::arg("output_dir"),
      "Export configuration for analysis");

// Command-Line Execution
m.def("run_cli_mode", &runCLIMode,
      "Run NLM in command-line mode");

m.def("run_experiment_cli", &runExperimentCLI,
      py::arg("experiment_config"),
      "Run experiment from configuration file");

// Experiment Management
m.def("create_experiment", &createExperiment,
      py::arg("name"), "Create a new experiment");

m.def("run_experiment", &runExperiment,
      py::arg("experiment"), py::arg("steps"),
      "Run an experiment");
```

## Implementation Order

### Phase 1: Core Infrastructure (Weeks 1-2)
1. Configuration Builder class
2. Context managers
3. Performance monitoring
4. Basic error handling

### Phase 2: Advanced Features (Weeks 3-4)
1. Batch processing
2. Action builder
3. Quick setup functions
4. Command-line interface

### Phase 3: Integration and Testing (Weeks 5-6)
1. Complete integration testing
2. Documentation updates
3. Example gallery
4. User tutorials

## Files to Modify

### Primary Changes:
1. **`bindings.cpp`** - Add all new Python bindings
2. **`ConfigBuilder.hpp`** - Configuration builder implementation
3. **`BrainContextManager.hpp`** - Brain context manager
4. **`AgentContextManager.hpp`** - Agent context manager
5. **`BrainBatchProcessor.hpp`** - Batch processor
6. **`ActionBuilder.hpp`** - Action builder
7. **`PerformanceMonitor.hpp`** - Performance monitor
8. **`Profiler.hpp`** - Profiler

### Helper Files:
1. **`ConfigHelpers.cpp`** - Configuration helper functions
2. **`ErrorHandlers.cpp`** - Error recovery helpers
3. **`SimulationHelpers.cpp`** - Simulation helper functions
4. **`PerformanceTools.cpp`** - Performance monitoring tools

## Testing Strategy

### Unit Tests:
1. Configuration builder tests
2. Context manager tests
3. Batch processor tests
4. Performance monitor tests
5. Action builder tests

### Integration Tests:
1. Complete simulation tests
2. Error recovery tests
3. Performance benchmarking
4. Quick setup tests

### Example Tests:
1. Beginner examples
2. Intermediate examples
3. Advanced examples
4. Real-world scenarios

## Migration Guide

### For Existing Users:
1. All existing code continues to work unchanged
2. New features are available through new classes and functions
3. Examples provided for both old and new approaches

### Migration Examples:

```python
# OLD WAY (still works)
brain = pynlm.createBrain(config)
brain.initialize()

# NEW WAY (if desired)
with pynlm.brain_context(config) as brain:
    # Automatic initialization and cleanup

# OLD WAY
for i in range(1000):
    brain.step(i)

# NEW WAY  
processor = pynlm.create_batch_processor(brain)
processor.step_batch(range(1000))

# OLD WAY
config = pynlm.createDefaultConfig()
config.set("neuron.gain", 0.5)

# NEW WAY
config = pynlm.createConfigProfile("research")
```

## Performance Considerations

### Memory Management:
- Use context managers for automatic resource cleanup
- Batch operations reduce overhead from individual function calls
- Smart pointers ensure proper memory management

### CPU Performance:
- Batch operations enable better optimization opportunities
- Parallel processing available for independent operations
- Performance monitoring helps identify bottlenecks

### Scaling:
- All new features scale linearly with operation count
- Memory usage is optimized for batch operations
- Performance monitoring helps with scaling analysis

## Conclusion

This implementation guide provides a comprehensive blueprint for enhancing the NLM Python API. The changes are structured to:

1. **Improve accessibility** for beginners through simplified setup functions
2. **Enhance productivity** for intermediate users through fluent APIs
3. **Enable advanced usage** for experts through powerful tools
4. **Maintain compatibility** for all existing users
5. **Provide performance benefits** through optimized batch operations

The implementation follows Pythonic principles and will make NLM significantly more accessible and powerful for Python users while preserving all existing functionality.