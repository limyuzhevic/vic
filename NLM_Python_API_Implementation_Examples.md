# NLM Python API - Practical Implementation Examples

## Example 1: Configuration Builder Implementation

This example shows how to implement a fluent configuration builder for Python.

### C++ Implementation

```cpp
// Add to bindings.cpp

// Configuration builder class
py::class_<ConfigBuilder> configBuilder(m, "ConfigBuilder");
configBuilder
    .def(py::init<>())
    .def("with_neural_params", &ConfigBuilder::withNeuralParams,
         py::arg("gain"), py::arg("threshold") = 0.5f,
         "Configure neural parameters")
    .def("with_plasticity", &ConfigBuilder::withPlasticity,
         py::arg("learning_rate"), py::arg("hebbian_strength") = 0.5f,
         "Configure plasticity parameters")
    .def("with_development", &ConfigBuilder::withDevelopment,
         py::arg("stage"), py::arg("plasticity_rate") = 1.0f,
         "Configure development parameters")
    .def("with_memory", &ConfigBuilder::withMemory,
         py::arg("working_memory_size"), py::arg("episodic_memory_size") = 1000,
         "Configure memory parameters")
    .def("build", &ConfigBuilder::build, "Build and return the configuration");

// Configuration profile functions
m.def("create_config_profile", [](const std::string& profile_name) {
    Config config;
    if (profile_name == "default") {
        config.set("neuron.gain", 1.0);
        config.set("neuron.threshold", 0.5);
    } else if (profile_name == "research") {
        config.set("neuron.gain", 0.8);
        config.set("neuron.threshold", 0.3);
        config.set("plasticity.stdp.learning_rate", 0.05);
        config.set("plasticity.hebbian.strength", 0.3);
    } else if (profile_name == "high_plasticity") {
        config.set("plasticity.stdp.learning_rate", 0.1);
        config.set("plasticity.hebbian.strength", 0.5);
        config.set("plasticity.structural.enabled", true);
    }
    return config;
}, py::arg("profile_name"), "Create configuration from predefined profile");

m.def("list_config_profiles", []() {
    return std::vector<std::string>{"default", "research", "high_plasticity"};
}, "List available configuration profiles");
```

### Usage Example

```python
import pynlm

# Create configuration using fluent builder
config = (pynlm.ConfigBuilder()
          .with_neural_params(gain=0.8, threshold=0.3)
          .with_plasticity(learning_rate=0.05)
          .with_development(stage="Research", plasticity_rate=1.5)
          .build())

# Create from profile
config = pynlm.create_config_profile("research")

# List available profiles
profiles = pynlm.list_config_profiles()
```

## Example 2: Context Manager Implementation

### C++ Implementation

```cpp
// Brain context manager
py::class_<BrainContextManager> brainContextManager(m, "BrainContextManager");
brainContextManager
    .def(py::init<std::shared_ptr<Config>>())
    .def("__enter__", &BrainContextManager::enter)
    .def("__exit__", &BrainContextManager::exit)
    .def("brain", &BrainContextManager::getBrain)
    .def("config", &BrainContextManager::getConfig);

// Agent context manager  
py::class_<AgentContextManager> agentContextManager(m, "AgentContextManager");
agentContextManager
    .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>>())
    .def("__enter__", &AgentContextManager::enter)
    .def("__exit__", &AgentContextManager::exit)
    .def("agent", &AgentContextManager::getAgent)
    .def("world", &AgentContextManager::getWorld);

// Helper functions
m.def("brain_context", [](std::shared_ptr<Config> config) {
    return BrainContextManager(config);
}, py::arg("config") = nullptr, "Create a brain context manager");

m.def("agent_context", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
    return AgentContextManager(brain, world);
}, py::arg("brain"), py::arg("world"), "Create an agent context manager");
```

### Usage Example

```python
import pynlm

# Using brain context manager
with pynlm.brain_context(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    # Do work with brain
    # Context manager automatically cleans up

# Using agent context manager  
world = pynlm.createSimpleWorld()
world.configure(10, 10, 8, 8)
world.reset()
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

with pynlm.agent_context(brain, world) as agent:
    agent.initialize(world)
    # Work with agent
    # Context manager automatically cleans up
```

## Example 3: Batch Processing Implementation

### C++ Implementation

```cpp
// Batch processor class
py::class_<BrainBatchProcessor> brainBatchProcessor(m, "BrainBatchProcessor");
brainBatchProcessor
    .def(py::init<std::shared_ptr<Brain>>())
    .def("step_batch", &BrainBatchProcessor::stepBatch,
         py::arg("steps"), py::arg("parallel") = false,
         "Execute multiple simulation steps")
    .def("process_sensory_batch", &BrainBatchProcessor::processSensoryBatch,
         py::arg("percepts"), "Process multiple sensory inputs")
    .def("execute_action_batch", &BrainBatchProcessor::executeActionBatch,
         py::arg("actions"), py::arg("timestamps"),
         "Execute multiple actions")
    .def("reset_batch", &BrainBatchProcessor::resetBatch,
         py::arg("regions") = std::vector<RegionId>(),
         "Reset brain regions");

// Factory function
m.def("create_batch_processor", [](std::shared_ptr<Brain> brain) {
    return std::make_shared<BrainBatchProcessor>(brain);
}, py::arg("brain"), "Create a batch processor");

// Quick simulation helper
m.def("run_simulation", [](std::shared_ptr<SimpleWorld> world,
                         std::shared_ptr<Brain> brain,
                         std::shared_ptr<AgentBrain> agent,
                         int steps, double timestep) {
    // Implementation details...
    return runCompleteSimulation(world, brain, agent, steps, timestep);
}, py::arg("world"), py::arg("brain"), py::arg("agent"),
          py::arg("steps"), py::arg("timestep") = 0.1,
          "Run a complete simulation with error handling");
```

### Usage Example

```python
import pynlm

# Create batch processor
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
batch_processor = pynlm.create_batch_processor(brain)

# Batch step execution
steps = list(range(100))
batch_processor.step_batch(steps, parallel=False)

# Or using quick simulation helper
def run_agent_episode(world, agent, steps=100):
    pynlm.run_simulation(world, agent.getBrain(), agent, steps)
    return agent.getBrain().getTotalSpikeCount()

# Using sensory batch processing
percepts = [world.getSensoryPercept() for _ in range(10)]
agent.process_sensory_batch(percepts)
```

## Example 4: Action Builder Implementation

### C++ Implementation

```cpp
// Action builder class
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
    .def("build", &ActionBuilder::build, "Build the action");

// Action factory function
m.def("action_builder", [](ActionType type) {
    return ActionBuilder(type);
}, py::arg("type"), "Create an action builder");

// Convenience action functions
m.def("create_move_action", [](float x, float y) {
    auto action = ActionBuilder(ActionType.MoveForward);
    action.with_parameter("dx", x)
          .with_parameter("dy", y);
    return action.build();
}, py::arg("x"), py::arg("y"), "Create a move action");

m.def("create_look_action", [](float angle) {
    auto action = ActionBuilder(ActionType.Look);
    action.with_parameter("angle", angle);
    return action.build();
}, py::arg("angle"), "Create a look action");
```

### Usage Example

```python
import pynlm

# Create action using builder
action = (pynlm.action_builder(pynlm.ActionType.MoveForward)
          .with_parameter("distance", 5.0)
          .with_duration(2.0)
          .build())

# Quick action creation
action = pynlm.create_move_action(10.0, 5.0)
action = pynlm.create_look_action(45.0)

# Action with multiple parameters
action = (pynlm.action_builder(pynlm.ActionType.Custom)
          .with_parameters({
              "power": 0.8,
              "duration": 3.0,
              "target": "enemy"
          })
          .build())
```

## Example 5: Performance Monitoring Implementation

### C++ Implementation

```cpp
// Performance monitor class
py::class_<PerformanceMonitor> performanceMonitor(m, "PerformanceMonitor");
performanceMonitor
    .def(py::init<>())
    .def("start", &PerformanceMonitor::start, "Start monitoring")
    .def("stop", &PerformanceMonitor::stop, "Stop monitoring and return stats")
    .def("get_stats", &PerformanceMonitor::getStats, "Get performance statistics")
    .def("reset", &PerformanceMonitor::reset, "Reset statistics");

// Profiler class
py::class_<Profiler> profiler(m, "Profiler");
profiler
    .def(py::init<>())
    .def("trace", &Profiler::trace,
         py::arg("name"), py::arg("duration_seconds") = 0.0,
         "Trace a function call")
    .def("get_profile", &Profiler::getProfile, "Get profiling data")
    .def("reset", &Profiler::reset, "Reset profiler");

// Factory functions
m.def("create_performance_monitor", []() {
    return std::make_shared<PerformanceMonitor>();
}, "Create a performance monitor");

m.def("create_profiler", []() {
    return std::make_shared<Profiler>();
}, "Create a profiler");

// Utility functions
m.def("get_memory_usage", &getMemoryUsage, "Get current memory usage in MB");
m.def("get_brain_statistics", &getBrainStatistics,
     py::arg("brain"), "Get brain statistics");
```

### Usage Example

```python
import pynlm
import time

# Performance monitoring
perf = pynlm.create_performance_monitor()
perf.start()

# Do work
for i in range(1000):
    brain.step(i)

stats = perf.stop()
print(f"Performance stats: {stats}")
print(f"FPS: {stats.fps}")
print(f"Memory: {stats.memory_mb:.1f} MB")

# Profiling
profiler = pynlm.create_profiler()
with profiler.trace("brain_step", 0.1):
    brain.step(0)

profile_data = profiler.get_profile()

# Quick memory check
memory_mb = pynlm.get_memory_usage()
print(f"Current memory usage: {memory_mb:.1f} MB")

# Brain statistics
brain_stats = pynlm.get_brain_statistics(brain)
print(f"Brain stats: {brain_stats}")
```

## Example 6: Error Handling Enhancement

### C++ Implementation

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
          "Safely execute brain operation with error recovery");

m.def("validate_brain_integrity", [](std::shared_ptr<Brain> brain) {
    return validateBrainIntegrity(brain);
}, py::arg("brain"), "Validate brain integrity");

// Configuration validation
m.def("validate_config_for_task", [](const std::string& task_type,
                                     std::shared_ptr<Config> config) {
    return validateConfigForTask(task_type, config);
}, py::arg("task_type"), py::arg("config"),
          "Validate configuration for task");

// Convenience validation functions
m.def("assert_brain_healthy", [](std::shared_ptr<Brain> brain) {
    if (!brain->isHealthy()) {
        throw BrainInitializationError("Brain is not healthy");
    }
}, py::arg("brain"), "Assert brain health");
```

### Usage Example

```python
import pynlm

# Safe operation with error recovery
def safe_simulation(brain, steps=100):
    try:
        for i in range(steps):
            brain.step(i)
    except pynlm.BrainInitializationError as e:
        print(f"Brain initialization error: {e}")
        brain.reset()
        # Retry
        for i in range(steps):
            brain.step(i)
    except pynlm.MemoryError as e:
        print(f"Memory error: {e}")
        # Force garbage collection
        import gc
        gc.collect()
        # Retry
        for i in range(steps):
            brain.step(i)

# Validate brain integrity
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
if not pynlm.validate_brain_integrity(brain):
    print("Brain integrity check failed!")

# Validate configuration for task
config = pynlm.createDefaultConfig()
if pynlm.validate_config_for_task("reinforcement_learning", config):
    print("Configuration is valid for reinforcement learning")
```

## Example 7: Quick Setup Functions

### C++ Implementation

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

### Usage Example

```python
import pynlm

# Quick setup with defaults
brain, world, agent = pynlm.create_agent_world_setup()

# Quick setup with custom parameters
brain, world, agent = pynlm.create_agent_world_setup(
    width=20, height=20, vision_width=16, vision_height=16,
    enable_learning=True
)

# Create research brain
brain = pynlm.create_research_brain(width=30, height=30, vision_size=16)

# Create development brain
brain = pynlm.create_development_brain(
    stage=pynlm.DevelopmentalStage.Maturation,
    enable_plasticity=True
)
```

## Summary of Implementation Requirements

### Required Changes to bindings.cpp:

1. **Add 7 new classes**:
   - ConfigBuilder
   - BrainContextManager
   - AgentContextManager
   - BrainBatchProcessor
   - ActionBuilder
   - PerformanceMonitor
   - Profiler

2. **Add 12 new functions**:
   - createConfigProfile
   - listConfigProfiles
   - brain_context
   - agent_context
   - create_batch_processor
   - run_simulation
   - action_builder
   - create_move_action
   - create_look_action
   - create_performance_monitor
   - create_profiler
   - get_memory_usage
   - get_brain_statistics

3. **Add 4 custom exception types**:
   - BrainInitializationError
   - MemoryError
   - ConfigurationError
   - SimulationError

4. **Add 4 helper functions**:
   - safe_brain_operation
   - validate_brain_integrity
   - validate_config_for_task
   - assert_brain_healthy

5. **Add 3 quick setup functions**:
   - create_agent_world_setup
   - create_research_brain
   - create_development_brain

### Header File Requirements:

- `ConfigBuilder.hpp` - Configuration builder implementation
- `BrainContextManager.hpp` - Brain context manager
- `AgentContextManager.hpp` - Agent context manager
- `BrainBatchProcessor.hpp` - Batch processor
- `ActionBuilder.hpp` - Action builder
- `PerformanceMonitor.hpp` - Performance monitor
- `Profiler.hpp` - Profiler

### Helper Function Implementation Files:

- `ConfigHelpers.cpp` - Configuration helper functions
- `ErrorHandlers.cpp` - Error recovery helpers
- `SimulationHelpers.cpp` - Simulation helper functions
- `PerformanceTools.cpp` - Performance monitoring tools

This implementation provides a comprehensive set of Pythonic improvements for the NLM Python API while maintaining full backward compatibility. The new features address all the missing areas identified in the analysis.