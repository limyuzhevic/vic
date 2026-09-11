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
        .def("set_from_dict", &Config::setFromDict,
             py::arg("key"), py::arg("value"), py::arg("source").none_or_default(),
             "Set configuration value from dictionary (Python-like convenience)")
        .def("get_bool", &Config::getBool, py::arg("key"),
             "Get boolean value for key (type-safe)")
        .def("get_int", &Config::getInt, py::arg("key"),
             "Get integer value for key (type-safe)")
        .def("get_double", &Config::getDouble, py::arg("key"),
             "Get double value for key (type-safe)")
        .def("get_string", &Config::getString, py::arg("key"),
             "Get string value for key (type-safe)")
        .def("set_and_validate", &Config::setAndValidate,
             py::arg("key"), py::arg("value"), py::arg("source").none_or_default(),
             "Set value with validation")
        .def("validate", &Config::validate, py::arg("errors"),
             "Validate configuration and return error list")
        .def("is_valid", &Config::isValid,
             "Check if configuration is valid")
        .def("load_from_dict", &Config::loadFromDict, py::arg("config_dict"),
             "Load configuration from Python-like dictionary")
        .def("to_json", &Config::toJson,
             "Export configuration to JSON string")
        .def("from_json", &Config::fromJson, py::arg("json_str"),
             "Load configuration from JSON string")
        .def("set_batch", &Config::setBatch, py::arg("values"), py::arg("source").none_or_default(),
             "Set multiple values from dictionary")
        .def("get_batch", &Config::getBatch, py::arg("keys"),
             "Get multiple values as dictionary")
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

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("configureWorld", &SimpleWorld::configureWorld,
             "Create configuration builder for method chaining")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("moveAgent", &SimpleWorld::moveAgent, py::arg("dx"), py::arg("dy"),
             "Move agent by delta")
        .def("turnAgent", &SimpleWorld::turnAgent, py::arg("angleDelta"),
             "Turn agent by angle delta")
        .def("moveTo", &SimpleWorld::moveTo, py::arg("x"), py::arg("y"), py::arg("maxSteps").none_or_default(),
             "Move agent to target position")
        .def("exploreArea", &SimpleWorld::exploreArea, py::arg("radius"), py::arg("searchPattern").none_or_default(),
             "Explore area with search pattern")
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("interactWithObject", &SimpleWorld::interactWithObject, py::arg("x"), py::arg("y"),
             "Interact with object at position")
        .def("collectResource", &SimpleWorld::collectResource, py::arg("x"), py::arg("y"),
             "Collect resource at position")
        .def("avoidHazard", &SimpleWorld::avoidHazard, py::arg("x"), py::arg("y"),
             "Avoid hazard at position")
        .def("useMarker", &SimpleWorld::useMarker, py::arg("x"), py::arg("y"),
             "Use marker at position")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("addResource", &SimpleWorld::addResource, py::arg("x"), py::arg("y"), py::arg("value").none_or_default(),
             "Add resource object")
        .def("addHazard", &SimpleWorld::addHazard, py::arg("x"), py::arg("y"), py::arg("damage").none_or_default(),
             "Add hazard object")
        .def("addWall", &SimpleWorld::addWall, py::arg("x"), py::arg("y"), py::arg("radius").none_or_default(),
             "Add wall object")
        .def("addMarker", &SimpleWorld::addMarker, py::arg("x"), py::arg("y"), py::arg("value").none_or_default(),
             "Add marker object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal)
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Vision and perception helpers
        .def("generateVision", &SimpleWorld::generateVision)
        .def("getVisionValue", &SimpleWorld::getVisionValue, py::arg("x"), py::arg("y"),
             "Get vision value at position")
        .def("isVisionInRange", &SimpleWorld::isVisionInRange, py::arg("x"), py::arg("y"),
             "Check if vision range includes position")
        // Agent state management
        .def("resetAgent", &SimpleWorld::resetAgent, py::arg("x").none_or_default(), py::arg("y").none_or_default(), py::arg("energy").none_or_default(),
             "Reset agent state")
        .def("setAgentEnergy", &SimpleWorld::setAgentEnergy, py::arg("energy"),
             "Set agent energy")
        .def("setAgentHealth", &SimpleWorld::setAgentHealth, py::arg("health"),
             "Set agent health")
        .def("setAgentAge", &SimpleWorld::setAgentAge, py::arg("age"),
             "Set agent age")
        // Object manipulation
        .def("addBatchObjects", &SimpleWorld::addBatchObjects, py::arg("objects"),
             "Add multiple objects")
        .def("removeBatchObjects", &SimpleWorld::removeBatchObjects, py::arg("positions"),
             "Remove multiple objects")
        .def("getObjectsInArea", &SimpleWorld::getObjectsInArea, py::arg("centerX"), py::arg("centerY"), py::arg("radius"),
             "Get objects in area")
        // Pathfinding and navigation
        .def("findPath", &SimpleWorld::findPath, py::arg("startX"), py::arg("startY"), py::arg("targetX"), py::arg("targetY"),
             "Find path to target")
        .def("exploreNeighborhood", &SimpleWorld::exploreNeighborhood, py::arg("centerX"), py::arg("centerY"), py::arg("radius"),
             "Explore neighborhood")
        // Statistical analysis
        .def("computeVisionStatistics", &SimpleWorld::computeVisionStatistics,
             "Compute vision statistics")
        .def("computeObjectDistribution", &SimpleWorld::computeObjectDistribution,
             "Compute object distribution")
        // Visualization and debugging helpers
        .def("generateWorldMap", &SimpleWorld::generateWorldMap,
             "Generate world map string representation")
        .def("generateObjectReport", &SimpleWorld::generateObjectReport,
             "Generate object report")
        .def("generateAgentState", &SimpleWorld::generateAgentState,
             "Generate agent state string representation")
        // Experimental and research mode helpers
        .def("enableExperimentalMode", &SimpleWorld::enableExperimentalMode,
             "Enable experimental mode")
        .def("enableResearchMode", &SimpleWorld::enableResearchMode,
             "Enable research mode")
        .def("disableAllObjects", &SimpleWorld::disableAllObjects,
             "Disable all objects")
        // Debugging and profiling methods
        .def("startProfiler", &SimpleWorld::startProfiler,
             "Start performance profiler")
        .def("stopProfiler", &SimpleWorld::stopProfiler,
             "Stop performance profiler")
        .def("isProfilerActive", &SimpleWorld::isProfilerActive,
             "Check if profiler is active")
        .def("getProfilerReport", &SimpleWorld::getProfilerReport,
             "Get profiler report")
        .def("setDebugLevel", &SimpleWorld::setDebugLevel,
             py::arg("level"),
             "Set debug level")
        .def("getDebugLevel", &SimpleWorld::getDebugLevel,
             "Get current debug level")
        .def("enableLoggingToFile", &SimpleWorld::enableLoggingToFile,
             py::arg("filepath"),
             "Enable file logging")
        .def("disableLoggingToFile", &SimpleWorld::disableLoggingToFile,
             "Disable file logging")
        .def("isLoggingToFile", &SimpleWorld::isLoggingToFile,
             "Check if logging to file is enabled")
        // Performance monitoring
        .def("getSimulationSpeed", &SimpleWorld::getSimulationSpeed,
             "Get simulation speed")
        .def("getAverageUpdateTime", &SimpleWorld::getAverageUpdateTime,
             "Get average update time")
        .def("getActiveObjectCount", &SimpleWorld::getActiveObjectCount,
             "Get number of active objects")
        // State management
        .def("saveState", &SimpleWorld::saveState,
             py::arg("filepath"),
             "Save world state")
        .def("loadState", &SimpleWorld::loadState,
             py::arg("filepath"),
             "Load world state")
        // Configuration validation
        .def("validateConfiguration", &SimpleWorld::validateConfiguration,
             py::arg("errors"),
             "Validate configuration")
        .def("isConfigurationValid", &SimpleWorld::isConfigurationValid,
             "Check if configuration is valid")
        // Export and visualization helpers
        .def("exportToJson", &SimpleWorld::exportToJson,
             "Export world to JSON")
        .def("importFromJson", &SimpleWorld::importFromJson,
             py::arg("jsonStr"),
             "Import world from JSON")
        .def("exportToImage", &SimpleWorld::exportToImage,
             py::arg("filepath"), py::arg("scale").none_or_default(),
             "Export world to image")
        // Simulation state management
        .def("pauseSimulation", &SimpleWorld::pauseSimulation,
             "Pause simulation")
        .def("resumeSimulation", &SimpleWorld::resumeSimulation,
             "Resume simulation")
        .def("isSimulationPaused", &SimpleWorld::isSimulationPaused,
             "Check if simulation is paused")
        // Checkpoint and recovery
        .def("createCheckpoint", &SimpleWorld::createCheckpoint,
             py::arg("prefix").none_or_default(),
             "Create checkpoint")
        .def("restoreFromCheckpoint", &SimpleWorld::restoreFromCheckpoint,
             py::arg("filepath"),
             "Restore from checkpoint")
        .def("getAvailableCheckpoints", &SimpleWorld::getAvailableCheckpoints,
             "Get available checkpoints")
        // Existing methods (for backward compatibility)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("initializeWithConfig", &Brain::initializeWithConfig,
             py::arg("config_dict"),
             "Initialize brain with configuration dictionary")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("runSimulationSteps", &Brain::runSimulationSteps,
             py::arg("steps"), py::arg("timestep").none_or_default(),
             "Run simulation for specified number of steps")
        .def("runSimulationTime", &Brain::runSimulationTime,
             py::arg("duration"), py::arg("timestep").none_or_default(),
             "Run simulation until time limit")
        .def("runEpisode", &Brain::runEpisode,
             py::arg("maxDuration").none_or_default(), py::arg("timestep").none_or_default(),
             "Run full episode simulation")
        .def("createSession", &Brain::createSession,
             py::arg("timestep"), py::arg("startStep").none_or_default(),
             "Create a simulation session for context management")
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
        // Statistical analysis methods
        .def("getStatisticalMean", &Brain::getStatisticalMean,
             py::arg("data"),
             "Get statistical mean of data")
        .def("getStatisticalStdDev", &Brain::getStatisticalStdDev,
             py::arg("data"), py::arg("mean"),
             "Get statistical standard deviation")
        .def("getStatisticalVariance", &Brain::getStatisticalVariance,
             py::arg("data"),
             "Get statistical variance")
        .def("getStatisticalBounds", &Brain::getStatisticalBounds,
             py::arg("data"),
             "Get statistical bounds (min, max)")
        // Neural activity analysis
        .def("getNeutronActivityPattern", &Brain::getNeutronActivityPattern,
             py::arg("regionId"),
             "Get neural activity pattern for region")
        .def("getRegionExcitabilityRatio", &Brain::getRegionExcitabilityRatio,
             py::arg("regionId"),
             "Get excitation/inhibition ratio for region")
        .def("getTopActiveNeurons", &Brain::getTopActiveNeurons,
             py::arg("topN").none_or_default(),
             "Get top N most active neurons")
        // Memory system analysis
        .def("getWorkingMemoryUsage", &Brain::getWorkingMemoryUsage,
             "Get working memory usage")
        .def("getEpisodicMemoryCount", &Brain::getEpisodicMemoryCount,
             "Get episodic memory episode count")
        .def("getMemoryConsolidationScore", &Brain::getMemoryConsolidationScore,
             "Get memory consolidation score")
        // Development and learning metrics
        .def("getPlasticityIndex", &Brain::getPlasticityIndex,
             "Get plasticity index")
        .def("getLearningRate", &Brain::getLearningRate,
             "Get learning rate")
        .def("getExplorationRate", &Brain::getExplorationRate,
             "Get exploration rate")
        // Experimental mode helpers
        .def("enableExperimentalMode", &Brain::enableExperimentalMode,
             "Enable experimental mode")
        .def("enableResearchMode", &Brain::enableResearchMode,
             "Enable research mode")
        .def("disableAllModulators", &Brain::disableAllModulators,
             "Disable all neuromodulators")
        // Debugging and profiling methods
        .def("startProfiler", &Brain::startProfiler,
             "Start performance profiler")
        .def("stopProfiler", &Brain::stopProfiler,
             "Stop performance profiler")
        .def("isProfilerActive", &Brain::isProfilerActive,
             "Check if profiler is active")
        .def("getProfilerReport", &Brain::getProfilerReport,
             "Get profiler report")
        .def("setDebugLevel", &Brain::setDebugLevel,
             py::arg("level"),
             "Set debug level")
        .def("getDebugLevel", &Brain::getDebugLevel,
             "Get current debug level")
        .def("enableLoggingToFile", &Brain::enableLoggingToFile,
             py::arg("filepath"),
             "Enable file logging")
        .def("disableLoggingToFile", &Brain::disableLoggingToFile,
             "Disable file logging")
        .def("isLoggingToFile", &Brain::isLoggingToFile,
             "Check if logging to file is enabled")
        // Performance monitoring
        .def("getSimulationSpeed", &Brain::getSimulationSpeed,
             "Get simulation speed (steps per second)")
        .def("getAverageFiringRatePerStep", &Brain::getAverageFiringRatePerStep,
             "Get average firing rate per step")
        .def("getMemoryFootprint", &Brain::getMemoryFootprint,
             "Get memory footprint (bytes)")
        // Simulation state management
        .def("pauseSimulation", &Brain::pauseSimulation,
             "Pause simulation")
        .def("resumeSimulation", &Brain::resumeSimulation,
             "Resume simulation")
        .def("isSimulationPaused", &Brain::isSimulationPaused,
             "Check if simulation is paused")
        // Checkpoint and recovery
        .def("createCheckpoint", &Brain::createCheckpoint,
             py::arg("prefix").none_or_default(),
             "Create checkpoint with optional prefix")
        .def("restoreFromCheckpoint", &Brain::restoreFromCheckpoint,
             py::arg("filepath"),
             "Restore brain state from checkpoint")
        .def("getAvailableCheckpoints", &Brain::getAvailableCheckpoints,
             "Get list of available checkpoints")
        // Configuration validation
        .def("validateConfiguration", &Brain::validateConfiguration,
             py::arg("errors"),
             "Validate configuration and return errors")
        .def("isConfigurationValid", &Brain::isConfigurationValid,
             "Check if configuration is valid")
        // Convenience methods for common patterns
        .def("setupForControlTask", &Brain::setupForControlTask,
             py::arg("targetReward").none_or_default(), py::arg("explorationBonus").none_or_default(),
             "Setup brain for control task")
        .def("setupForExplorationTask", &Brain::setupForExplorationTask,
             py::arg("noveltyThreshold").none_or_default(), py::arg("curiosityFactor").none_or_default(),
             "Setup brain for exploration task")
        .def("setupForMemoryTask", &Brain::setupForMemoryTask,
             py::arg("memoryCapacity").none_or_default(), py::arg("consolidationRate").none_or_default(),
             "Setup brain for memory task")
        // Export and visualization helpers
        .def("generateActivityReport", &Brain::generateActivityReport,
             "Generate neural activity report")
        .def("generateConnectivityMatrix", &Brain::generateConnectivityMatrix,
             py::arg("regionId"),
             "Generate connectivity matrix for region")
        .def("exportNeutronActivity", &Brain::exportNeutronActivity,
             py::arg("filepath"), py::arg("regionId"),
             "Export neutron activity to file")
        .def("exportSynapseWeights", &Brain::exportSynapseWeights,
             py::arg("filepath"),
             "Export synapse weights to file")
        // Statistical analysis helpers
        .def("computeStatisticalSummary", &Brain::computeStatisticalSummary,
             py::arg("data"),
             "Compute statistical summary of data")
        .def("computeStatisticalSummaryByRegion", &Brain::computeStatisticalSummaryByRegion,
             "Compute statistical summary by region")
        // Debugging helpers
        .def("setNeuronDebugFlag", &Brain::setNeuronDebugFlag,
             py::arg("id"), py::arg("enabled"),
             "Set debug flag for neuron")
        .def("getNeuronDebugFlag", &Brain::getNeuronDebugFlag,
             py::arg("id"),
             "Get debug flag for neuron")
        .def("clearAllDebugFlags", &Brain::clearAllDebugFlags,
             "Clear all debug flags")
        // Error handling and recovery
        .def("executeSafely", &Brain::executeSafely,
             py::arg("simulationFunc"),
             "Execute simulation function with error handling")
        // Timing and synchronization
        .def("setTimeStep", &Brain::setTimeStep,
             py::arg("timestep"),
             "Set simulation time step")
        .def("getTimeStep", &Brain::getTimeStep,
             "Get current simulation time step")
        .def("setRandomSeed", &Brain::setRandomSeed,
             py::arg("seed"),
             "Set random seed for reproducibility")
        .def("getRandomSeed", &Brain::getRandomSeed,
             "Get random seed")
        // Real-time control
        .def("setSimulationSpeed", &Brain::setSimulationSpeed,
             py::arg("speed"),
             "Set simulation speed multiplier")
        .def("getSimulationSpeed", &Brain::getSimulationSpeed,
             "Get current simulation speed")
        // Advanced simulation control
        .def("enableFixedTimeStep", &Brain::enableFixedTimeStep,
             py::arg("enabled"),
             "Enable/disable fixed time step mode")
        .def("isFixedTimeStepEnabled", &Brain::isFixedTimeStepEnabled,
             "Check if fixed time step is enabled")
        .def("setMaxSimulationSteps", &Brain::setMaxSimulationSteps,
             py::arg("maxSteps"),
             "Set maximum simulation steps")
        .def("getMaxSimulationSteps", &Brain::getMaxSimulationSteps,
             "Get maximum simulation steps")
        .def("enableGracefulShutdown", &Brain::enableGracefulShutdown,
             py::arg("enabled"),
             "Enable/disable graceful shutdown")
        .def("isGracefulShutdownEnabled", &Brain::isGracefulShutdownEnabled,
             "Check if graceful shutdown is enabled")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

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
