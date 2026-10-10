#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <cmath>

namespace py = pybind11;
namespace nlm {

// Python-specific exception types
class NLMRuntimeError : public std::runtime_error {
public:
    explicit NLMRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMValidationError : public NLMRuntimeError {
public:
    explicit NLMValidationError(const std::string& msg) : NLMRuntimeError("Validation Error: " + msg) {}
};

class NLMFileError : public NLMRuntimeError {
public:
    explicit NLMFileError(const std::string& msg) : NLMRuntimeError("File Error: " + msg) {}
};

// Forward declarations
class Brain;
class Config;
class AgentBrain;
class SimpleWorld;
class NeuralRegion;
class NeuralPopulation;
class Neuron;
class Synapse;

// Parameter validation helper
void validateParameter(bool condition, const std::string& paramName, const std::string& message) {
    if (!condition) {
        throw NLMValidationError("Parameter '" + paramName + "': " + message);
    }
}

// File operation with error handling
bool safeFileOperation(const std::string& filepath, std::function<bool(const std::string&)> operation, 
                      const std::string& operationName) {
    try {
        validateParameter(!filepath.empty(), "filepath", "File path cannot be empty");
        
        std::ifstream file(filepath);
        if (!file.good()) {
            return operation(filepath);
        }
        file.close();
        return operation(filepath);
    } catch (const std::exception& e) {
        std::stringstream ss;
        ss << operationName << " failed for file " << filepath << ": " << e.what();
        throw NLMFileError(ss.str());
    }
}

// Context manager helper
class SimulationSession {
public:
    SimulationSession(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                     std::shared_ptr<AgentBrain> agentBrain)
        : brain_(brain), world_(world), agentBrain_(agentBrain), active_(false) {}
    
    ~SimulationSession() {
        if (active_) {
            try {
                pause();
            } catch (...) {
                // Ignore errors during destruction
            }
        }
    }
    
    void start() {
        if (active_) {
            throw NLMRuntimeError("SimulationSession already active");
        }
        validateParameter(brain_, "brain", "Brain cannot be null");
        validateParameter(world_, "world", "World cannot be null");
        
        // Initialize all components
        if (agentBrain_) {
            agentBrain_->initialize(*world_);
        }
        
        active_ = true;
    }
    
    void pause() {
        if (!active_) {
            throw NLMRuntimeError("SimulationSession not active");
        }
        active_ = false;
    }
    
    void resume() {
        if (active_) {
            throw NLMRuntimeError("SimulationSession already active");
        }
        active_ = true;
    }
    
    bool isActive() const { return active_; }
    
    std::shared_ptr<Brain> brain() const { return brain_; }
    std::shared_ptr<SimpleWorld> world() const { return world_; }
    std::shared_ptr<AgentBrain> agentBrain() const { return agentBrain_; }
    
    std::string getSessionInfo() const {
        std::stringstream ss;
        ss << "SimulationSession: active=" << (active_ ? "true" : "false") << 
              ", brain neurons=" << (brain_ ? std::to_string(brain_->getTotalNeuronCount()) : "0") << 
              ", world size=" << (world_ ? std::to_string(world_->getWidth()) + "x" + std::to_string(world_->getHeight()) : "0x0") << ";" ;
        return ss.str();
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agentBrain_;
    bool active_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        Includes Pythonic features, enhanced error handling, and advanced simulation capabilities.
    )pbdoc";

    // Register Python-specific exception types
    py::register_exception<NLMValidationError>(m, "NLMValidationError", PyExc_ValueError);
    py::register_exception<NLMFileError>(m, "NLMFileError", PyExc_IOError);
    py::register_exception<NLMRuntimeError>(m, "NLMRuntimeError", PyExc_RuntimeError);

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // NeuronId class with enhanced Python features
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
        })
        .def("to_dict", [](const NeuronId& id) {
            return py::dict("value"_a = id.value);
        });

    // SynapseId class with enhanced Python features
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
        })
        .def("to_dict", [](const SynapseId& id) {
            return py::dict("value"_a = id.value);
        });

    // RegionId class with enhanced Python features
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
        })
        .def("to_dict", [](const RegionId& id) {
            return py::dict("value"_a = id.value);
        });

    // PopulationId class with enhanced Python features
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return "PopulationId(" + std::to_string(id.value) + ")";
        })
        .def("to_dict", [](const PopulationId& id) {
            return py::dict("value"_a = id.value);
        });

    // Enums
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

    // Config class with Pythonic features
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            safeFileOperation(filepath, 
                [&](const std::string& fp) { return self.saveToFile(fp); },
                "saveToFile");
        }, py::arg("filepath"),
             "Save configuration to a JSON file")
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
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        })
        .def("to_dict", [](const Config& cfg) {
            py::dict result;
            auto keys = cfg.getKeys();
            for (const auto& key : keys) {
                auto value = cfg.getOr<std::string>(key, "");
                result[key] = value;
            }
            return result;
        })
        .def("get", [](Config& self, const std::string& key) {
            auto value = self.getOr<std::string>(key, "");
            return value;
        }, py::arg("key"),
             "Get configuration value")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set configuration value")
        .def("set_int", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set configuration integer value")
        .def("set_float", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set configuration float value")
        .def("set_bool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set configuration boolean value");

    // SensoryInput class with Pythonic features
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput: type=" + std::to_string(input.getType()) + ">";
        })
        .def("__str__", [](const SensoryInput& input) {
            return "SensoryInput(type=" + std::to_string(input.getType()) + ")";
        });

    // Vision class with Pythonic features
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
        .def("__repr__", [](const Vision& v) {
            return "<Vision: " + std::to_string(v.getWidth()) + "x" + std::to_string(v.getHeight()) + " @ " + std::to_string(v.getChannels()) + " channels>";
        })
        .def("__str__", [](const Vision& v) {
            return "Vision(width=" + std::to_string(v.getWidth()) + ", height=" + std::to_string(v.getHeight()) + ")";
        })
        .def("normalize", [](Vision& self, float min = 0.0f, float max = 1.0f) {
            auto data = self.getData();
            std::transform(data.begin(), data.end(), data.begin(),
                         [min, max](float val) { return (val - min) / (max - min); });
            self.setData(data);
        }, py::arg("min") = 0.0f, py::arg("max") = 1.0f,
             "Normalize vision data to range [min, max]")
        .def("to_dict", [](const Vision& v) {
            return py::dict(
                "width"_a = v.getWidth(),
                "height"_a = v.getHeight(),
                "channels"_a = v.getChannels(),
                "data"_a = v.getData()
            );
        });

    // Audio class with Pythonic features
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("__repr__", [](const Audio& a) {
            return "<Audio: " + std::to_string(a.getSampleRate()) + " Hz, " + std::to_string(a.getNumSamples()) + " samples>";
        })
        .def("__str__", [](const Audio& a) {
            return "Audio(sampleRate=" + std::to_string(a.getSampleRate()) + ", samples=" + std::to_string(a.getNumSamples()) + ")";
        });

    // InternalSignals class with Pythonic features
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("__repr__", [](const InternalSignals& s) {
            return "<InternalSignals>";
        })
        .def("to_dict", [](const InternalSignals& s) {
            return py::dict("signals"_a = s.getData());
        });

    // Action class with Pythonic features and JSON serialization
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", [](const Action& action) {
            return action.clone();
        })
        .def("__repr__", [](const Action& a) {
            return "<Action: " + a.getName() + ">";
        })
        .def("__str__", [](const Action& a) {
            return "Action(" + a.getName() + ")";
        })
        .def("to_dict", [](const Action& a) {
            return py::dict(
                "type"_a = a.getType(),
                "parameters"_a = a.getParameters(),
                "name"_a = a.getName()
            );
        })
        .def("to_json", [](const Action& a) {
            py::dict result;
            result["type"] = static_cast<int>(a.getType());
            result["parameters"] = a.getParameters();
            result["name"] = a.getName();
            return py::str(py::json(result).dump());
        });

    // WorldObject class with Pythonic features
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
        .def_readwrite("active", &WorldObject::active)
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: type=" + std::to_string(static_cast<int>(obj.type)) + ", pos=(" + std::to_string(obj.x) + "," + std::to_string(obj.y) + ")>";
        })
        .def("__str__", [](const WorldObject& obj) {
            return "WorldObject(type=" + std::to_string(static_cast<int>(obj.type)) + ", x=" + std::to_string(obj.x) + ")";
        })
        .def("distance_to", [](const WorldObject& obj, float x, float y) {
            return std::sqrt(std::pow(obj.x - x, 2) + std::pow(obj.y - y, 2));
        }, py::arg("x"), py::arg("y"),
             "Calculate distance to a point")
        .def("is_near", [](const WorldObject& obj, float x, float y, float distance) {
            return distance_to(obj, x, y) <= distance;
        }, py::arg("x"), py::arg("y"), py::arg("distance"),
             "Check if object is near a point");

    // AgentBody class with Pythonic features
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
        .def("reset", &AgentBody::reset)
        .def("distance_to", [](const AgentBody& body, float x, float y) {
            return std::sqrt(std::pow(body.x - x, 2) + std::pow(body.y - y, 2));
        }, py::arg("x"), py::arg("y"),
             "Calculate distance to a point")
        .def("move_towards", [](AgentBody& body, float targetX, float targetY, float speed, double dt) {
            float dx = targetX - body.x;
            float dy = targetY - body.y;
            float distance = std::sqrt(dx*dx + dy*dy);
            if (distance > 0.001f) {
                body.x += dx / distance * speed * dt;
                body.y += dy / distance * speed * dt;
                body.orientation = std::atan2(dy, dx);
                body.isMoving = true;
            }
        }, py::arg("targetX"), py::arg("targetY"), py::arg("speed"), py::arg("dt"),
             "Move agent towards target position")
        .def("to_dict", [](const AgentBody& body) {
            py::dict result;
            result["x"] = body.x;
            result["y"] = body.y;
            result["orientation"] = body.orientation;
            result["velocityX"] = body.velocityX;
            result["velocityY"] = body.velocityY;
            result["energy"] = body.energy;
            result["health"] = body.health;
            result["age"] = body.age;
            return result;
        });

    // ActionResult class with Pythonic features
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
              py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("is_good", [](const ActionResult& result) {
            return result.success && result.reward > 0.0f;
        })
        .def("is_bad", [](const ActionResult& result) {
            return !result.success && result.reward < 0.0f;
        })
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: reward=" + std::to_string(result.reward) + ", success=" + std::to_string(result.success) + ">";
        })
        .def("__str__", [](const ActionResult& result) {
            return "ActionResult(reward=" + std::to_string(result.reward) + ", success=" + std::to_string(result.success) + ")";
        });

    // SensoryPercept class with Pythonic features
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept>";
        })
        .def("normalize_vision", [](SensoryPercept& percept, float min = 0.0f, float max = 1.0f) {
            auto vision = percept.getVision();
            std::transform(vision.begin(), vision.end(), vision.begin(),
                         [min, max](float val) { return (val - min) / (max - min); });
            percept.setVision(vision);
        }, py::arg("min") = 0.0f, py::arg("max") = 1.0f,
             "Normalize vision data")
        .def("to_dict", [](const SensoryPercept& percept) {
            py::dict result;
            result["vision"] = percept.getVision();
            result["touch"] = percept.getTouch();
            result["internal"] = percept.getInternal();
            result["proprioception"] = percept.getProprioception();
            result["audio"] = percept.getAudio();
            return result;
        });

    // SimpleWorld class with Pythonic features and batch processing
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
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("step_many", [](SimpleWorld& self, double timestep, size_t steps) {
            for (size_t i = 0; i < steps; ++i) {
                self.update(timestep);
            }
        }, py::arg("timestep"), py::arg("steps"),
             "Advance simulation by multiple steps")
        .def("get_objects_in_radius", [](const SimpleWorld& self, float x, float y, float radius) {
            std::vector<WorldObject> result;
            for (const auto& obj : self.objects_) {
                float dist = std::sqrt(std::pow(obj.x - x, 2) + std::pow(obj.y - y, 2));
                if (dist <= radius) {
                    result.push_back(obj);
                }
            }
            return result;
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
             "Get all objects within a radius")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" + std::to_string(world.getHeight()) + ">";
        })
        .def("__str__", [](const SimpleWorld& world) {
            return "SimpleWorld(width=" + std::to_string(world.getWidth()) + ", height=" + std::to_string(world.getHeight()) + ")";
        })
        .def("to_dict", [](const SimpleWorld& self) {
            py::dict result;
            result["width"] = self.getWidth();
            result["height"] = self.getHeight();
            result["agent_x"] = self.getAgentBody().x;
            result["agent_y"] = self.getAgentBody().y;
            result["agent_orientation"] = self.getAgentBody().orientation;
            return result;
        });

    // NeuralRegion class with Pythonic features
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Neural region containing neurons and synapses)pbdoc")
        .def(py::init<RegionId>(), py::arg("id"))
        .def(py::init<RegionId, std::string>(), py::arg("id"), py::arg("name"))
        .def("getId", &NeuralRegion::getId)
        .def("getName", &NeuralRegion::getName)
        .def("setName", &NeuralRegion::setName, py::arg("name"))
        .def("getPopulationCount", &NeuralRegion::getPopulationCount)
        .def("getSynapseCount", &NeuralRegion::getSynapseCount)
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount)
        .def("getActiveNeuronCount", &NeuralRegion::getActiveNeuronCount)
        .def("getFiringNeuronCount", &NeuralRegion::getFiringNeuronCount)
        .def("getAverageFiringRate", &NeuralRegion::getAverageFiringRate)
        .def("getAverageSynapticWeight", &NeuralRegion::getAverageSynapticWeight)
        .def("getSynapticDensity", &NeuralRegion::getSynapticDensity)
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("size"), py::arg("type") = NeuronType::Internal)
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"))
        .def("getRegions", &NeuralRegion::getRegions)
        .def("addSynapse", &NeuralRegion::addSynapse, py::arg("source"), py::arg("destination"), 
              py::arg("weight") = 0.0f, py::arg("delay") = 1)
        .def("getSynapse", &NeuralRegion::getSynapse, py::arg("id"))
        .def("getSynapsesFrom", &NeuralRegion::getSynapsesFrom, py::arg("neuron"))
        .def("getSynapsesTo", &NeuralRegion::getSynapsesTo, py::arg("neuron"))
        .def("addInterRegionConnection", &Brain::addInterRegionConnection,
              py::arg("source"), py::arg("target"), py::arg("weight") = 0.0f, py::arg("delay") = 1)
        .def("removeInterRegionConnection", &Brain::removeInterRegionConnection,
              py::arg("source"), py::arg("target"))
        .def("step", &NeuralRegion::step, py::arg("currentTime"))
        .def("reset", &NeuralRegion::reset)
        .def("to_dict", [](const NeuralRegion& region) {
            py::dict result;
            result["id"] = region.getId().index();
            result["name"] = region.getName();
            result["population_count"] = region.getPopulationCount();
            result["synapse_count"] = region.getSynapseCount();
            result["neuron_count"] = region.getTotalNeuronCount();
            result["active_neuron_count"] = region.getActiveNeuronCount();
            result["firing_neuron_count"] = region.getFiringNeuronCount();
            result["average_firing_rate"] = region.getAverageFiringRate();
            result["average_synaptic_weight"] = region.getAverageSynapticWeight();
            result["synaptic_density"] = region.getSynapticDensity();
            return result;
        })
        .def("__repr__", [](const NeuralRegion& region) {
            return "<NeuralRegion: id=" + std::to_string(region.getId().index()) + ", name=\"" + region.getName() + "\">";
        })
        .def("__str__", [](const NeuralRegion& region) {
            return "NeuralRegion(id=" + std::to_string(region.getId().index()) + ", name=\"" + region.getName() + "\")";
        });

    // NeuralPopulation class with Pythonic features
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Neural population of neurons)pbdoc")
        .def("getId", &NeuralPopulation::getId)
        .def("getSize", &NeuralPopulation::getSize)
        .def("isEmpty", &NeuralPopulation::isEmpty)
        .def("getNeuronType", &NeuralPopulation::getNeuronType)
        .def("setNeuronType", &NeuralPopulation::setNeuronType, py::arg("type"))
        .def("getActiveCount", &NeuralPopulation::getActiveCount)
        .def("getFiringCount", &NeuralPopulation::getFiringCount)
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate)
        .def("getActivityLevel", &NeuralPopulation::getActivityLevel)
        .def("getNeuronIds", &NeuralPopulation::getNeuronIds)
        .def("getNeurons", &NeuralPopulation::getNeurons)
        .def("step", &NeuralPopulation::step, py::arg("currentTime"))
        .def("reset", &NeuralPopulation::reset)
        .def("recordActivity", &NeuralPopulation::recordActivity)
        .def("getMeanMembranePotential", &NeuralPopulation::getMeanMembranePotential)
        .def("getVarianceMembranePotential", &NeuralPopulation::getVarianceMembranePotential)
        .def("__repr__", [](const NeuralPopulation& pop) {
            return "<NeuralPopulation: id=" + std::to_string(pop.getId().index()) + ", size=" + std::to_string(pop.getSize()) + ">";
        })
        .def("__str__", [](const NeuralPopulation& pop) {
            return "NeuralPopulation(id=" + std::to_string(pop.getId().index()) + ", size=" + std::to_string(pop.getSize()) + ")";
        })
        .def("to_dict", [](const NeuralPopulation& pop) {
            py::dict result;
            result["id"] = pop.getId().index();
            result["size"] = pop.getSize();
            result["is_empty"] = pop.isEmpty();
            result["neuron_type"] = static_cast<int>(pop.getNeuronType());
            result["active_count"] = pop.getActiveCount();
            result["firing_count"] = pop.getFiringCount();
            result["average_firing_rate"] = pop.getAverageFiringRate();
            result["activity_level"] = pop.getActivityLevel();
            return result;
        });

    // Neuron class with Pythonic features
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Individual neuron model)pbdoc")
        .def("getId", &Neuron::getId)
        .def("getType", &Neuron::getType)
        .def("setType", &Neuron::setType, py::arg("type"))
        .def("getMembranePotential", &Neuron::getMembranePotential)
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"))
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"))
        .def("getThreshold", &Neuron::getThreshold)
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"))
        .def("isFiring", &Neuron::isFiring)
        .def("isRefractory", &Neuron::isRefractory)
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"))
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"))
        .def("decrementRefractory", &Neuron::decrementRefractory)
        .def("getFiringRate", &Neuron::getFiringRate)
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"))
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"))
        .def("getLeakConductance", &Neuron::getLeakConductance)
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod)
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"))
        .def("getRestingPotential", &Neuron::getRestingPotential)
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("potential"))
        .def("checkThreshold", &Neuron::checkThreshold)
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime)
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"))
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"))
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"))
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"))
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"))
        .def("getTotalCurrent", &Neuron::getTotalCurrent)
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent)
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"))
        .def("getSpikeHistory", &Neuron::getSpikeHistory)
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory)
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("handle"))
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("handle"))
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses)
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses)
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"))
        .def("getRegionId", &Neuron::getRegionId)
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"))
        .def("getPopulationId", &Neuron::getPopulationId)
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"))
        .def("step", &Neuron::step, py::arg("currentTime"))
        .def("reset", &Neuron::reset)
        .def("initializeRandom", [](Neuron& neuron, std::function<float()> rng) {
            // This is a simplified version - real implementation would need RandomGenerator
            neuron.reset();
        }, py::arg("rng"),
             "Initialize neuron with random parameters")
        .def("__repr__", [](const Neuron& n) {
            return "<Neuron: id=" + std::to_string(n.getId().index()) + ", type=" + std::to_string(static_cast<int>(n.getType())) + ">";
        })
        .def("__str__", [](const Neuron& n) {
            return "Neuron(id=" + std::to_string(n.getId().index()) + ", type=" + std::to_string(static_cast<int>(n.getType())) + ")";
        })
        .def("to_dict", [](const Neuron& n) {
            py::dict result;
            result["id"] = n.getId().index();
            result["type"] = static_cast<int>(n.getType());
            result["membrane_potential"] = n.getMembranePotential();
            result["threshold"] = n.getThreshold();
            result["is_firing"] = n.isFiring();
            result["is_refractory"] = n.isRefractory();
            result["firing_rate"] = n.getFiringRate();
            result["last_spike_time"] = n.getLastSpikeTime();
            return result;
        });

    // Synapse class with Pythonic features
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synaptic connection between neurons)pbdoc")
        .def("getId", &Synapse::getId)
        .def("getSourceNeuronId", &Synapse::getSourceNeuronId)
        .def("getTargetNeuronId", &Synapse::getTargetNeuronId)
        .def("getWeight", &Synapse::getWeight)
        .def("setWeight", &Synapse::setWeight, py::arg("weight"))
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"))
        .def("getType", &Synapse::getType)
        .def("setType", &Synapse::setType, py::arg("type"))
        .def("getDelay", &Synapse::getDelay)
        .def("setDelay", &Synapse::setDelay, py::arg("delay"))
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags)
        .def("enablePlasticity", &Synapse::enablePlasticity, 
              py::arg("hebbian"), py::arg("stdp"), py::arg("reward_modulated"))
        .def("step", &Synapse::step, py::arg("currentTime"))
        .def("reset", &Synapse::reset)
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace)
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("rate"))
        .def("__repr__", [](const Synapse& s) {
            return "<Synapse: id=" + std::to_string(s.getId().index()) + ", weight=" + std::to_string(s.getWeight()) + ">";
        })
        .def("__str__", [](const Synapse& s) {
            return "Synapse(id=" + std::to_string(s.getId().index()) + ", weight=" + std::to_string(s.getWeight()) + ")";
        })
        .def("to_dict", [](const Synapse& s) {
            py::dict result;
            result["id"] = s.getId().index();
            result["source_id"] = s.getSourceNeuronId().index();
            result["target_id"] = s.getTargetNeuronId().index();
            result["weight"] = s.getWeight();
            result["type"] = static_cast<int>(s.getType());
            result["delay"] = s.getDelay();
            return result;
        });

    // Brain class with Pythonic features
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
        .def("step_many", [](Brain& self, size_t steps, double timestep = 1.0) {
            for (size_t i = 0; i < steps; ++i) {
                self.step(i, timestep * i);
            }
        }, py::arg("steps"), py::arg("timestep") = 1.0,
             "Perform multiple simulation steps")
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
        .def("save", [](Brain& self, const std::string& filepath) {
            safeFileOperation(filepath,
                [&](const std::string& fp) { return self.save(fp); },
                "save");
        }, py::arg("filepath"),
             "Save brain state to file")
        .def("load", [](Brain& self, const std::string& filepath) {
            safeFileOperation(filepath,
                [&](const std::string& fp) { return self.load(fp); },
                "load");
        }, py::arg("filepath"),
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
        def("getRegions", &Brain::getRegions,
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
        .def("getNeuronIds", &Brain::getNeuronIds,
             "Get all neuron IDs")
        .def("setGoal", &Brain::setGoal, py::arg("goal"),
             "Set a goal for the brain")
        .def("getGoal", &Brain::getGoal,
             "Get current goal")
        .def("isActive", &Brain::isActive,
             "Check if brain is active")
        .def("pause", &Brain::pause,
             "Pause brain simulation")
        .def("resume", &Brain::resume,
             "Resume brain simulation")
        .def("to_dict", [](const Brain& brain) {
            py::dict result;
            result["total_neuron_count"] = brain.getTotalNeuronCount();
            result["total_synapse_count"] = brain.getTotalSynapseCount();
            result["active_neuron_count"] = brain.getActiveNeuronCount();
            result["firing_neuron_count"] = brain.getFiringNeuronCount();
            result["average_firing_rate"] = brain.getAverageFiringRate();
            result["excitement_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
            result["total_spike_count"] = brain.getTotalSpikeCount();
            result["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
            result["region_count"] = brain.getRegionCount();
            return result;
        })
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + " neurons, " + std::to_string(brain.getRegionCount()) + " regions>";
        })
        .def("__str__", [](const Brain& brain) {
            return "Brain(neurons=" + std::to_string(brain.getTotalNeuronCount()) + ", regions=" + std::to_string(brain.getRegionCount()) + ")";
        });

    // AgentBrain class with Pythonic features
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
        .def("to_dict", [](const AgentBrain& agentBrain) {
            py::dict result;
            result["sensory_input_size"] = agentBrain.getSensoryInputSize();
            result["motor_output_size"] = agentBrain.getMotorOutputSize();
            result["developmental_stage"] = static_cast<int>(agentBrain.getDevelopmentalStage());
            result["neuromodulation_level"] = agentBrain.getNeuromodulationLevel();
            result["curiosity_level"] = agentBrain.getCuriosityLevel();
            result["novelty_level"] = agentBrain.getNoveltyLevel();
            result["prediction_error"] = agentBrain.getPredictionError();
            return result;
        })
        .def("__repr__", [](const AgentBrain& agentBrain) {
            return "<AgentBrain: stage=" + std::to_string(static_cast<int>(agentBrain.getDevelopmentalStage())) + ">";
        })
        .def("__str__", [](const AgentBrain& agentBrain) {
            return "AgentBrain(stage=" + std::to_string(static_cast<int>(agentBrain.getDevelopmentalStage())) + ")";
        });

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        validateParameter(config, "config", "Config cannot be null");
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        validateParameter(brain, "brain", "Brain cannot be null");
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("createSession", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                           std::shared_ptr<AgentBrain> agentBrain) -> std::shared_ptr<SimulationSession> {
        return std::make_shared<SimulationSession>(brain, world, agentBrain);
    }, py::arg("brain"), py::arg("world"), py::arg("agentBrain") = nullptr,
        "Create a new simulation session");

    m.def("simulate_episode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                               std::shared_ptr<AgentBrain> agentBrain, size_t steps, double timestep = 1.0) {
        validateParameter(brain, "brain", "Brain cannot be null");
        validateParameter(world, "world", "World cannot be null");
        validateParameter(agentBrain, "agentBrain", "AgentBrain cannot be null");
        
        // Run episode
        for (size_t i = 0; i < steps; ++i) {
            // Get sensory percept
            const auto& percept = world->getSensoryPercept();
            agentBrain->processSensoryInput(percept);
            
            // Produce action
            auto action = brain->produceAction();
            
            // Apply to world
            auto result = world->applyMotorCommand(action->getType(), world->getSimulationTime());
            
            // Apply reward modulation
            agentBrain->applyRewardModulation(result.reward, agentBrain->getPredictionError());
            
            // Step brain
            brain->step(i, world->getSimulationTime());
            
            // Update world
            world->update(timestep);
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agentBrain"), py::arg("steps"), py::arg("timestep") = 1.0,
        "Run a complete episode simulation");

    // Invalid ID sentinels
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Helper functions
    m.def("normalize", [](const std::vector<float>& data, float min = 0.0f, float max = 1.0f) {
        std::vector<float> result = data;
        if (max > min) {
            std::transform(result.begin(), result.end(), result.begin(),
                         [min, max](float val) { return (val - min) / (max - min); });
        }
        return result;
    }, py::arg("data"), py::arg("min") = 0.0f, py::arg("max") = 1.0f,
        "Normalize data to range [min, max]");

    m.def("filter", [](const std::vector<float>& data, float threshold) {
        std::vector<float> result;
        for (float val : data) {
            if (std::abs(val) > threshold) {
                result.push_back(val);
            }
        }
        return result;
    }, py::arg("data"), py::arg("threshold") = 0.0f,
        "Filter data above absolute threshold");

    m.def("spike_rate_analysis", [](const std::vector<Timestamp>& spikeTimes, double duration) {
        if (spikeTimes.empty() || duration <= 0.0) {
            return 0.0;
        }
        return spikeTimes.size() / duration;
    }, py::arg("spike_times"), py::arg("duration"),
        "Calculate average spike rate from spike times");
}

} // namespace nlm
