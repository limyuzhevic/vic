#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/performance/MemoryPool.hpp"

namespace py = pybind11;
namespace nlm {

// Enhanced configuration helper functions
inline std::shared_ptr<Config> createConfigWithDefaults() {
    auto config = std::make_shared<Config>();
    
    // Set reasonable defaults for typical use
    config->set("brain.neuron_count", 1000);
    config->set("brain.synapse_density", 0.1f);
    config->set("simulation.timestep", 0.001);
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.hebbian.enable", true);
    config->set("neuromod.dopamine.scale", 1.0f);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    
    return config;
}

inline std::shared_ptr<Config> createConfigFromJSON(const std::string& filepath) {
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(filepath)) {
        throw std::runtime_error("Failed to load configuration from " + filepath);
    }
    return config;
}

inline std::shared_ptr<Config> createConfigFromArgs(int argc, char** argv) {
    auto config = std::make_shared<Config>();
    if (!config->loadFromArgs(argc, argv)) {
        throw std::runtime_error("Failed to load configuration from command line arguments");
    }
    return config;
}

// Memory management utilities
struct MemoryStats {
    size_t neuronPoolAllocations = 0;
    size_t synapsePoolAllocations = 0;
    size_t neuronPoolDeallocations = 0;
    size_t synapsePoolDeallocations = 0;
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "Neuron Pool: " << neuronPoolAllocations << " allocations, " 
                                 << neuronPoolDeallocations << " deallocations\n";
        oss << "Synapse Pool: " << synapsePoolAllocations << " allocations, " 
                                 << synapsePoolDeallocations << " deallocations\n";
        return oss.str();
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------
        An enhanced Python binding for the NLM C++ neural simulation framework.
        Provides enhanced APIs for easier use and advanced functionality.
        
        Key improvements:
        - Enhanced configuration helpers
        - Memory management utilities
        - Advanced plasticity controls
        - Development system integration
        - Performance monitoring
        - Convenience methods for common operations
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // =======================================================================
    // Enhanced Configuration System
    // =======================================================================

    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system with validation)pbdoc")
        .def(py::init<>())
        // File loading with error handling
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            if (!self.loadFromFile(filepath)) {
                throw std::runtime_error("Failed to load configuration from file: " + filepath);
            }
        }, py::arg("filepath"), "Load and validate configuration from a JSON file")
        .def("loadFromString", [](Config& self, const std::string& jsonContent) {
            // Create a temporary file-like interface
            std::istringstream iss(jsonContent);
            // For now, just use the existing file loader
            // In a full implementation, we'd add JSON string parsing
            throw std::runtime_error("JSON string parsing not yet implemented");
        }, py::arg("jsonContent"), "Load configuration from JSON string")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            if (!self.loadFromArgs(argc, argv)) {
                throw std::runtime_error("Failed to load configuration from command line arguments");
            }
        }, py::arg("argc"), py::arg("argv"), "Load and validate configuration from command line arguments")
        .def("validate", &Config::validateAllEntries,
             "Validate configuration against schema")
        .def("getValidationErrors", &Config::getSchemaErrors,
             "Get configuration validation errors")
        // Enhanced convenience methods
        .def("setBrain", [](Config& self, size_t neuronCount) {
            self.set("brain.neuron_count", neuronCount);
        }, py::arg("neuronCount"), "Set brain neuron count (alias)")
        .def("setSimulation", [](Config& self, double timestep, size_t neuronCount) {
            self.set("simulation.timestep", timestep);
            self.set("brain.neuron_count", neuronCount);
        }, py::arg("timestep"), py::arg("neuronCount"), "Set simulation parameters")
        .def("setPlasticity", [](Config& self, bool stdp, bool hebbian, bool structural) {
            self.set("plasticity.stdp.enable", stdp);
            self.set("plasticity.hebbian.enable", hebbian);
            self.set("plasticity.structural.enable", structural);
        }, py::arg("stdp"), py::arg("hebbian"), py::arg("structural"), "Enable/disable plasticity rules")
        .def("setNeuromodulation", [](Config& self, bool curiosity, bool novelty, float dopamineScale) {
            self.set("neuromod.curiosity.enable", curiosity);
            self.set("neuromod.novelty.enable", novelty);
            self.set("neuromod.dopamine.scale", dopamineScale);
        }, py::arg("curiosity"), py::arg("novelty"), py::arg("dopamineScale"), "Configure neuromodulation")
        // Configuration summaries
        .def("getSummary", &Config::summary,
             "Get a detailed configuration summary")
        .def("printSummary", [](const Config& self) {
            std::cout << self.summary() << std::endl;
        }, "Print configuration summary to console")
        .def("__repr__", [](const Config& cfg) {
            std::ostringstream oss;
            oss << "<Config: " << cfg.getKeys().size() << " keys, ";
            oss << cfg.getEntryCount(ConfigSource::Default) << " default, ";
            oss << cfg.getEntryCount(ConfigSource::File) << " file, ";
            oss << cfg.getEntryCount(ConfigSource::CommandLine) << " command-line, ";
            oss << cfg.getEntryCount(ConfigSource::Runtime) << " runtime>";
            return oss.str();
        });

    // Enhanced configuration factory functions
    m.def("createConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create an empty configuration");
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return createConfigWithDefaults();
    }, "Create configuration with recommended defaults");
    
    m.def("createConfigFromFile", [](const std::string& filepath) -> std::shared_ptr<Config> {
        return createConfigFromJSON(filepath);
    }, py::arg("filepath"), "Create configuration from JSON file");
    
    m.def("createConfigFromArgs", [](int argc, char** argv) -> std::shared_ptr<Config> {
        return createConfigFromArgs(argc, argv);
    }, py::arg("argc"), py::arg("argv"), "Create configuration from command line arguments");

    // =======================================================================
    // Enhanced Brain Interface
    // =======================================================================

    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced neural simulation brain class with memory management)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), "Initialize brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration and memory pools")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        // Enhanced sensory and motor interfaces
        .def("processSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Process sensory input (enhanced interface)")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        // Enhanced state management
        .def("reset", &Brain::reset,
             "Reset brain state including memory pools")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state including memory pool state")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state including memory pool state")
        // Enhanced statistics and monitoring
        .def("getMemoryStats", [](const std::shared_ptr<Brain>& brain) {
            MemoryStats stats;
            // In a full implementation, we would access Brain's internal memory pool stats
            return stats;
        }, "Get memory pool statistics", py::return_value_policy::copy)
        .def("getPerformanceStats", [](const std::shared_ptr<Brain>& brain) {
            // Enhanced performance statistics
            std::ostringstream oss;
            oss << "Brain Performance Statistics:\n";
            oss << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
            oss << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
            oss << "  Active neurons: " << brain->getActiveNeuronCount() << "\n";
            oss << "  Firing neurons: " << brain->getFiringNeuronCount() << "\n";
            oss << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
            oss << "  E/I ratio: " << brain->getExcitationInhibitionRatio() << "\n";
            return oss.str();
        }, "Get comprehensive performance statistics")
        // Enhanced region and population management
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getAllRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        // Development system interface
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("advanceDevelopment", [](std::shared_ptr<Brain>& brain) {
            // Advance development by one stage
            DevelopmentalStage current = brain->getDevelopmentalStage();
            switch (current) {
                case DevelopmentalStage::Initial:
                    brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    brain->setDevelopmentalStage(DevelopmentalStage::Maturation);
                    break;
                case DevelopmentalStage::Maturation:
                    brain->setDevelopmentalStage(DevelopmentalStage::Adult);
                    break;
                case DevelopmentalStage::Adult:
                    brain->setDevelopmentalStage(DevelopmentalStage::Aging);
                    break;
                case DevelopmentalStage::Aging:
                    // Stay in aging stage
                    break;
            }
        }, "Advance brain development to next stage")
        // Memory management utilities
        .def("reserveMemory", [](std::shared_ptr<Brain>& brain, size_t neurons, size_t synapses) {
            // In a full implementation, this would configure memory pool sizes
            return;
        }, py::arg("neurons"), py::arg("synapses"), "Reserve memory in pools")
        .def("getMemoryUsage", [](const std::shared_ptr<Brain>& brain) {
            // Get memory usage statistics
            return 0; // Placeholder
        }, "Get total memory usage in bytes")
        // Convenience methods for common operations
        .def("runEpisode", [](std::shared_ptr<Brain>& brain, size_t steps) {
            for (size_t i = 0; i < steps; ++i) {
                brain->step(i);
            }
        }, py::arg("steps"), "Run a complete simulation episode")
        .def("getFiringStats", [](const std::shared_ptr<Brain>& brain, size_t numSteps) {
            size_t maxFiring = 0;
            float avgFiring = 0.0f;
            
            for (size_t i = 0; i < numSteps; ++i) {
                brain->step(i);
                size_t firing = brain->getFiringNeuronCount();
                maxFiring = std::max(maxFiring, firing);
                avgFiring += static_cast<float>(firing);
            }
            
            if (numSteps > 0) {
                avgFiring /= static_cast<float>(numSteps);
            }
            
            return std::make_tuple(maxFiring, avgFiring);
        }, py::arg("steps"), "Get firing statistics over a number of steps");

    // =======================================================================
    // Enhanced AgentBrain Interface
    // =======================================================================

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain with development and learning)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Initialize agent brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world and enable all subsystems")
        // Enhanced sensory processing
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory input with advanced feature extraction")
        .def("getSensoryFeatures", [](const std::shared_ptr<AgentBrain>& agent, const SensoryPercept& percept) {
            std::vector<float> features;
            // In a full implementation, this would extract advanced features
            return features;
        }, py::arg("percept"), "Extract advanced sensory features")
        // Enhanced motor decoding
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode motor command with action selection")
        .def("getActionSequence", [](const std::shared_ptr<AgentBrain>& agent, size_t length) {
            std::vector<Action> actions;
            // In a full implementation, this would generate action sequences
            return actions;
        }, py::arg("length"), "Generate action sequences")
        // Enhanced neuromodulation
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system with rewards")
        // Enhanced subsystem control
        .def("enableAllSystems", [](std::shared_ptr<AgentBrain>& agent) {
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
        }, "Enable all learning subsystems")
        .def("disableAllSystems", [](std::shared_ptr<AgentBrain>& agent) {
            agent->enableRewardModulation(false);
            agent->enableStructuralPlasticity(false);
            agent->enableDevelopment(false);
            agent->enableCuriosity(false);
        }, "Disable all learning subsystems")
        // Enhanced statistics and monitoring
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity-driven exploration level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty detection level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error signal")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get overall neuromodulation level")
        .def("getLearningStats", [](const std::shared_ptr<AgentBrain>& agent) {
            // Enhanced learning statistics
            std::ostringstream oss;
            oss << "Agent Learning Statistics:\n";
            oss << "  Curiosity: " << agent->getCuriosityLevel() << "\n";
            oss << "  Novelty: " << agent->getNoveltyLevel() << "\n";
            oss << "  Prediction Error: " << agent->getPredictionError() << "\n";
            oss << "  Neuromodulation: " << agent->getNeuromodulationLevel() << "\n";
            oss << "  Developmental Stage: " << static_cast<int>(agent->getDevelopmentalStage()) << "\n";
            return oss.str();
        }, "Get comprehensive learning statistics");

    // =======================================================================
    // Convenience Factory Functions
    // =======================================================================

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->enableAllSystems();
        return agent;
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("createSimpleAgent", [](std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto config = createDefaultConfig();
        auto brain = std::make_shared<Brain>(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        agent->enableAllSystems();
        return agent;
    }, py::arg("world"), "Create a simple agent with default configuration");

    m.def("runSimulation", [](std::shared_ptr<Brain> brain, size_t steps, size_t reportInterval = 100) {
        std::vector<std::string> stats;
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
            
            if (i % reportInterval == 0) {
                std::ostringstream oss;
                oss << "Step " << i << ": " 
                    << brain->getFiringNeuronCount() << " firing, "
                    << brain->getAverageFiringRate() << " Hz average";
                stats.push_back(oss.str());
            }
        }
        return stats;
    }, py::arg("brain"), py::arg("steps"), py::arg("reportInterval") = 100,
        "Run a complete simulation and return progress statistics");

    // =======================================================================
    // Utility Functions
    // =======================================================================

    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, size_t steps) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::ostringstream oss;
        oss << "Benchmark completed " << steps << " steps in " 
            << duration.count() << " ms\n";
        oss << "Average time per step: " 
            << (duration.count() / static_cast<double>(steps)) << " ms\n";
        oss << "Steps per second: " 
            << (steps / (duration.count() / 1000.0)) << "\n";
        
        return oss.str();
    }, py::arg("brain"), py::arg("steps"), "Benchmark brain performance");

    m.def("getVersion", []() {
        return "NLM Enhanced Python Bindings v2.0";
    }, "Get the version of the bindings");

    // =======================================================================
    // Type aliases for convenience
    // =======================================================================

    py::class_<std::vector<NeuronId>>("NeuronIdList")
        .def(py::init<>());
    
    py::class_<std::vector<SynapseId>>("SynapseIdList")
        .def(py::init<>());
    
    py::class_<std::vector<RegionId>>("RegionIdList")
        .def(py::init<>());
    
    py::class_<std::vector<Action>>("ActionList")
        .def(py::init<>());
    
    py::class_<std::vector<float>>("FloatList")
        .def(py::init<>());

    // =======================================================================
    // Constants and Enums
    // =======================================================================

    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_SYNAPSE_DENSITY") = 0.1f;
    m.attr("DEFAULT_SIMULATION_TIMESTEP") = 0.001f;
    m.attr("MIN_WEIGHT") = -1.0f;
    m.attr("MAX_WEIGHT") = 1.0f;
    m.attr("MIN_EFFICIENCY") = 0.0f;
    m.attr("MAX_EFFICIENCY") = 2.0f;

    // =======================================================================
    // End of module
    // =======================================================================

} // namespace nlm

} // namespace pybind11