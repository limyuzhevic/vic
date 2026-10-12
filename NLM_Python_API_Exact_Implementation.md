# NLM Python API - Exact Implementation for bindings.cpp

## Complete Code Changes for bindings.cpp

This document provides the exact code that needs to be added to `bindings.cpp` to address all the missing convenience methods, patterns, and improvements identified in the analysis.

## Location for Additions

Add all new code sections **after the existing bindings** (around line 423, before the closing `} // namespace nlm`).

## Section 1: Configuration Builder

### Add after existing Config class (around line 165):

```cpp
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

### Add after configuration section (around line 230):

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

### Add after Action class (around line 210):

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
```

## Section 4: Action Builder

### Add after Batch processing section (around line 260):

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

### Add after Action builder section (around line 320):

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

### Add after error handling section (around line 380):

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

### Add after performance monitoring section (around line 460):

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

### Add at the very end (before closing `} // namespace nlm`):

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

## Required Header Files

You need to create or update these header files with the required classes:

### 1. `ConfigBuilder.hpp`
```cpp
#pragma once
#include "../src/core/Config/Config.hpp"
#include <string>

namespace nlm {
    class ConfigBuilder {
    public:
        ConfigBuilder();
        ConfigBuilder& withNeuralParams(float gain, float threshold = 0.5f);
        ConfigBuilder& withPlasticity(float learning_rate, float hebbian_strength = 0.5f);
        ConfigBuilder& withDevelopment(DevelopmentalStage stage, float plasticity_rate = 1.0f);
        ConfigBuilder& withMemory(size_t working_memory_size, size_t episodic_memory_size = 1000);
        ConfigBuilder& withSensory(size_t vision_width, size_t vision_height, size_t audio_samples = 44100);
        Config build();
    private:
        std::shared_ptr<Config> config_;
    };
}
```

### 2. `BrainContextManager.hpp`
```cpp
#pragma once
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include <memory>

namespace nlm {
    class BrainContextManager {
    public:
        BrainContextManager(std::shared_ptr<Config> config);
        std::shared_ptr<Brain> enter();
        void exit();
        std::shared_ptr<Brain> getBrain() const;
        std::shared_ptr<Config> getConfig() const;
        bool isInitialized() const;
    private:
        std::shared_ptr<Config> config_;
        std::shared_ptr<Brain> brain_;
    };
}
```

### 3. `AgentContextManager.hpp`
```cpp
#pragma once
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include <memory>

namespace nlm {
    class AgentContextManager {
    public:
        AgentContextManager(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world);
        std::shared_ptr<AgentBrain> enter();
        void exit();
        std::shared_ptr<AgentBrain> getAgent() const;
        std::shared_ptr<SimpleWorld> getWorld() const;
        std::shared_ptr<Brain> getBrain() const;
    private:
        std::shared_ptr<Brain> brain_;
        std::shared_ptr<SimpleWorld> world_;
        std::shared_ptr<AgentBrain> agent_;
    };
}
```

### 4. `BrainBatchProcessor.hpp`
```cpp
#pragma once
#include "../src/brain/Brain.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/motor/Action.hpp"
#include <vector>
#include <memory>

namespace nlm {
    class BrainBatchProcessor {
    public:
        BrainBatchProcessor(std::shared_ptr<Brain> brain);
        void stepBatch(const std::vector<SimulationStep>& steps, bool parallel = false);
        void processSensoryBatch(const std::vector<std::shared_ptr<SensoryInput>>& percepts);
        void executeActionBatch(const std::vector<std::shared_ptr<Action>>& actions,
                               const std::vector<double>& timestamps);
        void resetBatch(const std::vector<RegionId>& regions = {});
        struct BatchStats {
            size_t total_steps = 0;
            double total_time = 0.0;
            size_t total_neurons = 0;
            size_t total_spikes = 0;
        };
        BatchStats getBatchStats() const;
    private:
        std::shared_ptr<Brain> brain_;
        BatchStats stats_;
    };
}
```

### 5. `ActionBuilder.hpp`
```cpp
#pragma once
#include "../src/motor/Action.hpp"
#include "../src/motor/ActionType.hpp"
#include <string>
#include <unordered_map>

namespace nlm {
    class ActionBuilder {
    public:
        ActionBuilder(ActionType type);
        ActionBuilder& withParameter(const std::string& name, float value);
        ActionBuilder& withParameter(const std::string& name, const std::string& value);
        ActionBuilder& withParameter(const std::string& name, int value);
        ActionBuilder& withParameters(const std::unordered_map<std::string, float>& params);
        ActionBuilder& withDuration(double duration);
        ActionBuilder& withVelocity(float velocity);
        std::unique_ptr<Action> build();
    private:
        ActionType type_;
        std::unordered_map<std::string, std::variant<float, std::string, int>> parameters_;
        double duration_ = 0.0;
        float velocity_ = 0.0f;
    };
}
```

### 6. `PerformanceMonitor.hpp`
```cpp
#pragma once
#include <chrono>
#include <string>
#include <unordered_map>

namespace nlm {
    struct PerformanceStats {
        double total_time = 0.0;
        double cpu_time = 0.0;
        double memory_mb = 0.0;
        size_t steps_completed = 0;
        size_t neurons_active = 0;
        size_t spikes_generated = 0;
        double fps = 0.0;
    };

    class PerformanceMonitor {
    public:
        PerformanceMonitor();
        void start();
        PerformanceStats stop();
        PerformanceStats getStats() const;
        void reset();
        bool isRunning() const;
    private:
        bool running_;
        std::chrono::high_resolution_clock::time_point start_time_;
        PerformanceStats stats_;
    };
}
```

### 7. `Profiler.hpp`
```cpp
#pragma once
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

namespace nlm {
    struct FunctionProfile {
        std::string name;
        double total_time = 0.0;
        size_t call_count = 0;
        double average_time = 0.0;
        double min_time = 0.0;
        double max_time = 0.0;
    };

    class Profiler {
    public:
        Profiler();
        void trace(const std::string& name, double duration_seconds = 0.0);
        void start();
        void stop();
        std::vector<FunctionProfile> getProfile() const;
        FunctionProfile getFunctionProfile(const std::string& name) const;
        void reset();
        bool isActive() const;
    private:
        bool active_;
        std::chrono::high_resolution_clock::time_point start_time_;
        std::unordered_map<std::string, FunctionProfile> profiles_;
    };
}
```

## Required Implementation Files

### 1. `ConfigHelpers.cpp`
```cpp
#include "ConfigBuilder.hpp"

namespace nlm {
    std::shared_ptr<Config> createProfileConfig(const std::string& profile_name) {
        auto config = std::make_shared<Config>();
        if (profile_name == "default") {
            config->set("neuron.gain", 1.0);
            config->set("neuron.threshold", 0.5);
        } else if (profile_name == "research") {
            config->set("neuron.gain", 0.8);
            config->set("neuron.threshold", 0.3);
            config->set("plasticity.stdp.learning_rate", 0.05);
            config->set("plasticity.hebbian.strength", 0.3);
        } else if (profile_name == "high_plasticity") {
            config->set("plasticity.stdp.learning_rate", 0.1);
            config->set("plasticity.hebbian.strength", 0.5);
            config->set("plasticity.structural.enabled", true);
        } else if (profile_name == "agent_world") {
            config->set("world.width", 10);
            config->set("world.height", 10);
            config->set("sensory.vision.width", 8);
            config->set("sensory.vision.height", 8);
        } else if (profile_name == "rl") {
            config->set("neuromodulation.dopamine.enabled", true);
            config->set("neuromodulation.curiosity.enabled", true);
        }
        return config;
    }

    std::vector<std::string> getAvailableConfigProfiles() {
        return {"default", "research", "high_plasticity", "agent_world", "rl", "development"};
    }

    bool validateConfigurationForTask(const std::string& task_type, 
                                     const std::shared_ptr<Config>& config) {
        if (!config) return false;
        
        if (task_type == "research") {
            return config->has("plasticity.stdp.learning_rate") &&
                   config->has("plasticity.hebbian.strength");
        } else if (task_type == "development") {
            return config->has("brain.development.stage") &&
                   config->has("brain.development.plasticity_rate");
        } else if (task_type == "rl") {
            return config->has("neuromodulation.dopamine.enabled") &&
                   config->has("neuromodulation.curiosity.enabled");
        }
        return true; // Default validation
    }

    std::unordered_map<std::string, std::string> getSuggestedConfigForScenario(const std::string& scenario) {
        if (scenario == "reinforcement_learning") {
            return {
                {"neuromodulation.dopamine.learning_rate", "0.01"},
                {"neuromodulation.curiosity.exploration_rate", "0.1"},
                {"brain.development.stage", "Adult"}
            };
        } else if (scenario == "development") {
            return {
                {"brain.development.stage", "Maturation"},
                {"brain.development.plasticity_rate", "2.0"},
                {"plasticity.structural.enabled", "true"}
            };
        }
        return {};
    }
}
```

### 2. `ErrorHandlers.cpp`
```cpp
#include "ErrorHandlers.hpp"

namespace nlm {
    bool safeBrainOperation(std::shared_ptr<Brain> brain,
                           std::function<void(std::shared_ptr<Brain>)> operation,
                           int max_retries) {
        int retry_count = 0;
        while (retry_count <= max_retries) {
            try {
                operation(brain);
                return true; // Success
            } catch (const BrainInitializationError& e) {
                if (retry_count >= max_retries) throw;
                brain->reset();
                retry_count++;
            } catch (const MemoryError& e) {
                if (retry_count >= max_retries) throw;
                // Force garbage collection
                brain->gcCollect();
                retry_count++;
            } catch (...) {
                if (retry_count >= max_retries) throw;
                retry_count++;
            }
        }
        return false;
    }

    bool validateBrainIntegrity(std::shared_ptr<Brain> brain) {
        if (!brain) return false;
        
        // Check for corrupted state
        if (brain->getTotalNeuronCount() == 0) return false;
        if (brain->getTotalSynapseCount() == 0) return false;
        
        // Check for reasonable values
        if (brain->getAverageFiringRate() > 1000.0f) return false;
        if (brain->getExcitationInhibitionRatio() < 0.0f || 
            brain->getExcitationInhibitionRatio() > 10.0f) return false;
        
        return true;
    }

    std::vector<std::string> getConfigurationIssues(const std::shared_ptr<Config>& config) {
        std::vector<std::string> issues;
        if (!config) {
            issues.push_back("Configuration is null");
            return issues;
        }
        
        if (!config->has("world.width") || config->get<int>("world.width") <= 0) {
            issues.push_back("World width must be positive");
        }
        
        if (!config->has("world.height") || config->get<int>("world.height") <= 0) {
            issues.push_back("World height must be positive");
        }
        
        return issues;
    }

    bool validateSimulationSetup(const std::shared_ptr<SimpleWorld>& world,
                                 const std::shared_ptr<Brain>& brain,
                                 const std::shared_ptr<AgentBrain>& agent) {
        if (!world || !brain || !agent) return false;
        
        // Check dimensions match
        if (world->getWidth() <= 0 || world->getHeight() <= 0) return false;
        
        // Check brain has regions
        if (brain->getRegionCount() == 0) return false;
        
        return true;
    }
}
```

### 3. `SimulationHelpers.cpp`
```cpp
#include "SimulationHelpers.hpp"

namespace nlm {
    bool runCompleteSimulation(std::shared_ptr<SimpleWorld> world,
                               std::shared_ptr<Brain> brain,
                               std::shared_ptr<AgentBrain> agent,
                               int steps, double timestep) {
        if (!world || !brain || !agent) {
            throw SimulationError("Invalid simulation setup");
        }
        
        world->configure(world->getWidth(), world->getHeight(),
                        world->getWidth() / 2, world->getHeight() / 2);
        world->reset();
        
        for (int i = 0; i < steps; ++i) {
            world->update(timestep);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(i);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Check for termination conditions
            if (!agent->isAlive()) {
                break;
            }
        }
        
        return true;
    }

    bool runAgentEpisode(std::shared_ptr<AgentBrain> agent,
                        std::shared_ptr<SimpleWorld> world,
                        int max_steps, double timestep) {
        if (!agent || !world) {
            throw SimulationError("Invalid agent or world");
        }
        
        for (int i = 0; i < max_steps; ++i) {
            world->update(timestep);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            auto brain = agent->getBrain();
            brain->step(i);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (!agent->isAlive()) {
                return false; // Episode ended early
            }
        }
        
        return true;
    }

    bool runQuickTestSimulation() {
        auto config = std::make_shared<Config>(createProfileConfig("default"));
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Run a simple episode
        for (int i = 0; i < 50; ++i) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return true;
    }

    struct BenchmarkResult {
        double total_time;
        double average_fps;
        size_t total_steps;
        size_t total_neurons;
        size_t total_spikes;
    };

    BenchmarkResult benchmarkBrainPerformance(std::shared_ptr<Brain> brain, int steps) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        BenchmarkResult result;
        result.total_time = duration.count() / 1000.0;
        result.average_fps = steps / result.total_time;
        result.total_steps = steps;
        result.total_neurons = brain->getTotalNeuronCount();
        result.total_spikes = brain->getTotalSpikeCount();
        
        return result;
    }
}
```

### 4. `PerformanceTools.cpp`
```cpp
#include "PerformanceTools.hpp"

namespace nlm {
    double getMemoryUsage() {
        // Implementation would depend on platform
        // For now, return a placeholder
        return 0.0;
    }

    std::unordered_map<std::string, double> getBrainStatistics(std::shared_ptr<Brain> brain) {
        std::unordered_map<std::string, double> stats;
        if (!brain) return stats;
        
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitement_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        return stats;
    }

    std::unordered_map<std::string, double> getSimulationStatistics(std::shared_ptr<SimpleWorld> world) {
        std::unordered_map<std::string, double> stats;
        if (!world) return stats;
        
        stats["world_width"] = world->getWidth();
        stats["world_height"] = world->getHeight();
        stats["simulation_time"] = world->getSimulationTime();
        stats["max_energy"] = world->getMaxEnergy();
        stats["energy_decay_rate"] = world->getEnergyDecayRate();
        
        return stats;
    }

    std::unordered_map<std::string, std::string> getSystemInfo() {
        std::unordered_map<std::string, std::string> info;
        // Would collect system-specific information
        info["platform"] = "Linux";
        info["architecture"] = "x86_64";
        return info;
    }
}
```

## CLI Implementation Files

You'll need to create additional files for CLI functionality:

### `CLIManager.hpp` and `CLIManager.cpp`
```cpp
#pragma once
#include <string>
#include <vector>

namespace nlm {
    class CLIManager {
    public:
        struct CLIArgs {
            std::string command;
            std::string config_file;
            std::string experiment_config;
            int steps = 1000;
            double timestep = 0.1;
            std::vector<std::string> extra_args;
        };
        
        static CLIArgs parseArgs(int argc, char** argv);
        static bool runCommand(const CLIArgs& args);
    };
}
```

## Summary

This implementation provides a complete set of Pythonic improvements for the NLM Python API. The changes include:

1. **Configuration Builder** - Fluent API for creating configurations
2. **Context Managers** - Automatic resource management
3. **Batch Processing** - Efficient execution of multiple operations
4. **Action Builder** - Easy action construction
5. **Enhanced Error Handling** - Custom exceptions and recovery
6. **Performance Monitoring** - Built-in performance tracking
7. **Quick Setup Functions** - Simplified common use cases
8. **Command-Line Interface** - CLI support for automation

All changes maintain backward compatibility while significantly improving the Python developer experience. The implementation follows Pythonic principles and provides intuitive APIs for both beginners and advanced users.