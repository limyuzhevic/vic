#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <cmath>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"
#include "../src/experiments/Phase6Config.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations for helper functions
class Brain;

// Enhanced exception types
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message) : std::runtime_error(message) {}
};

class ValidationError : public NLMException {
public:
    ValidationError(const std::string& message) : NLMException(message) {}
};

class ConfigurationError : public NLMException {
public:
    ConfigurationError(const std::string& message) : NLMException(message) {}
};

class BrainInitializationError : public NLMException {
public:
    BrainInitializationError(const std::string& message) : NLMException(message) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Edition
        -------------------------------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        Enhanced with advanced features, better error handling, and extensive utilities.
    )pbdoc";

    // Register enhanced exception types
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ValidationError>(m, "ValidationError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<BrainInitializationError>(m, "BrainInitializationError");

    // Register basic types with enhanced documentation
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

    // Enhanced enum definitions with more details
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Fast-spiking excitatory neuron")
        .value("Inhibitory", NeuronType::Inhibitory, "Fast-spiking inhibitory neuron")
        .value("Modulatory", NeuronType::Modulatory, " neuromodulatory neuron")
        .value("Sensory", NeuronType::Sensory, "Sensory input neuron")
        .value("Motor", NeuronType::Motor, "Motor output neuron")
        .value("Internal", NeuronType::Internal, "Internal computational neuron")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Standard excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Standard inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Neuromodulatory synapse")
        .value("Electrical", SynapseType::Electrical, "Electrical synaptic connection")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction coupling")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage (high plasticity)")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period with specialized learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase")
        .value("Adult", DevelopmentalStage::Adult, "Adult stable phase")
        .value("Aging", DevelopmentalStage::Aging, "Aging phase (declining plasticity)")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Not firing, resting state")
        .value("Active", FiringState::Active, "Firing or preparing to fire")
        .value("Refractory", FiringState::Refractory, "After firing, cannot fire again")
        .value("Inhibited", FiringState::Inhibited, "Temporarily inhibited")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward")
        .value("MoveLeft", ActionType::MoveLeft, "Move left")
        .value("MoveRight", ActionType::MoveRight, "Move right")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left")
        .value("TurnRight", ActionType::TurnRight, "Turn right")
        .value("Look", ActionType::Look, "Look forward")
        .value("LookUp", ActionType::LookUp, "Look up")
        .value("LookDown", ActionType::LookDown, "Look down")
        .value("Interact", ActionType::Interact, "Interact with object")
        .value("Eat", ActionType::Eat, "Eat food")
        .value("Drink", ActionType::Drink, "Drink water")
        .value("Rest", ActionType::Rest, "Rest/idle")
        .value("Wait", ActionType::Wait, "Wait for command")
        .value("Custom", ActionType::Custom, "Custom action with parameters")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact")
        .value("Wait", MotorCommand::Wait, "Wait")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Resource/energ	y")
        .value("Hazard", WorldObjectType::Hazard, "Hazardous object")
        .value("Wall", WorldObjectType::Wall, "Wall/boundary")
        .value("Marker", WorldObjectType::Marker, "Marker/landmark")
        .export_values();

    // Enhanced Config class with more functionality
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system with enhanced functionality)pbdoc")
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
        .def("get", py::overload_cast<const std::string&>(&Config::get),
             py::arg("key"), "Get a configuration value")
        .def("getOr", py::overload_cast<const std::string&, const std::string&>(&Config::getOr),
             py::arg("key"), py::arg("default"), "Get a configuration value or default")
        .def("set", &Config::set, py::arg("key"), py::arg("value"), py::arg("source"),
             "Set a configuration value with source tracking")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Default);
        }, py::arg("key"), py::arg("value"), "Set a string configuration value")
        .def("set", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, ConfigSource::Default);
        }, py::arg("key"), py::arg("value"), "Set an integer configuration value")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::Default);
        }, py::arg("key"), py::arg("value"), "Set a float configuration value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Default);
        }, py::arg("key"), py::arg("value"), "Set a boolean configuration value")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("getValues", &Config::getValues,
             "Get all configuration values")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("to_dict", &Config::to_dict,
             "Convert configuration to Python dictionary")
        .def("from_dict", [](Config& self, const py::dict& d) {
            self.from_dict(py::cast<std::map<std::string, std::string>>(d));
        }, py::arg("config_dict"), "Load configuration from Python dictionary")
        .def("validate", &Config::validate,
             "Validate configuration values")
        .def("get_source", &Config::get_source, py::arg("key"),
             "Get the source of a configuration value")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // Enhanced SensoryInput with better error handling
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input with enhanced functionality)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("validate", &SensoryInput::validate,
             "Validate sensory input data")
        .def("copy", &SensoryInput::copy,
             "Create a copy of the sensory input");

    // Enhanced Vision with more features
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input with enhanced features)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromArray", [](Vision& self, const py::array_t<float>& array) {
            py::buffer_info info = array.request();
            float* ptr = static_cast<float*>(info.ptr);
            size_t size = info.size;
            std::vector<float> data(ptr, ptr + size);
            self.setData(data);
        }, py::arg("data"), "Set data from NumPy array")
        .def("normalize", &Vision::normalize,
             "Normalize vision data to [0, 1]")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getDataPointer", [](Vision& self) {
            return py::memory_view<float>(self.getData());
        }, "Get data as Python memory view")
        .def("to_numpy", [](Vision& self) {
            return py::array_t<float>(self.getData());
        }, "Convert to NumPy array");

    // Enhanced Audio with more features
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input with enhanced features)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromArray", [](Audio& self, const py::array_t<float>& array) {
            py::buffer_info info = array.request();
            float* ptr = static_cast<float*>(info.ptr);
            size_t size = info.size;
            std::vector<float> data(ptr, ptr + size);
            self.setData(data);
        }, py::arg("data"), "Set data from NumPy array")
        .def("normalize", &Audio::normalize,
             "Normalize audio data to [-1, 1]")
        .def("applyFilter", &Audio::applyFilter, py::arg("cutoff"), py::arg("order"),
             "Apply low-pass filter to audio data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", &Audio::getDuration)
        .def("getDataPointer", [](Audio& self) {
            return py::memory_view<float>(self.getData());
        }, "Get data as Python memory view")
        .def("to_numpy", [](Audio& self) {
            return py::array_t<float>(self.getData());
        }, "Convert to NumPy array");

    // Enhanced InternalSignals
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input with enhanced features)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("addSignals", [](InternalSignals& self, const std::vector<float>& values) {
            for (float v : values) self.addSignal(v);
        }, py::arg("values"), "Add multiple signals")
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount)
        .def("getMaxSignal", &InternalSignals::getMaxSignal)
        .def("getMinSignal", &InternalSignals::getMinSignal)
        .def("getMeanSignal", &InternalSignals::getMeanSignal)
        .def("getVariance", &InternalSignals::getVariance)
        .def("normalize", &InternalSignals::normalize);

    // Enhanced Action with better parameter handling
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output with enhanced features)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("addParameter", &Action::addParameter, py::arg("value"))
        .def("getParameter", &Action::getParameter, py::arg("index"))
        .def("setParameter", &Action::setParameter, py::arg("index"), py::arg("value"))
        .def("getName", &Action::getName)
        .def("getDescription", &Action::getDescription)
        .def("clone", &Action::clone)
        .def("equals", &Action::equals, py::arg("other"),
             "Check if action equals another action")
        .def("to_dict", &Action::to_dict,
             "Convert action to Python dictionary")
        .def("from_dict", [](Action& self, const py::dict& d) {
            self.from_dict(py::cast<std::map<std::string, std::string>>(d));
        }, py::arg("action_dict"), "Load action from Python dictionary");

    // Enhanced WorldObject
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation with enhanced features)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def("getPosition", [](WorldObject& self) {
            return std::make_pair(self.x, self.y);
        })
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("distanceTo", &WorldObject::distanceTo, py::arg("other"),
             "Calculate distance to another object")
        .def("isWithinRange", &WorldObject::isWithinRange, py::arg("other"), py::arg("range"),
             "Check if object is within range of another object")
        .def("containsPoint", &WorldObject::containsPoint, py::arg("x"), py::arg("y"),
             "Check if point is within object's bounds")
        .def("intersectWith", &WorldObject::intersectWith, py::arg("other"),
             "Check if object intersects with another object")
        .def("mergeWith", &WorldObject::mergeWith, py::arg("other"),
             "Merge this object with another object")
        .def("split", &WorldObject::split, py::arg("newType"),
             "Split object into two objects")
        .def("isActive", []() { return WorldObject::isActive; }, "Check if object is active")
        .def("setActive", []() { return WorldObject::setActive; }, "Set object active state")
        .def("getArea", &WorldObject::getArea,
             "Get area of the object")
        .def("getVolume", &WorldObject::getVolume,
             "Get volume of the object")
        .def("to_dict", &WorldObject::to_dict,
             "Convert object to Python dictionary")
        .def("from_dict", [](WorldObject& self, const py::dict& d) {
            self.from_dict(py::cast<std::map<std::string, std::string>>(d));
        }, py::arg("object_dict"), "Load object from Python dictionary");

    // Enhanced AgentBody
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state with enhanced features)pbdoc")
        .def(py::init<>())
        .def("getPosition", [](AgentBody& self) {
            return std::make_pair(self.x, self.y);
        })
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("getOrientation", &AgentBody::getOrientation)
        .def("setOrientation", &AgentBody::setOrientation, py::arg("orientation"),
             "Set orientation")
        .def("getVelocity", [](AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        })
        .def("setVelocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
        }, py::arg("velocityX"), py::arg("velocityY"), "Set velocity")
        .def("getAngularVelocity", &AgentBody::getAngularVelocity)
        .def("setAngularVelocity", &AgentBody::setAngularVelocity, py::arg("angularVelocity"),
             "Set angular velocity")
        .def("getSpeed", &AgentBody::getSpeed)
        .def("getDirection", &AgentBody::getDirection)
        .def("move", [](AgentBody& self, float dx, float dy, float dt) {
            self.x += dx;
            self.y += dy;
            self.velocityX = dx / dt;
            self.velocityY = dy / dt;
        }, py::arg("dx"), py::arg("dy"), py::arg("dt"), "Move body")
        .def("rotate", &AgentBody::rotate, py::arg("angle"), py::arg("dt"),
             "Rotate body")
        .def("updatePhysics", &AgentBody::updatePhysics, py::arg("dt"), py::arg("gravity"),
             "Update physics simulation")
        .def("reset", &AgentBody::reset,
             "Reset body state")
        .def("isMoving", &AgentBody::isMoving)
        .def("setMoving", &AgentBody::setMoving, py::arg("moving"),
             "Set moving state")
        .def("to_dict", &AgentBody::to_dict,
             "Convert body to Python dictionary")
        .def("from_dict", [](AgentBody& self, const py::dict& d) {
            self.from_dict(py::cast<std::map<std::string, std::string>>(d));
        }, py::arg("body_dict"), "Load body from Python dictionary");

    // Enhanced ActionResult
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world with enhanced features)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def("getReward", &ActionResult::getReward)
        .def("setReward", &ActionResult::setReward, py::arg("reward"),
             "Set reward")
        .def("getSuccess", &ActionResult::getSuccess)
        .def("setSuccess", &ActionResult::setSuccess, py::arg("success"),
             "Set success status")
        .def("getMessage", &ActionResult::getMessage)
        .def("setMessage", &ActionResult::setMessage, py::arg("message"),
             "Set message")
        .def("isSuccessful", [](const ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, "Check if action was successful")
        .def("to_dict", &ActionResult::to_dict,
             "Convert result to Python dictionary")
        .def("from_dict", [](ActionResult& self, const py::dict& d) {
            self.from_dict(py::cast<std::map<std::string, std::string>>(d));
        }, py::arg("result_dict"), "Load result from Python dictionary");

    // Enhanced SensoryPercept
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data with enhanced features)pbdoc")
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
        .def("addSignal", [](SensoryPercept& self, const std::string& type, const std::vector<float>& data) {
            if (type == "vision") {
                auto vision = std::make_shared<Vision>();
                vision->setData(data);
                self.setVision(vision);
            } else if (type == "audio") {
                auto audio = std::make_shared<Audio>();
                audio->setData(data);
                self.setAudio(audio);
            } else {
                self.setInternal(data);
            }
        }, py::arg("type"), py::arg("data"), "Add signal data")
        .def("getSignal", [](SensoryPercept& self, const std::string& type) {
            if (type == "vision") return self.getVision();
            else if (type == "audio") return self.getAudio();
            else return self.getInternal();
        }, py::arg("type"), "Get signal data")
        .def("normalizeAll", &SensoryPercept::normalizeAll,
             "Normalize all sensory signals")
        .def("validateAll", &SensoryPercept::validateAll,
             "Validate all sensory signals")
        .def("to_dict", &SensoryPercept::to_dict,
             "Convert percept to Python dictionary")
        .def("from_dict", [](SensoryPercept& self, const py::dict& d) {
            self.from_dict(py::cast<std::map<std::string, std::string>>(d));
        }, py::arg("percept_dict"), "Load percept from Python dictionary");

    // Enhanced SimpleWorld with advanced features
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation with enhanced features)pbdoc")
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
        .def("removeObjectByRef", [](SimpleWorld& self, const WorldObject& obj) {
            self.removeObject(obj.x, obj.y);
        }, py::arg("obj"), "Remove object by reference")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal, "Get object at position")
        .def("getObjectsInRange", &SimpleWorld::getObjectsInRange, py::arg("x"), py::arg("y"), py::arg("range"),
             "Get objects within range")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getAgentDistance", &SimpleWorld::getAgentDistance, py::arg("x"), py::arg("y"),
             "Get distance to agent")
        .def("getAgentAngle", &SimpleWorld::getAgentAngle, py::arg("x"), py::arg("y"),
             "Get angle to agent")
        .def("isAgentInDanger", &SimpleWorld::isAgentInDanger, py::arg("dangerRadius"),
             "Check if agent is in danger")
        .def("getSafeZone", &SimpleWorld::getSafeZone, py::arg("minX"), py::arg("minY"), py::arg("maxX"), py::arg("maxY"),
             "Get safe zone rectangle")
        .def("to_dict", &SimpleWorld::to_dict,
             "Convert world to Python dictionary")
        .def("from_dict", [](SimpleWorld& self, const py::dict& d) {
            self.from_dict(py::cast<std::map<std::string, std::string>>(d));
        }, py::arg("world_dict"), "Load world from Python dictionary");

    // Enhanced Brain with advanced features
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with enhanced functionality)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("stepMultiple", [](Brain& self, size_t steps, double timestep) {
            for (size_t i = 0; i < steps; ++i) {
                self.step(i, i * timestep);
            }
        }, py::arg("steps"), py::arg("timestep"), "Perform multiple simulation steps")
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
        .def("findRegion", [](Brain& self, const std::string& name) {
            for (auto& region : self.getRegions()) {
                if (region->getName() == name) return region;
            }
            return std::shared_ptr<NeuralRegion>(nullptr);
        }, py::arg("name"), "Find region by name")
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
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random number generator")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("validate", [](Brain& self) {
            return self.getTotalNeuronCount() > 0 && self.getTotalSynapseCount() > 0;
        }, "Validate brain configuration")
        .def("getNeuronStats", [](Brain& self) {
            return std::make_tuple(self.getTotalNeuronCount(), self.getActiveNeuronCount(),
                                  self.getFiringNeuronCount());
        }, "Get neuron statistics (total, active, firing)")
        .def("getSynapseStats", [](Brain& self) {
            return std::make_tuple(self.getTotalSynapseCount(), self.getExcitationInhibitionRatio());
        }, "Get synapse statistics (total, E/I ratio)")
        .def("getPerformanceMetrics", [](Brain& self) {
            py::dict metrics;
            metrics["neurons"] = self.getTotalNeuronCount();
            metrics["synapses"] = self.getTotalSynapseCount();
            metrics["active_neurons"] = self.getActiveNeuronCount();
            metrics["firing_neurons"] = self.getFiringNeuronCount();
            metrics["avg_firing_rate"] = self.getAverageFiringRate();
            metrics["e_iratio"] = self.getExcitationInhibitionRatio();
            metrics["total_spikes"] = self.getTotalSpikeCount();
            return metrics;
        }, "Get performance metrics as Python dictionary");

    // Enhanced AgentBrain with advanced agent functionality
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world with enhanced functionality)pbdoc")
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
        .def("toggleAllFeatures", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all agent features")
        .def("getFeaturesStatus", [](AgentBrain& self) {
            py::dict features;
            features["reward_modulation"] = self.isRewardModulationEnabled();
            features["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            features["development"] = self.isDevelopmentEnabled();
            features["curiosity"] = self.isCuriosityEnabled();
            return features;
        }, "Get status of all features");

    // Enhanced convenience functions
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

    m.def("createPhase6Config", []() -> std::shared_ptr<Phase6Config> {
        return std::make_shared<Phase6Config>();
    }, "Create a Phase 6 configuration");

    m.def("runPhase6Demo", [](std::shared_ptr<Config> config, const std::string& mode = "full") {
        Phase6IntegratedExperiment experiment;
        Phase6Config phase6Config;
        if (config) {
            if (config->has("neuron_count")) phase6Config.neuronCount = config->get<uint64_t>("neuron_count");
            if (config->has("max_steps")) phase6Config.maxSteps = config->get<uint64_t>("max_steps");
            phase6Config.enableCheckpointing = config->getOr<bool>("enable_checkpointing", true);
            phase6Config.enableReplay = config->getOr<bool>("enable_replay", true);
            phase6Config.enableDevelopment = config->getOr<bool>("enable_development", true);
        }
        
        if (mode == "quick") {
            return experiment.verifyIntegration();
        } else if (mode == "full") {
            auto result = experiment.run(phase6Config);
            py::dict result_dict;
            result_dict["success"] = result.memoryWorkingMemoryIntegrated && 
                                 result.memoryEpisodicMemoryIntegrated &&
                                 result.neuromodulationIntegrated &&
                                 result.predictionIntegrated &&
                                 result.developmentIntegrated;
            result_dict["total_reward"] = result.totalReward;
            result_dict["avg_firing_rate"] = result.avgFiringRate;
            result_dict["memory_episodes"] = result.memoryEpisodesStored;
            return result_dict;
        } else {
            throw py::value_error("Unknown mode: " + mode);
        }
    }, py::arg("config") = nullptr, py::arg("mode") = "full", "Run Phase 6 demo");

    m.def("validateConfig", [](std::shared_ptr<Config> config) {
        if (!config) throw ConfigurationError("Config is null");
        if (!config->validate()) {
            throw ValidationError("Configuration validation failed");
        }
        return true;
    }, py::arg("config"), "Validate configuration");

    m.def("initializeBrain", [](std::shared_ptr<Brain> brain) {
        if (!brain) throw BrainInitializationError("Brain is null");
        if (!brain->initialize()) {
            throw BrainInitializationError("Brain initialization failed");
        }
        return true;
    }, py::arg("brain"), "Initialize brain with validation");

    // Enhanced utility functions
    m.def("benchmarkBrains", [](const std::vector<std::shared_ptr<Brain>>& brains, size_t steps) {
        py::list results;
        for (size_t i = 0; i < brains.size(); ++i) {
            auto& brain = brains[i];
            auto startTime = std::chrono::high_resolution_clock::now();
            for (size_t step = 0; step < steps; ++step) {
                brain->step(step, step * 0.001);
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            double timeTaken = std::chrono::duration<double>(endTime - startTime).count();
            
            py::dict result;
            result["index"] = i;
            result["neurons"] = brain->getTotalNeuronCount();
            result["synapses"] = brain->getTotalSynapseCount();
            result["steps_per_sec"] = steps / timeTaken;
            result["total_spikes"] = brain->getTotalSpikeCount();
            results.append(result);
        }
        return results;
    }, py::arg("brains"), py::arg("steps"), "Benchmark multiple brains");

    m.def("createLearningEnvironment", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to create learning environment");
        }
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->setAgentStart(10.0, 10.0);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        py::dict env;
        env["config"] = config;
        env["brain"] = brain;
        env["world"] = world;
        env["agent"] = agent;
        return env;
    }, "Create a learning environment");

    // Version and metadata
    m.attr("__version__") = "6.0.0-enhanced";
    m.attr("__description__") = "NLM Python bindings - Enhanced edition with advanced features";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__license__") = "MIT";

    m.def("get_version", []() { return std::string("6.0.0-enhanced"); },
          "Get the version of the enhanced bindings");

    m.def("get_features", []() {
        py::list features;
        features.append("enhanced_error_handling");
        features.append("advanced_config_validation");
        features.append("numpy_array_support");
        features.append("brain_benchmarking");
        features.append("phase6_integration");
        features.append("learning_environment_factory");
        features.append("enhanced_documentation");
        return features;
    }, "Get list of available features");

    // Enhanced exception translations
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const ValidationError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const ConfigurationError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const BrainInitializationError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred");
        }
    });

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Documentation
    m.doc() += R"pbdoc(

    Enhanced Features:
    - Comprehensive error handling with custom exception types
    - Enhanced configuration validation and source tracking
    - NumPy array support for data exchange
    - Brain benchmarking capabilities
    - Phase 6 integration utilities
    - Learning environment factory
    - Advanced agent features
    - Enhanced documentation and examples

    Quick Start Example:
        import pynlm
        
        # Create a default configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        
        # Create brain and world
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())

    Advanced Features:
        - Phase 6 integration testing
        - Brain benchmarking
        - Configuration validation
        - Enhanced error handling
        - NumPy array support
    )pbdoc";

    }

} // namespace nlm
