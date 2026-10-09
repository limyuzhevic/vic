// Advanced Python bindings with better API design

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/agent/AgentBrain.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations for helper functions
class SmartBrainWrapper;
class EnhancedConfigWrapper;

/**
 * @brief Enhanced Python API for NLM with improved usability
 * 
 * This module provides a high-level Python API for the NLM neural simulation framework.
 * It includes:
 * - Simplified configuration and brain creation
 * - Context managers for resource management
 * - Better error handling and type safety
 * - Advanced features for power users
 * - Comprehensive documentation
 */

// Helper classes for better Python API design
class SmartBrainWrapper {
public:
    std::shared_ptr<Brain> brain;
    
    SmartBrainWrapper(std::shared_ptr<Config> config) : brain(std::make_shared<Brain>(config)) {}
    
    // Context manager support
    void __enter__() {
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
    }
    
    void __exit__(int, const char*, const char*, int) {
        // Cleanup handled by shared_ptr
    }
    
    // Convenience methods
    void train(SimulationStep steps, const std::string& description = "") {
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step);
        }
        if (!description.empty()) {
            std::cout << "Training completed: " << description << std::endl;
        }
    }
    
    void runEpisode(const std::string& description = "") {
        // Reset for new episode
        brain->reset();
        brain->initialize();
        
        if (!description.empty()) {
            std::cout << "Starting episode: " << description << std::endl;
        }
    }
    
    // Statistics helpers
    size_t getNeuronCount() const { return brain->getTotalNeuronCount(); }
    size_t getSynapseCount() const { return brain->getTotalSynapseCount(); }
    size_t getSpikeCount() const { return brain->getTotalSpikeCount(); }
    float getFiringRate() const { return brain->getAverageFiringRate(); }
    
    // State management
    void save(const std::string& filename) { brain->save(filename); }
    bool load(const std::string& filename) { return brain->load(filename); }
    
    // Access to underlying systems
    auto getConfig() { return brain->getConfig(); }
    auto getWorkingMemory() { return brain->getWorkingMemory(); }
    auto getEpisodicMemory() { return brain->getEpisodicMemory(); }
    auto getAssociativeMemory() { return brain->getAssociativeMemory(); }
};

class EnhancedConfigWrapper {
public:
    std::shared_ptr<Config> config;
    
    EnhancedConfigWrapper() : config(std::make_shared<Config>()) {}
    
    // Chainable configuration methods for better API
    EnhancedConfigWrapper& setRandomSeed(uint64_t seed) {
        config->set("random_seed", seed);
        return *this;
    }
    
    EnhancedConfigWrapper& setNeuronCount(size_t count) {
        config->set("neuron_count", count);
        return *this;
    }
    
    EnhancedConfigWrapper& setRegionCount(size_t count) {
        config->set("region_count", count);
        return *this;
    }
    
    EnhancedConfigWrapper& setConnectionProbability(float prob) {
        config->set("connection_probability", prob);
        return *this;
    }
    
    EnhancedConfigWrapper& setSTDPParams(float ltp, float ltd, float tau) {
        config->set("stdp_ltp_weight", ltp);
        config->set("stdp_ltd_weight", ltd);
        config->set("stdp_tau", tau);
        return *this;
    }
    
    EnhancedConfigWrapper& setStructuralPlasticity(float synap, float prune) {
        config->set("synaptogenesis_rate", synap);
        config->set("pruning_rate", prune);
        return *this;
    }
    
    EnhancedConfigWrapper& setMemoryParams(size_t workCap, size_t epiMax, size_t replayInterval) {
        config->set("working_memory_capacity", workCap);
        config->set("episodic_memory_max_episodes", epiMax);
        config->set("replay_interval", replayInterval);
        return *this;
    }
    
    // Convenience factory methods
    static EnhancedConfigWrapper createDefault() {
        EnhancedConfigWrapper wrapper;
        return wrapper;
    }
    
    static EnhancedConfigWrapper createForSpeed() {
        EnhancedConfigWrapper wrapper;
        wrapper.setNeuronCount(500)
               .setRegionCount(1)
               .setConnectionProbability(0.15f);
        return wrapper;
    }
    
    static EnhancedConfigWrapper createForAccuracy() {
        EnhancedConfigWrapper wrapper;
        wrapper.setNeuronCount(5000)
               .setRegionCount(3)
               .setConnectionProbability(0.05f)
               .setSTDPParams(0.02f, 0.018f, 25.0f)
               .setMemoryParams(1000, 5000, 50);
        return wrapper;
    }
    
    static EnhancedConfigWrapper createForLearning() {
        EnhancedConfigWrapper wrapper;
        wrapper.setNeuronCount(2000)
               .setRegionCount(2)
               .setConnectionProbability(0.1f)
               .setSTDPParams(0.015f, 0.013f, 20.0f)
               .setMemoryParams(500, 2000, 100)
               .setStructuralPlasticity(0.00005f, 0.000005f);
        return wrapper;
    }
    
    // Getters with convenience methods
    uint64_t getRandomSeed() const {
        uint64_t seed;
        config->get("random_seed", seed);
        return seed;
    }
    
    size_t getNeuronCount() const {
        size_t count;
        config->get("neuron_count", count);
        return count;
    }
    
    float getTimestep() const {
        float timestep;
        config->get("simulation_timestep", timestep);
        return timestep;
    }
    
    // Serialization
    std::string toJSON() const {
        return config->serialize();
    }
    
    bool fromJSON(const std::string& json) {
        return config->deserialize(json);
    }
    
    // Validation
    bool validate() const {
        return config->validate();
    }
    
    std::string getValidationErrors() const {
        auto errors = config->getValidationErrors();
        std::stringstream ss;
        for (const auto& error : errors) {
            ss << error << std::endl;
        }
        return ss.str();
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python API
        ================================================
        
        A modern, Pythonic API for the NLM neural simulation framework.
        
        Key improvements over the original API:
        
        • Simplified configuration with chainable methods
        • Context manager support for automatic resource management
        • Enhanced error handling with meaningful exceptions
        • Convenience factory methods for different use cases
        • Better performance with optimized bindings
        • Comprehensive documentation and examples
        
        Usage examples:
        
        Basic usage (simplified):
        ```python
        import pynlm
        
        # Create brain with default configuration
        with pynlm.create_brain() as brain:
            for step in range(1000):
                brain.step(step)
            
            print(f"Neurons: {brain.getNeuronCount()}")
            print(f"Spikes: {brain.getSpikeCount()}")
        ```
        
        Advanced configuration (chainable API):
        ```python
        import pynlm
        
        config = (pynlm.EnhancedConfig()
                 .setRandomSeed(42)
                 .setNeuronCount(2000)
                 .setRegionCount(2)
                 .setConnectionProbability(0.1)
                 .setSTDPParams(0.015, 0.013, 20.0))
        
        with pynlm.create_brain(config) as brain:
            brain.train(500, "Learning with custom STDP")
        ```
        
        Specialized configurations:
        ```python
        # Speed-focused configuration for testing
        config = pynlm.EnhancedConfig.createForSpeed()
        
        # Accuracy-focused configuration for research
        config = pynlm.EnhancedConfig.createForAccuracy()
        
        # Learning-focused configuration for exploration
        config = pynlm.EnhancedConfig.createForLearning()
        ```
        
        The enhanced API provides:
        - Type safety and better error messages
        - Resource management with context managers
        - Multiple configuration presets
        - Convenience methods for common operations
        - Comprehensive statistics and monitoring
    )pbdoc";
    
    // Register exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
    
    // Enhanced configuration class
    py::class_<EnhancedConfigWrapper>(m, "EnhancedConfig", 
        R"pbdoc(Advanced configuration class with chainable API methods)"pbdoc")
        .def(py::init<>())
        .def("setRandomSeed", &EnhancedConfigWrapper::setRandomSeed, 
             py::arg("seed"), "Set random seed for reproducibility")
        .def("setNeuronCount", &EnhancedConfigWrapper::setNeuronCount,
             py::arg("count"), "Set number of neurons")
        .def("setRegionCount", &EnhancedConfigWrapper::setRegionCount,
             py::arg("count"), "Set number of neural regions")
        .def("setConnectionProbability", &EnhancedConfigWrapper::setConnectionProbability,
             py::arg("prob"), "Set synaptic connection probability")
        .def("setSTDPParams", &EnhancedConfigWrapper::setSTDPParams,
             py::arg("ltp"), py::arg("ltd"), py::arg("tau"),
             "Set STDP plasticity parameters")
        .def("setStructuralPlasticity", &EnhancedConfigWrapper::setStructuralPlasticity,
             py::arg("synaptogenesis"), py::arg("pruning"),
             "Set structural plasticity rates")
        .def("setMemoryParams", &EnhancedConfigWrapper::setMemoryParams,
             py::arg("work_capacity"), py::arg("epi_max"), py::arg("replay_interval"),
             "Set memory system parameters")
        .def("createDefault", &EnhancedConfigWrapper::createDefault,
             "Create default configuration")
        .def("createForSpeed", &EnhancedConfigWrapper::createForSpeed,
             "Create configuration optimized for speed/testing")
        .def("createForAccuracy", &EnhancedConfigWrapper::createForAccuracy,
             "Create configuration optimized for accuracy")
        .def("createForLearning", &EnhancedConfigWrapper::createForLearning,
             "Create configuration optimized for learning")
        .def("getRandomSeed", &EnhancedConfigWrapper::getRandomSeed,
             "Get random seed")
        .def("getNeuronCount", &EnhancedConfigWrapper::getNeuronCount,
             "Get number of neurons")
        .def("getTimestep", &EnhancedConfigWrapper::getTimestep,
             "Get simulation timestep")
        .def("toJSON", &EnhancedConfigWrapper::toJSON,
             "Export configuration to JSON format")
        .def("fromJSON", &EnhancedConfigWrapper::fromJSON,
             py::arg("json"), "Import configuration from JSON format")
        .def("validate", &EnhancedConfigWrapper::validate,
             "Validate configuration")
        .def("getValidationErrors", &EnhancedConfigWrapper::getValidationErrors,
             "Get validation error messages");
    
    // Enhanced brain wrapper with context manager support
    py::class_<SmartBrainWrapper>(m, "SmartBrain", 
        R"pbdoc(Advanced brain wrapper with context manager support and convenience methods)"pbdoc")
        .def(py::init<std::shared_ptr<Config>>())
        .def("__enter__", &SmartBrainWrapper::__enter__,
             "Enter context manager (initialize brain)")
        .def("__exit__", &SmartBrainWrapper::__exit__,
             "Exit context manager")
        .def("train", &SmartBrainWrapper::train,
             py::arg("steps"), py::arg("description"),
             "Train brain for specified number of steps")
        .def("runEpisode", &SmartBrainWrapper::runEpisode,
             py::arg("description"), "Run a new episode")
        .def("getNeuronCount", &SmartBrainWrapper::getNeuronCount,
             "Get total neuron count")
        .def("getSynapseCount", &SmartBrainWrapper::getSynapseCount,
             "Get total synapse count")
        .def("getSpikeCount", &SmartBrainWrapper::getSpikeCount,
             "Get total spike count")
        .def("getFiringRate", &SmartBrainWrapper::getFiringRate,
             "Get average firing rate")
        .def("save", &SmartBrainWrapper::save,
             py::arg("filename"), "Save brain state to file")
        .def("load", &SmartBrainWrapper::load,
             py::arg("filename"), "Load brain state from file")
        .def("getConfig", &SmartBrainWrapper::getConfig,
             "Get configuration object")
        .def("getWorkingMemory", &SmartBrainWrapper::getWorkingMemory,
             "Get working memory system")
        .def("getEpisodicMemory", &SmartBrainWrapper::getEpisodicMemory,
             "Get episodic memory system")
        .def("getAssociativeMemory", &SmartBrainWrapper::getAssociativeMemory,
             "Get associative memory system");
    
    // Convenience factory functions
    m.def("createDefaultConfig", []() {
        return EnhancedConfigWrapper::createDefault().config;
    }, "Create default configuration");
    
    m.def("createSpeedConfig", []() {
        return EnhancedConfigWrapper::createForSpeed().config;
    }, "Create speed-optimized configuration");
    
    m.def("createAccuracyConfig", []() {
        return EnhancedConfigWrapper::createForAccuracy().config;
    }, "Create accuracy-optimized configuration");
    
    m.def("createLearningConfig", []() {
        return EnhancedConfigWrapper::createForLearning().config;
    }, "Create learning-optimized configuration");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) {
        return SmartBrainWrapper(config);
    }, py::arg("config"), "Create smart brain wrapper");
    
    m.def("createBrain", []() {
        return SmartBrainWrapper(std::make_shared<Config>());
    }, "Create brain with default configuration");
    
    // Original API functions (for backward compatibility)
    m.def("createBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create brain (original API)");
    
    m.def("createDefaultConfig", []() {
        return std::make_shared<Config>();
    }, "Create default configuration (original API)");
    
    // Type aliases and constants
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)"pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=);
    
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)"pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index);
    
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)"pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index);
    
    // Neuron type enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)"pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    // Action type enum
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)"pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();
    
    // Developmental stage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)"pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    // Constants
    m.attr("DEFAULT_RESTING_POTENTIAL") = Constants::DEFAULT_RESTING_POTENTIAL;
    m.attr("DEFAULT_THRESHOLD") = Constants::DEFAULT_THRESHOLD;
    m.attr("DEFAULT_RESET_POTENTIAL") = Constants::DEFAULT_RESET_POTENTIAL;
    m.attr("DEFAULT_LEAK_CONDUCTANCE") = Constants::DEFAULT_LEAK_CONDUCTANCE;
    m.attr("DEFAULT_REFECTORY_PERIOD") = Constants::DEFAULT_REFECTORY_PERIOD;
    m.attr("DEFAULT_LTP_WEIGHT") = Constants::DEFAULT_LTP_WEIGHT;
    m.attr("DEFAULT_LTD_WEIGHT") = Constants::DEFAULT_LTD_WEIGHT;
    m.attr("DEFAULT_STDP_TIME_CONSTANT") = Constants::DEFAULT_STDP_TIME_CONSTANT;
    
    // Error constants
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::out_of_range& e) {
            PyErr_SetString(PyExc_IndexError, e.what());
        }
    });
    
    // Module metadata
    m.attr("__version__") = "2.0.0"; // Enhanced API version
    m.attr("__description__") = "Enhanced NLM Python API with improved usability and documentation";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__license__") = "MIT";
}

} // namespace nlm
