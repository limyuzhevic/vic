#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
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

// Custom Python exception classes
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message)
        : std::runtime_error(message) {}
};

class ValidationException : public NLMException {
public:
    explicit ValidationException(const std::string& message)
        : NLMException("Validation Error: " + message) {}
};

class ConfigurationException : public NLMException {
public:
    explicit ConfigurationException(const std::string& message)
        : NLMException("Configuration Error: " + message) {}
};

class SimulationException : public NLMException {
public:
    explicit SimulationException(const std::string& message)
        : NLMException("Simulation Error: " + message) {}
};

class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message)
        : NLMException("Memory Error: " + message) {}
};

// Helper function to format error messages with context
inline std::string formatError(const std::string& operation, const std::string& context, const std::string& message) {
    std::ostringstream oss;
    oss << "[" << operation << "]";
    if (!context.empty()) oss << " Context: " << context;
    oss << " Error: " << message;
    return oss.str();
}

// Validation functions
inline uint64_t validateParam(uint64_t value, const std::string& name,
                              const std::string& operation,
                              uint64_t min = 0, uint64_t max = 0,
                              bool positive = false,
                              const std::string& context = "") {
    std::ostringstream err;
    if (positive && value < 0) {
        err << "Parameter '" << name << "' must be non-negative";
        throw ValidationException(formatError(operation, context, err.str()));
    }
    if (min > 0 && value < min) {
        err << "Parameter '" << name << "' must be >= " << min;
        throw ValidationException(formatError(operation, context, err.str()));
    }
    if (max > 0 && value > max) {
        err << "Parameter '" << name << "' must be <= " << max;
        throw ValidationException(formatError(operation, context, err.str()));
    }
    return value;
}

inline float validateParam(float value, const std::string& name,
                           const std::string& operation,
                           float min = std::numeric_limits<float>::min(),
                           float max = std::numeric_limits<float>::max(),
                           bool positive = false,
                           const std::string& context = "") {
    std::ostringstream err;
    if (positive && value < 0.0f) {
        err << "Parameter '" << name << "' must be non-negative";
        throw ValidationException(formatError(operation, context, err.str()));
    }
    if (min > std::numeric_limits<float>::min() && value < min) {
        err << "Parameter '" << name << "' must be >= " << min;
        throw ValidationException(formatError(operation, context, err.str()));
    }
    if (max > std::numeric_limits<float>::min() && value > max) {
        err << "Parameter '" << name << "' must be <= " << max;
        throw ValidationException(formatError(operation, context, err.str()));
    }
    return value;
}

inline size_t validateParam(size_t value, const std::string& name,
                            const std::string& operation,
                            size_t min = 0, size_t max = 0,
                            bool positive = false,
                            const std::string& context = "") {
    std::ostringstream err;
    if (positive && value < 0) {
        err << "Parameter '" << name << "' must be non-negative";
        throw ValidationException(formatError(operation, context, err.str()));
    }
    if (min > 0 && value < min) {
        err << "Parameter '" << name << "' must be >= " << min;
        throw ValidationException(formatError(operation, context, err.str()));
    }
    if (max > 0 && value > max) {
        err << "Parameter '" << name << "' must be <= " << max;
        throw ValidationException(formatError(operation, context, err.str()));
    }
    return value;
}

inline std::string validateString(const std::string& value, const std::string& name,
                                  const std::string& operation,
                                  size_t minLength = 0, size_t maxLength = 1000,
                                  const std::string& context = "") {
    std::ostringstream err;
    if (value.length() < minLength) {
        err << "Parameter '" << name << "' must be at least " << minLength << " characters";
        throw ValidationException(formatError(operation, context, err.str()));
    }
    if (value.length() > maxLength) {
        err << "Parameter '" << name << "' must be at most " << maxLength << " characters";
        throw ValidationException(formatError(operation, context, err.str()));
    }
    return value;
}

inline std::string validateKey(const std::string& key, const std::string& operation,
                              const std::string& context = "") {
    return validateString(key, "key", operation, 1, 100, context);
}

// Register custom Python exceptions
inline void registerExceptions(py::module_& m) {
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ValidationException>(m, "ValidationException");
    py::register_exception<ConfigurationException>(m, "ConfigurationException");
    py::register_exception<SimulationException>(m, "SimulationException");
    py::register_exception<MemoryException>(m, "MemoryException");
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Register custom Python exceptions
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ValidationException>(m, "ValidationException");
    py::register_exception<ConfigurationException>(m, "ConfigurationException");
    py::register_exception<SimulationException>(m, "SimulationException");
    py::register_exception<MemoryException>(m, "MemoryException");

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
        
        // Enhanced configuration methods
        .def("merge", [](Config& self, const Config& other) {
            // Merge another config into this one
            for (const auto& key : other.getKeys()) {
                if (auto val = other.get<std::string>(key); val.has_value()) {
                    self.set(key, *val);
                } else if (auto val = other.get<int>(key); val.has_value()) {
                    self.set(key, *val);
                } else if (auto val = other.get<double>(key); val.has_value()) {
                    self.set(key, *val);
                } else if (auto val = other.get<bool>(key); val.has_value()) {
                    self.set(key, *val);
                }
            }
            return &self;
        }, py::arg("other"),
             "Merge another configuration into this one (returns self for chaining)")
        
        .def("createTemplate", [](const Config& self) {
            return std::make_shared<Config>(self);
        }, "Create a configuration template copy")
        
        .def("validateSchema", [](Config& self, const std::string& schemaDef) {
            // Validate configuration against a simple schema definition
            // Schema format: "key:type|min|max|default"
            // Example: "neuron_count:int|100|10000|1000"
            try {
                std::istringstream iss(schemaDef);
                std::string key;
                if (!(iss >> key) || key.empty() || key[0] == '#') {
                    return false;  // Skip comments or empty lines
                }
                
                // Parse schema
                std::string typeStr, minStr, maxStr, defaultStr;
                char pipe;
                std::getline(iss >> std::ws, typeStr, '|');
                if (std::getline(iss >> std::ws, minStr, '|')) {
                    std::getline(iss >> std::ws, maxStr, '|');
                    std::getline(iss >> std::ws, defaultStr, '|');
                }
                
                // Validate type
                if (self.has(key)) {
                    auto& val = self.getKeys();
                    // Type checking would go here based on typeStr
                    // For now, just validate presence
                    return true;
                }
                
                // If key doesn't exist but has default, that's OK
                if (!defaultStr.empty()) {
                    return true;
                }
                
                return false; // Missing required key
            } catch (...) {
                return false;
            }
        }, py::arg("schemaDef"),
             "Validate configuration against a schema definition string")
        
        .def("getTemplate", [](const Config& self) {
            // Create a template version with only the structure, no values
            auto templateConfig = std::make_shared<Config>();
            for (const auto& key : self.getKeys()) {
                // Add key with a placeholder value
                templateConfig->set(key + "_placeholder", std::string(""));
            }
            return templateConfig;
        }, "Create a configuration template for schema generation")
        
        .def("validateWithSchema", [](const Config& self, const std::string& schemaFilepath) {
            // Load schema from file and validate
            // For now, placeholder implementation
            std::ifstream file(schemaFilepath);
            if (!file.is_open()) {
                throw ConfigurationException("Cannot open schema file: " + schemaFilepath);
            }
            // Read and parse schema file (simplified)
            std::string line;
            std::vector<std::string> schemaLines;
            while (std::getline(file, line)) {
                if (!line.empty() && line[0] != '#') {
                    schemaLines.push_back(line);
                }
            }
            
            // Basic validation - check for required keys
            for (const auto& line : schemaLines) {
                size_t pos = line.find(':');
                if (pos != std::string::npos) {
                    std::string key = line.substr(0, pos);
                    if (!self.has(key)) {
                        throw ConfigurationException("Missing required configuration key: " + key);
                    }
                }
            }
            return true;
        }, py::arg("schemaFilepath"),
             "Validate configuration against a schema file")
        
        .def("inheritFrom", [](Config& self, const Config& parent) {
            // Copy values from parent config where keys don't exist in self
            for (const auto& key : parent.getKeys()) {
                if (!self.has(key)) {
                    if (auto val = parent.get<std::string>(key); val.has_value()) {
                        self.set(key, *val);
                    } else if (auto val = parent.get<int>(key); val.has_value()) {
                        self.set(key, *val);
                    } else if (auto val = parent.get<double>(key); val.has_value()) {
                        self.set(key, *val);
                    } else if (auto val = parent.get<bool>(key); val.has_value()) {
                        self.set(key, *val);
                    }
                }
            }
            return &self;
        }, py::arg("parent"),
             "Inherit missing values from parent configuration (returns self for chaining)")
        
        .def("validate", [](Config& self) {
            // Validate all configuration values
            std::vector<std::string> errors;
            
            // Validate numeric ranges for known keys
            if (self.has("neuron_count")) {
                if (auto val = self.get<size_t>("neuron_count"); val.has_value()) {
                    if (*val < 1) {
                        errors.push_back("neuron_count must be >= 1");
                    } else if (*val > 1000000) {
                        errors.push_back("neuron_count must be <= 1,000,000");
                    }
                }
            }
            
            if (self.has("simulation_timestep")) {
                if (auto val = self.get<double>("simulation_timestep"); val.has_value()) {
                    if (*val <= 0.0) {
                        errors.push_back("simulation_timestep must be > 0");
                    } else if (*val > 0.1) {
                        errors.push_back("simulation_timestep must be <= 0.1");
                    }
                }
            }
            
            if (!errors.empty()) {
                std::ostringstream oss;
                oss << "Configuration validation failed with " << errors.size() << " error(s):\n";
                for (const auto& err : errors) {
                    oss << "  - " << err << "\n";
                }
                throw ConfigurationException(oss.str());
            }
            
            return true;
        }, "Validate all configuration values and throw if invalid")
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
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
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

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

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

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
        
        // Advanced learning rate control methods
        .def("setSTDPParameters", [](Brain& self, float ltpWeight, float ltdWeight, float tau) {
            validateParam(ltpWeight, "ltpWeight", "setSTDPParameters", 0.0f, 0.1f);
            validateParam(ltdWeight, "ltdWeight", "setSTDPParameters", 0.0f, 0.1f);
            validateParam(tau, "tau", "setSTDPParameters", 1.0f, 100.0f);
            
            auto* stdp = self.getSTDP();
            if (stdp) {
                stdp->configure(ltpWeight, ltdWeight, tau);
            }
        }, py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"),
             "Configure STDP learning rule parameters")
        
        .def("setHebbianLearningRate", [](Brain& self, float learningRate) {
            validateParam(learningRate, "learningRate", "setHebbianLearningRate", 0.0f, 1.0f);
            
            auto* hebbian = self.getHebbian();
            if (hebbian) {
                hebbian->setLearningRate(learningRate);
            }
        }, py::arg("learningRate"),
             "Set Hebbian learning rate")
        
        .def("setPlasticityFactor", [](Brain& self, float factor) {
            validateParam(factor, "plasticityFactor", "setPlasticityFactor", 0.0f, 2.0f);
            auto* stdp = self.getSTDP();
            if (stdp) {
                stdp->setPlasticityFactor(factor);
            }
        }, py::arg("factor"),
             "Set global plasticity factor (affects all plasticity rules)")
        
        // Plasticity rule configuration methods
        .def("enablePlasticityRule", [](Brain& self, const std::string& ruleName) {
            validateKey(ruleName, "enablePlasticityRule", "Brain");
            
            if (ruleName == "stdp") {
                auto* stdp = self.getSTDP();
                if (stdp) stdp->enable();
            } else if (ruleName == "hebbian") {
                auto* hebbian = self.getHebbian();
                if (hebbian) hebbian->enable();
            } else if (ruleName == "structural") {
                auto* structural = self.getStructuralPlasticity();
                if (structural) structural->enable();
            } else {
                throw ConfigurationException("Unknown plasticity rule: " + ruleName);
            }
        }, py::arg("ruleName"),
             "Enable a specific plasticity rule")
        
        .def("disablePlasticityRule", [](Brain& self, const std::string& ruleName) {
            validateKey(ruleName, "disablePlasticityRule", "Brain");
            
            if (ruleName == "stdp") {
                auto* stdp = self.getSTDP();
                if (stdp) stdp->disable();
            } else if (ruleName == "hebbian") {
                auto* hebbian = self.getHebbian();
                if (hebbian) hebbian->disable();
            } else if (ruleName == "structural") {
                auto* structural = self.getStructuralPlasticity();
                if (structural) structural->disable();
            } else {
                throw ConfigurationException("Unknown plasticity rule: " + ruleName);
            }
        }, py::arg("ruleName"),
             "Disable a specific plasticity rule")
        
        .def("getPlasticityRuleStatus", [](const Brain& self, const std::string& ruleName) {
            validateKey(ruleName, "getPlasticityRuleStatus", "Brain");
            
            if (ruleName == "stdp") {
                auto* stdp = self.getSTDP();
                return stdp ? stdp->isEnabled() : false;
            } else if (ruleName == "hebbian") {
                auto* hebbian = self.getHebbian();
                return hebbian ? hebbian->isEnabled() : false;
            } else if (ruleName == "structural") {
                auto* structural = self.getStructuralPlasticity();
                return structural ? structural->isEnabled() : false;
            } else {
                throw ConfigurationException("Unknown plasticity rule: " + ruleName);
            }
        }, py::arg("ruleName"),
             "Get the enabled status of a plasticity rule")
        
        // Developmental stage control methods
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        
        .def("advanceDevelopment", [](Brain& self, double timeStep) {
            validateParam(timeStep, "timeStep", "advanceDevelopment", 0.0, 1000.0);
            auto* dev = self.getDevelopmentSystem();
            if (dev) {
                dev->advanceDevelopment(timeStep);
            }
        }, py::arg("timeStep"),
             "Advance development system by specified time")
        
        .def("getCurrentDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        
        .def("getDevelopmentProgress", [](const Brain& self) {
            auto* dev = self.getDevelopmentSystem();
            if (dev) {
                return dev->getProgress();
            }
            return 0.0f;
        }, "Get development progress (0.0 to 1.0)")
        
        .def("isCriticalPeriod", [](const Brain& self) {
            return self.getDevelopmentalStage() == DevelopmentalStage::CriticalPeriod;
        }, "Check if current stage is critical period")
        
        // Comprehensive metrics collection
        .def("getMetrics", [](const Brain& self) {
            py::dict metrics;
            
            // Basic statistics
            metrics["total_neurons"] = self.getTotalNeuronCount();
            metrics["total_synapses"] = self.getTotalSynapseCount();
            metrics["active_neurons"] = self.getActiveNeuronCount();
            metrics["firing_neurons"] = self.getFiringNeuronCount();
            metrics["total_spikes"] = self.getTotalSpikeCount();
            metrics["avg_firing_rate"] = self.getAverageFiringRate();
            metrics["e_i_ratio"] = self.getExcitationInhibitionRatio();
            
            // Advanced metrics
            auto* stdp = self.getSTDP();
            if (stdp) {
                metrics["stdp_total_weight_change"] = stdp->getTotalWeightChange();
                metrics["stdp_avg_weight_magnitude"] = stdp->getAverageWeight();
            }
            
            auto* hebbian = self.getHebbian();
            if (hebbian) {
                metrics["hebbian_total_connections"] = hebbian->getTotalConnections();
                metrics["hebbian_avg_association_strength"] = hebbian->getAverageAssociation();
            }
            
            auto* structural = self.getStructuralPlasticity();
            if (structural) {
                metrics["structural_synaptogenesis_count"] = structural->getSynaptogenesisCount();
                metrics["structural_pruning_count"] = structural->getPruningCount();
            }
            
            // Memory metrics
            auto* workingMem = self.getWorkingMemory();
            if (workingMem) {
                metrics["working_memory_traces"] = workingMem->getActiveTraces();
                metrics["working_memory_capacity"] = workingMem->getCapacity();
            }
            
            auto* episodicMem = self.getEpisodicMemory();
            if (episodicMem) {
                metrics["episodic_memory_episodes"] = episodicMem->getEpisodeCount();
                metrics["episodic_memory_capacity"] = episodicMem->getMaxEpisodes();
            }
            
            // Neuromodulation metrics
            auto* dopamine = self.getDopamine();
            if (dopamine) {
                metrics["dopamine_level"] = dopamine->getLevel();
                metrics["dopamine_plasticity_factor"] = dopamine->getPlasticityFactor();
            }
            
            auto* curiosity = self.getCuriosity();
            if (curiosity) {
                metrics["curiosity_level"] = curiosity->getLevel();
            }
            
            auto* novelty = self.getNovelty();
            if (novelty) {
                metrics["novelty_level"] = novelty->getLevel();
            }
            
            auto* predError = self.getPredictionErrorSignal();
            if (predError) {
                metrics["prediction_error"] = predError->getError();
            }
            
            // Development metrics
            metrics["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            
            return metrics;
        }, "Get comprehensive metrics dictionary")
        
        .def("getPerformanceStats", [](const Brain& self) {
            py::dict stats;
            
            // Get metrics
            py::dict metrics = self.getMetrics();
            
            // Calculate derived statistics
            stats["efficiency"] = metrics["total_spikes"] / std::max(metrics["total_neurons"].cast<size_t>(), size_t(1));
            stats["stability"] = 1.0f - (metrics["e_i_ratio"].cast<float>() / 10.0f); // Normalized
            stats["plasticity"] = metrics["stdp_total_weight_change"].cast<float>();
            
            // Memory efficiency
            stats["memory_efficiency"] = static_cast<float>(metrics["working_memory_traces"].cast<size_t>()) / 
                                       std::max(metrics["working_memory_capacity"].cast<size_t>(), size_t(1));
            
            // Neuromodulation balance
            float dopamine = metrics["dopamine_level"].cast<float>();
            float curiosity = metrics["curiosity_level"].cast<float>();
            float novelty = metrics["novelty_level"].cast<float>();
            stats["modulation_balance"] = (dopamine + curiosity + novelty) / 3.0f;
            
            return stats;
        }, "Get performance statistics")
        
        .def("resetMetrics", [](Brain& self) {
            // Reset statistics counters
            auto* stdp = self.getSTDP();
            if (stdp) stdp->resetStatistics();
            
            auto* hebbian = self.getHebbian();
            if (hebbian) hebbian->resetStatistics();
            
            auto* structural = self.getStructuralPlasticity();
            if (structural) structural->resetStatistics();
            
            auto* workingMem = self.getWorkingMemory();
            if (workingMem) workingMem->clear();
            
            auto* episodicMem = self.getEpisodicMemory();
            if (episodicMem) episodicMem->clear();
        }, "Reset all statistics and metrics")
        
        // Additional methods from original binding
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
             "Log brain status");

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
        
        // Advanced neuromodulation control
        .def("applyAdvancedNeuromodulation", [](AgentBrain& self, float reward, float predictedReward,
                                                  float novelty, float curiosity, float exploration) {
            validateParam(reward, "reward", "applyAdvancedNeuromodulation", -1.0f, 1.0f);
            validateParam(predictedReward, "predictedReward", "applyAdvancedNeuromodulation", -1.0f, 1.0f);
            validateParam(novelty, "novelty", "applyAdvancedNeuromodulation", 0.0f, 1.0f);
            validateParam(curiosity, "curiosity", "applyAdvancedNeuromodulation", 0.0f, 1.0f);
            validateParam(exploration, "exploration", "applyAdvancedNeuromodulation", 0.0f, 1.0f);
            
            self.applyRewardModulation(reward, predictedReward);
            
            // Store advanced neuromodulation parameters
            self.setNoveltyLevel(novelty);
            self.setCuriosityLevel(curiosity);
            self.setExplorationLevel(exploration);
        }, py::arg("reward"), py::arg("predictedReward"), py::arg("novelty"),
             py::arg("curiosity"), py::arg("exploration"),
             "Apply advanced neuromodulation with multiple factors")
        
        .def("setNeuromodulationParameters", [](AgentBrain& self, const py::dict& params) {
            for (auto item : params) {
                std::string key = item.first.cast<std::string>();
                py::object value = item.second;
                
                if (key == "dopamine_level" && value.is<float>()) {
                    self.setDopamineLevel(value.cast<float>());
                } else if (key == "reward_prediction_error" && value.is<float>()) {
                    self.setPredictionError(value.cast<float>());
                } else if (key == "novelty_sensitivity" && value.is<float>()) {
                    self.setNoveltySensitivity(value.cast<float>());
                } else if (key == "curiosity_sensitivity" && value.is<float>()) {
                    self.setCuriositySensitivity(value.cast<float>());
                } else if (key == "exploration_rate" && value.is<float>()) {
                    self.setExplorationRate(value.cast<float>());
                }
            }
        }, py::arg("params"),
             "Set multiple neuromodulation parameters from a dictionary")
        
        .def("getNeuromodulationParameters", [](const AgentBrain& self) {
            py::dict params;
            
            params["dopamine_level"] = self.getNeuromodulationLevel();
            params["reward_prediction_error"] = self.getPredictionError();
            params["novelty_level"] = self.getNoveltyLevel();
            params["curiosity_level"] = self.getCuriosityLevel();
            params["exploration_level"] = self.getExplorationLevel();
            
            return params;
        }, "Get all neuromodulation parameters as dictionary")
        
        // Curiosity-driven behavior control
        .def("setCuriosityDrivenExploration", [](AgentBrain& self, bool enable, float threshold, float learningRate) {
            validateParam(threshold, "threshold", "setCuriosityDrivenExploration", 0.0f, 1.0f);
            validateParam(learningRate, "learningRate", "setCuriosityDrivenExploration", 0.0f, 1.0f);
            
            self.enableCuriosity(enable);
            self.setCuriosityThreshold(threshold);
            self.setCuriosityLearningRate(learningRate);
        }, py::arg("enable"), py::arg("threshold"), py::arg("learningRate"),
             "Enable or disable curiosity-driven exploration behavior")
        
        .def("triggerExplorationMode", [](AgentBrain& self, float intensity) {
            validateParam(intensity, "intensity", "triggerExplorationMode", 0.0f, 1.0f);
            
            if (self.isCuriosityEnabled()) {
                self.setExplorationLevel(std::min(intensity, 1.0f));
            }
        }, py::arg("intensity"),
             "Trigger exploration mode with specified intensity")
        
        .def("calculateExplorationScore", [](const AgentBrain& self, const std::vector<float>& stateHistory) {
            if (stateHistory.empty()) return 0.0f;
            
            // Calculate novelty from state history
            float totalVariation = 0.0f;
            for (size_t i = 1; i < stateHistory.size(); ++i) {
                totalVariation += std::abs(stateHistory[i] - stateHistory[i-1]);
            }
            
            float avgVariation = totalVariation / (stateHistory.size() - 1);
            return std::min(avgVariation * 2.0f, 1.0f); // Normalize to [0, 1]
        }, py::arg("stateHistory"),
             "Calculate exploration score from state history")
        
        // Meta-learning capabilities
        .def("enableMetaLearning", [](AgentBrain& self, bool enable, float metaLearningRate) {
            validateParam(metaLearningRate, "metaLearningRate", "enableMetaLearning", 0.0f, 1.0f);
            
            self.enableMetaLearning(enable);
            self.setMetaLearningRate(metaLearningRate);
        }, py::arg("enable"), py::arg("metaLearningRate"),
             "Enable or disable meta-learning capabilities")
        
        .def("updateMetaLearningModel", [](AgentBrain& self, const std::vector<float>& experience,
                                             const std::vector<float>& outcome) {
            if (self.isMetaLearningEnabled()) {
                // Store experience for meta-learning
                self.addExperienceToMetaLearning(experience, outcome);
            }
        }, py::arg("experience"), py::arg("outcome"),
             "Update meta-learning model with new experience and outcome")
        
        .def("getMetaLearningInsights", [](const AgentBrain& self) {
            py::dict insights;
            
            if (self.isMetaLearningEnabled()) {
                insights["experience_count"] = self.getMetaLearningExperienceCount();
                insights["learning_rate"] = self.getMetaLearningRate();
                insights["knowledge_gained"] = self.getMetaLearningKnowledgeGained();
                insights["adaptation_speed"] = self.getMetaLearningAdaptationSpeed();
            }
            
            return insights;
        }, "Get meta-learning insights and statistics")
        
        // Predictive model access
        .def("updatePredictiveModel", [](AgentBrain& self, const std::vector<float>& state,
                                           const std::vector<float>& action) {
            auto* predSys = self.getBrain()->getPredictionSystem();
            if (predSys) {
                predSys->updatePrediction(state, action);
            }
        }, py::arg("state"), py::arg("action"),
             "Update predictive model with current state and action")
        
        .def("getPrediction", [](const AgentBrain& self, const std::vector<float>& state) {
            auto* predSys = self.getBrain()->getPredictionSystem();
            if (predSys) {
                return predSys->getPrediction(state);
            }
            return std::vector<float>();
        }, py::arg("state"),
             "Get prediction for given state using predictive model")
        
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        
        .def("getModelConfidence", [](const AgentBrain& self, const std::vector<float>& state) {
            auto* predSys = self.getBrain()->getPredictionSystem();
            if (predSys) {
                return predSys->getPredictionConfidence(state);
            }
            return 0.0f;
        }, py::arg("state"),
             "Get confidence level of prediction for given state")
        
        // Goal management
        .def("setGoals", [](AgentBrain& self, const std::vector<std::string>& goals) {
            validateParam(static_cast<size_t>(goals.size()), "goals", "setGoals", 0, 10);
            
            self.clearGoals();
            for (const auto& goal : goals) {
                self.addGoal(goal);
            }
        }, py::arg("goals"),
             "Set multiple goals for the agent")
        
        .def("addGoal", [](AgentBrain& self, const std::string& goal, float priority, float deadline) {
            validateKey(goal, "addGoal", "AgentBrain");
            validateParam(priority, "priority", "addGoal", 0.0f, 1.0f);
            validateParam(deadline, "deadline", "addGoal", 0.0f, 1000000.0f);
            
            self.addGoal(goal, priority, deadline);
        }, py::arg("goal"), py::arg("priority"), py::arg("deadline"),
             "Add a new goal with priority and deadline")
        
        .def("getCurrentGoals", [](const AgentBrain& self) {
            return self.getGoals();
        }, "Get list of current goals")
        
        .def("getGoalProgress", [](const AgentBrain& self, const std::string& goal) {
            validateKey(goal, "getGoalProgress", "AgentBrain");
            return self.getGoalProgress(goal);
        }, py::arg("goal"),
             "Get progress on a specific goal")
        
        .def("getCompletedGoals", [](const AgentBrain& self) {
            return self.getCompletedGoals();
        }, "Get list of completed goals")
        
        .def("advanceGoals", [](AgentBrain& self, float timestep) {
            validateParam(timestep, "timestep", "advanceGoals", 0.0, 1000.0);
            self.advanceGoals(timestep);
        }, py::arg("timestep"),
             "Advance goal management system")
        
        .def("getPrimaryGoal", [](const AgentBrain& self) {
            return self.getPrimaryGoal();
        }, "Get primary goal")
        
        // Additional methods from original binding
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
