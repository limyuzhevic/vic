#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <limits>
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

namespace py = pybind11;
namespace nlm {

// Custom NLM exception hierarchy for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError("Brain error: " + msg) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& msg) : NLMError("Memory error: " + msg) {}
};

class IntegrationError : public NLMError {
public:
    explicit IntegrationError(const std::string& msg) : NLMError("Integration error: " + msg) {}
};

// Utility functions for enhanced Python bindings
namespace py_utils {

    // Validate configuration parameter bounds
    template<typename T>
    T clampConfigValue(const std::string& key, T value, T min, T max, T default_val) {
        if (value < min) {
            return min;
        }
        if (value > max) {
            return max;
        }
        return value;
    }

    // Safe parameter extraction with validation
    template<typename T>
    T safeGetConfig(const std::shared_ptr<Config>& config, 
                    const std::string& key, 
                    T default_val,
                    T min_val = std::numeric_limits<T>::lowest(),
                    T max_val = std::numeric_limits<T>::max()) {
        try {
            if (!config->has(key)) {
                return default_val;
            }
            T value = config->get<T>(key);
            return clampConfigValue(key, value, min_val, max_val, default_val);
        } catch (const std::exception& e) {
            throw ConfigError("Failed to get config key '" + key + "': " + e.what());
        }
    }

    // Safe neuron ID validation
    NeuronId validateNeuronId(const std::vector<Neuron*>& neurons, size_t index) {
        if (index >= neurons.size()) {
            throw BrainError("Neuron index out of bounds: " + std::to_string(index) + 
                           " (total neurons: " + std::to_string(neurons.size()) + ")");
        }
        return neurons[index]->getId();
    }

    // Safe region ID validation
    RegionId validateRegionId(const std::vector<std::unique_ptr<NeuralRegion>>& regions, size_t index) {
        if (index >= regions.size()) {
            throw BrainError("Region index out of bounds: " + std::to_string(index) +
                           " (total regions: " + std::to_string(regions.size()) + ")");
        }
        return regions[index]->getId();
    }

    // Validate action parameters
    void validateActionParameters(const Action& action) {
        if (static_cast<size_t>(action.getType()) >= 50) {  // Arbitrary large number
            throw BrainError("Invalid action type: " + std::to_string(static_cast<int>(action.getType())));
        }
        const auto& params = action.getParameters();
        if (params.size() > 10) {  // Reasonable parameter limit
            throw BrainError("Action parameters too large: " + std::to_string(params.size()));
        }
    }

    // Safe sensory data access
    std::vector<float> safeGetSensoryData(const SensoryInput& input, size_t max_dims = 1000000) {
        auto data = input.getData();
        if (data.size() > max_dims) {
            throw BrainError("Sensory data too large: " + std::to_string(data.size()) +
                           " (max allowed: " + std::to_string(max_dims) + ")");
        }
        return data;
    }

    // Create default configuration with validation
    std::shared_ptr<Config> createValidatedConfig() {
        auto config = std::make_shared<Config>();
        
        // Set safe default values with validation
        config->set("neuron_count", static_cast<size_t>(1000));
        config->set("region_count", static_cast<size_t>(1));
        config->set("connection_probability", 0.1f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", 42ull);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        
        return config;
    }

    // Batch operation helper
    template<typename Callable>
    std::vector<NeuronId> batchOperation(const std::vector<Neuron*>& neurons, 
                                        Callable&& operation,
                                        const std::string& operation_name) {
        std::vector<NeuronId> results;
        try {
            for (size_t i = 0; i < neurons.size(); ++i) {
                results.push_back(operation(neurons[i]));
            }
        } catch (...) {
            throw BrainError("Batch operation '" + operation_name + "' failed");
        }
        return results;
    }

} // namespace py_utils

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ==============================================
        
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This module provides access to the complete artificial brain system, including:
        
        * **Brain class**: Central neural simulation with integrated memory, prediction,
          cognition, and neuromodulation systems
        * **Config class**: Configuration management with validation and serialization
        * **AgentBrain class**: Interface for agent-world interaction
        * **SimpleWorld class**: 2D simulation environment
        * **SensoryInput classes**: Vision, audio, and internal signal processing
        * **Action classes**: Motor commands and behavior selection
        
        The bindings include enhanced error handling, type safety, and comprehensive
        documentation for better Python developer experience.
        
        Examples:
            >>> import pynlm
            >>> config = pynlm.create_default_config()
            >>> brain = pynlm.create_brain(config)
            >>> world = pynlm.create_simple_world()
            >>> agent = pynlm.create_agent_brain(brain)
            >>> agent.initialize(world)
        
        .. version:: 0.1.0
        .. author:: NLM Research Team
        .. license:: MIT
    )pbdoc";

    // Register custom NLM exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<MemoryError>(m, "MemoryError");
    py::register_exception<IntegrationError>(m, "IntegrationError");

    // NeuronId class with enhanced functionality
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron in the neural network.
        
        NeuronId provides a robust identifier system for neurons, supporting
        equality comparison, hashing for dictionary keys, and string representation.
        
        Examples:
            >>> nid = nlm.NeuronId(42)
            >>> nid.value
            42
            >>> str(nid)
            '<NeuronId: 42>'
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             R"pbdoc(Construct a NeuronId with a specific value.
             
             Args:
                 value: Unique identifier value for the neuron
                 
             Raises:
                 ValueError: If value is negative
            )pbdoc")
        .def_readwrite("value", &NeuronId::value,
                      R"pbdoc(The integer value of this neuron identifier)pbdoc")
        .def("index", &NeuronId::index,
             R"pbdoc(Get the numeric index of this neuron identifier.
             
             Returns:
                 Integer index representation
             )pbdoc")
        .def("__eq__", &NeuronId::operator==,
             R"pbdoc(Compare neuron identifiers for equality.
             
             Args:
                 other: Another NeuronId to compare against
                 
             Returns:
                 True if both identifiers have the same value
             )pbdoc")
        .def("__ne__", &NeuronId::operator!=,
             R"pbdoc(Compare neuron identifiers for inequality.
             
             Args:
                 other: Another NeuronId to compare against
                 
             Returns:
                 True if identifiers differ
             )pbdoc")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             R"pbdoc(Generate hash for use in dictionaries and sets.
             
             Returns:
                 Hash value suitable for collections
             )pbdoc")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, R"pbdoc(String representation of the neuron identifier.
        
        Returns:
            Formatted string like '<NeuronId: 42>'
        )pbdoc")
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        }, R"pbdoc(String representation for display.
        
        Returns:
            String representation of the identifier value
        )pbdoc");

    // SynapseId class
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse in the neural network.
        
        SynapseId provides identification for synaptic connections between neurons,
        supporting equality operations and dictionary usage.
        )pbdoc")
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

    // RegionId class
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        RegionId identifies distinct computational regions within the brain,
        each containing multiple neural populations.
        )pbdoc")
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

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        PopulationId identifies groups of neurons with similar properties
        and connectivity patterns within brain regions.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Type classification for neurons in the neural network.
        
        Neurons are categorized by their functional role in information processing:
        - Excitatory: Promote neural activation
        - Inhibitory: Suppress neural activation  
        - Modulatory: Release neuromodulators affecting plasticity
        - Sensory: Process external/internal sensory input
        - Motor: Generate action commands
        - Internal: Handle internal state and cognition
        )pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Type classification for synaptic connections.
        
        Different synapse types support various neural communication mechanisms:
        - Excitatory: Promote postsynaptic neuron firing
        - Inhibitory: Suppress postsynaptic neuron firing
        - Modulatory: Release neuromodulators affecting plasticity
        - Electrical: Direct ion flow between neurons
        - GapJunction: Electrical coupling through channels
        )pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage classification for brain maturation.
        
        The brain progresses through distinct developmental phases affecting
        plasticity rates and learning capabilities:
        - Initial: High plasticity, basic learning
        - CriticalPeriod: Enhanced sensory learning window
        - Maturation: Refining connections, reduced plasticity
        - Adult: Stable adult brain with limited changes
        - Aging: Gradual decline in plasticity and function
        )pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Neuron firing state classification.
        
        Neurons operate in different firing states representing their current
        activity level and readiness to spike:
        - Resting: No active processing
        - Active: Receiving excitatory input
        - Refractory: Just fired, cannot fire immediately
        - Inhibited: Suppressed by inhibitory inputs
        )pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Motor action type classification.
        
        Available actions for agent behavior selection:
        - MoveForward, MoveBackward, MoveLeft, MoveRight: Basic locomotion
        - TurnLeft, TurnRight: Rotational movement
        - Look, LookUp, LookDown: Gaze control
        - Interact, Eat, Drink: Environmental interaction
        - Rest, Wait: State changes
        - Custom: User-defined action
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

    // MotorCommand enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command classification.
        
        Basic motor commands for direct neural control:
        - MoveForward, MoveBackward, TurnLeft, TurnRight: Basic movement
        - LookLeft, LookRight: Gaze control
        - Interact: Environmental interaction
        - Wait: No movement
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

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Classification of objects in the simulation world.
        
        Different types of interactive objects in the environment:
        - Empty: No object (background)
        - Resource: Valuable item (food, water)
        - Hazard: Dangerous object (fire, predator)
        - Wall: Physical barrier
        - Marker: Reference point or landmark
        )pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Config class with enhanced Python interface
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration manager for NLM brain systems.
        
        The Config class provides a flexible key-value storage system for
        brain parameters, supporting serialization and validation.
        
        Examples:
            >>> config = nlm.create_default_config()
            >>> config.set("neuron_count", 2000)
            >>> brain = nlm.create_brain(config)
            >>> brain.initialize()
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file.
             
             Args:
                 filepath: Path to JSON configuration file
                 
             Raises:
                 ConfigError: If file cannot be read or parsed
                 NLMError: For other I/O errors
            )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           R"pbdoc(Load configuration from command line arguments.
           
           Args:
               argc: Argument count
               argv: Argument vector
               
           Returns:
               True if successful, False if configuration error
               
           Raises:
               ConfigError: If arguments cannot be parsed
           )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file.
             
             Args:
                 filepath: Output path for JSON configuration
                 
             Raises:
                 ConfigError: If file cannot be written
                 NLMError: For other I/O errors
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists.
             
             Args:
                 key: Configuration key to check
                 
             Returns:
                 True if key exists, False otherwise
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(Get all configuration keys.
             
             Returns:
                 List of all configuration keys
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries.
             
             This removes all configuration settings, returning to default state.
             )pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary string of the configuration.
             
             Returns:
                 Human-readable summary of configuration settings
             )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("get", [](const Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw ConfigError("Configuration key not found: '" + key + "'");
            }
            return self.get<std::string>(key);
        }, py::arg("key"),
             R"pbdoc(Generic getter for configuration values.
             
             Args:
                 key: Configuration key
                 
             Returns:
                 Configuration value as string
                 
             Raises:
                 ConfigError: If key doesn't exist or type conversion fails
            )pbdoc")
        .def("getInt", [](const Config& self, const std::string& key, int default_val) {
            return py_utils::safeGetConfig(&self, key, default_val);
        }, py::arg("key"), py::arg("default_val") = 0,
           R"pbdoc(Get integer configuration value with validation.
           
           Args:
               key: Configuration key
               default_val: Default value if key doesn't exist
               
           Returns:
               Integer configuration value
               
           Raises:
               ConfigError: If key exists but type is wrong
           )pbdoc")
        .def("getFloat", [](const Config& self, const std::string& key, float default_val) {
            return py_utils::safeGetConfig(&self, key, default_val);
        }, py::arg("key"), py::arg("default_val") = 0.0f,
           R"pbdoc(Get float configuration value with validation.
           
           Args:
               key: Configuration key
               default_val: Default value if key doesn't exist
               
           Returns:
               Float configuration value
               
           Raises:
               ConfigError: If key exists but type is wrong
           )pbdoc")
        .def("getBool", [](const Config& self, const std::string& key, bool default_val) {
            if (!self.has(key)) {
                return default_val;
            }
            try {
                return self.get<bool>(key);
            } catch (const std::exception& e) {
                throw ConfigError("Failed to get boolean config key '" + key + "': " + e.what());
            }
        }, py::arg("key"), py::arg("default_val") = false,
           R"pbdoc(Get boolean configuration value.
           
           Args:
               key: Configuration key
               default_val: Default value if key doesn't exist
               
           Returns:
               Boolean configuration value
               
           Raises:
               ConfigError: If key exists but type is wrong
           )pbdoc")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           R"pbdoc(Set configuration value as string.
           
           Args:
               key: Configuration key
               value: String value
           )pbdoc")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           R"pbdoc(Set configuration integer value.
           
           Args:
               key: Configuration key
               value: Integer value
           )pbdoc")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           R"pbdoc(Set configuration float value.
           
           Args:
               key: Configuration key
               value: Float value
           )pbdoc")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           R"pbdoc(Set configuration boolean value.
           
           Args:
               key: Configuration key
               value: Boolean value
           )pbdoc");

    // SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input in the neural network.
        
        SensoryInput provides the interface for external and internal
        information processing, including vision, audio, and internal signals.
        
        Subclasses include Vision, Audio, and InternalSignals.
        )pbdoc")
        .def("getType", &SensoryInput::getType,
             R"pbdoc(Get the type of sensory input.
             
             Returns:
                 NeuronType indicating the input type
             )pbdoc")
        .def("getData", &SensoryInput::getData,
             R"pbdoc(Get the raw data as a vector.
             
             Returns:
                 Vector of float values representing the sensory input
             )pbdoc")
        .def("getDimensions", &SensoryInput::getDimensions,
             R"pbdoc(Get the dimensionality of the input.
             
             Returns:
                 Number of dimensions in the input data
             )pbdoc")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             R"pbdoc(Get the timestamp of this sensory input.
             
             Returns:
                 Simulation timestamp when input was received
             )pbdoc")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             R"pbdoc(Set the timestamp of this sensory input.
             
             Args:
                 timestamp: New timestamp value
             )pbdoc")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput type=" + std::to_string(static_cast<int>(input.getType())) + ">";
        });

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Visual sensory input for the neural network.
        
        Vision represents 2D visual information with width, height, and color channels,
        supporting image-like input for scene processing.
        
        Examples:
            >>> vision = nlm.Vision(64, 64, 3)
            >>> vision.setData(data_vector)
            >>> width = vision.getWidth()
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             R"pbdoc(Construct a Vision input.
             
             Args:
                 width: Image width in pixels
                 height: Image height in pixels
                 channels: Number of color channels (default: 3 for RGB)
                 
             Raises:
                 ValueError: If dimensions are invalid
            )pbdoc")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            py_utils::safeGetSensoryData(self, 1000000); // 1M pixel limit
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(Set vision data with validation.
             
             Args:
                 data: Vector of float values (RGB for each pixel)
                 
             Raises:
                 ValueError: If data size doesn't match dimensions
                 MemoryError: If data exceeds size limits
            )pbdoc")
        .def("getWidth", &Vision::getWidth,
             R"pbdoc(Get image width.
             
             Returns:
                 Width in pixels
             )pbdoc")
        .def("getHeight", &Vision::getHeight,
             R"pbdoc(Get image height.
             
             Returns:
                 Height in pixels
             )pbdoc")
        .def("getChannels", &Vision::getChannels,
             R"pbdoc(Get number of color channels.
             
             Returns:
                 Number of channels (typically 3 for RGB)
             )pbdoc")
        .def("getDataPtr", [](const Vision& self) {
            // Return pointer for direct access (use with caution!)
            auto data = self.getData();
            return py::memoryview(py::buffer(data));
        }, R"pbdoc(Get direct pointer to vision data.
        
        Warning:
            This returns a memoryview for performance. Be careful not to
            modify the underlying data directly.
            
        Returns:
            Memory view of vision data
        )pbdoc")
        .def("clone", [](const Vision& self) {
            return Vision(self);
        }, R"pbdoc(Create a copy of this vision input.
        
        Returns:
            New Vision instance with same data
        )pbdoc");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input for the neural network.
        
        Audio represents sound data with sample rate and duration,
        supporting auditory information processing.
        
        Examples:
            >>> audio = nlm.Audio(44100, 4410)  # 1 second of audio
            >>> audio.setData(sound_samples)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             R"pbdoc(Construct an Audio input.
             
             Args:
                 sampleRate: Sampling rate in Hz
                 numSamples: Number of audio samples
                 
             Raises:
                 ValueError: If parameters are invalid
            )pbdoc")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            py_utils::safeGetSensoryData(self, 1000000); // 1M samples limit
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(Set audio data with validation.
             
             Args:
                 data: Vector of audio samples
                 
             Raises:
                 MemoryError: If data exceeds size limits
            )pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             R"pbdoc(Set audio sample rate.
             
             Args:
                 sampleRate: New sample rate in Hz
            )pbdoc")
        .def("getSampleRate", &Audio::getSampleRate,
             R"pbdoc(Get audio sample rate.
             
             Returns:
                 Sample rate in Hz
             )pbdoc")
        .def("getNumSamples", &Audio::getNumSamples,
             R"pbdoc(Get number of audio samples.
             
             Returns:
                 Number of samples in the audio buffer
             )pbdoc")
        .def("getDuration", [](const Audio& self) {
            return static_cast<double>(self.getNumSamples()) / self.getSampleRate();
        }, R"pbdoc(Get audio duration in seconds.
        
        Returns:
            Duration in seconds
        )pbdoc");

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal state signals for the neural network.
        
        InternalSignals represent the agent's internal state, including
        hunger, thirst, energy, and other physiological variables.
        
        Examples:
            >>> signals = nlm.InternalSignals()
            >>> signals.addSignal(0.5)  # Add internal signal value
            >>> energy = signals.getData()[0]
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             R"pbdoc(Add an internal signal value.
             
             Args:
                 value: Signal value to add
                 
             Raises:
                 ValueError: If value is outside [-1, 1] range
            )pbdoc")
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getData().size();
        }, R"pbdoc(Get number of internal signals.
        
        Returns:
            Number of active internal signals
        )pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals,
             R"pbdoc(Clear all internal signals.
             
             This resets the internal state to neutral values.
             )pbdoc")
        .def("getDataPtr", [](const InternalSignals& self) {
            return py::memoryview(py::buffer(self.getData()));
        }, R"pbdoc(Get direct pointer to internal signals.
        
        Warning:
            This returns a memoryview for performance. Be careful not to
            modify the underlying data directly.
            
        Returns:
            Memory view of internal signal data
        )pbdoc");

    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output in the neural network.
        
        Action encapsulates the agent's behavioral choice, including the
        type of action and associated parameters for execution.
        
        Examples:
            >>> move_action = nlm.Action(nlm.ActionType.MoveForward)
            >>> interact_action = nlm.Action(nlm.ActionType.Interact, [0.5, 0.3])
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             R"pbdoc(Construct an action with just type.
             
             Args:
                 type: Action type (MoveForward, Interact, etc.)
            )pbdoc")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             R"pbdoc(Construct a complex action with type and parameters.
             
             Args:
                 type: Action type
                 parameters: List of action parameters
                 
             Raises:
                 ValueError: If parameters exceed 10 values
            )pbdoc")
        .def("getType", &Action::getType,
             R"pbdoc(Get the action type.
             
             Returns:
                 ActionType enum value
             )pbdoc")
        .def("setType", &Action::setType, py::arg("type"),
             R"pbdoc(Set the action type.
             
             Args:
                 type: New action type
                 
             Raises:
                 ValueError: If action type is invalid
            )pbdoc")
        .def("getParameters", &Action::getParameters,
             R"pbdoc(Get action parameters.
             
             Returns:
                 Vector of float parameters
             )pbdoc")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             R"pbdoc(Set action parameters with validation.
             
             Args:
                 params: New parameter vector
                 
             Raises:
                 ValueError: If parameter count exceeds 10 or contains NaN/inf
            )pbdoc")
        .def("getName", &Action::getName,
             R"pbdoc(Get human-readable action name.
             
             Returns:
                 String name of the action type
             )pbdoc")
        .def("getDescription", [](const Action& self) {
            std::string name = self.getName();
            const auto& params = self.getParameters();
            std::string result = name;
            if (!params.empty()) {
                result += "(";
                for (size_t i = 0; i < params.size(); ++i) {
                    if (i > 0) result += ", ";
                    result += std::to_string(params[i]);
                }
                result += ")";
            }
            return result;
        }, R"pbdoc(Get detailed action description.
        
        Returns:
            Human-readable description including parameters
        )pbdoc")
        .def("clone", &Action::clone,
             R"pbdoc(Create a copy of this action.
             
             Returns:
                 New Action instance with same type and parameters
            )pbdoc")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        });

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        Physical object in the simulation world.
        
        WorldObject represents interactive elements in the environment,
        including resources, hazards, walls, and landmarks.
        
        Examples:
            >>> food = nlm.WorldObject(1.0, 2.0, nlm.WorldObjectType.Resource, 10.0)
            >>> wall = nlm.WorldObject(5.0, 5.0, nlm.WorldObjectType.Wall)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             R"pbdoc(Construct a world object.
             
             Args:
                 x: X coordinate
                 y: Y coordinate
                 type: Object type (Resource, Hazard, Wall, etc.)
                 value: Object value (e.g., food energy)
                 radius: Interaction radius
                 
             Raises:
                 ValueError: If coordinates or radius are invalid
            )pbdoc")
        .def_readwrite("x", &WorldObject::x,
                      R"pbdoc(X coordinate)")
        .def_readwrite("y", &WorldObject::y,
                      R"pbdoc(Y coordinate)")
        .def_readwrite("radius", &WorldObject::radius,
                      R"pbdoc(Interaction radius)")
        .def_readwrite("type", &WorldObject::type,
                      R"pbdoc(Object type (WorldObjectType enum))")
        .def_readwrite("value", &WorldObject::value,
                      R"pbdoc(Object value (e.g., food energy, hazard danger))")
        .def_readwrite("active", &WorldObject::active,
                      R"pbdoc(Whether object is active in simulation)")
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
           R"pbdoc(Calculate distance to a point.
           
           Args:
               x: X coordinate
               y: Y coordinate
               
           Returns:
               Distance to the point
           )pbdoc")
        .def("isCollidingWith", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            return distance < (self.radius + other.radius);
        }, py::arg("other"),
           R"pbdoc(Check collision with another object.
           
           Args:
               other: Another WorldObject to check against
               
           Returns:
               True if objects overlap
           )pbdoc")
        .def("__repr__", [](const WorldObject& obj) {
            std::string type_name;
            switch (obj.type) {
                case WorldObjectType::Empty: type_name = "Empty"; break;
                case WorldObjectType::Resource: type_name = "Resource"; break;
                case WorldObjectType::Hazard: type_name = "Hazard"; break;
                case WorldObjectType::Wall: type_name = "Wall"; break;
                case WorldObjectType::Marker: type_name = "Marker"; break;
                default: type_name = "Unknown";
            }
            return "<WorldObject " + type_name + " at (" + std::to_string(obj.x) + ", " + 
                   std::to_string(obj.y) + ") radius=" + std::to_string(obj.radius) + ">";
        });

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state and physics in the simulation world.
        
        AgentBody tracks the agent's position, orientation, velocity, and
        physiological state, providing the interface for motor control.
        
        Examples:
            >>> body = nlm.AgentBody()
            >>> body.x = 1.0
            >>> body.y = 2.0
            >>> body.orientation = 45.0
        )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, R"pbdoc(X position)")
        .def_readwrite("y", &AgentBody::y, R"pbdoc(Y position)")
        .def_readwrite("orientation", &AgentBody::orientation, R"pbdoc(Angle in degrees)")
        .def_readwrite("velocityX", &AgentBody::velocityX, R"pbdoc(X velocity)")
        .def_readwrite("velocityY", &AgentBody::velocityY, R"pbdoc(Y velocity)")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, R"pbdoc(Rotational velocity)")
        .def_readwrite("energy", &AgentBody::energy, R"pbdoc(Energy level (0-100)")
        .def_readwrite("health", &AgentBody::health, R"pbdoc(Health level (0-100)")
        .def_readwrite("age", &AgentBody::age, R"pbdoc(Age in simulation steps)")
        .def_readwrite("isMoving", &AgentBody::isMoving, R"pbdoc(Whether agent is moving)")
        .def_readwrite("isTurning", &AgentBody::isTurning, R"pbdoc(Whether agent is turning)")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, R"pbdoc(Time since last action)")
        .def("getPosition", [](const AgentBody& self) {
            return py::make_tuple(self.x, self.y);
        }, R"pbdoc(Get position as (x, y) tuple.
        
        Returns:
            Tuple of (x, y) coordinates
        )pbdoc")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"),
           R"pbdoc(Set position.
           
           Args:
               x: X coordinate
               y: Y coordinate
           )pbdoc")
        .def("getVelocity", [](const AgentBody& self) {
            return py::make_tuple(self.velocityX, self.velocityY);
        }, R"pbdoc(Get velocity as (vx, vy) tuple.
        
        Returns:
            Tuple of (vx, vy) velocities
        )pbdoc")
        .def("setVelocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
            self.isMoving = (vx != 0.0f || vy != 0.0f);
        }, py::arg("vx"), py::arg("vy"),
           R"pbdoc(Set velocity.
           
           Args:
               vx: X velocity
               vy: Y velocity
           )pbdoc")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, R"pbdoc(Get speed magnitude.
        
        Returns:
            Speed in simulation units per step
        )pbdoc")
        .def("getHeadingAngle", [](const AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX) * 180.0f / M_PI;
        }, R"pbdoc(Get heading angle in degrees.
        
        Returns:
            Angle in degrees (0-360)
        )pbdoc")
        .def("reset", &AgentBody::reset,
             R"pbdoc(Reset body to initial state.
             
             Resets position, velocity, and other state variables to defaults.
             )pbdoc")
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody at (" + std::to_string(body.x) + ", " + std::to_string(body.y) +
                   ") orientation=" + std::to_string(body.orientation) + ">";
        });

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result from executing an action in the world.
        
        ActionResult captures the outcome of an agent's action, including
        reward, success status, and descriptive message.
        
        Examples:
            >>> result = nlm.ActionResult(10.0, True, "Found food")
            >>> print(f"Reward: {result.reward}")
            >>> print(f"Success: {result.success}")
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             R"pbdoc(Construct an action result.
             
             Args:
                 reward: Reward value (positive for good, negative for bad)
                 success: Whether action was successful
                 message: Descriptive message about result
                 
             Raises:
                 ValueError: If reward is infinite or NaN
            )pbdoc")
        .def_readwrite("reward", &ActionResult::reward,
                      R"pbdoc(Reward value (can be positive or negative)")
        .def_readwrite("success", &ActionResult::success,
                      R"pbdoc(Whether action was successful)")
        .def_readwrite("message", &ActionResult::message,
                      R"pbdoc(Descriptive message about the result)")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult reward=" + std::to_string(result.reward) +
                   " success=" + std::to_string(result.success) + 
                   " message='" + result.message + "'>";
        });

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Combined sensory input from all modalities.
        
        SensoryPercept aggregates vision, touch, internal signals, and
        proprioception into a unified perception for the agent.
        
        Examples:
            >>> percept = nlm.SensoryPercept()
            >>> percept.setVision(vision_data)
            >>> percept.setTouch(touch_data)
            >>> features = percept.getAllSignals()
        )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             R"pbdoc(Get vision data.
             
             Returns:
                 Vision data as vector of floats
             )pbdoc")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             R"pbdoc(Set vision data.
             
             Args:
                 vision: Vision data vector
                 
             Raises:
                 ValueError: If data size doesn't match expected dimensions
            )pbdoc")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             R"pbdoc(Get vision width in pixels.
             
             Returns:
                 Image width
             )pbdoc")
        .def("getVisionHeight", &SensoryPercept::getHeight,
             R"pbdoc(Get vision height in pixels.
             
             Returns:
                 Image height
             )pbdoc")
        .def("getTouch", &SensoryPercept::getTouch,
             R"pbdoc(Get touch data.
             
             Returns:
                 Touch data as vector of floats
             )pbdoc")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             R"pbdoc(Set touch data.
             
             Args:
                 touch: Touch data vector
            )pbdoc")
        .def("getInternal", &SensoryPercept::getInternal,
             R"pbdoc(Get internal signals.
             
             Returns:
                 Internal signals as vector of floats
             )pbdoc")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             R"pbdoc(Set internal signals.
             
             Args:
                 internal: Internal signals vector
            )pbdoc")
        .def("getProprioception", &SensoryPercept::getProprioception,
             R"pbdoc(Get proprioception data.
             
             Returns:
                 Proprioception data as vector of floats
             )pbdoc")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             R"pbdoc(Set proprioception data.
             
             Args:
                 proprioception: Proprioception data vector
            )pbdoc")
        .def("getAudio", &SensoryPercept::getAudio,
             R"pbdoc(Get audio data.
             
             Returns:
                 Audio data as vector of floats
             )pbdoc")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             R"pbdoc(Set audio data.
             
             Args:
                 audio: Audio data vector
            )pbdoc")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             R"pbdoc(Get all sensory signals concatenated.
             
             Returns:
                 Combined sensory data vector
             )pbdoc")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             R"pbdoc(Get percept timestamp.
             
             Returns:
                 Timestamp of this percept
             )pbdoc")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             R"pbdoc(Set percept timestamp.
             
             Args:
                 timestamp: New timestamp
             )pbdoc")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept timestamp=" + std::to_string(percept.getTimestamp()) + ">";
        });

    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D simulation world for NLM agents.
        
        SimpleWorld provides a grid-based environment for agent simulation,
        supporting object placement, movement, and interaction.
        
        Examples:
            >>> world = nlm.create_simple_world(100, 100)
            >>> world.setAgentStart(50, 50)
            >>> percept = world.getSensoryPercept()
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(Configure the world dimensions.
             
             Args:
                 width: World width in cells
                 height: World height in cells
                 visionWidth: Vision sensor width
                 visionHeight: Vision sensor height
                 
             Raises:
                 ValueError: If dimensions are invalid
            )pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(Reset world to initial state.
             
             Clears all objects and resets agent position.
             )pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(Set agent starting position.
             
             Args:
                 x: X coordinate
                 y: Y coordinate
                 
             Raises:
                 ValueError: If position is out of bounds
            )pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(Update world state.
             
             Args:
                 timestep: Time step duration
                 
             Returns:
                 True if update successful, False if error
            )pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(Apply motor command to agent.
             
             Args:
                 cmd: Motor command to execute
                 currentTime: Current simulation time
                 
             Returns:
                 ActionResult from world interaction
            )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             R"pbdoc(Get current sensory percept.
             
             Returns:
                 SensoryPercept object with all sensory data
             )pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             R"pbdoc(Get current agent body state.
             
             Returns:
                 AgentBody object with position, velocity, etc.
             )pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(Add object to world.
             
             Args:
                 obj: WorldObject to add
                 
             Raises:
                 ValueError: If object position is invalid
            )pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(Remove object at position.
             
             Args:
                 x: X coordinate
                 y: Y coordinate
                 
             Returns:
                 True if object was removed, False if not found
            )pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(Check if position is valid.
             
             Args:
                 x: X coordinate
                 y: Y coordinate
                 
             Returns:
                 True if position is valid and empty
            )pbdoc")
        .def("getWidth", &SimpleWorld::getWidth,
             R"pbdoc(Get world width.
             
             Returns:
                 World width in cells
             )pbdoc")
        .def("getHeight", &SimpleWorld::getHeight,
             R"pbdoc(Get world height.
             
             Returns:
                 World height in cells
             )pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(Get maximum energy capacity.
             
             Returns:
                 Maximum energy value
             )pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(Set maximum energy capacity.
             
             Args:
                 e: New maximum energy value
            )pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc(Get energy decay rate.
             
             Returns:
                 Energy decay rate per timestep
             )pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(Set energy decay rate.
             
             Args:
                 r: New energy decay rate
            )pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             R"pbdoc(Get current simulation time.
             
             Returns:
                 Current simulation time
             )pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(Set random seed.
             
             Args:
                 seed: Random seed value
            )pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             R"pbdoc(Get current random seed.
             
             Returns:
                 Current random seed
             )pbdoc")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld " + std::to_string(world.getWidth()) + "x" + 
                   std::to_string(world.getHeight()) + ">";
        });

    // Brain class with enhanced Python interface
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain with integrated cognitive systems.
        
        Brain implements a complete artificial brain with neural dynamics, memory,
        prediction, cognition, and neuromodulation systems. It serves as the
        core computational engine for agent intelligence.
        
        Examples:
            >>> config = nlm.create_default_config()
            >>> brain = nlm.create_brain(config)
            >>> brain.initialize()
            >>> action = brain.produce_action()
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(Construct a brain with configuration.
             
             Args:
                 config: Brain configuration
                 
             Raises:
                 BrainError: If configuration is invalid
            )pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(Initialize the brain with current configuration.
             
             Sets up neural regions, populations, and integrated systems.
             
             Returns:
                 True if initialization successful, False otherwise
                 
             Raises:
                 BrainError: If initialization fails
            )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(Perform a simulation step.
             
             Args:
                 currentStep: Current simulation step number
                 
             Raises:
                 BrainError: If step execution fails
            )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(Perform a simulation step with explicit time.
             
             Args:
                 currentStep: Current simulation step number
                 currentTime: Current simulation time
                 
             Raises:
                 BrainError: If step execution fails
            )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             R"pbdoc(Inject sensory input into the brain.
             
             Args:
                 input: Sensory input data
                 
             Raises:
                 BrainError: If input processing fails
            )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             R"pbdoc(Inject current into a specific neuron.
             
             Args:
                 neuron: Target neuron identifier
                 current: Current value to inject
                 
             Raises:
                 BrainError: If neuron not found or current invalid
            )pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(Inject current into all neurons of a specific type.
             
             Args:
                 type: Neuron type (Excitatory, Inhibitory, etc.)
                 current: Current value to inject
                 
             Raises:
                 BrainError: If neuron type not found or current invalid
            )pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(Generate motor action based on neural activity.
             
             Returns:
                 Action object representing selected behavior
                 
             Raises:
                 BrainError: If action selection fails
            )pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(Reset brain state to initial conditions.
             
             Clears all neural activity, memories, and internal state.
             )pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(Save brain state to file.
             
             Args:
                 filepath: Output file path
                 
             Returns:
                 True if save successful, False otherwise
                 
             Raises:
                 BrainError: If save operation fails
            )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(Load brain state from file.
             
             Args:
                 filepath: Input file path
                 
             Returns:
                 True if load successful, False otherwise
                 
             Raises:
                 BrainError: If load operation fails
            )pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(Add a new neural region.
             
             Args:
                 name: Optional region name
                 
             Returns:
                 RegionId of the newly created region
                 
             Raises:
                 BrainError: If region creation fails
            )pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             R"pbdoc(Get a neural region by ID.
             
             Args:
                 id: Region identifier
                 
             Returns:
                 NeuralRegion object or None if not found
            )pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(Get the number of neural regions.
             
             Returns:
                 Number of regions
            )pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(Get all region IDs.
             
             Returns:
                 List of all region identifiers
            )pbdoc")
        .def("getRegions", &Brain::getRegions,
             R"pbdoc(Get all neural regions.
             
             Returns:
                 List of all neural region objects
            )pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(Get total neuron count across all regions.
             
             Returns:
                 Total number of neurons
            )pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(Get total synapse count across all regions.
             
             Returns:
                 Total number of synapses
            )pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(Get count of active neurons.
             
             Returns:
                 Number of currently active neurons
            )pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(Get count of currently firing neurons.
             
             Returns:
                 Number of firing neurons this step
            )pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(Get average firing rate across all neurons.
             
             Returns:
                 Average firing rate in Hz
            )pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(Get excitation/inhibition balance ratio.
             
             Returns:
                 Ratio of total excitation to inhibition
                 (infinity if no inhibition, 0 if no excitation)
            )pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(Get total spike count.
             
             Returns:
                 Total number of spikes since initialization
            )pbdoc")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             R"pbdoc(Get count of pending spike events.
             
             Returns:
                 Number of queued spike events
            )pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.
             
             Returns:
                 Current developmental stage
            )pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             R"pbdoc(Set developmental stage.
             
             Args:
                 stage: New developmental stage
                 
             Raises:
                 ValueError: If stage is invalid
            )pbdoc")
        .def("getConfig", &Brain::getConfig,
             R"pbdoc(Get brain configuration.
             
             Returns:
                 Configuration object
            )pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(Log brain status and statistics.
             
             Outputs comprehensive information about brain state,
             including neuron counts, memory status, and neuromodulation levels.
             )pbdoc")
        // Memory system access
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             R"pbdoc(Get working memory system.
             
             Returns:
                 WorkingMemory object or None if not available
            )pbdoc")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             R"pbdoc(Get episodic memory system.
             
             Returns:
                 EpisodicMemory object or None if not available
            )pbdoc")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             R"pbdoc(Get associative memory system.
             
             Returns:
                 AssociativeMemory object or None if not available
            )pbdoc")
        // Prediction system access
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             R"pbdoc(Get prediction system.
             
             Returns:
                 PredictionSystem object or None if not available
            )pbdoc")
        // Cognition system access
        .def("getPlanner", &Brain::getPlanner,
             R"pbdoc(Get neural planner system.
             
             Returns:
                 NeuralPlanner object or None if not available
            )pbdoc")
        .def("getConceptFormation", &Brain::getConceptFormation,
             R"pbdoc(Get concept formation system.
             
             Returns:
                 ConceptFormation object or None if not available
            )pbdoc")
        .def("getAttention", &Brain::getAttention,
             R"pbdoc(Get attentional selection system.
             
             Returns:
                 AttentionalSelection object or None if not available
            )pbdoc")
        // Development system access
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             R"pbdoc(Get development system.
             
             Returns:
                 DevelopmentSystem object or None if not available
            )pbdoc")
        // Neuromodulation system access
        .def("getDopamine", &Brain::getDopamine,
             R"pbdoc(Get dopamine neuromodulation system.
             
             Returns:
                 Dopamine object or None if not available
            )pbdoc")
        .def("getCuriosity", &Brain::getCuriosity,
             R"pbdoc(Get curiosity neuromodulation system.
             
             Returns:
                 Curiosity object or None if not available
            )pbdoc")
        .def("getNovelty", &Brain::getNovelty,
             R"pbdoc(Get novelty detection system.
             
             Returns:
                 Novelty object or None if not available
            )pbdoc")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             R"pbdoc(Get prediction error system.
             
             Returns:
                 PredictionError object or None if not available
            )pbdoc")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             R"pbdoc(Get random number generator.
             
             Returns:
                 RandomGenerator object
            )pbdoc")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             R"pbdoc(Get spike system.
             
             Returns:
                 SpikeSystem object
            )pbdoc")
        .def("getSTDP", &Brain::getSTDP,
             R"pbdoc(Get STDP plasticity system.
             
             Returns:
                 STDP object
            )pbdoc")
        .def("getHebbian", &Brain::getHebbian,
             R"pbdoc(Get Hebbian plasticity system.
             
             Returns:
                 Hebbian object
            )pbdoc")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             R"pbdoc(Get structural plasticity system.
             
             Returns:
                 StructuralPlasticity object
            )pbdoc")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain regions=" + std::to_string(brain.getRegionCount()) +
                   " neurons=" + std::to_string(brain.getTotalNeuronCount()) + ">";
        });

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world simulation.
        
        AgentBrain provides the high-level interface for agent behavior,
        handling sensory processing, motor control, and neuromodulation.
        
        Examples:
            >>> brain = nlm.create_brain(config)
            >>> agent = nlm.create_agent_brain(brain)
            >>> agent.initialize(world)
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(Construct an agent brain with underlying brain.
             
             Args:
                 brain: Underlying brain system
            )pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(Initialize agent with world interface.
             
             Args:
                 world: World object for interaction
                 
             Raises:
                 BrainError: If initialization fails
            )pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(Get expected sensory input size.
             
             Returns:
                 Number of expected sensory input values
            )pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(Get expected motor output size.
             
             Returns:
                 Number of possible motor outputs
            )pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             R"pbdoc(Process sensory percept and inject into brain.
             
             Args:
                 percept: Sensory percept containing all modalities
                 
             Raises:
                 BrainError: If sensory processing fails
            )pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(Decode brain motor activity into motor command.
             
             Returns:
                 MotorCommand selected by agent
                 
             Raises:
                 BrainError: If motor decoding fails
            )pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(Apply reward-based neuromodulation.
             
             Args:
                 reward: Actual reward received
                 predictedReward: Expected reward
                 
             Raises:
                 BrainError: If reward modulation fails
            )pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
             R"pbdoc(Update development system.
             
             Args:
                 timestep: Time step duration
                 
             Raises:
                 BrainError: If development update fails
            )pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.
             
             Returns:
                 Current developmental stage
            )pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(Get current neuromodulation level.
             
             Returns:
                 Neuromodulation level (0-1)
            )pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(Get curiosity level.
             
             Returns:
                 Curiosity level (0-1)
            )pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(Get novelty detection level.
             
             Returns:
                 Novelty level (0-1)
            )pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(Get prediction error.
             
             Returns:
                 Prediction error value
            )pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(Reset agent for new episode.
             
             Clears all internal state and neuromodulation levels.
             )pbdoc")
        .def("getBrain", &AgentBrain::getBrain,
             R"pbdoc(Get the underlying brain.
             
             Returns:
                 Brain object
            )pbdoc")
        // Enable/disable system flags
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
             R"pbdoc(Enable or disable reward modulation.
             
             Args:
                 enable: Whether to enable reward modulation
            )pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
             R"pbdoc(Enable or disable structural plasticity.
             
             Args:
                 enable: Whether to enable structural plasticity
            )pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"),
             R"pbdoc(Enable or disable development.
             
             Args:
                 enable: Whether to enable development
            )pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"),
             R"pbdoc(Enable or disable curiosity-driven exploration.
             
             Args:
                 enable: Whether to enable curiosity
            )pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc(Check if reward modulation is enabled.
             
             Returns:
                 True if reward modulation is enabled
            )pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(Check if structural plasticity is enabled.
             
             Returns:
                 True if structural plasticity is enabled
            )pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc(Check if development is enabled.
             
             Returns:
                 True if development is enabled
            )pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc(Check if curiosity is enabled.
             
             Returns:
                 True if curiosity is enabled
            )pbdoc")
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain>";
        });

    // Factory functions for convenience
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return py_utils::createValidatedConfig();
    }, R"pbdoc(Create a default configuration with safe values.
    
    Returns:
        Configured Config object with validated defaults
    )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create a new brain with configuration.
    
    Args:
        config: Brain configuration
        
    Returns:
        Brain object
        
    Raises:
        BrainError: If configuration is invalid
    )pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create a new simple world.
    
    Returns:
        SimpleWorld object
    )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create a new agent brain interface.
    
    Args:
        brain: Underlying brain system
        
    Returns:
        AgentBrain object
    )pbdoc");

    // Module constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Module-level utility functions
    m.def("validate_config", [](const std::shared_ptr<Config>& config) {
        bool valid = true;
        std::vector<std::string> errors;
        
        try {
            if (config->has("neuron_count")) {
                size_t neurons = config->get<size_t>("neuron_count");
                if (neurons == 0) {
                    errors.push_back("neuron_count must be > 0");
                    valid = false;
                }
            }
            
            if (config->has("region_count")) {
                size_t regions = config->get<size_t>("region_count");
                if (regions == 0) {
                    errors.push_back("region_count must be > 0");
                    valid = false;
                }
            }
            
            if (config->has("connection_probability")) {
                float prob = config->get<float>("connection_probability");
                if (prob < 0.0f || prob > 1.0f) {
                    errors.push_back("connection_probability must be in [0, 1]");
                    valid = false;
                }
            }
        } catch (const std::exception& e) {
            errors.push_back(std::string("Validation exception: ") + e.what());
            valid = false;
        }
        
        if (valid) {
            return py::make_tuple(true, std::vector<std::string>());
        } else {
            return py::make_tuple(false, errors);
        }
    }, py::arg("config"), R"pbdoc(Validate configuration parameters.
    
    Args:
        config: Configuration to validate
        
    Returns:
        Tuple of (is_valid, error_messages)
    )pbdoc");

    m.def("getBrainVersion", []() {
        return "NLM Phase 6 - Python Bindings v0.1.0";
    }, R"pbdoc(Get the version of the NLM Python bindings.
    
    Returns:
        Version string
    )pbdoc");

    m.def("testIntegration", []() {
        auto config = py_utils::createValidatedConfig();
        config->set("neuron_count", 100);
        config->set("region_count", 2);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw IntegrationError("Brain initialization failed");
        }
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(50, 50, 16, 16);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Run a few steps
        for (int i = 0; i < 10; ++i) {
            brain->step(i);
        }
        
        auto action = brain->produceAction();
        
        return py::make_tuple(
            true,
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            action->getName()
        );
    }, R"pbdoc(Test basic integration between systems.
    
    Returns:
        Tuple of (success, neuron_count, synapse_count, action_name)
        
    Raises:
        IntegrationError: If integration test fails
    )pbdoc");
}

} // namespace nlm

// Backward compatibility - maintain old exception types
PYBIND11_MODULE(pynlm_legacy, m) {
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    // Import all symbols from pynlm
    auto pynlm_module = py::module::import("pynlm");
    for (const auto& name : pynlm_module.attr("__dict__"__)) {
        if (PyDict_CheckExact(pynlm_module.attr("__dict__").ptr()) && 
            PyDict_Contains(pynlm_module.attr("__dict__").ptr(), name.ptr()) != 0) {
            m.add_object(name, pynlm_module.attr(name));
        }
    }
}