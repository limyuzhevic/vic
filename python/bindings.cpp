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

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system with advanced validation and schema support)pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("filepath"),
             "Load configuration from file in constructor")
        
        // File operations
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON/YAML file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        
        // Configuration access and manipulation
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", &Config::get, py::arg("key"), py::arg("default") = nullptr,
             "Get configuration value by key")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set configuration value")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        
        // Validation and schema
        .def("validate", &Config::validate,
             "Validate configuration and return list of errors")
        .def("validateSchema", &Config::validateSchema,
             "Validate configuration against schema")
        .def("getSchema", &Config::getSchema,
             "Get configuration schema definition")
        
        // Configuration inheritance and merging
        .def("merge", &Config::merge, py::arg("other"),
             "Merge configuration with another config")
        .def("inherit", &Config::inherit, py::arg("parent"),
             "Inherit configuration from parent")
        
        // Environment variable integration
        .def("fromEnv", &Config::fromEnv,
             "Load configuration from environment variables")
        .def("fromEnvWithPrefix", &Config::fromEnvWithPrefix, py::arg("prefix"),
             "Load configuration from environment variables with prefix")
        
        // Configuration versioning
        .def("getVersion", &Config::getVersion,
             "Get current configuration version")
        .def("setVersion", &Config::setVersion, py::arg("version"),
             "Set configuration version")
        .def("getSchemaVersion", &Config::getSchemaVersion,
             "Get schema version for validation")
        
        // Configuration templates
        .def("applyTemplate", &Config::applyTemplate, py::arg("templateName"),
             "Apply predefined configuration template")
        .def("saveTemplate", &Config::saveTemplate, py::arg("filename"),
             "Save current configuration as template")
        .def("getAvailableTemplates", &Config::getAvailableTemplates,
             "Get list of available configuration templates")
        
        // Configuration utility methods
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("copy", &Config::copy,
             "Create a copy of the configuration")
        .def("clone", &Config::clone,
             "Clone the configuration")
        
        // Advanced features
        .def("watchForChanges", &Config::watchForChanges,
             "Watch configuration for changes")
        .def("reload", &Config::reload,
             "Reload configuration from source")
        
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("toDict", &Config::toDict,
             "Convert configuration to Python dictionary")
        .def("fromDict", &Config::fromDict, py::arg("configDict"),
             "Load configuration from Python dictionary")
        .def("toJSON", &Config::toJSON,
             "Export configuration as JSON string")
        .def("fromJSON", &Config::fromJSON, py::arg("jsonString"),
             "Load configuration from JSON string")
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

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation with advanced extensions)pbdoc")
        .def(py::init<>())
        
        // Basic configuration
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        
        // Advanced world configuration
        .def("configureAdvanced", &SimpleWorld::configureAdvanced, 
             py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
             py::arg("maxEnergy"), py::arg("energyDecayRate"), py::arg("resourceEnergyGain"),
             "Configure world with advanced parameters")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get current random seed")
        
        // Simulation controls
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state with timestep")
        .def("step", &SimpleWorld::update, py::arg("timestep"),
             "Alias for update (for consistency)")
        
        // Advanced motor command application
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command with detailed result")
        .def("applyAction", &SimpleWorld::applyAction,
             py::arg("action"), py::arg("currentTime"),
             "Apply action with automatic command conversion")
        
        // Enhanced sensory processing
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("addSensoryModality", &SimpleWorld::addSensoryModality,
             py::arg("name"), py::arg("processor"),
             "Add custom sensory modality processor")
        .def("removeSensoryModality", &SimpleWorld::removeSensoryModality,
             py::arg("name"),
             "Remove sensory modality")
        .def("getSensoryModalities", &SimpleWorld::getSensoryModalities,
             "Get list of active sensory modalities")
        
        // World event handling
        .def("addEventListener", &SimpleWorld::addEventListener,
             py::arg("eventType"), py::arg("callback"),
             "Add event listener for world events")
        .def("removeEventListener", &SimpleWorld::removeEventListener,
             py::arg("eventType"), py::arg("callback"),
             "Remove event listener")
        .def("triggerEvent", &SimpleWorld::triggerEvent,
             py::arg("eventType"), py::arg("data"),
             "Trigger custom world event")
        
        // Advanced object management
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("addObjectAdvanced", &SimpleWorld::addObjectAdvanced,
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Add object with advanced parameters")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             "Get object at position")
        .def("getObjects", &SimpleWorld::getObjects,
             "Get all world objects")
        .def("removeObjectAt", &SimpleWorld::removeObjectAt, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("getObjectsInRadius", &SimpleWorld::getObjectsInRadius,
             py::arg("x"), py::arg("y"), py::arg("radius"),
             "Get objects within radius")
        
        // Environment simulation controls
        .def("enableEnvironmentalPhysics", &SimpleWorld::enableEnvironmentalPhysics,
             py::arg("enable"),
             "Enable/disable environmental physics simulation")
        .def("enableResourceDynamics", &SimpleWorld::enableResourceDynamics,
             py::arg("enable"),
             "Enable/disable resource dynamics")
        .def("enableHazards", &SimpleWorld::enableHazards,
             py::arg("enable"),
             "Enable/disable hazard simulation")
        .def("setHazardDensity", &SimpleWorld::setHazardDensity, py::arg("density"),
             "Set hazard density (0.0-1.0)")
        .def("addHazard", &SimpleWorld::addHazard, py::arg("x"), py::arg("y"), py::arg("type"),
             "Add hazard at position")
        .def("getHazardCount", &SimpleWorld::getHazardCount,
             "Get total number of hazards")
        
        // Vision and perception
        .def("getVision", &SimpleWorld::getVision,
             "Get vision sensory data")
        .def("getVisionWidth", &SimpleWorld::getVisionWidth,
             "Get vision width in cells")
        .def("getVisionHeight", &SimpleWorld::getVisionHeight,
             "Get vision height in cells")
        
        // Agent body state
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("updateAgentBody", &SimpleWorld::updateAgentBody, py::arg("agent"),
             "Update agent body state directly")
        
        // Utility functions
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        .def("getWorldBounds", &SimpleWorld::getWorldBounds,
             "Get world boundary coordinates")
        .def("getAccessibleArea", &SimpleWorld::getAccessibleArea,
             "Get accessible area where agent can move")
        
        // Simulation time and statistics
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("setSimulationTime", &SimpleWorld::setSimulationTime, py::arg("time"),
             "Set simulation time")
        .def("getStepCount", &SimpleWorld::getStepCount,
             "Get number of simulation steps taken")
        
        // Resource management
        .def("getResourceCount", &SimpleWorld::getResourceCount,
             "Get number of resources in world")
        .def("getHazardCount", &SimpleWorld::getHazardCount,
             "Get number of hazards in world")
        .def("getAgentEnergy", &SimpleWorld::getAgentEnergy,
             "Get agent current energy")
        .def("setAgentEnergy", &SimpleWorld::setAgentEnergy, py::arg("energy"),
             "Set agent energy level")
        
        // Visualization and debugging
        .def("generateMap", &SimpleWorld::generateMap, py::arg("format") = "ascii",
             "Generate world map for visualization")
        .def("exportState", &SimpleWorld::exportState, py::arg("filepath"),
             "Export world state to file")
        .def("importState", &SimpleWorld::importState, py::arg("filepath"),
             "Import world state from file")
        .def("logState", &SimpleWorld::logState,
             "Log current world state for debugging")

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        
        // Simulation operations
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        
        // Batch processing capabilities
        .def("batchStep", &Brain::batchStep, py::arg("steps"), py::arg("timestep"),
             "Perform multiple simulation steps in batch")
        .def("runSimulation", &Brain::runSimulation, py::arg("steps"), py::arg("timestep"), py::arg("startStep"),
             "Run complete simulation with specified steps")
        
        // Parallel processing support
        .def("parallelStep", &Brain::parallelStep, py::arg("steps"), py::arg("timestep"), py::arg("workers"),
             "Perform parallel simulation steps")
        .def("setWorkerCount", &Brain::setWorkerCount, py::arg("workers"),
             "Set number of worker threads for parallel processing")
        
        // Brain cloning and copying
        .def("clone", &Brain::clone,
             "Create a deep copy of the brain")
        .def("copy", &Brain::copy,
             "Create a shallow copy of the brain")
        .def("duplicate", &Brain::duplicate,
             "Duplicate brain configuration and state")
        
        // Input and output operations
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
        
        // State management
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("clear", &Brain::clear,
             "Clear all neural state and data")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        
        // Brain structure manipulation
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
        
        // Developmental control
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        
        // Data access and analysis
        .def("getActivityData", &Brain::getActivityData,
             "Get neural activity data as array")
        .def("getSpikeTimes", &Brain::getSpikeTimes,
             "Get spike times for all neurons")
        .def("getNeuronStates", &Brain::getNeuronStates,
             "Get current states of all neurons")
        .def("getConnectionMatrix", &Brain::getConnectionMatrix,
             "Get synaptic connection matrix")
        
        // Configuration and status
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        
        // Performance monitoring and profiling
        .def("startProfiler", &Brain::startProfiler,
             "Start performance profiling")
        .def("stopProfiler", &Brain::stopProfiler,
             "Stop performance profiling")
        .def("getProfilingStats", &Brain::getProfilingStats,
             "Get profiling statistics")
        .def("getPerformanceMetrics", &Brain::getPerformanceMetrics,
             "Get performance metrics")
        
        // Visualization
        .def("visualize", &Brain::visualize, py::arg("outputFormat") = "text",
             "Generate visualization of brain state")
        .def("generateHeatmap", &Brain::generateHeatmap, py::arg("neuronType"),
             "Generate activity heatmap for neuron type")
        .def("generateConnectivityPlot", &Brain::generateConnectivityPlot,
             "Generate connectivity visualization")
        .def("exportForVisualization", &Brain::exportForVisualization, py::arg("format"),
             "Export brain data for external visualization tools");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world with enhanced sensory processing)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        
        // Advanced sensory processing options
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
        
        // Custom action selection strategies
        .def("selectAction", &AgentBrain::selectAction,
             "Select action using advanced strategies (curiosity, exploration, etc.)")
        .def("setActionSelectionStrategy", &AgentBrain::setActionSelectionStrategy,
             py::arg("strategy"),
             "Set action selection strategy (epsilon-greedy, Thompson sampling, etc.)")
        .def("getActionSelectionStrategy", &AgentBrain::getActionSelectionStrategy,
             "Get current action selection strategy")
        
        // Advanced neuromodulation controls
        .def("updateNeuromodulation", &AgentBrain::updateNeuromodulation,
             "Update all neuromodulation systems")
        .def("setNeuromodulationLevel", &AgentBrain::setNeuromodulationLevel,
             py::arg("level"),
             "Set global neuromodulation level")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        
        // Development stage management
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &AgentBrain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getLearningRate", &AgentBrain::getLearningRate,
             "Get current learning rate")
        .def("setLearningRate", &AgentBrain::setLearningRate,
             py::arg("rate"),
             "Set learning rate for plasticity")
        
        // Neuromodulation system access
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("getDopamineLevel", &AgentBrain::getDopamineLevel,
             "Get dopamine level")
        
        // Advanced sensory processing
        .def("addCustomSensoryModality", &AgentBrain::addCustomSensoryModality,
             py::arg("name"), py::arg("processor"),
             "Add custom sensory modality processor")
        .def("setSensoryWeighting", &AgentBrain::setSensoryWeighting,
             py::arg("modality"), py::arg("weight"),
             "Set weighting for a sensory modality")
        .def("getSensoryWeights", &AgentBrain::getSensoryWeights,
             "Get current sensory modality weights")
        
        // Reset and state management
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("clear", &AgentBrain::clear,
             "Clear agent state")
        .def("saveState", &AgentBrain::saveState, py::arg("filepath"),
             "Save agent state to file")
        .def("loadState", &AgentBrain::loadState, py::arg("filepath"),
             "Load agent state from file")
        
        // Configuration flags
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("enableExploration", &AgentBrain::enableExploration,
             py::arg("enable"))
        .def("enableNoveltyDetection", &AgentBrain::enableNoveltyDetection,
             py::arg("enable"))
        .def("enablePrediction", &AgentBrain::enablePrediction,
             py::arg("enable"))
        
        // Status checking
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("isExplorationEnabled", &AgentBrain::isExplorationEnabled)
        .def("isNoveltyDetectionEnabled", &AgentBrain::isNoveltyDetectionEnabled)
        .def("isPredictionEnabled", &AgentBrain::isPredictionEnabled)
        
        // Advanced analysis
        .def("analyzeBehavior", &AgentBrain::analyzeBehavior,
             "Analyze agent behavior patterns")
        .def("getBehaviorStats", &AgentBrain::getBehaviorStats,
             "Get behavioral statistics")
        .def("getPerformanceMetrics", &AgentBrain::getPerformanceMetrics,
             "Get performance metrics")
        
        // Context management
        .def("createContext", &AgentBrain::createContext,
             py::arg("name"), py::arg("params") = "",
             "Create execution context")
        .def("getContext", &AgentBrain::getContext,
             py::arg("name"),
             "Get execution context")
        .def("getAllContexts", &AgentBrain::getAllContexts,
             "Get all execution contexts")
        .def("removeContext", &AgentBrain::removeContext,
             py::arg("name"),
             "Remove execution context")
        
        // Memory management
        .def("getMemoryUsage", &AgentBrain::getMemoryUsage,
             "Get memory usage statistics")
        .def("clearMemory", &AgentBrain::clearMemory,
             "Clear internal memory buffers")
        
        // Brain reference
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain");

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
