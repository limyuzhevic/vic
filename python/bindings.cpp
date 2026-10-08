#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
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

// Helper template for creating ID class bindings
// Reduces code duplication across all ID types
template<typename IDClass>
void bindIdClass(py::module_& m, const std::string& className, const std::string& docString) {
    std::string reprPrefix = "<" + className + ": ";
    std::string reprSuffix = ">";
    
    py::class_<IDClass>(m, className.c_str(), py::doc(docString.c_str()))
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_property("value", &IDClass::value, nullptr, 
                     "Get or set the ID value")
        .def("index", &IDClass::index, "Get the index of this ID")
        .def("__eq__", &IDClass::operator==, "Check equality with another ID")
        .def("__ne__", &IDClass::operator!=, "Check inequality with another ID")
        .def("__hash__", [](const IDClass& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash value for use in Python sets and dictionaries")
        .def("__repr__", [reprPrefix, reprSuffix](const IDClass& id) {
            return reprPrefix + std::to_string(id.value) + reprSuffix;
        }, "Return string representation of the ID")
        .def("is_valid", [](const IDClass& id) { 
            return id.value != UINT64_MAX && id.value != 0; 
        }, "Check if the ID is valid (not zero or max)")
        .def("is_invalid", [](const IDClass& id) { 
            return id.value == UINT64_MAX || id.value == 0; 
        }, "Check if the ID is invalid (zero or max)")
        .def("to_string", [](const IDClass& id) { 
            return std::to_string(id.value); 
        }, "Convert ID to string representation")
        .def_static("from_string", [](const std::string& str) {
            try {
                return IDClass(std::stoull(str));
            } catch (const std::exception&) {
                throw std::runtime_error("Invalid string format for ID. Must be a non-negative integer.");
            }
        }, py::arg("str"), "Create ID from string representation")
        .def("to_int", [](const IDClass& id) { 
            return static_cast<int>(id.value); 
        }, "Convert ID to integer (truncated if larger than int range)")
        .def_static("from_int", [](int value) {
            if (value < 0) {
                throw std::runtime_error("ID value cannot be negative.");
            }
            return IDClass(static_cast<uint64_t>(value));
        }, py::arg("value"), "Create ID from integer");
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        Provides classes for:
        - Brain: Central neural simulation brain class
        - Config: Configuration management
        - AgentBrain: Interface connecting NLM brain to world
        - SimpleWorld: Simple 2D world for NLM simulation
        - SensoryInput: Base class for sensory input
        - Action: Action representation for motor output
        - And more...
        
        Includes strongly-typed identifiers (NeuronId, SynapseId, etc.) with Python-friendly methods
        for validation, conversion, and string operations.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Bind all ID classes using the helper template
    bindIdClass<NeuronId>(m, "NeuronId", 
        "Unique identifier for a neuron. Supports Python string/integer conversion and validation.");
    bindIdClass<SynapseId>(m, "SynapseId", 
        "Unique identifier for a synapse. Supports Python string/integer conversion and validation.");
    bindIdClass<RegionId>(m, "RegionId", 
        "Unique identifier for a brain region. Supports Python string/integer conversion and validation.");
    bindIdClass<PopulationId>(m, "PopulationId", 
        "Unique identifier for a neuron population. Supports Python string/integer conversion and validation.");

    // Basic type aliases for convenience
    m.attr("SIMULATION_STEP") = py::int_(std::numeric_limits<nlm::SimulationStep>::max());
    m.attr("TIMESTAMP_MAX") = py::float_(std::numeric_limits<nlm::Timestamp>::max());
    m.attr("MAX_UINT64") = py::int_(UINT64_MAX);
    
    // Conversion utilities
    m.def("parse_uint64", [](const std::string& str) {
        try {
            return std::stoull(str);
        } catch (const std::exception&) {
            throw std::runtime_error("Invalid number format: '" + str + "'");
        }
    }, py::arg("str"), "Parse string to unsigned 64-bit integer");
    
    m.def("format_uint64", [](uint64_t value) {
        return std::to_string(value);
    }, py::arg("value"), "Format unsigned 64-bit integer to string");

    py::enum_<NeuronType>(m, "NeuronType", "Enumeration of neuron types")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron type")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron type")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron type")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron type")
        .value("Motor", NeuronType::Motor, "Motor neuron type")
        .value("Internal", NeuronType::Internal, "Internal neuron type")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", "Enumeration of synapse types")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse type")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse type")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse type")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse type")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse type")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", "Enumeration of developmental stages")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period stage")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", "Enumeration of neuron firing states")
        .value("Resting", FiringState::Resting, "Neuron is resting")
        .value("Active", FiringState::Active, "Neuron is firing")
        .value("Refractory", FiringState::Refractory, "Neuron is in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron is inhibited")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", "Enumeration of action types")
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

    py::enum_<MotorCommand>(m, "MotorCommand", "Enumeration of low-level motor commands")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward command")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward command")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left command")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right command")
        .value("LookLeft", MotorCommand::LookLeft, "Look left command")
        .value("LookRight", MotorCommand::LookRight, "Look right command")
        .value("Interact", MotorCommand::Interact, "Interact command")
        .value("Wait", MotorCommand::Wait, "Wait command")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", "Enumeration of world object types")
        .value("Empty", WorldObjectType::Empty, "Empty object")
        .value("Resource", WorldObjectType::Resource, "Resource object")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object")
        .value("Wall", WorldObjectType::Wall, "Wall object")
        .value("Marker", WorldObjectType::Marker, "Marker object")
        .export_values();

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system
        
        The Config class provides a flexible key-value store for managing NLM configuration parameters.
        Values can be integers, doubles, booleans, strings, or lists of these types.
        
        Example:
            >>> config = Config()
            >>> config.set("neuron_count", 100)
            >>> config.set("connection_probability", 0.1)
            >>> config["neuron_count"]
            100
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Throws RuntimeError if file cannot be read or parsed.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments. Standard argument parsing applies.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Throws RuntimeError if file cannot be written.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns True if key exists, False otherwise.")
        .def("__contains__", &Config::has,
             "Check if a configuration key exists. Returns True if key exists, False otherwise.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list of strings.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration. Useful for debugging.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "Return string representation of the configuration.")
        .def("__getitem__", [](Config& self, const std::string& key) {
            if constexpr (std::is_same_v<Config, nlm::Config>) {
                // Try common types
                if (auto val = self.get<int64_t>(key)) return *val;
                if (auto val = self.get<double>(key)) return *val;
                if (auto val = self.get<bool>(key)) return *val;
                if (auto val = self.get<std::string>(key)) return *val;
                if (auto val = self.get<std::vector<int>>(key)) return *val;
                if (auto val = self.get<std::vector<double>>(key)) return *val;
                if (auto val = self.get<std::vector<std::string>>(key)) return *val;
            }
            throw std::runtime_error("Key not found or wrong type: '" + key + "'");
        }, py::arg("key"), "Get configuration value by key. Returns the value as appropriate Python type.")
        .def("__setitem__", [](Config& self, const std::string& key, const py::object& value) {
            if (value.is_none()) {
                throw std::runtime_error("Cannot set None value for key: '" + key + "'");
            }
            if (value.is(py::bool_())) {
                self.set(key, value.cast<bool>());
            } else if (value.is(py::int_())) {
                self.set(key, value.cast<int64_t>());
            } else if (value.is(py::float_())) {
                self.set(key, value.cast<double>());
            } else if (value.is(py::str())) {
                self.set(key, value.cast<std::string>());
            } else if (py::isinstance(value, py::list())) {
                // Try to infer list element type
                auto py_list = value.cast<std::vector<py::object>>();
                if (!py_list.empty()) {
                    if (py::isinstance(py_list[0], py::int_())) {
                        self.set(key, py_list | std::views::transform([](const py::object& obj) { return obj.cast<int64_t>(); }) | std::views::common);
                    } else if (py::isinstance(py_list[0], py::float_())) {
                        self.set(key, py_list | std::views::transform([](const py::object& obj) { return obj.cast<double>(); }) | std::views::common);
                    } else if (py::isinstance(py_list[0], py::str())) {
                        self.set(key, py_list | std::views::transform([](const py::object& obj) { return obj.cast<std::string>(); }) | std::views::common);
                    } else {
                        throw std::runtime_error("Unsupported list element type for key: '" + key + "'");
                    }
                } else {
                    self.set(key, std::vector<int64_t>()); // Empty int list
                }
            } else {
                throw std::runtime_error("Unsupported value type for key: '" + key + "'. Supported types: int, float, str, bool, list of int/float/str");
            }
        }, py::arg("key"), py::arg("value"), "Set configuration value by key. Supports Python int, float, str, bool, and list types.");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input
        
        Abstract base class for all sensory input types. Provides common interface for
        accessing sensory data and timestamps.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp. Throws RuntimeError if timestamp is negative.");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input
        
        Represents visual input with width, height, and color channels.
        
        Example:
            >>> vision = Vision(640, 480, 3)
            >>> vision.setData(pixel_data)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Create vision input with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw std::runtime_error("Cannot set empty vision data");
            }
            self.setData(data);
        }, py::arg("data"), "Set vision data. Throws RuntimeError if data is empty.")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input
        
        Represents audio input with sample rate and number of samples.
        
        Example:
            >>> audio = Audio(44100, 1024)
            >>> audio.setData(audio_samples)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create audio input with specified sample rate and number of samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw std::runtime_error("Cannot set empty audio data");
            }
            self.setData(data);
        }, py::arg("data"), "Set audio data. Throws RuntimeError if data is empty.")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate. Throws RuntimeError if sample rate is zero or negative.")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input
        
        Represents internal neural signals from the brain for processing.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value. Throws RuntimeError if value is NaN.")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all internal signals");

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output
        
        Represents a motor action with type and optional parameters.
        
        Example:
            >>> action = Action(ActionType.MoveForward)
            >>> action = Action(ActionType.Custom, [0.5, 0.5])
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with just type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get human-readable action name")
        .def("clone", &Action::clone, "Create a copy of this action");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation
        
        Represents an object in the simulation world with position, type, and properties.
        
        Example:
            >>> obj = WorldObject(1.0, 2.0, WorldObjectType.Resource, 10.0, 0.5)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create world object with position, type, value, and radius")
        .def_property("x", &WorldObject::x, nullptr, "X coordinate")
        .def_property("y", &WorldObject::y, nullptr, "Y coordinate")
        .def_property("radius", &WorldObject::radius, nullptr, "Radius")
        .def_property("type", &WorldObject::type, nullptr, "Object type")
        .def_property("value", &WorldObject::value, nullptr, "Object value")
        .def_property("active", &WorldObject::active, nullptr, "Whether object is active");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state
        
        Represents the state of an agent in the simulation world.
        
        Example:
            >>> body = AgentBody()
            >>> body.x = 1.0
            >>> body.y = 2.0
        )pbdoc")
        .def(py::init<>())
        .def_property("x", &AgentBody::x, nullptr, "X coordinate")
        .def_property("y", &AgentBody::y, nullptr, "Y coordinate")
        .def_property("orientation", &AgentBody::orientation, nullptr, "Orientation in radians")
        .def_property("velocityX", &AgentBody::velocityX, nullptr, "X velocity")
        .def_property("velocityY", &AgentBody::velocityY, nullptr, "Y velocity")
        .def_property("angularVelocity", &AgentBody::angularVelocity, nullptr, "Angular velocity")
        .def_property("energy", &AgentBody::energy, nullptr, "Energy level")
        .def_property("health", &AgentBody::health, nullptr, "Health level")
        .def_property("age", &AgentBody::age, nullptr, "Age in simulation steps")
        .def_property("isMoving", &AgentBody::isMoving, nullptr, "Whether agent is moving")
        .def_property("isTurning", &AgentBody::isTurning, nullptr, "Whether agent is turning")
        .def_property("lastActionTime", &AgentBody::lastActionTime, nullptr, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset body to initial state");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world
        
        Represents the result of an action taken in the world.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success flag, and optional message")
        .def_property("reward", &ActionResult::reward, nullptr, "Reward value")
        .def_property("success", &ActionResult::success, nullptr, "Whether action succeeded")
        .def_property("message", &ActionResult::message, nullptr, "Result message");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data
        
        Contains all sensory inputs from the world as perceived by the agent.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision input")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch input")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch input")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio input")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio input")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all signals as a dictionary")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set percept timestamp");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation
        
        A simple 2D world environment for NLM simulation. Manages agent position,
        objects, and world boundaries.
        
        Example:
            >>> world = SimpleWorld()
            >>> world.configure(100, 100, 20, 20)
            >>> world.setAgentStart(50, 50)
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation by one timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command and return action result")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (within world bounds and not occupied)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy level")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy level")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get current random seed");

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class
        
        The central neural simulation brain. Manages neurons, synapses, regions, and all
        neural processing components. This is the core of the NLM system.
        
        Example:
            >>> config = Config()
            >>> config.set("neuron_count", 100)
            >>> brain = Brain(config)
            >>> brain.initialize()
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step with step number")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with step number and timestamp")
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
             "Save brain state to file. Throws RuntimeError if save fails.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Throws RuntimeError if load fails.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region. Returns region ID.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID. Returns None if region not found.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs as a list")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions as a list")
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
             "Log brain status to logger");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world
        
        Interface connecting NLM brain to world. Processes sensory input and produces
        motor actions through the world simulation.
        
        Example:
            >>> config = Config()
            >>> brain = Brain(config)
            >>> agent_brain = AgentBrain(brain)
            >>> agent_brain.initialize(world)
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain with reference to central brain")
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
             "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration object");

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

    // Create module-level constants for convenience
    py::module_ sys_module = py::module_::import("sys");
    sys_module.attr("version") = "0.1.0";
    
    m.doc() += R"\n
    Installation:
        pip install pynlm

    Quick Start:
        >>> import pynlm
        >>> config = pynlm.Config()
        >>> brain = pynlm.Brain(config)
        >>> world = pynlm.SimpleWorld()
        >>> world.configure(100, 100, 20, 20)
        >>> agent_brain = pynlm.AgentBrain(brain)
        >>> agent_brain.initialize(world)
        >>> brain.initialize()

    Best Practices:
        - Always initialize Brain with Config before use
        - Use AgentBrain for world interaction
        - Save brain state with save() method for checkpointing
        - Use NeuronId, SynapseId, etc. for safe neuron/synapse referencing
    )pbdoc";
}

} // namespace nlm
