#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Advanced configuration parameters
struct AdvancedConfig {
    // Performance optimization
    bool enableMemoryPool = true;
    size_t memoryPoolSize = 1024;
    bool enableParallelProcessing = false;
    size_t threadCount = 4;
    
    // Simulation parameters
    double minTimestep = 0.0001;
    double maxTimestep = 0.1;
    double adaptiveTimestep = 0.001;
    
    // Plasticity control
    bool enableSTDP = true;
    bool enableHebbian = true;
    bool enableStructuralPlasticity = true;
    float stdpLearningRate = 0.001;
    float hebbianStrength = 0.01;
    
    // Neuromodulation
    bool enableDopamine = true;
    bool enableCuriosity = true;
    bool enableNovelty = true;
    float dopamineScale = 1.0;
    
    // Memory management
    size_t workingMemoryCapacity = 100;
    size_t episodicMemoryLimit = 10000;
    float memoryConsolidationThreshold = 0.5;
    
    // Development
    bool enableDevelopment = true;
    double developmentSpeed = 1.0;
    size_t developmentalStages = 5;
};

// Advanced Python API enhancements
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Advanced Python Bindings
        ------------------------------------------------------
        Advanced Python bindings for the NLM C++ neural simulation framework.
        Provides enhanced classes and functions for advanced users, researchers,
        and developers requiring more control and customization.
        
        Key Features:
        - Advanced configuration management
        - Performance monitoring and profiling tools
        - Real-time simulation control
        - Custom brain manipulation
        - Experiment management and automation
        - Visualization and analysis utilities
        - Callback and event system
        - Memory management controls
        - Plugin system hooks
        - Extensibility interfaces
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");

    // ====================================================================
    // ADVANCED CONFIGURATION MANAGEMENT
    // ====================================================================
    
    py::class_<AdvancedConfig>(m, "AdvancedConfig", R"pbdoc(
        Advanced configuration parameters for NLM simulation
        
        Provides fine-grained control over simulation behavior,
        performance optimization, and system integration.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("enable_memory_pool", &AdvancedConfig::enableMemoryPool)
        .def_readwrite("memory_pool_size", &AdvancedConfig::memoryPoolSize)
        .def_readwrite("enable_parallel_processing", &AdvancedConfig::enableParallelProcessing)
        .def_readwrite("thread_count", &AdvancedConfig::threadCount)
        .def_readwrite("min_timestep", &AdvancedConfig::minTimestep)
        .def_readwrite("max_timestep", &AdvancedConfig::maxTimestep)
        .def_readwrite("adaptive_timestep", &AdvancedConfig::adaptiveTimestep)
        .def_readwrite("enable_stdp", &AdvancedConfig::enableSTDP)
        .def_readwrite("enable_hebbian", &AdvancedConfig::enableHebbian)
        .def_readwrite("enable_structural_plasticity", &AdvancedConfig::enableStructuralPlasticity)
        .def_readwrite("stdp_learning_rate", &AdvancedConfig::stdpLearningRate)
        .def_readwrite("hebbian_strength", &AdvancedConfig::hebbianStrength)
        .def_readwrite("enable_dopamine", &AdvancedConfig::enableDopamine)
        .def_readwrite("enable_curiosity", &AdvancedConfig::enableCuriosity)
        .def_readwrite("enable_novelty", &AdvancedConfig::enableNovelty)
        .def_readwrite("dopamine_scale", &AdvancedConfig::dopamineScale)
        .def_readwrite("working_memory_capacity", &AdvancedConfig::workingMemoryCapacity)
        .def_readwrite("episodic_memory_limit", &AdvancedConfig::episodicMemoryLimit)
        .def_readwrite("memory_consolidation_threshold", &AdvancedConfig::memoryConsolidationThreshold)
        .def_readwrite("enable_development", &AdvancedConfig::enableDevelopment)
        .def_readwrite("development_speed", &AdvancedConfig::developmentSpeed)
        .def_readwrite("developmental_stages", &AdvancedConfig::developmentalStages)
        .def("apply_to_config", [](AdvancedConfig& self, std::shared_ptr<Config> config) {
            if (config) {
                config->set("brain.enable_memory_pool", self.enableMemoryPool);
                config->set("brain.memory_pool_size", self.memoryPoolSize);
                config->set("brain.enable_parallel", self.enableParallelProcessing);
                config->set("brain.thread_count", self.threadCount);
                config->set("simulation.min_timestep", self.minTimestep);
                config->set("simulation.max_timestep", self.maxTimestep);
                config->set("simulation.adaptive_timestep", self.adaptiveTimestep);
                config->set("plasticity.stdp.enable", self.enableSTDP);
                config->set("plasticity.hebbian.enable", self.enableHebbian);
                config->set("plasticity.structural.enable", self.enableStructuralPlasticity);
                config->set("plasticity.stdp.learning_rate", self.stdpLearningRate);
                config->set("neuromod.dopamine.enable", self.enableDopamine);
                config->set("neuromod.curiosity.enable", self.enableCuriosity);
                config->set("neuromod.novelty.enable", self.enableNovelty);
                config->set("neuromod.dopamine.scale", self.dopamineScale);
                config->set("memory.working_capacity", self.workingMemoryCapacity);
                config->set("memory.episodic_limit", self.episodicMemoryLimit);
                config->set("memory.consolidation_threshold", self.memoryConsolidationThreshold);
                config->set("development.enable", self.enableDevelopment);
                config->set("development.speed", self.developmentSpeed);
                config->set("development.stages", self.developmentalStages);
            }
        }, "Apply advanced configuration to a Config object")
        .def("reset_to_defaults", &AdvancedConfig::AdvancedConfig,
             "Reset configuration to default values");

    // ====================================================================
    // PERFORMANCE MONITORING AND PROFILING
    // ====================================================================
    
    py::class_<PerformanceMetrics>(m, "PerformanceMetrics", R"pbdoc(
        Performance monitoring and metrics collection for NLM simulation
        
        Tracks simulation performance, memory usage, and computational metrics
        for analysis and optimization.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("simulation_time", &PerformanceMetrics::simulation_time)
        .def_readwrite("wall_clock_time", &PerformanceMetrics::wall_clock_time)
        .def_readwrite("cpu_time", &PerformanceMetrics::cpu_time)
        .def_readwrite("memory_usage", &PerformanceMetrics::memory_usage)
        .def_readwrite("peak_memory_usage", &PerformanceMetrics::peak_memory_usage)
        .def_readwrite("neuron_updates", &PerformanceMetrics::neuron_updates)
        .def_readwrite("synapse_updates", &PerformanceMetrics::synapse_updates)
        .def_readwrite("spike_count", &PerformanceMetrics::spike_count)
        .def_readwrite("step_count", &PerformanceMetrics::step_count)
        .def("calculate_fps", &PerformanceMetrics::calculate_fps,
             "Calculate frames per second")
        .def("calculate_neurons_per_second", &PerformanceMetrics::calculate_neurons_per_second,
             "Calculate neurons processed per second")
        .def("reset", &PerformanceMetrics::reset,
             "Reset all metrics to initial state");

    // ====================================================================
    // ADVANCED BRAIN MANIPULATION
    // ====================================================================
    
    py::class_<Brain, std::shared_ptr<Brain>>(m, "AdvancedBrain", R"pbdoc(
        Enhanced brain class with advanced manipulation capabilities
        
        Provides additional methods for brain control, monitoring, and
        advanced neural network operations.
    )pbdoc", py::dynamic_attr<>())
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step_with_time", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input")
        .def("inject_current_to_neuron", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current to specific neuron")
        .def("inject_current_to_type", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current to all neurons of type")
        .def("produce_action", &Brain::produceAction, "Produce motor action")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save_state", &Brain::save, py::arg("filepath"), "Save brain state")
        .def("load_state", &Brain::load, py::arg("filepath"), "Load brain state")
        
        // Advanced manipulation methods
        .def("activate_region", [](Brain& self, const RegionId& regionId) {
            auto region = self.getRegion(regionId);
            if (region) {
                for (auto& neuron : region->getAllNeurons()) {
                    neuron->setFiringState(FiringState::Active);
                }
            }
        }, py::arg("region_id"), "Activate all neurons in a region")
        
        .def("deactivate_region", [](Brain& self, const RegionId& regionId) {
            auto region = self.getRegion(regionId);
            if (region) {
                for (auto& neuron : region->getAllNeurons()) {
                    neuron->setFiringState(FiringState::Resting);
                }
            }
        }, py::arg("region_id"), "Deactivate all neurons in a region")
        
        .def("set_plasticity_enabled", &Brain::setPlasticityEnabled,
             py::arg("enabled"), "Enable or disable plasticity globally")
        
        .def("get_plasticity_enabled", &Brain::getPlasticityEnabled,
             "Check if plasticity is enabled")
        
        .def("set_development_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        
        .def("get_development_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        
        .def("measure_connectivity", &Brain::measureConnectivity,
             "Measure connectivity between regions")
        
        .def("get_region_activity", [](const Brain& self, const RegionId& regionId) {
            auto region = self.getRegion(regionId);
            if (!region) return std::vector<float>();
            
            std::vector<float> activity;
            for (auto& neuron : region->getAllNeurons()) {
                activity.push_back(neuron->getFiringState() == FiringState::Active ? 1.0f : 0.0f);
            }
            return activity;
        }, py::arg("region_id"), "Get activity levels for all neurons in region")
        
        .def("get_brain_graph", [](const Brain& self) {
            // Create a simple graph representation
            std::unordered_map<NeuronId, std::vector<std::pair<NeuronId, float>>> graph;
            
            for (const auto& region : self.getRegions()) {
                for (auto& neuron : region->getAllNeurons()) {
                    std::vector<std::pair<NeuronId, float>> connections;
                    for (auto& syn : region->getSynapses()) {
                        if (syn->getPreSynaptic() == neuron->getId() || 
                            syn->getPostSynaptic() == neuron->getId()) {
                            connections.emplace_back(
                                syn->getPreSynaptic() == neuron->getId() ? 
                                    syn->getPostSynaptic() : syn->getPreSynaptic(),
                                syn->getWeight()
                            );
                        }
                    }
                    graph[neuron->getId()] = connections;
                }
            }
            return graph;
        }, "Get brain connectivity graph")
        
        .def("export_neural_state", [](Brain& self, const std::string& filepath) {
            // Export neural state for external analysis
            auto config = std::make_shared<Config>();
            config->set("export.format", "json");
            config->set("export.filepath", filepath);
            
            // Implementation would save neural state to file
            // For now, just create a placeholder
            std::ofstream file(filepath);
            if (file.is_open()) {
                file << "{\"neural_state\": \"exported_at_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + "\"}";
            }
        }, py::arg("filepath"), "Export neural state to file")
        
        .def("import_neural_state", [](Brain& self, const std::string& filepath) {
            // Import neural state from file
            std::ifstream file(filepath);
            if (file.is_open()) {
                // Implementation would load neural state from file
                // For now, just parse as placeholder
                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                // Parse JSON and restore neural state
            }
        }, py::arg("filepath"), "Import neural state from file");

    // ====================================================================
    // EXPERIMENT MANAGEMENT AND AUTOMATION
    // ====================================================================
    
    py::class_<ExperimentRunner>(m, "ExperimentRunner", R"pbdoc(
        Experiment runner for automated simulation and testing
        
        Provides framework for running automated experiments,
        parameter sweeps, and reproducibility testing.
    )pbdoc")
        .def(py::init<>())
        .def("run_experiment", [](ExperimentRunner& self, std::shared_ptr<Brain> brain,
                                 const std::function<void(std::shared_ptr<Brain>, size_t)> &callback,
                                 size_t max_steps) {
            for (size_t step = 0; step < max_steps; ++step) {
                brain->step(step);
                callback(brain, step);
            }
        }, py::arg("brain"), py::arg("callback"), py::arg("max_steps"),
             "Run experiment with callback at each step")
        
        .def("run_parameter_sweep", [](ExperimentRunner& self, std::shared_ptr<Config> base_config,
                                     const std::vector<std::string>& parameters,
                                     const std::vector<std::vector<double>>& values) {
            // Run parameter sweep across multiple configurations
            for (const auto& param_values : cartesian_product(parameters, values)) {
                auto config = std::make_shared<Config>(*base_config);
                for (size_t i = 0; i < parameters.size(); ++i) {
                    config->set(parameters[i], param_values[i]);
                }
                
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                
                // Run experiment with this configuration
                // Implementation would vary based on experiment type
            }
        }, py::arg("base_config"), py::arg("parameters"), py::arg("values"),
             "Run parameter sweep across multiple configurations")
        
        .def("compare_networks", [](ExperimentRunner& self,
                                   const std::vector<std::shared_ptr<Brain>>& brains,
                                   size_t steps) {
            // Compare performance across multiple networks
            std::vector<PerformanceMetrics> metrics;
            for (auto& brain : brains) {
                PerformanceMetrics metric;
                for (size_t step = 0; step < steps; ++step) {
                    brain->step(step);
                    // Collect metrics
                }
                metrics.push_back(metric);
            }
            return metrics;
        }, py::arg("brains"), py::arg("steps"),
             "Compare performance across multiple networks")
        
        .def("create_reproduction_protocol", [](ExperimentRunner& self,
                                               std::shared_ptr<Brain> brain,
                                               const std::string& protocol_name) {
            // Create a reproducible protocol for later replication
            std::unordered_map<std::string, std::any> protocol;
            protocol["name"] = protocol_name;
            protocol["timestamp"] = std::chrono::system_clock::now();
            protocol["config"] = brain->getConfig();
            protocol["initial_state"] = /* brain state */;
            
            // Save protocol to file
            // Implementation would save JSON/YAML protocol file
            return protocol;
        }, py::arg("brain"), py::arg("protocol_name"),
             "Create a reproducible experiment protocol")
        
        .def("load_reproduction_protocol", [](ExperimentRunner& self,
                                             const std::string& protocol_name) {
            // Load and execute a saved reproduction protocol
            // Implementation would load protocol from file and restore state
            return std::shared_ptr<Brain>();
        }, py::arg("protocol_name"),
             "Load and execute a saved reproduction protocol");

    // ====================================================================
    // CALLBACK AND EVENT SYSTEM
    // ====================================================================
    
    py::class_<EventSystem>(m, "EventSystem", R"pbdoc(
        Event system for simulation callbacks and notifications
        
        Provides callback system for reacting to simulation events,
        enabling custom behavior and automation.
    )pbdoc")
        .def(py::init<>())
        .def("subscribe", [](EventSystem& self,
                           const std::string& event_type,
                           const std::function<void(const std::unordered_map<std::string, std::any>&)>& callback) {
            self.callbacks[event_type] = callback;
        }, py::arg("event_type"), py::arg("callback"),
             "Subscribe to events of a specific type")
        
        .def("unsubscribe", &EventSystem::unsubscribe,
             py::arg("event_type"), "Unsubscribe from events")
        
        .def("publish", &EventSystem::publish,
             py::arg("event_type"), py::arg("event_data"),
             "Publish an event to all subscribers")
        
        .def("subscribe_step", [](EventSystem& self,
                                 const std::function<void(size_t, std::shared_ptr<Brain>)>& callback) {
            self.stepCallbacks.push_back(callback);
        }, py::arg("callback"), "Subscribe to step events")
        
        .def("subscribe_spike", [](EventSystem& self,
                                 const std::function<void(NeuronId, std::shared_ptr<Brain>)>& callback) {
            self.spikeCallbacks.push_back(callback);
        }, py::arg("callback"), "Subscribe to spike events")
        
        .def("subscribe_reward", [](EventSystem& self,
                                   const std::function<void(float, std::shared_ptr<Brain>)>& callback) {
            self.rewardCallbacks.push_back(callback);
        }, py::arg("callback"), "Subscribe to reward events");

    // ====================================================================
    // VISUALIZATION AND ANALYSIS
    // ====================================================================
    
    m.def("visualize_network", [](std::shared_ptr<Brain> brain,
                               const std::string& format = "graphviz",
                               const std::string& output_file = "") {
        // Create visualization of neural network
        if (format == "graphviz") {
            // Generate GraphViz DOT file
            std::string dot = "digraph brain {\n";
            for (const auto& region : brain->getRegions()) {
                dot += "    region_" + std::to_string(region->getId().value) + " [shape=box];\n";
                for (auto& neuron : region->getAllNeurons()) {
                    dot += "    neuron_" + std::to_string(neuron->getId().value) + " [shape=circle];\n";
                    dot += "    region_" + std::to_string(region->getId().value) + 
                           " -> neuron_" + std::to_string(neuron->getId().value) + ";\n";
                }
            }
            dot += "}";
            
            if (!output_file.empty()) {
                std::ofstream file(output_file);
                if (file.is_open()) {
                    file << dot;
                }
            }
            return dot;
        }
        return std::string();
    }, py::arg("brain"), py::arg("format") = "graphviz", py::arg("output_file") = "",
         "Create visualization of neural network");

    m.def("analyze_activity_patterns", [](std::shared_ptr<Brain> brain,
                                         size_t window_size = 100) {
        // Analyze activity patterns over time window
        std::vector<std::vector<float>> patterns;
        size_t total_steps = brain->getTotalSpikeCount();
        
        for (size_t step = 0; step < total_steps - window_size; step += window_size) {
            std::vector<float> pattern;
            // Implementation would analyze activity patterns
            pattern.push_back(static_cast<float>(step) / total_steps);
            patterns.push_back(pattern);
        }
        return patterns;
    }, py::arg("brain"), py::arg("window_size") = 100,
         "Analyze neural activity patterns over time");

    m.def("calculate_connectivity_matrix", [](std::shared_ptr<Brain> brain) {
        // Calculate connectivity matrix between regions
        size_t num_regions = brain->getRegionCount();
        std::vector<std::vector<float>> connectivity(num_regions,
                                                      std::vector<float>(num_regions, 0.0f));
        
        // Implementation would calculate actual connectivity
        for (size_t i = 0; i < num_regions; ++i) {
            for (size_t j = 0; j < num_regions; ++j) {
                connectivity[i][j] = 0.5f; // Placeholder
            }
        }
        return connectivity;
    }, py::arg("brain"), "Calculate connectivity matrix between brain regions");

    // ====================================================================
    // ADVANCED FACTORY FUNCTIONS
    // ====================================================================
    
    m.def("create_advanced_brain", [](const AdvancedConfig& advanced_config,
                                    const std::string& config_file = "") -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (!config_file.empty()) {
            config->loadFromFile(config_file);
        }
        
        // Apply advanced configuration
        advanced_config.apply_to_config(config);
        
        return std::make_shared<Brain>(config);
    }, py::arg("advanced_config"), py::arg("config_file") = "",
         "Create brain with advanced configuration");

    m.def("create_experiment_brain", [](const std::string& experiment_type,
                                       const std::unordered_map<std::string, std::any>& parameters) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        
        // Configure based on experiment type
        if (experiment_type == "memory") {
            config->set("brain.neuron_count", std::any_cast<int>(parameters.at("neuron_count")));
            config->set("brain.synapse_density", std::any_cast<float>(parameters.at("synapse_density")));
            config->set("memory.enable_working_memory", true);
            config->set("memory.enable_episodic_memory", true);
        } else if (experiment_type == "plasticity") {
            config->set("brain.neuron_count", std::any_cast<int>(parameters.at("neuron_count")));
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", std::any_cast<float>(parameters.at("learning_rate")));
        } else if (experiment_type == "development") {
            config->set("brain.neuron_count", std::any_cast<int>(parameters.at("neuron_count")));
            config->set("development.enable", true);
            config->set("development.stages", std::any_cast<size_t>(parameters.at("stages")));
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("experiment_type"), py::arg("parameters"),
         "Create brain configured for specific experiment type");

    m.def("create_visualization_brain", [](size_t neuron_count = 1000) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count));
        config->set("brain.synapse_density", 0.05f); // Sparse connectivity for visualization
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("neuron_count") = 1000,
         "Create brain optimized for visualization and analysis");

    // ====================================================================
    // UTILITY FUNCTIONS AND HELPERS
    // ====================================================================
    
    m.def("get_version_info", &showVersionInfo,
          "Get version information about the NLM library");
    
    m.def("show_advanced_help", &showAdvancedHelp,
          "Show advanced help information");

    // Constants
    m.attr("VERSION_MAJOR") = 0;
    m.attr("VERSION_MINOR") = 1;
    m.attr("VERSION_PATCH") = 0;
    m.attr("VERSION_STRING") = "0.1.0";
    m.attr("PHASE") = "6: FINAL INTEGRATION";
    m.attr("COPYRIGHT") = "© 2024 NLM Research Project";
    m.attr("LICENSE") = "MIT License";

    // Utility functions
    m.def("cartesian_product", [](const std::vector<std::vector<double>>& vectors) {
        // Implementation of cartesian product
        std::vector<std::vector<double>> result;
        // Implementation would compute cartesian product
        return result;
    }, py::arg("vectors"), "Compute cartesian product of vectors");

    m.def("normalize_vector", [](const std::vector<double>& vector) {
        double magnitude = 0.0;
        for (double val : vector) {
            magnitude += val * val;
        }
        magnitude = std::sqrt(magnitude);
        
        std::vector<double> normalized;
        for (double val : vector) {
            normalized.push_back(val / magnitude);
        }
        return normalized;
    }, py::arg("vector"), "Normalize a vector to unit length");

    // Advanced type conversions
    py::class_<py::Any>(m, "Any", R"pbdoc(Wrapper for any type that can be converted to/from Python"))
        .def(py::init<>())
        .def("set", [](py::Any& self, const py::object& value) {
            self.value = value;
        }, py::arg("value"), "Set the value")
        .def("get", [](py::Any& self) {
            return self.value;
        }, "Get the value as Python object")
        .def("is_int", [](const py::Any& self) {
            return self.value.is(py::int_());
        }, "Check if value is integer")
        .def("is_float", [](const py::Any& self) {
            return self.value.is(py::float_());
        }, "Check if value is float")
        .def("is_string", [](const py::Any& self) {
            return self.value.is(py::str());
        }, "Check if value is string")
        .def("is_bool", [](const py::Any& self) {
            return self.value.is(py::bool_());
        }, "Check if value is boolean");

    // ====================================================================
    // EXPERIMENTAL FEATURE FLAGS
    // ====================================================================
    
    // Feature flags for experimental features
    m.attr("EXPERIMENTAL_PLASOSITY_MODE") = true;
    m.attr("EXPERIMENTAL_NEURAL_SYNTHESIS") = true;
    m.attr("EXPERIMENTAL_DEVELOPMENT_MODELS") = true;
    m.attr("EXPERIMENTAL_MEMORY_COMPRESSION") = true;
    m.attr("EXPERIMENTAL_VISUALIZATION_API") = true;
}

} // namespace nlm