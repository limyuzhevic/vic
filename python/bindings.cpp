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

    // Environment-related classes
    py::class_<EnvironmentState>(m, "EnvironmentState", R"pbdoc(Environment state information)pbdoc")
        .def(py::init<>())
        .def("is_terminal", &EnvironmentState::is_terminal)
        .def("set_terminal", &EnvironmentState::set_terminal);

    py::class_<Environment>(m, "Environment", R"pbdoc(Base environment interface)pbdoc")
        .def(py::init<>())
        .def("reset", &Environment::reset)
        .def("step", &Environment::step, py::arg("action"))
        .def("get_state", &Environment::getState, py::return_value_policy::reference_internal)
        .def("get_last_reward", &Environment::getLastReward)
        .def("is_done", &Environment::isDone)
        .def("get_name", &Environment::getName)
        .def("get_action_space_size", &Environment::getActionSpaceSize)
        .def("get_observation_space_size", &Environment::getObservationSpaceSize);

    py::class_<GridWorldEnvironment>(m, "GridWorldEnvironment", R"pbdoc(Grid world environment)pbdoc")
        .def(py::init<int, int>(), py::arg("width"), py::arg("height"))
        .def("configure", &GridWorldEnvironment::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &GridWorldEnvironment::reset)
        .def("step", &GridWorldEnvironment::step, py::arg("action"))
        .def("get_state", &GridWorldEnvironment::getState, py::return_value_policy::reference_internal)
        .def("get_last_reward", &GridWorldEnvironment::getLastReward)
        .def("is_done", &GridWorldEnvironment::isDone)
        .def("get_name", &GridWorldEnvironment::getName)
        .def("get_action_space_size", &GridWorldEnvironment::getActionSpaceSize)
        .def("get_observation_space_size", &GridWorldEnvironment::getObservationSpaceSize);

    // Action result and observation classes
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Result of taking an action)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(), py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<Observation>(m, "Observation", R"pbdoc(Sensory observation data)pbdoc")
        .def(py::init<>())
        .def("get_data", &Observation::getData)
        .def("get_dimensions", &Observation::getDimensions)
        .def("set_from_percept", &Observation::setFromPercept, py::arg("percept"));

    // Development system classes
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system for brain maturation)pbdoc")
        .def(py::init<>())
        .def("update", &DevelopmentSystem::update, py::arg("brain"), py::arg("rng"), py::arg("dt"));

    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Synaptic formation process)pbdoc")
        .def(py::init<>())
        .def("update", &Synaptogenesis::update, py::arg("brain"), py::arg("rng"), py::arg("dt"));

    py::class_<Maturation>(m, "Maturation", R"pbdoc(Brain maturation process)pbdoc")
        .def(py::init<>());

    py::class_<Pruning>(m, "Pruning", R"pbdoc(Synaptic pruning process)pbdoc")
        .def(py::init<>());

    // Neuromodulation system
    py::class_<Reward>(m, "Reward", R"pbdoc(Reward system)pbdoc")
        .def(py::init<>())
        .def("get_value", &Reward::getValue)
        .def("set_value", &Reward::setValue, py::arg("value"))
        .def("add", &Reward::add, py::arg("delta"))
        .def("reset", &Reward::reset)
        .def("compute_reward", &Reward::computeReward, py::arg("observation"))
        .def("get_history", &Reward::getHistory)
        .def("clear_history", &Reward::clearHistory);

    // Experiment system
    py::class_<Experiment>(m, "Experiment", R"pbdoc(Experiment tracking system)pbdoc")
        .def(py::init<>())
        .def(py::init<std::string>(), py::arg("name"))
        .def("get_name", &Experiment::getName)
        .def("set_name", &Experiment::setName, py::arg("name"))
        .def("set_config_value", &Experiment::setConfigValue, py::arg("key"), py::arg("value"))
        .def("get_config_value", &Experiment::getConfigValue, py::arg("key"))
        .def("record_metric", static_cast<void (Experiment::*)(const std::string&, double)>(&Experiment::recordMetric), py::arg("name"), py::arg("value"))
        .def("record_metric", static_cast<void (Experiment::*)(const std::string&, float)>(&Experiment::recordMetric), py::arg("name"), py::arg("value"))
        .def("record_metric", static_cast<void (Experiment::*)(const std::string&, int)>(&Experiment::recordMetric), py::arg("name"), py::arg("value"))
        .def("get_metrics", &Experiment::getMetrics)
        .def("get_metric", &Experiment::getMetric, py::arg("name"))
        .def("has_metric", &Experiment::hasMetric, py::arg("name"))
        .def("record_step", &Experiment::recordStep, py::arg("step"), py::arg("neuron_count"), py::arg("synapse_count"), py::arg("firing_count"), py::arg("avg_firing_rate"))
        .def("get_step_data", &Experiment::getStepData)
        .def("get_start_step", &Experiment::getStartStep)
        .def("set_start_step", &Experiment::setStartStep, py::arg("step"))
        .def("get_end_step", &Experiment::getEndStep)
        .def("set_end_step", &Experiment::setEndStep, py::arg("step"))
        .def("add_note", &Experiment::addNote, py::arg("note"))
        .def("get_notes", &Experiment::getNotes)
        .def("add_note", &Experiment::addNote, py::arg("note"))
        .def("get_notes", &Experiment::getNotes);

    // Performance and utilities
    py::class_<Metrics>(m, "Metrics", R"pbdoc(System metrics collection)pbdoc")
        .def(py::init<>());

    py::class_<CheckpointManager>(m, "CheckpointManager", R"pbdoc(Checkpoint persistence system)pbdoc")
        .def(py::init<>())
        .def("configure", &CheckpointManager::configure, py::arg("dir"), py::arg("maxCheckpoints"), py::arg("maxFileSize"), py::arg("compress"))
        .def("create_checkpoint", &CheckpointManager::createCheckpoint, py::arg("brain"), py::arg("step"), py::arg("time"))
        .def("restore_checkpoint", &CheckpointManager::restoreCheckpoint, py::arg("brain"), py::arg("step"), py::arg("time"))
        .def("get_latest_checkpoint", &CheckpointManager::getLatestCheckpoint)
        .def("get_checkpoint_info", &CheckpointManager::getCheckpointInfo)
        .def("list_checkpoints", &CheckpointManager::listCheckpoints)
        .def("remove_checkpoint", &CheckpointManager::removeCheckpoint, py::arg("step"));

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

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
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

    // Environment and action factories
    m.def("createEnvironment", []() -> std::shared_ptr<Environment> {
        return std::make_shared<Environment>();
    }, "Create a base environment");

    m.def("createGridWorldEnvironment", [](int width, int height) -> std::shared_ptr<GridWorldEnvironment> {
        return std::make_shared<GridWorldEnvironment>(width, height);
    }, py::arg("width") = 10, py::arg("height") = 10, 
       "Create a grid world environment");

    m.def("createAction", [](ActionType type, std::vector<float> params) -> std::shared_ptr<Action> {
        return std::make_shared<Action>(type, params);
    }, py::arg("type"), py::arg("params") = std::vector<float>(), 
       "Create an action with type and optional parameters");

    m.def("createActionResult", [](float reward, bool success, std::string message) -> std::shared_ptr<ActionResult> {
        return std::make_shared<ActionResult>(reward, success, message);
    }, py::arg("reward"), py::arg("success"), py::arg("message") = "",
       "Create an action result");

    m.def("createObservation", [](std::vector<float> data) -> std::shared_ptr<Observation> {
        auto obs = std::make_shared<Observation>();
        // Note: This is a simple constructor - in practice, observations are created by environments
        return obs;
    }, py::arg("data"), "Create an observation with data");

    m.def("createDevelopmentSystem", []() -> std::shared_ptr<DevelopmentSystem> {
        return std::make_shared<DevelopmentSystem>();
    }, "Create a development system");

    m.def("createSynaptogenesis", []() -> std::shared_ptr<Synaptogenesis> {
        return std::make_shared<Synaptogenesis>();
    }, "Create a synaptogenesis system");

    m.def("createMaturation", []() -> std::shared_ptr<Maturation> {
        return std::make_shared<Maturation>();
    }, "Create a maturation system");

    m.def("createPruning", []() -> std::shared_ptr<Pruning> {
        return std::make_shared<Pruning>();
    }, "Create a pruning system");

    m.def("createMetrics", []() -> std::shared_ptr<Metrics> {
        return std::make_shared<Metrics>();
    }, "Create a metrics system");

    m.def("createCheckpointManager", []() -> std::shared_ptr<CheckpointManager> {
        return std::make_shared<CheckpointManager>();
    }, "Create a checkpoint manager");

    m.def("createReward", []() -> std::shared_ptr<Reward> {
        return std::make_shared<Reward>();
    }, "Create a reward system");

    m.def("createExperiment", [](std::string name) -> std::shared_ptr<Experiment> {
        return std::make_shared<Experiment>(name);
    }, py::arg("name") = "Default", "Create an experiment with optional name");

    m.def("createPhase6Config", []() -> Phase6Config {
        Phase6Config config;
        config.neuronCount = 1000;
        config.maxSteps = 10000;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        config.enableNeuromodulation = false;
        config.enablePrediction = false;
        config.logLevel = "INFO";
        return config;
    }, "Create a Phase 6 experiment configuration");

    // Update development system with experiment tracking
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(
            this,
            *pImpl->rng,
            static_cast<TimestepDuration>(pImpl->timestep * pImpl->currentStep)
        );
        // Track development metrics
        pImpl->experiment->recordMetric("development_stage", 
            static_cast<double>(pImpl->developmentalStage));
        pImpl->experiment->recordMetric("plasticity_rate", 
            static_cast<double>(pImpl->structuralPlasticity->getSynaptogenesisRate()));
    }

    // Record metrics at regular intervals
    if (currentStep % 100 == 0) {
        size_t activeNeurons = getActiveNeuronCount();
        float firingRate = getAverageFiringRate();
        pImpl->experiment->recordMetric("active_neurons", static_cast<double>(activeNeurons));
        pImpl->experiment->recordMetric("firing_rate", static_cast<double>(firingRate));
        pImpl->experiment->recordStep(currentStep, 
            getTotalNeuronCount(), getTotalSynapseCount(), 
            getFiringNeuronCount(), firingRate);
    }
