#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iomanip>

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

// Custom exception types for Python

class BrainInitializationError : public std::runtime_error {
public:
    explicit BrainInitializationError(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidOperationError : public std::runtime_error {
public:
    explicit InvalidOperationError(const std::string& msg) : std::runtime_error(msg) {}
};

class ResourceError : public std::runtime_error {
public:
    explicit ResourceError(const std::string& msg) : std::runtime_error(msg) {}
};

class ValidationError : public std::runtime_error {
public:
    explicit ValidationError(const std::string& msg) : std::runtime_error(msg) {}
};

// Register exceptions with Python
inline void register_exceptions(py::module_& m) {
    py::register_exception<BrainInitializationError>(m, "BrainInitializationError");
    py::register_exception<InvalidOperationError>(m, "InvalidOperationError");
    py::register_exception<ResourceError>(m, "ResourceError");
    py::register_exception<ValidationError>(m, "ValidationError");
}

// Template functions for repetitive conversions and common operations
template<typename From, typename To>
py::object convertType(const From& value) {
    try {
        return py::cast(To(value));
    } catch (const std::exception& e) {
        throw ValidationError("Type conversion failed: " + std::string(e.what()));
    }
}

template<typename Container>
py::object convertToPythonList(const Container& container) {
    return py::cast(container);
}

// Validation helpers
inline void validateRange(float value, float min, float max, const std::string& paramName) {
    if (value < min || value > max) {
        std::ostringstream oss;
        oss << "Parameter '" << paramName << "' must be in range [" 
            << min << ", " << max << "], got " << value;
        throw ValidationError(oss.str());
    }
}

inline void validatePositive(float value, const std::string& paramName) {
    if (value <= 0) {
        std::ostringstream oss;
        oss << "Parameter '" << paramName << "' must be positive, got " << value;
        throw ValidationError(oss.str());
    }
}

inline void validateNonNegative(float value, const std::string& paramName) {
    if (value < 0) {
        std::ostringstream oss;
        oss << "Parameter '" << paramName << "' must be non-negative, got " << value;
        throw ValidationError(oss.str());
    }
}

inline void validateNotEmpty(const std::string& value, const std::string& paramName) {
    if (value.empty()) {
        std::ostringstream oss;
        oss << "Parameter '" << paramName << "' cannot be empty";
        throw ValidationError(oss.str());
    }
}

// Progress tracking utilities
class ProgressTracker {
public:
    using Callback = std::function<void(float, const std::string&)>;
    
    ProgressTracker(Callback callback = nullptr, float updateInterval = 0.1f)
        : callback_(callback), updateInterval_(updateInterval), lastUpdate_(0.0f) {}
    
    void update(float progress, const std::string& message = "") {
        if (callback_) {
            callback_(progress, message);
        }
    }
    
    void increment(float amount = 0.01f, const std::string& message = "") {
        lastUpdate_ += amount;
        if (callback_ && (lastUpdate_ >= updateInterval_ || progress_ >= 1.0f)) {
            callback_(std::min(lastUpdate_, 1.0f), message);
        }
    }
    
    void set(float progress, const std::string& message = "") {
        progress_ = progress;
        if (callback_) {
            callback_(progress_, message);
        }
    }
    
    float getProgress() const { return progress_; }
    
    void reset() {
        progress_ = 0.0f;
        lastUpdate_ = 0.0f;
    }
    
private:
    Callback callback_;
    float updateInterval_;
    float progress_ = 0.0f;
    float lastUpdate_ = 0.0f;
};

// Context manager for resource management
class ResourceManager {
public:
    using Deleter = std::function<void()>;
    
    ResourceManager(const std::string& name) : name_(name) {}
    
    template<typename T>
    void registerResource(const std::string& key, std::shared_ptr<T> resource, Deleter deleter = nullptr) {
        if (deleter) {
            resources_[key] = std::make_pair(resource, deleter);
        } else {
            resources_[key] = std::make_pair(resource, [](auto&) {});
        }
    }
    
    template<typename T>
    std::shared_ptr<T> getResource(const std::string& key) {
        auto it = resources_.find(key);
        if (it == resources_.end()) {
            throw ResourceError("Resource not found: " + key);
        }
        return std::static_pointer_cast<T>(it->second.first);
    }
    
    void cleanup() {
        for (auto& pair : resources_) {
            pair.second.second(pair.second.first);
        }
        resources_.clear();
    }
    
    ~ResourceManager() {
        cleanup();
    }
    
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    
private:
    std::string name_;
    std::unordered_map<std::string, std::pair<std::shared_ptr<void>, Deleter>> resources_;
};

// Configuration presets
namespace ConfigPresets {
    inline std::shared_ptr<Config> createDefault() {
        auto config = std::make_shared<Config>();
        
        // Set default values
        config->set("brain.init_neurons", 1000);
        config->set("brain.learning_rate", 0.01f);
        config->set("brain.decay_rate", 0.95f);
        config->set("world.width", 100.0f);
        config->set("world.height", 100.0f);
        config->set("world.max_energy", 1.0f);
        config->set("world.energy_decay", 0.01f);
        config->set("sensory.vision.width", 10);
        config->set("sensory.vision.height", 10);
        config->set("sensory.vision.channels", 1);
        config->set("sensory.audio.sample_rate", 22050);
        config->set("sensory.audio.samples", 1024);
        
        return config;
    }
    
    inline std::shared_ptr<Config> createDevelopment() {
        auto config = createDefault();
        
        // Development-focused settings
        config->set("brain.enable_stdp", true);
        config->set("brain.enable_hebbian", true);
        config->set("brain.enable_structural_plasticity", true);
        config->set("brain.enable_reward_modulation", true);
        config->set("brain.enable_development", true);
        config->set("brain.enable_curiosity", true);
        config->set("brain.learning_rate", 0.05f);  // Higher learning rate
        config->set("world.max_energy", 2.0f);      // Higher energy
        
        return config;
    }
    
    inline std::shared_ptr<Config> createProduction() {
        auto config = createDefault();
        
        // Production-focused settings (stable, efficient)
        config->set("brain.enable_stdp", true);
        config->set("brain.enable_hebbian", false);  // Simpler learning
        config->set("brain.enable_structural_plasticity", false);  // No structural changes
        config->set("brain.enable_reward_modulation", true);
        config->set("brain.enable_development", false);  // Fixed behavior
        config->set("brain.enable_curiosity", false);   // No exploration
        config->set("brain.learning_rate", 0.001f);   // Conservative learning
        config->set("world.max_energy", 1.0f);       // Standard energy
        
        return config;
    }
    
    inline std::shared_ptr<Config> createExploration() {
        auto config = createDefault();
        
        // Exploration-focused settings
        config->set("brain.enable_stdp", true);
        config->set("brain.enable_hebbian", true);
        config->set("brain.enable_structural_plasticity", true);
        config->set("brain.enable_reward_modulation", true);
        config->set("brain.enable_development", true);
        config->set("brain.enable_curiosity", true);
        config->set("brain.learning_rate", 0.1f);    // Fast learning
        config->set("world.max_energy", 5.0f);      // High energy for exploration
        config->set("world.energy_decay", 0.05f);   // Slower decay
        
        return config;
    }
}

// Utility functions
namespace Utils {
    inline std::vector<std::string> getBrainStatistics(const Brain& brain) {
        std::vector<std::string> stats;
        
        stats.push_back("Total neurons: " + std::to_string(brain.getTotalNeuronCount()));
        stats.push_back("Total synapses: " + std::to_string(brain.getTotalSynapseCount()));
        stats.push_back("Active neurons: " + std::to_string(brain.getActiveNeuronCount()));
        stats.push_back("Firing neurons: " + std::to_string(brain.getFiringNeuronCount()));
        stats.push_back("Average firing rate: " + std::to_string(brain.getAverageFiringRate()));
        stats.push_back("Excit/Inhib ratio: " + std::to_string(brain.getExcitationInhibitionRatio()));
        stats.push_back("Total spikes: " + std::to_string(brain.getTotalSpikeCount()));
        
        return stats;
    }
    
    inline std::vector<std::string> getWorldInfo(const SimpleWorld& world) {
        std::vector<std::string> info;
        
        info.push_back("World size: " + std::to_string(world.getWidth()) + "x" + std::to_string(world.getHeight()));
        info.push_back("Max energy: " + std::to_string(world.getMaxEnergy()));
        info.push_back("Energy decay: " + std::to_string(world.getEnergyDecayRate()));
        info.push_back("Simulation time: " + std::to_string(world.getSimulationTime()));
        info.push_back("Random seed: " + std::to_string(world.getRandomSeed()));
        
        return info;
    }
    
    inline std::string formatFloat(float value, int precision = 6) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << value;
        return oss.str();
    }
    
    inline std::vector<float> normalize(const std::vector<float>& data, float min = 0.0f, float max = 1.0f) {
        if (data.empty()) return {};
        
        float actualMin = *std::min_element(data.begin(), data.end());
        float actualMax = *std::max_element(data.begin(), data.end());
        
        std::vector<float> result;
        result.reserve(data.size());
        
        for (float value : data) {
            if (actualMax > actualMin) {
                float normalized = (value - actualMin) / (actualMax - actualMin);
                result.push_back(min + (max - min) * normalized);
            } else {
                result.push_back((min + max) / 2.0f);
            }
        }
        
        return result;
    }
    
    inline std::vector<float> linspace(float start, float stop, size_t num) {
        std::vector<float> result;
        if (num == 0) return result;
        
        float step = (stop - start) / (num - 1);
        result.reserve(num);
        
        for (size_t i = 0; i < num; ++i) {
            result.push_back(start + i * step);
        }
        
        return result;
    }
}

// Context manager classes
class BrainContextManager {
public:
    BrainContextManager(std::shared_ptr<Brain> brain) : brain_(brain) {
        if (brain_) {
            brain_->reset();
        }
    }
    
    ~BrainContextManager() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    // Disable copying
    BrainContextManager(const BrainContextManager&) = delete;
    BrainContextManager& operator=(const BrainContextManager&) = delete;
    
    // Enable moving
    BrainContextManager(BrainContextManager&& other) noexcept : brain_(std::move(other.brain_)) {}
    BrainContextManager& operator=(BrainContextManager&& other) noexcept {
        if (this != &other) {
            brain_ = std::move(other.brain_);
        }
        return *this;
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
    BrainContextManager& step(SimulationStep step, Timestamp time = 0.0) {
        if (brain_) {
            brain_->step(step, time);
        }
        return *this;
    }
    
    BrainContextManager& inject(float neuronId, MembranePotential current) {
        if (brain_) {
            brain_->injectCurrent(NeuronId(static_cast<uint64_t>(neuronId)), current);
        }
        return *this;
    }
    
    ActionResult produceAction() {
        if (!brain_) {
            throw InvalidOperationError("Brain is not initialized");
        }
        return *brain_->produceAction();
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

class WorldContextManager {
public:
    WorldContextManager(std::shared_ptr<SimpleWorld> world) : world_(world) {
        if (world_) {
            world_->reset();
        }
    }
    
    ~WorldContextManager() {
        if (world_) {
            world_->reset();
        }
    }
    
    // Disable copying
    WorldContextManager(const WorldContextManager&) = delete;
    WorldContextManager& operator=(const WorldContextManager&) = delete;
    
    // Enable moving
    WorldContextManager(WorldContextManager&& other) noexcept : world_(std::move(other.world_)) {}
    WorldContextManager& operator=(WorldContextManager&& other) noexcept {
        if (this != &other) {
            world_ = std::move(other.world_);
        }
        return *this;
    }
    
    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }
    
    WorldContextManager& update(double timestep) {
        if (world_) {
            world_->update(timestep);
        }
        return *this;
    }
    
    ActionResult apply(MotorCommand cmd) {
        if (!world_) {
            throw InvalidOperationError("World is not initialized");
        }
        return world_->applyMotorCommand(cmd, world_->getSimulationTime());
    }
    
private:
    std::shared_ptr<SimpleWorld> world_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        Key Features:
        - Pythonic, intuitive APIs with context managers
        - Enhanced error handling with descriptive exceptions
        - Progress tracking and callbacks for long-running operations
        - Configuration presets for common use cases
        - Advanced utility functions and memory management
        - Batch operations and type conversion utilities
        - Integration with C++ logging and debugging
        
        Examples:
        >>> import nlm
        >>> 
        >>> # Create a simple brain with default configuration
        >>> config = nlm.createConfig(nlm.Presets.DEFAULT)
        >>> brain = nlm.Brain(config)
        >>> 
        >>> # Use context manager for safe resource management
        >>> with brain.context() as ctx:
        ...     ctx.step(1)
        ...     action = ctx.produce_action()
        >>> 
        >>> # Monitor progress with callbacks
        >>> def progress_callback(progress, message):
        ...     print(f"Progress: {progress:.1%}, {message}")
        >>> tracker = nlm.ProgressTracker(progress_callback)
        >>> 
        >>> # Batch operations for efficiency
        >>> results = nlm.batch_process_neurons(brain, neuron_ids, current_values)
        >>> 
        >>> # Use configuration presets
        >>> config = nlm.createConfig(nlm.Presets.EXPLORATION)
        >>> # or
        >>> config = nlm.ConfigPresets.createExploration()
    )pbdoc";
    
    // Register custom exceptions
    register_exceptions(m);

    // NeuronId class
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        Example:
        >>> import nlm
        >>> nid = nlm.NeuronId(42)
        >>> print(f"Neuron index: {nid.index()}")
        Neuron index: 42
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index,
             "Get the neuron index as a 64-bit unsigned integer")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId class
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        Example:
        >>> import nlm
        >>> sid = nlm.SynapseId(100)
        >>> print(f"Synapse index: {sid.index()}")
        Synapse index: 100
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index,
             "Get the synapse index as a 64-bit unsigned integer")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // RegionId class
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        Example:
        >>> import nlm
        >>> rid = nlm.RegionId(7)
        >>> print(f"Region index: {rid.index()}")
        Region index: 7
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index,
             "Get the region index as a 64-bit unsigned integer")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        Example:
        >>> import nlm
        >>> pid = nlm.PopulationId(3)
        >>> print(f"Population index: {pid.index()}")
        Population index: 3
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index,
             "Get the population index as a 64-bit unsigned integer")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); });

    // NeuronType enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Neuron type enumeration.
        
        Examples:
        >>> import nlm
        >>> print(nlm.NeuronType.Excitatory)
        NeuronType.Excitatory
        >>> print(nlm.NeuronType.Inhibitory)
        NeuronType.Inhibitory
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Synapse type enumeration.
        
        Examples:
        >>> import nlm
        >>> print(nlm.SynapseType.Excitatory)
        SynapseType.Excitatory
        >>> print(nlm.SynapseType.Inhibitory)
        SynapseType.Inhibitory
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage enumeration.
        
        Examples:
        >>> import nlm
        >>> print(nlm.DevelopmentalStage.Initial)
        DevelopmentalStage.Initial
        >>> print(nlm.DevelopmentalStage.Maturation)
        DevelopmentalStage.Maturation
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Neuron firing state enumeration.
        
        Examples:
        >>> import nlm
        >>> print(nlm.FiringState.Resting)
        FiringState.Resting
        >>> print(nlm.FiringState.Active)
        FiringState.Active
    )pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enum
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Action type enumeration for motor output.
        
        Examples:
        >>> import nlm
        >>> print(nlm.ActionType.MoveForward)
        ActionType.MoveForward
        >>> print(nlm.ActionType.Interact)
        ActionType.Interact
    )pbdoc")
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

    // MotorCommand enum
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command enumeration.
        
        Examples:
        >>> import nlm
        >>> print(nlm.MotorCommand.MoveForward)
        MotorCommand.MoveForward
        >>> print(nlm.MotorCommand.Interact)
        MotorCommand.Interact
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    // WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        World object type enumeration.
        
        Examples:
        >>> import nlm
        >>> print(nlm.WorldObjectType.Resource)
        WorldObjectType.Resource
        >>> print(nlm.WorldObjectType.Hazard)
        WorldObjectType.Hazard
    )pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Config class
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        Provides key-value configuration storage with type conversion support.
        Can load from JSON files, command line arguments, or programmatically.
        
        Examples:
        >>> import nlm
        >>> config = nlm.Config()
        >>> 
        >>> # Set values
        >>> config.set("brain.learning_rate", 0.01)
        >>> config.set("world.max_energy", 1.0)
        >>> 
        >>> # Get values with type conversion
        >>> learning_rate = config.get("brain.learning_rate")
        >>> energy = config.get("world.max_energy")
        >>> 
        >>> # Check if key exists
        >>> config.has("brain.learning_rate")
        True
        >>> 
        >>> # Save to file
        >>> config.save_to_file("config.json")
    )pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(
             Load configuration from a JSON file.
             
             Args:
                 filepath: Path to the JSON configuration file
                 
             Returns:
                 bool: True if loading succeeded, False otherwise
                 
             Example:
             >>> config.load_from_file("config.json")
             "pbdoc")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Constants for configuration presets
    py::class_<ConfigPresets>(m, "ConfigPresets", R"pbdoc(
        Pre-built configuration presets for common use cases.
        
        Provides factory methods for creating configurations optimized for
        different scenarios like development, production, or exploration.
        
        Examples:
        >>> import nlm
        >>> 
        >>> # Default configuration (balanced)
        >>> config = nlm.ConfigPresets.createDefault()
        >>> 
        >>> # Development configuration (aggressive learning)
        >>> config = nlm.ConfigPresets.createDevelopment()
        >>> 
        >>> # Production configuration (stable, conservative)
        >>> config = nlm.ConfigPresets.createProduction()
        >>> 
        >>> # Exploration configuration (high plasticity)
        >>> config = nlm.ConfigPresets.createExploration()
    )pbdoc")
        .def_static("create_default", &ConfigPresets::createDefault,
                   "Create a default (balanced) configuration")
        .def_static("create_development", &ConfigPresets::createDevelopment,
                   "Create a development-focused configuration")
        .def_static("create_production", &ConfigPresets::createProduction,
                   "Create a production-focused configuration")
        .def_static("create_exploration", &ConfigPresets::createExploration,
                   "Create an exploration-focused configuration");

    // Utility functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return ConfigPresets::createDefault();
    }, "Create a default configuration with sensible values");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Config creation helper with presets
    m.def("create_config", [](const std::string& preset) -> std::shared_ptr<Config> {
        if (preset == "default") {
            return ConfigPresets::createDefault();
        } else if (preset == "development") {
            return ConfigPresets::createDevelopment();
        } else if (preset == "production") {
            return ConfigPresets::createProduction();
        } else if (preset == "exploration") {
            return ConfigPresets::createExploration();
        } else {
            throw ValidationError("Unknown configuration preset: " + preset);
        }
    }, py::arg("preset"),
       "Create a configuration with the specified preset.\n\n"
       "Args:\n"
       "  preset: One of 'default', 'development', 'production', 'exploration'\n\n"
       "Examples:\n"
       "  >>> config = create_config('development')");

    // Memory management utilities
    py::class_<ResourceManager>(m, "ResourceManager", R"pbdoc(
        Context manager for resource allocation and cleanup.
        
        Provides automatic cleanup of registered resources when the manager
        is destroyed, with support for custom deleters.
        
        Examples:
        >>> import nlm
        >>> manager = nlm.ResourceManager("brain_resources")
        >>> 
        >>> # Register a resource with custom deleter
        >>> def cleanup(resource):
        ...     resource.reset()
        >>> manager.register_resource("brain", brain_ptr, cleanup)
        >>> 
        >>> # Resources are automatically cleaned up when manager is destroyed
    )pbdoc")
        .def(py::init<const std::string&>(), py::arg("name"))
        .def("register_resource",
             [](ResourceManager& self, const std::string& key, std::shared_ptr<void> resource,
                py::object deleter) {
                 auto deleterFunc = [deleter](std::shared_ptr<void> ptr) {
                     if (deleter) {
                         deleter(ptr);
                     }
                 };
                 self.registerResource(key, resource, deleterFunc);
             },
             py::arg("key"), py::arg("resource"), py::arg("deleter"),
             "Register a resource with a custom deleter function")
        .def("cleanup", &ResourceManager::cleanup,
             "Explicitly clean up all registered resources");

    // Progress tracking utilities
    py::class_<ProgressTracker>(m, "ProgressTracker", R"pbdoc(
        Progress tracking utility with callbacks.
        
        Monitors progress of long-running operations and calls back with
        progress updates. Useful for monitoring simulation steps or loading operations.
        
        Examples:
        >>> import nlm
        >>> def progress_callback(progress, message):
        ...     print(f"Progress: {progress:.1%}: {message}")
        >>> 
        >>> tracker = nlm.ProgressTracker(progress_callback, 0.1)
        >>> 
        >>> # Update progress
        >>> tracker.set(0.5, "Loading neurons")
        >>> tracker.increment(0.2, "Processing spikes")
    )pbdoc")
        .def(py::init<py::function<void(float, const std::string&)>, float>(),
             py::arg("callback"), py::arg("update_interval") = 0.1f)
        .def("update", &ProgressTracker::update,
             py::arg("progress"), py::arg("message") = "",
             "Update progress to a specific value")
        .def("increment", &ProgressTracker::increment,
             py::arg("amount") = 0.01f, py::arg("message") = "",
             "Increment progress by an amount")
        .def("set", &ProgressTracker::set,
             py::arg("progress"), py::arg("message") = "",
             "Set progress to a specific value")
        .def("get_progress", &ProgressTracker::getProgress,
             "Get current progress (0.0 to 1.0)")
        .def("reset", &ProgressTracker::reset,
             "Reset progress to 0.0");

    // Batch operations utilities
    m.def("batch_inject_current",
         [](std::shared_ptr<Brain> brain, const std::vector<std::pair<uint64_t, float>>& injections) {
             std::vector<NeuronId> neurons;
             std::vector<MembranePotential> currents;
             
             neurons.reserve(injections.size());
             currents.reserve(injections.size());
             
             for (const auto& inj : injections) {
                 neurons.emplace_back(inj.first);
                 currents.push_back(inj.second);
             }
             
             if (injections.size() == 1) {
                 brain->injectCurrent(neurons[0], currents[0]);
             } else {
                 // Create temporary vectors for batch injection
                 std::vector<NeuronId> temp_neurons = neurons;
                 for (size_t i = 0; i < neurons.size(); ++i) {
                     brain->injectCurrent(temp_neurons[i], currents[i]);
                 }
             }
         },
         py::arg("brain"), py::arg("injections"),
         R"pbdoc(
         Batch inject current into multiple neurons.
         
         Args:
             brain: Brain instance
             injections: List of (neuron_id, current) pairs
             
         Example:
         >>> import nlm
         >>> injections = [(0, 0.1), (1, 0.2), (2, 0.15)]
         >>> batch_inject_current(brain, injections)
         "pbdoc");

    m.def("batch_process_sensory",
         [](std::shared_ptr<AgentBrain> agent_brain,
            const std::vector<std::shared_ptr<SensoryInput>>& inputs) {
             for (const auto& input : inputs) {
                 SensoryPercept percept;
                 if (input) {
                     // Convert SensoryInput to SensoryPercept (simplified)
                     // In practice, this would be a proper conversion
                     agent_brain->processSensoryInput(percept);
                 }
             }
         },
         py::arg("agent_brain"), py::arg("inputs"),
         R"pbdoc(
         Process multiple sensory inputs in batch.
         
         Args:
             agent_brain: AgentBrain instance
             inputs: List of sensory inputs to process
             
         Example:
         >>> import nlm
         >>> inputs = [vision1, vision2, audio1]
         >>> batch_process_sensory(agent_brain, inputs)
         "pbdoc");

    // Type conversion utilities
    m.def("convert_to_int", convertType<double, int>,
         py::arg("value"),
         "Convert a value to integer type.");
    
    m.def("convert_to_float", convertType<int, float>,
         py::arg("value"),
         "Convert a value to float type.");
    
    m.def("convert_to_string", convertType<int, std::string>,
         py::arg("value"),
         "Convert a value to string type.");

    // Statistical utilities
    m.def("brain_statistics",
         [](const std::shared_ptr<Brain>& brain) {
             return Utils::getBrainStatistics(*brain);
         },
         py::arg("brain"),
         R"pbdoc(
         Get statistical information about a brain.
         
         Args:
             brain: Brain instance
             
         Returns:
             List of strings with statistics
             
         Example:
         >>> stats = brain_statistics(brain)
         >>> for stat in stats:
         ...     print(stat)
         "pbdoc");

    m.def("world_info",
         [](const std::shared_ptr<SimpleWorld>& world) {
             return Utils::getWorldInfo(*world);
         },
         py::arg("world"),
         R"pbdoc(
         Get information about a world.
         
         Args:
             world: SimpleWorld instance
             
         Returns:
             List of strings with world information
             
         Example:
         >>> info = world_info(world)
         >>> for item in info:
         ...     print(item)
         "pbdoc");

    // Math utilities
    m.def("normalize", &Utils::normalize,
         py::arg("data"), py::arg("min") = 0.0f, py::arg("max") = 1.0f,
         R"pbdoc(
         Normalize a vector of values to a specified range.
         
         Args:
             data: Input vector of values
             min: Minimum value in output range (default: 0.0)
             max: Maximum value in output range (default: 1.0)
             
         Returns:
             Normalized vector
             
         Example:
         >>> data = [1.0, 2.0, 3.0, 4.0, 5.0]
         >>> normalized = normalize(data, 0.0, 1.0)
         "pbdoc");

    m.def("linspace", &Utils::linspace,
         py::arg("start"), py::arg("stop"), py::arg("num"),
         R"pbdoc(
         Create a linearly spaced vector.
         
         Args:
             start: Start value
             stop: End value
             num: Number of points to generate
             
         Returns:
             Vector with linearly spaced values
             
         Example:
         >>> values = linspace(0.0, 1.0, 11)  # [0.0, 0.1, 0.2, ..., 1.0]
         "pbdoc");

    m.def("format_float", &Utils::formatFloat,
         py::arg("value"), py::arg("precision") = 6,
         R"pbdoc(
         Format a float value with specified precision.
         
         Args:
             value: Float value to format
             precision: Number of decimal places (default: 6)
             
         Returns:
             Formatted string representation
             
         Example:
         >>> format_float(3.14159, 2)
         "3.14"
         "pbdoc");

    // Context manager for Brain
    py::class_<BrainContextManager>(m, "BrainContext", R"pbdoc(
        Context manager for safe brain operations.
        
        Provides automatic reset of brain state when exiting the context
        and methods for easy interaction.
        
        Example:
        >>> import nlm
        >>> brain = nlm.Brain(config)
        >>> 
        >>> # Use context manager for automatic reset
        >>> with brain.context() as ctx:
        ...     ctx.step(1)  # Advance simulation
        ...     action = ctx.produce_action()  # Get action
        ...     ctx.inject(0, 0.5)  # Inject current
        >>> # Brain state is automatically reset on exit
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("step", &BrainContextManager::step,
             py::arg("step"), py::arg("time") = 0.0,
             "Perform a simulation step")
        .def("inject", &BrainContextManager::inject,
             py::arg("neuron_id"), py::arg("current"),
             "Inject current into a neuron")
        .def("produce_action", &BrainContextManager::produce_action,
             "Produce motor action from the brain");

    // Context manager for World
    py::class_<WorldContextManager>(m, "WorldContext", R"pbdoc(
        Context manager for safe world operations.
        
        Provides automatic reset of world state when exiting the context
        and methods for easy interaction.
        
        Example:
        >>> import nlm
        >>> world = nlm.SimpleWorld()
        >>> 
        >>> # Use context manager for automatic reset
        >>> with world.context() as ctx:
        ...     ctx.update(0.1)  # Advance simulation
        ...     action = ctx.apply(nlm.MotorCommand.MoveForward)  # Apply action
        >>> # World state is automatically reset on exit
    )pbdoc")
        .def(py::init<std::shared_ptr<SimpleWorld>>(), py::arg("world"))
        .def("update", &WorldContextManager::update,
             py::arg("timestep"), "Advance simulation")
        .def("apply", &WorldContextManager::apply,
             py::arg("command"), "Apply a motor command");

    // Enhanced Brain class with Pythonic interfaces
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        This class provides the main interface for neural simulation with
        enhanced Python features including context managers, progress tracking,
        and batch operations.
        
        Example:
        >>> import nlm
        >>> config = nlm.ConfigPresets.createDevelopment()
        >>> brain = nlm.Brain(config)
        >>> 
        >>> # Use context manager for safe operation
        >>> with brain.context() as ctx:
        ...     for step in range(10):
        ...         ctx.step(step)
        ...         action = ctx.produce_action()
        ...         # Process action in world
        >>> 
        >>> # Get brain statistics
        >>> stats = brain.statistics()
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(
             Create a brain with configuration.
             
             Args:
                 config: Configuration for the brain
                 
             Example:
             >>> brain = Brain(config)
             "pbdoc")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("current_step"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("current_step"), py::arg("current_time"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status")
        .def("context", [](std::shared_ptr<Brain> brain) {
            return std::make_shared<BrainContextManager>(brain);
        }, "Get a context manager for safe brain operations")
        .def("statistics", [](const std::shared_ptr<Brain>& brain) {
            return Utils::getBrainStatistics(*brain);
        }, "Get brain statistics as a list of strings");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        Provides enhanced Python interfaces for processing sensory input
        and decoding motor commands with improved error handling and
        validation.
        
        Example:
        >>> import nlm
        >>> brain = nlm.Brain(config)
        >>> agent_brain = nlm.AgentBrain(brain)
        >>> 
        >>> # Process sensory input
        >>> agent_brain.process_sensory_input(percept)
        >>> 
        >>> # Decode motor command
        >>> command = agent_brain.decode_motor_command()
        >>> 
        >>> # Apply neuromodulation
        >>> agent_brain.apply_reward_modulation(1.0, 0.8)
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        Provides enhanced Python interfaces with better error handling,
        validation, and context manager support.
        
        Example:
        >>> import nlm
        >>> world = nlm.SimpleWorld()
        >>> world.configure(100.0, 100.0, 10, 10)
        >>> 
        >>> # Use context manager for safe operation
        >>> with world.context() as ctx:
        ...     for step in range(100):
        ...         ctx.update(0.1)
        ...         action = ctx.apply(nlm.MotorCommand.MoveForward)
        ...         # Process action result
        >>> 
        >>> # Get world information
        >>> info = world.info()
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"),
             "Configure the world dimensions and vision")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (call each simulation step)")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"),
             "Apply motor command to agent")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept for the agent")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        .def("get_object_at", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal,
             "Get object at position (if any)")
        .def("get_width", &SimpleWorld::getWidth,
             "Get world width")
        .def("get_height", &SimpleWorld::getHeight,
             "Get world height")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy,
             "Get max energy")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set max energy")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility")
        .def("get_random_seed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        .def("context", [](std::shared_ptr<SimpleWorld> world) {
            return std::make_shared<WorldContextManager>(world);
        }, "Get a context manager for safe world operations")
        .def("info", [](const std::shared_ptr<SimpleWorld>& world) {
            return Utils::getWorldInfo(*world);
        }, "Get world information as a list of strings");

    // Enhanced SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for all sensory input.
        
        Provides enhanced Python interfaces with better validation
        and type conversion support.
        
        Example:
        >>> import nlm
        >>> # Create vision input
        >>> vision = nlm.Vision(10, 10, 1)
        >>> 
        >>> # Set data
        >>> vision.set_data([0.1, 0.2, 0.3, ...])
        >>> 
        >>> # Get data
        >>> data = vision.get_data()
        >>> 
        >>> # Get timestamp
        >>> timestamp = vision.get_timestamp()
    )pbdoc")
        .def(py::init<>())
        .def("get_type", &SensoryInput::getType,
             "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData,
             "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions,
             "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp,
             "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("get_timestamp", &SensoryInput::getTimestamp,
             "Get the timestamp (alias)")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp (alias)");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input for visual perception.
        
        Provides enhanced Python interfaces with validation and
        convenient methods for common operations.
        
        Example:
        >>> import nlm
        >>> vision = nlm.Vision(10, 10, 1)
        >>> vision.set_data([0.1, 0.2, ..., 1.0])
        >>> 
        >>> # Get dimensions
        >>> width, height, channels = vision.width, vision.height, vision.channels
        >>> 
        >>> # Convert to numpy-like array (if numpy available)
        >>> import numpy as np
        >>> array = np.array(vision.get_data())
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            validateNonNegative(data.size(), "data size");
            self.setData(data);
        }, py::arg("data"),
             "Set vision data with validation")
        .def("get_width", &Vision::getWidth,
             "Get vision width")
        .def("get_height", &Vision::getHeight,
             "Get vision height")
        .def("get_channels", &Vision::getChannels,
             "Get number of channels")
        .def_readonly("width", &Vision::getWidth,
                     "Vision width (read-only property)")
        .def_readonly("height", &Vision::getHeight,
                     "Vision height (read-only property)")
        .def_readonly("channels", &Vision::getChannels,
                     "Vision channels (read-only property)");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input for sound perception.
        
        Provides enhanced Python interfaces with validation and
        convenient methods for common operations.
        
        Example:
        >>> import nlm
        >>> audio = nlm.Audio(22050, 1024)
        >>> audio.set_data([0.1, 0.2, ..., 0.9])
        >>> 
        >>> # Get audio properties
        >>> sample_rate = audio.get_sample_rate()
        >>> num_samples = audio.get_num_samples()
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            validateNonNegative(data.size(), "data size");
            self.setData(data);
        }, py::arg("data"),
             "Set audio data with validation")
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"),
             "Set audio sample rate")
        .def("get_sample_rate", &Audio::getSampleRate,
             "Get audio sample rate")
        .def("get_num_samples", &Audio::getNumSamples,
             "Get number of audio samples");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input.
        
        Represents signals from within the brain (homeostatic, neuromodulatory, etc.).
        
        Example:
        >>> import nlm
        >>> signals = nlm.InternalSignals()
        >>> signals.add_signal(0.5)
        >>> signals.add_signal(0.8)
        >>> signals.add_signal(0.3)
        >>> 
        >>> # Get all signals
        >>> data = signals.get_data()
    )pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"),
             "Add an internal signal value")
        .def("clear_signals", &InternalSignals::clearSignals,
             "Clear all internal signals");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output.
        
        Provides enhanced Python interfaces with validation, convenience methods,
        and serialization support.
        
        Example:
        >>> import nlm
        >>> # Create action from type
        >>> action = nlm.Action(nlm.ActionType.MoveForward)
        >>> 
        >>> # Create action with parameters
        >>> action = nlm.Action(nlm.ActionType.Custom, [0.5, 0.7, 0.9])
        >>> 
        >>> # Get action properties
        >>> print(f"Type: {action.get_type()}")
        >>> print(f"Parameters: {action.get_parameters()}")
        >>> print(f"Name: {action.get_name()}")
        >>> 
        >>> # Clone action
        >>> cloned = action.clone()
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType,
             "Get the action type")
        .def("set_type", &Action::setType, py::arg("type"),
             "Set the action type")
        .def("get_parameters", &Action::getParameters,
             "Get the action parameters")
        .def("set_parameters", &Action::setParameters, py::arg("params"),
             "Set the action parameters")
        .def("get_name", &Action::getName,
             "Get the action name as a string")
        .def("clone", &Action::clone,
             "Clone the action");

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation.
        
        Represents an object in the simulated world with position, type, and properties.
        
        Example:
        >>> import nlm
        >>> # Create empty object
        >>> obj = nlm.WorldObject()
        >>> 
        >>> # Create resource object
        >>> obj = nlm.WorldObject(10.0, 10.0, nlm.WorldObjectType.Resource, 1.0, 0.5)
        >>> 
        >>> # Access properties
        >>> print(f"Position: ({obj.x}, {obj.y})")
        >>> print(f"Type: {obj.type}")
        >>> print(f"Value: {obj.value}")
        >>> print(f"Radius: {obj.radius}")
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x,
                      "X coordinate")
        .def_readwrite("y", &WorldObject::y,
                      "Y coordinate")
        .def_readwrite("radius", &WorldObject::radius,
                      "Object radius")
        .def_readwrite("type", &WorldObject::type,
                      "Object type")
        .def_readwrite("value", &WorldObject::value,
                      "Object value (reward/damage)")
        .def_readwrite("active", &WorldObject::active,
                      "Whether object is active");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state.
        
        Contains physical properties and internal state of the agent.
        
        Example:
        >>> import nlm
        >>> body = nlm.AgentBody()
        >>> 
        >>> # Access properties
        >>> print(f"Position: ({body.x}, {body.y})")
        >>> print(f"Orientation: {body.orientation} radians")
        >>> print(f"Energy: {body.energy} / {body.health}")
        >>> 
        >>> # Reset body
        >>> body.reset()
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x,
                      "X position")
        .def_readwrite("y", &AgentBody::y,
                      "Y position")
        .def_readwrite("orientation", &AgentBody::orientation,
                      "Orientation in radians")
        .def_readwrite("velocity_x", &AgentBody::velocityX,
                      "X velocity")
        .def_readwrite("velocity_y", &AgentBody::velocityY,
                      "Y velocity")
        .def_readwrite("angular_velocity", &AgentBody::angularVelocity,
                      "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy,
                      "Energy level (0-1)")
        .def_readwrite("health", &AgentBody::health,
                      "Health level (0-1)")
        .def_readwrite("age", &AgentBody::age,
                      "Age in simulation steps")
        .def_readwrite("is_moving", &AgentBody::isMoving,
                      "Whether agent is moving")
        .def_readwrite("is_turning", &AgentBody::isTurning,
                      "Whether agent is turning")
        .def_readwrite("last_action_time", &AgentBody::lastActionTime,
                      "Time of last action")
        .def("reset", &AgentBody::reset,
             "Reset all body state to initial values");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Action result from world.
        
        Contains the result of an action applied in the world,
        including reward and success status.
        
        Example:
        >>> import nlm
        >>> result = nlm.ActionResult()
        >>> 
        >>> # Create successful result with reward
        >>> result = nlm.ActionResult(reward=1.0, success=True, message="Found food")
        >>> 
        >>> # Access properties
        >>> print(f"Reward: {result.reward}")
        >>> print(f"Success: {result.success}")
        >>> print(f"Message: {result.message}")
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::init<bool>(), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward,
                      "Reward value")
        .def_readwrite("success", &ActionResult::success,
                      "Whether action succeeded")
        .def_readwrite("message", &ActionResult::message,
                      "Result message");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory data packet - what the agent perceives.
        
        Contains low-level sensory signals including vision, touch, internal
        state, proprioception, and audio signals.
        
        Example:
        >>> import nlm
        >>> percept = nlm.SensoryPercept()
        >>> 
        >>> # Set vision data
        >>> vision_data = [0.1, 0.2, ..., 1.0]  # 10x10 grid flattened
        >>> percept.set_vision(vision_data)
        >>> 
        >>> # Get all signals
        >>> all_signals = percept.get_all_signals()
        >>> 
        >>> # Access individual modalities
        >>> touch = percept.get_touch()
        >>> internal = percept.get_internal()
    )pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision,
             "Get vision data")
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data")
        .def("get_vision_width", &SensoryPercept::getVisionWidth,
             "Get vision width")
        .def("get_vision_height", &SensoryPercept::getVisionHeight,
             "Get vision height")
        .def("get_touch", &SensoryPercept::getTouch,
             "Get touch data")
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data")
        .def("get_internal", &SensoryPercept::getInternal,
             "Get internal signals")
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals")
        .def("get_proprioception", &SensoryPercept::getProprioception,
             "Get proprioception data")
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data")
        .def("get_audio", &SensoryPercept::getAudio,
             "Get audio data")
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        .def("get_all_signals", &SensoryPercept::getAllSignals,
             "Get all signals concatenated")
        .def("get_timestamp", &SensoryPercept::getTimestamp,
             "Get timestamp")
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp");

    // Constants
    m.attr("VISION_TYPE") = "Vision";
    m.attr("AUDIO_TYPE") = "Audio";
    m.attr("INTERNAL_SIGNALS_TYPE") = "InternalSignals";

    // Error classes for user convenience
    m.attr("BrainInitializationError") = py::cast(py::type::of<BrainInitializationError>());
    m.attr("InvalidOperationError") = py::cast(py::type::of<InvalidOperationError>());
    m.attr("ResourceError") = py::cast(py::type::of<ResourceError>());
    m.attr("ValidationError") = py::cast(py::type::of<ValidationError>());

    // Error throwing functions
    m.def("throw_brain_initialization_error", [](const std::string& msg) {
        throw BrainInitializationError(msg);
    }, py::arg("message"),
          "Throw a BrainInitializationError with the given message");
    
    m.def("throw_invalid_operation_error", [](const std::string& msg) {
        throw InvalidOperationError(msg);
    }, py::arg("message"),
          "Throw an InvalidOperationError with the given message");
    
    m.def("throw_resource_error", [](const std::string& msg) {
        throw ResourceError(msg);
    }, py::arg("message"),
          "Throw a ResourceError with the given message");
    
    m.def("throw_validation_error", [](const std::string& msg) {
        throw ValidationError(msg);
    }, py::arg("message"),
          "Throw a ValidationError with the given message");

    // Integration with C++ logging (placeholder for actual logging)
    m.def("log_to_file", [](const std::string& message, const std::string& filepath) {
        std::ofstream file(filepath, std::ios::app);
        if (file.is_open()) {
            file << "[" << std::chrono::system_clock::now() << "] " << message << std::endl;
        }
    }, py::arg("message"), py::arg("filepath"),
       "Log a message to a file with timestamp");

    m.def("log_brain_status", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        brain->logStatus();
        m.attr("log_to_file")("Brain status logged", filepath);
    }, py::arg("brain"), py::arg("filepath"),
       "Log brain status and save to file");

    // Type conversion helpers for Python-idiomatic code
    m.def("as_float", [](py::object obj) {
        if (py::isinstance<py::int_>(obj)) {
            return obj.cast<int>().operator float();
        } else if (py::isinstance<py::float_>(obj)) {
            return obj.cast<float>();
        } else {
            throw ValidationError("Cannot convert to float");
        }
    }, py::arg("obj"),
         "Convert Python object to float (int or float)");

    m.def("as_int", [](py::object obj) {
        if (py::isinstance<py::int_>(obj)) {
            return obj.cast<int>();
        } else if (py::isinstance<py::float_>(obj)) {
            return static_cast<int>(obj.cast<float>());
        } else {
            throw ValidationError("Cannot convert to int");
        }
    }, py::arg("obj"),
         "Convert Python object to int (float or int)");

    m.def("as_bool", [](py::object obj) {
        return obj.cast<bool>();
    }, py::arg("obj"),
         "Convert Python object to bool");

    // Additional utility functions
    m.def("safe_divide", [](float a, float b, float default_value = 0.0f) {
        if (std::abs(b) < 1e-10) {
            return default_value;
        }
        return a / b;
    }, py::arg("a"), py::arg("b"), py::arg("default_value") = 0.0f,
       "Safely divide a by b, returning default_value if b is near zero");

    m.def("clamp", [](float value, float min, float max) {
        return std::max(min, std::min(value, max));
    }, py::arg("value"), py::arg("min"), py::arg("max"),
       "Clamp a value between min and max");

    m.def("lerp", [](float start, float end, float t) {
        return start + (end - start) * t;
    }, py::arg("start"), py::arg("end"), py::arg("t"),
       "Linear interpolation between start and end");

    // Documentation and help functions
    m.def("get_help", [](const std::string& topic) {
        if (topic == "config") {
            return R"pbdoc(
                Config:
                - Create a config with: config = nlm.Config()
                - Set values with: config.set("key", value)
                - Get values with: config.get("key")
                - Load from file: config.load_from_file("config.json")
                - Save to file: config.save_to_file("config.json")
            )pbdoc";
        } else if (topic == "brain") {
            return R"pbdoc(
                Brain:
                - Create brain with: brain = nlm.Brain(config)
                - Initialize: brain.initialize()
                - Step simulation: brain.step(step)
                - Get action: brain.produce_action()
                - Save: brain.save("brain.bin")
                - Load: brain.load("brain.bin")
            )pbdoc";
        } else if (topic == "world") {
            return R"pbdoc(
                SimpleWorld:
                - Create world: world = nlm.SimpleWorld()
                - Configure: world.configure(width, height, vision_width, vision_height)
                - Update: world.update(timestep)
                - Get percept: world.get_sensory_percept()
                - Get agent: world.get_agent_body()
            )pbdoc";
        }
        return "No help available for topic: " + topic;
    }, py::arg("topic"),
       "Get help information for a topic");

    m.def("get_version", []() {
        return "NLM Python Bindings 1.0.0";
    }, "Get the version of the NLM Python bindings");

    m.def("get_system_info", []() {
        std::vector<std::string> info;
        info.push_back("Platform: Linux");
        info.push_back("Compiler: GCC");
        info.push_back("Python bindings: pybind11");
        return info;
    }, "Get system information");
}

} // namespace nlm