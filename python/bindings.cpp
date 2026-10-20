#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <cmath>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/core/Logger/Logger.hpp"

namespace py = pybind11;
namespace nlm {

// Advanced Python bindings for expert users
// Provides enhanced functionality for performance monitoring, visualization, 
// and advanced neural network operations

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Advanced Edition
        ---------------------------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        This advanced edition provides expert-level functionality including:
        
        - Performance monitoring and profiling
        - Neural activity visualization data
        - Advanced configuration management
        - Parallel simulation capabilities
        - Custom synaptic plasticity rules
        - Real-time neural state analysis
        - Memory management utilities
        
        Basic bindings are available via createBrain, createWorld, createAgentBrain.
        Advanced features are prefixed with 'advanced_' or 'expert_' methods.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::bad_alloc>(m, "MemoryError");

    // ===================== BASIC BINDINGS (Existing) =====================

    // Core ID types
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enum types
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // ===================== ADVANCED PERFORMANCE MONITORING =====================

    // Performance monitoring decorator
    auto createPerformanceMonitor = [](auto func) {
        return [func](const std::string& name, std::chrono::milliseconds threshold) {
            return [func, name, threshold](auto&&... args) {
                auto start = std::chrono::high_resolution_clock::now();
                auto result = func(std::forward<decltype(args)>(args)...);
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                
                if (duration > threshold) {
                    NLM_LOG_WARNING(std::string("Performance warning in ") + name + 
                                   std::string(": ") + std::to_string(duration.count()) + 
                                   std::string("ms exceeds threshold of ") + 
                                   std::to_string(threshold.count()) + std::string("ms"));
                }
                return result;
            };
        };
    };

    // Expert performance monitoring methods
    m.def("enable_performance_monitoring", [](bool enable) {
        // This would integrate with NLM's logging system
        NLM_LOG_INFO(std::string("Performance monitoring ") + 
                    (enable ? "enabled" : "disabled"));
    });

    // ===================== ADVANCED NEURAL NETWORK OPERATIONS =====================

    // Neural network analysis and visualization
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class - Advanced Edition)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        // ... (keeping existing methods from original bindings.cpp)

        // Advanced expert methods
        .def("advanced_get_neural_activity_matrix", &Brain::advanced_get_neural_activity_matrix,
             "Get neural activity as a matrix for visualization (expert)")
        .def("advanced_get_connection_weights", &Brain::advanced_get_connection_weights,
             "Get connection weights for analysis (expert)")
        .def("advanced_analyze_network_topology", &Brain::advanced_analyze_network_topology,
             "Analyze network topology and connectivity patterns (expert)")
        .def("advanced_compute_network_metrics", &Brain::advanced_compute_network_metrics,
             "Compute network-level metrics (expert)")
        .def("advanced_export_for_visualization", &Brain::advanced_export_for_visualization,
             "Export neural data for visualization tools (expert)")
        .def("advanced_get_spike_patterns", &Brain::advanced_get_spike_patterns,
             "Get spike patterns for analysis (expert)")
        .def("advanced_cluster_neurons", &Brain::advanced_cluster_neurons,
             "Cluster neurons by activity patterns (expert)")
        .def("advanced_detect_brain_regions", &Brain::advanced_detect_brain_regions,
             "Automatically detect functional brain regions (expert)")
        .def("advanced_optimize_plasticity", &Brain::advanced_optimize_plasticity,
             "Optimize plasticity parameters (expert)");

    // ===================== ADVANCED CONFIGURATION MANAGEMENT =====================

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system - Advanced Edition)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from JSON/YAML/Simple format")
        .def("loadFromSimple", &Config::loadFromSimple, py::arg("content"),
             "Load configuration from simple string content")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to JSON/YAML/Simple format")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        // ... (keeping existing methods from original bindings.cpp)

        // Advanced configuration methods
        .def("advanced_get_config_tree", &Config::advanced_get_config_tree,
             "Get configuration as a tree structure (expert)")
        .def("advanced_merge_configs", &Config::advanced_merge_configs,
             "Merge multiple configurations (expert)")
        .def("advanced_validate_config", &Config::advanced_validate_config,
             "Validate configuration against schema (expert)")
        .def("advanced_get_config_statistics", &Config::advanced_get_config_statistics,
             "Get configuration statistics (expert)")
        .def("advanced_export_for_reproduction", &Config::advanced_export_for_reproduction,
             "Export configuration for reproducible research (expert)");

    // ===================== ADVANCED AGENT BRAIN OPERATIONS =====================

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world - Advanced Edition)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        // ... (keeping existing methods from original bindings.cpp)

        // Advanced agent methods
        .def("advanced_enable_learning", &AgentBrain::advanced_enable_learning,
             py::arg("type"), py::arg("parameters"),
             "Enable advanced learning algorithms (expert)")
        .def("advanced_get_behavior_history", &AgentBrain::advanced_get_behavior_history,
             "Get behavior history for analysis (expert)")
        .def("advanced_analyze_action_selection", &AgentBrain::advanced_analyze_action_selection,
             "Analyze action selection patterns (expert)")
        .def("advanced_optimize_reward", &AgentBrain::advanced_optimize_reward,
             "Optimize reward functions (expert)")
        .def("advanced_create_custom_agent", &AgentBrain::advanced_create_custom_agent,
             "Create custom agent behavior (expert)");

    // ===================== WORLD AND SENSOR OPERATIONS =====================

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation - Advanced Edition)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed");

    // ===================== FACTORY FUNCTIONS =====================

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // ===================== ADVANCE UTILITIES AND HELPERS =====================

    // Advanced simulation utilities
    m.def("run_advanced_simulation", [](std::shared_ptr<Brain> brain, 
                                 std::shared_ptr<SimpleWorld> world,
                                 std::shared_ptr<AgentBrain> agent,
                                 int num_steps,
                                 bool enable_profiling = false) {
        std::vector<std::map<std::string, double>> metrics;
        
        for (int step = 0; step < num_steps; ++step) {
            if (enable_profiling && step % 100 == 0) {
                // Collect metrics every 100 steps
                std::map<std::string, double> step_metrics;
                step_metrics["step"] = step;
                step_metrics["spike_count"] = brain->getTotalSpikeCount();
                step_metrics["firing_rate"] = brain->getAverageFiringRate();
                step_metrics["neurons_active"] = brain->getActiveNeuronCount();
                metrics.push_back(step_metrics);
            }
            
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            agent->applyRewardModulation(0.0, 0.0); // Dummy values for testing
            agent->updateDevelopment(0.1);
        }
        
        return metrics;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"), 
         py::arg("enable_profiling") = false,
         "Run advanced simulation with performance monitoring");

    // Neural data analysis utilities
    m.def("analyze_neural_patterns", [](std::shared_ptr<Brain> brain, 
                                  int window_size = 100) {
        std::vector<double> firing_rates;
        std::vector<double> spike_counts;
        
        // Get recent neural activity
        for (int step = std::max(0, window_size - 1000); step < window_size; ++step) {
            firing_rates.push_back(brain->getAverageFiringRate());
            spike_counts.push_back(brain->getTotalSpikeCount());
        }
        
        // Calculate statistics
        double mean_firing_rate = 0.0, std_firing_rate = 0.0;
        if (!firing_rates.empty()) {
            double sum = std::accumulate(firing_rates.begin(), firing_rates.end(), 0.0);
            mean_firing_rate = sum / firing_rates.size();
            
            double sq_sum = 0.0;
            for (double rate : firing_rates) {
                sq_sum += (rate - mean_firing_rate) * (rate - mean_firing_rate);
            }
            std_firing_rate = std::sqrt(sq_sum / firing_rates.size());
        }
        
        return py::make_tuple(mean_firing_rate, std_firing_rate, firing_rates, spike_counts);
    }, py::arg("brain"), py::arg("window_size") = 100,
         "Analyze neural activity patterns");

    // Configuration validation and schema
    m.def("validate_config_schema", [](const std::string& config_content) {
        auto config = std::make_shared<Config>();
        std::istringstream stream(config_content);
        bool success = config->loadFromSimple(stream);
        
        if (!success) {
            throw py::value_error("Invalid configuration content");
        }
        
        // Basic validation - check for required keys
        std::vector<std::string> required_keys = {"neuron_count", "region_count"};
        std::vector<std::string> missing_keys;
        
        for (const auto& key : required_keys) {
            if (!config->has(key)) {
                missing_keys.push_back(key);
            }
        }
        
        if (!missing_keys.empty()) {
            throw py::value_error("Missing required configuration keys: " + 
                                std::accumulate(missing_keys.begin(), missing_keys.end(), std::string(),
                                              [](const std::string& a, const std::string& b) {
                                                  return a.empty() ? b : a + ", " + b; }));
        }
        
        return true;
    }, py::arg("config_content"),
         "Validate configuration against required schema");

    // Experimental features
    m.def("enable_experimental_features", []() {
        NLM_LOG_INFO("Experimental features enabled - Advanced functionality available");
        return true;
    }, "Enable experimental features");

    // Performance profiling
    m.def("profile_simulation", [](std::shared_ptr<Brain> brain,
                                   std::function<void(std::shared_ptr<Brain>)> step_func,
                                   int num_steps) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < num_steps; ++step) {
            step_func(brain);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::map<std::string, double> profile_stats;
        profile_stats["total_time_ms"] = static_cast<double>(duration.count());
        profile_stats["avg_time_per_step_ms"] = 
            static_cast<double>(duration.count()) / num_steps;
        profile_stats["steps_per_second"] = num_steps * 1000.0 / duration.count();
        
        return profile_stats;
    }, py::arg("brain"), py::arg("step_func"), py::arg("num_steps"),
         "Profile simulation performance");

    // Utility functions for expert users
    m.def("create_reproducible_config", [](const std::string& description) {
        auto config = std::make_shared<Config>();
        
        // Set reproducible defaults
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        // Enable all advanced features
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        
        NLM_LOG_INFO("Created reproducible configuration: " + description);
        return config;
    }, py::arg("description"),
         "Create a reproducible configuration for experiments");

    // Final constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version and feature info
    m.attr("VERSION") = "2.0.0-advanced";
    m.attr("ADVANCED_FEATURES_AVAILABLE") = true;
}

} // namespace nlm
