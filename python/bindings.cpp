#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

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

// Python exception class
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

// Helper functions for parameter validation
template<typename T>
void validateNonNull(const T& value, const std::string& name) {
    if (!value) {
        throw NLMError("Parameter '" + name + "' cannot be null");
    }
}

template<typename T>
void validatePositive(const T& value, const std::string& name) {
    if (value <= 0) {
        throw NLMError("Parameter '" + name + "' must be positive, got " + std::to_string(value));
    }
}

template<typename T>
void validateNonNegative(const T& value, const std::string& name) {
    if (value < 0) {
        throw NLMError("Parameter '" + name + "' must be non-negative, got " + std::to_string(value));
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key features:
        - Complete Python API with type checking
        - Comprehensive error handling and validation
        - Advanced user features (callbacks, properties, iterators)
        - Performance optimizations for large simulations
        - Integration with Python scientific computing ecosystem
    )pbdoc";
    
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework";

    // Register Python exception
    py::register_exception<NLMError>(m, "NLMError");

    // NeuronId class with enhanced Python features
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index, "Get the index of this neuron ID")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId class with enhanced Python features
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index, "Get the index of this synapse ID")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // RegionId class with enhanced Python features
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index, "Get the index of this region ID")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // PopulationId class with enhanced Python features
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index, "Get the index of this population ID")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); });

    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron that promotes firing")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron that suppresses firing")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron affecting plasticity")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron receiving external input")
        .value("Motor", NeuronType::Motor, "Motor neuron producing actions")
        .value("Internal", NeuronType::Internal, "Internal processing neuron")
        .export_values();

    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse that increases firing")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse that decreases firing")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse affecting plasticity")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse with fast transmission")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction coupling")
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period for learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron at rest")
        .value("Active", FiringState::Active, "Neuron actively firing")
        .value("Refractory", FiringState::Refractory, "Neuron in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron inhibited from firing")
        .export_values();

    // ActionType enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward action")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward action")
        .value("MoveLeft", ActionType::MoveLeft, "Move left action")
        .value("MoveRight", ActionType::MoveRight, "Move right action")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left action")
        .value("TurnRight", ActionType::TurnRight, "Turn right action")
        .value("Look", ActionType::Look, "Look action")
        .value("LookUp", ActionType::LookUp, "Look up action")
        .value("LookDown", ActionType::LookDown, "Look down action")
        .value("Interact", ActionType::Interact, "Interact action")
        .value("Eat", ActionType::Eat, "Eat action")
        .value("Drink", ActionType::Drink, "Drink action")
        .value("Rest", ActionType::Rest, "Rest action")
        .value("Wait", ActionType::Wait, "Wait action")
        .value("Custom", ActionType::Custom, "Custom action")
        .export_values();

    // MotorCommand enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward command")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward command")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left command")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right command")
        .value("LookLeft", MotorCommand::LookLeft, "Look left command")
        .value("LookRight", MotorCommand::LookRight, "Look right command")
        .value("Interact", MotorCommand::Interact, "Interact command")
        .value("Wait", MotorCommand::Wait, "Wait command")
        .export_values();

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty world cell")
        .value("Resource", WorldObjectType::Resource, "Resource object")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object")
        .value("Wall", WorldObjectType::Wall, "Wall object")
        .value("Marker", WorldObjectType::Marker, "Marker object")
        .export_values();

    // Config class with enhanced Python features
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
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        })
        // Add convenience methods for Python users
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, value, nlm::ConfigSource::Default);
        }, py::arg("key"), py::arg("value"), "Set a float configuration value")
        .def("setInt", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, nlm::ConfigSource::Default);
        }, py::arg("key"), py::arg("value"), "Set an integer configuration value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, nlm::ConfigSource::Default);
        }, py::arg("key"), py::arg("value"), "Set a string configuration value")
        .def("getFloat", [](Config& self, const std::string& key, float defaultValue = 0.0f) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0.0f, "Get a float configuration value")
        .def("getInt", [](Config& self, const std::string& key, int64_t defaultValue = 0) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0, "Get an integer configuration value");

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput type=" + std::to_string(static_cast<int>(input.getType())) + ">";
        });

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Create a Vision input with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            validateNonNull(self, "Vision");
            self.setData(data);
        }, py::arg("data"), "Set the vision data")
        .def("getWidth", &Vision::getWidth, "Get the width of vision data")
        .def("getHeight", &Vision::getHeight, "Get the height of vision data")
        .def("getChannels", &Vision::getChannels, "Get the number of color channels")
        .def("setDataValidated", [](Vision& self, const std::vector<float>& data, size_t width, size_t height) {
            validateNonNull(self, "Vision");
            if (width == 0 || height == 0) {
                throw NLMError("Vision dimensions must be positive");
            }
            if (data.size() != width * height * 3) {
                throw NLMError("Vision data size mismatch: expected " + 
                    std::to_string(width * height * 3) + " elements, got " + 
                    std::to_string(data.size()));
            }
            self.setData(data);
        }, py::arg("data"), py::arg("width"), py::arg("height"), "Set validated vision data");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create an Audio input with specified sample rate and number of samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            validateNonNull(self, "Audio");
            self.setData(data);
        }, py::arg("data"), "Set the audio data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get the sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get the number of samples")
        .def("setSampleRateValidated", [](Audio& self, size_t sampleRate) {
            validatePositive(sampleRate, "sampleRate");
            self.setSampleRate(sampleRate);
        }, py::arg("sampleRate"), "Set validated sample rate");

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add an internal signal value")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals")
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get the number of internal signals");

    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"), "Create an action with specified type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create an action with type and parameters")
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get the action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set the action parameters")
        .def("getName", &Action::getName, "Get the action name")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("__repr__", [](const Action& action) {
            return "<Action type=" + std::to_string(static_cast<int>(action.getType())) + 
                   " parameters=" + std::to_string(action.getParameters().size()) + ">";
        });

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, "Create a world object with position and properties")
        .def_readwrite("x", &WorldObject::x, "X coordinate")
        .def_readwrite("y", &WorldObject::y, "Y coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Radius of the object")
        .def_readwrite("type", &WorldObject::type, "Type of the object")
        .def_readwrite("value", &WorldObject::value, "Value of the object")
        .def_readwrite("active", &WorldObject::active, "Whether the object is active")
        .def("distanceTo", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position");

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X coordinate")
        .def_readwrite("y", &AgentBody::y, "Y coordinate")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level")
        .def_readwrite("health", &AgentBody::health, "Health level")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether the agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether the agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset body to default state")
        .def("setPosition", [](AgentBody& body, float x, float y) {
            validateNonNegative(x, "x");
            validateNonNegative(y, "y");
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("setVelocity", [](AgentBody& body, float vx, float vy) {
            body.velocityX = vx;
            body.velocityY = vy;
        }, py::arg("vx"), py::arg("vy"), "Set agent velocity");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create an action result with reward, success, and message")
        .def_readwrite("reward", &ActionResult::reward, "Reward received")
        .def_readwrite("success", &ActionResult::success, "Whether the action succeeded")
        .def_readwrite("message", &ActionResult::message, "Result message")
        .def("__repr__", [](const ActionResult& result) {
            std::string successStr = result.success ? "SUCCESS" : "FAILURE";
            return "<ActionResult reward=" + std::to_string(result.reward) + 
                   " success=" + successStr + ">";
        });

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, py::return_value_policy::reference_internal,
             "Get vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, py::return_value_policy::reference_internal,
             "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, py::return_value_policy::reference_internal,
             "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, py::return_value_policy::reference_internal,
             "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, py::return_value_policy::reference_internal,
             "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, py::return_value_policy::reference_internal,
             "Get all sensory signals")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp")
        .def("hasVision", [](const SensoryPercept& percept) {
            return percept.getVision() != nullptr;
        }, "Check if percept has vision data")
        .def("hasAudio", [](const SensoryPercept& percept) {
            return percept.getAudio() != nullptr;
        }, "Check if percept has audio data")
        .def("hasTouch", [](const SensoryPercept& percept) {
            return percept.getTouch() != nullptr;
        }, "Check if percept has touch data")
        .def("hasInternal", [](const SensoryPercept& percept) {
            return percept.getInternal() != nullptr;
        }, "Check if percept has internal signals");

    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world dimensions and agent sensing capabilities")
        .def("reset", &SimpleWorld::reset, "Reset the world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set the agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update the world simulation by the given timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command to the world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get the current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get the agent's body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove a world object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not occupied by wall)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulation")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("setAgentStartValidated", [](SimpleWorld& world, float x, float y) {
            validateNonNegative(x, "x");
            validateNonNegative(y, "y");
            world.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), "Set and validate agent starting position")
        .def("updateValidated", [](SimpleWorld& world, float timestep) {
            validatePositive(timestep, "timestep");
            world.update(timestep);
        }, py::arg("timestep"), "Update world with validation");

    // Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a brain with configuration")
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
        .def("injectCurrentValidated", [](Brain& brain, NeuronId neuron, float current) {
            validatePositive(current, "current");
            brain.injectCurrent(neuron, current);
        }, py::arg("neuron"), py::arg("current"),
           "Inject current into a specific neuron with validation")
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
        .def("stepWithTime", [](Brain& brain, size_t step, float time) {
            brain.step(step, time);
        }, py::arg("step"), py::arg("time"),
           "Convenience method for stepping with time")
        .def("getBrainStats", [](Brain& brain) {
            py::dict stats;
            stats["total_neurons"] = brain.getTotalNeuronCount();
            stats["total_synapses"] = brain.getTotalSynapseCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["active_neurons"] = brain.getActiveNeuronCount();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            stats["avg_firing_rate"] = brain.getAverageFiringRate();
            stats["e_i_ratio"] = brain.getExcitationInhibitionRatio();
            stats["development_stage"] = static_cast<int>(brain.getDevelopmentalStage());
            return stats;
        }, "Get comprehensive brain statistics");

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an agent brain interface")
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
             py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("getAgentStats", [](AgentBrain& agent) {
            py::dict stats;
            stats["curiosity"] = agent.getCuriosityLevel();
            stats["novelty"] = agent.getNoveltyLevel();
            stats["neuromodulation"] = agent.getNeuromodulationLevel();
            stats["prediction_error"] = agent.getPredictionError();
            stats["dev_stage"] = static_cast<int>(agent.getDevelopmentalStage());
            stats["reward_modulation_enabled"] = agent.isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = agent.isStructuralPlasticityEnabled();
            stats["development_enabled"] = agent.isDevelopmentEnabled();
            stats["curiosity_enabled"] = agent.isCuriosityEnabled();
            return stats;
        }, "Get comprehensive agent statistics");

    // Module functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        validateNonNull(config, "config");
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        validateNonNull(brain, "brain");
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Context manager support for Brain
    py::class_<Brain>::def("__enter__", [](Brain& self) -> Brain& { return self; });
    py::class_<Brain>::def("__exit__", [](Brain& self, py::args) {});

    // Iterator support for collections
    py::class_<std::vector<NeuronId>>().def("__iter__", [](py::object& vec) {
        return py::make_iterator(vec.cast<std::vector<NeuronId>&>().begin(),
                               py::make_iterator(vec.cast<std::vector<NeuronId>&>().end()));
    });

    py::class_<std::vector<SynapseId>>().def("__iter__", [](py::object& vec) {
        return py::make_iterator(vec.cast<std::vector<SynapseId>&>().begin(),
                               py::make_iterator(vec.cast<std::vector<SynapseId>&>().end()));
    });

    py::class_<std::vector<RegionId>>().def("__iter__", [](py::object& vec) {
        return py::make_iterator(vec.cast<std::vector<RegionId>&>().begin(),
                               py::make_iterator(vec.cast<std::vector<RegionId>&>().end()));
    });

    // Advanced user features
    m.def("createCallbackBrain", [](std::function<void(size_t)> callback, std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Set up callback for each step
        py::cpp_function pyCallback(callback);
        
        return py::make_tuple(brain, pyCallback);
    }, py::arg("callback"), py::arg("config"),
       "Create a brain with a callback for monitoring (Advanced feature)");

    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, size_t steps, size_t numRuns) {
        if (!brain) {
            throw NLMError("Brain cannot be null");
        }
        validatePositive(steps, "steps");
        validatePositive(numRuns, "numRuns");
        
        std::vector<double> times;
        for (size_t run = 0; run < numRuns; ++run) {
            brain->reset();
            brain->initialize();
            
            auto start = std::chrono::high_resolution_clock::now();
            for (size_t step = 0; step < steps; ++step) {
                brain->step(step);
            }
            auto end = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration<double>(end - start).count();
            times.push_back(duration);
        }
        
        double avgTime = 0.0;
        double minTime = times[0];
        double maxTime = times[0];
        for (double t : times) {
            avgTime += t;
            if (t < minTime) minTime = t;
            if (t > maxTime) maxTime = t;
        }
        avgTime /= times.size();
        
        py::dict results;
        results["avg_time"] = avgTime;
        results["min_time"] = minTime;
        results["max_time"] = maxTime;
        results["steps"] = steps;
        results["runs"] = numRuns;
        results["steps_per_second"] = steps / avgTime;
        
        return results;
    }, py::arg("brain"), py::arg("steps"), py::arg("numRuns"),
       "Benchmark brain performance (Advanced feature)");
}

} // namespace nlm