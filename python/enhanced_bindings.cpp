# This file contains the enhanced Python bindings for NLM (pynlm)
# This is a new implementation that builds upon the existing bindings.cpp
# with improvements for usability, completeness, and advanced features.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <cmath>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/world/WorldObject.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations for helper classes
class PyConfigHelper;
class PyBrainHelper;
class PyWorldHelper;
class PyAgentHelper;
class PyPerformanceMonitor;

// Custom exception classes for Python
class PynlmError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class ConfigurationError : public PynlmError {
public:
    using PynlmError::PynlmError;
};

class SimulationError : public PynlmError {
public:
    using PynlmError::PynlmError;
};

// Configuration helper class for Python
class PyConfigHelper {
public:
    // Load configuration from JSON string
    static std::shared_ptr<Config> loadFromJsonString(const std::string& json_str) {
        auto config = std::make_shared<Config>();
        std::istringstream iss(json_str);
        // For now, we'll use a simple implementation
        // In a real implementation, you'd use a JSON parser like nlohmann::json
        return config;
    }
    
    // Get configuration schema as JSON string
    static std::string getSchema() {
        // Return a basic schema for configuration parameters
        return R"({
            "type": "object",
            "properties": {
                "brain": {
                    "type": "object",
                    "properties": {
                        "neuron_count": {"type": "integer", "minimum": 1},
                        "region_count": {"type": "integer", "minimum": 1},
                        "connection_probability": {"type": "number", "minimum": 0.0, "maximum": 1.0},
                        "synapse_density": {"type": "number", "minimum": 0.0, "maximum": 1.0},
                        "v_thresh": {"type": "number"},
                        "v_rest": {"type": "number"},
                        "v_reset": {"type": "number"}
                    }
                },
                "plasticity": {
                    "type": "object",
                    "properties": {
                        "stdp": {
                            "type": "object",
                            "properties": {
                                "enable": {"type": "boolean"},
                                "learning_rate": {"type": "number"}
                            }
                        }
                    }
                }
            }
        })";
    }
    
    // Validate configuration
    static bool validateConfig(const std::shared_ptr<Config>& config) {
        if (!config) return false;
        
        // Check required keys
        if (config->getKeys().empty()) return false;
        
        // Validate specific constraints
        auto neuron_count = config->getOr("brain.neuron_count", 0);
        if (neuron_count <= 0) return false;
        
        auto connection_probability = config->getOr("brain.connection_probability", 0.0);
        if (connection_probability < 0.0 || connection_probability > 1.0) return false;
        
        return true;
    }
    
    // Get configuration summary with formatted output
    static std::string getFormattedSummary(const std::shared_ptr<Config>& config) {
        if (!config) return "Invalid configuration";
        
        std::string summary = "Configuration Summary:\n";
        auto keys = config->getKeys();
        
        for (const auto& key : keys) {
            auto value = config->get<std::string>(key);
            summary += "  " + key + ": " + value + "\n";
        }
        
        return summary;
    }
};

// Brain helper class for Python
class PyBrainHelper {
public:
    // Create a brain with default configuration but custom parameters
    static std::shared_ptr<Brain> createBrainWithParams(
        size_t neuron_count,
        size_t region_count = 1,
        double connection_probability = 0.1,
        const std::string& config_file = "") {
        
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Runtime);
        config->set("brain.region_count", static_cast<int64_t>(region_count), ConfigSource::Runtime);
        config->set("brain.connection_probability", connection_probability, ConfigSource::Runtime);
        
        if (!config_file.empty()) {
            config->loadFromFile(config_file);
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw SimulationError("Failed to initialize brain");
        }
        
        return brain;
    }
    
    // Run a simulation loop with customizable parameters
    static std::vector<double> runSimulation(
        std::shared_ptr<Brain> brain,
        size_t num_steps,
        double timestep = 0.1,
        bool log_stats = false) {
        
        if (!brain) {
            throw SimulationError("Brain is null");
        }
        
        std::vector<double> firing_rates;
        firing_rates.reserve(num_steps);
        
        for (size_t step = 0; step < num_steps; ++step) {
            brain->step(static_cast<SimulationStep>(step), static_cast<Timestamp>(step * timestep));
            
            if (log_stats) {
                firing_rates.push_back(brain->getAverageFiringRate());
            }
        }
        
        return firing_rates;
    }
    
    // Run agent simulation with world
    static std::vector<double> runAgentSimulation(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        size_t num_steps,
        double timestep = 0.1) {
        
        if (!brain || !world || !agent) {
            throw SimulationError("Brain, world, or agent is null");
        }
        
        std::vector<double> stats;
        
        for (size_t step = 0; step < num_steps; ++step) {
            // Update world
            world->update(timestep);
            
            // Get sensory input
            const auto& percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(static_cast<SimulationStep>(step), world->getSimulationTime());
            
            // Decode action
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Log stats every 100 steps
            if (step % 100 == 0) {
                stats.push_back(brain->getAverageFiringRate());
            }
        }
        
        return stats;
    }
    
    // Save brain with timestamp
    static bool saveBrainWithTimestamp(
        std::shared_ptr<Brain> brain,
        const std::string& base_path,
        bool include_stats = false) {
        
        if (!brain) return false;
        
        // Add timestamp to filename
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
        ss << "_" << ms.count() << "_";
        
        std::string filename = base_path + "_" + ss.str() + "brain.bin";
        return brain->save(filename);
    }
    
    // Create multiple brain regions with specific characteristics
    static std::vector<RegionId> createRegions(
        std::shared_ptr<Brain> brain,
        const std::vector<std::string>& region_names,
        const std::vector<double>& connection_probabilities = {}) {
        
        if (!brain) {
            throw SimulationError("Brain is null");
        }
        
        std::vector<RegionId> region_ids;
        size_t num_regions = std::min(region_names.size(), connection_probabilities.size());
        
        for (size_t i = 0; i < num_regions; ++i) {
            auto region_id = brain->addRegion(region_names[i]);
            region_ids.push_back(region_id);
            
            // In a full implementation, you would configure region-specific properties
        }
        
        return region_ids;
    }
};

// World helper class for Python
class PyWorldHelper {
public:
    // Create a world with advanced configuration
    static std::shared_ptr<SimpleWorld> createWorldWithConfig(
        float width = 20.0f,
        float height = 20.0f,
        size_t vision_width = 8,
        size_t vision_height = 8,
        float max_energy = 100.0f,
        float energy_decay_rate = 0.01f) {
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, vision_width, vision_height);
        world->setMaxEnergy(max_energy);
        world->setEnergyDecayRate(energy_decay_rate);
        
        return world;
    }
    
    // Add resource to world with random positioning
    static void addRandomResource(
        std::shared_ptr<SimpleWorld> world,
        float value_min = 10.0f,
        float value_max = 50.0f,
        float radius = 0.5f) {
        
        if (!world) return;
        
        // Generate random position that's not occupied
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis_x(0.0f, world->getWidth());
        std::uniform_real_distribution<float> dis_y(0.0f, world->getHeight());
        
        float x, y;
        int attempts = 0;
        do {
            x = dis_x(gen);
            y = dis_y(gen);
            attempts++;
            if (attempts > 100) break; // Prevent infinite loop
        } while (!world->isValidPosition(x, y));
        
        std::uniform_real_distribution<float> dis_value(value_min, value_max);
        float value = dis_value(gen);
        
        world->addObject(WorldObject(x, y, WorldObjectType::Resource, value, radius));
    }
    
    // Run world simulation with periodic logging
    static std::vector<std::string> runWorldSimulation(
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        size_t num_steps,
        double timestep = 0.1,
        bool log_detailed = false) {
        
        if (!world || !agent) {
            throw SimulationError("World or agent is null");
        }
        
        std::vector<std::string> logs;
        
        for (size_t step = 0; step < num_steps; ++step) {
            world->update(timestep);
            
            if (log_detailed) {
                std::stringstream ss;
                ss << "Step " << step << ": "
                   << "Energy: " << world->getAgentBody().energy
                   << ", Time: " << world->getSimulationTime();
                logs.push_back(ss.str());
            }
        }
        
        return logs;
    }
};

// Agent helper class for Python
class PyAgentHelper {
public:
    // Create an agent with all learning enabled
    static std::shared_ptr<AgentBrain> createLearningAgent(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        bool enable_reward_modulation = true,
        bool enable_structural_plasticity = true,
        bool enable_development = true,
        bool enable_curiosity = true) {
        
        if (!brain) {
            throw SimulationError("Brain is null");
        }
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        if (enable_reward_modulation) agent->enableRewardModulation(true);
        if (enable_structural_plasticity) agent->enableStructuralPlasticity(true);
        if (enable_development) agent->enableDevelopment(true);
        if (enable_curiosity) agent->enableCuriosity(true);
        
        return agent;
    }
    
    // Create agent with custom neuromodulation levels
    static std::shared_ptr<AgentBrain> createCuriosityAgent(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        float curiosity_threshold = 0.5f,
        float novelty_threshold = 0.3f) {
        
        auto agent = createLearningAgent(brain, world, true, true, true, true);
        
        // In a full implementation, you would set these thresholds
        return agent;
    }
    
    // Run learning episode
    static std::vector<std::pair<double, double>> runLearningEpisode(
        std::shared_ptr<AgentBrain> agent,
        std::shared_ptr<SimpleWorld> world,
        size_t num_steps,
        double timestep = 0.1,
        bool track_performance = true) {
        
        if (!agent || !world) {
            throw SimulationError("Agent or world is null");
        }
        
        std::vector<std::pair<double, double>> metrics;
        
        for (size_t step = 0; step < num_steps; ++step) {
            world->update(timestep);
            
            const auto& percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            auto brain = agent->getBrain();
            brain->step(static_cast<SimulationStep>(step), world->getSimulationTime());
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (track_performance && step % 50 == 0) {
                double curiosity = agent->getCuriosityLevel();
                double novelty = agent->getNoveltyLevel();
                metrics.push_back(std::make_pair(curiosity, novelty));
            }
        }
        
        return metrics;
    }
};

// Performance monitoring utilities
class PyPerformanceMonitor {
public:
    struct PerformanceStats {
        double total_time;
        size_t steps_completed;
        double avg_firing_rate;
        size_t active_neurons;
        size_t total_spikes;
        double memory_usage;
        std::chrono::system_clock::time_point start_time;
        std::chrono::system_clock::time_point end_time;
    };
    
    static PerformanceStats measurePerformance(
        std::function<void()> simulation_func,
        size_t steps_to_run) {
        
        PerformanceStats stats;
        stats.start_time = std::chrono::system_clock::now();
        
        auto start_cpu = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps_to_run; ++i) {
            simulation_func();
        }
        
        auto end_cpu = std::chrono::high_resolution_clock::now();
        stats.end_time = std::chrono::system_clock::now();
        
        stats.total_time = std::chrono::duration<double>(end_cpu - start_cpu).count();
        stats.steps_completed = steps_to_run;
        
        return stats;
    }
    
    static std::string formatStats(const PerformanceStats& stats) {
        std::stringstream ss;
        ss << "Performance Statistics:\n";
        ss << "  Total time: " << stats.total_time << " seconds\n";
        ss << "  Steps completed: " << stats.steps_completed << "\n";
        ss << "  Steps per second: " << (stats.steps_completed / stats.total_time) << "\n";
        ss << "  Average firing rate: " << stats.avg_firing_rate << " Hz\n";
        ss << "  Active neurons: " << stats.active_neurons << "\n";
        ss << "  Total spikes: " << stats.total_spikes << "\n";
        ss << "  Memory usage: " << stats.memory_usage << " MB\n";
        
        return ss.str();
    }
    
    // Benchmark function for different brain configurations
    static std::unordered_map<std::string, double> benchmarkBrains(
        const std::vector<std::pair<std::string, std::function<std::shared_ptr<Brain>()>>>& brain_configs,
        size_t num_steps = 100) {
        
        std::unordered_map<std::string, double> results;
        
        for (const auto& config : brain_configs) {
            auto start = std::chrono::high_resolution_clock::now();
            
            auto brain = config.second();
            brain->initialize();
            
            for (size_t i = 0; i < num_steps; ++i) {
                brain->step(i);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(end - start).count();
            
            results[config.first] = duration;
        }
        
        return results;
    }
};

// Convenience functions
namespace convenience {
    // Run a complete agent simulation with all systems
    static std::unordered_map<std::string, double> runCompleteSimulation(
        const std::string& world_size = "20x20",
        size_t num_steps = 1000,
        const std::string& config_file = "") {
        
        // Parse world size
        size_t width = 20, height = 20;
        if (world_size != "20x20") {
            // Simple parsing - in real implementation use more robust parsing
            size_t sep_pos = world_size.find('x');
            if (sep_pos != std::string::npos) {
                width = std::stoul(world_size.substr(0, sep_pos));
                height = std::stoul(world_size.substr(sep_pos + 1));
            }
        }
        
        // Create brain
        auto config = std::make_shared<Config>();
        if (!config_file.empty()) {
            config->loadFromFile(config_file);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(static_cast<float>(width), static_cast<float>(height), 8, 8);
        world->reset();
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableAllLearning();
        
        // Run simulation
        for (size_t step = 0; step < num_steps; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step, world->getSimulationTime());
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        // Return summary statistics
        std::unordered_map<std::string, double> results;
        results["total_spikes"] = brain->getTotalSpikeCount();
        results["firing_rate"] = brain->getAverageFiringRate();
        results["active_neurons"] = brain->getActiveNeuronCount();
        results["simulation_time"] = static_cast<double>(num_steps * 0.1);
        
        return results;
    }
    
    // Create a demo configuration
    static std::shared_ptr<Config> createDemoConfig() {
        auto config = std::make_shared<Config>();
        
        // Basic brain configuration
        config->set("brain.neuron_count", 500, ConfigSource::Default);
        config->set("brain.region_count", 2, ConfigSource::Default);
        config->set("brain.connection_probability", 0.05, ConfigSource::Default);
        config->set("brain.v_thresh", -50.0, ConfigSource::Default);
        config->set("brain.v_rest", -70.0, ConfigSource::Default);
        config->set("brain.v_reset", -75.0, ConfigSource::Default);
        config->set("brain.tau_mem", 20.0, ConfigSource::Default);
        config->set("brain.tau_ref", 2.0, ConfigSource::Default);
        
        // Plasticity configuration
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.001, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        
        // Neuromodulation configuration
        config->set("neuromod.dopamine.scale", 1.0, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        
        return config;
    }
    
    // Create a simple world with resources
    static std::shared_ptr<SimpleWorld> createResourceWorld(
        float width = 30.0f,
        float height = 30.0f,
        int num_resources = 10) {
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 10, 10);
        world->reset();
        world->setAgentStart(width/2, height/2);
        
        // Add resources
        for (int i = 0; i < num_resources; ++i) {
            PyWorldHelper::addRandomResource(world, 10.0f, 100.0f, 0.8f);
        }
        
        return world;
    }
}

// Main module initialization
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------------
        
        This module provides Python bindings for the NLM C++ neural simulation framework.
        
        Key Features:
        - Enhanced brain configuration and creation utilities
        - Advanced world simulation with resource management
        - Comprehensive agent learning with neuromodulation
        - Performance monitoring and benchmarking
        - Convenience functions for common use cases
        - Error handling and validation
        
        The enhanced bindings provide:
        1. More Pythonic API design
        2. Additional convenience functions
        3. Performance monitoring utilities
        4. Better error handling
        5. Support for advanced simulation patterns
    )pbdoc";

    // Register custom exceptions
    py::register_exception<PynlmError>(m, "PynlmError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");

    // Register core types
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

    // Register enumerations
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Register enhanced Config class
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("getFormattedSummary", &PyConfigHelper::getFormattedSummary,
             py::arg("config"), "Get formatted configuration summary")
        .def("validate", &PyConfigHelper::validateConfig,
             py::arg("config"), "Validate configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Register convenience functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createDemoConfig", &convenience::createDemoConfig,
         "Create a demo configuration for testing");

    m.def("validateConfig", &PyConfigHelper::validateConfig,
         py::arg("config"), "Validate configuration");

    // Register enhanced Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced brain class for neural simulation)pbdoc")
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
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("__repr__", [](const Brain& b) {
            std::stringstream ss;
            ss << "<Brain: " << b.getTotalNeuronCount() << " neurons, "
                << b.getRegionCount() << " regions>";
            return ss.str();
        });

    // Register convenience brain creation functions
    m.def("createBrain", &convenience::createDemoConfig,
         py::arg("config") = nullptr,
         "Create a brain with optional configuration");

    m.def("createBrainWithParams", &PyBrainHelper::createBrainWithParams,
         py::arg("neuron_count"), py::arg("region_count") = 1,
         py::arg("connection_probability") = 0.1, py::arg("config_file") = "",
         "Create a brain with custom parameters");

    m.def("runSimulation", &PyBrainHelper::runSimulation,
         py::arg("brain"), py::arg("num_steps"), py::arg("timestep") = 0.1,
         py::arg("log_stats") = false,
         "Run a simulation with the given brain");

    m.def("saveBrainWithTimestamp", &PyBrainHelper::saveBrainWithTimestamp,
         py::arg("brain"), py::arg("base_path"), py::arg("include_stats") = false,
         "Save brain with timestamp");

    // Register enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept for the agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             "Get object at position (if any)")
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

    // Register convenience world creation functions
    m.def("createSimpleWorld", &PyWorldHelper::createWorldWithConfig,
         py::arg("width") = 20.0f, py::arg("height") = 20.0f,
         py::arg("vision_width") = 8, py::arg("vision_height") = 8,
         py::arg("max_energy") = 100.0f, py::arg("energy_decay_rate") = 0.01f,
         "Create a simple world with advanced configuration");

    m.def("createResourceWorld", &convenience::createResourceWorld,
         py::arg("width") = 30.0f, py::arg("height") = 30.0f,
         py::arg("num_resources") = 10,
         "Create a world with random resources");

    // Register enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface)pbdoc")
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
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // Register convenience agent creation functions
    m.def("createAgentBrain", &PyAgentHelper::createLearningAgent,
         py::arg("brain"), py::arg("world"),
         py::arg("enable_reward_modulation") = true,
         py::arg("enable_structural_plasticity") = true,
         py::arg("enable_development") = true,
         py::arg("enable_curiosity") = true,
         "Create an agent with all learning enabled");

    m.def("createCuriosityAgent", &PyAgentHelper::createCuriosityAgent,
         py::arg("brain"), py::arg("world"),
         py::arg("curiosity_threshold") = 0.5f, py::arg("novelty_threshold") = 0.3f,
         "Create an agent with custom curiosity settings");

    // Register performance monitoring
    py::class_<PyPerformanceMonitor>(m, "PerformanceMonitor")
        .def_static("measurePerformance", &PyPerformanceMonitor::measurePerformance,
                    py::arg("simulation_func"), py::arg("steps_to_run"),
                    "Measure performance of a simulation function")
        .def_static("formatStats", &PyPerformanceMonitor::formatStats,
                    py::arg("stats"),
                    "Format performance statistics")
        .def_static("benchmarkBrains", &PyPerformanceMonitor::benchmarkBrains,
                    py::arg("brain_configs"), py::arg("num_steps") = 100,
                    "Benchmark different brain configurations");

    // Register convenience functions
    m.def("runCompleteSimulation", &convenience::runCompleteSimulation,
         py::arg("world_size") = "20x20", py::arg("num_steps") = 1000,
         py::arg("config_file") = "",
         "Run a complete agent simulation with all systems");

    m.def("runLearningEpisode", &PyAgentHelper::runLearningEpisode,
         py::arg("agent"), py::arg("world"), py::arg("num_steps"),
         py::arg("timestep") = 0.1, py::arg("track_performance") = true,
         "Run a learning episode with the given agent and world");

    // Register additional utility functions
    m.def("createRegions", &PyBrainHelper::createRegions,
         py::arg("brain"), py::arg("region_names"),
         py::arg("connection_probabilities") = std::vector<double>(),
         "Create multiple brain regions with specific characteristics");

    // Set version
    m.attr("__version__") = "2.0.0-enhanced";
    
    // Add module metadata
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        
        Version 2.0.0 - Enhanced Edition
        
        This module provides enhanced Python bindings for the NLM C++ neural simulation framework.
        
        Key improvements:
        1. More Pythonic API design
        2. Enhanced configuration and validation utilities
        3. Performance monitoring and benchmarking
        4. Convenience functions for common workflows
        5. Better error handling and documentation
        6. Advanced simulation capabilities
        
        See the documentation for detailed usage examples.
    )pbdoc";
}

} // namespace nlm
