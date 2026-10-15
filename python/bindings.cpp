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

        This enhanced Python API provides:
        - Context managers for resource management
        - Convenience methods and factory functions
        - Simulation helpers for common patterns
        - Advanced statistical analysis tools
        - Configuration presets for research and development
        - Comprehensive error handling and input validation
        - Performance monitoring and debugging tools
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");

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

    // Context manager wrapper for Brain
    py::class_<Brain, std::shared_ptr<Brain>, py::call_guard<py::gil_scoped_release>>(m, "BrainContextManager")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("__enter__", [](Brain& self) -> std::shared_ptr<Brain> {
            self.initialize();
            return std::make_shared<Brain>(self);
        })
        .def("__exit__", [](Brain& /*self*/, py::args) {
            // Brain destructor will handle cleanup
        });

    // Context manager wrapper for AgentBrain
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>, py::call_guard<py::gil_scoped_release>>(m, "AgentBrainContextManager")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("__enter__", [](AgentBrain& self, const SimpleWorld& world) -> std::shared_ptr<AgentBrain> {
            self.initialize(world);
            return std::make_shared<AgentBrain>(self);
        })
        .def("__exit__", [](AgentBrain& /*self*/, py::args) {
            // AgentBrain destructor will handle cleanup
        });

    // Context manager wrapper for SimpleWorld
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>, py::call_guard<py::gil_scoped_release>>(m, "SimpleWorldContextManager")
        .def(py::init<>())
        .def("__enter__", [](SimpleWorld& self) -> std::shared_ptr<SimpleWorld> {
            self.reset();
            return std::make_shared<SimpleWorld>(self);
        })
        .def("__exit__", [](SimpleWorld& /*self*/, py::args) {
            // SimpleWorld destructor will handle cleanup
        });

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

    // Enhanced SimpleWorld class with context manager and convenience methods
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>, py::call_guard<py::gil_scoped_release>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"), R"pbdoc(Configure world dimensions)pbdoc")
        .def("reset", &SimpleWorld::reset, R"pbdoc(Reset world to initial state)pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), R"pbdoc(Set agent starting position)pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"), R"pbdoc(Update world state by timestep seconds)pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), R"pbdoc(Apply motor command to agent)pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, R"pbdoc(Get current sensory percept for the agent)pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, R"pbdoc(Get agent body state)pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), R"pbdoc(Add object to world)pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), R"pbdoc(Remove object at position)pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), R"pbdoc(Check if position is valid (not wall))pbdoc")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"), R"pbdoc(Get object at position)pbdoc")
        .def("getWidth", &SimpleWorld::getWidth, R"pbdoc(Get world width)pbdoc")
        .def("getHeight", &SimpleWorld::getHeight, R"pbdoc(Get world height)pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, R"pbdoc(Get maximum energy)pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), R"pbdoc(Set maximum energy)pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, R"pbdoc(Get energy decay rate)pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), R"pbdoc(Set energy decay rate)pbdoc")
        .def("getEnergyGainRate", &SimpleWorld::getEnergyGainRate, R"pbdoc(Get energy gain rate)pbdoc")
        .def("setEnergyGainRate", &SimpleWorld::setEnergyGainRate, py::arg("r"), R"pbdoc(Set energy gain rate)pbdoc")
        .def("getResourceEnergyGain", &SimpleWorld::getResourceEnergyGain, R"pbdoc(Get resource energy gain)pbdoc")
        .def("setResourceEnergyGain", &SimpleWorld::setResourceEnergyGain, py::arg("gain"), R"pbdoc(Set resource energy gain)pbdoc")
        .def("getHazardEnergyLoss", &SimpleWorld::getHazardEnergyLoss, R"pbdoc(Get hazard energy loss)pbdoc")
        .def("setHazardEnergyLoss", &SimpleWorld::setHazardEnergyLoss, py::arg("loss"), R"pbdoc(Set hazard energy loss)pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, R"pbdoc(Get current simulation time)pbdoc")
        .def("setSimulationTime", &SimpleWorld::setSimulationTime, py::arg("time"), R"pbdoc(Set simulation time)pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, R"pbdoc(Get random seed)pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), R"pbdoc(Set random seed for reproducibility)pbdoc")
        .def("getCollisionDetectionRange", &SimpleWorld::getCollisionDetectionRange, R"pbdoc(Get collision detection range)pbdoc")
        .def("setCollisionDetectionRange", &SimpleWorld::setCollisionDetectionRange, py::arg("range"), R"pbdoc(Set collision detection range)pbdoc")
        .def("getInteractionRadius", &SimpleWorld::getInteractionRadius, R"pbdoc(Get interaction radius)pbdoc")
        .def("setInteractionRadius", &SimpleWorld::setInteractionRadius, py::arg("radius"), R"pbdoc(Set interaction radius)pbdoc")
        // World convenience methods
        .def("createResource", [](SimpleWorld& self, float x, float y, float value = 1.0f) {
            WorldObject obj(x, y, WorldObjectType::Resource, value);
            self.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f, R"pbdoc(Create and add a resource object)pbdoc")
        .def("createHazard", [](SimpleWorld& self, float x, float y, float damage = 10.0f) {
            WorldObject obj(x, y, WorldObjectType::Hazard, damage);
            self.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("damage") = 10.0f, R"pbdoc(Create and add a hazard object)pbdoc")
        .def("createWall", [](SimpleWorld& self, float x, float y, float radius = 0.5f) {
            WorldObject obj(x, y, WorldObjectType::Wall, 0.0f, radius);
            self.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, R"pbdoc(Create and add a wall object)pbdoc")
        .def("createMarker", [](SimpleWorld& self, float x, float y) {
            WorldObject obj(x, y, WorldObjectType::Marker, 0.0f);
            self.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), R"pbdoc(Create and add a marker object)pbdoc")
        .def("getResourceAt", [](SimpleWorld& self, float x, float y) {
            WorldObject* obj = self.getObjectAt(x, y);
            return obj && obj->type == WorldObjectType::Resource ? *obj : WorldObject();
        }, py::arg("x"), py::arg("y"), R"pbdoc(Get resource at position)pbdoc")
        .def("getHazardAt", [](SimpleWorld& self, float x, float y) {
            WorldObject* obj = self.getObjectAt(x, y);
            return obj && obj->type == WorldObjectType::Hazard ? *obj : WorldObject();
        }, py::arg("x"), py::arg("y"), R"pbdoc(Get hazard at position)pbdoc")
        .def("getWallAt", [](SimpleWorld& self, float x, float y) {
            WorldObject* obj = self.getObjectAt(x, y);
            return obj && obj->type == WorldObjectType::Wall ? *obj : WorldObject();
        }, py::arg("x"), py::arg("y"), R"pbdoc(Get wall at position)pbdoc")
        .def("getObjectCount", [](const SimpleWorld& self, WorldObjectType type) {
            size_t count = 0;
            for (const auto& obj : self.getObjects()) {
                if (obj.type == type && obj.active) {
                    count++;
                }
            }
            return count;
        }, py::arg("type"), R"pbdoc(Get count of objects of specific type)pbdoc")
        .def("getTotalObjectCount", [](const SimpleWorld& self) {
            return self.getObjectCount();
        }, R"pbdoc(Get total count of active objects)pbdoc")
        .def("runSimulationStep", [](SimpleWorld& self, double timestep, const AgentBrain& agent) {
            // Get sensory input
            const SensoryPercept& percept = self.getSensoryPercept();
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Update development
            agent.updateDevelopment(timestep);
            
            // Decode motor command
            MotorCommand action = agent.decodeMotorCommand();
            
            // Apply motor command
            self.applyMotorCommand(action, self.getSimulationTime());
            
            return action;
        }, py::arg("timestep"), py::arg("agent"), R"pbdoc(Run a single simulation step with agent)pbdoc")
        .def("runEpisode", [](SimpleWorld& self, AgentBrain& agent, size_t numSteps, double timestep = 0.1) {
            for (size_t step = 0; step < numSteps; ++step) {
                self.runSimulationStep(timestep, agent);
                self.update(timestep);
            }
        }, py::arg("agent"), py::arg("numSteps"), py::arg("timestep") = 0.1, R"pbdoc(Run a complete episode)pbdoc")
        .def("getWorldState", [](SimpleWorld& self) {
            std::ostringstream state;
            state << "World State:" << std::endl;
            state << "  Time: " << self.getSimulationTime() << std::endl;
            state << "  Dimensions: " << self.getWidth() << " x " << self.getHeight() << std::endl;
            state << "  Agent Position: (" << self.getAgentBody().x << ", " << self.getAgentBody().y << ")" << std::endl;
            state << "  Agent Energy: " << self.getAgentBody().energy << "/" << self.getMaxEnergy() << std::endl;
            state << "  Objects: " << self.getObjectCount() << std::endl;
            state << "  Vision Size: " << self.getVisionWidth() << " x " << self.getVisionHeight() << std::endl;
            return state.str();
        }, R"pbdoc(Get human-readable world state)pbdoc");

    // Enhanced Brain class with context manager and convenience methods
    py::class_<Brain, std::shared_ptr<Brain>, py::call_guard<py::gil_scoped_release>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), R"pbdoc(Create a new brain with configuration)pbdoc")
        .def("initialize", &Brain::initialize, R"pbdoc(Initialize the brain with configuration. Always call this before using the brain.)pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), R"pbdoc(Perform a simulation step)pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), R"pbdoc(Perform a simulation step with timestamp)pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), R"pbdoc(Inject sensory input into the brain)pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), R"pbdoc(Inject current into a specific neuron)pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), R"pbdoc(Inject current into all neurons of a specific type)pbdoc")
        .def("produceAction", &Brain::produceAction, R"pbdoc(Produce motor action based on neural activity)pbdoc")
        .def("reset", &Brain::reset, R"pbdoc(Reset brain state)pbdoc")
        .def("save", &Brain::save, py::arg("filepath"), R"pbdoc(Save brain state to file)pbdoc")
        .def("load", &Brain::load, py::arg("filepath"), R"pbdoc(Load brain state from file)pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", R"pbdoc(Add a new neural region)pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"), py::return_value_policy::reference_internal, R"pbdoc(Get a region by ID)pbdoc")
        .def("getRegionCount", &Brain::getRegionCount, R"pbdoc(Get the number of regions)pbdoc")
        .def("getRegionIds", &Brain::getRegionIds, R"pbdoc(Get all region IDs)pbdoc")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal, R"pbdoc(Get all regions)pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, R"pbdoc(Get total neuron count across all regions)pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, R"pbdoc(Get total synapse count across all regions)pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, R"pbdoc(Get count of active neurons)pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, R"pbdoc(Get count of currently firing neurons)pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, R"pbdoc(Get average firing rate across all neurons)pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, R"pbdoc(Get excitation/inhibition balance ratio)pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, R"pbdoc(Get total spike count)pbdoc")
        .def("getTotalSpikeCount", [](const Brain& self) { return self.getTotalSpikeCount(); }, R"pbdoc(Get total spike count)pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, R"pbdoc(Get current developmental stage)pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), R"pbdoc(Set developmental stage)pbdoc")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, R"pbdoc(Get the configuration)pbdoc")
        .def("logStatus", &Brain::logStatus, R"pbdoc(Log brain status)pbdoc")
        // Statistical analysis methods
        .def("getFiringStatistics", [](const Brain& self) {
            size_t total = self.getTotalNeuronCount();
            size_t firing = self.getFiringNeuronCount();
            size_t active = self.getActiveNeuronCount();
            size_t spikes = self.getTotalSpikeCount();
            float avgRate = self.getAverageFiringRate();
            float eir = self.getExcitationInhibitionRatio();
            
            std::map<std::string, double> stats;
            stats["total_neurons"] = total;
            stats["firing_neurons"] = firing;
            stats["active_neurons"] = active;
            stats["total_spikes"] = spikes;
            stats["average_firing_rate"] = avgRate;
            stats["e_i_ratio"] = eir;
            stats["firing_percentage"] = total > 0 ? (firing * 100.0 / total) : 0.0;
            stats["activity_percentage"] = total > 0 ? (active * 100.0 / total) : 0.0;
            stats["spikes_per_neuron"] = total > 0 ? (spikes * 1.0 / total) : 0.0;
            
            return stats;
        }, R"pbdoc(Compute comprehensive firing statistics)pbdoc")
        .def("getSpikeRateDistribution", [](const Brain& self, size_t numBins = 10) {
            // Note: This is a simplified implementation
            // In a real implementation, you would need access to spike timing data
            size_t totalSpikes = self.getTotalSpikeCount();
            float avgRate = self.getAverageFiringRate();
            
            std::vector<float> distribution;
            for (size_t i = 0; i < numBins; ++i) {
                float rate = (i + 0.5) * (avgRate * 2.0 / numBins); // Assuming reasonable range
                distribution.push_back(rate);
            }
            
            return distribution;
        }, py::arg("numBins") = 10, R"pbdoc(Get spike rate distribution)pbdoc")
        .def("compareBrains", [](const Brain& self, const Brain& other, std::string metric = "firing_rate") {
            std::map<std::string, double> comparison;
            
            if (metric == "firing_rate") {
                comparison["brain1_firing_rate"] = self.getAverageFiringRate();
                comparison["brain2_firing_rate"] = other.getAverageFiringRate();
                comparison["difference"] = comparison["brain2_firing_rate"] - comparison["brain1_firing_rate"];
                comparison["percent_change"] = comparison["brain1_firing_rate"] > 0 ? 
                    (comparison["difference"] / comparison["brain1_firing_rate"] * 100.0) : 0.0;
            } else if (metric == "total_spikes") {
                comparison["brain1_total_spikes"] = self.getTotalSpikeCount();
                comparison["brain2_total_spikes"] = other.getTotalSpikeCount();
                comparison["difference"] = comparison["brain2_total_spikes"] - comparison["brain1_total_spikes"];
                comparison["percent_change"] = comparison["brain1_total_spikes"] > 0 ? 
                    (comparison["difference"] / comparison["brain1_total_spikes"] * 100.0) : 0.0;
            } else if (metric == "neuron_count") {
                comparison["brain1_neuron_count"] = self.getTotalNeuronCount();
                comparison["brain2_neuron_count"] = other.getTotalNeuronCount();
                comparison["difference"] = comparison["brain2_neuron_count"] - comparison["brain1_neuron_count"];
                comparison["percent_change"] = comparison["brain1_neuron_count"] > 0 ? 
                    (comparison["difference"] / comparison["brain1_neuron_count"] * 100.0) : 0.0;
            }
            
            return comparison;
        }, py::arg("other"), py::arg("metric") = "firing_rate", R"pbdoc(Compare two brains on a specific metric)pbdoc")
        // Convenience methods
        .def("runSteps", [](Brain& self, size_t numSteps) {
            for (size_t step = 0; step < numSteps; ++step) {
                self.step(static_cast<nlm::SimulationStep>(step));
            }
        }, py::arg("numSteps"), R"pbdoc(Run multiple simulation steps quickly)pbdoc")
        .def("runStepsWithTime", [](Brain& self, size_t numSteps, double timeStep) {
            for (size_t step = 0; step < numSteps; ++step) {
                self.step(static_cast<nlm::SimulationStep>(step), 
                         static_cast<nlm::Timestamp>(step * timeStep));
            }
        }, py::arg("numSteps"), py::arg("timeStep") = 0.1, R"pbdoc(Run multiple simulation steps with time tracking)pbdoc")
        .def("getBrainSummary", [](const Brain& self) {
            std::ostringstream summary;
            summary << "Brain State Summary:" << std::endl;
            summary << "  Total Neurons: " << self.getTotalNeuronCount() << std::endl;
            summary << "  Total Synapses: " << self.getTotalSynapseCount() << std::endl;
            summary << "  Firing Neurons: " << self.getFiringNeuronCount() << std::endl;
            summary << "  Total Spikes: " << self.getTotalSpikeCount() << std::endl;
            summary << "  Average Firing Rate: " << self.getAverageFiringRate() << " Hz" << std::endl;
            summary << "  E/I Ratio: " << self.getExcitationInhibitionRatio() << std::endl;
            summary << "  Developmental Stage: " << static_cast<int>(self.getDevelopmentalStage()) << std::endl;
            return summary.str();
        }, R"pbdoc(Get a human-readable summary of brain state)pbdoc");

    // Enhanced AgentBrain class with context manager and convenience methods
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>, py::call_guard<py::gil_scoped_release>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), R"pbdoc(Create a new agent brain interface)pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"), R"pbdoc(Initialize with world)pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, R"pbdoc(Get expected sensory input size)pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, R"pbdoc(Get expected motor output size)pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), R"pbdoc(Process sensory percept and inject into brain)pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, R"pbdoc(Decode brain motor activity into motor command)pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), R"pbdoc(Apply reward-based neuromodulation)pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), R"pbdoc(Update development system)pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, R"pbdoc(Get current developmental stage)pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, R"pbdoc(Get current neuromodulation level)pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, R"pbdoc(Get curiosity level)pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, R"pbdoc(Get novelty level)pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError, R"pbdoc(Get prediction error)pbdoc")
        .def("reset", &AgentBrain::reset, R"pbdoc(Reset agent for new episode)pbdoc")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, R"pbdoc(Get the underlying brain)pbdoc")
        // Subsystem enable/disable
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"), R"pbdoc(Enable/disable reward modulation)pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"), R"pbdoc(Enable/disable structural plasticity)pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"), R"pbdoc(Enable/disable development)pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"), R"pbdoc(Enable/disable curiosity)pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, R"pbdoc(Check if reward modulation is enabled)pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, R"pbdoc(Check if structural plasticity is enabled)pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, R"pbdoc(Check if development is enabled)pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, R"pbdoc(Check if curiosity is enabled)pbdoc")
        // Agent convenience methods
        .def("runEpisode", [](AgentBrain& self, SimpleWorld& world, size_t numSteps) {
            for (size_t step = 0; step < numSteps; ++step) {
                // Get sensory input
                const SensoryPercept& percept = world.getSensoryPercept();
                self.processSensoryInput(percept);
                
                // Update development
                self.updateDevelopment(0.1);
                
                // Decode motor command
                MotorCommand action = self.decodeMotorCommand();
                
                // Apply to world
                world.applyMotorCommand(action, world.getSimulationTime());
            }
        }, py::arg("world"), py::arg("numSteps"), R"pbdoc(Run a complete episode)pbdoc")
        .def("getAgentStatus", [](const AgentBrain& self, const SimpleWorld& world) {
            std::ostringstream status;
            status << "Agent Status:" << std::endl;
            status << "  Curiosity Level: " << self.getCuriosityLevel() << std::endl;
            status << "  Novelty Level: " << self.getNoveltyLevel() << std::endl;
            status << "  Prediction Error: " << self.getPredictionError() << std::endl;
            status << "  Neuromodulation Level: " << self.getNeuromodulationLevel() << std::endl;
            status << "  Developmental Stage: " << static_cast<int>(self.getDevelopmentalStage()) << std::endl;
            status << "  Reward Mod Enabled: " << (self.isRewardModulationEnabled() ? "Yes" : "No") << std::endl;
            status << "  Structural Plasticity Enabled: " << (self.isStructuralPlasticityEnabled() ? "Yes" : "No") << std::endl;
            status << "  Development Enabled: " << (self.isDevelopmentEnabled() ? "Yes" : "No") << std::endl;
            status << "  Curiosity Enabled: " << (self.isCuriosityEnabled() ? "Yes" : "No") << std::endl;
            status << "  Agent Position: (" << world.getAgentBody().x << ", " << world.getAgentBody().y << ")" << std::endl;
            status << "  Agent Energy: " << world.getAgentBody().energy << std::endl;
            return status.str();
        }, py::arg("world"), R"pbdoc(Get human-readable agent status)pbdoc");

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

    // Enhanced global factory functions and convenience methods
    m.def("createResearchConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Research-optimized settings
        config->set("brain.neuron_count", 2000, nlm::ConfigSource::Runtime);
        config->set("brain.synapse_density", 0.15, nlm::ConfigSource::Runtime);
        config->set("brain.connection_probability", 0.08, nlm::ConfigSource::Runtime);
        config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Runtime);
        config->set("plasticity.stdp.learning_rate", 0.002, nlm::ConfigSource::Runtime);
        config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Runtime);
        config->set("plasticity.structural.enable", true, nlm::ConfigSource::Runtime);
        config->set("neuromod.dopamine.scale", 1.5, nlm::ConfigSource::Runtime);
        config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Runtime);
        config->set("neuromod.novelty.enable", true, nlm::ConfigSource::Runtime);
        config->set("development.enable", true, nlm::ConfigSource::Runtime);
        config->set("development.initial_neurons", 500, nlm::ConfigSource::Runtime);
        return config;
    }, "Create a research-oriented configuration with enhanced plasticity and neuromodulation");

    m.def("createDevelopmentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Development-optimized settings for training/learning
        config->set("brain.neuron_count", 1500, nlm::ConfigSource::Runtime);
        config->set("brain.synapse_density", 0.12, nlm::ConfigSource::Runtime);
        config->set("brain.connection_probability", 0.06, nlm::ConfigSource::Runtime);
        config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Runtime);
        config->set("plasticity.stdp.learning_rate", 0.001, nlm::ConfigSource::Runtime);
        config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Runtime);
        config->set("plasticity.structural.enable", true, nlm::ConfigSource::Runtime);
        config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Runtime);
        config->set("neuromod.novelty.enable", true, nlm::ConfigSource::Runtime);
        config->set("development.enable", true, nlm::ConfigSource::Runtime);
        config->set("development.maturation_rate", 0.8, nlm::ConfigSource::Runtime);
        return config;
    }, "Create a development-oriented configuration with focus on learning and maturation");

    m.def("createTestingConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Testing-optimized settings for quick verification
        config->set("brain.neuron_count", 100, nlm::ConfigSource::Runtime);
        config->set("brain.synapse_density", 0.1, nlm::ConfigSource::Runtime);
        config->set("brain.connection_probability", 0.05, nlm::ConfigSource::Runtime);
        config->set("plasticity.stdp.enable", false, nlm::ConfigSource::Runtime); // Disable for stability
        config->set("neuromod.curiosity.enable", false, nlm::ConfigSource::Runtime); // Disable for predictability
        config->set("neuromod.novelty.enable", false, nlm::ConfigSource::Runtime); // Disable for predictability
        config->set("development.enable", false, nlm::ConfigSource::Runtime); // Disable for stability
        return config;
    }, "Create a testing configuration with minimal complexity for stable unit testing");

    m.def("createProductionConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Production-optimized settings for stable simulation
        config->set("brain.neuron_count", 1000, nlm::ConfigSource::Runtime);
        config->set("brain.synapse_density", 0.1, nlm::ConfigSource::Runtime);
        config->set("brain.connection_probability", 0.05, nlm::ConfigSource::Runtime);
        config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Runtime);
        config->set("plasticity.stdp.learning_rate", 0.0005, nlm::ConfigSource::Runtime);
        config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Runtime);
        config->set("neuromod.curiosity.enable", false, nlm::ConfigSource::Runtime); // Disable for predictability
        config->set("development.enable", false, nlm::ConfigSource::Runtime); // Disable for stability
        return config;
    }, "Create a production-oriented configuration with stable, predictable behavior");

    m.def("createReadyToRunEnvironment", []() {
        std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> env;
        
        auto config = createResearchConfig();
        auto brain = createBrain(config);
        brain->initialize();
        
        auto world = createSimpleWorld();
        world->configure(20, 20, 8, 8);
        world->setAgentStart(10.0, 10.0);
        
        auto agent = createAgentBrain(brain);
        agent->initialize(*world);
        
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        env = std::make_tuple(brain, world, agent);
        return env;
    }, "Create a ready-to-run environment with brain, world, and agent configured for research");

    m.def("run_simple_brain_example", [](size_t numSteps = 100) {
        auto config = createDefaultConfig();
        auto brain = createBrain(config);
        brain->initialize();
        
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(static_cast<nlm::SimulationStep>(step));
            
            if (step % 20 == 0) {
                std::cout << "Step " << step << ": " << brain->getFiringNeuronCount() 
                         << " neurons firing, " << brain->getTotalSpikeCount() << " total spikes" << std::endl;
            }
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("numSteps") = 100, "Run a simple brain simulation and return total spike count");

    m.def("run_agent_simulation", [](size_t numSteps = 500, const std::string& configType = "research") {
        std::shared_ptr<Config> config;
        
        if (configType == "research") {
            config = createResearchConfig();
        } else if (configType == "development") {
            config = createDevelopmentConfig();
        } else if (configType == "testing") {
            config = createTestingConfig();
        } else if (configType == "production") {
            config = createProductionConfig();
        } else {
            config = createDefaultConfig();
        }
        
        auto brain = createBrain(config);
        brain->initialize();
        
        auto world = createSimpleWorld();
        world->configure(20, 20, 8, 8);
        world->setAgentStart(10.0, 10.0);
        
        auto agent = createAgentBrain(brain);
        agent->initialize(*world);
        
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        size_t totalSpikes = 0;
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1);
            
            const SensoryPercept& percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(static_cast<nlm::SimulationStep>(step));
            
            MotorCommand action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            totalSpikes = brain->getTotalSpikeCount();
            
            if (step % 100 == 0) {
                std::cout << "Step " << step << ": " << brain->getFiringNeuronCount() 
                         << " firing neurons, " << agent->getCuriosityLevel() << " curiosity, "
                         << agent->getNoveltyLevel() << " novelty" << std::endl;
            }
        }
        
        return std::make_tuple(totalSpikes, brain->getAverageFiringRate(), agent->getCuriosityLevel());
    }, py::arg("numSteps") = 500, py::arg("configType") = "research", "Run a complete agent simulation and return results");

    m.def("benchmark_brain_performance", [](const Brain& brain, size_t numSteps = 1000) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < numSteps; ++step) {
            brain.step(static_cast<nlm::SimulationStep>(step));
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        double stepsPerSecond = numSteps / (duration.count() / 1000.0);
        
        std::map<std::string, double> results;
        results["total_steps"] = numSteps;
        results["total_time_ms"] = duration.count();
        results["steps_per_second"] = stepsPerSecond;
        results["spikes_per_step"] = brain.getTotalSpikeCount() / (numSteps > 0 ? numSteps : 1);
        
        return results;
    }, py::arg("brain"), py::arg("numSteps") = 1000, "Benchmark brain performance");

    m.def("validate_config", [](const Config& config) {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        // Check for required keys
        if (!config.has("brain.neuron_count")) {
            warnings.push_back("No neuron_count specified, using default");
        }
        
        if (!config.has("brain.synapse_density")) {
            warnings.push_back("No synapse_density specified, using default");
        }
        
        if (config.has("brain.neuron_count")) {
            auto neuronCountOpt = config.get<int64_t>("brain.neuron_count");
            if (neuronCountOpt && (*neuronCountOpt <= 0)) {
                errors.push_back("neuron_count must be positive");
            }
        }
        
        if (config.has("brain.synapse_density")) {
            auto densityOpt = config.get<double>("brain.synapse_density");
            if (densityOpt && (*densityOpt < 0.0 || *densityOpt > 1.0)) {
                errors.push_back("synapse_density must be between 0.0 and 1.0");
            }
        }
        
        return std::make_pair(errors, warnings);
    }, py::arg("config"), "Validate configuration and return errors/warnings");

    m.def("save_config_to_file", [](const Config& config, const std::string& filepath) {
        return config.saveToFile(filepath);
    }, py::arg("config"), py::arg("filepath"), "Save configuration to file");

    m.def("load_config_from_file", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }, py::arg("filepath"), "Load configuration from file");

    // Advanced research tools
    m.def("create_neural_network_visualizer", [](const Brain& brain) {
        // Create a simple visualization summary
        std::ostringstream viz;
        viz << "Neural Network Visualization:" << std::endl;
        viz << "===========================" << std::endl;
        viz << "Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
        viz << "Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
        viz << "Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
        viz << "Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
        viz << "Average Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
        viz << "E/I Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
        viz << "Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
        viz << "" << std::endl;
        viz << "Regions:" << std::endl;
        
        auto regionIds = brain.getRegionIds();
        for (size_t i = 0; i < regionIds.size(); ++i) {
            auto* region = brain.getRegion(regionIds[i]);
            if (region) {
                viz << "  - " << region->getName() << ": "
                    << region->getNeuronCount() << " neurons" << std::endl;
            }
        }
        
        return viz.str();
    }, py::arg("brain"), "Create a simple text-based visualization of the brain's structure");

    m.def("analyze_brain_activity", [](const Brain& brain, size_t windowSize = 100) {
        // Analyze recent activity patterns
        std::map<std::string, double> analysis;
        
        analysis["total_spikes"] = brain.getTotalSpikeCount();
        analysis["firing_neuron_percentage"] = 
            brain.getTotalNeuronCount() > 0 ? 
            (brain.getFiringNeuronCount() * 100.0 / brain.getTotalNeuronCount()) : 0.0;
        analysis["average_firing_rate"] = brain.getAverageFiringRate();
        analysis["e_i_ratio"] = brain.getExcitationInhibitionRatio();
        
        // Determine activity level
        double firingRate = brain.getAverageFiringRate();
        if (firingRate < 1.0) {
            analysis["activity_level"] = "low";
        } else if (firingRate < 5.0) {
            analysis["activity_level"] = "moderate";
        } else if (firingRate < 10.0) {
            analysis["activity_level"] = "high";
        } else {
            analysis["activity_level"] = "very_high";
        }
        
        // Determine stability
        float eir = brain.getExcitationInhibitionRatio();
        if (eir > 0.8 && eir < 1.2) {
            analysis["stability"] = "stable";
        } else if (eir > 0.5 && eir < 2.0) {
            analysis["stability"] = "moderately_stable";
        } else {
            analysis["stability"] = "unstable";
        }
        
        return analysis;
    }, py::arg("brain"), py::arg("windowSize") = 100, "Analyze brain activity patterns");

    m.def("create_brain_snapshot", [](const Brain& brain, const std::string& description = "") {
        // Create a snapshot of brain state for later comparison
        std::map<std::string, double> snapshot;
        
        snapshot["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        snapshot["description"] = description;
        snapshot["total_neurons"] = brain.getTotalNeuronCount();
        snapshot["total_synapses"] = brain.getTotalSynapseCount();
        snapshot["active_neurons"] = brain.getActiveNeuronCount();
        snapshot["firing_neurons"] = brain.getFiringNeuronCount();
        snapshot["total_spikes"] = brain.getTotalSpikeCount();
        snapshot["average_firing_rate"] = brain.getAverageFiringRate();
        snapshot["e_i_ratio"] = brain.getExcitationInhibitionRatio();
        snapshot["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
        
        return snapshot;
    }, py::arg("brain"), py::arg("description") = "", "Create a snapshot of brain state");

    m.def("compare_brain_snapshots", [](const std::map<std::string, double>& snapshot1, 
                                        const std::map<std::string, double>& snapshot2) {
        std::map<std::string, double> comparison;
        
        for (const auto& [key, value1] : snapshot1) {
            auto it = snapshot2.find(key);
            if (it != snapshot2.end()) {
                double value2 = it->second;
                comparison[key + "_difference"] = value2 - value1;
                
                if (value1 != 0.0) {
                    comparison[key + "_percent_change"] = (value2 - value1) / value1 * 100.0;
                } else {
                    comparison[key + "_percent_change"] = value2 != 0.0 ? 100.0 : 0.0;
                }
            }
        }
        
        return comparison;
    }, py::arg("snapshot1"), py::arg("snapshot2"), "Compare two brain snapshots");

    m.def("get_version", []() {
        return "pynlm 0.1.0 - Enhanced Python Bindings for NLM";
    }, "Get version information");

    // Error handling and validation
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p)
                std::rethrow_exception(p);
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

} // namespace nlm
