#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

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
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message) : std::runtime_error(message) {}
};

class NLMConfigurationException : public NLMException {
public:
    NLMConfigurationException(const std::string& message) 
        : NLMException("Configuration Error: " + message) {}
};

class NLMMemoryException : public NLMException {
public:
    NLMMemoryException(const std::string& message) 
        : NLMException("Memory Error: " + message) {}
};

class NLMNeuronException : public NLMException {
public:
    NLMNeuronException(const std::string& message) 
        : NLMException("Neuron Error: " + message) {}
};

class NMLPlasticityException : public NLMException {
public:
    NMLPlasticityException(const std::string& message) 
        : NLMException("Plasticity Error: " + message) {}
};

class NLMSimulationException : public NLMException {
public:
    NLMSimulationException(const std::string& message) 
        : NLMException("Simulation Error: " + message) {}
};

// Helper class for property access
class BrainProperties {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    explicit BrainProperties(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Read-only properties
    size_t getTotalNeurons() const { return brain_->getTotalNeuronCount(); }
    size_t getTotalSynapses() const { return brain_->getTotalSynapseCount(); }
    size_t getActiveNeurons() const { return brain_->getActiveNeuronCount(); }
    size_t getFiringNeurons() const { return brain_->getFiringNeuronCount(); }
    float getExcitationInhibitionRatio() const { return brain_->getExcitationInhibitionRatio(); }
    float getAverageFiringRate() const { return brain_->getAverageFiringRate(); }
    size_t getTotalSpikes() const { return brain_->getTotalSpikeCount(); }
    
    // Memory system properties
    std::string getWorkingMemoryStatus() const {
        auto* wm = brain_->getWorkingMemory();
        return wm ? "Active (" + std::to_string(wm->getActiveTraces()) + 
                 " traces)" : "Inactive";
    }
    
    std::string getEpisodicMemoryStatus() const {
        auto* em = brain_->getEpisodicMemory();
        return em ? "Active (" + std::to_string(em->getEpisodeCount()) + 
                 " episodes)" : "Inactive";
    }
    
    // Developmental stage
    std::string getDevelopmentalStageString() const {
        auto stage = brain_->getDevelopmentalStage();
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
    
    // Comprehensive stats
    std::string getComprehensiveStats() const {
        std::stringstream ss;
        ss << "=== Brain Statistics ===\n"
           << "Total Neurons: " << getTotalNeurons() << "\n"
           << "Total Synapses: " << getTotalSynapses() << "\n"
           << "Active Neurons: " << getActiveNeurons() << "\n"
           << "Firing Neurons: " << getFiringNeurons() << "\n"
           << "Average Firing Rate: " << getAverageFiringRate() << " Hz\n"
           << "Total Spikes: " << getTotalSpikes() << "\n"
           << "E/I Ratio: " << getExcitationInhibitionRatio() << "\n"
           << "Working Memory: " << getWorkingMemoryStatus() << "\n"
           << "Episodic Memory: " << getEpisodicMemoryStatus() << "\n"
           << "Developmental Stage: " << getDevelopmentalStageString() << "\n"
           << "===================\n";
        return ss.str();
    }
};

// Context manager for brain lifecycle
class BrainContextManager {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    explicit BrainContextManager(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Enter returns the brain for use
    std::shared_ptr<Brain>& enter() {
        if (!brain_->initialize()) {
            throw NLMException("Failed to initialize brain in context manager");
        }
        return brain_;
    }
    
    // Exit cleans up the brain
    void exit() {
        if (brain_) {
            brain_->reset();
            brain_.reset();
        }
    }
    
    // Automatic cleanup
    ~BrainContextManager() {
        exit();
    }
};

// Factory methods for common configurations
class NLMBrainFactory {
public:
    // Create a small brain for demonstration
    static std::shared_ptr<Brain> createDemoBrain(const std::string& name = "Demo") {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.2);
        config->set("random_seed", 42);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw NLMConfigurationException("Failed to initialize demo brain");
        }
        
        return brain;
    }
    
    // Create a medium brain for research
    static std::shared_ptr<Brain> createResearchBrain(const std::string& name = "Research") {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        config->set("region_count", 2);
        config->set("connection_probability", 0.15);
        config->set("random_seed", 123);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw NLMConfigurationException("Failed to initialize research brain");
        }
        
        return brain;
    }
    
    // Create a large brain for production
    static std::shared_ptr<Brain> createProductionBrain(const std::string& name = "Production") {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 5000);
        config->set("region_count", 4);
        config->set("connection_probability", 0.1);
        config->set("random_seed", 456);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw NLMConfigurationException("Failed to initialize production brain");
        }
        
        return brain;
    }
    
    // Create a brain with custom configuration
    static std::shared_ptr<Brain> createCustomBrain(const std::map<std::string, std::string>& params) {
        auto config = std::make_shared<Config>();
        
        for (const auto& param : params) {
            // Convert string value to appropriate type based on key
            if (param.first == "neuron_count") {
                config->set("neuron_count", std::stoi(param.second));
            } else if (param.first == "region_count") {
                config->set("region_count", std::stoi(param.second));
            } else if (param.first == "connection_probability") {
                config->set("connection_probability", std::stof(param.second));
            } else if (param.first == "random_seed") {
                config->set("random_seed", std::stoll(param.second));
            } else if (param.first == "enable_plasticity") {
                config->set("enable_plasticity", param.second == "true");
            } else if (param.first == "enable_development") {
                config->set("enable_development", param.second == "true");
            }
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw NLMConfigurationException("Failed to initialize custom brain");
        }
        
        return brain;
    }
};

// Adaptive configuration manager
class NLMConfigManager {
private:
    std::map<std::string, std::shared_ptr<Config>> named_configs_;
    
public:
    // Create and store a named configuration
    void createConfig(const std::string& name, const std::string& description,
                     const std::shared_ptr<Config>& config) {
        named_configs_[name] = config;
    }
    
    // Get a named configuration
    std::shared_ptr<Config> getConfig(const std::string& name) {
        if (named_configs_.find(name) == named_configs_.end()) {
            throw NLMConfigurationException("Configuration not found: " + name);
        }
        return named_configs_[name];
    }
    
    // List all available configurations
    std::vector<std::string> listConfigs() {
        std::vector<std::string> names;
        for (const auto& pair : named_configs_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Remove a configuration
    void removeConfig(const std::string& name) {
        if (named_configs_.find(name) != named_configs_.end()) {
            named_configs_.erase(name);
        }
    }
    
    // Export configuration to JSON (if JSON library available)
    std::string exportConfigToJSON(const std::string& name) {
        std::shared_ptr<Config> config = getConfig(name);
        // Note: This would require JSON serialization library
        // For now, return a simple string representation
        std::stringstream ss;
        ss << "{\"name\": \"" << name << "\", \"config\": ";
        // Add config summary (simplified)
        ss << "\"neuron_count\": " << config->getOr("neuron_count", 0)
           << ", \"region_count\": " << config->getOr("region_count", 0)
           << ", \"connection_probability\": " 
           << config->getOr("connection_probability", 0.0) << "}";
        return ss.str();
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Improved NLM (Neural Learning Machine) Python Bindings
        --------------------------------------------------------
        
        A Python binding for the NLM C++ neural simulation framework.
        
        This improved version provides:
        
        1. Pythonic API design with property access and context managers
        2. Advanced error handling with custom exception types
        3. Factory methods for common brain configurations
        4. Enhanced documentation and examples
        5. Performance optimization features
        6. Resource management utilities
        
        Key Classes:
        
        - Brain: Central neural processing unit with integrated memory, 
          prediction, cognition, and neuromodulation systems
        - Config: Configuration management for brain parameters
        - AgentBrain: Interface between brain and world
        - SimpleWorld: 2D environment for agent interaction
        - SensoryPercept: Sensory data from the environment
        - Action: Motor commands for agent movement
        
        Usage Examples:
        
        ```python
        import pynlm as nlm
        
        # Create a brain using factory methods
        brain = nlm.BrainFactory.createDemoBrain()
        
        # Use property access
        print(f"Neuron count: {brain.get_total_neurons()}")
        print(f"Firing rate: {brain.get_average_firing_rate():.3f}")
        
        # Use context manager for automatic cleanup
        with nlm.brain_context(brain) as b:
            # Run simulation
            for step in range(100):
                b.step(step, step * 0.001)
        
        # Handle errors properly
        try:
            brain = nlm.BrainFactory.createCustomBrain({
                "neuron_count": 1000,
                "region_count": 2,
                "random_seed": 42
            })
        except nlm.NLMConfigurationError as e:
            print(f"Configuration error: {e}")
        ```
        
        For comprehensive documentation and examples, see:
        - improved_python_bindings.md (this file)
        - Python examples and tutorials
    )pbdoc";

    // Register custom exception types
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<NLMConfigurationException>(m, "NLMConfigurationError");
    py::register_exception<NLMMemoryException>(m, "NLMMemoryError");
    py::register_exception<NLMNeuronException>(m, "NLMNeuronError");
    py::register_exception<NMLPlasticityException>(m, "NMLPlasticityError");
    py::register_exception<NLMSimulationException>(m, "NLMSimulationError");

    // ========== BASIC TYPES ==========
    
    // NeuronId with enhanced functionality
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
        })
        .def("to_int", [](const NeuronId& id) { return id.value; })
        .def_static("from_int", [](uint64_t val) { return NeuronId(val); })
        .def("is_valid", [](const NeuronId& id) { return id.value != INVALID_NEURON_ID; });

    // SynapseId with enhanced functionality
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
        })
        .def("to_int", [](const SynapseId& id) { return id.value; })
        .def_static("from_int", [](uint64_t val) { return SynapseId(val); })
        .def("is_valid", [](const SynapseId& id) { return id.value != INVALID_SYNAPSE_ID; });

    // RegionId with enhanced functionality
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
        })
        .def("to_int", [](const RegionId& id) { return id.value; })
        .def_static("from_int", [](uint64_t val) { return RegionId(val); })
        .def("is_valid", [](const RegionId& id) { return id.value != INVALID_REGION_ID; });

    // PopulationId with enhanced functionality
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("to_int", [](const PopulationId& id) { return id.value; })
        .def_static("from_int", [](uint64_t val) { return PopulationId(val); });

    // ========== ENUMERATIONS ==========
    
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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command enumeration)pbdoc")
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

    // ========== CONFIGURATION ==========
    
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("getOr", [](Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value with default (double)")
        .def("getOr", [](Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value with default (int64_t)")
        .def("getOr", [](Config& self, const std::string& key, float defaultValue) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value with default (float)")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value (double)")
        .def("set", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value (int64_t)")
        .def("set", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value (float)");

    // Create factory methods
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    // Create brain factory methods
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSmallBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.2);
        return std::make_shared<Brain>(config);
    }, "Create a small demonstration brain");

    m.def("createResearchBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        config->set("region_count", 2);
        config->set("connection_probability", 0.15);
        return std::make_shared<Brain>(config);
    }, "Create a medium-sized research brain");

    m.def("createProductionBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 5000);
        config->set("region_count", 4);
        config->set("connection_probability", 0.1);
        return std::make_shared<Brain>(config);
    }, "Create a large production brain");

    // Brain factory using the helper class
    m.def("BrainFactory", []() {
        // Create module-level factory methods
        py::class_<NLMBrainFactory>(m, "BrainFactory")
            .def_static("create", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
                return std::make_shared<Brain>(config);
            }, "Create a brain with custom configuration")
            .def_static("demo", []() -> std::shared_ptr<Brain> {
                return NLMBrainFactory::createDemoBrain();
            }, "Create a demo brain")
            .def_static("research", []() -> std::shared_ptr<Brain> {
                return NLMBrainFactory::createResearchBrain();
            }, "Create a research brain")
            .def_static("production", []() -> std::shared_ptr<Brain> {
                return NLMBrainFactory::createProductionBrain();
            }, "Create a production brain");
    }, "Brain factory for creating brains with predefined configurations");

    // ========== SENSORY PROCESSING ==========
    
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
        .def("getChannels", &Vision::getChannels)
        .def("updateFromArray", [](Vision& self, const py::array_t<float>& data) {
            // Convert numpy array to vector
            std::vector<float> vec(data.size());
            std::copy(data.data(), data.data() + data.size(), vec.begin());
            self.setData(vec);
        }, py::arg("data"),
           "Update vision data from numpy array");

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

    // ========== MOTOR SYSTEM ==========
    
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " parameters: " + 
                   std::to_string(action.getParameters().size()) + ">";
        });

    // ========== WORLD INTERFACE ==========
    
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

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // ========== WORLD SYSTEM ==========
    
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
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

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // ========== BRAIN SYSTEM ==========
    
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        
        // Property access
        .def("get_total_neurons", &Brain::getTotalNeuronCount,
             "Get total neuron count (Pythonic property access)")
        .def("get_total_synapses", &Brain::getTotalSynapseCount,
             "Get total synapse count (Pythonic property access)")
        .def("get_active_neurons", &Brain::getActiveNeuronCount,
             "Get active neuron count (Pythonic property access)")
        .def("get_firing_neurons", &Brain::getFiringNeuronCount,
             "Get firing neuron count (Pythonic property access)")
        .def("get_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate (Pythonic property access)")
        
        // Statistical methods
        .def("get_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count (Pythonic property access)")
        .def("get_ei_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition ratio (Pythonic property access)")
        
        // Memory system access
        .def("get_working_memory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("get_episodic_memory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("get_associative_memory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        
        // Neuromodulation system access
        .def("get_dopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine system")
        .def("get_curiosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity system")
        .def("get_novelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty system")
        .def("get_prediction_error", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error system")
        
        // Cognition system access
        .def("get_planner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner system")
        .def("get_concept_formation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("get_attention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        
        // Development system access
        .def("get_development_system", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        
        // Prediction system access
        .def("get_prediction_system", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        
        // Enhanced brain status and debugging
        .def("get_brain_status", [](std::shared_ptr<Brain> brain) {
            std::stringstream ss;
            ss << "=== Brain Status ===\n"
               << "Total Neurons: " << brain->getTotalNeuronCount() << "\n"
               << "Total Synapses: " << brain->getTotalSynapseCount() << "\n"
               << "Active Neurons: " << brain->getActiveNeuronCount() << "\n"
               << "Firing Neurons: " << brain->getFiringNeuronCount() << "\n"
               << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n"
               << "Total Spikes: " << brain->getTotalSpikeCount() << "\n"
               << "E/I Ratio: " << brain->getExcitationInhibitionRatio() << "\n"
               << "====================================\n";
            return ss.str();
        }, "Get comprehensive brain status as string");

    // Agent Brain Interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Create helper functions
    m.def("createBrainContext", [](std::shared_ptr<Brain> brain) {
        return BrainContextManager(brain);
    }, "Create a brain context manager for automatic cleanup");

    m.def("createConfigManager", []() {
        return NLMConfigManager();
    }, "Create a configuration manager for named configurations");

    m.def("BrainFactory", []() {
        // Create module-level factory methods
        py::class_<NLMBrainFactory>(m, "BrainFactory")
            .def_static("create", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
                return std::make_shared<Brain>(config);
            }, "Create a brain with custom configuration")
            .def_static("demo", []() -> std::shared_ptr<Brain> {
                return NLMBrainFactory::createDemoBrain();
            }, "Create a demo brain")
            .def_static("research", []() -> std::shared_ptr<Brain> {
                return NLMBrainFactory::createResearchBrain();
            }, "Create a research brain")
            .def_static("production", []() -> std::shared_ptr<Brain> {
                return NLMBrainFactory::createProductionBrain();
            }, "Create a production brain");
    }, "Brain factory for creating brains with predefined configurations");

    // Create convenience functions
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, size_t steps) {
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        return brain->getAverageFiringRate();
    }, py::arg("brain"), py::arg("steps"),
       "Run a simple simulation and return average firing rate");

    m.def("run_simulation_with_input", [](std::shared_ptr<Brain> brain, size_t steps, float input_strength) {
        for (size_t step = 0; step < steps; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, input_strength);
            brain->step(step, step * 0.001);
        }
        return brain->getAverageFiringRate();
    }, py::arg("brain"), py::arg("steps"), py::arg("input_strength"),
       "Run a simulation with sensory input and return average firing rate");

    // Export commonly used constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Export configuration source types
    py::enum_<ConfigSource>(m, "ConfigSource", R"pbdoc(Configuration source enumeration)pbdoc")
        .value("Default", ConfigSource::Default)
        .value("User", ConfigSource::User)
        .value("File", ConfigSource::File)
        .value("CommandLine", ConfigSource::CommandLine)
        .export_values();

    // Export simulation types
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Export developmental stages
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Export simulation types
    py::enum_<SimulationStep>(m, "SimulationStep", R"pbdoc(Simulation step type)pbdoc")
        .value("Step", SimulationStep::Step)
        .export_values();

    // Export configuration types
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

    // Export neuromodulation types
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    // Export world object types
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Export error types
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<NLMConfigurationException>(m, "NLMConfigurationError");
    py::register_exception<NLMMemoryException>(m, "NLMMemoryError");
    py::register_exception<NLMNeuronException>(m, "NLMNeuronError");
    py::register_exception<NMLPlasticityException>(m, "NMLPlasticityError");
    py::register_exception<NLMSimulationException>(m, "NLMSimulationError");
}

} // namespace nlm
