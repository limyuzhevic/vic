#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/eigen.h>
#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <map>
#include <unordered_map>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Custom exception types
class NLMException : public std::exception {
public:
    NLMException(const std::string& message) : message_(message) {}
    const char* what() const noexcept override { return message_.c_str(); }
private:
    std::string message_;
};

class ConfigurationError : public NLMException {
public:
    ConfigurationError(const std::string& message) : NLMException(message) {}
};

class SimulationError : public NLMException {
public:
    SimulationError(const std::string& message) : NLMException(message) {}
};

class BrainInitializationError : public SimulationError {
public:
    BrainInitializationError(const std::string& message) : SimulationError(message) {}
};

namespace py = pybind11;
namespace nlm {

// Custom type for configuration presets
struct LearningProfile {
    std::string name;
    std::string description;
    std::map<std::string, std::variant<int, double, bool>> settings;
    
    LearningProfile(const std::string& n, const std::string& d,
                   const std::map<std::string, std::variant<int, double, bool>>& s)
        : name(n), description(d), settings(s) {}
};

// Context manager for automatic brain cleanup
class BrainSimulationManager {
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
    bool initialized_ = false;
    
public:
    BrainSimulationManager(std::shared_ptr<Brain> brain) : brain_(brain) {}
    BrainSimulationManager(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)
        : brain_(brain), world_(world) {}
    
    ~BrainSimulationManager() {
        try {
            if (agent_) agent_->reset();
            if (brain_) brain_->reset();
        } catch (...) {
            // Silent cleanup to avoid exceptions in destructor
        }
    }
    
    BrainSimulationManager& withAgentBrain() {
        agent_ = std::make_shared<AgentBrain>(brain_);
        return *this;
    }
    
    BrainSimulationManager& withWorld(std::shared_ptr<SimpleWorld> world) {
        world_ = world;
        if (agent_) agent_->initialize(world_);
        return *this;
    }
    
    BrainSimulationManager& enableAllSubsystems() {
        if (agent_) {
            agent_->enableRewardModulation(true);
            agent_->enableStructuralPlasticity(true);
            agent_->enableDevelopment(true);
            agent_->enableCuriosity(true);
        }
        return *this;
    }
    
    void runSteps(size_t num_steps) {
        if (!brain_) throw BrainInitializationError("Brain not initialized");
        
        for (size_t step = 0; step < num_steps; ++step) {
            brain_->step(step);
        }
    }
    
    void runEpisode(size_t num_steps) {
        if (!agent_ || !world_) {
            throw SimulationError("Agent or world not configured");
        }
        
        for (size_t step = 0; step < num_steps; ++step) {
            // World update
            world_->update(0.1);
            
            // Sensory input
            auto percept = world_->getSensoryPercept();
            agent_->processSensoryInput(percept);
            
            // Brain step
            brain_->step(step);
            
            // Motor output
            auto action = agent_->decodeMotorCommand();
            world_->applyMotorCommand(action, world_->getSimulationTime());
        }
    }
    
    // Pythonic property access
    std::shared_ptr<Brain>& getBrain() { return brain_; }
    std::shared_ptr<AgentBrain>& getAgent() { return agent_; }
    std::shared_ptr<SimpleWorld>& getWorld() { return world_; }
    
    bool isInitialized() const { return initialized_; }
};

// Factory functions for common configurations
std::map<std::string, std::variant<int, double, bool>> createLearningConfig(const std::string& profile_name) {
    static const std::map<std::string, std::map<std::string, std::variant<int, double, bool>>> presets = {
        {"exploration", {
            {"brain.neuron_count", 1000},
            {"brain.synapse_density", 0.1},
            {"neuromod.curiosity.enable", true},
            {"neuromod.novelty.enable", true},
            {"plasticity.structural.enable", true},
            {"development.enabled", true}
        }},
        {"development", {
            {"brain.neuron_count", 500},
            {"brain.synapse_density", 0.05},
            {"neuromod.curiosity.enable", true},
            {"neuromod.novelty.enable", true},
            {"plasticity.structural.enable", true},
            {"development.enabled", true},
            {"development.critical_period", 100}
        }},
        {"stable", {
            {"brain.neuron_count", 2000},
            {"brain.synapse_density", 0.15},
            {"neuromod.curiosity.enable", false},
            {"neuromod.novelty.enable", false},
            {"plasticity.structural.enable", false},
            {"development.enabled", false},
            {"plasticity.stdp.enable", true}
        }},
        {"creative", {
            {"brain.neuron_count", 1500},
            {"brain.synapse_density", 0.12},
            {"neuromod.curiosity.enable", true},
            {"neuromod.novelty.enable", true},
            {"plasticity.structural.enable", true},
            {"development.enabled", true},
            {"plasticity.hebbian.enable", true}
        }}
    };
    
    auto it = presets.find(profile_name);
    if (it != presets.end()) {
        return it->second;
    }
    
    throw ConfigurationError("Unknown learning profile: " + profile_name);
}

// Performance optimization profiles
std::map<std::string, std::variant<int, double, bool>> createPerformanceConfig(const std::string& profile_name) {
    static const std::map<std::string, std::map<std::string, std::variant<int, double, bool>>> presets = {
        {"balanced", {
            {"performance.profile", "balanced"},
            {"performance.optimization_level", 2},
            {"performance.simd.enable", true},
            {"performance.parallel.enable", true},
            {"performance.memory_pool.enable", true},
            {"performance.caching.enable", true},
            {"learning.plasticity.adaptive.enable", true},
            {"learning.curiosity.weight", 0.5},
            {"learning.novelty.sensitivity", 1.0},
            {"memory.pools.enable", true},
            {"memory.consolidation.enabled", true}
        }},
        {"speed", {
            {"performance.profile", "speed"},
            {"performance.optimization_level", 3},
            {"performance.simd.enable", true},
            {"performance.parallel.enable", true},
            {"performance.memory_pool.enable", false}, // Disable for speed
            {"performance.caching.enable", true},
            {"learning.plasticity.adaptive.enable", false},
            {"learning.curiosity.weight", 0.3},
            {"learning.novelty.sensitivity", 0.5},
            {"memory.pools.enable", false}, // Disable to reduce overhead
            {"memory.consolidation.enabled", false}
        }},
        {"memory", {
            {"performance.profile", "memory"},
            {"performance.optimization_level", 3},
            {"performance.simd.enable", false}, // Disable for memory efficiency
            {"performance.parallel.enable", false}, // Disable to save memory
            {"performance.memory_pool.enable", true},
            {"performance.caching.enable", true},
            {"learning.plasticity.adaptive.enable", false},
            {"learning.curiosity.weight", 0.2},
            {"learning.novelty.sensitivity", 0.3},
            {"memory.pools.enable", true},
            {"memory.consolidation.enabled", true}
        }},
        {"accuracy", {
            {"performance.profile", "accuracy"},
            {"performance.optimization_level", 3},
            {"performance.simd.enable", true},
            {"performance.parallel.enable", true},
            {"performance.memory_pool.enable", true},
            {"performance.caching.enable", true},
            {"learning.plasticity.adaptive.enable", true},
            {"learning.curiosity.weight", 0.7},
            {"learning.novelty.sensitivity", 1.5},
            {"memory.pools.enable", true},
            {"memory.consolidation.enabled", true}
        }},
        {"adaptive", {
            {"performance.profile", "adaptive"},
            {"performance.optimization_level", 3},
            {"performance.simd.enable", true},
            {"performance.parallel.enable", true},
            {"performance.memory_pool.enable", true},
            {"performance.caching.enable", true},
            {"learning.plasticity.adaptive.enable", true},
            {"learning.curiosity.weight", 0.5},
            {"learning.novelty.sensitivity", 1.0},
            {"memory.pools.enable", true},
            {"memory.consolidation.enabled", true}
        }}
    };
    
    auto it = presets.find(profile_name);
    if (it != presets.end()) {
        return it->second;
    }
    
    throw ConfigurationError("Unknown performance profile: " + profile_name);
}

// Research-specific configurations
std::map<std::string, std::variant<int, double, bool>> createResearchConfig(const std::string& experiment_type) {
    static const std::map<std::string, std::map<std::string, std::variant<int, double, bool>>> presets = {
        {"attention", {
            {"performance.profile", "research"},
            {"performance.optimization_level", 1},
            {"experiment.research.focus", "attention"},
            {"brain.neuron_count", 500},
            {"brain.synapse_density", 0.05},
            {"learning.plasticity.adaptive.enable", true},
            {"learning.curiosity.weight", 0.7},
            {"learning.novelty.sensitivity", 1.2},
            {"memory.working_memory.size", 200},
            {"memory.episodic_memory.capacity", 2000},
            {"prediction.horizon", 20},
            {"prediction.confidence_threshold", 0.9},
            {"development.critical_period.enabled", true},
            {"development.critical_period.duration", 500}
        }},
        {"memory", {
            {"performance.profile", "research"},
            {"performance.optimization_level", 1},
            {"experiment.research.focus", "memory"},
            {"brain.neuron_count", 300},
            {"brain.synapse_density", 0.03},
            {"learning.plasticity.adaptive.enable", true},
            {"learning.curiosity.weight", 0.4},
            {"learning.novelty.sensitivity", 1.5},
            {"memory.working_memory.size", 300},
            {"memory.episodic_memory.capacity", 5000},
            {"memory.consolidation.enabled", true},
            {"prediction.horizon", 15},
            {"prediction.confidence_threshold", 0.85},
            {"development.maturation.rate", 0.02}
        }},
        {"prediction", {
            {"performance.profile", "research"},
            {"performance.optimization_level", 1},
            {"experiment.research.focus", "prediction"},
            {"brain.neuron_count", 400},
            {"brain.synapse_density", 0.08},
            {"learning.plasticity.adaptive.enable", true},
            {"learning.curiosity.weight", 0.6},
            {"learning.novelty.sensitivity", 1.3},
            {"memory.working_memory.size", 150},
            {"memory.episodic_memory.capacity", 1000},
            {"prediction.horizon", 50},
            {"prediction.confidence_threshold", 0.8},
            {"prediction.error.scaling.dynamic", true},
            {"neuromodulation.dopamine.scale", 1.2},
            {"development.aging.enable", true}
        }},
        {"development", {
            {"performance.profile", "research"},
            {"performance.optimization_level", 1},
            {"experiment.research.focus", "development"},
            {"brain.neuron_count", 600},
            {"brain.synapse_density", 0.06},
            {"learning.plasticity.adaptive.enable", true},
            {"learning.curiosity.weight", 0.8},
            {"learning.novelty.sensitivity", 0.8},
            {"memory.working_memory.size", 100},
            {"memory.episodic_memory.capacity", 800},
            {"development.critical_period.enabled", true},
            {"development.critical_period.duration", 800},
            {"development.maturation.rate", 0.015},
            {"development.aging.enable", true},
            {"development.aging.rate", 0.002}
        }}
    };
    
    auto it = presets.find(experiment_type);
    if (it != presets.end()) {
        return it->second;
    }
    
    throw ConfigurationError("Unknown research experiment type: " + experiment_type);
}

// Production-ready configurations
std::map<std::string, std::variant<int, double, bool>> createProductionConfig(const std::string& tier) {
    static const std::map<std::string, std::map<std::string, std::variant<int, double, bool>>> presets = {
        {"prototype", {
            {"performance.profile", "production"},
            {"performance.optimization_level", 1},
            {"production.tier", "prototype"},
            {"enable_profiling", true},
            {"enable_memory_tracking", true},
            {"performance.simd.enable", true},
            {"performance.parallel.enable", false}, // Conservative in prototype
            {"learning.plasticity.adaptive.enable", true},
            {"learning.curiosity.weight", 0.4},
            {"learning.novelty.sensitivity", 1.0},
            {"memory.pools.enable", true},
            {"memory.consolidation.enabled", true},
            {"checkpoint_interval", 1000},
            {"enable_checkpointing", true},
            {"enable_replay", false} // Disable replay in prototype for simplicity
        }},
        {"staging", {
            {"performance.profile", "production"},
            {"performance.optimization_level", 2},
            {"production.tier", "staging"},
            {"enable_profiling", true},
            {"enable_memory_tracking", true},
            {"performance.simd.enable", true},
            {"performance.parallel.enable", true},
            {"learning.plasticity.adaptive.enable", true},
            {"learning.curiosity.weight", 0.5},
            {"learning.novelty.sensitivity", 1.0},
            {"memory.pools.enable", true},
            {"memory.consolidation.enabled", true},
            {"checkpoint_interval", 5000},
            {"enable_checkpointing", true},
            {"enable_replay", true}
        }},
        {"production", {
            {"performance.profile", "production"},
            {"performance.optimization_level", 3},
            {"production.tier", "production"},
            {"enable_profiling", false},
            {"enable_memory_tracking", false},
            {"performance.simd.enable", true},
            {"performance.parallel.enable", true},
            {"performance.memory_pool.enable", true},
            {"performance.caching.enable", true},
            {"learning.plasticity.adaptive.enable", true},
            {"learning.curiosity.weight", 0.3},
            {"learning.novelty.sensitivity", 0.7},
            {"memory.pools.enable", true},
            {"memory.consolidation.enabled", true},
            {"checkpoint_interval", 10000},
            {"enable_checkpointing", true},
            {"enable_replay", true}
        }}
    };
    
    auto it = presets.find(tier);
    if (it != presets.end()) {
        return it->second;
    }
    
    throw ConfigurationError("Unknown production tier: " + tier);
}

// Existing learning configuration (keep for backward compatibility)
std::map<std::string, std::variant<int, double, bool>> createLearningConfig(const std::string& profile_name) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        --------------------------------------------------------
        
        A comprehensive Python API for the NLM neural simulation framework.
        This enhanced version provides Pythonic interfaces, better error handling,
        and powerful convenience functions for both beginners and advanced users.
        
        Key Features:
        - Pythonic property access with descriptors
        - Context managers for automatic resource management
        - Enhanced sensory input support (numpy arrays, tensors, lists)
        - Learning profile presets (exploration, development, stable, creative)
        - Advanced neuromodulation control
        - Custom configuration templates
        - Comprehensive exception hierarchy
        - Performance monitoring and profiling tools
        - Factory methods for common scenarios
        
        Usage Examples:
        
        # Simple brain creation
        brain = pynlm.createBrain()
        
        # Using learning profiles
        brain = pynlm.createBrain(pynlm.LearningProfile.CREATIVE)
        
        # Context manager for automatic cleanup
        with pynlm.simulation(brain) as sim:
            sim.runSteps(100)
        
        # Advanced sensory input
        import numpy as np
        vision_data = np.random.rand(8, 8, 3)
        percept = pynlm.SensoryInput(vision_data)
        brain.receiveSensoryInput(percept)
        
        # Enhanced neuromodulation
        brain.enable_learning(motivation=0.8, exploration=0.5)
    )pbdoc";

    // Custom exception hierarchy
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown NLM error");
        }
    });

    // Custom exception types
    py::class_<NLMException, std::exception>(m, "NLMException")
        .def(py::init<const std::string&>());

    py::class_<ConfigurationError, NLMException>(m, "ConfigurationError")
        .def(py::init<const std::string&>());

    py::class_<SimulationError, NLMException>(m, "SimulationError")
        .def(py::init<const std::string&>());

    py::class_<BrainInitializationError, SimulationError>(m, "BrainInitializationError")
        .def(py::init<const std::string&>());

    // NeuronId with enhanced Python support
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // Enhanced enum definitions
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

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
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

    // Enhanced Config class with Pythonic interface
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system with Pythonic interface)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON/YAML file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON/YAML file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", &Config::get<std::string>, py::arg("key"),
             "Get configuration value (returns None if not found)")
        .def("getInt", &Config::getOr<int>, py::arg("key"), py::arg("default"),
             "Get integer value")
        .def("getFloat", &Config::getOr<double>, py::arg("key"), py::arg("default"),
             "Get float value")
        .def("getBool", &Config::getOr<bool>, py::arg("key"), py::arg("default"),
             "Get boolean value")
        .def("set", static_cast<void (Config::*)(const std::string&, const std::variant<int, double, bool, std::string>&)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value")
        .def("setInt", static_cast<void (Config::*)(const std::string&, int)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set integer value")
        .def("setFloat", static_cast<void (Config::*)(const std::string&, float)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set float value")
        .def("setBool", static_cast<void (Config::*)(const std::string&, bool)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set boolean value")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("validate", [](const Config& self) {
            std::vector<std::string> errors;
            // Validation logic would go here
            if (errors.empty()) {
                return true;
            } else {
                throw ConfigurationError("Configuration validation failed:\n" + 
                                       std::accumulate(errors.begin(), errors.end(), std::string(),
                                                      [](const std::string& a, const std::string& b) { return a + "\n" + b; }));
            }
        }, "Validate configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__contains__", &Config::has,
             "Check if key exists in configuration");

    // Enhanced SensoryInput with automatic type conversion
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input with enhanced Python support)pbdoc")
        .def(py::init<>())
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision with automatic data conversion
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input with automatic data conversion)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def(py::init([](const std::vector<std::vector<std::vector<float>>>& data) {
            Vision v;
            size_t height = data.size();
            if (height > 0) {
                size_t width = data[0].size();
                if (width > 0) {
                    v.setData(data);
                }
            }
            return v;
        }), py::arg("data"),
             "Create Vision from 3D list (width, height, channels)")
        .def(py::init([](const py::array_t<float>& array) {
            Vision v;
            if (array.ndim() == 3) {
                size_t width = array.shape(0);
                size_t height = array.shape(1);
                size_t channels = array.shape(2);
                v = Vision(width, height, channels);
                auto buf = array.request();
                float* ptr = (float*)buf.ptr;
                std::vector<float> data(ptr, ptr + width * height * channels);
                v.setData(data);
            }
            return v;
        }), py::arg("array"),
             "Create Vision from numpy array (width, height, channels)")
        .def("setData", static_cast<void (Vision::*)(const std::vector<float>&)>(&Vision::setData),
             py::arg("data"),
             "Set data from vector")
        .def("setData", [](Vision& self, const py::array_t<float>& array) {
            auto buf = array.request();
            float* ptr = (float*)buf.ptr;
            std::vector<float> data(ptr, ptr + buf.size);
            self.setData(data);
        }, py::arg("array"),
             "Set data from numpy array")
        .def("setData", [](Vision& self, const std::vector<std::vector<std::vector<float>>>& data) {
            std::vector<float> flat_data;
            for (const auto& plane : data) {
                for (const auto& row : plane) {
                    flat_data.insert(flat_data.end(), row.begin(), row.end());
                }
            }
            self.setData(flat_data);
        }, py::arg("data"),
             "Set data from nested Python list")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("to_numpy", [](const Vision& self) {
            py::array_t<float> array({self.getWidth(), self.getHeight(), self.getChannels()});
            auto buf = array.request();
            float* ptr = (float*)buf.ptr;
            std::vector<float> data = self.getData();
            std::copy(data.begin(), data.end(), ptr);
            return array;
        }, "Convert Vision data to numpy array");

    // Audio with automatic data conversion
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input with enhanced Python support)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def(py::init([](const std::vector<float>& data, size_t sampleRate) {
            Audio a(sampleRate, data.size());
            a.setData(data);
            return a;
        }), py::arg("data"), py::arg("sampleRate") = 44100,
             "Create Audio from vector with sample rate")
        .def(py::init([](const py::array_t<float>& array, size_t sampleRate) {
            Audio a(sampleRate, array.size());
            auto buf = array.request();
            float* ptr = (float*)buf.ptr;
            std::vector<float> data(ptr, ptr + buf.size);
            a.setData(data);
            return a;
        }), py::arg("array"), py::arg("sampleRate") = 44100,
             "Create Audio from numpy array with sample rate")
        .def("setData", static_cast<void (Audio::*)(const std::vector<float>&)>(&Audio::setData),
             py::arg("data"),
             "Set data from vector")
        .def("setData", [](Audio& self, const py::array_t<float>& array) {
            auto buf = array.request();
            float* ptr = (float*)buf.ptr;
            std::vector<float> data(ptr, ptr + buf.size);
            self.setData(data);
        }, py::arg("array"),
             "Set data from numpy array")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("to_numpy", [](const Audio& self) {
            py::array_t<float> array({self.getNumSamples()});
            auto buf = array.request();
            float* ptr = (float*)buf.ptr;
            std::vector<float> data = self.getData();
            std::copy(data.begin(), data.end(), ptr);
            return array;
        }, "Convert Audio data to numpy array");

    // Enhanced InternalSignals
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("addSignals", [](InternalSignals& self, const std::vector<float>& values) {
            for (float v : values) self.addSignal(v);
        }, py::arg("values"),
             "Add multiple signals from vector")
        .def("addSignals", [](InternalSignals& self, const py::array_t<float>& array) {
            auto buf = array.request();
            float* ptr = (float*)buf.ptr;
            std::vector<float> values(ptr, ptr + buf.size);
            for (float v : values) self.addSignal(v);
        }, py::arg("array"),
             "Add multiple signals from numpy array")
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignals", &InternalSignals::getData, "Get all signals")
        .def("getSignalCount", &InternalSignals::getDimensions, "Get number of signals");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def(py::init<ActionType, const py::array_t<float>&>(), py::arg("type"), py::arg("parameters"),
             "Create Action with type and numpy array parameters")
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", static_cast<void (Action::*)(const std::vector<float>&)>(&Action::setParameters),
             py::arg("params"),
             "Set parameters from vector")
        .def("setParameters", [](Action& self, const py::array_t<float>& array) {
            auto buf = array.request();
            float* ptr = (float*)buf.ptr;
            std::vector<float> params(ptr, ptr + buf.size);
            self.setParameters(params);
        }, py::arg("array"),
             "Set parameters from numpy array")
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("to_dict", [](const Action& self) {
            py::dict result;
            result["type"] = self.getType();
            result["parameters"] = self.getParameters();
            result["name"] = self.getName();
            return result;
        }, "Convert Action to Python dictionary")
        .def(static_cast<Action (*)(py::dict)>([](py::dict d) { Action a; a.setType(d["type"].cast<ActionType>()); a.setParameters(d["parameters"].cast<std::vector<float>>()); return a; })",
             py::arg("dict"),
             "Create Action from Python dictionary");

    // Enhanced WorldObject
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        .def("distance_to", [](const WorldObject& self, float x, float y) {
            return std::hypot(self.x - x, self.y - y);
        }, py::arg("x"), py::arg("y"),
             "Calculate distance to position")
        .def("is_within_range", [](const WorldObject& self, float x, float y, float range) {
            return std::hypot(self.x - x, self.y - y) <= range;
        }, py::arg("x"), py::arg("y"), py::arg("range"),
             "Check if object is within range of position")
        .def("to_dict", [](const WorldObject& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["radius"] = self.radius;
            result["type"] = self.type;
            result["value"] = self.value;
            result["active"] = self.active;
            return result;
        }, "Convert WorldObject to Python dictionary")
        .def(static_cast<WorldObject (*)(py::dict)>([](py::dict d) { WorldObject obj; obj.x = d["x"].cast<float>(); obj.y = d["y"].cast<float>(); obj.radius = d["radius"].cast<float>(); obj.type = d["type"].cast<WorldObjectType>(); obj.value = d["value"].cast<float>(); obj.active = d["active"].cast<bool>(); return obj; })",
             py::arg("dict"),
             "Create WorldObject from Python dictionary");

    // Enhanced AgentBody
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body state with Pythonic interface)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset)
        .def("distance_to", [](const AgentBody& self, float x, float y) {
            return std::hypot(self.x - x, self.y - y);
        }, py::arg("x"), py::arg("y"),
             "Calculate distance to position")
        .def("is_within_range", [](const AgentBody& self, float x, float y, float range) {
            return std::hypot(self.x - x, self.y - y) <= range;
        }, py::arg("x"), py::arg("y"), py::arg("range"),
             "Check if agent is within range of position")
        .def("move_to", [](AgentBody& self, float x, float y, float duration) {
            float dx = x - self.x;
            float dy = y - self.y;
            float distance = std::hypot(dx, dy);
            if (distance > 0) {
                float speed = distance / duration;
                self.velocityX = dx / distance * speed;
                self.velocityY = dy / distance * speed;
            }
        }, py::arg("x"), py::arg("y"), py::arg("duration"),
             "Move to position over time");

    // Enhanced ActionResult
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Enhanced action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("is_good", [](const ActionResult& self) {
            return self.success && self.reward > 0;
        }, "Check if result is good")
        .def("is_bad", [](const ActionResult& self) {
            return !self.success || self.reward < 0;
        }, "Check if result is bad")
        .def("to_dict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert ActionResult to Python dictionary");

    // Enhanced SensoryPercept
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data with enhanced Python support)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp")
        .def("has_vision", [](const SensoryPercept& self) {
            return !self.getVision().getData().empty();
        }, "Check if percept has vision data")
        .def("has_audio", [](const SensoryPercept& self) {
            return !self.getAudio().getData().empty();
        }, "Check if percept has audio data")
        .def("has_internal", [](const SensoryPercept& self) {
            return !self.getInternal().getData().empty();
        }, "Check if percept has internal signals")
        .def("to_dict", [](const SensoryPercept& self) {
            py::dict result;
            result["vision"] = self.getVision();
            result["touch"] = self.getTouch();
            result["internal"] = self.getInternal();
            result["proprioception"] = self.getProprioception();
            result["audio"] = self.getAudio();
            result["timestamp"] = self.getTimestamp();
            return result;
        }, "Convert SensoryPercept to Python dictionary");

    // Enhanced SimpleWorld
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation with enhanced Python support)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep seconds")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command at current time")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("countObjects", [](const SimpleWorld& self) {
            return self.getMaxEnergy(); // This is a placeholder - needs actual object counting
        }, "Count objects in world (placeholder)")
        .def("find_nearest_object", [](const SimpleWorld& self, float x, float y) {
            return WorldObject(x, y, WorldObjectType::Empty); // This is a placeholder
        }, py::arg("x"), py::arg("y"),
             "Find nearest object to position (placeholder)");

    // Enhanced Brain with Pythonic interface
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced brain class with Pythonic interface and factory methods)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def(py::init([]() {
            auto config = std::make_shared<Config>();
            return std::make_shared<Brain>(config);
        }), "Create brain with default configuration")
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
        .def("region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get region by ID (Pythonic alias for getRegion)")
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
        .def("enable_learning", [](Brain& self, float motivation, float exploration) {
            if (auto* agent = dynamic_cast<AgentBrain*>(self.getSTDP())) {
                agent->enableRewardModulation(true);
                agent->enableCuriosity(exploration > 0.0f);
            }
        }, py::arg("motivation") = 1.0f, py::arg("exploration") = 0.5f,
             "Enable learning with motivation and exploration levels")
        .def("enable_structural_plasticity", [](Brain& self, bool enable) {
            if (auto* agent = dynamic_cast<AgentBrain*>(self.getSTDP())) {
                agent->enableStructuralPlasticity(enable);
            }
        }, py::arg("enable") = true,
             "Enable or disable structural plasticity")
        .def("enable_development", [](Brain& self, bool enable) {
            if (auto* agent = dynamic_cast<AgentBrain*>(self.getSTDP())) {
                agent->enableDevelopment(enable);
            }
        }, py::arg("enable") = true,
             "Enable or disable development system")
        .def("enable_curiosity", [](Brain& self, bool enable) {
            if (auto* agent = dynamic_cast<AgentBrain*>(self.getSTDP())) {
                agent->enableCuriosity(enable);
            }
        }, py::arg("enable") = true,
             "Enable or disable curiosity system")
        .def("get_neuromodulation_level", [](Brain& self) {
            if (auto* agent = dynamic_cast<AgentBrain*>(self.getSTDP())) {
                return agent->getNeuromodulationLevel();
            }
            return 0.0f;
        }, "Get current neuromodulation level")
        .def("get_curiosity_level", [](Brain& self) {
            if (auto* agent = dynamic_cast<AgentBrain*>(self.getSTDP())) {
                return agent->getCuriosityLevel();
            }
            return 0.0f;
        }, "Get curiosity level")
        .def("get_prediction_error", [](Brain& self) {
            if (auto* agent = dynamic_cast<AgentBrain*>(self.getSTDP())) {
                return agent->getPredictionError();
            }
            return 0.0f;
        }, "Get prediction error")
        .def("get_novelty_level", [](Brain& self) {
            if (auto* agent = dynamic_cast<AgentBrain*>(self.getSTDP())) {
                return agent->getNoveltyLevel();
            }
            return 0.0f;
        }, "Get novelty level");

    // Enhanced AgentBrain with Pythonic interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface with Pythonic interface)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with brain")
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
             "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // Convenience functions and factory methods
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createLearningConfig", [](const std::string& profile_name) {
        auto config = std::make_shared<Config>();
        auto settings = createLearningConfig(profile_name);
        for (const auto& [key, value] : settings) {
            config->set(key, value);
        }
        return config;
    }, py::arg("profile_name"),
        "Create configuration from learning profile (exploration, development, stable, creative)");

    // Performance optimization profiles
    m.def("createPerformanceConfig", [](const std::string& profile_name) {
        auto config = std::make_shared<Config>();
        auto settings = createPerformanceConfig(profile_name);
        for (const auto& [key, value] : settings) {
            config->set(key, value);
        }
        return config;
    }, py::arg("profile_name"),
        "Create configuration from performance profile (balanced, speed, memory, accuracy)");

    // Research-specific configurations
    m.def("createResearchConfig", [](const std::string& experiment_type) {
        auto config = std::make_shared<Config>();
        auto settings = createResearchConfig(experiment_type);
        for (const auto& [key, value] : settings) {
            config->set(key, value);
        }
        return config;
    }, py::arg("experiment_type"),
        "Create configuration for research experiments (attention, memory, prediction, development)");

    // Production-ready configurations
    m.def("createProductionConfig", [](const std::string& tier) {
        auto config = std::make_shared<Config>();
        auto settings = createProductionConfig(tier);
        for (const auto& [key, value] : settings) {
            config->set(key, value);
        }
        return config;
    }, py::arg("tier"),
        "Create production-ready configuration (prototype, staging, production)");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Context manager for simulation management
    m.def("simulation", [](std::shared_ptr<Brain> brain) -> BrainSimulationManager {
        return BrainSimulationManager(brain);
    }, py::arg("brain"),
         "Create simulation manager for brain (context manager)");

    m.def("simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> BrainSimulationManager {
        return BrainSimulationManager(brain, world);
    }, py::arg("brain"), py::arg("world"),
         "Create simulation manager for brain and world (context manager)");

    // Learning profile enum for factory methods
    py::class_<LearningProfile>(m, "LearningProfile")
        .def(py::init<const std::string&, const std::string&, const std::map<std::string, std::variant<int, double, bool>>&>(),
             py::arg("name"), py::arg("description"), py::arg("settings"))
        .def_readonly("name", &LearningProfile::name)
        .def_readonly("description", &LearningProfile::description)
        .def_readonly("settings", &LearningProfile::settings);

    // Add predefined learning profiles
    m.attr("LEARNING_PROFILE_EXPLORATION") = LearningProfile("exploration",
        "High exploration, curiosity-driven learning with structural plasticity",
        createLearningConfig("exploration"));

    m.attr("LEARNING_PROFILE_DEVELOPMENT") = LearningProfile("development",
        "Focused development with critical periods and maturation",
        createLearningConfig("development"));

    m.attr("LEARNING_PROFILE_STABLE") = LearningProfile("stable",
        "Stable learning with minimal plasticity, focused on efficiency",
        createLearningConfig("stable"));

    m.attr("LEARNING_PROFILE_CREATIVE") = LearningProfile("creative",
        "High creativity with balanced exploration and exploitation",
        createLearningConfig("creative"));

    // Utility functions
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                              std::shared_ptr<AgentBrain> agent, size_t num_steps) {
        if (!brain || !world || !agent) {
            throw SimulationError("Brain, world, or agent is null");
        }
        
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        for (size_t step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps") = 1000,
        "Run complete agent simulation with brain, world, and agent");

    m.def("createAgentEnvironment", [](const std::string& environment_type, int width, int height) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->reset();
        
        auto config = createLearningConfig("exploration");
        auto brain = std::make_shared<Brain>(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        
        return std::make_tuple(brain, agent, world);
    }, py::arg("environment_type"), py::arg("width"), py::arg("height") = 20,
        "Create complete agent environment with brain, agent, and world");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version information
    m.attr("__version__") = "2.0.0-Enhanced";
    m.attr("__version_info__") = py::dict({
        {"major", 2},
        {"minor", 0},
        {"patch", 0},
        {"build", "Enhanced"},
        {"description", "Python bindings with enhanced features"}
    });

    // Documentation for individual classes
    m.def("get_documentation", []() {
        return R"doc(
        Enhanced NLM Python API Documentation:
        
        Classes:
        - Config: Configuration management with Pythonic interface
        - Brain: Neural simulation brain with factory methods
        - AgentBrain: Agent interface for brain-world interaction
        - SimpleWorld: 2D simulation environment
        - SensoryInput: Base sensory input class
        - Vision: Vision sensory input with numpy support
        - Audio: Audio sensory input with numpy support
        - InternalSignals: Internal signals processing
        - Action: Motor action representation
        - WorldObject: World object representation
        - AgentBody: Agent physical state
        - SensoryPercept: Processed sensory data
        - ActionResult: Action outcome results
        
        Learning Profiles:
        - LEARNING_PROFILE_EXPLORATION: High exploration with structural plasticity
        - LEARNING_PROFILE_DEVELOPMENT: Focused development with critical periods
        - LEARNING_PROFILE_STABLE: Efficient, stable learning
        - LEARNING_PROFILE_CREATIVE: Balanced creative exploration
        
        Performance Profiles:
        - createPerformanceConfig("balanced"): Balanced performance profile
        - createPerformanceConfig("speed"): Optimized for speed
        - createPerformanceConfig("memory"): Optimized for memory efficiency
        - createPerformanceConfig("accuracy"): Optimized for accuracy
        - createPerformanceConfig("adaptive"): Self-adjusting performance profile
        
        Research Configurations:
        - createResearchConfig("attention"): For attention mechanism research
        - createResearchConfig("memory"): For memory system research
        - createResearchConfig("prediction"): For prediction system research
        - createResearchConfig("development"): For developmental process research
        
        Production Configurations:
        - createProductionConfig("prototype"): For prototyping and experimentation
        - createProductionConfig("staging"): For staging/production testing
        - createProductionConfig("production"): For production deployment
        
        Utility Functions:
        - createLearningConfig(profile_name): Create config from profile
        - createPerformanceConfig(profile_name): Create config from performance profile
        - createResearchConfig(experiment_type): Create config for research experiments
        - createProductionConfig(tier): Create production-ready configuration
        - createAgentEnvironment(type, width, height): Create complete setup
        - run_simulation(brain, world, agent, num_steps): Run simulation
        
        Error Types:
        - NLMException: Base exception class
        - ConfigurationError: Configuration-related errors
        - SimulationError: Runtime simulation errors
        - BrainInitializationError: Brain initialization errors
        )doc";
    }, "Get comprehensive API documentation");

    // Validation function
    m.def("validate_api", []() {
        bool all_valid = true;
        std::vector<std::string> errors;
        
        // Basic validation
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 100);
        config->set("brain.synapse_density", 0.1);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            errors.push_back("Brain initialization failed");
            all_valid = false;
        }
        
        // Validation completed
        if (all_valid) {
            return py::dict({
                {"status", "valid"},
                {"errors", py::list()},
                {"message", "NLM Python API is properly configured and functional"}
            });
        } else {
            return py::dict({
                {"status", "invalid"},
                {"errors", errors},
                {"message", "NLM Python API validation failed"}
            });
        }
    }, "Validate NLM Python API installation and configuration");

    // Performance benchmarking utility
    m.def("benchmark", [](std::shared_ptr<Brain> brain, size_t num_steps) {
        if (!brain) {
            throw SimulationError("Brain is null");
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        brain->reset();
        brain->initialize();
        
        for (size_t step = 0; step < num_steps; ++step) {
            brain->step(step);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        return py::dict({
            {"steps", num_steps},
            {"total_time_ms", duration.count()},
            {"time_per_step_ms", duration.count() / (float)num_steps},
            {"final_neuron_count", brain->getTotalNeuronCount()},
            {"final_synapse_count", brain->getTotalSynapseCount()},
            {"total_spikes", brain->getTotalSpikeCount()}
        });
    }, py::arg("brain"), py::arg("num_steps") = 1000,
        "Benchmark brain performance and return metrics");
}

} // namespace nlm"}