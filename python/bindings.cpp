#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <system_error>

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

// Custom exception classes for better error reporting
class ValidationError : public std::runtime_error {
public:
    ValidationError(const std::string& message, int errorCode = 1000)
        : std::runtime_error(message), errorCode(errorCode) {}
    int getErrorCode() const { return errorCode; }
private:
    int errorCode;
};

class ResourceError : public std::runtime_error {
public:
    ResourceError(const std::string& message, int errorCode = 2000)
        : std::runtime_error(message), errorCode(errorCode) {}
    int getErrorCode() const { return errorCode; }
private:
    int errorCode;
};

class StateConsistencyError : public std::runtime_error {
public:
    StateConsistencyError(const std::string& message, int errorCode = 3000)
        : std::runtime_error(message), errorCode(errorCode) {}
    int getErrorCode() const { return errorCode; }
private:
    int errorCode;
};

class InvalidOperationError : public std::runtime_error {
public:
    InvalidOperationError(const std::string& message, int errorCode = 4000)
        : std::runtime_error(message), errorCode(errorCode) {}
    int getErrorCode() const { return errorCode; }
private:
    int errorCode;
};

// Exception registration function
void registerExceptionTypes(py::module_& m) {
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<ValidationError>(m, "ValidationError", "RuntimeError");
    py::register_exception<ResourceError>(m, "ResourceError", "RuntimeError");
    py::register_exception<StateConsistencyError>(m, "StateConsistencyError", "RuntimeError");
    py::register_exception<InvalidOperationError>(m, "InvalidOperationError", "RuntimeError");
}

// Helper function to convert C++ exception to Python exception
void translateCppException(const std::exception& e) {
    py::register_exception_translator([](std::exception_ptr p) {
        if (p) {
            try {
                std::rethrow_exception(p);
            } catch (const ValidationError& e) {
                throw py::value_error(e.what());
            } catch (const ResourceError& e) {
                throw py::value_error(e.what());
            } catch (const StateConsistencyError& e) {
                throw py::value_error(e.what());
            } catch (const InvalidOperationError& e) {
                throw py::value_error(e.what());
            } catch (const std::runtime_error& e) {
                throw py::value_error(e.what());
            } catch (const std::exception& e) {
                throw py::value_error("Unhandled exception: " + std::string(e.what()));
            }
        }
    });
}

// Helper function to validate input parameters
void validateInput(const std::string& paramName, const std::string& value, size_t maxLength = 1000) {
    if (value.empty()) {
        throw ValidationError("Empty value provided for parameter: " + paramName, 1001);
    }
    if (value.length() > maxLength) {
        throw ValidationError("Value for parameter " + paramName + " exceeds maximum length of " + std::to_string(maxLength) + " characters", 1002);
    }
    if (value.find_first_of("\\0") != std::string::npos) {
        throw ValidationError("Invalid characters detected in parameter " + paramName, 1003);
    }
}

// Helper function to check resource availability
void checkResourceAvailability(size_t requiredMemoryMB = 100) {
    // This is a placeholder - in a real implementation, you would check actual memory availability
    // For now, we'll just check if the required value is reasonable
    if (requiredMemoryMB > 1024) {
        throw ResourceError("Insufficient memory resources. Required: " + std::to_string(requiredMemoryMB) + "MB", 2001);
    }
}

// Helper function to validate state consistency
void validateStateConsistency(const Brain* brain) {
    if (!brain) {
        throw StateConsistencyError("Brain pointer is null", 3001);
    }
    
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t activeNeurons = brain->getActiveNeuronCount();
    
    if (totalNeurons == 0) {
        throw StateConsistencyError("Brain has no neurons initialized", 3002);
    }
    
    if (activeNeurons > totalNeurons) {
        throw StateConsistencyError("Inconsistent state: active neurons exceed total neurons", 3003);
    }
    
    // Check for reasonable neuron activity
    float firingRate = brain->getAverageFiringRate();
    if (firingRate < 0.0f || firingRate > 1000.0f) {
        throw StateConsistencyError("Unreasonable firing rate detected: " + std::to_string(firingRate), 3004);
    }
}

// Helper function to validate configuration
void validateConfiguration(const Config* config) {
    if (!config) {
        throw StateConsistencyError("Config pointer is null", 3101);
    }
    
    // Check for required keys
    std::vector<std::string> requiredKeys = {"neuron_count", "region_count"};
    for (const auto& key : requiredKeys) {
        if (!config->has(key)) {
            throw ValidationError("Missing required configuration key: " + key, 1101);
        }
    }
}

// Helper function to convert brain state to Python dict
py::dict brainToDict(const Brain* brain) {
    py::dict state;
    state["step"] = brain->getTotalSpikeCount();
    state["neurons"] = static_cast<size_t>(brain->getTotalNeuronCount());
    state["firing"] = brain->getFiringNeuronCount();
    state["stage"] = static_cast<int>(brain->getDevelopmentalStage());
    return state;
}

// Helper function to convert Config to Python dict
py::dict configToDict(const Config* config) {
    py::dict cfg;
    for (const auto& key : config->getKeys()) {
        // Try to get value based on type
        auto intVal = config->get<int64_t>(key);
        if (intVal.has_value()) {
            cfg[key] = *intVal;
        } else {
            auto doubleVal = config->get<double>(key);
            if (doubleVal.has_value()) {
                cfg[key] = *doubleVal;
            } else {
                auto boolVal = config->get<bool>(key);
                if (boolVal.has_value()) {
                    cfg[key] = *boolVal;
                } else {
                    auto strVal = config->get<std::string>(key);
                    if (strVal.has_value()) {
                        cfg[key] = *strVal;
                    }
                }
            }
        }
    }
    return cfg;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Register custom exception types
    registerExceptionTypes(m);
    
    // Setup exception translation
    translateCppException(std::current_exception());

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

    // Enhanced Config class with comprehensive error handling
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            try {
                validateInput("filepath", filepath, 4096);
                if (!self.loadFromFile(filepath)) {
                    throw ResourceError("Failed to load configuration from file: " + filepath, 2101);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Configuration load error: ") + e.what());
            }
        }, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                if (argc <= 0 || argv == nullptr) {
                    throw ValidationError("Invalid arguments for configuration loading", 1102);
                }
                if (!self.loadFromArgs(argc, argv)) {
                    throw ResourceError("Failed to load configuration from arguments", 2102);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Configuration load error: ") + e.what());
            }
        }, py::arg("argc"), py::arg("argv"),
             "Load configuration from command line arguments")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            try {
                validateInput("filepath", filepath, 4096);
                checkResourceAvailability(10);  // Minimum 10MB required
                if (!self.saveToFile(filepath)) {
                    throw ResourceError("Failed to save configuration to file: " + filepath, 2103);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Configuration save error: ") + e.what());
            }
        }, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", [](Config& self, const std::string& key) {
            try {
                validateInput("key", key, 256);
                return self.has(key);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Configuration check error: ") + e.what());
            }
        }, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", [](Config& self) {
            try {
                return self.getKeys();
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Configuration get keys error: ") + e.what());
            }
        }, "Get all configuration keys")
        .def("clear", [](Config& self) {
            try {
                self.clear();
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Configuration clear error: ") + e.what());
            }
        }, "Clear all configuration entries")
        .def("summary", [](Config& self) {
            try {
                return self.summary();
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Configuration summary error: ") + e.what());
            }
        }, "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced SensoryInput class with error handling
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", [](SensoryInput& self, double timestamp) {
            try {
                if (timestamp < 0.0) {
                    throw ValidationError("Timestamp cannot be negative", 1201);
                }
                self.setTimestamp(timestamp);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("SensoryInput timestamp error: ") + e.what());
            }
        }, py::arg("timestamp"), "Set the timestamp");

    // Enhanced Vision class with input validation
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
              py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            try {
                if (data.empty()) {
                    throw ValidationError("Vision data cannot be empty", 1202);
                }
                if (data.size() % 3 != 0) {
                    throw ValidationError("Vision data must be divisible by 3 (RGB channels)", 1203);
                }
                self.setData(data);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Vision data error: ") + e.what());
            }
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Enhanced Audio class with input validation
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            try {
                if (data.empty()) {
                    throw ValidationError("Audio data cannot be empty", 1203);
                }
                self.setData(data);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Audio data error: ") + e.what());
            }
        }, py::arg("data"))
        .def("setSampleRate", [](Audio& self, size_t sampleRate) {
            try {
                if (sampleRate == 0 || sampleRate > 192000) {
                    throw ValidationError("Invalid sample rate", 1204);
                }
                self.setSampleRate(sampleRate);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Audio sample rate error: ") + e.what());
            }
        }, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // Enhanced InternalSignals class with error handling
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", [](InternalSignals& self, float value) {
            try {
                if (std::isnan(value) || std::isinf(value)) {
                    throw ValidationError("Invalid signal value", 1205);
                }
                self.addSignal(value);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("InternalSignals add error: ") + e.what());
            }
        }, py::arg("value"))
        .def("clearSignals", [](InternalSignals& self) {
            try {
                self.clearSignals();
            } catch (const std::exception& e) {
                throw py::value_error(std::string("InternalSignals clear error: ") + e.what());
            }
        });

    // Enhanced Action class with validation
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", [](Action& self, ActionType type) {
            try {
                self.setType(type);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Action type error: ") + e.what());
            }
        }, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", [](Action& self, const std::vector<float>& params) {
            try {
                self.setParameters(params);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Action parameters error: ") + e.what());
            }
        }, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", [](const Action& self) {
            try {
                return self.clone();
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Action clone error: ") + e.what());
            }
        });

    // Enhanced WorldObject class with validation
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

    // Enhanced AgentBody class with validation
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
        .def("reset", [](AgentBody& self) {
            try {
                self.reset();
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBody reset error: ") + e.what());
            }
        });

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Enhanced SensoryPercept class with validation
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", [](SensoryPercept& self, const Vision& vision) {
            try {
                self.setVision(vision);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("SensoryPercept vision error: ") + e.what());
            }
        }, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", [](SensoryPercept& self, const std::vector<float>& touch) {
            try {
                self.setTouch(touch);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("SensoryPercept touch error: ") + e.what());
            }
        }, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", [](SensoryPercept& self, const std::vector<float>& internal) {
            try {
                self.setInternal(internal);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("SensoryPercept internal error: ") + e.what());
            }
        }, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", [](SensoryPercept& self, const std::vector<float>& proprioception) {
            try {
                self.setProprioception(proprioception);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("SensoryPercept proprioception error: ") + e.what());
            }
        }, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", [](SensoryPercept& self, const std::vector<float>& audio) {
            try {
                self.setAudio(audio);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("SensoryPercept audio error: ") + e.what());
            }
        }, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", [](SensoryPercept& self, double timestamp) {
            try {
                self.setTimestamp(timestamp);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("SensoryPercept timestamp error: ") + e.what());
            }
        }, py::arg("timestamp"));

    // Enhanced SimpleWorld class with comprehensive validation and error handling
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
            try {
                if (width == 0 || height == 0) {
                    throw ValidationError("World dimensions cannot be zero", 1301);
                }
                if (visionWidth > width || visionHeight > height) {
                    throw ValidationError("Vision dimensions cannot exceed world dimensions", 1302);
                }
                self.configure(width, height, visionWidth, visionHeight);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("World configure error: ") + e.what());
            }
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", [](SimpleWorld& self) {
            try {
                self.reset();
            } catch (const std::exception& e) {
                throw py::value_error(std::string("World reset error: ") + e.what());
            }
        })
        .def("setAgentStart", [](SimpleWorld& self, float x, float y) {
            try {
                if (x < 0 || x >= self.getWidth()) {
                    throw ValidationError("Agent start X position out of bounds", 1303);
                }
                if (y < 0 || y >= self.getHeight()) {
                    throw ValidationError("Agent start Y position out of bounds", 1304);
                }
                self.setAgentStart(x, y);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("World agent start error: ") + e.what());
            }
        }, py::arg("x"), py::arg("y"))
        .def("update", [](SimpleWorld& self, double timestep) {
            try {
                if (timestep <= 0.0) {
                    throw ValidationError("Time step must be positive", 1305);
                }
                self.update(timestep);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("World update error: ") + e.what());
            }
        }, py::arg("timestep"))
        .def("applyMotorCommand", [](SimpleWorld& self, const Action& cmd, double currentTime) {
            try {
                if (currentTime < 0.0) {
                    throw ValidationError("Current time cannot be negative", 1306);
                }
                self.applyMotorCommand(cmd, currentTime);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("World motor command error: ") + e.what());
            }
        }, py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", [](SimpleWorld& self, const WorldObject& obj) {
            try {
                if (obj.x < 0 || obj.x >= self.getWidth()) {
                    throw ValidationError("Object X position out of bounds", 1307);
                }
                if (obj.y < 0 || obj.y >= self.getHeight()) {
                    throw ValidationError("Object Y position out of bounds", 1308);
                }
                self.addObject(obj);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("World add object error: ") + e.what());
            }
        }, py::arg("obj"))
        .def("removeObject", [](SimpleWorld& self, float x, float y) {
            try {
                if (x < 0 || x >= self.getWidth()) {
                    throw ValidationError("Object X position out of bounds for removal", 1309);
                }
                if (y < 0 || y >= self.getHeight()) {
                    throw ValidationError("Object Y position out of bounds for removal", 1310);
                }
                self.removeObject(x, y);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("World remove object error: ") + e.what());
            }
        }, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", [](SimpleWorld& self, float e) {
            try {
                if (e < 0.0) {
                    throw ValidationError("Max energy cannot be negative", 1311);
                }
                self.setMaxEnergy(e);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("World max energy error: ") + e.what());
            }
        }, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", [](SimpleWorld& self, float r) {
            try {
                if (r < 0.0 || r > 1.0) {
                    throw ValidationError("Energy decay rate must be between 0 and 1", 1312);
                }
                self.setEnergyDecayRate(r);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("World energy decay rate error: ") + e.what());
            }
        }, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", [](SimpleWorld& self, uint64_t seed) {
            try {
                self.setRandomSeed(seed);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("World random seed error: ") + e.what());
            }
        }, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Brain context manager wrapper with enhanced error handling
    struct BrainContextWrapper {
        std::shared_ptr<Brain> brain;
        BrainContextWrapper(std::shared_ptr<Brain> b) : brain(std::move(b)) {}
        
        Brain& get() { return *brain; }
        
        py::dict getSimulationState() {
            try {
                validateStateConsistency(brain.get());
                return brainToDict(brain.get());
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain state retrieval error: ") + e.what());
            }
        }
        
        void setSimulationState(const py::dict& state) {
            try {
                validateStateConsistency(brain.get());
                brain->setDevelopmentalStage(static_cast<DevelopmentalStage>(
                    py::cast<int>(state["stage"])));
                if (state.contains("reset_spikes")) {
                    size_t spikes = py::cast<size_t>(state["reset_spikes"]);
                    if (spikes > 0) {
                        brain->reset();
                    }
                }
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain state setting error: ") + e.what());
            }
        }
    };
    
    py::class_<BrainContextWrapper>(m, "_BrainContextWrapper")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("get", &BrainContextWrapper::get, py::return_value_policy::reference_internal)
        .def("get_simulation_state", &BrainContextWrapper::getSimulationState,
             "Get current simulation state as a dictionary")
        .def("set_simulation_state", &BrainContextWrapper::setSimulationState,
             py::arg("state"), "Set simulation state from a dictionary");

    // Enhanced Brain class with comprehensive error handling
    py::class_<Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        // Context manager support
        .def("__enter__", [](Brain& self) {
            try {
                validateStateConsistency(&self);
                return py::cast(std::make_shared<BrainContextWrapper>(
                    std::shared_ptr<Brain>(&self, [](Brain*) {})));
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain context manager error: ") + e.what());
            }
        })
        .def("__exit__", [](Brain& self, const py::args&) {
            try {
                // Cleanup on context exit
                // This is where we would add any necessary cleanup logic
            } catch (const std::exception& e) {
                // Log error but don't propagate
                // In a real implementation, you might want to log this error
            }
        })
        // Convenience methods with Pythonic names
        .def("initialize", [](Brain& self) {
            try {
                validateStateConsistency(&self);
                validateConfiguration(self.getConfig().get());
                if (!self.initialize()) {
                    throw InvalidOperationError("Brain initialization failed", 4001);
                }
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain initialization error: ") + e.what());
            }
        }, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
              py::arg("step"),
              "Perform a simulation step")
        .def("step_with_time", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
              py::arg("step"), py::arg("time") = 0.0,
              "Perform a simulation step with timestamp")
        .def("inject_input", [](Brain& self, const SensoryInput& input) {
            try {
                validateStateConsistency(&self);
                self.receiveSensoryInput(input);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain input injection error: ") + e.what());
            }
        }, py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_neuron", [](Brain& self, const NeuronId& neuron, double current) {
            try {
                validateStateConsistency(&self);
                self.injectCurrent(neuron, static_cast<MembranePotential>(current));
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain neuron injection error: ") + e.what());
            }
        }, py::arg("neuron_id"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_by_type", [](Brain& self, NeuronType type, double current) {
            try {
                validateStateConsistency(&self);
                self.injectCurrentToNeurons(type, static_cast<MembranePotential>(current));
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain type injection error: ") + e.what());
            }
        }, py::arg("neuron_type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", [](Brain& self) {
            try {
                validateStateConsistency(&self);
                auto action = self.produceAction();
                if (!action) {
                    throw InvalidOperationError("Action production failed", 4002);
                }
                return action;
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain action production error: ") + e.what());
            }
        }, "Produce motor action based on neural activity")
        .def("reset", [](Brain& self) {
            try {
                validateStateConsistency(&self);
                self.reset();
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain reset error: ") + e.what());
            }
        }, "Reset brain state")
        .def("save", [](Brain& self, const std::string& filepath) {
            try {
                validateInput("filepath", filepath, 4096);
                checkResourceAvailability(50);  // Minimum 50MB required for brain save
                validateStateConsistency(&self);
                
                if (!self.save(filepath)) {
                    throw ResourceError("Failed to save brain state to file: " + filepath, 2201);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain save error: ") + e.what());
            }
        }, py::arg("filepath"),
             "Save brain state to file")
        .def("load", [](Brain& self, const std::string& filepath) {
            try {
                validateInput("filepath", filepath, 4096);
                checkResourceAvailability(50);  // Minimum 50MB required for brain load
                validateStateConsistency(&self);
                
                if (!self.load(filepath)) {
                    throw ResourceError("Failed to load brain state from file: " + filepath, 2202);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain load error: ") + e.what());
            }
        }, py::arg("filepath"),
             "Load brain state from file")
        // Region management
        .def("add_region", [](Brain& self, const std::string& name) {
            try {
                validateInput("name", name, 256);
                return self.addRegion(name);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain region addition error: ") + e.what());
            }
        }, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", [](Brain& self, const RegionId& regionId) {
            try {
                auto* region = self.getRegion(regionId);
                if (!region) {
                    throw InvalidOperationError("Region not found", 4003);
                }
                return region;
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain region retrieval error: ") + e.what());
            }
        }, py::arg("region_id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        // Statistics
        .def("total_neurons", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("total_synapses", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("active_neurons", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("firing_neurons", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("eih_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("total_spikes", &Brain::getTotalSpikeCount,
             "Get total spike count")
        // Development
        .def("developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", [](Brain& self, int stage) {
            try {
                validateStateConsistency(&self);
                self.setDevelopmentalStage(static_cast<DevelopmentalStage>(stage));
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain stage setting error: ") + e.what());
            }
        }, py::arg("stage"),
             "Set developmental stage")
        // Configuration
        .def("config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", [](Brain& self) {
            try {
                validateStateConsistency(&self);
                self.logStatus();
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain log status error: ") + e.what());
            }
        }, "Log brain status")
        // Batch operations
        .def("inject_current_batch", [](Brain& self, const std::vector<std::pair<NeuronId, double>>& injections) {
            try {
                validateStateConsistency(&self);
                for (const auto& inj : injections) {
                    self.injectCurrent(inj.first, static_cast<MembranePotential>(inj.second));
                }
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain batch injection error: ") + e.what());
            }
        }, py::arg("injections"),
             "Inject current into multiple neurons in a single batch call")
        .def("reset_all", [](Brain& self) {
            try {
                validateStateConsistency(&self);
                self.reset();
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain reset all error: ") + e.what());
            }
        }, "Reset brain state and all neurons")
        .def("get_all_neurons_summary", [](const Brain& self) {
            try {
                validateStateConsistency(&self);
                py::dict summary;
                summary["total"] = self.getTotalNeuronCount();
                summary["active"] = self.getActiveNeuronCount();
                summary["firing"] = self.getFiringNeuronCount();
                summary["eih_ratio"] = self.getExcitationInhibitionRatio();
                summary["total_spikes"] = self.getTotalSpikeCount();
                return summary;
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain summary error: ") + e.what());
            }
        }, "Get a summary of all neuron statistics")
        // Pythonic clone and copy support
        .def("clone", [](const Brain& self) -> std::shared_ptr<Brain> {
            try {
                validateStateConsistency(&self);
                auto config = self.getConfig();
                auto cloned = std::make_shared<Brain>(config);
                // Note: Brain doesn't have a save/load mechanism for cloning,
                // this is a basic clone that only shares configuration
                return cloned;
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain clone error: ") + e.what());
            }
        }, "Create a copy/clone of the brain with the same configuration")
        .def("copy_state", [](Brain& self, std::shared_ptr<Brain>& source) {
            try {
                validateStateConsistency(&self);
                if (!source) {
                    throw ValidationError("Source brain cannot be null", 1302);
                }
                // For now, just reset to source's developmental stage
                // Full state copying would require serialization support
                self.setDevelopmentalStage(source->getDevelopmentalStage());
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain copy state error: ") + e.what());
            }
        }, py::arg("source"), "Copy developmental state from another brain");

    // AgentBrain context manager
    struct AgentBrainContextWrapper {
        std::shared_ptr<AgentBrain> agent_brain;
        AgentBrainContextWrapper(std::shared_ptr<AgentBrain> ab) : agent_brain(std::move(ab)) {}
        
        AgentBrain& get() { return *agent_brain; }
        
        py::dict get_agent_state() {
            try {
                py::dict state;
                state["neuromodulation"] = agent_brain->getNeuromodulationLevel();
                state["curiosity"] = agent_brain->getCuriosityLevel();
                state["novelty"] = agent_brain->getNoveltyLevel();
                state["prediction_error"] = agent_brain->getPredictionError();
                state["development_stage"] = static_cast<int>(agent_brain->getDevelopmentalStage());
                state["reward_modulation_enabled"] = agent_brain->isRewardModulationEnabled();
                state["structural_plasticity_enabled"] = agent_brain->isStructuralPlasticityEnabled();
                state["development_enabled"] = agent_brain->isDevelopmentEnabled();
                state["curiosity_enabled"] = agent_brain->isCuriosityEnabled();
                return state;
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBrain state retrieval error: ") + e.what());
            }
        }
        
        void set_agent_state(const py::dict& state) {
            try {
                if (state.contains("neuromodulation")) {
                    agent_brain->applyRewardModulation(
                        py::cast<float>(state["neuromodulation"]),
                        py::cast<float>(state["neuromodulation"]));
                }
                if (state.contains("reward_modulation_enabled")) {
                    agent_brain->enableRewardModulation(py::cast<bool>(state["reward_modulation_enabled"]));
                }
                if (state.contains("structural_plasticity_enabled")) {
                    agent_brain->enableStructuralPlasticity(py::cast<bool>(state["structural_plasticity_enabled"]));
                }
                if (state.contains("development_enabled")) {
                    agent_brain->enableDevelopment(py::cast<bool>(state["development_enabled"]));
                }
                if (state.contains("curiosity_enabled")) {
                    agent_brain->enableCuriosity(py::cast<bool>(state["curiosity_enabled"]));
                }
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBrain state setting error: ") + e.what());
            }
        }
    };
    
    py::class_<AgentBrainContextWrapper>(m, "_AgentBrainContextWrapper")
        .def(py::init<std::shared_ptr<AgentBrain>>())
        .def("get", &AgentBrainContextWrapper::get, py::return_value_policy::reference_internal)
        .def("get_agent_state", &AgentBrainContextWrapper::get_agent_state,
             "Get agent brain state as a dictionary")
        .def("set_agent_state", &AgentBrainContextWrapper::set_agent_state,
             py::arg("state"), "Set agent brain state from a dictionary");

    // Enhanced AgentBrain class with comprehensive validation
    py::class_<AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        // Context manager support
        .def("__enter__", [](AgentBrain& self) {
            try {
                return py::cast(std::make_shared<AgentBrainContextWrapper>(
                    std::shared_ptr<AgentBrain>(&self, [](AgentBrain*) {})));
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBrain context manager error: ") + e.what());
            }
        })
        .def("__exit__", [](AgentBrain& self, const py::args&) {
            try {
                // Cleanup on context exit
                // This is where we would add any necessary cleanup logic
            } catch (const std::exception& e) {
                // Log error but don't propagate
                // In a real implementation, you might want to log this error
            }
        })
        // Convenience methods with Pythonic names
        .def("initialize", [](AgentBrain& self, const SimpleWorld& world) {
            try {
                validateStateConsistency(self.getBrain().get());
                validateConfiguration(self.getBrain()->getConfig().get());
                self.initialize(world);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBrain initialization error: ") + e.what());
            }
        }, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_input", [](AgentBrain& self, const SensoryPercept& percept) {
            try {
                validateStateConsistency(self.getBrain().get());
                self.processSensoryInput(percept);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBrain input processing error: ") + e.what());
            }
        }, py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_command", [](AgentBrain& self) {
            try {
                validateStateConsistency(self.getBrain().get());
                auto cmd = self.decodeMotorCommand();
                if (!cmd) {
                    throw InvalidOperationError("Command decoding failed", 4004);
                }
                return cmd;
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBrain command decoding error: ") + e.what());
            }
        }, "Decode brain motor activity into motor command")
        .def("apply_reward", [](AgentBrain& self, double reward, double predicted_reward) {
            try {
                validateStateConsistency(self.getBrain().get());
                self.applyRewardModulation(reward, predicted_reward);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBrain reward application error: ") + e.what());
            }
        }, py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation")
        .def("update_development", [](AgentBrain& self, double timestep) {
            try {
                validateStateConsistency(self.getBrain().get());
                if (timestep <= 0.0) {
                    throw ValidationError("Development timestep must be positive", 1401);
                }
                self.updateDevelopment(timestep);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBrain development update error: ") + e.what());
            }
        }, py::arg("timestep"),
             "Update development system")
        .def("developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", [](AgentBrain& self) {
            try {
                validateStateConsistency(self.getBrain().get());
                self.reset();
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBrain reset error: ") + e.what());
            }
        }, "Reset agent for new episode")
        .def("brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        // Configuration convenience methods
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled)
        // Agent state management
        .def("get_agent_state", [](AgentBrain& self) {
            try {
                validateStateConsistency(self.getBrain().get());
                py::dict state;
                state["neuromodulation"] = self.getNeuromodulationLevel();
                state["curiosity"] = self.getCuriosityLevel();
                state["novelty"] = self.getNoveltyLevel();
                state["prediction_error"] = self.getPredictionError();
                state["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
                state["reward_modulation_enabled"] = self.isRewardModulationEnabled();
                state["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
                state["development_enabled"] = self.isDevelopmentEnabled();
                state["curiosity_enabled"] = self.isCuriosityEnabled();
                return state;
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBrain state retrieval error: ") + e.what());
            }
        }, "Get complete agent brain state as a dictionary")
        .def("set_agent_state", [](AgentBrain& self, const py::dict& state) {
            try {
                validateStateConsistency(self.getBrain().get());
                if (state.contains("neuromodulation")) {
                    self.applyRewardModulation(
                        py::cast<double>(state["neuromodulation"]),
                        py::cast<double>(state["neuromodulation"]));
                }
                if (state.contains("reward_modulation_enabled")) {
                    self.enableRewardModulation(py::cast<bool>(state["reward_modulation_enabled"]));
                }
                if (state.contains("structural_plasticity_enabled")) {
                    self.enableStructuralPlasticity(py::cast<bool>(state["structural_plasticity_enabled"]));
                }
                if (state.contains("development_enabled")) {
                    self.enableDevelopment(py::cast<bool>(state["development_enabled"]));
                }
                if (state.contains("curiosity_enabled")) {
                    self.enableCuriosity(py::cast<bool>(state["curiosity_enabled"]));
                }
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBrain state setting error: ") + e.what());
            }
        }, py::arg("state"), "Set agent brain state from a dictionary")
        // Batch operations for AgentBrain
        .def("process_inputs_batch", [](AgentBrain& self, const std::vector<SensoryPercept>& percepts) {
            try {
                validateStateConsistency(self.getBrain().get());
                for (const auto& percept : percepts) {
                    self.processSensoryInput(percept);
                }
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBrain batch processing error: ") + e.what());
            }
        }, py::arg("percepts"),
             "Process multiple sensory percepts in a batch")
        .def("update_developments_batch", [](AgentBrain& self, const std::vector<double>& timesteps) {
            try {
                validateStateConsistency(self.getBrain().get());
                for (double timestep : timesteps) {
                    if (timestep <= 0.0) {
                        throw ValidationError("Development timestep must be positive", 1402);
                    }
                    self.updateDevelopment(timestep);
                }
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBrain batch development error: ") + e.what());
            }
        }, py::arg("timesteps"),
             "Update development system for multiple timesteps in a batch")
        // Error handling and validation
        .def("validate_state", [](AgentBrain& self) {
            try {
                validateStateConsistency(self.getBrain().get());
                validateConfiguration(self.getBrain()->getConfig().get());
                
                bool valid = true;
                std::string errors;
                
                if (!self.isRewardModulationEnabled() && self.getNeuromodulationLevel() > 0.0f) {
                    valid = false;
                    errors += "Reward modulation is disabled but neuromodulation level is high. ";
                }
                
                if (!self.isCuriosityEnabled() && self.getCuriosityLevel() > 0.0f) {
                    valid = false;
                    errors += "Curiosity is disabled but curiosity level is high. ";
                }
                
                return py::make_tuple(valid, errors);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBrain validation error: ") + e.what());
            }
        }, "Validate agent brain state and return (is_valid, error_message)")
        // Pythonic clone support
        .def("clone", [](const AgentBrain& self) -> std::shared_ptr<AgentBrain> {
            try {
                validateStateConsistency(self.getBrain().get());
                auto brain_clone = std::make_shared<Brain>(*self.getBrain());
                auto cloned = std::make_shared<AgentBrain>(brain_clone);
                return cloned;
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBrain clone error: ") + e.what());
            }
        }, "Create a copy/clone of the agent brain with its brain component")
        .def("copy_settings", [](AgentBrain& self, const AgentBrain& source) {
            try {
                validateStateConsistency(self.getBrain().get());
                validateStateConsistency(source.getBrain().get());
                
                self.enableRewardModulation(source.isRewardModulationEnabled());
                self.enableStructuralPlasticity(source.isStructuralPlasticityEnabled());
                self.enableDevelopment(source.isDevelopmentEnabled());
                self.enableCuriosity(source.isCuriosityEnabled());
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBrain copy settings error: ") + e.what());
            }
        }, py::arg("source"), "Copy configuration settings from another agent brain");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        try {
            checkResourceAvailability(10);  // Minimum 10MB required
            return std::make_shared<Config>();
        } catch (const std::exception& e) {
            throw py::value_error(std::string("Config creation error: ") + e.what());
        }
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        try {
            if (!config) {
                throw ValidationError("Config cannot be null", 1501);
            }
            checkResourceAvailability(100);  // Minimum 100MB required for brain
            validateConfiguration(config.get());
            return std::make_shared<Brain>(config);
        } catch (const std::exception& e) {
            throw py::value_error(std::string("Brain creation error: ") + e.what());
        }
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        try {
            checkResourceAvailability(10);  // Minimum 10MB required for world
            return std::make_shared<SimpleWorld>();
        } catch (const std::exception& e) {
            throw py::value_error(std::string("World creation error: ") + e.what());
        }
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        try {
            if (!brain) {
                throw ValidationError("Brain cannot be null", 1502);
            }
            checkResourceAvailability(150);  // Minimum 150MB required for agent brain
            return std::make_shared<AgentBrain>(brain);
        } catch (const std::exception& e) {
            throw py::value_error(std::string("AgentBrain creation error: ") + e.what());
        }
    }, py::arg("brain"), "Create a new agent brain interface");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
