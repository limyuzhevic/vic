#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <limits>

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

// Custom Python exception classes for NLM errors
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidParameterError : public NLMError {
public:
    explicit InvalidParameterError(const std::string& paramName, const std::string& reason)
        : NLMError("Invalid parameter " + paramName + ": " + reason) {}
};

class InvalidNeuronIdError : public InvalidParameterError {
public:
    InvalidNeuronIdError(uint64_t id)
        : InvalidParameterError("neuron_id", "Invalid neuron ID: " + std::to_string(id)) {}
};

class InvalidSynapseIdError : public InvalidParameterError {
public:
    InvalidSynapseIdError(uint64_t id)
        : InvalidParameterError("synapse_id", "Invalid synapse ID: " + std::to_string(id)) {}
};

class InvalidRegionIdError : public InvalidParameterError {
public:
    InvalidRegionIdError(uint64_t id)
        : InvalidParameterError("region_id", "Invalid region ID: " + std::to_string(id)) {}
};

class InvalidPopulationIdError : public InvalidParameterError {
public:
    InvalidPopulationIdError(uint64_t id)
        : InvalidParameterError("population_id", "Invalid population ID: " + std::to_string(id)) {}
};

class InvalidValueRangeError : public InvalidParameterError {
public:
    InvalidValueRangeError(const std::string& paramName, double value, double min, double max)
        : InvalidParameterError(paramName, "Value " + std::to_string(value) + " is outside valid range [" + 
                                   std::to_string(min) + ", " + std::to_string(max) + "]") {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& reason)
        : NLMError("Memory error: " + reason) {}
};

class OperationFailedError : public NLMError {
public:
    explicit OperationFailedError(const std::string& operation, const std::string& reason)
        : NLMError("Operation failed " + operation + ": " + reason) {}
};

// Validation helper functions
static NeuronId validateNeuronId(const NeuronId& id) {
    if (id.value == UINT64_MAX) {
        throw InvalidNeuronIdError(UINT64_MAX);
    }
    return id;
}

static SynapseId validateSynapseId(const SynapseId& id) {
    if (id.value == UINT64_MAX) {
        throw InvalidSynapseIdError(UINT64_MAX);
    }
    return id;
}

static RegionId validateRegionId(const RegionId& id) {
    if (id.value == UINT64_MAX) {
        throw InvalidRegionIdError(UINT64_MAX);
    }
    return id;
}

static PopulationId validatePopulationId(const PopulationId& id) {
    if (id.value == UINT64_MAX) {
        throw InvalidPopulationIdError(UINT64_MAX);
    }
    return id;
}

static void validateRange(double value, double min, double max, const std::string& paramName) {
    if (value < min || value > max) {
        throw InvalidValueRangeError(paramName, value, min, max);
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<InvalidParameterError>(m, "InvalidParameterError");
    py::register_exception<InvalidNeuronIdError>(m, "InvalidNeuronIdError");
    py::register_exception<InvalidSynapseIdError>(m, "InvalidSynapseIdError");
    py::register_exception<InvalidRegionIdError>(m, "InvalidRegionIdError");
    py::register_exception<InvalidPopulationIdError>(m, "InvalidPopulationIdError");
    py::register_exception<InvalidValueRangeError>(m, "InvalidValueRangeError");
    py::register_exception<MemoryError>(m, "MemoryError");
    py::register_exception<OperationFailedError>(m, "OperationFailedError");

    // Strongly-typed identifiers with validation
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

    // Config class with error handling
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            try {
                bool result = self.loadFromFile(filepath);
                if (!result) {
                    throw OperationFailedError("loadFromFile", "Failed to load configuration from file: " + filepath);
                }
                return result;
            } catch (const std::exception& e) {
                throw OperationFailedError("loadFromFile", "Error loading configuration: " + std::string(e.what()));
            }
        }, py::arg("filepath"), "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                bool result = self.loadFromArgs(argc, argv);
                if (!result) {
                    throw OperationFailedError("loadFromArgs", "Failed to load configuration from arguments");
                }
                return result;
            } catch (const std::exception& e) {
                throw OperationFailedError("loadFromArgs", "Error loading configuration from arguments: " + std::string(e.what()));
            }
        }, py::arg("argc"), py::arg("argv"), "Load configuration from command line arguments")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            try {
                bool result = self.saveToFile(filepath);
                if (!result) {
                    throw OperationFailedError("saveToFile", "Failed to save configuration to file: " + filepath);
                }
                return result;
            } catch (const std::exception& e) {
                throw OperationFailedError("saveToFile", "Error saving configuration: " + std::string(e.what()));
            }
        }, py::arg("filepath"), "Save configuration to a JSON file")
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
        });

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class with parameter validation
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            try {
                if (data.empty()) {
                    throw InvalidParameterError("data", "Data vector cannot be empty");
                }
                if (data.size() != static_cast<size_t>(self.getWidth() * self.getHeight() * self.getChannels())) {
                    throw InvalidParameterError("data", "Data size does not match image dimensions");
                }
                self.setData(data);
            } catch (const InvalidParameterError& e) {
                throw;
            } catch (const std::exception& e) {
                throw OperationFailedError("setData", "Error setting vision data: " + std::string(e.what()));
            }
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio class with validation
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            try {
                if (data.empty()) {
                    throw InvalidParameterError("data", "Audio data vector cannot be empty");
                }
                if (data.size() != self.getNumSamples()) {
                    throw InvalidParameterError("data", "Audio data size does not match number of samples");
                }
                self.setData(data);
            } catch (const InvalidParameterError& e) {
                throw;
            } catch (const std::exception& e) {
                throw OperationFailedError("setData", "Error setting audio data: " + std::string(e.what()));
            }
        }, py::arg("data"))
        .def("setSampleRate", [](Audio& self, size_t sampleRate) {
            try {
                if (sampleRate == 0) {
                    throw InvalidParameterError("sampleRate", "Sample rate cannot be zero");
                }
                self.setSampleRate(sampleRate);
            } catch (const InvalidParameterError& e) {
                throw;
            } catch (const std::exception& e) {
                throw OperationFailedError("setSampleRate", "Error setting sample rate: " + std::string(e.what()));
            }
        }, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", [](InternalSignals& self, float value) {
            try {
                validateRange(value, -1e6f, 1e6f, "value");
                self.addSignal(value);
            } catch (const InvalidParameterError& e) {
                throw;
            } catch (const std::exception& e) {
                throw OperationFailedError("addSignal", "Error adding signal: " + std::string(e.what()));
            }
        }, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class with validation
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"),
             "Set the action type")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", [](Action& self, const std::vector<float>& params) {
            try {
                for (size_t i = 0; i < params.size(); ++i) {
                    validateRange(params[i], -1e6f, 1e6f, "parameters[" + std::to_string(i) + "]");
                }
                self.setParameters(params);
            } catch (const InvalidParameterError& e) {
                throw;
            } catch (const std::exception& e) {
                throw OperationFailedError("setParameters", "Error setting parameters: " + std::string(e.what()));
            }
        }, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // WorldObject class
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

    // AgentBody class
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

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", [](SensoryPercept& self, const Vision& vision) {
            try {
                self.setVision(vision);
            } catch (const std::exception& e) {
                throw OperationFailedError("setVision", "Error setting vision percept: " + std::string(e.what()));
            }
        }, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", [](SensoryPercept& self, const std::vector<float>& touch) {
            try {
                self.setTouch(touch);
            } catch (const std::exception& e) {
                throw OperationFailedError("setTouch", "Error setting touch percept: " + std::string(e.what()));
            }
        }, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", [](SensoryPercept& self, const std::vector<float>& internal) {
            try {
                self.setInternal(internal);
            } catch (const std::exception& e) {
                throw OperationFailedError("setInternal", "Error setting internal percept: " + std::string(e.what()));
            }
        }, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", [](SensoryPercept& self, const std::vector<float>& proprioception) {
            try {
                self.setProprioception(proprioception);
            } catch (const std::exception& e) {
                throw OperationFailedError("setProprioception", "Error setting proprioception percept: " + std::string(e.what()));
            }
        }, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", [](SensoryPercept& self, const Audio& audio) {
            try {
                self.setAudio(audio);
            } catch (const std::exception& e) {
                throw OperationFailedError("setAudio", "Error setting audio percept: " + std::string(e.what()));
            }
        }, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // SimpleWorld class with comprehensive error handling
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, size_t width, size_t height,
                            size_t visionWidth, size_t visionHeight) {
            try {
                if (width == 0 || height == 0) {
                    throw InvalidParameterError("dimensions", "World dimensions cannot be zero");
                }
                if (visionWidth == 0 || visionHeight == 0) {
                    throw InvalidParameterError("visionDimensions", "Vision dimensions cannot be zero");
                }
                if (visionWidth > width || visionHeight > height) {
                    throw InvalidParameterError("visionDimensions", "Vision dimensions cannot exceed world dimensions");
                }
                self.configure(width, height, visionWidth, visionHeight);
            } catch (const InvalidParameterError& e) {
                throw;
            } catch (const std::exception& e) {
                throw OperationFailedError("configure", "Error configuring world: " + std::string(e.what()));
            }
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
            "Configure the world with specified dimensions")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent start position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body")
        .def("addObject", [](SimpleWorld& self, const WorldObject& obj) {
            try {
                if (obj.x < 0 || obj.x > self.getWidth()) {
                    throw InvalidParameterError("object.x", "X coordinate out of bounds");
                }
                if (obj.y < 0 || obj.y > self.getHeight()) {
                    throw InvalidParameterError("object.y", "Y coordinate out of bounds");
                }
                self.addObject(obj);
            } catch (const InvalidParameterError& e) {
                throw;
            } catch (const std::exception& e) {
                throw OperationFailedError("addObject", "Error adding object: " + std::string(e.what()));
            }
        }, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", [](SimpleWorld& self, float e) {
            try {
                validateRange(e, 0.0f, 1e6f, "maxEnergy");
                self.setMaxEnergy(e);
            } catch (const InvalidParameterError& e) {
                throw;
            } catch (const std::exception& e) {
                throw OperationFailedError("setMaxEnergy", "Error setting max energy: " + std::string(e.what()));
            }
        }, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", [](SimpleWorld& self, float r) {
            try {
                validateRange(r, 0.0f, 1.0f, "decayRate");
                self.setEnergyDecayRate(r);
            } catch (const InvalidParameterError& e) {
                throw;
            } catch (const std::exception& e) {
                throw OperationFailedError("setEnergyDecayRate", "Error setting energy decay rate: " + std::string(e.what()));
            }
        }, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Brain class with comprehensive error handling for neuron operations
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", [](Brain& self) {
            try {
                bool result = self.initialize();
                if (!result) {
                    throw OperationFailedError("initialize", "Brain initialization failed");
                }
                return result;
            } catch (const std::exception& e) {
                throw OperationFailedError("initialize", "Error initializing brain: " + std::string(e.what()));
            }
        }, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
              py::arg("currentStep"),
              "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
              py::arg("currentStep"), py::arg("currentTime"),
              "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", [](Brain& self, const SensoryInput& input) {
            try {
                self.receiveSensoryInput(input);
            } catch (const std::exception& e) {
                throw OperationFailedError("receiveSensoryInput", "Error receiving sensory input: " + std::string(e.what()));
            }
        }, py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", [](Brain& self, NeuronId neuron, MembranePotential current) {
            try {
                validateNeuronId(neuron);
                validateRange(current, -1e6f, 1e6f, "current");
                self.injectCurrent(neuron, current);
            } catch (const InvalidNeuronIdError& e) {
                throw;
            } catch (const InvalidValueRangeError& e) {
                throw;
            } catch (const std::exception& e) {
                throw OperationFailedError("injectCurrent", "Error injecting current into neuron: " + std::string(e.what()));
            }
        }, py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", [](Brain& self, NeuronType type, MembranePotential current) {
            try {
                validateRange(current, -1e6f, 1e6f, "current");
                self.injectCurrentToNeurons(type, current);
            } catch (const InvalidValueRangeError& e) {
                throw;
            } catch (const std::exception& e) {
                throw OperationFailedError("injectCurrentToNeurons", "Error injecting current to neurons: " + std::string(e.what()));
            }
        }, py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
              "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
              "Reset brain state")
        .def("save", [](Brain& self, const std::string& filepath) {
            try {
                bool result = self.save(filepath);
                if (!result) {
                    throw OperationFailedError("save", "Failed to save brain state to file: " + filepath);
                }
                return result;
            } catch (const std::exception& e) {
                throw OperationFailedError("save", "Error saving brain state: " + std::string(e.what()));
            }
        }, py::arg("filepath"), "Save brain state to file")
        .def("load", [](Brain& self, const std::string& filepath) {
            try {
                bool result = self.load(filepath);
                if (!result) {
                    throw OperationFailedError("load", "Failed to load brain state from file: " + filepath);
                }
                return result;
            } catch (const std::exception& e) {
                throw OperationFailedError("load", "Error loading brain state: " + std::string(e.what()));
            }
        }, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", [](Brain& self, const std::string& name) {
            try {
                if (name.empty()) {
                    throw InvalidParameterError("name", "Region name cannot be empty");
                }
                RegionId regionId = self.addRegion(name);
                if (regionId.value == UINT64_MAX) {
                    throw MemoryError("Failed to create new region");
                }
                return regionId;
            } catch (const InvalidParameterError& e) {
                throw;
            } catch (const MemoryError& e) {
                throw;
            } catch (const std::exception& e) {
                throw OperationFailedError("addRegion", "Error adding region: " + std::string(e.what()));
            }
        }, py::arg("name") = "", "Add a new neural region")
        .def("getRegion", [](Brain& self, RegionId id) {
            try {
                validateRegionId(id);
                NeuralRegion* region = self.getRegion(id);
                if (!region) {
                    throw InvalidRegionIdError(id.value);
                }
                return region;
            } catch (const InvalidRegionIdError& e) {
                throw;
            } catch (const std::exception& e) {
                throw OperationFailedError("getRegion", "Error getting region: " + std::string(e.what()));
            }
        }, py::arg("id"), py::return_value_policy::reference_internal, "Get a region by ID")
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
        .def("setDevelopmentalStage", [](Brain& self, DevelopmentalStage stage) {
            try {
                self.setDevelopmentalStage(stage);
            } catch (const std::exception& e) {
                throw OperationFailedError("setDevelopmentalStage", "Error setting developmental stage: " + std::string(e.what()));
            }
        }, py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
              py::return_value_policy::reference_internal,
              "Get the configuration")
        .def("logStatus", &Brain::logStatus,
              "Log brain status");

    // AgentBrain class with validation for agent operations
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", [](AgentBrain& self, const SensoryPercept& percept) {
            try {
                self.processSensoryInput(percept);
            } catch (const std::exception& e) {
                throw OperationFailedError("processSensoryInput", "Error processing sensory input: " + std::string(e.what()));
            }
        }, py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", [](AgentBrain& self, float reward, float predictedReward) {
            try {
                validateRange(reward, -1e6f, 1e6f, "reward");
                validateRange(predictedReward, -1e6f, 1e6f, "predictedReward");
                self.applyRewardModulation(reward, predictedReward);
            } catch (const InvalidValueRangeError& e) {
                throw;
            } catch (const std::exception& e) {
                throw OperationFailedError("applyRewardModulation", "Error applying reward modulation: " + std::string(e.what()));
            }
        }, py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", [](AgentBrain& self, double timestep) {
            try {
                validateRange(timestep, 0.0, 1e6, "timestep");
                self.updateDevelopment(timestep);
            } catch (const InvalidValueRangeError& e) {
                throw;
            } catch (const std::exception& e) {
                throw OperationFailedError("updateDevelopment", "Error updating development: " + std::string(e.what()));
            }
        }, py::arg("timestep"), "Update development system")
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

    // Factory functions with error handling
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        try {
            return std::make_shared<Config>();
        } catch (const std::exception& e) {
            throw MemoryError("Failed to create default configuration: " + std::string(e.what()));
        }
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        try {
            if (!config) {
                throw InvalidParameterError("config", "Configuration cannot be null");
            }
            return std::make_shared<Brain>(config);
        } catch (const InvalidParameterError& e) {
            throw;
        } catch (const std::exception& e) {
            throw MemoryError("Failed to create brain: " + std::string(e.what()));
        }
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        try {
            return std::make_shared<SimpleWorld>();
        } catch (const std::exception& e) {
            throw MemoryError("Failed to create simple world: " + std::string(e.what()));
        }
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        try {
            if (!brain) {
                throw InvalidParameterError("brain", "Brain cannot be null");
            }
            return std::make_shared<AgentBrain>(brain);
        } catch (const InvalidParameterError& e) {
            throw;
        } catch (const std::exception& e) {
            throw MemoryError("Failed to create agent brain: " + std::string(e.what()));
        }
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm