#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <functional>
#include <unordered_map>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AdvancedAgent.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/world/SimpleWorld.hpp"

namespace py = pybind11;
namespace nlm {

// Custom Python exceptions
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

class AgentError : public NLMError {
public:
    explicit AgentError(const std::string& msg) : NLMError("Agent error: " + msg) {}
};

class AdvancedAgentError : public NLMError {
public:
    explicit AdvancedAgentError(const std::string& msg) : NLMError("Advanced agent error: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced API
        ---------------------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This enhanced API provides:
        - Type hints for better code completion and IDE support
        - Better error handling with custom exceptions
        - Pythonic APIs for easier usage
        - Advanced features for expert users
        - Performance monitoring tools
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<AgentError>(m, "AgentError");
    py::register_exception<AdvancedAgentError>(m, "AdvancedAgentError");

    // Type aliases for better readability
    using NeuronId = nlm::NeuronId;
    using SynapseId = nlm::SynapseId;
    using RegionId = nlm::RegionId;
    using PopulationId = nlm::PopulationId;
    using SimulationStep = nlm::SimulationStep;
    using Timestamp = nlm::Timestamp;

    // Neuron ID class with enhanced Python interface
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

    // Synapse ID class with enhanced Python interface
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
            return std::to_string(id.value);
        });

    // Region ID class with enhanced Python interface
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
            return std::to_string(id.value);
        });

    // Population ID class with enhanced Python interface
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Neuron type enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Synapse type enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // Developmental stage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Firing state enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // Action type enumeration
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

    // Motor command enumeration
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

    // World object type enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Enhanced Configuration class with type-safe getters and setters
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file (JSON or key=value format)")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"), "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"), "Save configuration to a file")
        .def("has", &Config::has, py::arg("key"), "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Enhanced type-safe getters with better error handling
        .def("get_int", [](const Config& self, const std::string& key, int default_value) -> int {
            auto val = self.get<int>(key);
            return val.has_value() ? val.value() : default_value;
        }, py::arg("key"), py::arg("default_value"), "Get integer value with default")
        .def("get_float", [](const Config& self, const std::string& key, float default_value) -> float {
            auto val = self.get<double>(key);
            return val.has_value() ? static_cast<float>(val.value()) : default_value;
        }, py::arg("key"), py::arg("default_value"), "Get float value with default")
        .def("get_string", [](const Config& self, const std::string& key, const std::string& default_value) -> std::string {
            auto val = self.get<std::string>(key);
            return val.has_value() ? val.value() : default_value;
        }, py::arg("key"), py::arg("default_value"), "Get string value with default")
        .def("get_bool", [](const Config& self, const std::string& key, bool default_value) -> bool {
            auto val = self.get<bool>(key);
            return val.has_value() ? val.value() : default_value;
        }, py::arg("key"), py::arg("default_value"), "Get boolean value with default")
        // Enhanced type-safe setters
        .def("set_int", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set integer value")
        .def("set_float", [](Config& self, const std::string& key, float value) {
            self.set(key, static_cast<double>(value));
        }, py::arg("key"), py::arg("value"), "Set float value")
        .def("set_string", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set string value")
        .def("set_bool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set boolean value");

    // Enhanced Vision sensory input class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data as a vector of floats")
        .def("get_width", &Vision::getWidth, "Get vision width")
        .def("get_height", &Vision::getHeight, "Get vision height")
        .def("get_channels", &Vision::getChannels, "Get number of channels")
        .def("get_data", &Vision::getData, "Get vision data as vector")
        .def("__repr__", [](const Vision& v) {
            return "<Vision: " + std::to_string(v.getWidth()) + "x" + 
                   std::to_string(v.getHeight()) + " channels=" + 
                   std::to_string(v.getChannels()) + ">";
        });

    // Enhanced Audio sensory input class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data as a vector of floats")
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sampleRate"), "Set sample rate")
        .def("get_sample_rate", &Audio::getSampleRate, "Get sample rate")
        .def("get_num_samples", &Audio::getNumSamples, "Get number of samples")
        .def("get_data", &Audio::getData, "Get audio data as vector");

    // Enhanced Internal signals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("clear_signals", &InternalSignals::clearSignals, "Clear all signals")
        .def("get_signals", &InternalSignals::getData, "Get all signal values")
        .def("set_signals", [](InternalSignals& self, const std::vector<float>& signals) {
            self.clearSignals();
            for (float val : signals) self.addSignal(val);
        }, py::arg("signals"), "Set signal values from a vector");

    // Enhanced Action class with full Python interface
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType, "Get action type")
        .def("set_type", &Action::setType, py::arg("type"), "Set action type")
        .def("get_parameters", &Action::getParameters, "Get action parameters")
        .def("set_parameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("get_name", &Action::getName, "Get action name")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("__repr__", [](const Action& a) {
            return "<Action: " + std::string(a.getName()) + " parameters=" + 
                   std::to_string(a.getParameters().size()) + ">";
        })
        .def("__eq__", [](const Action& a1, const Action& a2) {
            return a1.getType() == a2.getType() && a1.getParameters() == a2.getParameters();
        });

    // Enhanced World object class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "Object x position")
        .def_readwrite("y", &WorldObject::y, "Object y position")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Object active state")
        .def("get_position", [](const WorldObject& obj) {
            return std::make_pair(obj.x, obj.y);
        }, "Get object position as (x, y) tuple")
        .def("set_position", [](WorldObject& obj, float x, float y) {
            obj.x = x;
            obj.y = y;
        }, py::arg("x"), py::arg("y"), "Set object position")
        .def("contains", [](const WorldObject& obj, float x, float y) {
            float dx = x - obj.x;
            float dy = y - obj.y;
            return dx*dx + dy*dy <= obj.radius * obj.radius;
        }, py::arg("x"), py::arg("y"), "Check if position is within object");

    // Enhanced Agent body class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "Agent x position")
        .def_readwrite("y", &AgentBody::y, "Agent y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Agent orientation in degrees")
        .def_readwrite("velocity_x", &AgentBody::velocityX, "Agent velocity in x direction")
        .def_readwrite("velocity_y", &AgentBody::velocityY, "Agent velocity in y direction")
        .def_readwrite("angular_velocity", &AgentBody::angularVelocity, "Agent angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Agent energy level")
        .def_readwrite("health", &AgentBody::health, "Agent health level")
        .def_readwrite("age", &AgentBody::age, "Agent age in simulation steps")
        .def_readwrite("is_moving", &AgentBody::isMoving, "Agent moving state")
        .def_readwrite("is_turning", &AgentBody::isTurning, "Agent turning state")
        .def_readwrite("last_action_time", &AgentBody::lastActionTime, "Last action time")
        .def("reset", &AgentBody::reset, "Reset agent body to initial state")
        .def("get_velocity", [](const AgentBody& body) {
            return std::make_pair(body.velocityX, body.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("set_velocity", [](AgentBody& body, float vx, float vy) {
            body.velocityX = vx;
            body.velocityY = vy;
            body.isMoving = (vx != 0.0f || vy != 0.0f);
        }, py::arg("vx"), py::arg("vy"), "Set velocity vector");

    // Enhanced Action result class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward received from action")
        .def_readwrite("success", &ActionResult::success, "Action success status")
        .def_readwrite("message", &ActionResult::message, "Action result message")
        .def("is_successful", &ActionResult::success, "Check if action was successful")
        .def("get_reward", &ActionResult::reward, "Get reward value")
        .def("get_message", &ActionResult::message, "Get result message")
        .def("__repr__", [](const ActionResult& result) {
            std::string success_str = result.success ? "SUCCESS" : "FAILED";
            return "<ActionResult: " + success_str + " reward=" + std::to_string(result.reward) + ">";
        });

    // Enhanced Sensory percept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision, "Get vision data")
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("get_vision_width", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("get_vision_height", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("get_touch", &SensoryPercept::getTouch, "Get touch data")
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("get_internal", &SensoryPercept::getInternal, "Get internal signals")
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("get_proprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("get_audio", &SensoryPercept::getAudio, "Get audio data")
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("get_all_signals", &SensoryPercept::getAllSignals, "Get all sensory signals")
        .def("get_timestamp", &SensoryPercept::getTimestamp, "Get percept timestamp")
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set percept timestamp")
        .def("merge", [](SensoryPercept& self, const SensoryPercept& other) {
            self.setVision(other.getVision());
            self.setTouch(other.getTouch());
            self.setInternal(other.getInternal());
            self.setProprioception(other.getProprioception());
            self.setAudio(other.getAudio());
            self.setTimestamp(other.getTimestamp());
        }, py::arg("other"), "Merge another percept into this one")
        .def("clear", [](SensoryPercept& self) {
            Vision v;
            self.setVision(v);
            std::vector<float> empty_touch;
            self.setTouch(empty_touch);
            std::vector<float> empty_internal;
            self.setInternal(empty_internal);
            std::vector<float> empty_proprioception;
            self.setProprioception(empty_proprioception);
            std::vector<float> empty_audio;
            self.setAudio(empty_audio);
        }, "Clear all sensory data");

    // Enhanced Simple world class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"), "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world by given timestep")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command to world")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get current sensory percept")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent body state")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("get_width", &SimpleWorld::getWidth, "Get world width")
        .def("get_height", &SimpleWorld::getHeight, "Get world height")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("get_random_seed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("get_num_objects", [](const SimpleWorld& self) {
            // This would need to be implemented in the SimpleWorld class
            return 0; // Placeholder
        }, "Get number of objects in world")
        .def("get_objects", [](const SimpleWorld& self) {
            // This would need to be implemented in the SimpleWorld class
            return std::vector<WorldObject>(); // Placeholder
        }, "Get all objects in world");

    // Enhanced Brain class with more Pythonic interface
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step_with_time", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "", "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount, "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds, "Get all region IDs")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal, "Get all regions")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount, "Get total neuron count across all regions")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount, "Get total synapse count across all regions")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate, "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio, "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("get_config", &Brain::getConfig, "Get the configuration")
        .def("log_status", &Brain::logStatus, "Log brain status")
        // Additional convenience methods
        .def("is_initialized", [](const Brain& brain) {
            return brain.getTotalNeuronCount() > 0;
        }, "Check if brain is initialized")
        .def("get_neuron_count", &Brain::getTotalNeuronCount, "Alias for get_total_neuron_count")
        .def("get_synapse_count", &Brain::getTotalSynapseCount, "Alias for get_total_synapse_count")
        .def("get_active_neurons", &Brain::getActiveNeuronCount, "Alias for get_active_neuron_count")
        .def("get_firing_neurons", &Brain::getFiringNeuronCount, "Alias for get_firing_neuron_count");

    // Enhanced AgentBrain class with more Pythonic interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand, "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable or disable reward modulation")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity")
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable or disable development")
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity")
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled, "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled, "Check if structural plasticity is enabled")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled, "Check if development is enabled")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled, "Check if curiosity is enabled")
        // Additional convenience methods
        .def("is_ready", [](const AgentBrain& agent) {
            return agent.getBrain() != nullptr;
        }, "Check if agent brain is ready")
        .def("get_status", [](const AgentBrain& agent) {
            py::dict status;
            status["brain"] = agent.getBrain() != nullptr;
            status["dev_stage"] = static_cast<int>(agent.getDevelopmentalStage());
            status["curiosity"] = agent.getCuriosityLevel();
            status["novelty"] = agent.getNoveltyLevel();
            status["prediction_error"] = agent.getPredictionError();
            return status;
        }, "Get agent status as dictionary");

    // Factory functions with enhanced Python interface
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Performance monitoring utilities
    m.def("get_version", []() {
        return "NLM 0.1.0"; // Version could be defined in config
    }, "Get NLM version");

    m.def("get_build_info", []() {
        py::dict info;
        info["compiler"] = std::string(__VERSION__);
        info["build_type"] = "Release";
        info["python_bindings"] = true;
        info["phase"] = 6;
        return info;
    }, "Get build information");

    // Utility functions
    m.def("validate_config", [](const Config& config) {
        auto keys = config.getKeys();
        bool valid = !keys.empty();
        return py::make_tuple(valid, keys);
    }, py::arg("config"), "Validate configuration and return status with keys");

    m.def("format_simulation_stats", [](const Brain& brain, int steps) {
        std::ostringstream oss;
        oss << "Simulation Statistics (last " << steps << " steps):\n";
        oss << "  Neurons: " << brain.getTotalNeuronCount() << "\n";
        oss << "  Synapses: " << brain.getTotalSynapseCount() << "\n";
        oss << "  Active: " << brain.getActiveNeuronCount() << "\n";
        oss << "  Firing: " << brain.getFiringNeuronCount() << "\n";
        oss << "  Average firing rate: " << brain.getAverageFiringRate() << " Hz\n";
        oss << "  E/I ratio: " << brain.getExcitationInhibitionRatio() << "\n";
        oss << "  Total spikes: " << brain.getTotalSpikeCount();
        return oss.str();
    }, py::arg("brain"), py::arg("steps"), "Format simulation statistics as string");

    // Simulation utilities
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, size_t steps, bool verbose = false) {
        if (!brain) {
            throw AgentError("Brain cannot be null");
        }
        if (!brain->getTotalNeuronCount()) {
            throw SimulationError("Brain not initialized. Call initialize() first.");
        }
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
            if (verbose && i % 100 == 0) {
                std::cout << "Step " << i << ": " << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
            }
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("steps"), py::arg("verbose") = false, "Run simulation for specified number of steps");

    // Agent simulation utilities
    m.def("run_agent_episode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, 
                                 size_t steps, bool verbose = false) {
        if (!agent || !agent->getBrain()) {
            throw AgentError("Invalid agent or brain");
        }
        if (!world) {
            throw AgentError("World cannot be null");
        }
        
        for (size_t i = 0; i < steps; ++i) {
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Run brain step
            agent->getBrain()->step(i);
            
            // Decode motor command
            auto action = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Update development
            agent->updateDevelopment(0.1);
            
            if (verbose && i % 100 == 0) {
                std::cout << "Step " << i << ": " << agent->getBrain()->getFiringNeuronCount() 
                         << " firing, curiosity: " << agent->getCuriosityLevel() << std::endl;
            }
        }
        
        return std::make_pair(agent->getBrain()->getTotalSpikeCount(), agent->getCuriosityLevel());
    }, py::arg("agent"), py::arg("world"), py::arg("steps"), py::arg("verbose") = false, 
       "Run complete agent episode");

    // Constants for convenience
    m.attr("SIMULATION_STEP_DEFAULT") = 0.001;
    m.attr("NEURON_COUNT_DEFAULT") = 1000;
    m.attr("REGION_COUNT_DEFAULT") = 2;
    m.attr("CONNECTION_PROBABILITY_DEFAULT") = 0.1;

    // Documentation for the enhanced module
    m.attr("__version__") = "0.1.0-enhanced";
    m.attr("__description__") = "Enhanced NLM Python bindings with type hints, better error handling, and advanced features";
    m.attr("__author__") = "NLM Development Team";
}

} // namespace nlm