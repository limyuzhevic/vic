// Standard includes for pybind11 bindings
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <memory>
#include <vector>

// Project includes
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"

namespace py = pybind11;

namespace nlm {
PYBIND11_MODULE(pynlm, m) {
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

    // ActionResult error codes and detailed results
    py::enum_<ActionResultCode>(m, "ActionResultCode", R"pbdoc(Action result error codes)pbdoc")
        .value("Success", ActionResultCode::Success)
        .value("InvalidAction", ActionResultCode::InvalidAction)
        .value("OutOfRange", ActionResultCode::OutOfRange)
        .value("Collision", ActionResultCode::Collision)
        .value("NoEnergy", ActionResultCode::NoEnergy)
        .value("InternalError", ActionResultCode::InternalError)
        .value("Unauthorized", ActionResultCode::Unauthorized)
        .value("InvalidState", ActionResultCode::InvalidState)
        .value("Timeout", ActionResultCode::Timeout)
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

    // Add improved error handling and documentation
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)
        
        Configuration management system for NLM parameters including:
        - brain.neuron_count: Number of neurons
        - brain.synapse_density: Connection density
        - plasticity.stdp.enable: STDP learning rule
        - neuromodulation.curiosity.enable: Curiosity-driven exploration
        
        Usage:
        >>> config = pynlm.createDefaultConfig()
        >>> config.set("brain.neuron_count", 2000)
        >>> brain = pynlm.createBrain(config)
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file path. Returns True on success.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments. Returns True on success.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file path. Returns True on success.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns True if key is found.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list of strings.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("summary", &Config::summary,
             "Get a formatted summary string of the current configuration.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // Enhanced SensoryInput with better documentation
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input data)
        
        Abstract base class representing sensory input from environment or internal state.
        Contains timestamp and type information with data accessors.
        
        Subclasses:
        - Vision: Visual input from camera/sensors
        - Audio: Audio input from microphones
        - InternalSignals: Internal neural signals and motivations
        )pbdoc")
        .def("getType", &SensoryInput::getType,
             "Get the sensory input type. Returns string identifying the input type.")
        .def("getData", &SensoryInput::getData,
             "Get the raw data as a vector of floats.")
        .def("getDimensions", &SensoryInput::getDimensions,
             "Get the dimensionality of the data. Returns tuple of dimensions.")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             "Get the timestamp of when this input was generated.")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp for this sensory input.")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput: type=" + input.getType() + 
                   " timestamp=" + std::to_string(input.getTimestamp()) + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class with better documentation
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)
        
        Visual input data with spatial dimensions and color channels.
        
        Attributes:
        - width: Image width in pixels
        - height: Image height in pixels  
        - channels: Number of color channels (1=grayscale, 3=RGB)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set visual data as a vector of normalized pixel values (0-1).")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels.")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels.")
        .def("getChannels", &Vision::getChannels, "Get number of color channels.")
        .def("__repr__", [](const Vision& vision) {
            return "<Vision: " + std::to_string(vision.getWidth()) + "x" +
                   std::to_string(vision.getHeight()) + " " +
                   std::to_string(vision.getChannels()) + "ch>";
        });

    // Enhanced Audio class with better documentation  
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)
        
        Audio input data with sampling rate and duration.
        
        Attributes:
        - sampleRate: Sampling frequency in Hz
        - numSamples: Number of audio samples
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data as a vector of normalized samples (0-1).")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the sampling rate in Hz.")
        .def("getSampleRate", &Audio::getSampleRate, "Get the sampling rate in Hz.")
        .def("getNumSamples", &Audio::getNumSamples, "Get the number of audio samples.")
        .def("__repr__", [](const Audio& audio) {
            return "<Audio: " + std::to_string(audio.getSampleRate()) + "Hz, " +
                   std::to_string(audio.getNumSamples()) + " samples>";
        });

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

    // Enhanced Action with better documentation
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)
        
        Motor action that the brain can produce to affect the world.
        
        Attributes:
        - type: Action type (MoveForward, TurnLeft, etc.)
        - parameters: Action-specific parameters (e.g., speed, angle)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get the action type.")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type.")
        .def("getParameters", &Action::getParameters, "Get action parameters.")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters.")
        .def("getName", &Action::getName, "Get the descriptive name of this action.")
        .def("clone", &Action::clone, "Create a copy of this action.")
        .def("__repr__", [](const Action& action) {
            std::string params = "";
            if (!action.getParameters().empty()) {
                params = " params=" + std::to_string(action.getParameters().size()) + " values";
            }
            return "<Action: " + action.getName() + params + ">";
        });

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

    // Enhanced AgentBody with better documentation
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)
        
        Physical properties and state of the agent in the world.
        
        Attributes:
        - x, y: Position coordinates
        - orientation: Heading angle in radians
        - velocityX, velocityY: Linear velocity components
        - angularVelocity: Rotational velocity
        - energy: Energy level (0-1)
        - health: Health level (0-1)
        - age: Developmental age in simulation steps
        - isMoving: Whether agent is currently moving
        - isTurning: Whether agent is currently turning
        - lastActionTime: Time of last action taken
        )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "Get/set x position.")
        .def_readwrite("y", &AgentBody::y, "Get/set y position.")
        .def_readwrite("orientation", &AgentBody::orientation, "Get/set orientation (radians).")
        .def_readwrite("velocityX", &AgentBody::velocityX, "Get/set x velocity.")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Get/set y velocity.")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Get/set angular velocity.")
        .def_readwrite("energy", &AgentBody::energy, "Get/set energy level (0-1).")
        .def_readwrite("health", &AgentBody::health, "Get/set health level (0-1).")
        .def_readwrite("age", &AgentBody::age, "Get/set agent age in steps.")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Get/set moving state.")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Get/set turning state.")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Get/set last action time.")
        .def("reset", &AgentBody::reset, "Reset agent to initial state.")
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: pos(" + std::to_string(body.x) + "," + 
                   std::to_string(body.y) + ") orient(" + 
                   std::to_string(body.orientation) + ") energy(" +
                   std::to_string(body.energy) + ")>";
        });

    // Update ActionResult to include error codes and detailed information
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def(py::init<float, bool, std::string, ActionResultCode>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             py::arg("errorCode"))
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def_readwrite("errorCode", &ActionResult::errorCode)
        .def("getErrorDescription", &ActionResult::getErrorDescription,
             "Get detailed error description for failure codes")
        .def("isSuccess", &ActionResult::isSuccess,
             "Check if action was successful")
        .def("isFailure", &ActionResult::isFailure,
             "Check if action failed")
        .def("__repr__", [](const ActionResult& result) {
            std::string status = result.isSuccess() ? "SUCCESS" : "FAILURE";
            return "<ActionResult: " + status + " reward=" + std::to_string(result.reward) +
                   " errorCode=" + std::to_string(static_cast<int>(result.errorCode)) + ">";
        });

    // Prediction system interface
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system for sensory prediction and error computation)pbdoc")
        .def("predictNextState", &PredictionSystem::predictNextState,
             py::arg("currentState"),
             "Make prediction for next timestep")
        .def("updatePredictions", &PredictionSystem::updatePredictions,
             py::arg("predicted"), py::arg("actual"),
             "Update predictions based on actual observation")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             "Get prediction error")
        .def("getConfidence", &PredictionSystem::getConfidence,
             "Get prediction confidence")
        .def("getErrorHistory", &PredictionSystem::getErrorHistory,
             "Get prediction error history")
        .def("clearHistory", &PredictionSystem::clearHistory,
             "Clear prediction error history")
        .def("train", &PredictionSystem::train,
             py::arg("observation"),
             "Train prediction model")
        .def("__repr__", [](const PredictionSystem& pred) {
            return "<PredictionSystem: error=" + std::to_string(pred.getPredictionError()) +
                   " confidence=" + std::to_string(pred.getConfidence()) + ">";
        });

    // Neuromodulator base class
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName,
             "Get modulator name")
        .def("getLevel", &Neuromodulator::getLevel,
             "Get current concentration/level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"),
             "Set the concentration/level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor,
             "Get neuromodulatory effect on plasticity")
        .def("update", &Neuromodulator::update, py::arg("dt"),
             "Update neuromodulator state")
        .def("__repr__", [](const Neuromodulator& mod) {
            return "<" + std::string(mod.getName()) + ": level=" + 
                   std::to_string(mod.getLevel()) + ">";
        });

    // Specific neuromodulators
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"),
             "Signal a reward")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError,
             py::arg("error"), "Signal reward prediction error");

    py::class_<Acetylcholine, Neuromodulator>(m, "Acetylcholine", R"pbdoc(Acetylcholine neuromodulator)pbdoc")
        .def(py::init<>());

    py::class_<Norepinephrine, Neuromodulator>(m, "Norepinephrine", R"pbdoc(Norepinephrine neuromodulator)pbdoc")
        .def(py::init<>());

    py::class_<Serotonin, Neuromodulator>(m, "Serotonin", R"pbdoc(Serotonin neuromodulator)pbdoc")
        .def(py::init<>());

    // Enhanced WorldObject with better documentation
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)
        
        Physical object in the simulation world.
        
        Attributes:
        - x, y: Position coordinates
        - radius: Object radius (size)
        - type: Object type (Resource, Hazard, Wall, etc.)
        - value: Object value (positive for resources, negative for hazards)
        - active: Whether object is currently active
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "Get/set x position.")
        .def_readwrite("y", &WorldObject::y, "Get/set y position.")
        .def_readwrite("radius", &WorldObject::radius, "Get/set object radius.")
        .def_readwrite("type", &WorldObject::type, "Get/set object type.")
        .def_readwrite("value", &WorldObject::value, "Get/set object value.")
        .def_readwrite("active", &WorldObject::active, "Get/set object active state.")
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: " + std::to_string(obj.x) + "," + 
                   std::to_string(obj.y) + " type=" + std::to_string(static_cast<int>(obj.type)) +
                   " value=" + std::to_string(obj.value) + " active=" +
                   std::to_string(obj.active) + ">";
        });

    // Enhanced SimpleWorld with better documentation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)
        
        Simulation environment with physics simulation and object management.
        
        Key features:
        - 2D grid world with configurable dimensions
        - Agent position and body state tracking
        - Physics simulation with energy and collision detection
        - Sensory perception and motor command execution
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and sensor properties.")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state.")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent's starting position.")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world physics simulation by timestep seconds.")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world and return action result.")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept available to agent.")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state.")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object to the simulation.")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove world object at position.")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not colliding, within bounds).")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width in units.")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height in units.")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy.")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy.")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, 
             "Get energy decay rate per timestep.")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate per timestep.")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, 
             "Get current simulation time.")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations.")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get current random seed.")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + " time=" +
                   std::to_string(world.getSimulationTime()) + ">";
        });

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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
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
        // Advanced system access methods
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get the spike system for neural dynamics")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get the spike-timing dependent plasticity (STDP) system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get the Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get the structural plasticity system")
        // Memory system access methods
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get the working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get the episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get the associative memory system")
        // Prediction and cognition system access methods
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get the prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get the neural planner for action planning")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get the concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get the attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get the development system")
        // Neuromodulation system access methods
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get the dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get the curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get the novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get the prediction error signal neuromodulator")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get the random number generator")
        // Pythonic interface methods
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain.getTotalSynapseCount()) + 
                   " synapses>";
        })
        // Context manager support for Brain lifecycle
        .def("__enter__", [](std::shared_ptr<Brain> self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<Brain> self, py::args) {
            self->reset();
        });

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Create utility functions for common simulation patterns
    m.def("run_agent_simulation", [](std::shared_ptr<Config> config, 
                                   std::shared_ptr<SimpleWorld> world, 
                                   std::shared_ptr<Brain> brain,
                                   std::shared_ptr<AgentBrain> agent,
                                   int num_steps) {
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("config"), py::arg("world"), py::arg("brain"), 
           py::arg("agent"), py::arg("num_steps"),
           "Run a complete agent simulation with brain and world");

    m.def("create_default_simulation", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        auto agent = std::make_shared<AgentBrain>(brain);
        return std::make_tuple(config, world, brain, agent);
    }, "Create a complete default simulation setup");

    // Enhanced configuration validation and validation tools
    m.def("validate_config", [](const Config& config) {
        std::vector<std::string> errors;
        
        // Check required keys
        auto keys = config.getKeys();
        bool hasNeuronCount = config.has("brain.neuron_count");
        bool hasSynapseDensity = config.has("brain.synapse_density");
        
        if (!hasNeuronCount) {
            errors.push_back("Missing required key: brain.neuron_count");
        } else {
            int neuronCount = config.get("brain.neuron_count", 1000);
            if (neuronCount <= 0 || neuronCount > 1000000) {
                errors.push_back("brain.neuron_count must be between 1 and 1,000,000");
            }
        }
        
        if (!hasSynapseDensity) {
            errors.push_back("Missing recommended key: brain.synapse_density");
        } else {
            float density = config.get("brain.synapse_density", 0.1);
            if (density < 0.0f || density > 1.0f) {
                errors.push_back("brain.synapse_density must be between 0.0 and 1.0");
            }
        }
        
        // Validate plasticity settings
        if (config.has("plasticity.stdp.enable")) {
            bool stdpEnabled = config.get("plasticity.stdp.enable", true);
            if (stdpEnabled && !config.has("plasticity.stdp.learning_rate")) {
                errors.push_back("plasticity.stdp.enable requires plasticity.stdp.learning_rate");
            }
        }
        
        if (config.has("neuromod.dopamine.scale")) {
            float dopamineScale = config.get("neuromod.dopamine.scale", 1.0);
            if (dopamineScale < 0.0f) {
                errors.push_back("neuromod.dopamine.scale must be non-negative");
            }
        }
        
        return errors;
    }, py::arg("config"),
        "Validate configuration and return list of error messages. Empty list means valid.");

    // Performance monitoring tools
    m.def("benchmark_brain", [](const std::shared_ptr<Brain>& brain, 
                              int num_steps = 100,
                              bool measure_memory = true,
                              bool measure_cpu = true) {
        auto spikeSystem = brain->getSpikeSystem();
        
        // Measure performance metrics
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < num_steps; ++step) {
            brain->step(step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::map<std::string, double> metrics;
        metrics["total_steps"] = num_steps;
        metrics["total_time_ms"] = duration.count();
        metrics["time_per_step_ms"] = duration.count() / num_steps;
        metrics["steps_per_second"] = num_steps * 1000.0 / duration.count();
        metrics["neurons_active"] = brain->getActiveNeuronCount();
        metrics["neurons_firing"] = brain->getFiringNeuronCount();
        metrics["total_spikes"] = brain->getTotalSpikeCount();
        metrics["spike_rate"] = brain->getAverageFiringRate();
        
        if (measure_memory) {
            // Memory metrics (approximate)
            metrics["memory_footprint_kb"] = brain->getTotalNeuronCount() * 10; // rough estimate
        }
        
        return metrics;
    }, py::arg("brain"), py::arg("num_steps") = 100,
        py::arg("measure_memory") = true, py::arg("measure_cpu") = true,
        "Benchmark brain performance and return metrics.");

    m.def("profile_simulation", [](const std::shared_ptr<Brain>& brain,
                                   const std::shared_ptr<SimpleWorld>& world,
                                   int num_steps = 100) {
        std::map<std::string, double> profile;
        
        // Simulate and collect performance data
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < num_steps; ++step) {
            auto stepStart = std::chrono::high_resolution_clock::now();
            
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            brain->receiveSensoryInput(percept);
            brain->step(step);
            
            auto stepEnd = std::chrono::high_resolution_clock::now();
            auto stepDuration = std::chrono::duration_cast<std::chrono::microseconds>(stepEnd - stepStart).count();
            profile["step_" + std::to_string(step) + "_time_us"] = stepDuration;
        }
        
        auto totalEnd = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(totalEnd - startTime).count();
        
        profile["total_time_ms"] = totalDuration;
        profile["total_steps"] = num_steps;
        profile["avg_step_time_us"] = totalDuration * 1000.0 / num_steps;
        
        return profile;
    }, py::arg("brain"), py::arg("world"), py::arg("num_steps") = 100,
        "Profile simulation performance and return timing data.");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
