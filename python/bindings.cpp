#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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

// Helper template function to reduce duplication in ID class bindings
template<typename IDType>
void bindId(py::class_<IDType>& cls, const std::string& docName) {
    cls.def(py::init<>())
       .def(py::init<uint64_t>(), py::arg("value"))
       .def_readwrite("value", &IDType::value)
       .def("index", &IDType::index)
       .def("__eq__", &IDType::operator==)
       .def("__ne__", &IDType::operator!=)
       .def("__hash__", [](const IDType& id) { return std::hash<uint64_t>{}(id.value); })
       .def("__repr__", [](const IDType& id) {
           return "<" + docName + ": " + std::to_string(id.value) + ">";
       });
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    bindId(py::class_<NeuronId>(m, "NeuronId"), "NeuronId");
    bindId(py::class_<SynapseId>(m, "SynapseId"), "SynapseId");
    bindId(py::class_<RegionId>(m, "RegionId"), "RegionId");
    bindId(py::class_<PopulationId>(m, "PopulationId"), "PopulationId");

    // Neuron type enumeration
    py::enum_<NeuronType>(m, "NeuronType", 
        "Neuron type enumeration.\n\n"
        "Defines different types of neurons with distinct functions:\n"
        "- Excitatory: Increases firing probability of postsynaptic neurons\n"
        "- Inhibitory: Decreases firing probability of postsynaptic neurons\n"
        "- Modulatory: Releases neuromodulators affecting synaptic strength\n"
        "- Sensory: Processes external sensory inputs\n"
        "- Motor: Produces motor commands for action execution\n"
        "- Internal: Regulates internal homeostasis and states")
        .value("Excitatory", NeuronType::Excitatory, "Increases postsynaptic firing probability")
        .value("Inhibitory", NeuronType::Inhibitory, "Decreases postsynaptic firing probability")
        .value("Modulatory", NeuronType::Modulatory, "Releases neuromodulators affecting synaptic strength")
        .value("Sensory", NeuronType::Sensory, "Processes external sensory inputs")
        .value("Motor", NeuronType::Motor, "Produces motor commands for action execution")
        .value("Internal", NeuronType::Internal, "Regulates internal homeostasis and states")
        .export_values();

    // Synapse type enumeration  
    py::enum_<SynapseType>(m, "SynapseType",
        "Synapse type enumeration.\n\n"
        "Defines different mechanisms of synaptic transmission:\n"
        "- Excitatory: Releases excitatory neurotransmitters\n"
        "- Inhibitory: Releases inhibitory neurotransmitters\n"
        "- Modulatory: Releases neuromodulators\n"
        "- Electrical: Direct electrical coupling via gap junctions\n"
        "- GapJunction: Electrical coupling through connexins")
        .value("Excitatory", SynapseType::Excitatory, "Releases excitatory neurotransmitters")
        .value("Inhibitory", SynapseType::Inhibitory, "Releases inhibitory neurotransmitters")
        .value("Modulatory", SynapseType::Modulatory, "Releases neuromodulators")
        .value("Electrical", SynapseType::Electrical, "Direct electrical coupling")
        .value("GapJunction", SynapseType::GapJunction, "Electrical coupling through connexins")
        .export_values();

    // Developmental stage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage",
        "Developmental stage enumeration.\n\n"
        "Represents the brain's developmental phase:\n"
        "- Initial: Initial learning phase, rapid synaptogenesis\n"
        "- CriticalPeriod: Enhanced plasticity for skill acquisition\n"
        "- Maturation: Stabilization of neural circuits\n"
        "- Adult: Mature, stable neural processing\n"
        "- Aging: Age-related changes in neural function")
        .value("Initial", DevelopmentalStage::Initial, "Initial learning phase, rapid synaptogenesis")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Enhanced plasticity for skill acquisition")
        .value("Maturation", DevelopmentalStage::Maturation, "Stabilization of neural circuits")
        .value("Adult", DevelopmentalStage::Adult, "Mature, stable neural processing")
        .value("Aging", DevelopmentalStage::Aging, "Age-related changes in neural function")
        .export_values();

    // Neuron firing state enumeration
    py::enum_<FiringState>(m, "FiringState",
        "Neuron firing state enumeration.\n\n"
        "Represents the current state of a neuron:\n"
        "- Resting: Below threshold, not firing\n"
        "- Active: Currently firing action potentials\n"
        "- Refractory: Just fired, cannot fire again\n"
        "- Inhibited: Actively suppressed from firing")
        .value("Resting", FiringState::Resting, "Below threshold, not firing")
        .value("Active", FiringState::Active, "Currently firing action potentials")
        .value("Refractory", FiringState::Refractory, "Just fired, cannot fire again")
        .value("Inhibited", FiringState::Inhibited, "Actively suppressed from firing")
        .export_values();

    // Motor command enumeration
    py::enum_<MotorCommand>(m, "MotorCommand",
        "Low-level motor command enumeration.\n\n"
        "Basic motor commands for agent movement and interaction:\n"
        "- MoveForward: Move forward in the direction of orientation\n"
        "- MoveBackward: Move backward\n"
        "- TurnLeft: Rotate counterclockwise\n"
        "- TurnRight: Rotate clockwise\n"
        "- LookLeft: Rotate gaze left\n"
        "- LookRight: Rotate gaze right\n"
        "- Interact: Interact with nearby objects\n"
        "- Wait: Do nothing (maintain current state)")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward in direction of orientation")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Rotate counterclockwise")
        .value("TurnRight", MotorCommand::TurnRight, "Rotate clockwise")
        .value("LookLeft", MotorCommand::LookLeft, "Rotate gaze left")
        .value("LookRight", MotorCommand::LookRight, "Rotate gaze right")
        .value("Interact", MotorCommand::Interact, "Interact with nearby objects")
        .value("Wait", MotorCommand::Wait, "Do nothing (maintain current state)")
        .export_values();

    // World object type enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType",
        "World object type enumeration.\n\n"
        "Types of objects that can exist in the simulated world:\n"
        "- Empty: No object (air/empty space)\n"
        "- Resource: Valuable resource for consumption\n"
        "- Hazard: Dangerous object that damages agent\n"
        "- Wall: Impassable boundary\n"
        "- Marker: Waypoint or goal location")
        .value("Empty", WorldObjectType::Empty, "No object (air/empty space)")
        .value("Resource", WorldObjectType::Resource, "Valuable resource for consumption")
        .value("Hazard", WorldObjectType::Hazard, "Dangerous object that damages agent")
        .value("Wall", WorldObjectType::Wall, "Impassable boundary")
        .value("Marker", WorldObjectType::Marker, "Waypoint or goal location")
        .export_values();

    // Action type enumeration
    py::enum_<ActionType>(m, "ActionType",
        "Action type enumeration.\n\n"
        "High-level actions that agents can perform:\n"
        "- MoveForward: Move forward in the direction of orientation\n"
        "- MoveBackward: Move backward\n"
        "- MoveLeft: Strafe left\n"
        "- MoveRight: Strafe right\n"
        "- TurnLeft: Rotate counterclockwise\n"
        "- TurnRight: Rotate clockwise\n"
        "- Look: Look in current direction\n"
        "- LookUp: Look upward\n"
        "- LookDown: Look downward\n"
        "- Interact: Interact with nearby objects\n"
        "- Eat: Consume food resource\n"
        "- Drink: Consume water resource\n"
        "- Rest: Stop moving and recover\n"
        "- Wait: Do nothing (idle state)\n"
        "- Custom: Custom action defined by user")
        .value("MoveForward", ActionType::MoveForward, "Move forward in direction of orientation")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward")
        .value("MoveLeft", ActionType::MoveLeft, "Strafe left")
        .value("MoveRight", ActionType::MoveRight, "Strafe right")
        .value("TurnLeft", ActionType::TurnLeft, "Rotate counterclockwise")
        .value("TurnRight", ActionType::TurnRight, "Rotate clockwise")
        .value("Look", ActionType::Look, "Look in current direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with nearby objects")
        .value("Eat", ActionType::Eat, "Consume food resource")
        .value("Drink", ActionType::Drink, "Consume water resource")
        .value("Rest", ActionType::Rest, "Stop moving and recover")
        .value("Wait", ActionType::Wait, "Do nothing (idle state)")
        .value("Custom", ActionType::Custom, "Custom action defined by user")
        .export_values();

    py::class_<Config>(m, "Config", 
        "Configuration class for NLM system.\n\n"
        "Provides key-value storage with type conversion support.\n"
        "Supports loading from JSON files or command-line arguments.")
        .def(py::init<>(), "Create empty configuration")
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            try {
                return self.loadFromFile(filepath);
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
                return false;
            }
        }, py::arg("filepath"),
             "Load configuration from a JSON file. Raises RuntimeError on failure.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                return self.loadFromArgs(argc, argv);
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
                return false;
            }
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments. Raises RuntimeError on failure.")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            try {
                return self.saveToFile(filepath);
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
                return false;
            }
        }, py::arg("filepath"),
             "Save configuration to a JSON file. Raises RuntimeError on failure.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", [](const Config& self) {
            try {
                return self.summary();
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
                return std::string("Config: error");
            }
        }, "Get a summary string of the configuration")
        .def("getFloat", [](Config& self, const std::string& key, float defaultValue) {
            try {
                auto value = self.get<float>(key);
                return value ? value.value() : defaultValue;
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
                return defaultValue;
            }
        }, py::arg("key"), py::arg("defaultValue") = 0.0f,
             "Get a float value. Raises RuntimeError if access fails.")
        .def("getInt", [](Config& self, const std::string& key, int defaultValue) {
            try {
                auto value = self.get<int>(key);
                return value ? value.value() : defaultValue;
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
                return defaultValue;
            }
        }, py::arg("key"), py::arg("defaultValue") = 0,
             "Get an int value. Raises RuntimeError if access fails.")
        .def("getString", [](Config& self, const std::string& key, const std::string& defaultValue) {
            try {
                auto value = self.get<std::string>(key);
                return value ? value.value() : defaultValue;
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
                return defaultValue;
            }
        }, py::arg("key"), py::arg("defaultValue") = "",
             "Get a string value. Raises RuntimeError if access fails.")
        .def("__repr__", [](const Config& cfg) {
            try {
                return "<Config: " + cfg.summary() + ">";
            } catch (const std::exception& e) {
                PyErr_SetString(PyExc_RuntimeError, e.what());
                return std::string("<Config: error>");
            }
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

    // Agent body state
    py::class_<AgentBody>(m, "AgentBody",
        "Agent body state representing physical properties.\n\n"
        "Contains position, velocity, orientation, energy, health, and age.\n"
        "Tracks movement, turning, and agent lifecycle.")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X coordinate position")
        .def_readwrite("y", &AgentBody::y, "Y coordinate position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation angle (radians)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity component")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity component")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity (radians/sec)")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level (0-1)")
        .def_readwrite("health", &AgentBody::health, "Health level (0-1)")
        .def_readwrite("age", &AgentBody::age, "Agent age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset agent to initial state")
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get speed (magnitude of velocity)")
        .def("getDirection", [](const AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get direction in radians (arctan2(velocityY, velocityX))")
        .def("__repr__", [](const AgentBody& body) {
            std::ostringstream oss;
            oss << "<AgentBody x=" << body.x << " y=" << body.y 
                << " energy=" << body.energy << " health=" << body.health 
                << " age=" << body.age << ">";
            return oss.str();
        });

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

    py::class_<SimpleWorld>(m, "SimpleWorld", 
        "Simple 2D world for NLM simulation.\n\n"
        "Manages agent movement, collisions, and sensory perception.\n"
        "Provides basic physics simulation with energy decay and world boundaries.")
        .def(py::init<>(), "Create empty world")
        .def(py::init<float, float, size_t, size_t>(), 
             py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 84, py::arg("visionHeight") = 84,
             "Create world with specified dimensions and agent vision")
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world simulation")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command to agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::copy, "Get sensory percept (copy)")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::copy, "Get agent body state (copy)")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("getObjectAt", [](SimpleWorld& self, float x, float y) {
            for (const auto& obj : self.getObjects()) {
                if (obj.active && std::abs(obj.x - x) < 0.1f && std::abs(obj.y - y) < 0.1f) {
                    return obj;
                }
            }
            return WorldObject{}; // Return empty object if not found
        }, py::arg("x"), py::arg("y"), "Get object at world coordinates")
        .def("getDistanceToAgent", [](SimpleWorld& self, float x, float y) {
            float agentX = self.getAgentBody().x;
            float agentY = self.getAgentBody().y;
            return std::sqrt(std::pow(x - agentX, 2) + std::pow(y - agentY, 2));
        }, py::arg("x"), py::arg("y"), "Get distance from agent to position")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")

    py::class_<Brain>(m, "Brain", 
        "Central neural simulation brain class.\n\n"
        "Implements spiking neural network with plasticity, neuromodulation, and learning.\n"
        "Provides full control over neural development, simulation, and behavior.")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
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
             py::return_value_policy::copy,
             "Get a region by ID (returns copy)")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::copy,
             "Get all regions (returns copy)")
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
             py::return_value_policy::copy,
             "Get configuration (returns copy)")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::copy,
             "Get spike system (returns copy)")
        .def("getPlasticity", &Brain::getPlasticity,
             py::return_value_policy::copy,
             "Get plasticity system (returns copy)")
        .def("getNeuromodulator", &Brain::getNeuromodulator,
             py::return_value_policy::copy,
             "Get neuromodulator system (returns copy)")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::copy,
             "Get prediction system (returns copy)")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("enableLearning", [](Brain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all learning mechanisms")
        .def("disableLearning", [](Brain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all learning mechanisms")
        .def("getLearningStatus", [](const Brain& self) {
            return std::make_tuple(
                self.isRewardModulationEnabled(),
                self.isStructuralPlasticityEnabled(),
                self.isDevelopmentEnabled(),
                self.isCuriosityEnabled()
            );
        }, "Get current learning system status")
        .def("getSystemStatus", [](const Brain& self) {
            std::ostringstream status;
            status << "Brain System Status:" << std::endl;
            status << "  Neurons: " << self.getTotalNeuronCount() << std::endl;
            status << "  Synapses: " << self.getTotalSynapseCount() << std::endl;
            status << "  Active: " << self.getActiveNeuronCount() << std::endl;
            status << "  Firing: " << self.getFiringNeuronCount() << std::endl;
            status << "  Developmental Stage: " << static_cast<int>(self.getDevelopmentalStage()) << std::endl;
            status << "  Reward Modulation: " << (self.isRewardModulationEnabled() ? "enabled" : "disabled") << std::endl;
            status << "  Structural Plasticity: " << (self.isStructuralPlasticityEnabled() ? "enabled" : "disabled") << std::endl;
            status << "  Development: " << (self.isDevelopmentEnabled() ? "enabled" : "disabled") << std::endl;
            status << "  Curiosity: " << (self.isCuriosityEnabled() ? "enabled" : "disabled") << std::endl;
            return status.str();
        }, "Get comprehensive system status")
        .def("__repr__", [](const Brain& brain) {
            std::ostringstream oss;
            oss << "<Brain neurons=" << brain.getTotalNeuronCount()
                << " synapses=" << brain.getTotalSynapseCount()
                << " stage=" << static_cast<int>(brain.getDevelopmentalStage())
                << ">";
            return oss.str();
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
