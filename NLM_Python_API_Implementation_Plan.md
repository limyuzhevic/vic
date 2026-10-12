# NLM Python API Enhancement Implementation Plan

## Implementation Roadmap

This document provides a detailed implementation plan for enhancing the NLM Python API in `bindings.cpp`. It specifies exactly what needs to be added and where.

## Phase 1: Core Pythonic Improvements (High Priority)

### 1.1 Add Configuration Builders

#### What to Add:
- Factory functions for common configuration scenarios
- Configuration template system
- Fluent configuration builder class

#### Implementation:
```cpp
// Add to bindings.cpp after existing Config class

// Configuration builders
m.def("createConfigProfile", [](const std::string& profile_name) {
    return std::make_shared<Config>(createProfileConfig(profile_name));
}, py::arg("profile_name"), "Create configuration from predefined profile");

m.def("createConfigProfile", [](const std::string& profile_name, const py::dict& custom_settings) {
    auto config = createProfileConfig(profile_name);
    for (auto item : custom_settings) {
        std::string key = item.first.cast<std::string>();
        config.set(key, pyObjectToConfigValue(item.second));
    }
    return config;
}, py::arg("profile_name"), py::arg("custom_settings"), 
          "Create configuration from profile with custom settings");

m.def("validateConfigForTask", [](const std::string& task_type) {
    return validateConfigForTaskType(task_type);
}, py::arg("task_type"), "Validate configuration for specific task type");

m.def("suggestConfigForScenario", [](const std::string& scenario) {
    return getSuggestedConfigForScenario(scenario);
}, py::arg("scenario"), "Get suggested configuration for scenario");

// Configuration template system
py::class_<ConfigTemplate>(m, "ConfigTemplate")
    .def(py::init<>())
    .def("name", &ConfigTemplate::name)
    .def("description", &ConfigTemplate::description)
    .def("parameters", &ConfigTemplate::parameters)
    .def("validate", &ConfigTemplate::validate);

m.def("getConfigTemplates", &getConfigTemplates, "Get all available configuration templates");
m.def("createConfigFromTemplate", [](const std::string& template_name) {
    return std::make_shared<Config>(Config::fromTemplate(template_name));
}, py::arg("template_name"), "Create configuration from template");

// Fluent configuration builder
py::class_<ConfigBuilder>(m, "ConfigBuilder")
    .def(py::init<>())
    .def("with_neural_params", &ConfigBuilder::withNeuralParams,
         py::arg("gain"), py::arg("threshold") = 0.5f)
    .def("with_plasticity", &ConfigBuilder::withPlasticity,
         py::arg("learning_rate"), py::arg("hebbian_strength") = 0.5f)
    .def("with_development", &ConfigBuilder::withDevelopment,
         py::arg("stage"), py::arg("plasticity_rate") = 1.0f)
    .def("with_memory", &ConfigBuilder::withMemory,
         py::arg("working_memory_size"), py::arg("episodic_memory_size") = 1000)
    .def("build", &ConfigBuilder::build, "Build the final configuration");
```

### 1.2 Add Context Managers

#### What to Add:
- Context managers for automatic resource cleanup
- Transaction-like operations
- Session management

#### Implementation:
```cpp
// Context manager for Brain
py::class_<BrainContextManager>(m, "BrainContextManager")
    .def(py::init<std::shared_ptr<Config>>())
    .def("__enter__", &BrainContextManager::enter)
    .def("__exit__", &BrainContextManager::exit)
    .def("config", &BrainContextManager::getConfig)
    .def("brain", &BrainContextManager::getBrain);

m.def("brain_context", [](std::shared_ptr<Config> config) {
    return BrainContextManager(config);
}, py::arg("config") = nullptr, "Create a brain context manager");

// Agent context manager
py::class_<AgentContextManager>(m, "AgentContextManager")
    .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>())
    .def("__enter__", &AgentContextManager::enter)
    .def("__exit__", &AgentContextManager::exit)
    .def("agent", &AgentContextManager::getAgent)
    .def("world", &AgentContextManager::getWorld);

m.def("agent_context", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
    return AgentContextManager(brain, world);
}, py::arg("brain"), py::arg("world"), 
          "Create an agent context manager");

// Transaction support
py::class_<BrainTransaction>(m, "BrainTransaction")
    .def(py::init<std::shared_ptr<Brain>>())
    .def("add_region", &BrainTransaction::addRegion,
         py::arg("name"), py::arg("neuron_type") = NeuronType::Internal)
    .def("add_synapse", &BrainTransaction::addSynapse,
         py::arg("source_region"), py::arg("target_region"),
         py::arg("weight") = 1.0f)
    .def("commit", &BrainTransaction::commit)
    .def("rollback", &BrainTransaction::rollback)
    .def("is_committed", &BrainTransaction::isCommitted);
```

### 1.3 Add Batch Operation Helpers

#### What to Add:
- Batch processing methods for common operations
- Helper classes for batch operations

#### Implementation:
```cpp
// Batch processing class
py::class_<BrainBatchProcessor>(m, "BrainBatchProcessor")
    .def(py::init<std::shared_ptr<Brain>>())
    .def("step_batch", &BrainBatchProcessor::stepBatch,
         py::arg("steps"), py::arg("parallel") = false)
    .def("process_sensory_batch", &BrainBatchProcessor::processSensoryBatch,
         py::arg("percepts"))
    .def("execute_action_batch", &BrainBatchProcessor::executeActionBatch,
         py::arg("actions"), py::arg("timestamps"))
    .def("reset_batch", &BrainBatchProcessor::resetBatch,
         py::arg("regions") = std::vector<RegionId>());

m.def("create_batch_processor", [](std::shared_ptr<Brain> brain) {
    return std::make_shared<BrainBatchProcessor>(brain);
}, py::arg("brain"), "Create a batch processor for brain operations");

// Simple simulation helper
m.def("run_simulation", [](std::shared_ptr<SimpleWorld> world, 
                         std::shared_ptr<Brain> brain,
                         std::shared_ptr<AgentBrain> agent,
                         int steps, double timestep) {
    return runSimulation(world, brain, agent, steps, timestep);
}, py::arg("world"), py::arg("brain"), py::arg("agent"),
          py::arg("steps"), py::arg("timestep") = 0.1,
          "Run a complete simulation with world, brain, and agent");

// Action builder
py::class_<ActionBuilder>(m, "ActionBuilder")
    .def(py::init<ActionType>())
    .def("with_parameter", &ActionBuilder::withParameter,
         py::arg("name"), py::arg("value"))
    .def("with_parameters", &ActionBuilder::withParameters,
         py::arg("parameters"))
    .def("with_duration", &ActionBuilder::withDuration,
         py::arg("duration"))
    .def("build", &ActionBuilder::build);

m.def("action_builder", [](ActionType type) {
    return ActionBuilder(type);
}, py::arg("type"), "Create an action builder for constructing actions");
```

## Phase 2: Enhanced Convenience Methods (Medium Priority)

### 2.1 Add Quick Setup Functions

#### What to Add:
- Quick setup functions for common scenarios
- Factory methods for specialized brain types

#### Implementation:
```cpp
// Quick setup functions
m.def("create_agent_world_setup", [](int width, int height, 
                                    int vision_width, int vision_height,
                                    bool enable_learning) {
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
    
    return std::make_tuple(brain, world, agent);
}, py::arg("width") = 10, py::arg("height") = 10,
          py::arg("vision_width") = 8, py::arg("vision_height") = 8,
          py::arg("enable_learning") = true,
          "Quick setup of agent, brain, and world");

// Specialized brain factories
m.def("create_research_brain", [](int width, int height, int vision_size) {
    auto config = std::make_shared<Config>(Config::fromTemplate("research"));
    config->set("world.width", width);
    config->set("world.height", height);
    config->set("sensory.vision.width", vision_size);
    config->set("sensory.vision.height", vision_size);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    return brain;
}, py::arg("width") = 10, py::arg("height") = 10,
          py::arg("vision_size") = 8,
          "Create a brain optimized for research");

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
          "Create a brain configured for development");
```

### 2.2 Enhanced Error Handling

#### What to Add:
- NLM-specific exception types
- Error recovery helpers
- Validation utilities

#### Implementation:
```cpp
// Custom exception types
py::register_exception<BrainInitializationError>(m, "BrainInitializationError");
py::register_exception<MemoryError>(m, "MemoryError");
py::register_exception<ConfigurationError>(m, "ConfigurationError");
py::register_exception<SimulationError>(m, "SimulationError");

// Error recovery helpers
m.def("safe_brain_operation", [](std::shared_ptr<Brain> brain,
                                std::function<void(std::shared_ptr<Brain>)> operation,
                                int max_retries = 3) {
    return safeBrainOperation(brain, operation, max_retries);
}, py::arg("brain"), py::arg("operation"), py::arg("max_retries") = 3,
          "Safely execute a brain operation with error recovery");

m.def("validate_brain_integrity", [](std::shared_ptr<Brain> brain) {
    return validateBrainIntegrity(brain);
}, py::arg("brain"), "Validate brain integrity and return health status");

// Configuration validation helpers
m.def("validate_config_for_task", [](const std::string& task_type,
                                     std::shared_ptr<Config> config) {
    return validateConfigForTask(task_type, config);
}, py::arg("task_type"), py::arg("config"),
          "Validate configuration for specific task type");

m.def("get_config_issues", [](std::shared_ptr<Config> config) {
    return getConfigurationIssues(config);
}, py::arg("config"), "Get configuration issues and suggestions");
```

### 2.3 Performance Monitoring

#### What to Add:
- Performance monitoring utilities
- Profiling tools
- Resource tracking

#### Implementation:
```cpp
// Performance monitoring
py::class_<PerformanceMonitor>(m, "PerformanceMonitor")
    .def(py::init<>())
    .def("start", &PerformanceMonitor::start)
    .def("stop", &PerformanceMonitor::stop)
    .def("get_stats", &PerformanceMonitor::getStats)
    .def("reset", &PerformanceMonitor::reset);

m.def("create_performance_monitor", []() {
    return std::make_shared<PerformanceMonitor>();
}, "Create a performance monitor");

// Profiling
py::class_<Profiler>(m, "Profiler")
    .def(py::init<>())
    .def("trace", &Profiler::trace,
         py::arg("name"), py::arg("duration_seconds") = 0.0)
    .def("get_profile", &Profiler::getProfile)
    .def("reset", &Profiler::reset);

m.def("create_profiler", []() {
    return std::make_shared<Profiler>();
}, "Create a profiler");

// Resource tracking
m.def("get_memory_usage", &getMemoryUsage, "Get current memory usage in MB");
m.def("get_brain_statistics", &getBrainStatistics,
     py::arg("brain"), "Get brain statistics");
m.def("get_simulation_stats", &getSimulationStatistics,
     py::arg("world"), "Get simulation statistics");
```

## Phase 3: Advanced Features (Low Priority)

### 3.1 Command-Line Interface

#### What to Add:
- Python CLI module
- Command-line argument parsing
- Experiment management

#### Implementation:
```cpp
// CLI module
m.def("parse_cli_args", &parseCLIArgs,
      "Parse command line arguments for NLM");

m.def("create_config_from_cli", &createConfigFromCLI,
      py::arg("argc"), py::arg("argv"),
      "Create configuration from command line arguments");

m.def("run_cli_mode", &runCLIMode,
      "Run NLM in command-line mode");

// Experiment management
py::class_<Experiment>(m, "Experiment")
    .def(py::init<std::string>())
    .def("add_parameter", &Experiment::addParameter,
         py::arg("name"), py::arg("value"))
    .def("run", &Experiment::run)
    .def("save_results", &Experiment::saveResults,
         py::arg("directory"))
    .def("load_results", &Experiment::loadResults,
         py::arg("directory"));
```

### 3.2 Documentation and Examples

#### What to Add:
- Comprehensive API documentation
- Example gallery
- Tutorial materials

#### Implementation:
```cpp
// API documentation generation
m.def("generate_api_docs", &generateAPIDocs,
      "Generate comprehensive API documentation");

m.def("get_example_gallery", &getExampleGallery,
      "Get available examples and tutorials");

m.def("get_tutorial", &getTutorial,
      py::arg("topic"), "Get tutorial for specific topic");
```

## Implementation Notes

### 1. Header File Updates

These functions require implementation in the corresponding header files:

- `ConfigBuilder.hpp`
- `BrainContextManager.hpp`
- `BrainBatchProcessor.hpp`
- `ActionBuilder.hpp`
- `PerformanceMonitor.hpp`
- `Profiler.hpp`

### 2. Helper Functions

Many of these new features will require helper functions implemented in .cpp files:

- `createProfileConfig()` - Create configuration from profile
- `validateConfigForTask()` - Validate configuration for task
- `runSimulation()` - Run complete simulation
- `safeBrainOperation()` - Execute operation with recovery
- `validateBrainIntegrity()` - Validate brain state

### 3. Template Specialization

Some features require template specialization:

```cpp
// Example template specialization for ConfigBuilder
namespace nlm {
    template<typename T>
    class ConfigBuilder {
    public:
        ConfigBuilder& with_neural_params(T gain, T threshold = 0.5f) {
            // Implementation
            return *this;
        }
        // ... other methods
    };
}
```

## Testing and Validation

### 1. Unit Tests

Add comprehensive unit tests for new features:

- Configuration builder tests
- Context manager tests
- Batch processor tests
- Performance monitor tests

### 2. Integration Tests

Test complex scenarios:

- Complete simulation with new helpers
- Error recovery scenarios
- Performance benchmarking

### 3. Example Tests

Validate examples:

- Quick setup examples
- Factory function examples
- Batch processing examples

## Backward Compatibility

All new features are additive and maintain full backward compatibility:

- Existing code continues to work unchanged
- New classes and functions are added to the `nlm` namespace
- Python bindings are extended but not modified for existing classes
- All existing function signatures remain the same

## Migration Path

For users wanting to migrate to new features:

### From Old to New API

```python
# Old way
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(10, 10, 8, 8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# New way (if desired)
brain, world, agent = pynlm.create_agent_world_setup(10, 10, 8, 8)

# Or using context manager
with pynlm.agent_context(brain, world) as agent:
    # Use agent
    pass
```

## Performance Considerations

### 1. Memory Management

- Use smart pointers consistently
- Context managers ensure proper cleanup
- Avoid deep copies of large objects

### 2. Batch Operations

- Batch operations are more efficient than individual calls
- Parallel processing can be enabled for independent operations
- Memory usage is optimized for batch processing

### 3. Profiling and Monitoring

- Performance monitoring adds minimal overhead
- Profiling can be enabled/disabled as needed
- Resource tracking helps identify bottlenecks

## Conclusion

This implementation plan provides a comprehensive roadmap for enhancing the NLM Python API with Pythonic improvements while maintaining full backward compatibility. The phased approach ensures that:

1. **High-priority features** provide immediate value for beginners
2. **Medium-priority features** add advanced capabilities
3. **Low-priority features** provide optional enhancements

The implementation will make NLM much more accessible to Python developers while preserving all existing functionality.