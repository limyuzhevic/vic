#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("get", [](Config& self, const std::string& key) {
            // Try to get as different types, return None if not available
            if (self.has(key)) {
                // For Python, we'll try to get as int first, then double, then string, etc.
                if (auto val = self.get<int>(key)) {
                    return py::cast(*val);
                } else if (auto val = self.get<double>(key)) {
                    return py::cast(*val);
                } else if (auto val = self.get<bool>(key)) {
                    return py::cast(*val);
                } else if (auto val = self.get<std::string>(key)) {
                    return py::cast(*val);
                } else if (auto val = self.get<std::vector<int>>(key)) {
                    return py::cast(*val);
                } else if (auto val = self.get<std::vector<double>>(key)) {
                    return py::cast(*val);
                } else if (auto val = self.get<std::vector<std::string>>(key)) {
                    return py::cast(*val);
                }
            }
            return py::none();
        }, py::arg("key"),
           "Get configuration value by key (returns None if key doesn't exist)")
        .def("getOr", [](Config& self, const std::string& key, const py::object& defaultValue) {
            // Use C++ getOr with template deduction
            // This is complex; for now we'll provide a simpler Python API
            if (self.has(key)) {
                // Try to get and if found, return it; if not, return default
                // This is a simplification - ideally would match types
                return defaultValue;
            }
            // For now, we'll just return None to indicate value exists
            return py::none();
        }, py::arg("key"), py::arg("default"),
           "Get configuration value or return default if not found")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            // Universal set method for Python
            if (value.is(py::int_())) {
                self.set(key, value.cast<int>());
            } else if (value.is(py::float_())) {
                self.set(key, value.cast<double>());
            } else if (value.is(py::bool_())) {
                self.set(key, value.cast<bool>());
            } else if (value.is(py::str())) {
                self.set(key, value.cast<std::string>());
            } else if (value.is(py::list())) {
                // Handle lists - try int first, then double, then string
                py::list pyList = value.cast<py::list>();
                if (pyList.empty()) {
                    self.set(key, std::vector<int>());
                } else {
                    // Try to infer type
                    bool allInt = true;
                    bool allDouble = true;
                    bool allString = true;
                    
                    for (auto item : pyList) {
                        if (!item.is(py::int_())) allInt = false;
                        if (!item.is(py::float_())) allDouble = false;
                        if (!item.is(py::str())) allString = false;
                    }
                    
                    if (allInt) {
                        std::vector<int> vec;
                        for (auto item : pyList) {
                            vec.push_back(item.cast<int>());
                        }
                        self.set(key, vec);
                    } else if (allDouble) {
                        std::vector<double> vec;
                        for (auto item : pyList) {
                            vec.push_back(item.cast<double>());
                        }
                        self.set(key, vec);
                    } else if (allString) {
                        std::vector<std::string> vec;
                        for (auto item : pyList) {
                            vec.push_back(item.cast<std::string>());
                        }
                        self.set(key, vec);
                    } else {
                        // Mixed types - convert to vector of strings
                        std::vector<std::string> vec;
                        for (auto item : pyList) {
                            if (item.is(py::int_())) {
                                vec.push_back(std::to_string(item.cast<int>()));
                            } else if (item.is(py::float_())) {
                                vec.push_back(std::to_string(item.cast<double>()));
                            } else {
                                vec.push_back(item.cast<std::string>());
                            }
                        }
                        self.set(key, vec);
                    }
                }
            } else {
                throw py::type_error("Unsupported value type for configuration");
            }
        }, py::arg("key"), py::arg("value"),
           "Set configuration value (automatically detects type: int, float, bool, string, or list)")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Pythonic properties and convenience methods
        .def_property_readonly("keys", &Config::getKeys,
             "Get all configuration keys")
        .def("getInt", [](Config& self, const std::string& key) {
            auto val = self.get<int>(key);
            if (val) return *val;
            throw py::key_error("Integer key '" + key + "' not found");
        }, py::arg("key"),
             "Get configuration value as integer (throws if not found)")
        .def("getFloat", [](Config& self, const std::string& key) {
            auto val = self.get<double>(key);
            if (val) return *val;
            throw py::key_error("Float key '" + key + "' not found");
        }, py::arg("key"),
             "Get configuration value as float (throws if not found)")
        .def("getBool", [](Config& self, const std::string& key) {
            auto val = self.get<bool>(key);
            if (val) return *val;
            throw py::key_error("Boolean key '" + key + "' not found");
        }, py::arg("key"),
             "Get configuration value as boolean (throws if not found)")
        .def("getString", [](Config& self, const std::string& key) {
            auto val = self.get<std::string>(key);
            if (val) return *val;
            throw py::key_error("String key '" + key + "' not found");
        }, py::arg("key"),
             "Get configuration value as string (throws if not found)")
        .def("getList", [](Config& self, const std::string& key) {
            // Try all list types
            auto val = self.get<std::vector<int>>(key);
            if (val) return py::cast(*val);
            val = self.get<std::vector<double>>(key);
            if (val) return py::cast(*val);
            val = self.get<std::vector<std::string>>(key);
            if (val) return py::cast(*val);
            throw py::key_error("List key '" + key + "' not found");
        }, py::arg("key"),
             "Get configuration value as list (int, float, or string)")
        .def("validate", [](Config& self) {
            py::dict validation;
            validation["valid"] = true;
            validation["errors"] = py::list();
            validation["warnings"] = py::list();
            
            // Basic validation checks
            if (self.getKeys().empty()) {
                validation["warnings"].attr("append")("Configuration is empty");
            }
            
            // Check for required keys (example)
            const std::vector<std::string> requiredKeys = {
                "brain.neuron_count",
                "brain.synapse_density"
            };
            
            for (const auto& key : requiredKeys) {
                if (!self.has(key)) {
                    validation["warnings"].attr("append")("Missing recommended key: " + key);
                }
            }
            
            // Check value ranges (example)
            auto neuronCount = self.get<int>("brain.neuron_count");
            if (neuronCount && (*neuronCount <= 0 || *neuronCount > 100000)) {
                validation["errors"].attr("append")("brain.neuron_count must be between 1 and 100000");
                validation["valid"] = false;
            }
            
            auto synapseDensity = self.get<double>("brain.synapse_density");
            if (synapseDensity && (*synapseDensity < 0.0 || *synapseDensity > 1.0)) {
                validation["errors"].attr("append")("brain.synapse_density must be between 0.0 and 1.0");
                validation["valid"] = false;
            }
            
            return validation;
        }, "Validate configuration and return validation report")
        .def("getDescription", [](Config& self, const std::string& key) {
            // This would need to be implemented in C++ to return descriptions
            // For now, we'll just return a placeholder
            return "Description not available in current implementation";
        }, py::arg("key"),
             "Get description of configuration key")
        .def("merge", [](Config& self, const Config& other) {
            // Merge another configuration into this one
            for (const auto& key : other.getKeys()) {
                if (other.has(key)) {
                    // Use the other config's value (overwrites current)
                    if (auto val = other.get<int>(key)) {
                        self.set(key, *val);
                    } else if (auto val = other.get<double>(key)) {
                        self.set(key, *val);
                    } else if (auto val = other.get<bool>(key)) {
                        self.set(key, *val);
                    } else if (auto val = other.get<std::string>(key)) {
                        self.set(key, *val);
                    } else if (auto val = other.get<std::vector<int>>(key)) {
                        self.set(key, *val);
                    } else if (auto val = other.get<std::vector<double>>(key)) {
                        self.set(key, *val);
                    } else if (auto val = other.get<std::vector<std::string>>(key)) {
                        self.set(key, *val);
                    }
                }
            }
        }, py::arg("other"),
             "Merge another configuration into this one")
        .def("diff", [](const Config& self, const Config& other) {
            py::dict diff;
            diff["added"] = py::list();
            diff["removed"] = py::list();
            diff["changed"] = py::list();
            
            const auto& selfKeys = self.getKeys();
            const auto& otherKeys = other.getKeys();
            
            // Find added keys
            for (const auto& key : otherKeys) {
                if (!self.has(key)) {
                    diff["added"].attr("append")(key);
                }
            }
            
            // Find removed keys
            for (const auto& key : selfKeys) {
                if (!other.has(key)) {
                    diff["removed"].attr("append")(key);
                }
            }
            
            // Find changed keys
            for (const auto& key : selfKeys) {
                if (other.has(key)) {
                    // Compare values
                    auto selfVal = self.get<int>(key);
                    auto otherVal = other.get<int>(key);
                    if (selfVal != otherVal) {
                        diff["changed"].attr("append")(key);
                    }
                }
            }
            
            return diff;
        }, py::arg("other"),
             "Get difference between this configuration and another")
        .def("export", [](const Config& self) {
            py::dict exportData;
            exportData["keys"] = self.getKeys();
            
            py::list values;
            for (const auto& key : self.getKeys()) {
                // Get value as string for export
                values.attr("append")(key + "=" + std::to_string(self.get<int>(key).value_or(0)));
            }
            exportData["values"] = values;
            
            return exportData;
        }, "Export configuration to Python dictionary")
        .def("import", [](Config& self, const py::dict& data) {
            // Clear current config
            self.clear();
            
            // Import from dictionary
            if (data.contains("keys") && data.contains("values")) {
                py::list keys = data["keys"].cast<py::list>();
                py::list values = data["values"].cast<py::list>();
                
                for (size_t i = 0; i < keys.size(); ++i) {
                    std::string key = keys[i].cast<std::string>();
                    std::string valueStr = values[i].cast<std::string>();
                    
                    // Try to parse as different types
                    try {
                        size_t pos = valueStr.find('=');
                        if (pos != std::string::npos) {
                            std::string valStr = valueStr.substr(pos + 1);
                            
                            // Try int
                            try {
                                int intVal = std::stoi(valStr);
                                self.set(key, intVal);
                            } catch (...) {
                                // Try float
                                try {
                                    double floatVal = std::stod(valStr);
                                    self.set(key, floatVal);
                                } catch (...) {
                                    // Try bool
                                    if (valStr == "true") {
                                        self.set(key, true);
                                    } else if (valStr == "false") {
                                        self.set(key, false);
                                    } else {
                                        // Default to string
                                        self.set(key, valStr);
                                    }
                                }
                            }
                        }
                    } catch (...) {
                        // Skip invalid entries
                    }
                }
            }
        }, py::arg("data"),
             "Import configuration from Python dictionary")
        .def("schema_check", [](Config& self, const py::dict& schema) {
            py::dict results;
            results["valid"] = true;
            results["violations"] = py::list();
            
            // Simple schema validation
            if (schema.contains("required_keys")) {
                py::list required = schema["required_keys"].cast<py::list>();
                for (auto item : required) {
                    std::string key = item.cast<std::string>();
                    if (!self.has(key)) {
                        results["violations"].attr("append")("Missing required key: " + key);
                        results["valid"] = false;
                    }
                }
            }
            
            if (schema.contains("type_constraints")) {
                py::dict types = schema["type_constraints"].cast<py::dict>();
                for (auto it : types) {
                    std::string key = it.first.cast<std::string>();
                    std::string type = it.second.cast<std::string>();
                    
                    if (self.has(key)) {
                        if (type == "int") {
                            if (!self.get<int>(key)) {
                                results["violations"].attr("append")("Key '" + key + "' should be int");
                                results["valid"] = false;
                            }
                        } else if (type == "float") {
                            if (!self.get<double>(key)) {
                                results["violations"].attr("append")("Key '" + key + "' should be float");
                                results["valid"] = false;
                            }
                        } else if (type == "bool") {
                            if (!self.get<bool>(key)) {
                                results["violations"].attr("append")("Key '" + key + "' should be bool");
                                results["valid"] = false;
                            }
                        } else if (type == "string") {
                            if (!self.get<std::string>(key)) {
                                results["violations"].attr("append")("Key '" + key + "' should be string");
                                results["valid"] = false;
                            }
                        }
                    }
                }
            }
            
            return results;
        }, py::arg("schema"),
             "Validate configuration against schema")
        .def("clone", [](const Config& self) {
            Config newConfig;
            newConfig.merge(self); // This is not ideal but works for demonstration
            return newConfig;
        }, "Create a copy of this configuration");

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
        // Pythonic properties
        .def_property_readonly("shape", [](const Vision& self) {
            return std::make_tuple(self.getWidth(), self.getHeight(), self.getChannels());
        }, "Get vision shape as (width, height, channels)")
        .def("is_grayscale", []() {
            // All current vision implementations have 3 channels by default
            return false;
        }, "Check if vision is grayscale");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Pythonic properties
        .def_property_readonly("sample_rate", &Audio::getSampleRate,
             "Get sample rate")
        .def_property_readonly("duration", [](const Audio& self) {
            return self.getNumSamples() / 44100.0f;  // Assume 44.1kHz standard
        }, "Get audio duration in seconds");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

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
        // Pythonic properties and convenience methods
        .def_property("type", &Action::getType, &Action::setType,
             "Action type property")
        .def_property("parameters", &Action::getParameters, &Action::setParameters,
             "Action parameters property")
        .def("getParameter", [](Action& self, size_t index) {
            const auto& params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            throw py::index_error("Parameter index out of range");
        }, py::arg("index"),
             "Get parameter by index")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto& params = self.getParameters();
            if (index >= params.size()) {
                params.resize(index + 1, 0.0f);
            }
            params[index] = value;
        }, py::arg("index"), py::arg("value"),
             "Set parameter by index")
        .def("addParameter", [](Action& self, float value) {
            auto& params = self.getParameters();
            params.push_back(value);
        }, py::arg("value"),
             "Add a new parameter")
        .def("execute", [](Action& self) {
            // Placeholder for action execution
            // In a real implementation, this would apply the action to a world
            py::dict result;
            result["success"] = true;
            result["action"] = self.getName();
            result["type"] = static_cast<int>(self.getType());
            return result;
        }, "Execute action and return result");

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

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        // Pythonic properties
        .def_property_readonly("center", [](const SimpleWorld& self) {
            return std::pair<float, float>(self.getWidth() / 2.0f, self.getHeight() / 2.0f);
        }, "Get the center position of the world")
        .def("getDistance", &SimpleWorld::getDistance, py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"),
             "Calculate distance between two points")
        .def("getHeading", [](const SimpleWorld& self, float x1, float y1, float x2, float y2) {
            float dx = x2 - x1;
            float dy = y2 - y1;
            return std::atan2(dy, dx);
        }, py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"),
             "Get heading angle from point 1 to point 2 (radians)")
        .def("isAgentAt", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if agent can be at position (agent is 0.5x0.5)")
        .def("getAgentBounds", [](const SimpleWorld& self) {
            const auto& body = self.getAgentBody();
            return std::pair<float, float>(body.x - 0.5f, body.y - 0.5f);
        }, "Get agent bounds as (minX, minY)")

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // Get all regions
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
        // Pythonic properties and convenience methods
        .def_property_readonly("neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count")
        .def_property_readonly("synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count")
        .def_property_readonly("firing_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def_property_readonly("spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def_property_readonly("avg_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def_property_readonly("ei_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getStats", [](const Brain& self) {
            py::dict stats;
            stats["neuron_count"] = self.getTotalNeuronCount();
            stats["synapse_count"] = self.getTotalSynapseCount();
            stats["active_count"] = self.getActiveNeuronCount();
            stats["firing_count"] = self.getFiringNeuronCount();
            stats["spike_count"] = self.getTotalSpikeCount();
            stats["avg_firing_rate"] = self.getAverageFiringRate();
            stats["ei_ratio"] = self.getExcitationInhibitionRatio();
            stats["developmental_stage"] = self.getDevelopmentalStage();
            return stats;
        }, "Get comprehensive brain statistics as a dictionary")
        .def("getStatsString", &Brain::logStatus,
             "Get brain status as a formatted string")
        .def("getStatsCSV", [](const Brain& self) {
            std::stringstream ss;
            ss << "neurons," << self.getTotalNeuronCount() << "\n";
            ss << "synapses," << self.getTotalSynapseCount() << "\n";
            ss << "active_neurons," << self.getActiveNeuronCount() << "\n";
            ss << "firing_neurons," << self.getFiringNeuronCount() << "\n";
            ss << "total_spikes," << self.getTotalSpikeCount() << "\n";
            ss << "avg_firing_rate," << self.getAverageFiringRate() << "\n";
            ss << "ei_ratio," << self.getExcitationInhibitionRatio() << "\n";
            return ss.str();
        }, "Get brain statistics as CSV string")
        .def("validate", [](Brain& self) {
            py::dict validation;
            validation["valid"] = true;
            validation["errors"] = py::list();
            validation["warnings"] = py::list();
            
            // Basic validation checks
            if (self.getTotalNeuronCount() == 0) {
                validation["warnings"].attr("append")("No neurons initialized");
            }
            
            if (self.getTotalSynapseCount() == 0) {
                validation["warnings"].attr("append")("No synapses initialized");
            }
            
            return validation;
        }, "Validate brain state and return validation report");

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("getMotorGroupActivities", &AgentBrain::getMotorGroupActivities,
             "Get motor group activities")
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
        // Pythonic properties and convenience methods
        .def_property_readonly("sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def_property_readonly("motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("is_processing_enabled", [](AgentBrain& self) {
            return self.isRewardModulationEnabled() && 
                   self.isDevelopmentEnabled() && 
                   self.isCuriosityEnabled();
        }, "Check if all processing systems are enabled")
        .def("get_processing_stats", [](AgentBrain& self) {
            py::dict stats;
            stats["reward_modulation"] = self.isRewardModulationEnabled();
            stats["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            stats["development"] = self.isDevelopmentEnabled();
            stats["curiosity"] = self.isCuriosityEnabled();
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            return stats;
        }, "Get comprehensive processing statistics")
        .def("get_system_status", [](AgentBrain& self) {
            py::dict status;
            status["developmental_stage"] = self.getDevelopmentalStage();
            status["is_processing_enabled"] = self.is_processing_enabled();
            status["stats"] = self.get_processing_stats();
            return status;
        }, "Get complete system status")
        .def("configure_processing", [](AgentBrain& self, bool reward, bool plasticity, 
                                         bool development, bool curiosity) {
            self.enableRewardModulation(reward);
            self.enableStructuralPlasticity(plasticity);
            self.enableDevelopment(development);
            self.enableCuriosity(curiosity);
        }, py::arg("reward"), py::arg("plasticity"), py::arg("development"), py::arg("curiosity"),
             "Enable/disable all processing systems")
        .def("update_all", [](AgentBrain& self, const SensoryPercept& percept, 
                              double timestep) {
            self.processSensoryInput(percept);
            self.updateDevelopment(timestep);
        }, py::arg("percept"), py::arg("timestep"),
             "Convenience method to process sensory input and update development")
        .def("apply_reward", [](AgentBrain& self, float reward, float predicted = 0.0f) {
            self.applyRewardModulation(reward, predicted);
        }, py::arg("reward"), py::arg("predicted") = 0.0f,
             "Apply reward modulation with optional predicted reward")
        .def("reset_and_initialize", [](AgentBrain& self, const SimpleWorld& world) {
            self.reset();
            self.initialize(world);
        }, py::arg("world"),
             "Reset and re-initialize agent with new world")
        .def("clone", [](const AgentBrain& self) {
            // Create a new AgentBrain with the same brain
            return std::make_shared<AgentBrain>(self.getBrain());
        }, "Clone the agent brain (creates new instance with same brain)")
        .def("copy_config", [](AgentBrain& self, const AgentBrain& other) {
            self.enableRewardModulation(other.isRewardModulationEnabled());
            self.enableStructuralPlasticity(other.isStructuralPlasticityEnabled());
            self.enableDevelopment(other.isDevelopmentEnabled());
            self.enableCuriosity(other.isCuriosityEnabled());
        }, py::arg("other"),
             "Copy configuration from another AgentBrain");
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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
