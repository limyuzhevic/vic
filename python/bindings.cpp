#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

namespace py = pybind11;

// Forward declarations
namespace nlm {
    class NeuronId;
    class SynapseId;
    class RegionId;
    class PopulationId;
    class Config;
    class SensoryInput;
    class Vision;
    class Audio;
    class InternalSignals;
    class Action;
    class WorldObject;
    class AgentBody;
    class ActionResult;
    class SensoryPercept;
    class SimpleWorld;
    class Brain;
    class AgentBrain;
    class NeuronType;
    class SynapseType;
    class DevelopmentalStage;
    class FiringState;
    class ActionType;
    class MotorCommand;
    class WorldObjectType;
    class Neuromodulator;
    class Dopamine;
    class Curiosity;
    class Novelty;
    class PredictionError;
}

// Exception classes for Python
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Config error: " + msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError("Brain error: " + msg) {}
};

class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& msg) : NLMError("World error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const ConfigError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const BrainError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const WorldError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const SimulationError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Type definitions
    py::class_<nlm::NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::NeuronId::value)
        .def("index", &nlm::NeuronId::index)
        .def("__eq__", &nlm::NeuronId::operator==)
        .def("__ne__", &nlm::NeuronId::operator!=)
        .def("__hash__", [](const nlm::NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const nlm::NeuronId& id) {
            return std::to_string(id.value);
        });

    py::class_<nlm::SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::SynapseId::value)
        .def("index", &nlm::SynapseId::index)
        .def("__eq__", &nlm::SynapseId::operator==)
        .def("__ne__", &nlm::SynapseId::operator!=)
        .def("__hash__", [](const nlm::SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const nlm::SynapseId& id) {
            return std::to_string(id.value);
        });

    py::class_<nlm::RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::RegionId::value)
        .def("index", &nlm::RegionId::index)
        .def("__eq__", &nlm::RegionId::operator==)
        .def("__ne__", &nlm::RegionId::operator!=)
        .def("__hash__", [](const nlm::RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const nlm::RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const nlm::RegionId& id) {
            return std::to_string(id.value);
        });

    py::class_<nlm::PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::PopulationId::value)
        .def("index", &nlm::PopulationId::index)
        .def("__eq__", &nlm::PopulationId::operator==)
        .def("__ne__", &nlm::PopulationId::operator!=);

    // Enums
    py::enum_<nlm::NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Internal", nlm::NeuronType::Internal)
        .export_values();

    py::enum_<nlm::SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", nlm::SynapseType::Excitatory)
        .value("Inhibitory", nlm::SynapseType::Inhibitory)
        .value("Modulatory", nlm::SynapseType::Modulatory)
        .value("Electrical", nlm::SynapseType::Electrical)
        .value("GapJunction", nlm::SynapseType::GapJunction)
        .export_values();

    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging)
        .export_values();

    py::enum_<nlm::FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", nlm::FiringState::Resting)
        .value("Active", nlm::FiringState::Active)
        .value("Refractory", nlm::FiringState::Refractory)
        .value("Inhibited", nlm::FiringState::Inhibited)
        .export_values();

    py::enum_<nlm::ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", nlm::ActionType::MoveForward)
        .value("MoveBackward", nlm::ActionType::MoveBackward)
        .value("MoveLeft", nlm::ActionType::MoveLeft)
        .value("MoveRight", nlm::ActionType::MoveRight)
        .value("TurnLeft", nlm::ActionType::TurnLeft)
        .value("TurnRight", nlm::ActionType::TurnRight)
        .value("Look", nlm::ActionType::Look)
        .value("LookUp", nlm::ActionType::LookUp)
        .value("LookDown", nlm::ActionType::LookDown)
        .value("Interact", nlm::ActionType::Interact)
        .value("Eat", nlm::ActionType::Eat)
        .value("Drink", nlm::ActionType::Drink)
        .value("Rest", nlm::ActionType::Rest)
        .value("Wait", nlm::ActionType::Wait)
        .value("Custom", nlm::ActionType::Custom)
        .export_values();

    py::enum_<nlm::MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", nlm::MotorCommand::MoveForward)
        .value("MoveBackward", nlm::MotorCommand::MoveBackward)
        .value("TurnLeft", nlm::MotorCommand::TurnLeft)
        .value("TurnRight", nlm::MotorCommand::TurnRight)
        .value("LookLeft", nlm::MotorCommand::LookLeft)
        .value("LookRight", nlm::MotorCommand::LookRight)
        .value("Interact", nlm::MotorCommand::Interact)
        .value("Wait", nlm::MotorCommand::Wait)
        .export_values();

    py::enum_<nlm::WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", nlm::WorldObjectType::Empty)
        .value("Resource", nlm::WorldObjectType::Resource)
        .value("Hazard", nlm::WorldObjectType::Hazard)
        .value("Wall", nlm::WorldObjectType::Wall)
        .value("Marker", nlm::WorldObjectType::Marker)
        .export_values();

    // Convenience factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<nlm::Config> {
        return std::make_shared<nlm::Config>();
    }, R"pbdoc(Create a default configuration with sensible values)pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<nlm::SimpleWorld> {
        return std::make_shared<nlm::SimpleWorld>();
    }, R"pbdoc(Create a simple 2D world for NLM simulation)pbdoc");

    m.def("createBrain", [](std::shared_ptr<nlm::Config> config) -> std::shared_ptr<nlm::Brain> {
        return std::make_shared<nlm::Brain>(config);
    }, py::arg("config"), R"pbdoc(Create a new brain with configuration)pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<nlm::Brain> brain) -> std::shared_ptr<nlm::AgentBrain> {
        return std::make_shared<nlm::AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create a new agent brain interface)pbdoc");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(nlm::INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(nlm::INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(nlm::INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(nlm::INVALID_POPULATION_ID);

    // Config class
    py::class_<nlm::Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &nlm::Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](nlm::Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &nlm::Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &nlm::Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &nlm::Config::getKeys,
             "Get all configuration keys")
        .def("clear", &nlm::Config::clear,
             "Clear all configuration entries")
        .def("summary", &nlm::Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const nlm::Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Convenience methods
        .def("setInt", [](nlm::Config& self, const std::string& key, int64_t value) {
            self.set(key, value, nlm::ConfigSource::Explicit);
        }, py::arg("key"), py::arg("value"),
           "Set an integer configuration value")
        .def("setFloat", [](nlm::Config& self, const std::string& key, float value) {
            self.set(key, value, nlm::ConfigSource::Explicit);
        }, py::arg("key"), py::arg("value"),
           "Set a float configuration value")
        .def("setBool", [](nlm::Config& self, const std::string& key, bool value) {
            self.set(key, value, nlm::ConfigSource::Explicit);
        }, py::arg("key"), py::arg("value"),
           "Set a boolean configuration value")
        .def("getInt", [](const nlm::Config& self, const std::string& key, int64_t defaultValue) {
            return self.get<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get an integer configuration value")
        .def("getFloat", [](const nlm::Config& self, const std::string& key, float defaultValue) {
            return self.get<float>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get a float configuration value")
        .def("getBool", [](const nlm::Config& self, const std::string& key, bool defaultValue) {
            return self.get<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get a boolean configuration value");

    // Sensory Input classes
    py::class_<nlm::SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &nlm::SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &nlm::SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &nlm::SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &nlm::SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &nlm::SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const nlm::SensoryInput& input) {
            std::stringstream ss;
            ss << "<SensoryInput type=" << static_cast<int>(input.getType())
               << " dim=" << input.getDimensions() << ">";
            return ss.str();
        });

    py::class_<nlm::Vision, nlm::SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](nlm::Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &nlm::Vision::getWidth)
        .def("getHeight", &nlm::Vision::getHeight)
        .def("getChannels", &nlm::Vision::getChannels)
        .def("__repr__", [](const nlm::Vision& vision) {
            std::stringstream ss;
            ss << "<Vision width=" << vision.getWidth()
               << " height=" << vision.getHeight()
               << " channels=" << vision.getChannels() << ">";
            return ss.str();
        });

    py::class_<nlm::Audio, nlm::SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](nlm::Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &nlm::Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &nlm::Audio::getSampleRate)
        .def("getNumSamples", &nlm::Audio::getNumSamples)
        .def("__repr__", [](const nlm::Audio& audio) {
            std::stringstream ss;
            ss << "<Audio sampleRate=" << audio.getSampleRate()
               << " numSamples=" << audio.getNumSamples() << ">";
            return ss.str();
        });

    py::class_<nlm::InternalSignals, nlm::SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &nlm::InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &nlm::InternalSignals::clearSignals)
        .def("getSignalCount", &nlm::InternalSignals::getSignalCount)
        .def("__repr__", [](const nlm::InternalSignals& signals) {
            return "<InternalSignals count=" + std::to_string(signals.getSignalCount()) + ">";
        });

    // Action class
    py::class_<nlm::Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<nlm::ActionType>(), py::arg("type"))
        .def(py::init<nlm::ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &nlm::Action::getType)
        .def("setType", &nlm::Action::setType, py::arg("type"))
        .def("getParameters", &nlm::Action::getParameters)
        .def("setParameters", &nlm::Action::setParameters, py::arg("params"))
        .def("getName", &nlm::Action::getName)
        .def("clone", &nlm::Action::clone)
        .def("__repr__", [](const nlm::Action& action) {
            return "<Action type=" + action.getName() + " params=" + std::to_string(action.getParameters().size()) + ">";
        });

    // WorldObject class
    py::class_<nlm::WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, nlm::WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &nlm::WorldObject::x)
        .def_readwrite("y", &nlm::WorldObject::y)
        .def_readwrite("radius", &nlm::WorldObject::radius)
        .def_readwrite("type", &nlm::WorldObject::type)
        .def_readwrite("value", &nlm::WorldObject::value)
        .def_readwrite("active", &nlm::WorldObject::active)
        .def("distanceTo", [](const nlm::WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position (x, y)")
        .def("__repr__", [](const nlm::WorldObject& obj) {
            std::stringstream ss;
            ss << "<WorldObject type=" << static_cast<int>(obj.type)
               << " x=" << obj.x << " y=" << obj.y
               << " radius=" << obj.radius << ">";
            return ss.str();
        });

    // AgentBody class
    py::class_<nlm::AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &nlm::AgentBody::x)
        .def_readwrite("y", &nlm::AgentBody::y)
        .def_readwrite("orientation", &nlm::AgentBody::orientation)
        .def_readwrite("velocityX", &nlm::AgentBody::velocityX)
        .def_readwrite("velocityY", &nlm::AgentBody::velocityY)
        .def_readwrite("angularVelocity", &nlm::AgentBody::angularVelocity)
        .def_readwrite("energy", &nlm::AgentBody::energy)
        .def_readwrite("health", &nlm::AgentBody::health)
        .def_readwrite("age", &nlm::AgentBody::age)
        .def_readwrite("isMoving", &nlm::AgentBody::isMoving)
        .def_readwrite("isTurning", &nlm::AgentBody::isTurning)
        .def_readwrite("lastActionTime", &nlm::AgentBody::lastActionTime)
        .def("reset", &nlm::AgentBody::reset)
        .def("getPosition", [](const nlm::AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get agent position as (x, y) tuple")
        .def("getVelocity", [](const nlm::AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get agent velocity as (vx, vy) tuple")
        .def("moveTo", [](nlm::AgentBody& self, float x, float y, float dt) {
            float dx = x - self.x;
            float dy = y - self.y;
            float dist = std::sqrt(dx*dx + dy*dy);
            if (dist > 0.0f) {
                self.velocityX = dx / dist * 1.0f; // Simple movement
                self.velocityY = dy / dist * 1.0f;
                self.x = x;
                self.y = y;
            }
        }, py::arg("x"), py::arg("y"), py::arg("dt") = 0.1f,
           "Move agent to position (x, y) over time dt")
        .def("__repr__", [](const nlm::AgentBody& body) {
            std::stringstream ss;
            ss << "<AgentBody x=" << body.x << " y=" << body.y
               << " energy=" << body.energy << " health=" << body.health << ">";
            return ss.str();
        });

    // ActionResult class
    py::class_<nlm::ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &nlm::ActionResult::reward)
        .def_readwrite("success", &nlm::ActionResult::success)
        .def_readwrite("message", &nlm::ActionResult::message)
        .def("__repr__", [](const nlm::ActionResult& result) {
            std::stringstream ss;
            ss << "<ActionResult reward=" << result.reward
               << " success=" << (result.success ? "True" : "False")
               << " message=\"" << result.message << \">\"";
            return ss.str();
        });

    // SensoryPercept class
    py::class_<nlm::SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &nlm::SensoryPercept::getVision)
        .def("setVision", &nlm::SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &nlm::SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &nlm::SensoryPercept::getVisionHeight)
        .def("getTouch", &nlm::SensoryPercept::getTouch)
        .def("setTouch", &nlm::SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &nlm::SensoryPercept::getInternal)
        .def("setInternal", &nlm::SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &nlm::SensoryPercept::getProprioception)
        .def("setProprioception", &nlm::SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &nlm::SensoryPercept::getAudio)
        .def("setAudio", &nlm::SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &nlm::SensoryPercept::getAllSignals)
        .def("getTimestamp", &nlm::SensoryPercept::getTimestamp)
        .def("setTimestamp", &nlm::SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("__repr__", [](const nlm::SensoryPercept& percept) {
            std::stringstream ss;
            ss << "<SensoryPercept vision=" << percept.getVisionWidth() << "x" << percept.getVisionHeight()
               << " touch=" << percept.getTouch().size()
               << " internal=" << percept.getInternal().size()
               << " proprioception=" << percept.getProprioception().size()
               << " audio=" << percept.getAudio().size() << ">";
            return ss.str();
        });

    // SimpleWorld class
    py::class_<nlm::SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &nlm::SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &nlm::SimpleWorld::reset)
        .def("setAgentStart", &nlm::SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &nlm::SimpleWorld::update, py::arg("timestep"),
             "Update world simulation by timestep")
        .def("applyMotorCommand", &nlm::SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command and return action result")
        .def("getSensoryPercept", &nlm::SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept (vision, touch, etc.)")
        .def("getAgentBody", &nlm::SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &nlm::SimpleWorld::addObject, py::arg("obj"),
             "Add a world object")
        .def("removeObject", &nlm::SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position (x, y)")
        .def("isValidPosition", &nlm::SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position (x, y) is valid (not in wall)")
        .def("getWidth", &nlm::SimpleWorld::getWidth)
        .def("getHeight", &nlm::SimpleWorld::getHeight)
        .def("getMaxEnergy", &nlm::SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &nlm::SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &nlm::SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &nlm::SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate per timestep")
        .def("getSimulationTime", &nlm::SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &nlm::SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &nlm::SimpleWorld::getRandomSeed)
        .def("getResourceCount", &nlm::SimpleWorld::getResourceCount,
             "Get number of resources in world")
        .def("getHazardCount", &nlm::SimpleWorld::getHazardCount,
             "Get number of hazards in world")
        .def("__repr__", [](const nlm::SimpleWorld& world) {
            std::stringstream ss;
            ss << "<SimpleWorld width=" << world.getWidth()
               << " height=" << world.getHeight()
               << " time=" << world.getSimulationTime() << ">";
            return ss.str();
        });

    // Brain class
    py::class_<nlm::Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<nlm::Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &nlm::Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (nlm::Brain::*)(nlm::SimulationStep)>(&nlm::Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (nlm::Brain::*)(nlm::SimulationStep, nlm::Timestamp)>(&nlm::Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &nlm::Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &nlm::Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &nlm::Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &nlm::Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &nlm::Brain::reset,
             "Reset brain state")
        .def("save", &nlm::Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &nlm::Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &nlm::Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &nlm::Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &nlm::Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &nlm::Brain::getRegionIds,
             "Get all region IDs")
        .def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &nlm::Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &nlm::Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &nlm::Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &nlm::Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getPendingSpikeEventCount", &nlm::Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("getDevelopmentalStage", &nlm::Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &nlm::Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &nlm::Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &nlm::Brain::logStatus,
             "Log brain status")
        // Additional convenience methods
        .def("isInitialized", &nlm::Brain::getRegionCount,
             "Check if brain is initialized (has regions)")
        .def("hasMemorySystems", &nlm::Brain::getWorkingMemory,
             "Check if brain has memory systems enabled")
        .def("hasPredictionSystem", &nlm::Brain::getPredictionSystem,
             "Check if brain has prediction system enabled")
        .def("hasCognitionSystems", []() { return true; },  // Always true in current implementation
             "Check if brain has cognition systems enabled")
        .def("hasNeuromodulationSystems", []() { return true; },
             "Check if brain has neuromodulation systems enabled")
        .def("hasDevelopmentSystem", &nlm::Brain::getDevelopmentSystem,
             "Check if brain has development system enabled")
        .def("__repr__", [](const nlm::Brain& brain) {
            return "<Brain neurons=" + std::to_string(brain.getTotalNeuronCount()) +
                   " regions=" + std::to_string(brain.getRegionCount()) + ">";
        });

    // AgentBrain class
    py::class_<nlm::AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<nlm::Brain>>(), py::arg("brain"),
             "Initialize agent brain with brain instance")
        .def("initialize", &nlm::AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &nlm::AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &nlm::AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &nlm::AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &nlm::AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &nlm::AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        // Enable/Disable features
        .def("enableRewardModulation", &nlm::AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &nlm::AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &nlm::AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &nlm::AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        // Check enable/disable status
        .def("isRewardModulationEnabled", &nlm::AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &nlm::AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &nlm::AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &nlm::AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("__repr__", [](const nlm::AgentBrain& agentBrain) {
            return "<AgentBrain>";
        });

    // Default configuration helper
    m.def("getDefaultConfig", []() {
        auto config = std::make_shared<nlm::Config>();
        
        // Set some reasonable defaults
        config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
        config->set("max_neurons_per_region", static_cast<int64_t>(300), nlm::ConfigSource::Default);
        config->set("learning_rate", 0.01f, nlm::ConfigSource::Default);
        config->set("spike_threshold", 15.0f, nlm::ConfigSource::Default);
        config->set("refractory_period", 5, nlm::ConfigSource::Default);
        config->set("initial_excitatory_ratio", 0.8f, nlm::ConfigSource::Default);
        config->set("initial_inhibitory_ratio", 0.2f, nlm::ConfigSource::Default);
        
        return config;
    }, R"pbdoc(Create a default configuration with sensible values for general use)pbdoc");

    // Simulation utilities
    m.def("run_brain_simulation", [](std::shared_ptr<nlm::Brain> brain, 
                                   std::shared_ptr<nlm::SimpleWorld> world,
                                   std::shared_ptr<nlm::AgentBrain> agent,
                                   int num_steps, bool verbose = false) {
        if (!brain || !world || !agent) {
            throw SimulationError("All brain, world, and agent must be provided");
        }
        
        agent->initialize(*world);
        
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1f);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": "
                         << "Firing neurons: " << brain->getFiringNeuronCount()
                         << " Total spikes: " << brain->getTotalSpikeCount()
                         << std::endl;
            }
        }
        
        if (verbose) {
            std::cout << "Simulation completed after " << num_steps << " steps" << std::endl;
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
       py::arg("verbose") = false,
       R"pbdoc(Run a complete brain simulation with world interaction)pbdoc");

    m.def("create_simple_agent", [](std::shared_ptr<nlm::Brain> brain,
                                   std::shared_ptr<nlm::SimpleWorld> world) {
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        agent->initialize(*world);
        return agent;
    }, py::arg("brain"), py::arg("world"),
       R"pbdoc(Create and initialize an agent brain with world)pbdoc");

    m.def("create_development_agent", [](std::shared_ptr<nlm::Brain> brain,
                                        std::shared_ptr<nlm::SimpleWorld> world) {
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        agent->initialize(*world);
        // Enable all major learning systems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(true);
        return agent;
    }, py::arg("brain"), py::arg("world"),
       R"pbdoc(Create a development agent with all learning systems enabled)pbdoc");

    // Debug and utility functions
    m.def("brain_summary", [](const nlm::Brain& brain) {
        std::stringstream ss;
        ss << "=== BRAIN SUMMARY ===\n";
        ss << "Neurons: " << brain.getTotalNeuronCount() << "\n";
        ss << "Synapses: " << brain.getTotalSynapseCount() << "\n";
        ss << "Firing: " << brain.getFiringNeuronCount() << "\n";
        ss << "Active: " << brain.getActiveNeuronCount() << "\n";
        ss << "Total Spikes: " << brain.getTotalSpikeCount() << "\n";
        ss << "E/I Ratio: " << brain.getExcitationInhibitionRatio() << "\n";
        ss << "Average Firing Rate: " << brain.getAverageFiringRate() << "\n";
        ss << "Regions: " << brain.getRegionCount() << "\n";
        ss << "Development Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n";
        return ss.str();
    }, R"pbdoc(Get a detailed summary of brain state)pbdoc");

    m.def("world_summary", [](const nlm::SimpleWorld& world) {
        std::stringstream ss;
        ss << "=== WORLD SUMMARY ===\n";
        ss << "Dimensions: " << world.getWidth() << "x" << world.getHeight() << "\n";
        ss << "Time: " << world.getSimulationTime() << "\n";
        ss << "Max Energy: " << world.getMaxEnergy() << "\n";
        ss << "Energy Decay: " << world.getEnergyDecayRate() << "\n";
        ss << "Random Seed: " << world.getRandomSeed() << "\n";
        return ss.str();
    }, R"pbdoc(Get a summary of world state)pbdoc");

    m.def("validate_brain", [](const nlm::Brain& brain) {
        std::vector<std::string> errors;
        
        if (brain.getTotalNeuronCount() == 0) {
            errors.push_back("Brain has no neurons");
        }
        
        if (brain.getRegionCount() == 0) {
            errors.push_back("Brain has no regions");
        }
        
        if (brain.getTotalSynapseCount() == 0) {
            errors.push_back("Brain has no synapses");
        }
        
        if (!brain.getConfig() || !brain.getConfig()->has("neuron_count")) {
            errors.push_back("Brain missing neuron_count config");
        }
        
        return errors;
    }, R"pbdoc(Validate brain configuration and return list of errors if any)pbdoc");

    m.def("validate_world", [](const nlm::SimpleWorld& world) {
        std::vector<std::string> errors;
        
        if (world.getWidth() <= 0 || world.getHeight() <= 0) {
            errors.push_back("Invalid world dimensions");
        }
        
        if (world.getMaxEnergy() <= 0.0f) {
            errors.push_back("Invalid max energy");
        }
        
        if (world.getEnergyDecayRate() < 0.0f || world.getEnergyDecayRate() > 1.0f) {
            errors.push_back("Invalid energy decay rate");
        }
        
        return errors;
    }, R"pbdoc(Validate world configuration and return list of errors if any)pbdoc");

    m.def("benchmark_brain_creation", []() {
        auto start = std::chrono::high_resolution_clock::now();
        
        auto config = nlm::createDefaultConfig();
        auto brain = nlm::createBrain(config);
        brain->initialize();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        return duration.count();
    }, R"pbdoc(Benchmark brain creation time in milliseconds)pbdoc");

    m.def("benchmark_step", [](std::shared_ptr<nlm::Brain> brain, int steps = 100) {
        if (!brain) {
            throw BrainError("Brain must be provided for benchmark");
        }
        
        brain->initialize();
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        double avg_time_per_step = static_cast<double>(duration.count()) / steps;
        
        return std::make_pair(duration.count(), avg_time_per_step);
    }, py::arg("brain"), py::arg("steps") = 100,
       R"pbdoc(Benchmark brain step performance, returns (total_time_ms, avg_time_per_step_ms))pbdoc");

    // Main convenience examples
    m.def("create_learning_agent_example", [](std::shared_ptr<nlm::SimpleWorld> world) {
        // Create brain with learning-focused configuration
        auto config = nlm::getDefaultConfig();
        config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
        config->set("learning_rate", 0.005f, nlm::ConfigSource::Default);
        config->set("curiosity_strength", 0.3f, nlm::ConfigSource::Default);
        
        auto brain = nlm::createBrain(config);
        brain->initialize();
        
        auto agent = nlm::createAgentBrain(brain);
        agent->initialize(*world);
        
        // Enable learning systems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        return std::make_tuple(brain, agent);
    }, py::arg("world") = nlm::createSimpleWorld(),
       R"pbdoc(Create a learning-focused agent brain with curiosity and reward modulation)pbdoc");

    m.def("create_exploration_agent_example", [](std::shared_ptr<nlm::SimpleWorld> world) {
        // Create brain optimized for exploration
        auto config = nlm::getDefaultConfig();
        config->set("neuron_count", static_cast<int64_t>(800), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.2f, nlm::ConfigSource::Default);
        config->set("novelty_threshold", 0.5f, nlm::ConfigSource::Default);
        
        auto brain = nlm::createBrain(config);
        brain->initialize();
        
        auto agent = nlm::createAgentBrain(brain);
        agent->initialize(*world);
        
        // Enable exploration systems
        agent->enableCuriosity(true);
        agent->enableNovelty(true);
        agent->enableRewardModulation(true);
        
        return std::make_tuple(brain, agent);
    }, py::arg("world") = nlm::createSimpleWorld(),
       R"pbdoc(Create an exploration-focused agent brain with novelty detection)pbdoc");

    m.def("create_development_agent_example", [](std::shared_ptr<nlm::SimpleWorld> world) {
        // Create a full development agent with all systems
        auto config = nlm::getDefaultConfig();
        config->set("neuron_count", static_cast<int64_t>(1200), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(6), nlm::ConfigSource::Default);
        
        auto brain = nlm::createBrain(config);
        brain->initialize();
        
        auto agent = nlm::createAgentBrain(brain);
        agent->initialize(*world);
        
        // Enable all development and learning systems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableNovelty(true);
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(true);
        
        return std::make_tuple(brain, agent);
    }, py::arg("world") = nlm::createSimpleWorld(),
       R"pbdoc(Create a full development agent with all cognitive and learning systems)pbdoc");

    // Configuration presets
    m.def("get_exploratory_config", []() {
        auto config = nlm::createDefaultConfig();
        config->set("connection_probability", 0.25f, nlm::ConfigSource::Default);
        config->set("curiosity_strength", 0.5f, nlm::ConfigSource::Default);
        config->set("novelty_threshold", 0.3f, nlm::ConfigSource::Default);
        config->set("epsilon_greedy", 0.2f, nlm::ConfigSource::Default);
        return config;
    }, R"pbdoc(Get an exploratory configuration optimized for discovery)pbdoc");

    m.def("get_conservative_config", []() {
        auto config = nlm::createDefaultConfig();
        config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
        config->set("learning_rate", 0.001f, nlm::ConfigSource::Default);
        config->set("curiosity_strength", 0.1f, nlm::ConfigSource::Default);
        return config;
    }, R"pbdoc(Get a conservative configuration optimized for stability)pbdoc");

    m.def("get_balanced_config", []() {
        auto config = nlm::createDefaultConfig();
        config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
        config->set("learning_rate", 0.01f, nlm::ConfigSource::Default);
        config->set("curiosity_strength", 0.3f, nlm::ConfigSource::Default);
        config->set("novelty_threshold", 0.5f, nlm::ConfigSource::Default);
        return config;
    }, R"pbdoc(Get a balanced configuration for general purpose use)pbdoc");

    // Command line interface helpers
    m.def("create_cli_config", [](int argc, char** argv) {
        auto config = nlm::createDefaultConfig();
        // Basic CLI argument parsing could be added here
        return config;
    }, py::arg("argc"), py::arg("argv"),
       R"pbdoc(Create configuration from command line arguments)pbdoc");

    // Monitoring and statistics
    m.def("get_system_stats", [](const nlm::Brain& brain) {
        std::map<std::string, double> stats;
        stats["neuron_count"] = static_cast<double>(brain.getTotalNeuronCount());
        stats["synapse_count"] = static_cast<double>(brain.getTotalSynapseCount());
        stats["firing_rate"] = brain.getAverageFiringRate();
        stats["excitement_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
        stats["total_spikes"] = static_cast<double>(brain.getTotalSpikeCount());
        stats["active_neuron_ratio"] = brain.getTotalNeuronCount() > 0 ? 
            static_cast<double>(brain.getActiveNeuronCount()) / brain.getTotalNeuronCount() : 0.0;
        return stats;
    }, R"pbdoc(Get system statistics as a dictionary)pbdoc");

    // Version and information
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__description__") = "Neural Learning Machine Python Bindings";
    
    m.def("get_version_info", []() {
        return std::make_tuple(std::string("0.1.0"), std::string("NLM Research Team"), std::string("Neural Learning Machine Python Bindings"));
    }, R"pbdoc(Get version information)pbdoc");

    // Quick help and examples
    m.def("examples", []() {
        return R"pbdoc(
# Quick Start Examples:

# 1. Simple Brain
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)

# 2. Agent in World
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# 3. Learning Agent
brain = pynlm.createBrain(pynlm.getDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Configuration Presets:
brain = pynlm.createBrain(pynlm.get_exploratory_config())
brain = pynlm.createBrain(pynlm.get_conservative_config())
brain = pynlm.createBrain(pynlm.get_balanced_config())

# Simulation Utilities:
brain, agent = pynlm.create_learning_agent_example()
brain, agent = pynlm.create_exploration_agent_example()
brain, agent = pynlm.create_development_agent_example()

# Monitoring:
stats = pynlm.get_system_stats(brain)
summary = pynlm.brain_summary(brain)
)pbdoc";
    }, R"pbdoc(Return quick start examples and usage guide)pbdoc");

    // Quality of life improvements
    py::class_<nlm::ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .def("__str__", [](nlm::ActionType type) {
            switch (type) {
                case nlm::ActionType::MoveForward: return std::string("MoveForward");
                case nlm::ActionType::MoveBackward: return std::string("MoveBackward");
                case nlm::ActionType::MoveLeft: return std::string("MoveLeft");
                case nlm::ActionType::MoveRight: return std::string("MoveRight");
                case nlm::ActionType::TurnLeft: return std::string("TurnLeft");
                case nlm::ActionType::TurnRight: return std::string("TurnRight");
                case nlm::ActionType::Look: return std::string("Look");
                case nlm::ActionType::LookUp: return std::string("LookUp");
                case nlm::ActionType::LookDown: return std::string("LookDown");
                case nlm::ActionType::Interact: return std::string("Interact");
                case nlm::ActionType::Eat: return std::string("Eat");
                case nlm::ActionType::Drink: return std::string("Drink");
                case nlm::ActionType::Rest: return std::string("Rest");
                case nlm::ActionType::Wait: return std::string("Wait");
                case nlm::ActionType::Custom: return std::string("Custom");
                default: return std::string("Unknown");
            }
        });
}
