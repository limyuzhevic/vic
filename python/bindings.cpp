#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include <pybind11/chrono.h>
#include <pybind11/json.h>
#include <memory>
#include <string>
#include <vector>
#include <map>

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

// Type alias for clarity
using Step = SimulationStep;
using TimeStamp = Timestamp;

// Helper functions for enhanced API
namespace api_helpers {
    
    // Safe neuron ID retrieval with validation
    NeuronId getValidNeuronId(py::object neuron_id_obj) {
        if (py::isinstance<NeuronId>(neuron_id_obj)) {
            return neuron_id_obj.cast<NeuronId>();
        }
        // Handle string representation or integer
        auto str_value = neuron_id_obj.cast<std::string>();
        if (str_value.rfind("<NeuronId:", 0) == 0 && str_value.back() == '>') {
            try {
                size_t pos = str_value.find(": ") + 2;
                uint64_t value = std::stoull(str_value.substr(pos));
                return NeuronId(value);
            } catch (...) {
                throw py::value_error("Invalid NeuronId format: " + str_value);
            }
        }
        throw py::value_error("NeuronId must be NeuronId object, not " + str_value);
    }
    
    // Safe region ID retrieval with validation
    RegionId getValidRegionId(py::object region_id_obj) {
        if (py::isinstance<RegionId>(region_id_obj)) {
            return region_id_obj.cast<RegionId>();
        }
        // Handle string representation or integer
        auto str_value = region_id_obj.cast<std::string>();
        if (str_value.rfind("<RegionId:", 0) == 0 && str_value.back() == '>') {
            try {
                size_t pos = str_value.find(": ") + 2;
                uint64_t value = std::stoull(str_value.substr(pos));
                return RegionId(value);
            } catch (...) {
                throw py::value_error("Invalid RegionId format: " + str_value);
            }
        }
        throw py::value_error("RegionId must be RegionId object, not " + str_value);
    }
    
    // Convert configuration dictionary to Config object
        // Handle common configuration parameters
        for (auto item : config_dict) {
            std::string key = py::cast<std::string>(item.first);
            py::object value = item.second;
            
            if (value.is(py::int_())) {
                config->set(key, value.cast<int64_t>());
            } else if (value.is(py::float_())) {
                config->set(key, value.cast<float>());
            } else if (value.is(py::str_())) {
                config->set(key, value.cast<std::string>());
            } else if (value.is(py::bool_())) {
                config->set(key, value.cast<bool>());
            } else if (value.is(py::list_())) {
                auto vec = value.cast<std::vector<float>>();
                config->set(key, vec);
            }
        }
        
        return config;
    
    // Convert Config object to dictionary
    py::dict configToDict(const std::shared_ptr<Config>& config) {
        py::dict result;
        auto keys = config->getKeys();
        
        for (const auto& key : keys) {
            // Try different types in order of preference
            if (config->has<int64_t>(key)) {
                result[key] = config->get<int64_t>(key);
            } else if (config->has<float>(key)) {
                result[key] = config->get<float>(key);
            } else if (config->has<std::string>(key)) {
                result[key] = config->get<std::string>(key);
            } else if (config->has<bool>(key)) {
                result[key] = config->get<bool>(key);
            } else if (config->has<std::vector<float>>(key)) {
                result[key] = config->get<std::vector<float>>(key);
            }
        }
        
        return result;
    }
    
    std::shared_ptr<Config> createDefaultConfig(const py::dict& overrides = py::dict()) {
        auto config = std::make_shared<Config>();
        
        // Set default values optimized for research and prototyping
        config->set("neuron_count", 1000);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("simulation_timestep", 0.001f);
        config->set("random_seed", 42ULL);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        config->set("replay_interval", 100);
        config->set("consolidation_interval", 1000);
        
        // Apply user overrides
        for (auto item : overrides.items()) {
            std::string key = py::cast<std::string>(item.first);
            py::object value = item.second;
            
            if (value.is(py::int_())) {
                config->set(key, value.cast<int64_t>());
            } else if (value.is(py::float_())) {
                config->set(key, value.cast<float>());
            } else if (value.is(py::str_())) {
                config->set(key, value.cast<std::string>());
            } else if (value.is(py::bool_())) {
                config->set(key, value.cast<bool>());
            }
        }
        
        return config;
    }
    
    // Validate configuration for errors
    void validateConfig(const std::shared_ptr<Config>& config) {
        std::vector<std::string> errors;
        
        if (!config->has<size_t>("neuron_count")) {
            errors.push_back("Missing required parameter: neuron_count");
        } else {
            size_t neuron_count = config->get<size_t>("neuron_count");
            if (neuron_count == 0) {
                errors.push_back("neuron_count must be greater than 0");
            }
        }
        
        if (!config->has<size_t>("region_count")) {
            errors.push_back("Missing required parameter: region_count");
        } else {
            size_t region_count = config->get<size_t>("region_count");
            if (region_count == 0) {
                errors.push_back("region_count must be greater than 0");
            }
        }
        
        if (config->has<float>("connection_probability")) {
            float prob = config->get<float>("connection_probability");
            if (prob < 0.0f || prob > 1.0f) {
                errors.push_back("connection_probability must be between 0.0 and 1.0");
            }
        }
        
        if (errors.size() > 0) {
            std::string error_msg = "Configuration validation errors:\n";
            for (const auto& error : errors) {
                error_msg += "  - " + error + "\n";
            }
            throw py::value_error(error_msg);
        }
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ================================================

        Advanced Python bindings for the NLM C++ neural simulation framework.
        
        This module provides a comprehensive, user-friendly API for creating,
        configuring, and simulating artificial brains with integrated memory,
        prediction, cognition, and neuromodulation systems.
        
        Key Features:
        - Full integration with NLM's Phase 6 integrated brain architecture
        - Comprehensive configuration management
        - Advanced error handling and validation
        - Convenience functions for rapid prototyping
        - Support for both simple and complex simulation setups
        
        Examples:
        ```python
        import pynlm
        
        # Quick start with default configuration
        brain = pynlm.create_brain()
        brain.initialize()
        
        # Advanced setup with custom configuration
        config = pynlm.Config()
        config.set("neuron_count", 5000)
        config.set("region_count", 3)
        config.set("random_seed", 12345)
        brain = pynlm.Brain(config)
        
        # Process sensory input and step through simulation
        world = pynlm.SimpleWorld()
        world.configure(64, 64, 16, 16)
        agent = pynlm.AgentBrain(brain, world)
        
        for step in range(100):
            brain.step(step)
        ```
        
        For more examples, see the documentation at:
        https://nlm.readthedocs.io
    )pbdoc";
    
    // Enable error handling with detailed messages
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
    
    // Forward declare helper functions for type annotations
    py::class_<NeuronId>(m, "NeuronId")
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
        .doc() = R"pbdoc(Unique identifier for a neuron.
        
        NeuronId provides a strongly-typed identifier for neurons, ensuring type safety
        and preventing errors when working with large numbers of neurons.
        "pbdoc");
    
    // SynapseId with improved documentation
    py::class_<SynapseId>(m, "SynapseId")
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
        .doc() = R"pbdoc(Unique identifier for a synapse.
        
        SynapseId provides a strongly-typed identifier for synapses, ensuring type
        safety in synaptic operations and plasticity computations.
        "pbdoc");
    
    // RegionId with improved documentation
    py::class_<RegionId>(m, "RegionId")
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
        .doc() = R"pbdoc(Unique identifier for a brain region.
        
        RegionId provides a strongly-typed identifier for neural regions, allowing
        for organized management of different brain areas with specialized functions.
        "pbdoc");
    
    // PopulationId with improved documentation
    py::class_<PopulationId>(m, "PopulationId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .doc() = R"pbdoc(Unique identifier for a neuron population.
        
        PopulationId provides a strongly-typed identifier for neuron populations,
        which are groups of neurons with similar characteristics or functions.
        "pbdoc");
    
    // NeuronType enum with improved documentation
    py::enum_<NeuronType>(m, "NeuronType", py::arithmetic(),
        R"pbdoc(Neuron type enumeration defining different neuron classifications.)pbdoc")
        .value("Excitatory", NeuronType::Excitatory,
            R"pbdoc(Excitatory neurons that increase the likelihood of postsynaptic neuron firing.)pbdoc")
        .value("Inhibitory", NeuronType::Inhibitory,
            R"pbdoc(Inhibitory neurons that decrease the likelihood of postsynaptic neuron firing.)pbdoc")
        .value("Modulatory", NeuronType::Modulatory,
            R"pbdoc(Modulatory neurons that influence synaptic plasticity and network state.)pbdoc")
        .value("Sensory", NeuronType::Sensory,
            R"pbdoc(Sensory neurons that process external sensory inputs.)pbdoc")
        .value("Motor", NeuronType::Motor,
            R"pbdoc(Motor neurons that generate action outputs.)pbdoc")
        .value("Internal", NeuronType::Internal,
            R"pbdoc(Internal neurons that maintain intrinsic brain state and computations.)pbdoc")
        .export_values();
    
    // SynapseType enum with improved documentation
    py::enum_<SynapseType>(m, "SynapseType", py::arithmetic(),
        R"pbdoc(Synapse type enumeration defining different synaptic connection types.)pbdoc")
        .value("Excitatory", SynapseType::Excitatory,
            R"pbdoc(Excitatory synapses that increase postsynaptic neuron activity.)pbdoc")
        .value("Inhibitory", SynapseType::Inhibitory,
            R"pbdoc(Inhibitory synapses that decrease postsynaptic neuron activity.)pbdoc")
        .value("Modulatory", SynapseType::Modulatory,
            R"pbdoc(Modulatory synapses that influence plasticity and network dynamics.)pbdoc")
        .value("Electrical", SynapseType::Electrical,
            R"pbdoc(Electrical synapses that allow direct ionic current flow between neurons.)pbdoc")
        .value("GapJunction", SynapseType::GapJunction,
            R"pbdoc(Gap junction synapses that enable electrical coupling.)pbdoc")
        .export_values();
    
    // DevelopmentalStage enum with improved documentation
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", py::arithmetic(),
        R"pbdoc(Developmental stage enumeration defining different phases of brain development.)pbdoc")
        .value("Initial", DevelopmentalStage::Initial,
            R"pbdoc(Initial developmental stage with high plasticity and rapid learning.)pbdoc")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod,
            R"pbdoc(Critical period stage with sensitive windows for learning and pruning.)pbdoc")
        .value("Maturation", DevelopmentalStage::Maturation,
            R"pbdoc(Maturation stage with moderate plasticity and concept formation.)pbdoc")
        .value("Adult", DevelopmentalStage::Adult,
            R"pbdoc(Adult stage with stable neural architecture and refined behavior.)pbdoc")
        .value("Aging", DevelopmentalStage::Aging,
            R"pbdoc(Aging stage with declining plasticity and maintenance functions.)pbdoc")
        .export_values();
    
    // FiringState enum with improved documentation
    py::enum_<FiringState>(m, "FiringState", py::arithmetic(),
        R"pbdoc(Neuron firing state enumeration defining different electrical states.)pbdoc")
        .value("Resting", FiringState::Resting,
            R"pbdoc(Neuron at resting membrane potential, not actively firing.)pbdoc")
        .value("Active", FiringState::Active,
            R"pbdoc(Neuron actively depolarizing towards firing threshold.)pbdoc")
        .value("Refractory", FiringState::Refractory,
            R"pbdoc(Neuron in refractory period after firing, unable to fire again immediately.)pbdoc")
        .value("Inhibited", FiringState::Inhibited,
            R"pbdoc(Neuron actively inhibited, preventing firing despite depolarization.)pbdoc")
        .export_values();
    
    // ActionType enum with improved documentation
    py::enum_<ActionType>(m, "ActionType", py::arithmetic(),
        R"pbdoc(Action type enumeration defining different motor and behavioral actions.)pbdoc")
        .value("MoveForward", ActionType::MoveForward,
            R"pbdoc(Forward movement action.)pbdoc")
        .value("MoveBackward", ActionType::MoveBackward,
            R"pbdoc(Backward movement action.)pbdoc")
        .value("MoveLeft", ActionType::MoveLeft,
            R"pbdoc(Left movement action.)pbdoc")
        .value("MoveRight", ActionType::MoveRight,
            R"pbdoc(Right movement action.)pbdoc")
        .value("TurnLeft", ActionType::TurnLeft,
            R"pbdoc(Turn left action.)pbdoc")
        .value("TurnRight", ActionType::TurnRight,
            R"pbdoc(Turn right action.)pbdoc")
        .value("Look", ActionType::Look,
            R"pbdoc(Look action.)pbdoc")
        .value("LookUp", ActionType::LookUp,
            R"pbdoc(Look up action.)pbdoc")
        .value("LookDown", ActionType::LookDown,
            R"pbdoc(Look down action.)pbdoc")
        .value("Interact", ActionType::Interact,
            R"pbdoc(Interaction action.)pbdoc")
        .value("Eat", ActionType::Eat,
            R"pbdoc(Eating action.)pbdoc")
        .value("Drink", ActionType::Drink,
            R"pbdoc(Drinking action.)pbdoc")
        .value("Rest", ActionType::Rest,
            R"pbdoc(Rest action.)pbdoc")
        .value("Wait", ActionType::Wait,
            R"pbdoc(Wait action.)pbdoc")
        .value("Custom", ActionType::Custom,
            R"pbdoc(Custom user-defined action.)pbdoc")
        .export_values();
    
    // MotorCommand enum with improved documentation
    py::enum_<MotorCommand>(m, "MotorCommand", py::arithmetic(),
        R"pbdoc(Motor command enumeration defining low-level motor control signals.)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward,
            R"pbdoc(Low-level forward movement command.)pbdoc")
        .value("MoveBackward", MotorCommand::MoveBackward,
            R"pbdoc(Low-level backward movement command.)pbdoc")
        .value("TurnLeft", MotorCommand::TurnLeft,
            R"pbdoc(Low-level turn left command.)pbdoc")
        .value("TurnRight", MotorCommand::TurnRight,
            R"pbdoc(Low-level turn right command.)pbdoc")
        .value("LookLeft", MotorCommand::LookLeft,
            R"pbdoc(Low-level look left command.)pbdoc")
        .value("LookRight", MotorCommand::LookRight,
            R"pbdoc(Low-level look right command.)pbdoc")
        .value("Interact", MotorCommand::Interact,
            R"pbdoc(Low-level interaction command.)pbdoc")
        .value("Wait", MotorCommand::Wait,
            R"pbdoc(Low-level wait/idle command.)pbdoc")
        .export_values();
    
    // WorldObjectType enum with improved documentation
    py::enum_<WorldObjectType>(m, "WorldObjectType", py::arithmetic(),
        R"pbdoc(World object type enumeration defining different environmental objects.)pbdoc")
        .value("Empty", WorldObjectType::Empty,
            R"pbdoc(Empty space with no object.)pbdoc")
        .value("Resource", WorldObjectType::Resource,
            R"pbdoc(Resource object that can be collected.)pbdoc")
        .value("Hazard", WorldObjectType::Hazard,
            R"pbdoc(Hazard object that causes damage.)pbdoc")
        .value("Wall", WorldObjectType::Wall,
            R"pbdoc(Wall object that blocks movement.)pbdoc")
        .value("Marker", WorldObjectType::Marker,
            R"pbdoc(Marker object for positioning or signaling.)pbdoc")
        .export_values();
    
    // Enhanced Config class with improved API
    py::class_<Config>(m, "Config", py::dynamic_attr(),
        R"pbdoc(Configuration class for NLM system with comprehensive parameter management.
        
        The Config class provides a flexible interface for managing all configuration
        parameters of the NLM brain simulation. It supports loading from various sources,
        validation, and provides type-safe access to configuration values.
        
        Configuration parameters include:
        - neuron_count: Number of neurons in the simulation
        - region_count: Number of brain regions
        - connection_probability: Probability of random connections between neurons
        - simulation_timestep: Duration of each simulation step (seconds)
        - random_seed: Seed for random number generation
        - plasticity parameters (STDP, Hebbian, structural)
        - memory system parameters
        - development parameters
        - neuromodulation parameters
        "pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file.
             
             Args:
                 filepath: Path to the JSON configuration file
                 
             Raises:
                 RuntimeError: If the file cannot be read or parsed
             "pbdoc")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments.
           
           This method parses command line arguments and updates the configuration.
           Common arguments include --neuron_count, --random_seed, etc.
           "pbdoc")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file.
             
             Args:
                 filepath: Path where the configuration will be saved
                 
             Raises:
                 RuntimeError: If the file cannot be written
             "pbdoc")
        .def("set", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration parameter as integer value.")
        .def("set", [](Config& self, const std::string& key, float value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration parameter as float value.")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration parameter as string value.")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration parameter as boolean value.")
        .def("set", [](Config& self, const std::string& key, const std::vector<float>& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration parameter as vector of floats.")
        .def("get_int", [](Config& self, const std::string& key, int64_t default_value) {
            return self.getOr(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
           "Get an integer configuration parameter with default value.")
        .def("get_float", [](Config& self, const std::string& key, float default_value) {
            return self.getOr(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
           "Get a float configuration parameter with default value.")
        .def("get_string", [](Config& self, const std::string& key, const std::string& default_value) {
            return self.getOr(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
           "Get a string configuration parameter with default value.")
        .def("get_bool", [](Config& self, const std::string& key, bool default_value) {
            return self.getOr(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
           "Get a boolean configuration parameter with default value.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists.")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("from_dict", &api_helpers::configFromDict,
             "Create a Config object from a Python dictionary.
             
             Args:
                 config_dict: Python dictionary with configuration parameters
                 
             Returns:
                 Config: Config object with parameters from the dictionary
             "pbdoc")
        .def("to_dict", &api_helpers::configToDict,
             "Convert Config object to Python dictionary.
             
             Returns:
                 dict: Dictionary with all configuration parameters
             "pbdoc")
        .def("validate", &api_helpers::validateConfig,
             "Validate the configuration for errors.
             
             Raises:
                 RuntimeError: If the configuration contains errors
             "pbdoc")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "Return string representation of the Config object.")
        .doc() = R"pbdoc(Configuration management with type-safe parameter access.
        
        The Config class provides a comprehensive interface for managing all
        simulation parameters with proper type checking and validation.
        
        Usage examples:
        ```python
        # Create configuration
        config = Config()
        
        # Set parameters using different types
        config.set("neuron_count", 1000)
        config.set("connection_probability", 0.1f)
        config.set("random_seed", 42)
        
        # Get parameters with type safety
        count = config.get_int("neuron_count", 1000)
        prob = config.get_float("connection_probability", 0.1f)
        
        # Create brain with configuration
        brain = Brain(config)
        ```
        "pbdoc");
    
    // Enhanced SensoryInput class with better API
    py::class_<SensoryInput>(m, "SensoryInput", py::dynamic_attr(),
        R"pbdoc(Base class for sensory input with unified interface.
        
        SensoryInput provides a unified interface for different types of sensory
        inputs (vision, audio, internal signals, etc.) with consistent methods
        for accessing data, dimensions, and timestamps.
        "pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input.")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector.")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality.")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp.")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp.")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput: type=" + std::to_string(static_cast<int>(input.getType())) + 
                   ", dim=" + std::to_string(input.getDimensions()) + ">";
        });
    
    // Enhanced Vision class with specialized methods
    py::class_<Vision, SensoryInput>(m, "Vision", py::dynamic_attr(),
        R"pbdoc(Vision sensory input with spatial structure.
        
        Vision provides visual input data in a 2D grid format, representing
        the agent's visual field of the environment. Each pixel represents
        the intensity or color information at that location.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             "Create a Vision input with specified dimensions.")
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set the vision data vector.")
        .def("set_data_grid", [](Vision& self, const py::array_t<float>& grid) {
            // Convert numpy array to vector
            auto buf = grid.request();
            std::vector<float> data(buf.size);
            std::copy(static_cast<float*>(buf.ptr), static_cast<float*>(buf.ptr) + buf.size, data.begin());
            self.setData(data);
        }, py::arg("grid"),
             "Set vision data from a 2D numpy array or flat array.")
        .def("get_width", &Vision::getWidth, "Get the vision width.")
        .def("get_height", &Vision::getHeight, "Get the vision height.")
        .def("get_channels", &Vision::getChannels, "Get the number of channels.")
        .def("get_data_as_grid", [](Vision& self, size_t width, size_t height) {
            const auto& data = self.getData();
            py::array_t<float> result({height, width});
            auto buf = result.request();
            float* ptr = static_cast<float*>(buf.ptr);
            size_t idx = 0;
            for (size_t y = 0; y < height; ++y) {
                for (size_t x = 0; x < width; ++x) {
                    if (idx < data.size()) {
                        ptr[y * width + x] = data[idx++];
                    } else {
                        ptr[y * width + x] = 0.0f;
                    }
                }
            }
            return result;
        }, py::arg("width"), py::arg("height"),
             "Get vision data as a 2D grid array.")
        .def("normalize", [](Vision& self, float min_val, float max_val) {
            auto& data = const_cast<std::vector<float>&>(self.getData());
            for (auto& val : data) {
                val = (val - min_val) / (max_val - min_val);
            }
        }, py::arg("min_val"), py::arg("max_val"),
             "Normalize vision data to [0, 1] range.")
        .doc() = R"pbdoc(Vision sensory input with spatial structure and manipulation methods.
        
        The Vision class provides specialized methods for handling visual data,
        including data conversion, normalization, and grid representation.
        
        Usage examples:
        ```python
        # Create vision input
        vision = Vision(64, 64, 3)
        
        # Set data from Python list
        vision.set_data([0.1, 0.5, 0.9] * 64 * 64)
        
        # Set data from numpy array
        import numpy as np
        vision.set_data_grid(np.random.rand(64, 64))
        
        # Normalize data
        vision.normalize(0.0, 255.0)
        
        # Access dimensions
        print(f"Vision: {vision.get_width()}x{vision.get_height()}x{vision.get_channels()}")
        ```
        "pbdoc");
    
    // Enhanced Audio class with specialized methods
    py::class_<Audio, SensoryInput>(m, "Audio", py::dynamic_attr(),
        R"pbdoc(Audio sensory input with temporal structure.
        
        Audio provides auditory input data representing sound signals over time.
        It supports sample rate and duration specifications for accurate
        auditory processing.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"),
             "Create an Audio input with specified sample rate and duration.")
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set the audio data vector.")
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"),
             "Set the audio sample rate.")
        .def("get_sample_rate", &Audio::getSampleRate, "Get the sample rate.")
        .def("get_num_samples", &Audio::getNumSamples, "Get the number of samples.")
        .def("get_duration", [](Audio& self) {
            return static_cast<float>(self.getNumSamples()) / self.getSampleRate();
        }, "Get the audio duration in seconds.")
        .def("normalize", [](Audio& self, float min_val, float max_val) {
            auto& data = const_cast<std::vector<float>&>(self.getData());
            for (auto& val : data) {
                val = (val - min_val) / (max_val - min_val);
            }
        }, py::arg("min_val"), py::arg("max_val"),
             "Normalize audio data to [0, 1] range.")
        .doc() = R"pbdoc(Audio sensory input with temporal structure and manipulation methods.
        
        The Audio class provides specialized methods for handling audio data,
        including duration calculation, sample rate management, and normalization.
        
        Usage examples:
        ```python
        # Create audio input
        audio = Audio(44100, 44100)  # 1 second of audio at 44.1 kHz
        
        # Set audio data
        audio.set_data(list of float values)
        
        # Set sample rate
        audio.set_sample_rate(48000)  # 48 kHz
        
        # Get duration
        duration = audio.get_duration()  # e.g., 0.5 seconds
        
        # Normalize data
        audio.normalize(-1.0, 1.0)
        ```
        "pbdoc");
    
    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", py::dynamic_attr(),
        R"pbdoc(Internal signals sensory input representing neural state.
        
        InternalSignals provides access to the agent's internal neural state,
        including firing rates, membrane potentials, and other internal metrics
        that influence behavior and learning.
        "pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals.")
        .def("add_multiple_signals", [](InternalSignals& self, const std::vector<float>& values) {
            for (float val : values) {
                self.addSignal(val);
            }
        }, py::arg("values"),
             "Add multiple signal values.")
        .def("clear_signals", &InternalSignals::clearSignals,
             "Clear all internal signals.")
        .def("get_signal_count", [](InternalSignals& self) {
            return self.getData().size();
        }, "Get the number of internal signals.")
        .def("get_signal", [](InternalSignals& self, size_t index) {
            const auto& data = self.getData();
            if (index < data.size()) {
                return data[index];
            }
            throw py::index_error("Signal index out of range");
        }, py::arg("index"),
             "Get a specific internal signal by index.")
        .doc() = R"pbdoc(Internal signals for neural state representation.
        
        InternalSignals manages the agent's internal neural state variables,
        including firing rates, membrane potentials, and other metrics that
        influence behavior and learning processes.
        
        Usage examples:
        ```python
        # Create internal signals
        internal = InternalSignals()
        
        # Add signals
        internal.add_signal(0.5)
        internal.add_signal(1.2)
        internal.add_multiple_signals([0.1, 0.2, 0.3, 0.4])
        
        # Access signals
        signal_count = internal.get_signal_count()
        signal_0 = internal.get_signal(0)
        
        # Clear signals
        internal.clear_signals()
        ```
        "pbdoc");
    
    // Enhanced Action class with convenience methods
    py::class_<Action>(m, "Action", py::dynamic_attr(),
        R"pbdoc(Action representation for motor output with convenience methods.
        
        Action represents a motor command that the agent can execute in the world.
        It includes type, parameters, and methods for action manipulation and
        string representation.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create an Action with specified type.")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create an Action with type and parameters.")
        .def("get_type", &Action::getType, "Get the action type.")
        .def("set_type", &Action::setType, py::arg("type"),
             "Set the action type.")
        .def("get_parameters", &Action::getParameters, "Get the action parameters.")
        .def("set_parameters", &Action::setParameters, py::arg("params"),
             "Set the action parameters.")
        .def("get_name", &Action::getName, "Get the human-readable action name.")
        .def("clone", &Action::clone, "Create a copy of this action.")
        .def("is_valid", [](const Action& self) {
            return self.getType() != ActionType::Custom || !self.getParameters().empty();
        }, "Check if this action is valid.")
        .def("add_parameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"),
             "Add a parameter to this action.")
        .def("to_dict", [](const Action& self) {
            py::dict result;
            result["type"] = static_cast<int>(self.getType());
            result["parameters"] = self.getParameters();
            return result;
        }, "Convert action to dictionary representation.")
        .def("from_dict", [](Action& self, const py::dict& dict) {
            self.setType(static_cast<ActionType>(dict["type"].cast<int>()));
            self.setParameters(dict["parameters"].cast<std::vector<float>>());
        }, py::arg("dict"),
             "Update action from dictionary representation.")
        .doc() = R"pbdoc(Action representation with enhanced API.
        
        The Action class provides a flexible interface for motor commands with
        additional convenience methods for action manipulation and conversion.
        
        Usage examples:
        ```python
        # Create actions
        move_forward = Action(ActionType.MoveForward)
        custom_action = Action(ActionType.Custom, [0.5, 1.0, 0.8])
        
        # Access action properties
        print(action.get_name())  # e.g., "MoveForward"
        print(action.get_type())  # e.g., ActionType.MoveForward
        
        # Add parameters
        action.add_parameter(0.7)
        
        # Convert to/from dictionary
        action_dict = action.to_dict()
        action2 = Action()
        action2.from_dict(action_dict)
        
        # Check validity
        if action.is_valid():
            # Execute action
            pass
        ```
        "pbdoc");
    
    // Enhanced WorldObject class with world operations
    py::class_<WorldObject>(m, "WorldObject", py::dynamic_attr(),
        R"pbdoc(World object representation with position and properties.
        
        WorldObject represents objects in the simulated world, including their
        position, type, and properties. This class provides methods for object
        manipulation and world interaction.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create a WorldObject at specified position.")
        .def("get_position", [](const WorldObject& obj) {
            return py::make_tuple(obj.x, obj.y);
        }, "Get the object's position as (x, y) tuple.")
        .def("set_position", [](WorldObject& obj, float x, float y) {
            obj.x = x;
            obj.y = y;
        }, py::arg("x"), py::arg("y"),
             "Set the object's position.")
        .def("distance_to", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"),
             "Calculate distance to specified position.")
        .def("is_near", [](const WorldObject& obj, float x, float y, float distance) {
            return obj.distance_to(x, y) <= distance;
        }, py::arg("x"), py::arg("y"), py::arg("distance"),
             "Check if object is within specified distance of position.")
        .def("move_to", [](WorldObject& obj, float x, float y, float speed, float timestep) {
            float dx = x - obj.x;
            float dy = y - obj.y;
            float dist = std::sqrt(dx*dx + dy*dy);
            if (dist > 0.0f) {
                float factor = speed * timestep / dist;
                obj.x += dx * factor;
                obj.y += dy * factor;
            }
        }, py::arg("x"), py::arg("y"), py::arg("speed"), py::arg("timestep"),
             "Move object toward specified position.")
        .def("set_type", &WorldObject::setType, py::arg("type"),
             "Set the object's type.")
        .def("set_value", &WorldObject::setValue, py::arg("value"),
             "Set the object's value.")
        .def("set_radius", &WorldObject::setRadius, py::arg("radius"),
             "Set the object's radius.")
        .def("get_type_name", [](const WorldObject& obj) {
            // Convert enum to readable name
            switch (obj.type) {
                case WorldObjectType::Empty: return "Empty";
                case WorldObjectType::Resource: return "Resource";
                case WorldObjectType::Hazard: return "Hazard";
                case WorldObjectType::Wall: return "Wall";
                case WorldObjectType::Marker: return "Marker";
                default: return "Unknown";
            }
        }, "Get human-readable type name.")
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: " + std::to_string(obj.x) + ", " + std::to_string(obj.y) +
                   ", type=" + std::to_string(static_cast<int>(obj.type)) + ">";
        })
        .doc() = R"pbdoc(World object with position and properties.
        
        The WorldObject class represents objects in the simulated environment with
        position, type, and properties. It includes methods for movement and
        spatial operations.
        
        Usage examples:
        ```python
        # Create world objects
        player = WorldObject(0.0, 0.0, WorldObjectType.Resource, 10.0)
        obstacle = WorldObject(5.0, 3.0, WorldObjectType.Wall)
        
        # Access object properties
        x, y = player.get_position()  # (0.0, 0.0)
        distance = player.distance_to(1.0, 1.0)
        print(f"Distance to (1,1): {distance}")
        
        # Move objects
        player.move_to(1.0, 1.0, 1.0, 0.1)  # Move toward (1,1) at speed 1.0
        
        # Check proximity
        if obstacle.is_near(player.get_x(), player.get_y(), 0.5):
            print("Player is near obstacle")
        ```
        "pbdoc");
    
    // Enhanced AgentBody class with agent operations
    py::class_<AgentBody>(m, "AgentBody", py::dynamic_attr(),
        R"pbdoc(Agent body state with physics and movement properties.
        
        AgentBody represents the physical state of the agent in the world,
        including position, orientation, velocity, and energy. It provides
        methods for movement, physics simulation, and state management.
        "pbdoc")
        .def(py::init<>())
        .def("get_position", [](const AgentBody& body) {
            return py::make_tuple(body.x, body.y);
        }, "Get the agent's position as (x, y) tuple.")
        .def("set_position", [](AgentBody& body, float x, float y) {
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"),
             "Set the agent's position.")
        .def("get_orientation", [](const AgentBody& body) {
            return body.orientation * 180.0f / M_PI;  // Convert to degrees
        }, "Get the agent's orientation in degrees.")
        .def("set_orientation", [](AgentBody& body, float degrees) {
            body.orientation = degrees * M_PI / 180.0f;  // Convert from degrees
        }, py::arg("degrees"),
             "Set the agent's orientation in degrees.")
        .def("get_velocity", [](const AgentBody& body) {
            return py::make_tuple(body.velocityX, body.velocityY);
        }, "Get the agent's velocity as (vx, vy) tuple.")
        .def("set_velocity", [](AgentBody& body, float vx, float vy) {
            body.velocityX = vx;
            body.velocityY = vy;
        }, py::arg("vx"), py::arg("vy"),
             "Set the agent's velocity.")
        .def("get_speed", [](const AgentBody& body) {
            return std::sqrt(body.velocityX * body.velocityX + body.velocityY * body.velocityY);
        }, "Get the agent's speed.")
        .def("get_angle", [](const AgentBody& body) {
            return std::atan2(body.velocityY, body.velocityX) * 180.0f / M_PI;
        }, "Get the agent's movement angle in degrees.")
        .def("move", [](AgentBody& body, float dx, float dy) {
            body.x += dx;
            body.y += dy;
        }, py::arg("dx"), py::arg("dy"),
             "Move the agent by (dx, dy).")
        .def("move_towards", [](AgentBody& body, float target_x, float target_y, float speed, float timestep) {
            float dx = target_x - body.x;
            float dy = target_y - body.y;
            float dist = std::sqrt(dx*dx + dy*dy);
            if (dist > 0.0f) {
                float factor = speed * timestep / dist;
                body.x += dx * factor;
                body.y += dy * factor;
                body.velocityX = dx * factor / timestep;
                body.velocityY = dy * factor / timestep;
            }
        }, py::arg("target_x"), py::arg("target_y"), py::arg("speed"), py::arg("timestep"),
             "Move the agent toward target position.")
        .def("rotate", [](AgentBody& body, float delta_degrees) {
            body.orientation += delta_degrees * M_PI / 180.0f;
        }, py::arg("delta_degrees"),
             "Rotate the agent by delta degrees.")
        .def("reset", &AgentBody::reset,
             "Reset the agent's state to initial values.")
        .def("is_moving", [](const AgentBody& body) {
            return body.isMoving;
        }, "Check if the agent is currently moving.")
        .def("is_turning", [](const AgentBody& body) {
            return body.isTurning;
        }, "Check if the agent is currently turning.")
        .doc() = R"pbdoc(Agent body with physics and movement capabilities.
        
        The AgentBody class represents the physical state of the agent, including
        position, orientation, velocity, and energy. It provides comprehensive
        movement and physics simulation methods.
        
        Usage examples:
        ```python
        # Create agent body
        body = AgentBody()
        
        # Set initial state
        body.set_position(0.0, 0.0)
        body.set_orientation(90.0)  # Face north
        body.set_velocity(1.0, 0.0)  # Move east
        
        # Move agent
        body.move(0.1, 0.0)  # Move 0.1 units east
        body.move_towards(5.0, 5.0, 2.0, 0.1)  # Move toward (5,5) at speed 2.0
        
        # Get state information
        x, y = body.get_position()
        angle = body.get_angle()
        speed = body.get_speed()
        
        # Rotate
        body.rotate(45.0)  # Turn right 45 degrees
        
        # Check movement state
        if body.is_moving():
            print("Agent is moving")
        ```
        "pbdoc");
    
    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", py::dynamic_attr(),
        R"pbdoc(Result of an action execution with reward and feedback.
        
        ActionResult contains the outcome of executing an action, including
        reward value, success status, and optional feedback message. This
        class provides a standardized interface for action results.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create an ActionResult with reward, success, and message.")
        .def("get_reward", &ActionResult::getReward, "Get the reward value.")
        .def("set_reward", &ActionResult::setReward, py::arg("reward"),
             "Set the reward value.")
        .def("get_success", &ActionResult::getSuccess, "Get the success status.")
        .def("set_success", &ActionResult::setSuccess, py::arg("success"),
             "Set the success status.")
        .def("get_message", &ActionResult::getMessage, "Get the feedback message.")
        .def("set_message", &ActionResult::setMessage, py::arg("message"),
             "Set the feedback message.")
        .def("is_positive_reward", [](const ActionResult& result) {
            return result.reward > 0.0f;
        }, "Check if reward is positive.")
        .def("is_negative_reward", [](const ActionResult& result) {
            return result.reward < 0.0f;
        }, "Check if reward is negative.")
        .def("is_zero_reward", [](const ActionResult& result) {
            return result.reward == 0.0f;
        }, "Check if reward is zero.")
        .def("to_dict", [](const ActionResult& result) {
            py::dict dict;
            dict["reward"] = result.reward;
            dict["success"] = result.success;
            dict["message"] = result.message;
            return dict;
        }, "Convert action result to dictionary.")
        .def("from_dict", [](ActionResult& result, const py::dict& dict) {
            result.reward = dict["reward"].cast<float>();
            result.success = dict["success"].cast<bool>();
            if (dict.contains("message")) {
                result.message = dict["message"].cast<std::string>();
            }
        }, py::arg("dict"),
             "Update action result from dictionary.")
        .doc() = R"pbdoc(Action execution result with reward and feedback.
        
        The ActionResult class provides a standardized interface for action
        outcomes, including reward values, success status, and feedback messages.
        
        Usage examples:
        ```python
        # Create action results
        success_result = ActionResult(1.0, True, "Successfully moved")
        failure_result = ActionResult(-0.5, False, "Collided with wall")
        neutral_result = ActionResult(0.0, True, "No change")
        
        # Access result properties
        print(f"Reward: {result.get_reward()}")
        print(f"Success: {result.get_success()}")
        print(f"Message: {result.get_message()}")
        
        # Check reward type
        if result.is_positive_reward():
            print("Got positive reward!")
        elif result.is_negative_reward():
            print("Got penalty!")
        else:
            print("No reward change")
        
        # Convert to/from dictionary
        result_dict = result.to_dict()
        result2 = ActionResult()
        result2.from_dict(result_dict)
        ```
        "pbdoc");
    
    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", py::dynamic_attr(),
        R"pbdoc(Sensory percept data with multiple modalities.
        
        SensoryPercept represents the combined sensory input from all modalities
        (vision, audio, internal signals, proprioception). It provides methods
        for accessing and manipulating sensory data.
        "pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision, "Get vision sensory data.")
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision sensory data.")
        .def("get_vision_width", &SensoryPercept::getVisionWidth, "Get vision width.")
        .def("get_vision_height", &SensoryPercept::getVisionHeight, "Get vision height.")
        .def("get_touch", &SensoryPercept::getTouch, "Get touch sensory data.")
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch sensory data.")
        .def("get_internal", &SensoryPercept::getInternal, "Get internal sensory data.")
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal sensory data.")
        .def("get_proprioception", &SensoryPercept::getProprioception, "Get proprioception data.")
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data.")
        .def("get_audio", &SensoryPercept::getAudio, "Get audio sensory data.")
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio sensory data.")
        .def("get_all_signals", &SensoryPercept::getAllSignals, "Get all sensory signals.")
        .def("get_timestamp", &SensoryPercept::getTimestamp, "Get the percept timestamp.")
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set the percept timestamp.")
        .def("has_vision", [](const SensoryPercept& self) {
            return !self.getVision().empty();
        }, "Check if vision data is available.")
        .def("has_audio", [](const SensoryPercept& self) {
            return !self.getAudio().empty();
        }, "Check if audio data is available.")
        .def("has_internal", [](const SensoryPercept& self) {
            return !self.getInternal().empty();
        }, "Check if internal data is available.")
        .def("has_proprioception", [](const SensoryPercept& self) {
            return !self.getProprioception().empty();
        }, "Check if proprioception data is available.")
        .def("get_total_sensory_data", [](const SensoryPercept& self) {
            const auto& vision = self.getVision();
            const auto& touch = self.getTouch();
            const auto& internal = self.getInternal();
            const auto& proprio = self.getProprioception();
            const auto& audio = self.getAudio();
            return vision.size() + touch.size() + internal.size() + proprio.size() + audio.size();
        }, "Get total number of sensory data points.")
        .doc() = R"pbdoc(Sensory perception data from all modalities.
        
        The SensoryPercept class combines sensory input from multiple sources
        (vision, audio, internal signals, proprioception) into a unified
        interface for agent perception.
        
        Usage examples:
        ```python
        # Create sensory percept
        percept = SensoryPercept()
        
        # Set sensory data
        percept.set_vision([0.1, 0.5, 0.9] * 64 * 64)
        percept.set_touch([0.0, 1.0])
        percept.set_internal([0.5])
        
        # Access sensory data
        vision = percept.get_vision()
        touch = percept.get_touch()
        
        # Check available modalities
        if percept.has_vision():
            print("Has visual input")
        if percept.has_audio():
            print("Has auditory input")
        
        # Get total sensory data
        total_data = percept.get_total_sensory_data()
        ```
        "pbdoc");
    
    // Enhanced SimpleWorld class with convenience methods
    py::class_<SimpleWorld>(m, "SimpleWorld", py::dynamic_attr(),
        R"pbdoc(Simple 2D world for NLM simulation with enhanced API.
        
        SimpleWorld provides a 2D simulation environment with agents, objects,
        and physics. It includes methods for world configuration, agent control,
        object management, and world state.
        "pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"),
             "Configure the world dimensions and sensor parameters.")
        .def("reset", &SimpleWorld::reset,
             "Reset the world to initial state.")
        .def("set_agent_start", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"),
             "Set the agent's starting position.")
        .def("update", &SimpleWorld::update,
             py::arg("timestep"),
             "Update the world simulation by one timestep.")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"),
             "Apply a motor command to the agent.")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             "Get the agent's sensory percept.")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             "Get the agent's body state.")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object to the environment.")
        .def("remove_object", &SimpleWorld::removeObject,
             py::arg("x"), py::arg("y"),
             "Remove an object at specified position.")
        .def("is_valid_position", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"),
             "Check if a position is valid (within bounds, not occupied).")
        .def("get_width", &SimpleWorld::getWidth, "Get the world width.")
        .def("get_height", &SimpleWorld::getHeight, "Get the world height.")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy, "Get the maximum energy.")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set the maximum energy.")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
             "Get the energy decay rate.")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set the energy decay rate.")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime,
             "Get the current simulation time.")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set the random seed for reproducible simulations.")
        .def("get_random_seed", &SimpleWorld::getRandomSeed,
             "Get the current random seed.")
        .def("create_vision_from_numpy", [](SimpleWorld& self, const py::array_t<float>& array) {
            // Convert numpy array to Vision object
            py::module cv2 = py::module::import("numpy");
            py::object vision_obj = cv2.attr("zeros")({self.getVisionWidth(), self.getVisionHeight(), 3}, "float32");
            // For simplicity, we'll just return an empty percept
            return self.getSensoryPercept();
        }, py::arg("array"),
             "Create vision input from numpy array (requires additional processing).")
        .def("add_resource", [](SimpleWorld& self, float x, float y, float value = 1.0f) {
            WorldObject obj(x, y, WorldObjectType::Resource, value, 0.3f);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f,
             "Add a resource object at specified position.")
        .def("add_hazard", [](SimpleWorld& self, float x, float y, float severity = 1.0f) {
            WorldObject obj(x, y, WorldObjectType::Hazard, severity, 0.4f);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("severity") = 1.0f,
             "Add a hazard object at specified position.")
        .def("add_wall", [](SimpleWorld& self, float x, float y, float thickness = 0.5f) {
            WorldObject obj(x, y, WorldObjectType::Wall, 0.0f, thickness);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("thickness") = 0.5f,
             "Add a wall object at specified position.")
        .def("get_nearby_objects", [](SimpleWorld& self, float x, float y, float radius) {
            py::list result;
            // This would require iteration over world objects
            // For now, we'll just return an empty list
            return result;
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
             "Get objects within specified radius of position.")
        .def("get_object_count", [](SimpleWorld& self) {
            // This would return the number of world objects
            // For now, we'll return 0 as placeholder
            return 0;
        }, "Get the number of world objects.")
        .doc() = R"pbdoc(Simple world simulation with enhanced API.
        
        The SimpleWorld class provides a 2D simulation environment with support
        for agents, objects, and physics. It includes convenience methods for
        object creation and world management.
        
        Usage examples:
        ```python
        # Create and configure world
        world = SimpleWorld()
        world.configure(64, 64, 16, 16)  # 64x64 world, 16x16 vision
        
        # Add environment objects
        world.add_resource(10.0, 20.0, 5.0)  # Resource at (10,20) with value 5
        world.add_hazard(30.0, 40.0, 0.8)   # Hazard at (30,40) with severity 0.8
        world.add_wall(50.0, 50.0)          # Wall at (50,50)
        
        # Set agent starting position
        world.set_agent_start(5.0, 5.0)
        
        # Get world state
        print(f"World size: {world.get_width()}x{world.get_height()}")
        print(f"Object count: {world.get_object_count()}")
        
        # Update simulation
        for step in range(100):
            world.update(0.1)
        ```
        "pbdoc");
    
    // Enhanced Brain class with convenience methods and validation
    py::class_<Brain>(m, "Brain", py::dynamic_attr(),
        R"pbdoc(Central neural simulation brain with enhanced API.
        
        Brain is the core component of the NLM system, implementing neural
        computation with integrated memory, prediction, cognition, and
        neuromodulation systems. It provides comprehensive control over
        brain initialization, simulation, and state management.
        "pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a Brain with specified configuration.")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration.")
        .def("step", static_cast<void (Brain::*)(Step)>(&Brain::step),
             py::arg("current_step"),
             "Perform a simulation step.")
        .def("step", static_cast<void (Brain::*)(Step, TimeStamp)>(&Brain::step),
             py::arg("current_step"), py::arg("current_time"),
             "Perform a simulation step with timestamp.")
        .def("receive_sensory_input", &Brain::receiveSensoryInput, py::arg("input"),
             "Receive sensory input from environment.")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron_id"), py::arg("current"),
             "Inject current into a specific neuron.")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("neuron_type"), py::arg("current"),
             "Inject current into all neurons of a specific type.")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity.")
        .def("reset", &Brain::reset,
             "Reset brain state to initial values.")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file.")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region.")
        .def("get_region", &Brain::getRegion, py::arg("region_id"),
             "Get a region by ID.")
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions.")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs.")
        .def("get_regions", &Brain::getRegions,
             "Get all regions.")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions.")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions.")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons.")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons.")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons.")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio.")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count.")
        .def("get_pending_spike_event_count", &Brain::getPendingSpikeEventCount,
             "Get number of pending spike events.")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage.")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage, py::arg("stage"),
             "Set developmental stage.")
        .def("get_config", &Brain::getConfig,
             "Get the configuration.")
        .def("get_random_generator", &Brain::getRandomGenerator,
             "Get the random generator.")
        .def("get_spike_system", &Brain::getSpikeSystem,
             "Get the spike system.")
        .def("get_stdp", &Brain::getSTDP,
             "Get the STDP plasticity system.")
        .def("get_hebbian", &Brain::getHebbian,
             "Get the Hebbian plasticity system.")
        .def("get_structural_plasticity", &Brain::getStructuralPlasticity,
             "Get the structural plasticity system.")
        .def("get_working_memory", &Brain::getWorkingMemory,
             "Get the working memory system.")
        .def("get_episodic_memory", &Brain::getEpisodicMemory,
             "Get the episodic memory system.")
        .def("get_associative_memory", &Brain::getAssociativeMemory,
             "Get the associative memory system.")
        .def("get_prediction_system", &Brain::getPredictionSystem,
             "Get the prediction system.")
        .def("get_planner", &Brain::getPlanner,
             "Get the neural planner.")
        .def("get_concept_formation", &Brain::getConceptFormation,
             "Get the concept formation system.")
        .def("get_attention", &Brain::getAttention,
             "Get the attentional selection system.")
        .def("get_development_system", &Brain::getDevelopmentSystem,
             "Get the development system.")
        .def("get_dopamine", &Brain::getDopamine,
             "Get the dopamine neuromodulator.")
        .def("get_curiosity", &Brain::getCuriosity,
             "Get the curiosity neuromodulator.")
        .def("get_novelty", &Brain::getNovelty,
             "Get the novelty detection neuromodulator.")
        .def("get_prediction_error", &Brain::getPredictionErrorSignal,
             "Get the prediction error neuromodulator.")
        .def("log_status", &Brain::logStatus,
             "Log brain status and statistics.")
        .def("create_default_config", &api_helpers::createDefaultConfig,
             "Create a default configuration with sensible values.")
        .def("create_brain", [](std::shared_ptr<Config> config) {
            return std::make_shared<Brain>(config);
        }, py::arg("config"),
             "Create a Brain instance with configuration.")
        .def("create_simple_world", []() {
            return std::make_shared<SimpleWorld>();
        },
             "Create a new SimpleWorld instance.")
        .def("validate_config", &api_helpers::validateConfig,
             "Validate a configuration for errors.")
        .def("from_dict", [](std::shared_ptr<Config> config, const py::dict& dict) {
            return api_helpers::configFromDict(dict);
        }, py::arg("config"), py::arg("dict"),
             "Create configuration from Python dictionary.")
        .def("to_dict", [](const std::shared_ptr<Config>& config) {
            return api_helpers::configToDict(config);
        }, py::arg("config"),
             "Convert configuration to Python dictionary.")
        .def("simulate_episode", [](Brain& brain, SimpleWorld& world, int steps) {
            // Simulate one episode of interaction
            for (int step = 0; step < steps; ++step) {
                brain.step(step);
                auto percept = world.getSensoryPercept();
                brain.receiveSensoryInput(percept);
                auto action = brain.produceAction();
                world.applyMotorCommand(action, step * brain.getConfig()->getOr<float>("simulation_timestep", 0.001));
            }
        }, py::arg("world"), py::arg("steps"),
             "Simulate one episode of agent-environment interaction.")
        .def("get_constants", []() {
            py::dict constants;
            constants["INVALID_NEURON_ID"] = INVALID_NEURON_ID;
            constants["INVALID_SYNAPSE_ID"] = INVALID_SYNAPSE_ID;
            constants["INVALID_REGION_ID"] = INVALID_REGION_ID;
            constants["INVALID_POPULATION_ID"] = INVALID_POPULATION_ID;
            return constants;
        }, "Get system constants and sentinel values.")
        .def("get_version", []() {
            return "NLM Phase 6 - Integrated Artificial Brain v0.1.0";
        }, "Get the NLM version information.")
        .doc() = R"pbdoc(Central neural simulation brain with comprehensive API.
        
        The Brain class provides complete control over the NLM neural simulation,
        including initialization, simulation steps, sensory processing, motor
        output, memory systems, and all integrated brain components.
        
        Key features:
        - Comprehensive initialization and configuration
        - Step-based simulation with precise timing
        - Sensory input processing and motor output generation
        - Full integration with memory and cognition systems
        - Checkpoint saving and loading
        - Real-time status monitoring and statistics
        
        Usage examples:
        ```python
        # Create brain with default configuration
        brain = Brain()
        brain.initialize()
        
        # Create world
        world = SimpleWorld()
        world.configure(64, 64, 16, 16)
        
        # Simulate interaction
        for step in range(1000):
            brain.step(step)
            percept = world.getSensoryPercept()
            brain.receiveSensoryInput(percept)
            action = brain.produceAction()
            world.applyMotorCommand(action, step * 0.001)
            
            if step % 100 == 0:
                print(f"Step {step}: {brain.get_status()}")
        ```
        "pbdoc");
    
    // Enhanced AgentBrain class with convenience methods
    py::class_<AgentBrain>(m, "AgentBrain", py::dynamic_attr(),
        R"pbdoc(Agent brain interface connecting NLM brain to world with enhanced API.
        
        AgentBrain provides a simplified interface for connecting the NLM brain
        to a simulated world, handling sensory transduction and motor decoding
        automatically. It includes curiosity-driven exploration and reward
        modulation.
        "pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an AgentBrain with specified brain.")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world.")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size.")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size.")
        .def("process_sensory_input", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain.")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command.")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation.")
        .def("update_development", &AgentBrain::updateDevelopment, py::arg("timestep"),
             "Update development system.")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage.")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level.")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level.")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level.")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error.")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode.")
        .def("get_brain", &AgentBrain::getBrain,
             "Get the underlying brain.")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
             "Enable or disable reward modulation.")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
             "Enable or disable structural plasticity.")
        .def("enable_development", &AgentBrain::enableDevelopment, py::arg("enable"),
             "Enable or disable development.")
        .def("enable_curiosity", &AgentBrain::enableCuriosity, py::arg("enable"),
             "Enable or disable curiosity-driven exploration.")
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled.")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled.")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled.")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled.")
        .def("run_episode", [](AgentBrain& agent, SimpleWorld& world, int steps) {
            // Run one complete episode
            for (int step = 0; step < steps; ++step) {
                auto percept = world.getSensoryPercept();
                agent.processSensoryInput(percept);
                
                float reward = 0.0f;
                if (agent.is_moving_near_resource()) {
                    reward += 1.0f;
                }
                if (agent.is_avoiding_hazard()) {
                    reward -= 0.5f;
                }
                
                auto action = agent.decode_motor_command();
                world.applyMotorCommand(action, step * 0.1);
                
                // For now, we'll use a simple reward
                if (step > 0 && step % 100 == 0) {
                    agent.apply_reward_modulation(0.5f, 0.0f);
                }
            }
        }, py::arg("world"), py::arg("steps"),
             "Run one complete episode of interaction.")
        .def("is_moving_near_resource", [](AgentBrain& agent) {
            // Check if near resource (would require world access)
            return false;
        }, "Check if agent is near a resource.")
        .def("is_avoiding_hazard", [](AgentBrain& agent) {
            // Check if avoiding hazard (would require world access)
            return false;
        }, "Check if agent is avoiding hazard.")
        .def("get_exploration_stats", [](AgentBrain& agent) {
            py::dict stats;
            stats["curiosity_level"] = agent.get_curiosity_level();
            stats["novelty_level"] = agent.get_novelty_level();
            stats["prediction_error"] = agent.get_prediction_error();
            stats["reward_modulation_enabled"] = agent.is_reward_modulation_enabled();
            stats["curiosity_enabled"] = agent.is_curiosity_enabled();
            return stats;
        }, "Get exploration and neuromodulation statistics.")
        .doc() = R"pbdoc(Agent brain interface with enhanced exploration and learning API.
        
        The AgentBrain class provides a simplified, high-level interface for
        connecting the NLM brain to a simulated world. It handles the complex
        details of sensory transduction, motor decoding, and neuromodulation,
        while providing convenience methods for common agent behaviors.
        
        Key features:
        - Automatic sensory processing and motor decoding
        - Curiosity-driven exploration for active learning
        - Reward modulation for reinforcement learning
        - Developmental learning over time
        - Convenience methods for common behaviors
        
        Usage examples:
        ```python
        # Create agent with brain
        brain = Brain()
        world = SimpleWorld()
        world.configure(64, 64, 16, 16)
        
        agent = AgentBrain(brain)
        agent.initialize(world)
        
        # Run episode
        agent.run_episode(world, 1000)
        
        # Get agent statistics
        stats = agent.get_exploration_stats()
        print(f"Curiosity level: {stats['curiosity_level']}")
        print(f"Novelty level: {stats['novelty_level']}")
        ```
        "pbdoc");
    
    // Module-level convenience functions
    m.def("create_default_config", &api_helpers::createDefaultConfig,
          "Create a default configuration with sensible values for rapid prototyping.")
    
    m.def("create_brain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, py::arg("config"),
          "Create a Brain instance with specified configuration.")
    
    m.def("create_simple_world", []() {
        return std::make_shared<SimpleWorld>();
    }, "Create a new SimpleWorld instance.")
    
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"),
          "Create an AgentBrain with specified brain.")
    
    m.def("validate_config", &api_helpers::validateConfig,
          "Validate a configuration for errors.")
    
    m.def("from_dict", [](const py::dict& dict) {
        return api_helpers::configFromDict(dict);
    }, py::arg("dict"),
          "Create configuration from Python dictionary.")
    
    m.def("to_dict", [](const std::shared_ptr<Config>& config) {
        return api_helpers::configToDict(config);
    }, py::arg("config"),
          "Convert configuration to Python dictionary.")
    
    // Error handling utilities
    m.def("get_last_error", []() {
        const char* last_error = PyErr_Fetch();
        if (last_error) {
            std::string error_msg = last_error;
            PyMem_Free(last_error);
            return error_msg;
        }
        return std::string("No error");
    }, "Get the last error message.")
    
    m.def("clear_error", []() {
        PyErr_Clear();
    }, "Clear any pending error.")
    
    // Version and information
    m.def("get_version", []() {
        return "NLM Phase 6 - Integrated Artificial Brain v0.1.0";
    }, "Get the NLM version information.")
    
    m.def("get_info", []() {
        py::dict info;
        info["version"] = "NLM Phase 6 - Integrated Artificial Brain v0.1.0";
        info["description"] = "Experimental artificial developmental brain";
        info["author"] = "NLM Research Team";
        info["license"] = "MIT";
        info["api_version"] = "enhanced";
        info["features"] = {"memory_systems", "prediction", "cognition", "neuromodulation", "development"};
        return info;
    }, "Get comprehensive information about the NLM system.")
    
    // Constants and sentinel values
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Performance and debugging utilities
    m.def("enable_debug_mode", []() {
        // Enable debug logging and verbose output
        // Implementation would depend on NLM's logging system
        py::module::import("logging").attr("basicConfig")(
            level=py::module::import("logging").attr("DEBUG")
        );
    }, "Enable debug mode with verbose logging.")
    
    m.def("disable_debug_mode", []() {
        // Disable debug logging
        py::module::import("logging").attr("basicConfig")(
            level=py::module::import("logging").attr("INFO")
        );
    }, "Disable debug mode.")
    
    m.def("get_performance_stats", [](Brain& brain) {
        py::dict stats;
        stats["total_neurons"] = brain.get_total_neuron_count();
        stats["total_synapses"] = brain.get_total_synapse_count();
        stats["active_neurons"] = brain.get_active_neuron_count();
        stats["firing_neurons"] = brain.get_firing_neuron_count();
        stats["total_spikes"] = brain.get_total_spike_count();
        stats["avg_firing_rate"] = brain.get_average_firing_rate();
        stats["ei_ratio"] = brain.get_excitation_inhibition_ratio();
        return stats;
    }, py::arg("brain"),
          "Get performance statistics from brain.")
    
    // Example configurations
    m.def("get_example_configs", []() {
        py::dict examples;
        
        // Exploration example
        py::dict exploration;
        exploration["neuron_count"] = 2000;
        exploration["region_count"] = 3;
        exploration["connection_probability"] = 0.15;
        exploration["random_seed"] = 12345;
        exploration["curiosity_enabled"] = True;
        exploration["development_enabled"] = True;
        examples["exploration"] = exploration;
        
        // Learning example
        py::dict learning;
        learning["neuron_count"] = 5000;
        learning["region_count"] = 2;
        learning["connection_probability"] = 0.1;
        learning["stdp_ltp_weight"] = 0.015;
        learning["stdp_ltd_weight"] = 0.018;
        learning["reward_modulation_enabled"] = True;
        learning["structural_plasticity_enabled"] = True;
        examples["learning"] = learning;
        
        // Development example
        py::dict development;
        development["neuron_count"] = 3000;
        development["region_count"] = 4;
        development["connection_probability"] = 0.12;
        development["synaptogenesis_rate"] = 0.00015;
        development["pruning_rate"] = 0.00005;
        development["development_enabled"] = True;
        development["critical_period_enabled"] = True;
        examples["development"] = development;
        
        return examples;
    }, "Get example configuration templates for different use cases.")
    
    // Help and documentation
    m.def("get_help", [](const std::string& topic) {
        py::dict help;
        
        if (topic == "quick_start") {
            help["title"] = "Quick Start Guide";
            help["content"] = R"pbdoc(
                Quick start for NLM Python API:
                
                1. Import the module
                import pynlm
                
                2. Create brain with default configuration
                brain = pynlm.Brain()
                
                3. Initialize brain
                brain.initialize()
                
                4. Create world and agent
                world = pynlm.SimpleWorld()
                world.configure(64, 64, 16, 16)
                agent = pynlm.AgentBrain(brain, world)
                
                5. Run simulation
                for step in range(1000):
                    brain.step(step)
                    percept = world.getSensoryPercept()
                    brain.receiveSensoryInput(percept)
                    action = brain.produceAction()
                    world.applyMotorCommand(action, step * 0.001)
            )pbdoc";
        } else {
            help["title"] = "Help Topic Not Found";
            help["content"] = "No help available for topic: " + topic;
        }
        
        return help;
    }, py::arg("topic"),
          "Get help documentation for specific topics.")
    
    // Tutorial and examples system
    m.def("get_tutorial", [](const std::string& lesson) {
        py::dict tutorial;
        
        if (lesson == "basics") {
            tutorial["title"] = "NLM Python Basics";
            tutorial["content"] = R"pbdoc(
                Lesson 1: Basic Brain Creation and Simulation
                
                This lesson covers the fundamentals of using the NLM Python API
                to create and simulate an artificial brain.
                
                Step 1: Import and Setup
                import pynlm
                
                Step 2: Create Configuration
                config = pynlm.Config()
                config.set("neuron_count", 1000)
                config.set("region_count", 1)
                
                Step 3: Create Brain
                brain = pynlm.Brain(config)
                
                Step 4: Initialize
                brain.initialize()
                
                Step 5: Create World
                world = pynlm.SimpleWorld()
                world.configure(64, 64, 16, 16)
                
                Step 6: Create Agent
                agent = pynlm.AgentBrain(brain, world)
                
                Step 7: Run Simulation
                for step in range(100):
                    brain.step(step)
                    percept = world.getSensoryPercept()
                    brain.receiveSensoryInput(percept)
                    action = brain.produceAction()
                    world.applyMotorCommand(action, step * 0.001)
            )pbdoc";
        } else {
            tutorial["title"] = "Tutorial Not Found";
            tutorial["content"] = "Tutorial not available for lesson: " + lesson;
        }
        
        return tutorial;
    }, py::arg("lesson"),
          "Get tutorial content for specific lessons.");
    
    // Type checking and validation utilities
    m.def("validate_neuron_id", &api_helpers::getValidNeuronId,
          "Validate and normalize a neuron ID.");
    
    m.def("validate_region_id", &api_helpers::getValidRegionId,
          "Validate and normalize a region ID.");
}

} // namespace nlm
