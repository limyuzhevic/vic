/*
 * Enhanced NLM Python Bindings
 * 
 * This file provides advanced Python APIs for the NLM (Neural Learning Machine) framework.
 * Includes batch operations, monitoring, error handling, and convenience methods.
 * 
 * Key Features:
 * - Batch operations for multiple brains
 * - Network statistics and monitoring
 * - Progress tracking and callbacks
 * - Comprehensive error handling with custom exceptions
 * - Pythonic API design with type hints
 * - Advanced configuration management
 * - Convenience methods for common operations
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <functional>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <stdexcept>

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

// Custom exception types for better error handling
class NLMBindingsError : public std::runtime_error {
public:
    explicit NLMBindingsError(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidArgumentError : public NLMBindingsError {
public:
    explicit InvalidArgumentError(const std::string& msg) : NLMBindingsError("InvalidArgument: " + msg) {}
};

class InitializationError : public NLMBindingsError {
public:
    explicit InitializationError(const std::string& msg) : NLMBindingsError("InitializationError: " + msg) {}
};

class SimulationError : public NLMBindingsError {
public:
    explicit SimulationError(const std::string& msg) : NLMBindingsError("SimulationError: " + msg) {}
};

// Progress tracker for long-running operations
class ProgressTracker {
public:
    using Callback = std::function<void(double, const std::string&)>;
    
    ProgressTracker(double total, const std::string& description, 
                   const Callback& callback = nullptr, double updateInterval = 0.1)
        : total_(total), current_(0), description_(description), 
          callback_(callback), updateInterval_(updateInterval), 
          lastUpdateTime_(std::chrono::steady_clock::now()) {}
    
    void update(double increment = 1.0, const std::string& message = "") {
        current_ += increment;
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - lastUpdateTime_);
        
        if (elapsed.count() >= updateInterval_ || current_ >= total_) {
            double progress = (total_ > 0) ? (current_ / total_) * 100.0 : 0.0;
            std::string msg = message.empty() ? description_ : message;
            if (callback_) {
                callback_(progress, msg);
            }
            lastUpdateTime_ = now;
        }
    }
    
    double getProgress() const {
        return (total_ > 0) ? (current_ / total_) * 100.0 : 0.0;
    }
    
    bool isComplete() const {
        return current_ >= total_;
    }
    
    std::string getDescription() const {
        return description_;
    }
    
private:
    double total_;
    double current_;
    std::string description_;
    Callback callback_;
    double updateInterval_;
    std::chrono::steady_clock::time_point lastUpdateTime_;
};

// Validation helper functions
inline void validateBrain(const std::shared_ptr<Brain>& brain, const std::string& operation) {
    if (!brain) {
        throw InvalidArgumentError("Brain is null for operation: " + operation);
    }
    if (!brain->getConfig()) {
        throw InitializationError("Brain config is not initialized for operation: " + operation);
    }
}

inline void validateWorld(const std::shared_ptr<SimpleWorld>& world, const std::string& operation) {
    if (!world) {
        throw InvalidArgumentError("World is null for operation: " + operation);
    }
}

inline void validateConfig(const std::shared_ptr<Config>& config, const std::string& operation) {
    if (!config) {
        throw InvalidArgumentError("Config is null for operation: " + operation);
    }
}

// Helper functions for batch operations
inline std::vector<std::shared_ptr<Brain>> createBrainsFromConfigs(const std::vector<std::shared_ptr<Config>>& configs) {
    std::vector<std::shared_ptr<Brain>> brains;
    brains.reserve(configs.size());
    
    for (const auto& config : configs) {
        if (!config) {
            throw InvalidArgumentError("Config is null when creating brains");
        }
        brains.push_back(std::make_shared<Brain>(config));
    }
    
    return brains;
}

inline std::vector<std::unique_ptr<Action>> brainProduceActions(const std::vector<std::shared_ptr<Brain>>& brains) {
    std::vector<std::unique_ptr<Action>> actions;
    actions.reserve(brains.size());
    
    for (const auto& brain : brains) {
        if (brain) {
            actions.push_back(brain->produceAction());
        } else {
            actions.push_back(nullptr);
        }
    }
    
    return actions;
}

// C++ helper classes for advanced functionality
class NetworkStatistics {
public:
    struct Stats {
        size_t total_neurons = 0;
        size_t total_synapses = 0;
        size_t active_neurons = 0;
        size_t firing_neurons = 0;
        float average_firing_rate = 0.0f;
        float excitation_inhibition_ratio = 0.0f;
        float total_spike_count = 0;
        std::map<NeuronType, size_t> neuron_type_counts;
        std::map<SynapseType, size_t> synapse_type_counts;
        std::vector<float> neuron_density_by_region;
        std::vector<float> connection_density_by_region;
    };
    
    static Stats compute(const std::shared_ptr<Brain>& brain) {
        Stats stats;
        if (!brain) return stats;
        
        stats.total_neurons = brain->getTotalNeuronCount();
        stats.total_synapses = brain->getTotalSynapseCount();
        stats.active_neurons = brain->getActiveNeuronCount();
        stats.firing_neurons = brain->getFiringNeuronCount();
        stats.average_firing_rate = brain->getAverageFiringRate();
        stats.excitation_inhibition_ratio = brain->getExcitationInhibitionRatio();
        stats.total_spike_count = brain->getTotalSpikeCount();
        
        // Get regions for detailed statistics
        const auto& regions = brain->getRegions();
        stats.neuron_density_by_region.reserve(regions.size());
        stats.connection_density_by_region.reserve(regions.size());
        
        for (const auto& region : regions) {
            // For now, basic statistics - could be enhanced with population data
            stats.neuron_density_by_region.push_back(static_cast<float>(region->getAllNeurons().size()) / 1000.0f); // normalized
            stats.connection_density_by_region.push_back(static_cast<float>(region->getSynapses().size()) / 1000.0f); // normalized
        }
        
        return stats;
    }
};

class LearningProgressTracker {
public:
    struct ProgressPoint {
        SimulationStep step;
        Timestamp time;
        float spike_count;
        float firing_rate;
        float learning_rate;
        float stability_score;
    };
    
    LearningProgressTracker() : baseline_spike_count_(0.0f), baseline_firing_rate_(0.0f) {}
    
    void setBaseline(float spike_count, float firing_rate) {
        baseline_spike_count_ = spike_count;
        baseline_firing_rate_ = firing_rate;
    }
    
    std::vector<ProgressPoint> trackProgress(const std::shared_ptr<Brain>& brain, 
                                           const std::vector<SimulationStep>& steps) {
        std::vector<ProgressPoint> progress;
        progress.reserve(steps.size());
        
        if (!brain) return progress;
        
        for (SimulationStep step : steps) {
            ProgressPoint point;
            point.step = step;
            point.time = static_cast<Timestamp>(step * 0.1); // assuming 0.1 time units per step
            point.spike_count = brain->getTotalSpikeCount();
            point.firing_rate = brain->getAverageFiringRate();
            point.learning_rate = baseline_spike_count_ > 0 ? 
                (brain->getTotalSpikeCount() - baseline_spike_count_) / baseline_spike_count_ : 0.0f;
            point.stability_score = 1.0f - (brain->getAverageFiringRate() / 100.0f); // simple stability metric
            
            progress.push_back(point);
        }
        
        return progress;
    }
    
    std::vector<ProgressPoint> getActivePaths(const std::shared_ptr<Brain>& brain, 
                                            const std::vector<NeuronId>& neuron_ids,
                                            const std::vector<SimulationStep>& steps) {
        std::vector<ProgressPoint> active_paths;
        active_paths.reserve(steps.size() * neuron_ids.size());
        
        if (!brain) return active_paths;
        
        for (SimulationStep step : steps) {
            brain->step(step);
            for (const NeuronId& neuron_id : neuron_ids) {
                ProgressPoint point;
                point.step = step;
                point.time = static_cast<Timestamp>(step * 0.1);
                
                // Check if this neuron is active (simplified check)
                bool neuron_active = false;
                const auto& regions = brain->getRegions();
                for (const auto& region : regions) {
                    for (auto* neuron : region->getAllNeurons()) {
                        if (neuron && neuron->getId() == neuron_id) {
                            neuron_active = true;
                            break;
                        }
                    }
                    if (neuron_active) break;
                }
                
                point.spike_count = neuron_active ? 1.0f : 0.0f;
                point.firing_rate = neuron_active ? 10.0f : 0.0f; // estimated firing rate
                point.learning_rate = 0.0f;
                point.stability_score = neuron_active ? 0.8f : 0.0f; // higher stability when active
                
                active_paths.push_back(point);
            }
        }
        
        return active_paths;
    }
    
private:
    float baseline_spike_count_;
    float baseline_firing_rate_;
};

class ConnectivityMatrix {
public:
    struct MatrixEntry {
        size_t row = 0;
        size_t col = 0;
        float weight = 0.0f;
        SynapseType type = SynapseType::Excitatory;
        float delay = 0.0f;
        bool is_direct = false;
    };
    
    static std::vector<MatrixEntry> getFullMatrix(const std::shared_ptr<Brain>& brain) {
        std::vector<MatrixEntry> matrix;
        if (!brain) return matrix;
        
        const auto& regions = brain->getRegions();
        size_t total_neurons = brain->getTotalNeuronCount();
        
        // For now, return empty matrix as the C++ implementation
        // would require access to internal neuron/synapse structures
        // This is a placeholder that can be expanded with actual connectivity data
        matrix.reserve(total_neurons * total_neurons);
        
        return matrix;
    }
};

// Enhanced binding functions
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------
        Advanced Python API for the NLM C++ neural simulation framework.
        
        New Features:
        - Batch operations for multiple brains
        - Network statistics and monitoring
        - Progress tracking with callbacks
        - Custom error handling
        - Convenience methods for common operations
        - Advanced configuration management
        - Neural pathway analysis tools
        
        Example usage:
        ```python
        import nlm
        
        # Create multiple brains
        brains = nlm.brain.initializeMultiple([
            nlm.createDefaultConfig(),
            nlm.createDefaultConfig()
        ])
        
        # Get network statistics
        stats = nlm.brain.getNetworkStats()
        print(f"Total neurons: {stats.total_neurons}")
        
        # Track learning progress
        progress = nlm.brain.getLearningProgress()
        
        # Save all brains
        nlm.brain.saveAll(brains, "/path/to/checkpoints/")
        ```
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMBindingsError>(m, "NLMBindingsError");
    py::register_exception<InvalidArgumentError>(m, "InvalidArgumentError");
    py::register_exception<InitializationError>(m, "InitializationError");
    py::register_exception<SimulationError>(m, "SimulationError");

    // Type definitions for convenience
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

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Enhanced Config class with nested key support
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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
        // New nested configuration methods
        .def("setNested", [](Config& self, const std::string& key_path, const py::object& value) {
            // Convert Python object to ConfigValue
            ConfigValue config_value;
            if (py::isinstance<py::int_>(value)) {
                config_value = value.cast<int64_t>();
            } else if (py::isinstance<py::float_>(value)) {
                config_value = value.cast<double>();
            } else if (py::isinstance<py::bool_>(value)) {
                config_value = value.cast<bool>();
            } else if (py::isinstance<py::str>(value)) {
                config_value = value.cast<std::string>();
            } else if (py::isinstance<py::list>(value)) {
                auto list = value.cast<std::vector<double>>();
                config_value = list;
            } else {
                throw InvalidArgumentError("Unsupported value type for nested key setting");
            }
            self.set(key_path, config_value, ConfigSource::Runtime);
        }, py::arg("key_path"), py::arg("value"),
             "Set nested configuration key using dot notation (e.g., 'system.brain.neuron_count')")
        .def("getNested", [](const Config& self, const std::string& key_path, const py::object& default_value) {
            auto value = self.get<std::string>(key_path);
            if (value.has_value()) {
                return py::cast(*value);
            }
            // Return default if provided, otherwise raise KeyError
            if (!default_value.is_none()) {
                return default_value;
            }
            throw InvalidArgumentError("Configuration key not found: " + key_path);
        }, py::arg("key_path"), py::arg("default_value") = py::none(),
             "Get nested configuration value using dot notation")
        .def("updateFromDict", [](Config& self, const py::dict& config_dict) {
            for (auto item : config_dict) {
                std::string key = py::str(item.first);
                py::object value = item.second;
                
                ConfigValue config_value;
                if (py::isinstance<py::int_>(value)) {
                    config_value = value.cast<int64_t>();
                } else if (py::isinstance<py::float_>(value)) {
                    config_value = value.cast<double>();
                } else if (py::isinstance<py::bool_>(value)) {
                    config_value = value.cast<bool>();
                } else if (py::isinstance<py::str>(value)) {
                    config_value = value.cast<std::string>();
                } else if (py::isinstance<py::list>(value)) {
                    config_value = value.cast<std::vector<double>>();
                } else {
                    throw InvalidArgumentError("Unsupported value type in config dict");
                }
                
                self.set(key, config_value, ConfigSource::Runtime);
            }
        }, py::arg("config_dict"),
             "Update configuration from Python dictionary")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced SensoryInput and derivatives
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Enhanced action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // Enhanced WorldObject class
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
        .def_readwrite("active", &WorldObject::active);

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body state)pbdoc")
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
        .def("reset", &AgentBody::reset);

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Enhanced action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enhanced sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // World convenience methods
        .def("createWorldWithObjects", [](std::shared_ptr<SimpleWorld>& self, 
                                         size_t width, size_t height, 
                                         const std::vector<WorldObject>& objects) {
            self->configure(width, height, 84, 84); // default vision size
            for (const auto& obj : objects) {
                self->addObject(obj);
            }
        }, py::arg("width"), py::arg("height"), py::arg("objects"),
             "Create world with predefined objects (convenience method)")
        .def("addObjectAt", [](std::shared_ptr<SimpleWorld>& self, float x, float y, 
                              WorldObjectType type, float value = 0.0f, float radius = 0.5f) {
            WorldObject obj(x, y, type, value, radius);
            self->addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Add object at specified position (convenience method)");

    // Enhanced Brain class with batch operations and monitoring
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Enhanced central neural simulation brain class)pbdoc")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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
        // New batch operations
        .def("initializeMultiple", [](std::shared_ptr<Brain>& self, 
                                     const std::vector<std::shared_ptr<Config>>& configs) {
            return createBrainsFromConfigs(configs);
        }, py::arg("configs"),
             "Initialize multiple brains from configs (convenience method)")
        .def("stepMultiple", [](const std::vector<std::shared_ptr<Brain>>& brains, 
                               const std::vector<SimulationStep>& steps) {
            std::vector<SimulationStep> all_steps;
            for (size_t i = 0; i < steps.size(); ++i) {
                all_steps.insert(all_steps.end(), steps[i], steps[i] + 1);
            }
            
            for (size_t step_idx = 0; step_idx < all_steps.size(); ++step_idx) {
                for (const auto& brain : brains) {
                    if (brain) {
                        brain->step(all_steps[step_idx]);
                    }
                }
            }
        }, py::arg("brains"), py::arg("steps"),
             "Run multiple brains in parallel (convenience method)")
        .def("resetMultiple", [](const std::vector<std::shared_ptr<Brain>>& brains) {
            for (const auto& brain : brains) {
                if (brain) brain->reset();
            }
        }, py::arg("brains"),
             "Reset multiple brains (convenience method)")
        .def("saveAll", [](const std::vector<std::shared_ptr<Brain>>& brains, 
                          const std::string& directory) {
            std::vector<std::string> filepaths;
            for (size_t i = 0; i < brains.size(); ++i) {
                std::string filepath = directory + "/brain_" + std::to_string(i) + ".bin";
                if (brains[i] && brains[i]->save(filepath)) {
                    filepaths.push_back(filepath);
                }
            }
            return filepaths;
        }, py::arg("brains"), py::arg("directory"),
             "Save multiple brains to checkpoint files (convenience method)")
        // Monitoring and statistics
        .def("getNetworkStats", [](const std::shared_ptr<Brain>& brain) {
            return NetworkStatistics::compute(brain);
        }, py::arg("brain"),
             "Get comprehensive network statistics (convenience method)")
        .def("getLearningProgress", [](std::shared_ptr<Brain>& brain, 
                                      float baseline_spike_count,
                                      float baseline_firing_rate) {
            LearningProgressTracker tracker;
            tracker.setBaseline(baseline_spike_count, baseline_firing_rate);
            
            std::vector<SimulationStep> steps = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
            return tracker.trackProgress(brain, steps);
        }, py::arg("brain"), py::arg("baseline_spike_count") = 0.0f, py::arg("baseline_firing_rate") = 0.0f,
             "Track learning progress over time (convenience method)")
        .def("findActivePaths", [](std::shared_ptr<Brain>& brain,
                                  const std::vector<NeuronId>& neuron_ids,
                                  const std::vector<SimulationStep>& steps) {
            LearningProgressTracker tracker;
            return tracker.getActivePaths(brain, neuron_ids, steps);
        }, py::arg("brain"), py::arg("neuron_ids"), py::arg("steps") = std::vector<SimulationStep>(),
             "Find active neural pathways (convenience method)")
        .def("getConnectivityMatrix", [](const std::shared_ptr<Brain>& brain) {
            return ConnectivityMatrix::getFullMatrix(brain);
        }, py::arg("brain"),
             "Get full connectivity matrix (convenience method)")
        // Convenience methods for common operations
        .def("trainOnPattern", [](std::shared_ptr<Brain>& brain, 
                                 const std::vector<float>& pattern, 
                                 SimulationStep duration) {
            if (!brain) throw InvalidArgumentError("Brain is null for training");
            
            // Convert pattern to sensory input
            auto vision = std::make_unique<Vision>(pattern.size(), 1, 1);
            vision->setData(pattern);
            
            // Train for specified duration
            for (SimulationStep step = 0; step < duration; ++step) {
                brain->receiveSensoryInput(*vision);
                brain->step(step);
            }
        }, py::arg("brain"), py::arg("pattern"), py::arg("duration") = 100,
             "Train brain on specific input pattern (convenience method)")
        .def("evaluateTestSet", [](std::shared_ptr<Brain>& brain,
                                  const std::vector<std::vector<float>>& test_patterns,
                                  const std::vector<SimulationStep>& steps) {
            if (!brain) throw InvalidArgumentError("Brain is null for evaluation");
            
            std::vector<float> accuracies;
            for (const auto& pattern : test_patterns) {
                // Store original state
                size_t original_spikes = brain->getTotalSpikeCount();
                float original_rate = brain->getAverageFiringRate();
                
                // Process pattern
                auto vision = std::make_unique<Vision>(pattern.size(), 1, 1);
                vision->setData(pattern);
                brain->receiveSensoryInput(*vision);
                
                for (SimulationStep step : steps) {
                    brain->step(step);
                }
                
                // Calculate accuracy based on spike activity
                size_t new_spikes = brain->getTotalSpikeCount() - original_spikes;
                float new_rate = brain->getAverageFiringRate();
                float spike_accuracy = static_cast<float>(new_spikes) / static_cast<float>(steps.size() * 10); // normalized
                float rate_accuracy = std::min(new_rate / 50.0f, 1.0f); // normalized to max firing rate
                
                accuracies.push_back((spike_accuracy + rate_accuracy) / 2.0f);
                
                // Reset for next pattern
                brain->reset();
                brain->initialize();
            }
            
            float average_accuracy = 0.0f;
            if (!accuracies.empty()) {
                float sum = std::accumulate(accuracies.begin(), accuracies.end(), 0.0f);
                average_accuracy = sum / accuracies.size();
            }
            
            return std::make_pair(average_accuracy, accuracies);
        }, py::arg("brain"), py::arg("test_patterns"), py::arg("steps") = std::vector<SimulationStep>(),
             "Evaluate brain performance on test patterns (convenience method)")
        .def("generateBehavior", [](std::shared_ptr<Brain>& brain, 
                                   std::shared_ptr<SimpleWorld>& world,
                                   SimulationStep steps) {
            if (!brain || !world) {
                throw InvalidArgumentError("Brain or world is null for behavior generation");
            }
            
            std::vector<std::unique_ptr<Action>> actions;
            
            for (SimulationStep step = 0; step < steps; ++step) {
                // Get action from brain
                auto action = brain->produceAction();
                if (action) {
                    // Apply action in world
                    world->applyMotorCommand(
                        action->getType() == ActionType::MoveForward ? MotorCommand::MoveForward :
                        action->getType() == ActionType::MoveBackward ? MotorCommand::MoveBackward :
                        action->getType() == ActionType::TurnLeft ? MotorCommand::TurnLeft :
                        action->getType() == ActionType::TurnRight ? MotorCommand::TurnRight :
                        MotorCommand::Wait,
                        static_cast<float>(step)
                    );
                    
                    actions.push_back(std::move(action));
                    
                    // Step world and get sensory input
                    world->update(step);
                    auto percept = world->getSensoryPercept();
                    brain->receiveSensoryInput(*percept);
                    
                    // Brain step
                    brain->step(step);
                }
            }
            
            return actions;
        }, py::arg("brain"), py::arg("world"), py::arg("steps") = 100,
             "Generate behavior in a world environment (convenience method)")
        .def("getActionDistribution", [](const std::shared_ptr<Brain>& brain,
                                        ActionType action_type,
                                        SimulationStep steps) {
            if (!brain) throw InvalidArgumentError("Brain is null for action distribution analysis");
            
            std::map<ActionType, size_t> action_counts;
            for (const auto& action_type_enum : {
                ActionType::MoveForward, ActionType::MoveBackward, ActionType::TurnLeft,
                ActionType::TurnRight, ActionType::Look, ActionType::LookUp,
                ActionType::LookDown, ActionType::Interact, ActionType::Eat,
                ActionType::Drink, ActionType::Rest, ActionType::Wait, ActionType::Custom
            }) {
                action_counts[action_type_enum] = 0;
            }
            
            // Generate actions and count them
            for (SimulationStep step = 0; step < steps; ++step) {
                brain->step(step);
                auto action = brain->produceAction();
                if (action) {
                    action_counts[action->getType()]++;
                }
            }
            
            // Normalize to get distribution
            std::map<ActionType, double> distribution;
            size_t total_actions = 0;
            for (const auto& pair : action_counts) {
                total_actions += pair.second;
            }
            
            if (total_actions > 0) {
                for (const auto& pair : action_counts) {
                    distribution[pair.first] = static_cast<double>(pair.second) / total_actions;
                }
            }
            
            return distribution;
        }, py::arg("brain"), py::arg("action_type") = ActionType::MoveForward, py::arg("steps") = 100,
             "Get distribution of action outputs (convenience method)");

    // Enhanced AgentBrain class
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)pbdoc")
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
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Convenience methods
        .def("createAgentWithAllSystems", [](std::shared_ptr<AgentBrain>& self,
                                            std::shared_ptr<Brain>& brain,
                                            std::shared_ptr<SimpleWorld>& world) {
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(world);
            return agent;
        }, py::arg("brain"), py::arg("world"),
             "Create agent with all neuromodulation systems (convenience method)")
        .def("enableAllSystems", [](std::shared_ptr<AgentBrain>& self) {
            if (self) {
                self->enableRewardModulation(true);
                self->enableStructuralPlasticity(true);
                self->enableDevelopment(true);
                self->enableCuriosity(true);
            }
        }, "Enable all neuromodulation systems (convenience method)");

    // Global convenience functions
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

    // Enhanced utility functions
    m.def("createQuickBrain", [](size_t neuron_count, double connection_prob) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", connection_prob, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("neuron_count") = 500, py::arg("connection_prob") = 0.1,
       "Create a brain with specified parameters (convenience function)");

    m.def("createWorldWithObjects", [](size_t width, size_t height,
                                      const std::vector<WorldObject>& objects) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 84, 84);
        for (const auto& obj : objects) {
            world->addObject(obj);
        }
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("objects"),
       "Create world with predefined objects (convenience function)");

    m.def("createAgentWithAllSystems", [](std::shared_ptr<Brain> brain,
                                        std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        return agent;
    }, py::arg("brain"), py::arg("world"),
       "Create agent with all neuromodulation systems (convenience function)");

    // Helper functions
    m.def("validateBrain", [](const std::shared_ptr<Brain>& brain, const std::string& operation) {
        if (!brain) {
            throw std::runtime_error("Brain is null for operation: " + operation);
        }
        return true;
    }, py::arg("brain"), py::arg("operation"),
       "Validate brain state for operation (utility function)");

    m.def("validateWorld", [](const std::shared_ptr<SimpleWorld>& world, const std::string& operation) {
        if (!world) {
            throw std::runtime_error("World is null for operation: " + operation);
        }
        return true;
    }, py::arg("world"), py::arg("operation"),
       "Validate world state for operation (utility function)");

    // Utility functions
    m.def("createQuickBrain", [](size_t neuron_count, double connection_prob) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", connection_prob, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("neuron_count") = 500, py::arg("connection_prob") = 0.1,
       "Create a brain with specified parameters (convenience function)");

    m.def("createWorldWithObjects", [](size_t width, size_t height,
                                       const std::vector<WorldObject>& objects) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 84, 84);
        for (const auto& obj : objects) {
            world->addObject(obj);
        }
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("objects"),
       "Create world with predefined objects (convenience function)");

    m.def("createAgentWithAllSystems", [](std::shared_ptr<Brain> brain,
                                         std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        agent->enableAllSystems();
        return agent;
    }, py::arg("brain"), py::arg("world"),
       "Create agent with all neuromodulation systems (convenience function)");

    // Helper functions
    m.def("validateBrain", &validateBrain,
          py::arg("brain"), py::arg("operation"),
          "Validate brain state for operation (utility function)");

    m.def("validateWorld", &validateWorld,
          py::arg("world"), py::arg("operation"),
          "Validate world state for operation (utility function)");

    m.def("createProgressTracker", [](double total, const std::string& description,
                                     const std::function<void(double, const std::string&)>& callback,
                                     double updateInterval) {
        return std::make_shared<ProgressTracker>(total, description, callback, updateInterval);
    }, py::arg("total"), py::arg("description"), py::arg("callback") = nullptr, py::arg("updateInterval") = 0.1,
       "Create a progress tracker (utility function)");

    // Expose constants and utility values
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
