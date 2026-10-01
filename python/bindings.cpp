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
        .def("__lt__", &NeuronId::operator<)
        .def("__gt__", [](const NeuronId& a, const NeuronId& b) { return b.operator<(a); })
        .def("__le__", [](const NeuronId& a, const NeuronId& b) { return a.operator<=(b); })
        .def("__ge__", [](const NeuronId& a, const NeuronId& b) { return b.operator<=(a); })
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
        .def("__lt__", &SynapseId::operator<)
        .def("__gt__", [](const SynapseId& a, const SynapseId& b) { return b.operator<(a); })
        .def("__le__", [](const SynapseId& a, const SynapseId& b) { return a.operator<=(b); })
        .def("__ge__", [](const SynapseId& a, const SynapseId& b) { return b.operator<=(a); })
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
        .def("__lt__", &RegionId::operator<)
        .def("__gt__", [](const RegionId& a, const RegionId& b) { return b.operator<(a); })
        .def("__le__", [](const RegionId& a, const RegionId& b) { return a.operator<=(b); })
        .def("__ge__", [](const RegionId& a, const RegionId& b) { return b.operator<=(a); })
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
        .def("__ne__", &PopulationId::operator!=)
        .def("__lt__", &PopulationId::operator<)
        .def("__gt__", [](const PopulationId& a, const PopulationId& b) { return b.operator<(a); })
        .def("__le__", [](const PopulationId& a, const PopulationId& b) { return a.operator<=(b); })
        .def("__ge__", [](const PopulationId& a, const PopulationId& b) { return b.operator<=(a); });

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

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](const std::shared_ptr<Config>& self, const std::string& filepath) {
            try {
                return self->loadFromFile(filepath);
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to load config from file: " + std::string(e.what())).c_str());
                return false;
            }
        }, py::arg("filepath"), "Load configuration from a JSON file (with JSON and fallback to simple format)")
        .def("loadFromArgs", [](const std::shared_ptr<Config>& self, int argc, char** argv) {
            try {
                return self->loadFromArgs(argc, argv);
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to load config from args: " + std::string(e.what())).c_str());
                return false;
            }
        }, py::arg("argc"), py::arg("argv"), "Load configuration from command line arguments")
        .def("saveToFile", [](const std::shared_ptr<Config>& self, const std::string& filepath) {
            try {
                return self->saveToFile(filepath);
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to save config to file: " + std::string(e.what())).c_str());
                return false;
            }
        }, py::arg("filepath"), "Save configuration to a JSON file (with JSON and fallback to simple format)")
        
        // Enhanced JSON serialization/deserialization methods
        .def("toJson", [](const std::shared_ptr<Config>& self) {
            try {
                return self->toJson();
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to serialize config to JSON: " + std::string(e.what())).c_str());
                return std::string();
            }
        }, "Convert configuration to JSON string")
        .def("fromJson", [](std::shared_ptr<Config>& self, const std::string& json_str) {
            try {
                return self->fromJson(json_str);
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to parse JSON config: " + std::string(e.what())).c_str());
                return false;
            }
        }, py::arg("json_str"), "Load configuration from JSON string")
        .def("validateJsonSchema", [](const std::shared_ptr<Config>& self, const std::string& schema_path) {
            try {
                return self->validateJsonSchema(schema_path);
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to validate JSON schema: " + std::string(e.what())).c_str());
                return false;
            }
        }, py::arg("schema_path"), "Validate configuration against JSON schema")
        .def("mergeJson", [](std::shared_ptr<Config>& self, const std::string& json_str) {
            try {
                return self->mergeJson(json_str);
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to merge JSON config: " + std::string(e.what())).c_str());
                return false;
            }
        }, py::arg("json_str"), "Merge configuration with JSON string")
        .def("diffJson", [](const std::shared_ptr<Config>& self, const std::string& other_json) {
            try {
                return self->diffJson(other_json);
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to compare JSON configs: " + std::string(e.what())).c_str());
                return std::string();
            }
        }, py::arg("other_json"), "Get difference between configurations as JSON diff")
        .def("importConfig", [](std::shared_ptr<Config>& self, const std::string& config_json) {
            try {
                return self->importConfig(config_json);
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to import config: " + std::string(e.what())).c_str());
                return false;
            }
        }, py::arg("config_json"), "Import configuration from another config's JSON representation")
        .def("exportConfig", [](const std::shared_ptr<Config>& self) {
            try {
                return self->exportConfig();
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to export config: " + std::string(e.what())).c_str());
                return std::string();
            }
        }, "Export configuration to portable JSON format")
        
        // Enhanced error handling and validation
        .def("validate", [](const std::shared_ptr<Config>& self) {
            return self->validate();
        }, "Validate configuration (all sections)")
        .def("validateSection", [](const std::shared_ptr<Config>& self, const std::string& section) {
            return self->validateSection(section);
        }, py::arg("section"), "Validate specific configuration section")
        .def("getValidationErrors", [](const std::shared_ptr<Config>& self) {
            auto errors = self->getValidationErrors();
            return std::vector<std::string>(errors.begin(), errors.end());
        }, "Get configuration validation errors")
        .def("clearErrors", [](std::shared_ptr<Config>& self) {
            self->clearErrors();
        }, "Clear validation errors")
        
        // Helper methods for JSON operations
        .def("prettyPrint", [](const std::shared_ptr<Config>& self, size_t indent = 2) {
            return self->prettyPrint(indent);
        }, py::arg("indent") = 2, "Get pretty-printed JSON representation")
        .def("compress", [](const std::shared_ptr<Config>& self) {
            return self->compress();
        }, "Get compressed JSON representation")
        .def("getJsonSize", [](const std::shared_ptr<Config>& self) {
            return self->getJsonSize();
        }, "Get size of JSON representation in bytes")
        
        // Common configuration operations
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        
        // Configuration setters
        .def("set", [](std::shared_ptr<Config>& self, const std::string& key, const std::string& value) {
            return self->set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set a configuration value")
        .def("set", [](std::shared_ptr<Config>& self, const std::string& key, int value) {
            return self->set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set an integer configuration value")
        .def("set", [](std::shared_ptr<Config>& self, const std::string& key, float value) {
            return self->set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set a float configuration value")
        .def("set", [](std::shared_ptr<Config>& self, const std::string& key, bool value) {
            return self->set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set a boolean configuration value")
        
        // Configuration getters
        .def("getString", [](const std::shared_ptr<Config>& self, const std::string& key, const std::string& defaultValue = "") {
            try {
                return self->getString(key, defaultValue);
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to get string config: " + std::string(e.what())).c_str());
                return defaultValue;
            }
        }, py::arg("key"), py::arg("defaultValue") = "",
             "Get a string configuration value")
        .def("getInt", [](const std::shared_ptr<Config>& self, const std::string& key, int defaultValue = 0) {
            try {
                return self->getInt(key, defaultValue);
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to get int config: " + std::string(e.what())).c_str());
                return defaultValue;
            }
        }, py::arg("key"), py::arg("defaultValue") = 0,
             "Get an integer configuration value")
        .def("getFloat", [](const std::shared_ptr<Config>& self, const std::string& key, float defaultValue = 0.0f) {
            try {
                return self->getFloat(key, defaultValue);
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to get float config: " + std::string(e.what())).c_str());
                return defaultValue;
            }
        }, py::arg("key"), py::arg("defaultValue") = 0.0f,
             "Get a float configuration value")
        .def("getBool", [](const std::shared_ptr<Config>& self, const std::string& key, bool defaultValue = false) {
            try {
                return self->getBool(key, defaultValue);
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to get bool config: " + std::string(e.what())).c_str());
                return defaultValue;
            }
        }, py::arg("key"), py::arg("defaultValue") = false,
             "Get a boolean configuration value")
        
        // Enhanced error handling
        .def("validateSection", [](const std::shared_ptr<Config>& self, const std::string& section) {
            try {
                return self->validateSection(section);
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to validate config section: " + std::string(e.what())).c_str());
                return false;
            }
        }, py::arg("section"),
             "Validate a configuration section")
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__enter__", [](std::shared_ptr<Config>& self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<Config>& self, py::args) {
            try {
                self->clear();
                return false; // Don't suppress exceptions
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Error in Config context manager: " + std::string(e.what())).c_str());
                return false;
            }
        });

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
        
        // Property access for vision data
        .def_property_readonly("width", &Vision::getWidth,
                             "Vision width in pixels")
        .def_property_readonly("height", &Vision::getHeight,
                             "Vision height in pixels")
        .def_property_readonly("channels", &Vision::getChannels,
                             "Number of color channels")
        .def_property_readonly("size", [](const Vision& self) {
            return self.getWidth() * self.getHeight() * self.getChannels();
        }, "Total number of pixels")
        
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            try {
                self.setData(data);
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to set vision data: " + std::string(e.what())).c_str());
            }
        }, py::arg("data"))
        .def("validateData", [](const Vision& self) {
            return self.validateData();
        }, "Validate vision data")
        .def("normalize", [](Vision& self) {
            try {
                self.normalize();
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to normalize vision data: " + std::string(e.what())).c_str());
            }
        }, "Normalize vision data to [0,1]")
        
        .def("__enter__", [](std::shared_ptr<Vision>& self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<Vision>& self, py::args) {
            try {
                // Vision doesn't have cleanup, but context manager still available
                return false;
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Error in Vision context manager: " + std::string(e.what())).c_str());
                return false;
            }
        });

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        
        // Property access for audio data
        .def_property_readonly("sample_rate", &Audio::getSampleRate,
                             "Audio sample rate in Hz")
        .def_property_readonly("num_samples", &Audio::getNumSamples,
                             "Number of audio samples")
        .def_property_readonly("duration", [](const Audio& self) {
            return static_cast<float>(self.getNumSamples()) / self.getSampleRate();
        }, "Audio duration in seconds")
        .def_property_readonly("sample_count", &Audio::getNumSamples,
                             "Number of audio samples (alias)")
        
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            try {
                self.setData(data);
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to set audio data: " + std::string(e.what())).c_str());
            }
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("validateData", [](const Audio& self) {
            return self.validateData();
        }, "Validate audio data")
        .def("resample", [](Audio& self, size_t newSampleRate) {
            try {
                self.resample(newSampleRate);
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to resample audio: " + std::string(e.what())).c_str());
            }
        }, py::arg("newSampleRate"),
             "Resample audio to new sample rate")
        
        .def("__enter__", [](std::shared_ptr<Audio>& self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<Audio>& self, py::args) {
            try {
                // Audio doesn't have cleanup, but context manager still available
                return false;
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Error in Audio context manager: " + std::string(e.what())).c_str());
                return false;
            }
        });

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        
        // Property access for internal signals
        .def_property_readonly("signal_count", &InternalSignals::getSignalCount,
                             "Number of signals in the internal signals")
        .def_property_readonly("signal_sum", [](const InternalSignals& self) {
            return self.getSignalSum();
        }, "Sum of all signal values")
        .def_property_readonly("signal_mean", [](const InternalSignals& self) {
            return self.getSignalMean();
        }, "Mean value of all signals")
        .def_property_readonly("signal_max", [](const InternalSignals& self) {
            return self.getSignalMax();
        }, "Maximum signal value")
        .def_property_readonly("signal_min", [](const InternalSignals& self) {
            return self.getSignalMin();
        }, "Minimum signal value")
        
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("validateSignals", [](const InternalSignals& self) {
            return self.validateSignals();
        }, "Validate internal signals")
        .def("normalizeSignals", [](InternalSignals& self) {
            try {
                self.normalizeSignals();
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to normalize internal signals: " + std::string(e.what())).c_str());
            }
        }, "Normalize internal signals to [0,1]")
        
        .def("__enter__", [](std::shared_ptr<InternalSignals>& self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<InternalSignals>& self, py::args) {
            try {
                self->clearSignals();
                return false; // Don't suppress exceptions
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Error in InternalSignals context manager: " + std::string(e.what())).c_str());
                return false;
            }
        });

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        
        // Property access for action attributes
        .def_property_readonly("action_type", &Action::getType,
                             "Action type (MoveForward, MoveBackward, etc.)")
        .def_property_readonly("type_name", &Action::getName,
                             "Human-readable name of the action")
        .def_property_readonly("parameter_count", [](const Action& self) {
            return self.getParameters().size();
        }, "Number of parameters")
        .def_property_readonly("is_movement", [](const Action& self) {
            return self.getType() == ActionType::MoveForward ||
                   self.getType() == ActionType::MoveBackward ||
                   self.getType() == ActionType::MoveLeft ||
                   self.getType() == ActionType::MoveRight;
        }, "Whether this is a movement action")
        .def_property_readonly("is_interactive", [](const Action& self) {
            return self.getType() == ActionType::Look ||
                   self.getType() == ActionType::Interact ||
                   self.getType() == ActionType::Eat ||
                   self.getType() == ActionType::Drink ||
                   self.getType() == ActionType::Custom;
        }, "Whether this is an interactive action")
        
        // Helper methods
        .def("getParameter", [](const Action& self, size_t index) {
            const auto& params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            PyErr_SetString(PyExc_IndexError, ("Parameter index out of range").c_str());
            return 0.0f;
        }, py::arg("index"), "Get parameter by index")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto& params = self.getParameters();
            if (index < params.size()) {
                params[index] = value;
                self.setParameters(params);
            } else {
                PyErr_SetString(PyExc_IndexError, ("Parameter index out of range").c_str());
            }
        }, py::arg("index"), py::arg("value"), "Set parameter by index")
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add a parameter")
        .def("getActionVector", [](const Action& self) {
            return self.getParameters();
        }, "Get action as parameter vector")
        .def("validateAction", [](const Action& self) {
            return self.validateAction();
        }, "Validate action parameters")
        .def("getActionDescription", [](const Action& self) {
            std::string desc = self.getName();
            if (!self.getParameters().empty()) {
                desc += " (params:";
                for (size_t i = 0; i < self.getParameters().size(); ++i) {
                    if (i > 0) desc += ",";
                    desc += std::to_string(self.getParameters()[i]);
                }
                desc += ")";
            }
            return desc;
        }, "Get human-readable action description")
        
        .def("clone", [](const Action& action) {
            try {
                return std::make_shared<Action>(action.clone());
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to clone action: " + std::string(e.what())).c_str());
                return std::shared_ptr<Action>(nullptr);
            }
        })
        
        .def("__enter__", [](std::shared_ptr<Action>& self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<Action>& self, py::args) {
            try {
                // Action cleanup if needed
                return false;
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Error in Action context manager: " + std::string(e.what())).c_str());
                return false;
            }
        });

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
               py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
               py::arg("radius") = 0.5f)
        
        // Property access for world object attributes
        .def_property_readonly("position", [](const WorldObject& self) {
            return std::make_pair(self.x, self.y);
        }, "Object position as (x, y) tuple")
        .def_property_readonly("distance_to", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, "Get distance to position (x, y)")
        .def_property_readonly("is_resource", [](const WorldObject& self) {
            return self.type == WorldObjectType::Resource;
        }, "Whether this is a resource object")
        .def_property_readonly("is_hazard", [](const WorldObject& self) {
            return self.type == WorldObjectType::Hazard;
        }, "Whether this is a hazard object")
        .def_property_readonly("is_wall", [](const WorldObject& self) {
            return self.type == WorldObjectType::Wall;
        }, "Whether this is a wall object")
        .def_property_readonly("is_interactable", [](const WorldObject& self) {
            return self.type == WorldObjectType::Marker ||
                   self.type == WorldObjectType::Resource ||
                   self.type == WorldObjectType::Hazard;
        }, "Whether this object is interactable")
        
        // Helper methods
        .def("moveTo", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Move object to position")
        .def("setRadius", [](WorldObject& self, float radius) {
            self.radius = radius;
        }, py::arg("radius"), "Set object radius")
        .def("setType", [](WorldObject& self, WorldObjectType type) {
            self.type = type;
        }, py::arg("type"), "Set object type")
        .def("setValue", [](WorldObject& self, float value) {
            self.value = value;
        }, py::arg("value"), "Set object value")
        .def("setActive", [](WorldObject& self, bool active) {
            self.active = active;
        }, py::arg("active"), "Set object active state")
        .def("isCollidingWith", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            float distance = std::sqrt(dx*dx + dy*dy);
            return distance < (self.radius + other.radius);
        }, py::arg("other"), "Check if colliding with another object")
        .def("getBoundingBox", [](const WorldObject& self) {
            return std::make_tuple(self.x - self.radius, self.y - self.radius,
                                   self.x + self.radius, self.y + self.radius);
        }, "Get object bounding box as (xmin, ymin, xmax, ymax)")
        
        .def("__enter__", [](std::shared_ptr<WorldObject>& self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<WorldObject>& self, py::args) {
            try {
                self->active = false; // Deactivate on exit
                return false;
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Error in WorldObject context manager: " + std::string(e.what())).c_str());
                return false;
            }
        });

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        
        // Property access for agent body attributes
        .def_property_readonly("position", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Agent position as (x, y) tuple")
        .def_property_readonly("velocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Agent velocity as (vx, vy) tuple")
        .def_property_readonly("has_motion", [](const AgentBody& self) {
            return self.isMoving || self.isTurning;
        }, "Whether agent is currently moving")
        .def_property_readonly("energy_level", [](const AgentBody& self) {
            return self.energy;
        }, "Agent energy level")
        .def_property_readonly("health_level", [](const AgentBody& self) {
            return self.health;
        }, "Agent health level")
        .def_property_readonly("is_alive", [](const AgentBody& self) {
            return self.health > 0.0f;
        }, "Whether agent is alive")
        .def_property_readonly("age_years", [](const AgentBody& self) {
            return self.age;
        }, "Agent age in years")
        
        // Helper methods
        .def("moveTo", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
            self.isMoving = true;
            self.lastActionTime = time(nullptr);
        }, py::arg("x"), py::arg("y"), "Move agent to position")
        .def("turn", [](AgentBody& self, float orientation) {
            self.orientation = orientation;
            self.isTurning = true;
            self.lastActionTime = time(nullptr);
        }, py::arg("orientation"), "Turn agent to orientation")
        .def("applyVelocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
            self.isMoving = (std::abs(vx) > 0.001f || std::abs(vy) > 0.001f);
        }, py::arg("vx"), py::arg("vy"), "Apply velocity to agent")
        .def("updateEnergy", [](AgentBody& self, float delta) {
            self.energy += delta;
            if (self.energy < 0.0f) self.energy = 0.0f;
        }, py::arg("delta"), "Update agent energy")
        .def("updateHealth", [](AgentBody& self, float delta) {
            self.health += delta;
            if (self.health < 0.0f) self.health = 0.0f;
        }, py::arg("delta"), "Update agent health")
        .def("getDistanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Get distance to position")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX*self.velocityX + self.velocityY*self.velocityY);
        }, "Get agent speed")
        
        .def("reset", [](AgentBody& self) {
            try {
                self.reset();
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to reset agent body: " + std::string(e.what())).c_str());
            }
        })
        
        .def("__enter__", [](std::shared_ptr<AgentBody>& self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<AgentBody>& self, py::args) {
            try {
                self->reset();
                return false; // Don't suppress exceptions
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Error in AgentBody context manager: " + std::string(e.what())).c_str());
                return false;
            }
        });

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        
        // Property access for action result attributes
        .def_property_readonly("success_status", [](const ActionResult& self) {
            return self.success;
        }, "Action success status")
        .def_property_readonly("reward_value", [](const ActionResult& self) {
            return self.reward;
        }, "Reward received from action")
        .def_property_readonly("message_text", [](const ActionResult& self) {
            return self.message;
        }, "Result message/description")
        .def_property_readonly("is_positive_reward", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Whether reward is positive")
        .def_property_readonly("is_negative_reward", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Whether reward is negative")
        
        // Helper methods
        .def("getRewardCategory", [](const ActionResult& self) {
            if (self.reward > 1.0f) return "Excellent";
            if (self.reward > 0.0f) return "Good";
            if (self.reward == 0.0f) return "Neutral";
            if (self.reward > -1.0f) return "Poor";
            return "Bad";
        }, "Get reward category (Excellent/Good/Neutral/Poor/Bad)")
        .def("addToMessage", [](ActionResult& self, const std::string& text) {
            if (!self.message.empty()) self.message += ": ";
            self.message += text;
        }, py::arg("text"), "Append text to result message")
        .def("setSuccessWithReward", [](ActionResult& self, float reward) {
            self.success = true;
            self.reward = reward;
            self.message = "Action succeeded with reward: " + std::to_string(reward);
        }, py::arg("reward"), "Mark action as successful with reward")
        .def("setFailureWithPenalty", [](ActionResult& self, float penalty, const std::string& reason) {
            self.success = false;
            self.reward = penalty;
            self.message = "Action failed: " + reason + " (penalty: " + std::to_string(penalty) + ")";
        }, py::arg("penalty"), py::arg("reason"), "Mark action as failed with penalty")
        .def("toDict", [](const ActionResult& self) {
            return std::map<std::string, py::object>{
                {"success", self.success},
                {"reward", self.reward},
                {"message", self.message}
            };
        }, "Convert action result to dictionary")
        
        .def("__enter__", [](std::shared_ptr<ActionResult>& self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<ActionResult>& self, py::args) {
            try {
                // Clear result on exit
                self->reward = 0.0f;
                self->success = false;
                self->message = "";
                return false;
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Error in ActionResult context manager: " + std::string(e.what())).c_str());
                return false;
            }
        });

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        
        // Property access for sensory percept attributes
        .def_property_readonly("timestamp", &SensoryPercept::getTimestamp,
                             "Percept timestamp")
        .def_property_readonly("has_vision", [](const SensoryPercept& self) {
            return self.getVision() != nullptr;
        }, "Whether percept contains vision data")
        .def_property_readonly("has_audio", [](const SensoryPercept& self) {
            return self.getAudio() != nullptr;
        }, "Whether percept contains audio data")
        .def_property_readonly("has_touch", [](const SensoryPercept& self) {
            return self.getTouch() != nullptr;
        }, "Whether percept contains touch data")
        .def_property_readonly("has_internal", [](const SensoryPercept& self) {
            return self.getInternal() != nullptr;
        }, "Whether percept contains internal signals")
        .def_property_readonly("has_proprioception", [](const SensoryPercept& self) {
            return self.getProprioception() != nullptr;
        }, "Whether percept contains proprioception data")
        .def_property_readonly("has_any_signals", [](const SensoryPercept& self) {
            return self.getVision() != nullptr ||
                   self.getAudio() != nullptr ||
                   self.getTouch() != nullptr ||
                   self.getInternal() != nullptr ||
                   self.getProprioception() != nullptr;
        }, "Whether percept contains any sensory signals")
        
        // Helper methods
        .def("getSignalVector", [](const SensoryPercept& self) {
            return self.getAllSignals();
        }, "Get all sensory signals as vector")
        .def("getSignalCount", [](const SensoryPercept& self) {
            return self.getAllSignals().size();
        }, "Get number of sensory signals")
        .def("getVisionData", [](const SensoryPercept& self) {
            auto vision = self.getVision();
            if (vision) {
                return vision->getData();
            }
            PyErr_SetString(PyExc_RuntimeError, "No vision data available");
            return std::vector<float>{};
        }, "Get vision data as vector")
        .def("getAudioData", [](const SensoryPercept& self) {
            auto audio = self.getAudio();
            if (audio) {
                return audio->getData();
            }
            PyErr_SetString(PyExc_RuntimeError, "No audio data available");
            return std::vector<float>{};
        }, "Get audio data as vector")
        .def("getTouchData", [](const SensoryPercept& self) {
            auto touch = self.getTouch();
            if (touch) {
                return touch->getData();
            }
            PyErr_SetString(PyExc_RuntimeError, "No touch data available");
            return std::vector<float>{};
        }, "Get touch data as vector")
        .def("getInternalData", [](const SensoryPercept& self) {
            auto internal = self.getInternal();
            if (internal) {
                return internal->getData();
            }
            PyErr_SetString(PyExc_RuntimeError, "No internal data available");
            return std::vector<float>{};
        }, "Get internal data as vector")
        .def("getProprioceptionData", [](const SensoryPercept& self) {
            auto proprioception = self.getProprioception();
            if (proprioception) {
                return proprioception->getData();
            }
            PyErr_SetString(PyExc_RuntimeError, "No proprioception data available");
            return std::vector<float>{};
        }, "Get proprioception data as vector")
        .def("validatePercept", [](const SensoryPercept& self) {
            return self.validatePercept();
        }, "Validate sensory percept")
        .def("normalizeSignals", [](SensoryPercept& self) {
            try {
                self.normalizeSignals();
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Failed to normalize sensory signals: " + std::string(e.what())).c_str());
            }
        }, "Normalize all sensory signals")
        
        .def("__enter__", [](std::shared_ptr<SensoryPercept>& self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<SensoryPercept>& self, py::args) {
            try {
                // Clear sensory data on exit
                self->setVision(nullptr);
                self->setTouch(nullptr);
                self->setInternal(nullptr);
                self->setProprioception(nullptr);
                self->setAudio(nullptr);
                self->setTimestamp(0.0);
                return false;
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Error in SensoryPercept context manager: " + std::string(e.what())).c_str());
                return false;
            }
        });

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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("__enter__", [](std::shared_ptr<SimpleWorld>& self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<SimpleWorld>& self, py::args) {
            try {
                self->reset();
                return false; // Don't suppress exceptions
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Error in SimpleWorld context manager: " + std::string(e.what())).c_str());
                return false;
            }
        });

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
        
        // Batch operations for improved efficiency
        .def("batchStep", [](Brain& self, const std::vector<std::pair<SimulationStep, Timestamp>>& steps) {
            for (const auto& step : steps) {
                self.step(step.first, step.second);
            }
        }, py::arg("steps"),
             "Perform multiple simulation steps in batch")
        .def("batchInjectCurrent", [](Brain& self, const std::vector<std::pair<NeuronId, MembranePotential>>& injections) {
            for (const auto& injection : injections) {
                self.injectCurrent(injection.first, injection.second);
            }
        }, py::arg("injections"),
             "Inject current into multiple neurons in batch")
        .def("batchInjectToType", [](Brain& self, const std::vector<std::pair<NeuronType, MembranePotential>>& injections) {
            for (const auto& injection : injections) {
                self.injectCurrentToNeurons(injection.first, injection.second);
            }
        }, py::arg("injections"),
             "Inject current into neurons by type in batch")
        
        // Helper methods for common operations
        .def("getNeuronsByType", [](const Brain& self, NeuronType type) {
            std::vector<NeuronId> neurons;
            auto regions = self.getRegions();
            for (const auto& region : regions) {
                auto populations = region->getPopulations();
                for (const auto& population : populations) {
                    auto neurons_vec = population->getNeurons();
                    for (const auto& neuron : neurons_vec) {
                        if (neuron->getType() == type) {
                            neurons.push_back(neuron->getId());
                        }
                    }
                }
            }
            return neurons;
        }, py::arg("type"), "Get all neurons of a specific type")
        .def("getStatisticsSummary", [](const Brain& self) {
            return std::map<std::string, py::object>{
                {"neuron_count", self.getTotalNeuronCount()},
                {"synapse_count", self.getTotalSynapseCount()},
                {"active_neuron_count", self.getActiveNeuronCount()},
                {"firing_neuron_count", self.getFiringNeuronCount()},
                {"average_firing_rate", self.getAverageFiringRate()},
                {"total_spike_count", self.getTotalSpikeCount()},
                {"excitation_inhibition_ratio", self.getExcitationInhibitionRatio()}
            };
        }, "Get comprehensive statistics summary")
        .def("getDevelopmentHistory", [](const Brain& self, size_t max_entries = 10) {
            std::vector<std::map<std::string, py::object>> history;
            auto current_stage = self.getDevelopmentalStage();
            auto current_time = std::time(nullptr);
            
            for (size_t i = 0; i < max_entries; ++i) {
                history.push_back({
                    {"stage", static_cast<int>(current_stage)},
                    {"timestamp", current_time - i * 3600}, // Approximate historical timestamps
                    {"neuron_count", self.getTotalNeuronCount()},
                    {"firing_rate", self.getAverageFiringRate()}
                });
            }
            return history;
        }, py::arg("max_entries") = 10, "Get developmental history" )
        .def("getConnectivityMatrix", [](const Brain& self) {
            std::map<std::string, std::map<std::string, float>> matrix;
            auto regions = self.getRegions();
            for (size_t i = 0; i < regions.size(); ++i) {
                auto region_i = regions[i];
                matrix[region_i->getName()] = std::map<std::string, float>();
                for (size_t j = 0; j < regions.size(); ++j) {
                    auto region_j = regions[j];
                    // Calculate connectivity between regions
                    float connectivity = 0.0f;
                    auto connections = region_i->getConnections();
                    for (const auto& conn : connections) {
                        if (conn.targetRegion == region_j->getId()) {
                            connectivity += conn.weight;
                        }
                    }
                    matrix[region_i->getName()][region_j->getName()] = connectivity;
                }
            }
            return matrix;
        }, "Get inter-regional connectivity matrix")
        
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
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain.getTotalSynapseCount()) +
                   " synapses>";
        })
        .def("__enter__", [](std::shared_ptr<Brain>& self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<Brain>& self, py::args) {
            try {
                self->reset();
                return false; // Don't suppress exceptions
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Error in Brain context manager: " + std::string(e.what())).c_str());
                return false;
            }
        });

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        
        // Batch processing for improved efficiency
        .def("batchProcessSensoryInputs", [](AgentBrain& self, const std::vector<std::shared_ptr<SensoryPercept>>& percepts) {
            for (const auto& percept : percepts) {
                self.processSensoryInput(*percept);
            }
        }, py::arg("percepts"),
             "Process multiple sensory inputs in batch")
        .def("batchApplyRewardModulation", [](AgentBrain& self, const std::vector<std::pair<float, float>>& reward_pairs) {
            for (const auto& reward_pair : reward_pairs) {
                self.applyRewardModulation(reward_pair.first, reward_pair.second);
            }
        }, py::arg("reward_pairs"),
             "Apply reward modulation for multiple pairs in batch")
        .def("batchUpdateDevelopment", [](AgentBrain& self, const std::vector<float>& timesteps) {
            for (const auto& timestep : timesteps) {
                self.updateDevelopment(timestep);
            }
        }, py::arg("timesteps"),
             "Update development for multiple timesteps in batch")
        
        // Helper methods for common operations
        .def("getNeuromodulationStats", [](const AgentBrain& self) {
            return std::map<std::string, float>{
                {"neuromodulation_level", self.getNeuromodulationLevel()},
                {"curiosity_level", self.getCuriosityLevel()},
                {"novelty_level", self.getNoveltyLevel()},
                {"prediction_error", self.getPredictionError()},
                {"developmental_stage", static_cast<float>(self.getDevelopmentalStage())}
            };
        }, "Get comprehensive neuromodulation statistics")
        .def("getEnabledSubsystems", [](const AgentBrain& self) {
            return std::map<std::string, bool>{
                {"reward_modulation", self.isRewardModulationEnabled()},
                {"structural_plasticity", self.isStructuralPlasticityEnabled()},
                {"development", self.isDevelopmentEnabled()},
                {"curiosity", self.isCuriosityEnabled()}
            };
        }, "Get status of all enabled subsystems")
        .def("getActionPreferences", [](const AgentBrain& self) {
            // Generate action preferences based on neuromodulation levels
            std::map<std::string, float> preferences;
            float curiosity = self.getCuriosityLevel();
            float novelty = self.getNoveltyLevel();
            float prediction_error = self.getPredictionError();
            
            // Calculate action preferences based on neuromodulators
            preferences["exploration"] = curiosity * 0.4f + novelty * 0.3f + (1.0f - prediction_error) * 0.3f;
            preferences["exploitation"] = 1.0f - preferences["exploration"];
            preferences["curiosity_driven"] = curiosity;
            preferences["novelty_seeking"] = novelty;
            preferences["risk_avoidance"] = prediction_error * 0.5f;
            
            return preferences;
        }, "Get action preferences based on neuromodulation levels")
        .def("getDecisionMakingStats", [](const AgentBrain& self) {
            return std::map<std::string, py::object>{
                {"curiosity_level", self.getCuriosityLevel()},
                {"novelty_level", self.getNoveltyLevel()},
                {"prediction_error", self.getPredictionError()},
                {"neuromodulation_level", self.getNeuromodulationLevel()},
                {"developmental_stage", static_cast<int>(self.getDevelopmentalStage())}
            };
        }, "Get decision-making related statistics")
        
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
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain: " + std::to_string(agent.getNeuromodulationLevel()) +
                   " neuromodulation, " + std::to_string(agent.getCuriosityLevel()) +
                   " curiosity, " + std::to_string(agent.getNoveltyLevel()) + " novelty>";
        })
        .def("__enter__", [](std::shared_ptr<AgentBrain>& self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<AgentBrain>& self, py::args) {
            try {
                self->reset();
                return false; // Don't suppress exceptions
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, ("Error in AgentBrain context manager: " + std::string(e.what())).c_str());
                return false;
            }
        });

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

    // Predefined configuration templates
    m.def("createLearningAgentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Learning-focused configuration
        config->set("brain.neuron_count", 2000);
        config->set("brain.region_count", 3);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.001);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.dopamine.scale", 1.0);
        config->set("development.stage", "CriticalPeriod");
        config->set("memory.working_memory.capacity", 500);
        config->set("memory.episodic_memory.max_episodes", 1000);
        
        return config;
    }, "Create a learning-focused configuration template");

    m.def("createReactiveAgentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Reactive, fast-response configuration
        config->set("brain.neuron_count", 1000);
        config->set("brain.region_count", 2);
        config->set("plasticity.stdp.enable", false);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", false);
        config->set("neuromod.curiosity.enable", false);
        config->set("neuromod.novelty.enable", false);
        config->set("neuromod.dopamine.scale", 0.5);
        config->set("development.stage", "Adult");
        config->set("memory.working_memory.capacity", 200);
        config->set("memory.episodic_memory.max_episodes", 100);
        
        return config;
    }, "Create a reactive agent configuration template");

    m.def("createDevelopmentalAgentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Development-focused configuration
        config->set("brain.neuron_count", 3000);
        config->set("brain.region_count", 4);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.002);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.dopamine.scale", 2.0);
        config->set("development.stage", "Initial");
        config->set("development.synaptogenesis_rate", 0.01);
        config->set("development.pruning_rate", 0.001);
        config->set("memory.working_memory.capacity", 800);
        config->set("memory.episodic_memory.max_episodes", 5000);
        config->set("replay.enable", true);
        config->set("replay.interval", 100);
        config->set("consolidation.enable", true);
        config->set("consolidation.interval", 1000);
        
        return config;
    }, "Create a developmental agent configuration template");

    m.def("createMinimalConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Minimal configuration for testing
        config->set("brain.neuron_count", 100);
        config->set("brain.region_count", 1);
        config->set("plasticity.stdp.enable", false);
        config->set("plasticity.hebbian.enable", false);
        config->set("plasticity.structural.enable", false);
        config->set("neuromod.curiosity.enable", false);
        config->set("neuromod.novelty.enable", false);
        config->set("development.stage", "Adult");
        config->set("memory.working_memory.capacity", 50);
        config->set("memory.episodic_memory.max_episodes", 50);
        
        return config;
    }, "Create a minimal configuration template for testing");

    m.def("createAdvancedLearningConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Advanced learning configuration
        config->set("brain.neuron_count", 5000);
        config->set("brain.region_count", 6);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.001);
        config->set("plasticity.stdp.ltp_weight", 0.02);
        config->set("plasticity.stdp.ltd_weight", 0.02);
        config->set("plasticity.stdp.time_constant", 15.0);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.hebbian.learning_rate", 0.005);
        config->set("plasticity.structural.enable", true);
        config->set("plasticity.structural.threshold", 0.5);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.curiosity.base_rate", 0.1);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.novelty.sensitivity", 0.8);
        config->set("neuromod.dopamine.scale", 1.5);
        config->set("neuromod.dopamine.baseline", 0.1);
        config->set("development.stage", "CriticalPeriod");
        config->set("development.synaptogenesis_rate", 0.005);
        config->set("development.pruning_rate", 0.0005);
        config->set("memory.working_memory.capacity", 1000);
        config->set("memory.episodic_memory.max_episodes", 10000);
        config->set("replay.enable", true);
        config->set("replay.interval", 50);
        config->set("replay.consolidation", true);
        config->set("consolidation.enable", true);
        config->set("consolidation.interval", 500);
        config->set("prediction.error_enabled", true);
        config->set("prediction.horizon", 100);
        config->set("planning.enabled", true);
        config->set("planning.planning_horizon", 200);
        
        return config;
    }, "Create an advanced learning configuration template");

    m.def("createEfficientConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Efficient, memory-constrained configuration
        config->set("brain.neuron_count", 500);
        config->set("brain.region_count", 2);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.002);
        config->set("plasticity.hebbian.enable", false);
        config->set("plasticity.structural.enable", false);
        config->set("neuromod.curiosity.enable", false);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.dopamine.scale", 0.8);
        config->set("development.stage", "Adult");
        config->set("memory.working_memory.capacity", 100);
        config->set("memory.episodic_memory.max_episodes", 200);
        config->set("replay.enable", true);
        config->set("replay.interval", 200);
        config->set("consolidation.enable", true);
        config->set("consolidation.interval", 2000);
        
        return config;
    }, "Create an efficient configuration template for resource-constrained environments");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
