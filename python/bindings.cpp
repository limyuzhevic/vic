#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <optional>

// Include all NLM headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/prediction/PredictionSystem.hpp"

namespace py = pybind11;
namespace nlm {

// Helper functions for Python integration
namespace py_helpers {
    
    // Type converter for ConfigValue to Python types
    py::object config_value_to_py(const ConfigValue& value) {
        return std::visit([](const auto& val) -> py::object {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return py::str(val);
            } else if constexpr (std::is_arithmetic_v<T>) {
                return py::float_(val);
            } else if constexpr (std::is_same_v<T, bool>) {
                return py::bool_(val);
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                return py::list(py::cast(val));
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                return py::list(py::cast(val));
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                return py::list(py::cast(val));
            } else {
                return py::none();
            }
        }, value);
    }
    
    // Type converter for Python to ConfigValue
    ConfigValue py_to_config_value(const py::object& obj) {
        if (obj.is_none()) {
            return std::string("");
        }
        
        if (py::isinstance<py::str>(obj)) {
            return obj.cast<std::string>();
        } else if (py::isinstance<py::int_>(obj)) {
            return obj.cast<int64_t>();
        } else if (py::isinstance<py::float_>(obj)) {
            return obj.cast<double>();
        } else if (py::isinstance<py::bool_>(obj)) {
            return obj.cast<bool>();
        } else if (py::isinstance<py::list>(obj) || py::isinstance<py::tuple>(obj)) {
            py::list list = obj.cast<py::list>();
            std::vector<double> vec;
            for (const auto& item : list) {
                if (py::isinstance<py::float_>(item)) {
                    vec.push_back(item.cast<double>());
                } else if (py::isinstance<py::int_>(item)) {
                    vec.push_back(static_cast<double>(item.cast<int64_t>()));
                }
            }
            return vec;
        }
        
        return std::string("");
    }
    
    // Exception handling for Python
    class PythonException : public std::runtime_error {
    public:
        PythonException(const std::string& msg) : std::runtime_error(msg) {}
    };
    
    // Python callback wrapper for configuration changes
    class PythonCallbackWrapper {
    public:
        py::object callback;
        
        PythonCallbackWrapper(const py::object& cb) : callback(cb) {}
        
        void operator()(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) {
            try {
                // Convert ConfigValue to Python object
                py::object oldPy = config_value_to_py(oldValue);
                py::object newPy = config_value_to_py(newValue);
                
                // Call Python callback
                callback(key, oldPy, newPy);
            } catch (const py::error_already_set& e) {
                // Let Python exception propagate
                throw;
            } catch (const std::exception& e) {
                throw PythonException(std::string("Callback error: ") + e.what());
            }
        }
    };
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (熙然) - Neural Learning Machine
        ======================================
        
        A neural simulation framework for artificial developmental systems.
        
        This Python binding provides access to the complete NLM brain architecture,
        including neurons, synapses, plasticity mechanisms, memory systems,
        and cognitive functions.
        
        Key Features:
        - Event-driven spiking neural networks
        - Multiple plasticity rules (STDP, Hebbian, reward-modulated)
        - Working and episodic memory systems
        - Neuromodulation (dopamine, curiosity, novelty)
        - Developmental stages and structural plasticity
        - Prediction and planning systems
        - Integration with simulated environments
        
        For detailed documentation, see the NLM project documentation.
    )pbdoc";

    // Register exception types
    py::register_exception<nlm::py_helpers::PythonException>(m, "NLMException");
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const nlm::py_helpers::PythonException& e) {
            py::set_error(PyExc_RuntimeError, e.what());
        } catch (const std::runtime_error& e) {
            py::set_error(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            py::set_error(PyExc_RuntimeError, e.what());
        }
    });

    // Strong type definitions with improved Python API
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
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enhanced neuron type with better Python integration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("is_excitatory", [](NeuronType type) { return type == NeuronType::Excitatory; })
        .def("is_inhibitory", [](NeuronType type) { return type == NeuronType::Inhibitory; })
        .def("is_sensory", [](NeuronType type) { return type == NeuronType::Sensory; })
        .def("is_motor", [](NeuronType type) { return type == NeuronType::Motor; });

    // Enhanced synapse type with additional methods
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def("is_excitatory", [](SynapseType type) { return type == SynapseType::Excitatory; })
        .def("is_inhibitory", [](SynapseType type) { return type == SynapseType::Inhibitory; })
        .def("is_modulatory", [](SynapseType type) { return type == SynapseType::Modulatory; });

    // Enhanced developmental stage with better Python API
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def("is_developing", [](DevelopmentalStage stage) { 
            return stage == DevelopmentalStage::Initial || 
                   stage == DevelopmentalStage::CriticalPeriod || 
                   stage == DevelopmentalStage::Maturation; 
        })
        .def("is_mature", [](DevelopmentalStage stage) { 
            return stage == DevelopmentalStage::Adult; 
        });

    // Enhanced firing state with helper methods
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .def("is_active", [](FiringState state) { return state == FiringState::Active; })
        .def("is_refractory", [](FiringState state) { return state == FiringState::Refractory; })
        .def("is_resting", [](FiringState state) { return state == FiringState::Resting; });

    // Enhanced action type with additional functionality
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
        .export_values()
        .def("is_movement", [](ActionType type) {
            return type == ActionType::MoveForward || type == ActionType::MoveBackward ||
                   type == ActionType::MoveLeft || type == ActionType::MoveRight;
        })
        .def("is_rotation", [](ActionType type) {
            return type == ActionType::TurnLeft || type == ActionType::TurnRight;
        })
        .def("is_interaction", [](ActionType type) {
            return type == ActionType::Interact || type == ActionType::Eat || type == ActionType::Drink || type == ActionType::Rest;
        });

    // Enhanced motor command with additional methods
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values()
        .def("is_movement", [](MotorCommand cmd) {
            return cmd == MotorCommand::MoveForward || cmd == MotorCommand::MoveBackward;
        })
        .def("is_rotation", [](MotorCommand cmd) {
            return cmd == MotorCommand::TurnLeft || cmd == MotorCommand::TurnRight;
        });

    // Enhanced world object type
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .def("is_consumable", [](WorldObjectType type) { return type == WorldObjectType::Resource; })
        .def("is_hazard", [](WorldObjectType type) { return type == WorldObjectType::Hazard; })
        .def("is_passable", [](WorldObjectType type) { return type == WorldObjectType::Empty; });

    // Enhanced Config class with better Python integration
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Advanced configuration manager for NLM)

        The Config class provides a unified interface for managing all configuration
        parameters of the NLM system. It supports multiple input formats (JSON, YAML,
        command-line, environment variables) and provides validation, callbacks, and
        serialization capabilities.
        
        Configuration keys include:
        - random_seed: Random seed for reproducible experiments
        - simulation_timestep: Simulation timestep in seconds
        - neuron_count: Number of neurons in the brain
        - region_count: Number of brain regions
        - connection_probability: Connection probability for random connectivity
        - And many more specific to different subsystems...
        
        Configuration values can be set using:
        - loadFromFile(): Load from JSON or YAML files
        - loadFromEnvironment(): Load from environment variables
        - loadFromArgs(): Load from Python argument lists
        - Direct assignment: config["key"] = value
        - Direct method calls: config.set("key", value)
        
        Key features:
        - Type-safe value access with automatic conversion
        - Configuration validation with detailed error messages
        - Change callbacks for reactive programming
        - Group-based configuration organization
        - Profile support for saving/loading configurations
        - JSON/YAML serialization
        - Environment variable integration
        - Command-line argument parsing
    )pbdoc")
        .def(py::init<>())
        
        // Enhanced loading methods
        .def("loadFromFile", py::overload_cast<bool(const std::string&, bool)>(&Config::loadFromFile),
             py::arg("filepath"), py::arg("validate_schema") = true,
             "Load configuration from a JSON or YAML file")
        .def("loadFromYaml", &Config::loadFromYaml, py::arg("filepath"), py::arg("validate_schema") = true,
             "Load configuration from a YAML file")
        .def("loadFromEnvironment", &Config::loadFromEnvironment,
             py::arg("prefix") = "NLM_", py::arg("validate_keys") = true,
             "Load configuration from environment variables")
        .def("loadFromArgs", py::overload_cast<bool(int, char**)>(&Config::loadFromArgs),
             py::arg("argc"), py::arg("argv"),
             "Load configuration from C-style command line arguments")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size() + 1);
            argv.push_back(const_cast<char*>("program"));
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
             "Load configuration from Python argument list (more convenient than C-style)")
        
        // Enhanced saving methods
        .def("saveToFile", py::overload_cast<bool(const std::string&, bool) const>(&Config::saveToFile),
             py::arg("filepath"), py::arg("pretty_print") = false,
             "Save configuration to a JSON or YAML file")
        .def("saveToJson", &Config::saveToJson, py::arg("filepath"), py::arg("pretty_print") = false,
             "Save configuration to a JSON file")
        .def("saveToYaml", &Config::saveToYaml, py::arg("filepath"), py::arg("pretty_print") = false,
             "Save configuration to a YAML file")
        
        // Enhanced value access with Pythonic interface
        .def("get", py::overload_cast<const std::string&>(&Config::getKeys),
             py::arg("key"),
             "Get a configuration value (returns None if key not found)")
        .def("get_or", py::overload_cast<const std::string&, const ConfigValue&>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get a configuration value or default if not found")
        .def("try_get", [](Config& self, const std::string& key, py::object default_value) {
            ConfigValue out;
            if (self.tryGet(key, out)) {
                return py_helpers::config_value_to_py(out);
            }
            return default_value;
        }, py::arg("key"), py::arg("default_value") = py::none(),
             "Try to get a configuration value, returns None if not found")
        
        // Enhanced setting methods
        .def("set", py::overload_cast<const std::string&, const ConfigValue&, ConfigSource, const std::string&>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             py::arg("description") = "",
             "Set a configuration value with source and description")
        .def("set", py::overload_cast<const std::string&, const std::string&, ConfigSource, const std::string&>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             py::arg("description") = "",
             "Set a string configuration value")
        .def("set", py::overload_cast<const std::string&, int, ConfigSource, const std::string&>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             py::arg("description") = "",
             "Set an integer configuration value")
        .def("set", py::overload_cast<const std::string&, int64_t, ConfigSource, const std::string&>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             py::arg("description") = "",
             "Set an int64_t configuration value")
        .def("set", py::overload_cast<const std::string&, double, ConfigSource, const std::string&>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             py::arg("description") = "",
             "Set a floating-point configuration value")
        .def("set", py::overload_cast<const std::string&, bool, ConfigSource, const std::string&>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             py::arg("description") = "",
             "Set a boolean configuration value")
        
        // Enhanced management methods
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("keys", &Config::getKeys,
             "Get all configuration keys")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys (alias for keys)")
        .def("sorted_keys", &Config::getSortedKeys,
             "Get configuration keys in sorted order")
        
        // Configuration introspection
        .def("get_source", &Config::getSource, py::arg("key"),
             "Get the source of a configuration value")
        .def("get_description", &Config::getDescription, py::arg("key"),
             "Get the description of a configuration value")
        .def("get_timestamp", &Config::getTimestamp, py::arg("key"),
             "Get the timestamp when a configuration value was set")
        .def("source_string", &Config::getSourceString, py::arg("source"),
             "Get a human-readable string representation of a config source")
        
        // Validation and groups
        .def("validate", &Config::validate,
             "Validate all configuration values")
        .def("validate_key", &Config::validateKey, py::arg("key"),
             "Validate a configuration key")
        .def("validate_value", &Config::validateValue, py::arg("key"), py::arg("value"),
             "Validate a configuration value")
        .def("begin_group", &Config::beginGroup, py::arg("group_name"),
             "Begin a configuration group")
        .def("end_group", &Config::end_group,
             "End the current configuration group")
        .def("in_group", &Config::in_group,
             "Check if currently in a configuration group")
        .def("get_current_group", &Config::get_current_group,
             "Get the name of the current configuration group")
        
        // Callback support
        .def("add_change_callback", [](Config& self, const py::object& callback) {
            auto wrapper = std::make_shared<nlm::py_helpers::PythonCallbackWrapper>(callback);
            self.add_change_callback([wrapper](const std::string& key, const ConfigValue& old_val, const ConfigValue& new_val) {
                (*wrapper)(key, old_val, new_val);
            });
        }, py::arg("callback"),
             "Add a Python callback for configuration changes")
        .def("remove_change_callback", [](Config& self, const py::object& callback) {
            auto wrapper = std::make_shared<nlm::py_helpers::PythonCallbackWrapper>(callback);
            self.remove_change_callback([wrapper](const std::string& key, const ConfigValue& old_val, const ConfigValue& new_val) {
                (*wrapper)(key, old_val, new_val);
            });
        }, py::arg("callback"),
             "Remove a Python callback for configuration changes")
        .def("clear_change_callbacks", &Config::clear_change_callbacks,
             "Clear all configuration change callbacks")
        
        // Profile support
        .def("save_profile", &Config::save_profile,
             py::arg("name"), py::arg("directory") = "profiles",
             "Save configuration to a profile file")
        .def("load_profile", &Config::load_profile,
             py::arg("name"), py::arg("directory") = "profiles",
             "Load configuration from a profile file")
        .def("get_available_profiles", &Config::get_available_profiles,
             "Get a list of available profile files")
        
        // Clone and merge
        .def("clone", &Config::clone,
             "Create a copy of this configuration")
        .def("merge", &Config::merge, py::arg("other"), py::arg("overwrite") = false,
             "Merge another configuration into this one")
        
        // Validation rules
        .def("validate_rules", [](Config& self, const std::vector<std::function<bool(const ConfigValue&)>>& rules) {
            std::vector<Config::ValidationRule> validationRules;
            for (size_t i = 0; i < rules.size(); ++i) {
                validationRules.push_back(Config::ValidationRule{std::to_string(i), rules[i], "Validation rule failed"});
            }
            return self.validate_rules(validationRules);
        }, py::arg("rules"),
             "Validate configuration against a list of Python validation functions")
        
        // Summary and representation
        .def("summary", py::overload_cast<bool, bool>(&Config::summary),
             py::arg("include_sources") = false, py::arg("include_timestamps") = false,
             "Get a summary of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // Enhanced sensory input classes with Pythonic interface
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for all sensory input)

        SensoryInput represents data from the environment that can be perceived by the agent.
        This includes visual, auditory, tactile, and internal signals.
    )pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput: type=" + std::to_string(static_cast<int>(input.getType())) + 
                   ", dims=" + std::to_string(input.getDimensions()) + ">";
        });

    // Enhanced Vision class with Pythonic interface
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Visual sensory input from the environment)

        Vision represents 2D or 3D visual input with multiple channels (e.g., RGB).
        This is typically used by agents to perceive their visual environment.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("configure", [](Vision& self, size_t width, size_t height, size_t channels) {
            self.setWidth(height);
            self.setHeight(height);
            self.setChannels(channels);
        }, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
             "Configure vision dimensions")
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set visual data as a flat array")
        .def("set_data_2d", [](Vision& self, const std::vector<std::vector<std::vector<float>>>& data) {
            size_t width = data.size();
            size_t height = data.empty() ? 0 : data[0].size();
            size_t channels = data.empty() || data[0].empty() ? 0 : data[0][0].size();
            self.configure(width, height, channels);
            
            std::vector<float> flatData;
            flatData.reserve(width * height * channels);
            for (size_t y = 0; y < height; ++y) {
                for (size_t x = 0; x < width; ++x) {
                    for (size_t c = 0; c < channels; ++c) {
                        flatData.push_back(data[y][x][c]);
                    }
                }
            }
            self.setData(flatData);
        }, py::arg("data"), "Set 2D visual data in array format")
        .def("get_width", &Vision::getWidth, "Get image width")
        .def("get_height", &Vision::getHeight, "Get image height")
        .def("get_channels", &Vision::getChannels, "Get number of color channels")
        .def("get_shape", [](const Vision& self) {
            return py::make_tuple(self.getWidth(), self.getHeight(), self.getChannels());
        }, "Get vision shape as (width, height, channels)")
        .def("normalize", &Vision::normalize, "Normalize pixel values to [0, 1]")
        .def("get_pixel", [](const Vision& self, size_t x, size_t y, size_t c = 0) {
            if (x >= self.getWidth() || y >= self.getHeight() || c >= self.getChannels()) {
                throw py::index_error("Pixel coordinates out of bounds");
            }
            return self.getData()[y * self.getWidth() * self.getChannels() + x * self.getChannels() + c];
        }, py::arg("x"), py::arg("y"), py::arg("c") = 0, "Get pixel value at (x, y)")
        .def("get_pixel_2d", [](const Vision& self, size_t x, size_t y) {
            std::vector<float> pixel;
            if (x < self.getWidth() && y < self.getHeight()) {
                size_t base = y * self.getWidth() * self.getChannels() + x * self.getChannels();
                for (size_t c = 0; c < self.getChannels(); ++c) {
                    pixel.push_back(self.getData()[base + c]);
                }
            }
            return pixel;
        }, py::arg("x"), py::arg("y"), "Get pixel as [R, G, B] tuple")
        .def("get_center_region", &Vision::getCenterRegion, "Get center region of vision")
        .def("__repr__", [](const Vision& v) {
            return "<Vision: " + std::to_string(v.getWidth()) + "x" + std::to_string(v.getHeight()) +
                   "x" + std::to_string(v.getChannels()) + " channels>";
        });

    // Enhanced Audio class with Pythonic interface
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Auditory sensory input)

        Audio represents sound data with sample rate and number of samples.
        This is used by agents to perceive audio in their environment.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"),
             "Set sample rate in Hz")
        .def("get_sample_rate", &Audio::getSampleRate, "Get sample rate in Hz")
        .def("get_num_samples", &Audio::getNumSamples, "Get number of audio samples")
        .def("get_duration", &Audio::getDuration, "Get audio duration in seconds")
        .def("normalize", &Audio::normalize, "Normalize audio amplitude")
        .def("get_frequency_content", &Audio::getFrequencyContent, "Get frequency analysis")
        .def("get_envelope", &Audio::getEnvelope, "Get amplitude envelope")
        .def("to_mono", &Audio::toMono, "Convert stereo to mono")
        .def("__repr__", [](const Audio& a) {
            return "<Audio: " + std::to_string(a.getSampleRate()) + " Hz, " +
                   std::to_string(a.getNumSamples()) + " samples>";
        });

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal physiological signals)

        InternalSignals represents internal states such as hunger, fatigue, stress, etc.
        This allows the agent to perceive its own internal state.
    )pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("get_signal_count", &InternalSignals::getSignalCount,
             "Get number of signals")
        .def("get_signal_sum", &InternalSignals::getSignalSum,
             "Get sum of all signals")
        .def("get_signal_mean", &InternalSignals::getSignalMean,
             "Get mean of all signals")
        .def("get_signal_variance", &InternalSignals::getSignalVariance,
             "Get variance of all signals")
        .def("get_signal_max", &InternalSignals::getSignalMax,
             "Get maximum signal value")
        .def("get_signal_min", &InternalSignals::getSignalMin,
             "Get minimum signal value")
        .def("normalize", &InternalSignals::normalize,
             "Normalize signal values")
        .def("clear_signals", &InternalSignals::clearSignals,
             "Clear all signals")
        .def("get_all_signals", &InternalSignals::getAllSignals,
             "Get all signals as a vector")
        .def("__repr__", [](const InternalSignals& sig) {
            return "<InternalSignals: " + std::to_string(sig.getSignalCount()) + " signals>";
        });

    // Enhanced Action class with Pythonic interface
    py::class_<Action>(m, "Action", R"pbdoc(Motor action for the agent to execute)

        Action represents a high-level action that an agent can take in the environment.
        This includes movements, interactions, and other goal-directed behaviors.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType, "Get action type")
        .def("set_type", &Action::setType, py::arg("type"), "Set action type")
        .def("get_parameters", &Action::getParameters, "Get action parameters")
        .def("set_parameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("get_parameter", [](Action& self, size_t index) {
            if (index < self.getParameters().size()) {
                return self.getParameters()[index];
            }
            throw py::index_error("Parameter index out of bounds");
        }, py::arg("index"), "Get a specific parameter value")
        .def("set_parameter", [](Action& self, size_t index, float value) {
            if (index >= self.getParameters().size()) {
                self.getParameters().resize(index + 1, 0.0f);
            }
            self.getParameters()[index] = value;
        }, py::arg("index"), py::arg("value"), "Set a specific parameter value")
        .def("get_name", &Action::getName, "Get action name")
        .def("add_parameter", &Action::addParameter, py::arg("value"), "Add a parameter")
        .def("clear_parameters", &Action::clearParameters, "Clear all parameters")
        .def("get_parameter_count", &Action::getParameterCount, "Get number of parameters")
        .def("is_movement", &Action::isMovement, "Check if action is a movement")
        .def("is_interaction", &Action::isInteraction, "Check if action is an interaction")
        .def("is_rest", &Action::isRest, "Check if action is rest")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("__repr__", [](const Action& a) {
            std::string typeName;
            switch (a.getType()) {
                case ActionType::MoveForward: typeName = "MoveForward"; break;
                case ActionType::MoveBackward: typeName = "MoveBackward"; break;
                case ActionType::MoveLeft: typeName = "MoveLeft"; break;
                case ActionType::MoveRight: typeName = "MoveRight"; break;
                case ActionType::TurnLeft: typeName = "TurnLeft"; break;
                case ActionType::TurnRight: typeName = "TurnRight"; break;
                case ActionType::Look: typeName = "Look"; break;
                case ActionType::LookUp: typeName = "LookUp"; break;
                case ActionType::LookDown: typeName = "LookDown"; break;
                case ActionType::Interact: typeName = "Interact"; break;
                case ActionType::Eat: typeName = "Eat"; break;
                case ActionType::Drink: typeName = "Drink"; break;
                case ActionType::Rest: typeName = "Rest"; break;
                case ActionType::Wait: typeName = "Wait"; break;
                case ActionType::Custom: typeName = "Custom"; break;
            }
            return "<Action: " + typeName + ", " + std::to_string(a.getParameterCount()) + " params>";
        });

    // Enhanced world object with Pythonic interface
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(Object in the simulated world)

        WorldObject represents an object that exists in the simulated world, such as
        resources, hazards, walls, or markers. Objects have position, type, and properties.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "X coordinate")
        .def_readwrite("y", &WorldObject::y, "Y coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Whether object is active")
        .def("is_resource", &WorldObject::isResource, "Check if object is a resource")
        .def("is_hazard", &WorldObject::isHazard, "Check if object is a hazard")
        .def("is_wall", &WorldObject::isWall, "Check if object is a wall")
        .def("is_marker", &WorldObject::isMarker, "Check if object is a marker")
        .def("get_distance_to", &WorldObject::getDistanceTo, "Get distance to another object")
        .def("is_colliding_with", &WorldObject::isCollidingWith, "Check collision with another object")
        .def("contains_point", &WorldObject::containsPoint, "Check if point is inside object")
        .def("__repr__", [](const WorldObject& obj) {
            std::string typeName;
            switch (obj.type) {
                case WorldObjectType::Empty: typeName = "Empty"; break;
                case WorldObjectType::Resource: typeName = "Resource"; break;
                case WorldObjectType::Hazard: typeName = "Hazard"; break;
                case WorldObjectType::Wall: typeName = "Wall"; break;
                case WorldObjectType::Marker: typeName = "Marker"; break;
            }
            return "<WorldObject: " + typeName + " at (" + std::to_string(obj.x) + ", " + 
                   std::to_string(obj.y) + "), radius=" + std::to_string(obj.radius) + ">";
        });

    // Enhanced AgentBody with Pythonic interface
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent's physical body state)

        AgentBody represents the physical state of an agent in the world, including
        position, orientation, velocity, energy, health, and other physiological
        parameters. This allows agents to perceive and interact with their physical
        representation in the environment.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level")
        .def_readwrite("health", &AgentBody::health, "Health level")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset body to initial state")
        .def("get_position", &AgentBody::getPosition, "Get position as (x, y)")
        .def("set_position", &AgentBody::setPosition, "Set position", py::arg("x"), py::arg("y"))
        .def("get_velocity", &AgentBody::getVelocity, "Get velocity as (vx, vy)")
        .def("set_velocity", &AgentBody::setVelocity, "Set velocity", py::arg("vx"), py::arg("vy"))
        .def("get_speed", &AgentBody::getSpeed, "Get speed")\n        .def("get_angular_speed", &AgentBody::getAngularSpeed, "Get angular speed")
        .def("set_angular_speed", &AgentBody::setAngularSpeed, "Set angular speed", py::arg("speed"))
        .def("move", [](AgentBody& self, float distance) {
            float radians = self.orientation;
            self.x += distance * std::cos(radians);
            self.y += distance * std::sin(radians);
            self.isMoving = true;
            self.lastActionTime = 0.0f;
        }, py::arg("distance"), "Move forward by distance")
        .def("rotate", [](AgentBody& self, float angle) {
            self.orientation += angle;
            self.isTurning = true;
            self.lastActionTime = 0.0f;
        }, py::arg("angle"), "Rotate by angle radians")
        .def("update", [](AgentBody& self, float timestep) {
            // Simple physics integration
            self.x += self.velocityX * timestep;
            self.y += self.velocityY * timestep;
            self.velocityX *= 0.99f; // Simple damping
            self.velocityY *= 0.99f;
            self.lastActionTime += timestep;
        }, py::arg("timestep"), "Update physics")
        .def("is_frozen", &AgentBody::isFrozen, "Check if agent is frozen (velocity zero)")
        .def("distance_to", &AgentBody::distanceTo, "Calculate distance to another body")
        .def("get_angle_to", &AgentBody::getAngleTo, "Get angle to another body")
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: pos(" + std::to_string(body.x) + ", " + std::to_string(body.y) +
                   "), energy=" + std::to_string(body.energy) + ", health=" + std::to_string(body.health) + ">";
        });

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Result of an action execution)

        ActionResult contains the outcome of an agent's action, including reward,
        success status, and descriptive message. This is used by the agent to learn
        from its experiences and update its internal models.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward received")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Descriptive message")
        .def("is_good_result", &ActionResult::isGoodResult, "Check if result is good (success and positive reward)")
        .def("is_bad_result", &ActionResult::isBadResult, "Check if result is bad (failure or negative reward)")
        .def("__repr__", [](const ActionResult& result) {
            std::string status = result.success ? "SUCCESS" : "FAILED";
            return "<ActionResult: " + status + ", reward=" + std::to_string(result.reward) + ">";
        });

    // Enhanced SensoryPercept class with Pythonic interface
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory perception data from the world)

        SensoryPercept combines all sensory inputs (vision, touch, internal signals, audio)
        into a single object that can be processed by the agent. This represents what the
        agent perceives about its environment and its own internal state.
    )pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision, "Get vision input")
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"), "Set vision input")
        .def("get_vision_width", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("get_vision_height", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("get_touch", &SensoryPercept::getTouch, "Get touch input")
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch input")
        .def("get_internal", &SensoryPercept::getInternal, "Get internal signals")
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("get_proprioception", &SensoryPercept::getProprioception, "Get proprioception")
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception")
        .def("get_audio", &SensoryPercept::getAudio, "Get audio input")
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio input")
        .def("get_all_signals", &SensoryPercept::getAllSignals, "Get all signals as a dictionary")
        .def("get_timestamp", &SensoryPercept::getTimestamp, "Get perception timestamp")
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set perception timestamp")
        .def("has_vision", &SensoryPercept::hasVision, "Check if vision data is present")
        .def("has_audio", &SensoryPercept::hasAudio, "Check if audio data is present")
        .def("has_internal", &SensoryPercept::hasInternal, "Check if internal data is present")
        .def("get_sensory_summary", [](const SensoryPercept& percept) {
            py::dict summary;
            if (percept.hasVision()) {
                py::object vision_data = py::cast(percept.getVision());
                summary["vision"] = vision_data;
            }
            if (percept.hasAudio()) {
                py::object audio_data = py::cast(percept.getAudio());
                summary["audio"] = audio_data;
            }
            if (percept.hasInternal()) {
                py::object internal_data = py::cast(percept.getInternal());
                summary["internal"] = internal_data;
            }
            return summary;
        }, "Get summary of all sensory data as a dictionary")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: vision=" + std::to_string(percept.getVisionWidth()) + "x" + 
                   std::to_string(percept.getVisionHeight()) + ", audio=" + 
                   std::to_string(percept.getAudio().getNumSamples()) + " samples>";
        });

    // Enhanced SimpleWorld class with Pythonic interface
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(2D simulated world for NLM agents)

        SimpleWorld represents a 2D environment in which NLM agents operate. It contains
        the world geometry, objects, and handles the execution of agent actions and
        their effects on the environment. This is a simplified world for testing and
        development, but still captures the essential dynamics of agent-environment
        interaction.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"),
             "Set agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world physics and dynamics")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"),
             "Apply a motor command and return result")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             "Get current sensory percept from the world")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             "Get agent body state")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in the world")
        .def("get_width", &SimpleWorld::getWidth, "Get world width")
        .def("get_height", &SimpleWorld::getHeight, "Get world height")
        .def("get_vision_width", &SimpleWorld::getVisionWidth, "Get agent's vision width")
        .def("get_vision_height", &SimpleWorld::getVisionHeight, "Get agent's vision height")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum agent energy")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("get_random_seed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("get_agent_position", &SimpleWorld::getAgentPosition, "Get agent position")
        .def("set_agent_position", &SimpleWorld::setAgentPosition, "Set agent position", py::arg("x"), py::arg("y"))
        .def("get_objects_in_radius", &SimpleWorld::getObjectsInRadius, "Get objects within radius")
        .def("find_nearest_object", &SimpleWorld::findNearestObject, "Find nearest object of specific type")
        .def("get_world_snapshot", &SimpleWorld::getWorldSnapshot, "Get world state snapshot")
        .def("restore_world_snapshot", &SimpleWorld::restoreWorldSnapshot, "Restore world state from snapshot")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" + 
                   std::to_string(world.getHeight()) + ", vision=" + std::to_string(world.getVisionWidth()) + 
                   "x" + std::to_string(world.getVisionHeight()) + ">";
        });

    // Enhanced Brain class with Pythonic interface
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain)

        Brain is the core neural computation engine in NLM. It implements spiking neural
        networks with multiple plasticity rules, memory systems, and cognitive functions.
        The brain processes sensory input, maintains internal state, and produces motor
        output through complex neural dynamics.

        The brain operates through a closed-loop architecture:
        1. Receive sensory input
        2. Process through neural dynamics
        3. Access memory systems
        4. Apply neuromodulation
        5. Select actions
        6. Produce motor output

        Key features:
        - Event-driven spiking neuron dynamics
        - Multiple plasticity mechanisms (STDP, Hebbian, reward-modulated)
        - Working and episodic memory systems
        - Neuromodulation (dopamine, curiosity, novelty)
        - Developmental stages and structural plasticity
        - Prediction and planning systems
        - Energy-based attention mechanisms
        - Modular architecture for extensibility
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), "Initialize with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", py::overload_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("current_step"),
             "Perform a simulation step")
        .def("step", py::overload_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("current_step"), py::arg("current_time"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", py::overload_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount, "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds, "Get all region IDs")
        .def("get_regions", &Brain::getRegions,
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
             py::arg("stage"), "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status")

        // Enhanced convenience methods
        .def("get_region", [](Brain& brain, int id) {
            return brain.getRegion(RegionId(static_cast<uint64_t>(id)));
        }, py::arg("id"), "Get region by integer ID (convenience method)")
        .def("get_region_ids", [](Brain& brain) {
            auto ids = brain.getRegionIds();
            std::vector<int> intIds;
            for (const auto& id : ids) {
                intIds.push_back(static_cast<int>(id.value));
            }
            return intIds;
        }, "Get region IDs as integers (convenience method)")
        .def("step", [](Brain& brain, int current_step, float current_time = 0.0f) {
            brain.step(static_cast<SimulationStep>(current_step), static_cast<Timestamp>(current_time));
        }, py::arg("current_step"), py::arg("current_time") = 0.0f,
             "Convenience method for stepping with float time")
        .def("inject_current_to_all", [](Brain& brain, float current) {
            brain.injectCurrentToNeurons(NeuronType::Internal, current);
        }, py::arg("current"), "Inject current to all internal neurons")
        .def("has_region", [](Brain& brain, int region_id) {
            return brain.getRegion(RegionId(static_cast<uint64_t>(region_id))) != nullptr;
        }, py::arg("region_id"), "Check if region exists")
        .def("get_spike_statistics", &Brain::getTotalSpikeCount,
             "Get basic spike statistics")
        .def("get_neuron_statistics", [](Brain& brain) {
            py::dict stats;
            stats["total"] = brain.getTotalNeuronCount();
            stats["active"] = brain.getActiveNeuronCount();
            stats["firing"] = brain.getFiringNeuronCount();
            stats["average_firing_rate"] = brain.getAverageFiringRate();
            return stats;
        }, "Get comprehensive neuron statistics")
        .def("get_synapse_statistics", [](Brain& brain) {
            py::dict stats;
            stats["total"] = brain.getTotalSynapseCount();
            stats["excitatory_inhibitory_ratio"] = brain.getExcitationInhibitionRatio();
            return stats;
        }, "Get basic synapse statistics")
        .def("simulate_episode", [](Brain& brain, const py::object& sensory_input, int steps, float dt = 0.001f) {
            if (!sensory_input.is_none()) {
                auto input = sensory_input.cast<std::shared_ptr<SensoryInput>>();
                brain.receiveSensoryInput(*input);
            }
            
            for (int step = 0; step < steps; ++step) {
                brain.step(step, step * dt);
            }
            
            return brain.produceAction();
        }, py::arg("sensory_input") = py::none(), py::arg("steps") = 100, py::arg("dt") = 0.001f,
             "Run a complete episode simulation")
        .def("export_brain_state", [](Brain& brain, const std::string& filepath) {
            return brain.save(filepath);
        }, py::arg("filepath"), "Export brain state to file")
        .def("import_brain_state", [](Brain& brain, const std::string& filepath) {
            return brain.load(filepath);
        }, py::arg("filepath"), "Import brain state from file")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + " neurons, " +
                   std::to_string(brain.getTotalSynapseCount()) + " synapses>";
        });

    // Enhanced AgentBrain class with Pythonic interface
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent-brain interface for NLM)

        AgentBrain serves as the interface between the NLM brain and the external world.
        It coordinates sensory input processing, motor output decoding, and learning
        through reward-modulated plasticity. This class enables agents to interact
        with environments and learn from their experiences.

        The AgentBrain implements the agent's control loop:
        1. Receive sensory percept from environment
        2. Process sensory input through neural network
        3. Decode neural activity into motor commands
        4. Execute actions in environment
        5. Receive reward signals
        6. Apply reward-modulated learning
        7. Update developmental state
        8. Repeat

        Key features:
        - Seamless integration with world environments
        - Adaptive action selection based on neural activity
        - Reward-based learning and neuromodulation
        - Developmental progression and structural plasticity
        - Curiosity-driven exploration
        - Novelty detection and surprise
        - Predictive coding and error signals
        - Energy-based attention and resource management
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Initialize with brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             "Get the underlying brain")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")

        // Enhanced convenience methods
        .def("act", [](AgentBrain& agent, const py::object& percept, float reward = 0.0f, bool success = true) {
            if (!percept.is_none()) {
                auto percept_ptr = percept.cast<std::shared_ptr<SensoryPercept>>();
                agent.processSensoryInput(*percept_ptr);
            }
            auto action = agent.decodeMotorCommand();
            agent.applyRewardModulation(reward, 0.0f); // Default predicted reward
            return action;
        }, py::arg("percept") = py::none(), py::arg("reward") = 0.0f, py::arg("success") = true,
             "Perform complete action cycle (process percept, decode action, apply reward)")
        .def("get_brain_state", [](AgentBrain& agent) {
            py::dict state;
            auto brain = agent.getBrain();
            state["total_neuron_count"] = brain->getTotalNeuronCount();
            state["total_synapse_count"] = brain->getTotalSynapseCount();
            state["total_spike_count"] = brain->getTotalSpikeCount();
            state["average_firing_rate"] = brain->getAverageFiringRate();
            state["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
            state["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
            return state;
        }, "Get comprehensive brain state as dictionary")
        .def("get_neuromodulation_state", [](AgentBrain& agent) {
            py::dict state;
            state["neuromodulation_level"] = agent.getNeuromodulationLevel();
            state["curiosity_level"] = agent.getCuriosityLevel();
            state["novelty_level"] = agent.getNoveltyLevel();
            state["prediction_error"] = agent.getPredictionError();
            return state;
        }, "Get neuromodulation state")
        .def("update_all_systems", [](AgentBrain& agent, const py::object& percept, int timestep, float reward = 0.0f) {
            if (!percept.is_none()) {
                auto percept_ptr = percept.cast<std::shared_ptr<SensoryPercept>>();
                agent.processSensoryInput(*percept_ptr);
            }
            
            agent.decodeMotorCommand();
            agent.applyRewardModulation(reward, 0.0f);
            agent.updateDevelopment(timestep);
        }, py::arg("percept") = py::none(), py::arg("timestep") = 1, py::arg("reward") = 0.0f,
             "Update all agent systems in one call")
        .def("is_agent_active", &AgentBrain::isDevelopmentEnabled,
             "Check if agent is currently developing/learning")
        .def("get_agent_status", [](AgentBrain& agent) {
            py::dict status;
            auto brain = agent.getBrain();
            status["neurons"] = brain->getTotalNeuronCount();
            status["synapses"] = brain->getTotalSynapseCount();
            status["spikes"] = brain->getTotalSpikeCount();
            status["development_enabled"] = agent.isDevelopmentEnabled();
            status["reward_modulation_enabled"] = agent.isRewardModulationEnabled();
            status["structural_plasticity_enabled"] = agent.isStructuralPlasticityEnabled();
            status["curiosity_enabled"] = agent.isCuriosityEnabled();
            status["developmental_stage"] = static_cast<int>(agent.getDevelopmentalStage());
            return status;
        }, "Get comprehensive agent status")
        .def("reset_for_episode", &AgentBrain::reset,
             "Reset agent state for new episode")
        .def("enable_all_systems", [](AgentBrain& agent) {
            agent.enableRewardModulation(true);
            agent.enableStructuralPlasticity(true);
            agent.enableDevelopment(true);
            agent.enableCuriosity(true);
        }, "Enable all plasticity and development systems")
        .def("disable_all_systems", [](AgentBrain& agent) {
            agent.enableRewardModulation(false);
            agent.enableStructuralPlasticity(false);
            agent.enableDevelopment(false);
            agent.enableCuriosity(false);
        }, "Disable all plasticity and development systems")
        .def("__repr__", [](const AgentBrain& agent) {
            auto brain = agent.getBrain();
            return "<AgentBrain: " + std::to_string(brain->getTotalNeuronCount()) + " neurons, " +
                   std::to_string(brain->getTotalSynapseCount()) + " synapses>";
        });

    // Factory functions for convenience
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration object");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Example and utility functions
    m.def("run_example_simulation", []() {
        // Create a simple example simulation
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("random_seed", 42);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create some simple input
        auto vision = std::make_shared<Vision>(10, 10, 3);
        std::vector<float> data(10 * 10 * 3, 0.5f);
        vision->setData(data);
        
        auto percept = std::make_shared<SensoryPercept>();
        percept->setVision(vision);
        
        // Run a few steps
        for (int step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto action = brain->produceAction();
        
        return py::dict(
            "brain", brain,
            "percept", percept,
            "action", action,
            "steps", 50
        );
    }, "Run a simple example simulation and return results");

    m.def("get_version_info", []() {
        py::dict info;
        info["version"] = "2.0.0";
        info["description"] = "NLM Python Bindings - Enhanced Edition";
        info["features"] = py::list({
            "Enhanced Config with JSON/YAML support",
            "Pythonic API with type safety",
            "Memory-efficient data structures",
            "Advanced neural simulation",
            "Neuromodulation support",
            "Development system",
            "Memory systems",
            "Predictive coding",
            "Agent-environment integration"
        });
        return info;
    }, "Get version and feature information");

    // Error classes
    py::class_<std::runtime_error>(m, "RuntimeError")
        .def(py::init<const std::string&>());

} // namespace nlm

// Alias for backward compatibility
PYBIND11_MODULE(pynlm, m) {
    nlm::PYBIND11_MODULE(m);
}

} // namespace nlm

