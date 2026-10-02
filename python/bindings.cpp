#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        The Config class now supports modern JSON/YAML configuration with proper
        serialization and validation. Previous key=value format is still supported
        for backward compatibility.
        
        Key improvements:
        - JSON/YAML configuration file support
        - Proper value serialization (no more PLACEHOLDER_VALUE)
        - Type-safe configuration access
        - Configuration validation
        - Backward compatibility with legacy config files
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Add Python-specific exceptions
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");
    py::register_exception<std::out_of_range>(m, "OutOfRange");

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
        .def("__ne__", &NeuronId::operator!=)
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
        .def("__ne__", &NeuronId::operator!=)
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
        .def("__ne__", &NeuronId::operator!=);

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

    // Enhanced Config class with JSON support
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)
        
        The Config class provides flexible configuration management with support for:
        - JSON/YAML configuration files (modern format)
        - Legacy key=value format (backward compatibility)
        - Command line argument loading
        - Type-safe value access
        - Configuration validation
        - Serialization/deserialization
        
        The configuration supports various data types:
        - Numbers (int, int64_t, double)
        - Booleans
        - Strings
        - Arrays of the above types
        
        Common configuration keys:
        - neuron_count: Number of neurons (default: 1000)
        - region_count: Number of brain regions (default: 1)
        - connection_probability: Connection probability (default: 0.1)
        - simulation_timestep: Time step size (default: 0.001)
        - random_seed: Random seed (default: 42)
        - And many more...
        
        Usage examples:
        
        // Create default config
        config = Config()
        
        // Load from JSON file
        config.load_from_json_file("config.json")
        
        // Load from legacy format
        config.load_from_file("config.txt")
        
        // Load from command line
        config.load_from_args(sys.argv)
        
        // Set values
        config.set("neuron_count", 2000)
        config.set("connection_probability", 0.15)
        
        // Get values
        neuron_count = config.get("neuron_count", 1000)
        
        // Save to JSON
        config.save_to_json_file("output_config.json")
        
        // Get configuration as Python dictionary
        config_dict = config.to_dict()
        
        // Load from dictionary
        config.from_dict(config_dict)
    )pbdoc")
        .def(py::init<>())
        
        // File I/O methods
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file (JSON, YAML, or legacy key=value format)")
        .def("load_from_json_file", &Config::loadFromJson,
             py::arg("json_string"), "Load configuration from JSON string")
        .def("load_from_legacy_file", [](Config& self, const std::string& filepath) {
            // Legacy format uses simple key=value parsing
            std::ifstream file(filepath);
            if (!file.is_open()) return false;
            return self.loadFromFile(filepath);
        }, py::arg("filepath"), "Load configuration from legacy key=value file")
        
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("save_to_json_file", &Config::saveToFile,
             py::arg("filepath"), "Save configuration to a JSON file (alias for save_to_file)")
        
        // JSON string methods
        .def("load_from_json", &Config::loadFromJson, py::arg("json_string"),
             "Load configuration from JSON string")
        .def("save_to_json", &Config::saveToJson,
             "Return configuration as JSON string")
        
        // Command line loading
        .def("load_from_args", &Config::loadFromArgs, py::arg("argc"), py::arg("argv"),
             "Load configuration from command line arguments")
        
        // Value access methods
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", &Config::get<std::string>, py::arg("key"),
             "Get configuration value as string (returns None if not found)")
        .def("get_int", &Config::get<int>, py::arg("key"),
             "Get configuration value as int (returns None if not found)")
        .def("get_float", &Config::get<double>, py::arg("key"),
             "Get configuration value as float (returns None if not found)")
        .def("get_bool", &Config::get<bool>, py::arg("key"),
             "Get configuration value as bool (returns None if not found)")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys as list")
        
        // Value setting methods
        .def("set", py::overload_cast<const std::string&, const std::string&, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value (string type)")
        .def("set", py::overload_cast<const std::string&, int, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value (int type)")
        .def("set", py::overload_cast<const std::string&, double, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value (float type)")
        .def("set", py::overload_cast<const std::string&, bool, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value (bool type)")
        .def("set_with_description", [](Config& self, const std::string& key, const std::string& value, 
                                 const std::string& description) {
            self.set(key, value, ConfigSource::Runtime);
            // Note: Setting description requires a separate method in the implementation
        }, py::arg("key"), py::arg("value"), py::arg("description"),
             "Set configuration value with description")
        
        // Utility methods
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
        .def("summary", &Config::summary,
             "Get configuration summary as string")
        .def("to_dict", &Config::getAsMap,
             "Convert configuration to Python dictionary")
        .def("from_dict", [](Config& self, const std::map<std::string, ConfigValue>& config) {
            self.clear();
            self.setFromMap(config);
        }, py::arg("config_dict"), "Load configuration from Python dictionary")
        .def("validate", &Config::validate,
             "Validate configuration values")
        .def("get_config_file_extension", &Config::getConfigFileExtension,
             "Get recommended configuration file extension")
        
        // Statistics and debugging
        .def("get_entry_count", &Config::getKeys,
             "Get number of configuration entries (alias for getKeys)")
        .def("is_valid", &Config::validate,
             "Check if configuration is valid")
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Add convenience functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible values for NLM");
    
    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants for convenience
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Configuration key constants
    py::class_<py::dict>(m, "ConfigKeys", "NLM configuration key constants")
        .def(py::init<>())  // Empty constructor
        .def_readonly("NEURON_COUNT", "neuron_count", "Number of neurons")
        .def_readonly("REGION_COUNT", "region_count", "Number of neural regions")
        .def_readonly("CONNECTION_PROBABILITY", "connection_probability", "Connection probability")
        .def_readonly("SIMULATION_TIMESTEP", "simulation_timestep", "Simulation time step")
        .def_readonly("RANDOM_SEED", "random_seed", "Random seed")
        .def_readonly("STDP_LTP_WEIGHT", "stdp_ltp_weight", "STDP LTP weight")
        .def_readonly("STDP_LTD_WEIGHT", "stdp_ltd_weight", "STDP LTD weight")
        .def_readonly("STDP_TAU", "stdp_tau", "STDP time constant")
        .def_readonly("SYNAPTOTELEGENESIS_RATE", "synaptogenesis_rate", "Synaptogenesis rate")
        .def_readonly("PRUNING_RATE", "pruning_rate", "Pruning rate");

    // Example configuration helper
    m.def("create_example_config", []() {
        auto config = std::make_shared<Config>();
        
        // Set example values for common use cases
        config->set("neuron_count", 500, ConfigSource::Default);
        config->set("region_count", 2, ConfigSource::Default);
        config->set("connection_probability", 0.1, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        return config;
    }, "Create an example configuration for basic NLM simulations");

    // Version and utility functions
    m.def("get_nlm_version", []() {
        return "NLM 0.1.0";
    }, "Get NLM version string");
    
    m.def("get_phase_info", []() {
        return "NLM is currently at Phase 6: FINAL INTEGRATION";
    }, "Get current NLM phase information");

} // namespace nlm
