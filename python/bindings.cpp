#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

// Include all NLM headers
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

// Helper functions for better error handling and type conversions

// Python-friendly exception wrapper
class PyNLMError : public std::runtime_error {
public:
    explicit PyNLMError(const std::string& msg) : std::runtime_error(msg) {}
};

// Safe brain access helper
inline Brain* getBrainFromAgent(AgentBrain* agent) {
    if (!agent) {
        throw PyNLMError("AgentBrain is null");
    }
    return agent->getBrain();
}

// Safe region access helper
inline NeuralRegion* getSafeRegion(Brain* brain, const RegionId& regionId) {
    if (!brain) {
        throw PyNLMError("Brain is null");
    }
    NeuralRegion* region = brain->getRegion(regionId);
    if (!region) {
        throw PyNLMError("Region not found with ID: " + std::to_string(regionId.value));
    }
    return region;
}

// Helper for creating vision from data
inline void visionFromData(Vision& vision, const std::vector<float>& data) {
    if (data.size() != vision.getWidth() * vision.getHeight() * vision.getChannels()) {
        throw PyNLMError("Data size mismatch for Vision. "
                       "Expected " + std::to_string(vision.getWidth() * vision.getHeight() * vision.getChannels()) + 
                       " elements, got " + std::to_string(data.size()));
    }
    vision.setData(data);
}

// Helper for creating audio from data
inline void audioFromData(Audio& audio, const std::vector<float>& data) {
    if (data.size() != audio.getNumSamples()) {
        throw PyNLMError("Data size mismatch for Audio. "
                       "Expected " + std::to_string(audio.getNumSamples()) + 
                       " samples, got " + std::to_string(data.size()));
    }
    audio.setData(data);
}

// Python-friendly iterator helper for neurons
inline std::vector<Neuron*> getNeuronPointersFromRegion(NeuralRegion* region) {
    return region->getAllNeurons();
}

// Python-friendly iterator helper for synapses
inline std::vector<Synapse*> getSynapsePointersFromRegion(NeuralRegion* region) {
    std::vector<Synapse*> synapses;
    for (const auto& syn : region->getSynapses()) {
        synapses.push_back(syn.get());
    }
    return synapses;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        --------------------------------------------------------
        
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        Key improvements over the original bindings:
        • Better error handling with descriptive Python exceptions
        • More Pythonic interfaces with automatic conversions
        • Enhanced documentation and examples
        • Advanced features for power users
        • Better type safety and validation
        
        This module provides classes for Brain, Config, AgentBrain, SimpleWorld,
        SensoryInput, and Action, plus enhanced utilities for neural simulation.
    )pbdoc";

    // Register custom exception for better error handling
    py::register_exception<PyNLMError>(m, "NLMError", PyExc_RuntimeError);

    // Register original RuntimeError for compatibility
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ============================================================================
    // BASIC TYPES AND IDENTIFIERS
    // ============================================================================

    // NeuronId - Unique identifier for neurons
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
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // SynapseId - Unique identifier for synapses
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
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    // RegionId - Unique identifier for brain regions
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
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    // PopulationId - Unique identifier for neuron populations
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return "PopulationId(" + std::to_string(id.value) + ")";
        });

    // ============================================================================
    // ENUMERATIONS
    // ============================================================================

    // Neuron types
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron")
        .value("Motor", NeuronType::Motor, "Motor neuron")
        .value("Internal", NeuronType::Internal, "Internal neuron")
        .export_values();

    // Synapse types
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse")
        .export_values();

    // Developmental stages
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period stage")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    // Firing states
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Resting state")
        .value("Active", FiringState::Active, "Active state")
        .value("Refractory", FiringState::Refractory, "Refractory state")
        .value("Inhibited", FiringState::Inhibited, "Inhibited state")
        .export_values();

    // Action types
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

    // Motor commands
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

    // World object types
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ============================================================================
    // CONFIG CLASS
    // ============================================================================

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        
        // Configuration loading
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            bool success = self.loadFromFile(filepath);
            if (!success) {
                throw PyNLMError("Failed to load configuration from file: " + filepath);
            }
            return success;
        }, py::arg("filepath"), "Load configuration from a JSON file")
        
        // Enhanced loadFromArgs with better Python interface
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"), "Load configuration from command line arguments")
        
        // Configuration saving
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            bool success = self.saveToFile(filepath);
            if (!success) {
                throw PyNLMError("Failed to save configuration to file: " + filepath);
            }
            return success;
        }, py::arg("filepath"), "Save configuration to a JSON file")
        
        // Configuration access
        .def("has", &Config::has, py::arg("key"), "Check if a configuration key exists")
        .def("get", [](const Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw PyNLMError("Configuration key not found: " + key);
            }
            return self.getOr<std::string>(key, "");
        }, py::arg("key"), "Get configuration value as string (throws if not found)")
        .def("get_int", [](const Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw PyNLMError("Configuration key not found: " + key);
            }
            return self.getOr<int64_t>(key, 0);
        }, py::arg("key"), "Get configuration value as int64 (throws if not found)")
        .def("get_float", [](const Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw PyNLMError("Configuration key not found: " + key);
            }
            return self.getOr<double>(key, 0.0);
        }, py::arg("key"), "Get configuration value as float (throws if not found)")
        .def("get_bool", [](const Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw PyNLMError("Configuration key not found: " + key);
            }
            // For boolean values, check if key exists and return true if present
            // This is a simplified approach for boolean config values
            return self.has(key);
        }, py::arg("key"), "Check if configuration key exists (returns bool)")
        
        // Convenience methods - use Runtime source (default for Python)
        .def("set_int", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value);  // Uses ConfigSource::Runtime as default
        }, py::arg("key"), py::arg("value"), "Set configuration value as int64")
        
        .def("set_float", [](Config& self, const std::string& key, double value) {
            self.set(key, value);  // Uses ConfigSource::Runtime as default
        }, py::arg("key"), py::arg("value"), "Set configuration value as float")
        
        .def("set_string", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);  // Uses ConfigSource::Runtime as default
        }, py::arg("key"), py::arg("value"), "Set configuration value as string")
        
        // Configuration management
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        
        // Utility methods
        .def("summary", &Config::summary, "Get a summary string of the configuration")
        .def("validate", [](const Config& self) {
            // Basic validation - check for required keys
            std::vector<std::string> missing_keys;
            std::vector<std::string> keys = self.getKeys();
            
            for (const auto& key : keys) {
                if (key.find(".") == std::string::npos) {
                    missing_keys.push_back(key);
                }
            }
            
            if (!missing_keys.empty()) {
                throw PyNLMError("Configuration validation failed. Missing expected dotted keys: " +
                               (missing_keys.size() > 3 ? "too many to list" : 
                                [missing_keys]() {
                                    std::string result = "";
                                    for (size_t i = 0; i < missing_keys.size(); ++i) {
                                        if (i > 0) result += ", ";
                                        result += missing_keys[i];
                                    }
                                    return result;
                                }()));
            }
            
            return true;
        }, "Validate configuration structure")
        
        // Pythonic interface methods
        .def("items", [](const Config& self) {
            std::vector<std::pair<std::string, std::string>> items;
            for (const auto& key : self.getKeys()) {
                items.emplace_back(key, self.getOr<std::string>(key, ""));
            }
            return items;
        }, "Get all configuration key-value pairs as a list of tuples")
        
        .def("__len__", [](const Config& self) {
            return self.getKeys().size();
        }, "Get number of configuration entries")
        
        .def("__contains__", [](const Config& self, const std::string& key) {
            return self.has(key);
        }, "Check if configuration contains key (Python 'in' operator)")
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "String representation of configuration")
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        }, "Human-readable string representation");

    // ============================================================================
    // SENSORY INPUT CLASSES
    // ============================================================================

    // Base SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set the timestamp")
        .def("clone", [](const SensoryInput& self) {
            // Note: This is a simplified approach - actual implementation
            // would need to be more sophisticated for polymorphic classes
            throw PyNLMError("Clone not directly supported for SensoryInput base class");
        }, "Clone the sensory input (not supported for base class)");

    // Vision sensory input
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            visionFromData(self, data);
        }, py::arg("data"), "Set vision data with automatic validation")
        .def("getWidth", &Vision::getWidth, "Get vision width")
        .def("getHeight", &Vision::getHeight, "Get vision height")
        .def("getChannels", &Vision::getChannels, "Get number of color channels")
        .def("getImageSize", [](const Vision& self) {
            return self.getWidth() * self.getHeight() * self.getChannels();
        }, "Get total number of pixels/elements in the image")
        .def("normalize", [](Vision& self) {
            std::vector<float> data = self.getData();
            float min_val = *std::min_element(data.begin(), data.end());
            float max_val = *std::max_element(data.begin(), data.end());
            if (max_val > min_val) {
                for (float& val : data) {
                    val = (val - min_val) / (max_val - min_val);
                }
                self.setData(data);
            }
            return self;
        }, "Normalize vision data to [0, 1] range")
        .def("denormalize", [](Vision& self) {
            std::vector<float> data = self.getData();
            for (float& val : data) {
                val = val * 255.0f;  // Scale back to 0-255 range
            }
            self.setData(data);
            return self;
        }, "Denormalize vision data from [0, 1] to [0, 255]")
        .def("resize", [](Vision& self, size_t new_width, size_t new_height) {
            // Note: This would require actual implementation
            throw PyNLMError("Vision.resize() not implemented - requires image processing library");
        }, py::arg("new_width"), py::arg("new_height"), "Resize vision data (requires additional libraries)");

    // Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            audioFromData(self, data);
        }, py::arg("data"), "Set audio data with automatic validation")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples")
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get audio duration in seconds")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set audio sample rate")
        .def("normalize", [](Audio& self) {
            std::vector<float> data = self.getData();
            float max_val = *std::max_element(data.begin(), data.end(), 
                                            [](float a, float b) { return std::abs(a) < std::abs(b); });
            if (max_val > 0.0f) {
                for (float& val : data) {
                    val = val / max_val;
                }
                self.setData(data);
            }
            return self;
        }, "Normalize audio data to [-1, 1] range")
        .def("amplify", [](Audio& self, float factor) {
            std::vector<float> data = self.getData();
            for (float& val : data) {
                val = val * factor;
            }
            self.setData(data);
            return self;
        }, py::arg("factor"), "Amplify audio by given factor")
        .def("applyEffect", [](Audio& self, const std::string& effect_name, const std::vector<float>& params) {
            // Note: This would require actual audio processing implementation
            throw PyNLMError("Audio.applyEffect() not implemented - requires audio processing library");
        }, py::arg("effect_name"), py::arg("params"), "Apply audio effect (requires audio processing)");

    // Internal signals
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("getSignals", &InternalSignals::getData, "Get all signal values")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals")
        .def("getSignalCount", &InternalSignals::getDimensions, "Get number of signals")
        .def("getMaxSignal", [](const InternalSignals& self) {
            std::vector<float> signals = self.getData();
            if (signals.empty()) return 0.0f;
            return *std::max_element(signals.begin(), signals.end());
        }, "Get maximum signal value")
        .def("getMinSignal", [](const InternalSignals& self) {
            std::vector<float> signals = self.getData();
            if (signals.empty()) return 0.0f;
            return *std::min_element(signals.begin(), signals.end());
        }, "Get minimum signal value")
        .def("normalize", [](InternalSignals& self) {
            std::vector<float> signals = self.getData();
            float max_val = *std::max_element(signals.begin(), signals.end());
            float min_val = *std::min_element(signals.begin(), signals.end());
            if (max_val > min_val) {
                for (float& val : signals) {
                    val = (val - min_val) / (max_val - min_val);
                }
                self.clearSignals();
                for (float val : signals) {
                    self.addSignal(val);
                }
            }
            return self;
        }, "Normalize internal signals to [0, 1] range");

    // ============================================================================
    // ACTION CLASS
    // ============================================================================

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get human-readable action name")
        .def("getDescription", [](const Action& self) {
            return self.getName();  // Simplified - could include more description
        }, "Get action description")
        .def("clone", &Action::clone, "Clone this action")
        .def("isValid", [](const Action& self) {
            return self.getType() != ActionType::Custom || !self.getParameters().empty();
        }, "Check if action is valid")
        .def("to_dict", [](const Action& self) {
            return py::dict(
                "type", static_cast<int>(self.getType()),
                "parameters", self.getParameters(),
                "name", self.getName()
            );
        }, "Convert action to dictionary representation")
        .def("from_dict", [](Action& self, const py::dict& d) {
            if (d.contains("type")) {
                self.setType(static_cast<ActionType>(d["type"].cast<int>()));
            }
            if (d.contains("parameters")) {
                self.setParameters(d["parameters"].cast<std::vector<float>>());
            }
            return self;
        }, py::arg("dict"), "Create action from dictionary representation")
        .def("__repr__", [](const Action& self) {
            return "<Action: " + self.getName() + " (type=" + 
                   std::to_string(static_cast<int>(self.getType())) + ")>";
        })
        .def("__str__", [](const Action& self) {
            return self.getName();
        });

    // ============================================================================
    // WORLD OBJECT CLASS
    // ============================================================================

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        
        // Position and properties
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        
        // Distance calculations
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to point (x, y)")
        
        .def("distanceToObject", [](const WorldObject& self, const WorldObject& other) {
            return self.distanceTo(other.x, other.y);
        }, py::arg("other"), "Calculate distance to another world object")
        
        // Type checking
        .def("isResource", [](const WorldObject& self) {
            return self.type == WorldObjectType::Resource;
        }, "Check if object is a resource")
        
        .def("isHazard", [](const WorldObject& self) {
            return self.type == WorldObjectType::Hazard;
        }, "Check if object is a hazard")
        
        .def("isWall", [](const WorldObject& self) {
            return self.type == WorldObjectType::Wall;
        }, "Check if object is a wall")
        
        // Utility methods
        .def("containsPoint", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return (dx*dx + dy*dy) <= (self.radius * self.radius);
        }, py::arg("x"), py::arg("y"), "Check if point is inside object")
        
        .def("getBounds", [](const WorldObject& self) {
            return py::make_tuple(self.x - self.radius, self.y - self.radius,
                                 self.x + self.radius, self.y + self.radius);
        }, "Get object bounding box as (xmin, ymin, xmax, ymax)")
        
        .def("clone", [](const WorldObject& self) {
            return WorldObject(self.x, self.y, self.type, self.value, self.radius);
        }, "Clone world object")
        
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: " + std::to_string(static_cast<int>(obj.type)) +
                   " at (" + std::to_string(obj.x) + ", " + std::to_string(obj.y") + ")>";
        })
        .def("__str__", [](const WorldObject& obj) {
            std::string type_name;
            switch(obj.type) {
                case WorldObjectType::Empty: type_name = "Empty"; break;
                case WorldObjectType::Resource: type_name = "Resource"; break;
                case WorldObjectType::Hazard: type_name = "Hazard"; break;
                case WorldObjectType::Wall: type_name = "Wall"; break;
                case WorldObjectType::Marker: type_name = "Marker"; break;
                default: type_name = "Unknown";
            }
            return type_name + " at (" + std::to_string(obj.x) + ", " + std::to_string(obj.y) + ")";
        });

    // ============================================================================
    // AGENT BODY CLASS
    // ============================================================================

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        
        // Position and movement
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        
        // Energy and health
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        
        // State flags
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        
        // Utility methods
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get current speed")
        
        .def("getPosition", &AgentBody::x, "Get x position")
        .def("getY", &AgentBody::y, "Get y position")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        
        .def("getHeadingAngle", [](const AgentBody& self) {
            // Convert orientation to angle in radians
            return self.orientation * (M_PI / 180.0);  // Assuming orientation is degrees
        }, "Get heading angle in radians")
        
        .def("isNear", [](const AgentBody& self, float x, float y, float distance) {
            float dx = self.x - x;
            float dy = self.y - y;
            return (dx*dx + dy*dy) <= (distance * distance);
        }, py::arg("x"), py::arg("y"), py::arg("distance"), "Check if near position")
        
        .def("reset", &AgentBody::reset, "Reset agent body to initial state")
        
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: position=(" + std::to_string(body.x) + ", " + std::to_string(body.y") + ")>";
        })
        .def("__str__", [](const AgentBody& body) {
            return "Position: (" + std::to_string(body.x) + ", " + std::to_string(body.y) + ")";
        });

    // ============================================================================
    // ACTION RESULT CLASS
    // ============================================================================

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        
        // Utility methods
        .def("getSuccess", &ActionResult::success, "Get success status")
        .def("getReward", &ActionResult::reward, "Get reward value")
        .def("getMessage", &ActionResult::message, "Get result message")
        .def("setSuccess", &ActionResult::success, py::arg("success"), "Set success status")
        .def("setReward", &ActionResult::reward, py::arg("reward"), "Set reward value")
        .def("setMessage", &ActionResult::message, py::arg("message"), "Set result message")
        
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: success=" + std::string(result.success ? "true" : "false") +
                   ", reward=" + std::to_string(result.reward) + ", message=\"" + result.message + "\">";
        })
        .def("__str__", [](const ActionResult& result) {
            return "Success: " + std::string(result.success ? "Yes" : "No") +
                   ", Reward: " + std::to_string(result.reward);
        });

    // ============================================================================
    // SENSORY PERCEPT CLASS
    // ============================================================================

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        
        // Vision accessors
        .def("getVision", &SensoryPercept::getVision, "Get vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        
        // Touch accessors
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        
        // Internal signals accessors
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        
        // Proprioception accessors
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, 
             py::arg("proprioception"), "Set proprioception data")
        
        // Audio accessors
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        
        // All signals access
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all signals as dictionary")
        
        // Timestamp
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp")
        
        // Data validation and utility
        .def("hasVision", [](const SensoryPercept& self) {
            return self.getVisionWidth() > 0 && self.getVisionHeight() > 0;
        }, "Check if vision data exists")
        
        .def("hasTouch", [](const SensoryPercept& self) {
            const auto& touch = self.getTouch();
            return !touch.empty();
        }, "Check if touch data exists")
        
        .def("hasInternal", [](const SensoryPercept& self) {
            const auto& internal = self.getInternal();
            return !internal.empty();
        }, "Check if internal signals exist")
        
        .def("hasAudio", [](const SensoryPercept& self) {
            const auto& audio = self.getAudio();
            return !audio.empty();
        }, "Check if audio data exists")
        
        .def("isComplete", [](const SensoryPercept& self) {
            return self.hasVision() && self.hasTouch() && self.hasInternal();
        }, "Check if percept contains all major sensory modalities")
        
        .def("clone", [](const SensoryPercept& self) {
            SensoryPercept clone;
            clone.setVision(self.getVision());
            clone.setTouch(self.getTouch());
            clone.setInternal(self.getInternal());
            clone.setProprioception(self.getProprioception());
            clone.setAudio(self.getAudio());
            clone.setTimestamp(self.getTimestamp());
            return clone;
        }, "Clone sensory percept")
        
        .def("merge", [](SensoryPercept& self, const SensoryPercept& other) {
            self.setVision(other.getVision());
            self.setTouch(other.getTouch());
            self.setInternal(other.getInternal());
            self.setProprioception(other.getProprioception());
            self.setAudio(other.getAudio());
            return self;
        }, py::arg("other"), "Merge another percept into this one")
        
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: vision=" + std::to_string(percept.getVisionWidth()) + "x" + 
                   std::to_string(percept.getVisionHeight()) + ", touch=" + 
                   std::to_string(percept.getTouch().size()) + ", internal=" + 
                   std::to_string(percept.getInternal().size()) + ", audio=" + 
                   std::to_string(percept.getAudio().size()) + ">";
        })
        .def("__str__", [](const SensoryPercept& percept) {
            return "SensoryPercept( vision:" + std::to_string(percept.getVisionWidth()) + "x" + 
                   std::to_string(percept.getVisionHeight()) + 
                   ", touch:" + std::to_string(percept.getTouch().size()) + 
                   ", internal:" + std::to_string(percept.getInternal().size()) + 
                   ", audio:" + std::to_string(percept.getAudio().size()) + " )";
        });

    // ============================================================================
    // SIMPLE WORLD CLASS
    // ============================================================================

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        
        // Configuration
        .def("configure", &SimpleWorld::configure, 
             py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        
        // Control
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world by timestep")
        
        // Agent management
        .def("setAgentStart", &SimpleWorld::setAgentStart, 
             py::arg("x"), py::arg("y"), "Set agent starting position")
        
        // World object management
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, 
             py::arg("x"), py::arg("y"), "Remove object at position")
        
        // World validation
        .def("isValidPosition", &SimpleWorld::isValidPosition, 
             py::arg("x"), py::arg("y"), "Check if position is valid")
        
        // State access
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent body")
        
        // Properties
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, 
             py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        
        // Advanced features
        .def("getWorldObjects", [](SimpleWorld& self) {
            // Note: This would need to be implemented in the actual SimpleWorld class
            throw PyNLMError("getWorldObjects() not implemented - requires additional interface");
        }, "Get all world objects (requires implementation)")
        
        .def("getObjectsByType", [](SimpleWorld& self, WorldObjectType type) {
            // Note: This would need to be implemented in the actual SimpleWorld class
            throw PyNLMError("getObjectsByType() not implemented - requires additional interface");
        }, py::arg("type"), "Get objects by type (requires implementation)")
        
        .def("findNearestObject", [](SimpleWorld& self, float x, float y, WorldObjectType type = WorldObjectType::Empty) {
            // Note: This would need to be implemented in the actual SimpleWorld class
            throw PyNLMError("findNearestObject() not implemented - requires additional interface");
        }, py::arg("x"), py::arg("y"), py::arg("type") = WorldObjectType::Empty,
         "Find nearest object of specified type (requires implementation)")
        
        .def("clearObjects", [](SimpleWorld& self) {
            // Note: This would need to be implemented in the actual SimpleWorld class
            throw PyNLMError("clearObjects() not implemented - requires additional interface");
        }, "Clear all objects (requires implementation)")
        
        .def("hasObjects", [](SimpleWorld& self) {
            // Note: This would need to be implemented in the actual SimpleWorld class
            throw PyNLMError("hasObjects() not implemented - requires additional interface");
        }, "Check if world has objects (requires implementation)")
        
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + " @ t=" +
                   std::to_string(world.getSimulationTime()) + "s>";
        })
        .def("__str__", [](const SimpleWorld& world) {
            return "SimpleWorld( " + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + " , time=" +
                   std::to_string(world.getSimulationTime()) + " )";
        });

    // ============================================================================
    // AGENT BRAIN CLASS
    // ============================================================================

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Create agent brain with brain")
        
        // Initialization
        .def("initialize", &AgentBrain::initialize, 
             py::arg("world"), "Initialize with world")
        
        // Sensory and motor interfaces
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        
        // Neuromodulation
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        
        // Development
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        
        // Development stage
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        
        // Neuromodulation levels
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        
        // Subsystem control
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        
        // Subsystem status
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        
        // Advanced features
        .def("getMotorGroupActivities", &AgentBrain::getMotorGroupActivities,
             "Get detailed motor group activities")
        .def("getThoughtTrace", &AgentBrain::getThoughtTrace,
             "Get thought trace for debugging")
        
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal, "Get the underlying brain")
        
        // Phase 6 advanced features
        .def("encodeText", &AgentBrain::encodeText, py::arg("text"),
             "Encode text into neuron pattern (Phase 6 feature)")
        
        .def("getInternalFiringRates", &AgentBrain::getInternalFiringRates,
             "Get internal firing rates for text association")
        
        .def("getConceptCount", &AgentBrain::getConceptCount,
             "Get concept count (Phase 6 feature)")
        
        .def("getSelfModelConfidence", &AgentBrain::getSelfModelConfidence,
             "Get self-model confidence (Phase 6 feature)")
        
        .def("getSocialObservationCount", &AgentBrain::getSocialObservationCount,
             "Get social observation count (Phase 6 feature)")
        
        .def("observeOtherAgent", &AgentBrain::observeOtherAgent,
             py::arg("observed"), py::arg("beforeVision"), py::arg("afterVision"),
             "Observe other agent (Phase 6 feature)")
        
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        
        // Diagnostic counters
        .def("getReflexBonusCount", &AgentBrain::getReflexBonusCount,
             "Get reflex bonus count (Phase 5 diagnostic)")
        .def("getReflexSuppressCount", &AgentBrain::getReflexSuppressCount,
             "Get reflex suppress count (Phase 5 diagnostic)")
        .def("getWtaPulseCount", &AgentBrain::getWtaPulseCount,
             "Get WTA pulse count (Phase 5 diagnostic)")
        
        .def("getPlannerOverrideCount", &AgentBrain::getPlannerOverrideCount,
             "Get planner override count (Phase 6 diagnostic)")
        .def("getPlannerAgreementCount", &AgentBrain::getPlannerAgreementCount,
             "Get planner agreement count (Phase 6 diagnostic)")
        .def("getPlannerDecisionCount", &AgentBrain::getPlannerDecisionCount,
             "Get planner decision count (Phase 6 diagnostic)")
        .def("getConceptBiasCount", &AgentBrain::getConceptBiasCount,
             "Get concept bias count (Phase 6 diagnostic)")
        .def("getSelfBiasCount", &AgentBrain::getSelfBiasCount,
             "Get self bias count (Phase 6 diagnostic)")
        .def("getSocialBiasCount", &AgentBrain::getSocialBiasCount,
             "Get social bias count (Phase 6 diagnostic)")
        
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain: " + agent.getThoughtTrace() + ">";
        })
        .def("__str__", [](const AgentBrain& agent) {
            return "AgentBrain( curiosity=" + std::to_string(agent.getCuriosityLevel()) +
                   ", novelty=" + std::to_string(agent.getNoveltyLevel()) + " )";
        });

    // ============================================================================
    // BRAIN CLASS
    // ============================================================================

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), "Create brain with configuration")
        
        // Initialization
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        
        // Simulation control
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        
        // Input/output
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"), "Apply neuromodulatory signals")
        
        // State management
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        
        // Region management
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add a new neural region")
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"), py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal, "Get all regions")
        
        // Statistics and metrics
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount, "Get count of pending spike events")
        
        // Development
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("develop", &Brain::develop, "Apply developmental changes")
        
        // Memory systems
        .def("getWorkingMemory", &Brain::getWorkingMemory, "Get working memory")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory, "Get episodic memory")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory, "Get associative memory")
        
        // Prediction system
        .def("getPredictionSystem", &Brain::getPredictionSystem, "Get prediction system")
        
        // Cognition systems
        .def("getPlanner", &Brain::getPlanner, "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation, "Get concept formation system")
        .def("getAttention", &Brain::getAttention, "Get attentional selection system")
        
        // Neuromodulation systems
        .def("getDopamine", &Brain::getDopamine, "Get dopamine system")
        .def("getCuriosity", &Brain::getCuriosity, "Get curiosity system")
        .def("getNovelty", &Brain::getNovelty, "Get novelty system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal, "Get prediction error signal")
        
        // Configuration and state
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, "Get the configuration")
        .def("getRandomGenerator", &Brain::getRandomGenerator, "Get random generator")
        
        // Phase 5 learned preference
        .def("getLearnedPreference", &Brain::getLearnedPreference, "Get learned preference trait")
        .def("setLearnedPreference", &Brain::setLearnedPreference, py::arg("preference"), "Set learned preference trait")
        
        // System access
        .def("getSpikeSystem", &Brain::getSpikeSystem, "Get spike system")
        .def("getSTDP", &Brain::getSTDP, "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian, "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity, "Get structural plasticity system")
        .def("updatePlasticity", &Brain::updatePlasticity, "Update plasticity rules")
        
        // Logging and diagnostics
        .def("logStatus", &Brain::logStatus, "Log brain status")
        
        // Advanced features
        .def("addInterRegionConnection", &Brain::addInterRegionConnection,
             py::arg("source"), py::arg("target"), py::arg("weight") = 0.0f, py::arg("delay") = 1,
             "Add inter-region connection")
        .def("removeInterRegionConnection", &Brain::removeInterRegionConnection,
             py::arg("source"), py::arg("target"), "Remove inter-region connection")
        
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + brain.getConfig()->summary() + ">";
        })
        .def("__str__", [](const Brain& brain) {
            return "Brain( neurons=" + std::to_string(brain.getTotalNeuronCount()) +
                   ", synapses=" + std::to_string(brain.getTotalSynapseCount()) + 
                   ", stage=" + std::to_string(static_cast<int>(brain.getDevelopmentalStage())) + " )";
        });

    // ============================================================================
    // FACTORY FUNCTIONS
    // ============================================================================

    // Brain factory functions
    m.def("createBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createDefaultConfig", []() {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    // World factory functions
    m.def("createSimpleWorld", []() {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    // Agent factory function
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // ============================================================================
    // CONSTANTS AND UTILITIES
    // ============================================================================

    // Invalid identifiers
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Utility functions
    m.def("validateBrainConfiguration", [](const Config& config) {
        return config.validate();
    }, py::arg("config"), "Validate brain configuration");

    m.def("createNeuronId", [](uint64_t value) {
        return NeuronId(value);
    }, py::arg("value"), "Create neuron ID from value");

    m.def("createSynapseId", [](uint64_t value) {
        return SynapseId(value);
    }, py::arg("value"), "Create synapse ID from value");

    m.def("createRegionId", [](uint64_t value) {
        return RegionId(value);
    }, py::arg("value"), "Create region ID from value");

    // ============================================================================
    // ADVANCED UTILITIES
    // ============================================================================

    // Batch operations
    m.def("batchInjectCurrent", [](Brain& brain, const std::vector<NeuronId>& neurons, 
                                 float current) {
        for (const auto& neuron : neurons) {
            brain.injectCurrent(neuron, current);
        }
    }, py::arg("brain"), py::arg("neurons"), py::arg("current"), 
       "Inject current into multiple neurons efficiently");

    m.def("batchProcessSensory", [](AgentBrain& agent, const std::vector<SensoryPercept>& percepts) {
        for (const auto& percept : percepts) {
            agent.processSensoryInput(percept);
        }
    }, py::arg("agent"), py::arg("percepts"), 
       "Process multiple sensory percepts efficiently");

    // Simulation utilities
    m.def("runSimulation", [](Brain& brain, size_t steps, double timestep = 0.001) {
        for (size_t step = 0; step < steps; ++step) {
            brain.step(static_cast<SimulationStep>(step), timestep * step);
        }
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.001,
       "Run simulation for specified number of steps");

    m.def("runAgentSimulation", [](AgentBrain& agent, SimpleWorld& world, 
                                  size_t steps, double timestep = 0.1) {
        for (size_t step = 0; step < steps; ++step) {
            world.update(timestep);
            agent.processSensoryInput(world.getSensoryPercept());
            agent.getBrain()->step(static_cast<SimulationStep>(step));
            // Note: Action production and application would need to be added
        }
    }, py::arg("agent"), py::arg("world"), py::arg("steps"), py::arg("timestep") = 0.1,
       "Run agent simulation for specified number of steps");

    // Error handling utilities
    m.def("getLastError", []() {
        return std::string("No error recorded");
    }, "Get last error message (placeholder)");

    m.def("clearErrors", []() {
        // Clear any recorded errors
    }, "Clear recorded errors");

    // ============================================================================
    // VERSION AND METADATA
    // ============================================================================

    m.attr("__version__") = "1.0.0";  // This should match the project version
    m.attr("__description__") = "Enhanced Python bindings for NLM neural simulation framework";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__copyright__") = "Copyright (c) 2024 NLM Project";
}

} // namespace nlm
