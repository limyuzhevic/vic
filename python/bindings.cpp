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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Custom Python exceptions for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::logic_error>(m, "LogicError");

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

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        // Loading/Saving
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        
        // Configuration management - Core APIs
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", py::overload_cast<const std::string&>(&Config::get), py::arg("key"),
             "Get configuration value as string (generic getter)")
        .def("set", py::overload_cast<const std::string&, const std::string&>(&Config::set), 
             py::arg("key"), py::arg("value"),
             "Set configuration value (string)")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        
        // Convenience methods for Brain configuration
        .def("configureBrain", [](Config& self, uint64_t neuronCount = 1000,
                                 float synapseDensity = 0.1f,
                                 float connectionProbability = 0.05f) {
            self.set("brain.neuron_count", std::to_string(neuronCount));
            self.set("brain.synapse_density", std::to_string(synapseDensity));
            self.set("brain.connection_probability", std::to_string(connectionProbability));
        }, py::arg("neuronCount") = 1000,
           py::arg("synapseDensity") = 0.1f,
           py::arg("connectionProbability") = 0.05f,
           "Configure brain-specific parameters with sensible defaults")
        
        // Convenience getters for common configuration values
        .def("getNeuronCount", [](const Config& self) -> int64_t {
            return self.get("brain.neuron_count", 1000);
        }, "Get the number of neurons configured")
        
        .def("getSynapseDensity", [](const Config& self) -> float {
            return self.get("brain.synapse_density", 0.1f);
        }, "Get the synapse density configured")
        
        .def("getConnectionProbability", [](const Config& self) -> float {
            return self.get("brain.connection_probability", 0.05f);
        }, "Get the connection probability configured")
        
        // Validation and utilities
        .def("validate", [](const Config& self) -> bool {
            // Basic validation - check that required keys exist
            return self.has("brain.neuron_count");
        }, "Validate the configuration (checks for required keys)")
        
        .def("merge", [](Config& self, const Config& other) {
            // Merge another config into this one
            auto keys = other.getKeys();
            for (const auto& key : keys) {
                self.set(key, other.get(key));
            }
        }, py::arg("other"),
             "Merge another configuration into this one (overwrites values)")
        
        // Summary and display
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        
        // Python-friendly string conversion
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
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
        // Python-friendly constructors
        .def("__init__", [](WorldObject& self, float x, float y, WorldObjectType type) {
            new (&self) WorldObject(x, y, type);
        }, py::arg("x"), py::arg("y"), py::arg("type"),
             "Create WorldObject with position and type only")
        .def("__init__", [](WorldObject& self, float x, float y, WorldObjectType type, float value) {
            new (&self) WorldObject(x, y, type, value);
        }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value"),
             "Create WorldObject with position, type, and value")
        .def("__init__", [](WorldObject& self, float x, float y, WorldObjectType type, float value, float radius) {
            new (&self) WorldObject(x, y, type, value, radius);
        }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value"), py::arg("radius"),
             "Create WorldObject with all parameters")
        
        // Convenience methods for common object types
        .def("isResource", [](const WorldObject& self) {
            return self.type == WorldObjectType::Resource;
        }, "Check if object is a resource")
        
        .def("isHazard", [](const WorldObject& self) {
            return self.type == WorldObjectType::Hazard;
        }, "Check if object is a hazard")
        
        .def("isWall", [](const WorldObject& self) {
            return self.type == WorldObjectType::Wall;
        }, "Check if object is a wall")
        
        .def("isEmpty", [](const WorldObject& self) {
            return self.type == WorldObjectType::Empty;
        }, "Check if object is empty (placeholder)")
        
        .def("getTypeName", [](const WorldObject& self) {
            switch (self.type) {
                case WorldObjectType::Empty: return "Empty";
                case WorldObjectType::Resource: return "Resource";
                case WorldObjectType::Hazard: return "Hazard";
                case WorldObjectType::Wall: return "Wall";
                case WorldObjectType::Marker: return "Marker";
                default: return "Unknown";
            }
        }, "Get human-readable type name")
        
        .def("clone", &WorldObject::clone,
             "Create a copy of this world object")
        
        // Python-friendly string conversion
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: " + obj.getTypeName() + " at (" + std::to_string(obj.x) + ", " + 
                   std::to_string(obj.y) + ") value=" + std::to_string(obj.value) + ">";
        });

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

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        
        // World configuration
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
             "Configure world dimensions and sensory input sizes")
        
        // World state management
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation")
        
        .def("step", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation (alias for update())")
        
        // Agent interaction
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to the agent")
        
        // State access
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        
        // World objects management
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object")
        
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove world object at position")
        
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not occupied by obstacle)")
        
        // World properties
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum agent energy")
        
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        
        // Random seed management
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        
        // Convenience methods for common world operations
        .def("teleportAgent", [](SimpleWorld& self, float x, float y) {
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"),
             "Teleport agent to position (resets direction)")
        
        .def("addResource", [](SimpleWorld& self, float x, float y, float value = 1.0f) {
            self.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType::Resource, value));
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f,
             "Add a resource object at position")
        
        .def("addHazard", [](SimpleWorld& self, float x, float y, float radius = 0.5f) {
            self.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType::Hazard, 0.0f, radius));
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f,
             "Add a hazard object at position")
        
        .def("simulateEpisode", [](SimpleWorld& self, int steps, float dt = 0.1f) {
            for (int step = 0; step < steps; ++step) {
                self.update(dt);
            }
        }, py::arg("steps"), py::arg("dt") = 0.1f,
             "Run world simulation for given number of steps")
        
        .def("getStatus", [](const SimpleWorld& self) {
            py::dict status;
            status["width"] = self.getWidth();
            status["height"] = self.getHeight();
            status["time"] = self.getSimulationTime();
            status["agent_x"] = self.getAgentBody().x;
            status["agent_y"] = self.getAgentBody().y;
            return status;
        }, "Get world status as a dictionary")
        
        // Python-friendly string conversion
        .def("__repr__", [](const SimpleWorld& w) {
            return "<SimpleWorld: " + std::to_string(w.getWidth()) + "x" + 
                   std::to_string(w.getHeight()) + " @ " + std::to_string(w.getSimulationTime()) + ">";
        });

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        
        // Initialization and lifecycle
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("isInitialized", &Brain::isInitialized,
             "Check if the brain has been initialized")
        
        // Simulation management - Core APIs
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("stepWithTime", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        
        // Simulation utilities
        .def("run", [](Brain& self, int steps, float dt = 0.1f) {
            for (int step = 0; step < steps; ++step) {
                self.step(step);
            }
        }, py::arg("steps"), py::arg("dt") = 0.1f,
             "Run the simulation for a given number of steps")
        
        .def("runTimed", [](Brain& self, int steps, float dt = 0.1f) {
            for (int step = 0; step < steps; ++step) {
                self.step(step, static_cast<Timestamp>(step * dt));
            }
        }, py::arg("steps"), py::arg("dt") = 0.1f,
             "Run the simulation with timestamps")
        
        .def("runEpisode", [](Brain& self, const std::vector<Timestamp>& times) {
            for (size_t i = 0; i < times.size(); ++i) {
                self.step(static_cast<SimulationStep>(i), times[i]);
            }
        }, py::arg("times"),
             "Run simulation with custom timestamps")
        
        // Input and output
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        
        // Neural current injection
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        
        .def("injectToAll", [](Brain& self, NeuronType type, float current) {
            self.injectCurrentToNeurons(type, current);
        }, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        
        .def("injectRandom", [](Brain& self, float current, float probability = 0.5f) {
            // This would need to be implemented in Brain.cpp
            // For now, provide a placeholder
            NLM_LOG_WARN("injectRandom is not yet implemented in Brain.cpp");
        }, py::arg("current"), py::arg("probability") = 0.5f,
             "Inject random current to neurons (placeholder)")
        
        // Action production
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        
        // State management
        .def("reset", &Brain::reset,
             "Reset brain state")
        
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        
        .def("checkpoint", [](Brain& self, const std::string& filepath) {
            return self.save(filepath);
        }, py::arg("filepath"),
             "Shortcut for save() (checkpoint)")
        
        // Region management
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        
        // Statistics and monitoring - Core APIs
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        
        // Neural population statistics
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
        
        // Developmental stages
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        
        .def("advanceDevelopment", [](Brain& self) {
            auto stage = self.getDevelopmentalStage();
            if (stage < DevelopmentalStage::Aging) {
                self.setDevelopmentalStage(static_cast<DevelopmentalStage>(static_cast<int>(stage) + 1));
            }
        }, "Advance to next developmental stage")
        
        // Configuration access
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        
        // Status and diagnostics
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        
        .def("getDiagnosticInfo", [](const Brain& self) {
            py::dict info;
            info["neurons"] = self.getTotalNeuronCount();
            info["synapses"] = self.getTotalSynapseCount();
            info["firing"] = self.getFiringNeuronCount();
            info["spikes"] = self.getTotalSpikeCount();
            info["rate"] = self.getAverageFiringRate();
            info["stage"] = static_cast<int>(self.getDevelopmentalStage());
            return info;
        }, "Get diagnostic information as a dictionary")
        
        // Python-friendly string conversion
        .def("__repr__", [](const Brain& b) {
            return "<Brain: neurons=" + std::to_string(b.getTotalNeuronCount()) +
                   ", synapses=" + std::to_string(b.getTotalSynapseCount()) + ")";
        });

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        
        // Initialization and lifecycle
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        
        // Sensory processing
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        
        .def("getSensoryData", [](AgentBrain& self) {
            auto percept = self.getSensoryPercept();
            py::dict sensoryData;
            
            auto vision = percept.getVision();
            sensoryData["vision"] = std::vector<float>(
                vision.getData().begin(), vision.getData().end());
            sensoryData["vision_width"] = vision.getWidth();
            sensoryData["vision_height"] = vision.getHeight();
            
            return sensoryData;
        }, "Get sensory data as dictionary (convenience method)")
        
        // Motor decoding
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        
        .def("getActionName", [](const AgentBrain& self) {
            auto action = self.decodeMotorCommand();
            return action.getName();
        }, "Get human-readable name of current motor command")
        
        // Neuromodulation
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        
        .def("applySimpleReward", [](AgentBrain& self, float reward) {
            self.applyRewardModulation(reward, 0.0f);
        }, py::arg("reward"),
             "Apply reward modulation (predicted reward = 0)")
        
        // Development
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        
        .def("advanceDevelopment", [](AgentBrain& self) {
            auto stage = self.getDevelopmentalStage();
            if (stage < DevelopmentalStage::Aging) {
                self.setDevelopmentalStage(static_cast<DevelopmentalStage>(static_cast<int>(stage) + 1));
            }
        }, "Advance to next developmental stage")
        
        // Subsystem management - Core APIs
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        
        // Subsystem status - Core APIs
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        
        // Statistics and monitoring - Core APIs
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        
        // Convenience methods for common operations
        .def("resetEpisode", [](AgentBrain& self) {
            self.reset();
            if (self.getBrain()) {
                self.getBrain()->reset();
            }
        }, "Reset both agent and brain for new episode")
        
        .def("getEpisodeStats", [](const AgentBrain& self) {
            py::dict stats;
            stats["curiosity"] = self.getCuriosityLevel();
            stats["novelty"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["neuromodulation"] = self.getNeuromodulationLevel();
            stats["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get comprehensive episode statistics")
        
        .def("configureForExploration", [](AgentBrain& self) {
            self.enableCuriosity(true);
            self.enableRewardModulation(true);
            self.enableDevelopment(true);
        }, "Enable all exploration subsystems")
        
        .def("configureForExploitation", [](AgentBrain& self) {
            self.enableCuriosity(false);
            self.enableRewardModulation(true);
            self.enableDevelopment(true);
        }, "Enable exploitation-focused subsystems")
        
        // Convenience simulation methods
        .def("runStep", [](AgentBrain& self, const SensoryPercept& percept) {
            self.processSensoryInput(percept);
            self.getBrain()->step(0);
        }, py::arg("percept"),
             "Run a single simulation step (process sensory input and step brain)")
        
        // Python-friendly string conversion
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain: curiosity=" + std::to_string(agent.getCuriosityLevel()) + ", " +
                   "novelty=" + std::to_string(agent.getNoveltyLevel()) + ", " +
                   "stage=" + std::to_string(static_cast<int>(agent.getDevelopmentalStage())) + ">";
        });

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
