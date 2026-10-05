// NLM Python Bindings - Core Module
// This module provides fundamental types and core functionality

namespace py = pybind11;
namespace nlm {

namespace core {

PYBIND11_MODULE(pynlm_core, m) {
    m.doc() = R"pbdoc(
        NLM Core Types Module
        -----------------------
        
        Fundamental data types and identifiers for the NLM neural simulation framework.
        This module provides:
        - Neuron, Synapse, Region, and Population identifiers
        - Core type definitions and constants
        - Basic type conversions and utilities
        
        This module is intended to be used internally by other NLM modules.
    )pbdoc";

    // Core type definitions
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        NeuronId is a wrapper around uint64_t providing:
        - Safe access to neuron IDs
        - Equality comparison operations
        - Hash function for use in Python dicts/sets
        - String representation for debugging
        
        Examples:
            >>> neuron_id = NeuronId(42)
            >>> print(neuron_id)
            <NeuronId: 42>
            >>> neuron_id.value
            42
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create NeuronId from raw value")
        .def_readwrite("value", &NeuronId::value,
                      "Raw numeric value of the neuron ID")
        .def("index", &NeuronId::index,
             "Return the index representation")
        .def("__eq__", &NeuronId::operator==,
             "Check equality with another NeuronId")
        .def("__ne__", &NeuronId::operator!=,
             "Check inequality with another NeuronId")
        .def("__hash__", [](const NeuronId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, "Hash function for use in Python dicts/sets")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "String representation")
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        }, "String conversion");

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        SynapseId provides safe synaptic connection identification.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a neural region.
        
        RegionId identifies brain regions in the neural network.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        PopulationId identifies groups of neurons with similar properties.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Type definitions
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of neuron types in the NLM brain.
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron")
        .value("Motor", NeuronType::Motor, "Motor neuron")
        .value("Internal", NeuronType::Internal, "Internal neuron")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of synapse connection types.
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory connection")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory connection")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory connection")
        .value("Electrical", SynapseType::Electrical, "Electrical connection")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction connection")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of brain development stages.
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Enumeration of neuron firing states.
    )pbdoc")
        .value("Resting", FiringState::Resting, "Not firing")
        .value("Active", FiringState::Active, "Currently firing")
        .value("Refractory", FiringState::Refractory, "In refractory period")
        .value("Inhibited", FiringState::Inhibited, "Inhibited from firing")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Enumeration of motor action types.
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward")
        .value("MoveLeft", ActionType::MoveLeft, "Move left")
        .value("MoveRight", ActionType::MoveRight, "Move right")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left")
        .value("TurnRight", ActionType::TurnRight, "Turn right")
        .value("Look", ActionType::Look, "Look center")
        .value("LookUp", ActionType::LookUp, "Look up")
        .value("LookDown", ActionType::LookDown, "Look down")
        .value("Interact", ActionType::Interact, "Interact with object")
        .value("Eat", ActionType::Eat, "Eat")
        .value("Drink", ActionType::Drink, "Drink")
        .value("Rest", ActionType::Rest, "Rest")
        .value("Wait", ActionType::Wait, "Wait")
        .value("Custom", ActionType::Custom, "Custom action")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Enumeration of low-level motor commands.
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact")
        .value("Wait", MotorCommand::Wait, "Wait")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enumeration of world object types.
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Resource item")
        .value("Hazard", WorldObjectType::Hazard, "Hazard item")
        .value("Wall", WorldObjectType::Wall, "Wall")
        .value("Marker", WorldObjectType::Marker, "Marker point")
        .export_values();

    // Type aliases for convenience
    using FloatArray = std::vector<float>;
    
    py::class_<FloatArray>(m, "FloatArray", R"pbdoc(
        Dynamic array of floating point values.
        
        Provides Python-like interface to C++ vector<float>:
        - Index-based access
        - Iteration support
        - Appending values
        - Length queries
        
        Examples:
            >>> arr = FloatArray([1.0, 2.0, 3.0])
            >>> len(arr)
            3
            >>> arr[0]
            1.0
            >>> arr.append(4.0)
            >>> arr
            [1.0, 2.0, 3.0, 4.0]
    )pbdoc")
        .def(py::init<>())
        .def(py::init<const std::vector<float>&>(), py::arg("data"),
             "Create FloatArray from Python list")
        .def("__len__", [](const std::vector<float>& v) { return v.size(); },
             "Return number of elements")
        .def("__getitem__", [](const std::vector<float>& v, size_t i) {
            return v.at(i);
        }, "Get element at index (raises IndexError if out of bounds)")
        .def("__setitem__", [](std::vector<float>& v, size_t i, float val) {
            v[i] = val;
        }, "Set element at index")
        .def("__iter__", [](const std::vector<float>& v) {
            return py::make_iterator(v.begin(), v.end());
        }, "Iterate over elements (Pythonic)")
        .def("append", &std::vector<float>::push_back,
             "Append value to end of array")
        .def("clear", &std::vector<float>::clear,
             "Remove all elements")
        .def("reserve", &std::vector<float>::reserve,
             "Reserve memory for given capacity")
        .def("capacity", [](const std::vector<float>& v) {
            return v.capacity();
        }, "Get allocated capacity")
        .def("resize", &std::vector<float>::resize,
             "Resize array to given length")
        .def("data", [](std::vector<float>& v) {
            return py::memoryview(v);
        }, "Get memoryview for efficient access")
        .def("to_list", [](const std::vector<float>& v) {
            return py::cast(v);
        }, "Convert to Python list");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // Utility functions
    m.def("create_neuron_id", [](uint64_t value) {
        return NeuronId(value);
    }, py::arg("value"), "Create NeuronId from uint64_t");

    m.def("create_synapse_id", [](uint64_t value) {
        return SynapseId(value);
    }, py::arg("value"), "Create SynapseId from uint64_t");

    m.def("create_region_id", [](uint64_t value) {
        return RegionId(value);
    }, py::arg("value"), "Create RegionId from uint64_t");

    m.def("create_population_id", [](uint64_t value) {
        return PopulationId(value);
    }, py::arg("value"), "Create PopulationId from uint64_t");

    // Type conversion utilities
    m.def("to_float_array", [](const py::list& list) {
        return std::vector<float>(list.size());
    }, py::arg("list"), "Convert Python list to FloatArray");

    m.def("from_float_array", [](const std::vector<float>& arr) {
        py::list result;
        for (float val : arr) {
            result.append(val);
        }
        return result;
    }, py::arg("array"), "Convert FloatArray to Python list");

    // Configuration utilities
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        The Config class manages simulation parameters and settings.
        It provides methods for loading from files, command line, and
        programmatic setup. Configuration can be accessed and modified
        using key-value pairs.
        
        Examples:
            >>> config = Config()
            >>> config.loadFromFile("config.json")
            >>> config.set("brain.neuron_count", 1000)
            >>> config.set("plasticity.learning_rate", 0.01)
            >>> value = config.has("neuron_count") and config.getOr("neuron_count", 0)
            >>> config.saveToFile("my_config.json")
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file.
             
             Args:
                 filepath: Path to configuration file
                 
             Returns:
                 True if successful, False otherwise
                 
             Raises:
                 RuntimeError: If file cannot be read or parsed
                 
             Example:
                 >>> config.loadFromFile("my_config.json")
                 True
             )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            R"pbdoc(Load configuration from command line arguments.
            
            Args:
                argc: Argument count
                argv: Argument values
                
            Returns:
                True if successful, False otherwise
                
            Example:
                >>> config.loadFromArgs(3, ["program", "--neuron_count=1000"])
                True
            )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file.
             
             Args:
                 filepath: Path where to save configuration
                 
             Returns:
                 True if successful, False otherwise
                 
             Raises:
                 RuntimeError: If file cannot be written
                 
             Example:
                 >>> config.saveToFile("config.json")
                 True
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists.
             
             Args:
                 key: Configuration key to check
                 
             Returns:
                 True if key exists, False otherwise
                 
             Example:
                 >>> config.has("neuron_count")
                 True
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(Get all configuration keys.
             
             Returns:
                 List of all configuration keys
                 
             Example:
                 >>> keys = config.getKeys()
                 ['neuron_count', 'plasticity.learning_rate', ...]
             )pbdoc")
        .def("set", py::overload_cast<const std::string&, int>(&Config::set),
             py::arg("key"), py::arg("value"),
             R"pbdoc(Set an integer configuration value.
             
             Args:
                 key: Configuration key
                 value: Integer value
                 
             Example:
                 >>> config.set("neuron_count", 1000)
             )pbdoc")
        .def("set", py::overload_cast<const std::string&, double>(&Config::set),
             py::arg("key"), py::arg("value"),
             R"pbdoc(Set a floating point configuration value.
             
             Args:
                 key: Configuration key
                 value: Floating point value
                 
             Example:
                 >>> config.set("learning_rate", 0.01)
             )pbdoc")
        .def("set", py::overload_cast<const std::string&, float>(&Config::set),
             py::arg("key"), py::arg("value"),
             R"pbdoc(Set a floating point configuration value (alias).
             
             Args:
                 key: Configuration key
                 value: Floating point value
                 
             Example:
                 >>> config.set("plasticity_rate", 0.1)
             )pbdoc")
        .def("set", py::overload_cast<const std::string&, std::string>(&Config::set),
             py::arg("key"), py::arg("value"),
             R"pbdoc(Set a string configuration value.
             
             Args:
                 key: Configuration key
                 value: String value
                 
             Example:
                 >>> config.set("environment", "grid")
             )pbdoc")
        .def("set", py::overload_cast<const std::string&, bool>(&Config::set),
             py::arg("key"), py::arg("value"),
             R"pbdoc(Set a boolean configuration value.
             
             Args:
                 key: Configuration key
                 value: Boolean value
                 
             Example:
                 >>> config.set("enabled", True)
             )pbdoc")
        .def("getOr", py::overload_cast<const std::string&, int>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             R"pbdoc(Get configuration value or return default if not found.
             
             Args:
                 key: Configuration key
                 default_value: Default value if key not found
                 
             Returns:
                 Configuration value or default
                 
             Example:
                 >>> value = config.getOr("neuron_count", 1000)
                 1000
             )pbdoc")
        .def("getOr", py::overload_cast<const std::string&, double>(&Config::getOr),
             py::arg("key"), py::arg("default_value"))
        .def("getOr", py::overload_cast<const std::string&, float>(&Config::getOr),
             py::arg("key"), py::arg("default_value"))
        .def("getOr", py::overload_cast<const std::string&, std::string>(&Config::getOr),
             py::arg("key"), py::arg("default_value"))
        .def("getOr", py::overload_cast<const std::string&, bool>(&Config::getOr),
             py::arg("key"), py::arg("default_value"))
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries.
             
             Example:
                 >>> config.clear()
                 >>> len(config.getKeys())
                 0
             )pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary string of the configuration.
             
             Returns:
                 Summary string
                 
             Example:
                 >>> summary = config.summary()
                 Configuration with 5 parameters:
                 - neuron_count: 1000
                 - plasticity.learning_rate: 0.01
                 ...
             )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        })
        .def("__contains__", &Config::has,
             R"pbdoc(Check if a configuration key exists (using 'in' operator).
             
             Args:
                 key: Configuration key to check
                 
             Returns:
                 True if key exists, False otherwise
                 
             Example:
                 >>> "neuron_count" in config
                 True
             )pbdoc");

    // Configuration utilities
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(Create a default configuration.
    
    Returns:
        New Config instance with default values
        
    Example:
        >>> config = createDefaultConfig()
        >>> config.set("neuron_count", 1000)
    )pbdoc");

    m.def("from_dict", [](const py::dict& d) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        for (auto item : d) {
            std::string key = item.first.cast<std::string>();
            py::object value = item.second;
            
            if (value.is(py::int_())) {
                config->set(key, value.cast<int64_t>());
            } else if (value.is(py::float_())) {
                config->set(key, value.cast<double>());
            } else if (value.is(py::bool_())) {
                config->set(key, value.cast<bool>());
            } else if (value.is(py::str())) {
                config->set(key, value.cast<std::string>());
            }
        }
        return config;
    }, py::arg("dict"),
        R"pbdoc(Create a Config from a Python dictionary.
        
        Args:
            dict: Python dictionary with configuration parameters
            
        Returns:
            New Config instance
            
        Example:
            >>> config = from_dict({
            ...     "neuron_count": 1000,
            ...     "learning_rate": 0.01
            ... })
        )pbdoc");

    m.def("validate_config", [](const Config& config) -> py::list {
        py::list errors;
        // Basic validation
        if (!config.has("brain.neuron_count") || 
            config.getOr<int64_t>("brain.neuron_count", 0) <= 0) {
            errors.append("brain.neuron_count must be positive");
        }
        if (!config.has("simulation_timestep") || 
            config.getOr<double>("simulation_timestep", 0.0) <= 0.0) {
            errors.append("simulation_timestep must be positive");
        }
        return errors;
    }, py::arg("config"),
        R"pbdoc(Validate a configuration and return errors.
        
        Args:
            config: Config object to validate
            
        Returns:
            List of error messages, empty if valid
            
        Example:
            >>> errors = validate_config(config)
            >>> if errors:
            ...     for error in errors:
            ...         print(error)
        )pbdoc");

    // Serialization utilities
    m.def("save_config", [](const Config& config, const std::string& filepath) {
        return config.saveToFile(filepath);
    }, py::arg("config"), py::arg("filepath"),
        R"pbdoc(Save configuration to file.
        
        Args:
            config: Config object to save
            filepath: Path to save to
            
        Returns:
            True if successful, False otherwise
        )pbdoc");

    m.def("load_config", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }, py::arg("filepath"),
        R"pbdoc(Load configuration from file.
        
        Args:
            filepath: Path to configuration file
            
        Returns:
            New Config instance with loaded values
            
        Raises:
            RuntimeError: If file cannot be read or parsed
        )pbdoc");

} // end nlm::core namespace

} // namespace nlm
