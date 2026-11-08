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
#include "../src/core/Random/Random.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/Synapse.hpp"

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

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", &Config::get<int>, py::arg("key"),
             "Get configuration value as int")
        .def("get", &Config::get<double>, py::arg("key"),
             "Get configuration value as double")
        .def("get", &Config::get<bool>, py::arg("key"),
             "Get configuration value as bool")
        .def("get", &Config::get<std::string>, py::arg("key"),
             "Get configuration value as string")
        .def("getOr", &Config::getOr<int>, py::arg("key"), py::arg("default"),
             "Get configuration value or default if not present (int)")
        .def("getOr", &Config::getOr<double>, py::arg("key"), py::arg("default"),
             "Get configuration value or default if not present (double)")
        .def("getOr", &Config::getOr<bool>, py::arg("key"), py::arg("default"),
             "Get configuration value or default if not present (bool)")
        .def("getOr", &Config::getOr<std::string>, py::arg("key"), py::arg("default"),
             "Get configuration value or default if not present (string)")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value as int")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value as double")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value as bool")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value as string")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
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
        .def("setData", [](Vision& self, float* data, size_t size) {
            self.setData(data, size);
        }, py::arg("data"), py::arg("size"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getData", &Vision::getData)
        .def("getDimensions", &Vision::getDimensions);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getData", &Audio::getData)
        .def("getDimensions", &Audio::getDimensions);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getData", &InternalSignals::getData)
        .def("getDimensions", &InternalSignals::getDimensions);

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

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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
        // Neural system access methods
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signal")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator")
        .def("getLearnedPreference", &Brain::getLearnedPreference,
             "Get learned preference trait")
        .def("setLearnedPreference", &Brain::setLearnedPreference,
             py::arg("preference"),
             "Set learned preference trait")

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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

    // Random number generator for reproducible experiments
    py::class_<RandomGenerator>(m, "RandomGenerator", R"pbdoc(Deterministic random number generator for reproducible experiments)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("seed"))
        .def("seed", &RandomGenerator::seed, py::arg("seed"), "Set random seed for reproducibility")
        .def("getSeed", &RandomGenerator::getSeed, "Get current random seed")
        .def("uniformInt", static_cast<uint32_t (RandomGenerator::*)(uint32_t, uint32_t)>(&RandomGenerator::uniformInt), 
             py::arg("min"), py::arg("max"), "Generate random integer in [min, max]")
        .def("uniformReal", static_cast<double (RandomGenerator::*)(double, double)>(&RandomGenerator::uniformReal),
             py::arg("min"), py::arg("max"), "Generate random float in [min, max]")
        .def("normal", static_cast<double (RandomGenerator::*)(double, double)>(&RandomGenerator::normal),
             py::arg("mean"), py::arg("stddev"), "Generate normally distributed random number")
        .def("bernoulli", &RandomGenerator::bernoulli, py::arg("p") = 0.5, "Generate Bernoulli random variable")
        .def("poisson", &RandomGenerator::poisson, py::arg("lambda"), "Generate Poisson random variable")
        .def("exponential", &RandomGenerator::exponential, py::arg("lambda"), "Generate exponential random variable")
        .def("randomBool", &RandomGenerator::randomBool, "Generate random boolean")
        .def("randomBit", &RandomGenerator::randomBit, "Generate random bit")
        .def("pickRandom", &RandomGenerator::pickRandom, py::arg("container"), 
             "Pick random element from container")
        .def("pickRandomIndex", &RandomGenerator::pickRandomIndex, py::arg("n"), 
             "Pick random index from 0 to n-1")
        .def("serializeState", &RandomGenerator::serializeState, "Serialize generator state")
        .def("restoreState", &RandomGenerator::restoreState, py::arg("state"), 
             "Restore generator state from serialized string");

    // Simulation clock for deterministic time management
    py::class_<SimulationClock>(m, "SimulationClock", R"pbdoc(Simulation clock for deterministic time management)pbdoc")
        .def(py::init<TimestepDuration>(), py::arg("timestep") = 0.001)
        .def("reset", &SimulationClock::reset, "Reset clock to initial state")
        .def("advance", &SimulationClock::advance, "Advance clock by one timestep")
        .def("getStep", &SimulationClock::getStep, "Get current simulation step")
        .def("setStep", &SimulationClock::setStep, py::arg("step"), "Set simulation step")
        .def("getTime", &SimulationClock::getTime, "Get current simulation time")
        .def("setTime", &SimulationClock::setTime, py::arg("time"), "Set simulation time")
        .def("getTimestep", &SimulationClock::getTimestep, "Get simulation timestep")
        .def("setTimestep", &SimulationClock::setTimestep, py::arg("dt"), "Set simulation timestep")
        .def("isFirstStep", &SimulationClock::isFirstStep, "Check if current step is first")
        .def("isLastStep", &SimulationClock::isLastStep, py::arg("totalSteps"), "Check if current step is last")
        .def("stepToTime", &SimulationClock::stepToTime, py::arg("step"), "Convert step to time")
        .def("timeToStep", &SimulationClock::timeToStep, py::arg("time"), "Convert time to step")
        .def("advanceBy", &SimulationClock::advanceBy, py::arg("steps"), "Advance by multiple steps")
        .def("timeAtFutureStep", &SimulationClock::timeAtFutureStep, py::arg("stepsAhead"), 
             "Get time at a future step from current time");

    // Logger for debugging and monitoring
    py::class_<Logger>(m, "Logger", R"pbdoc(Logging system for debugging and monitoring)pbdoc")
        .def(py::init<>())
        .def("setLevel", &Logger::setLevel, py::arg("level"), "Set minimum logging level")
        .def("isEnabled", &Logger::isEnabled, py::arg("level"), "Check if logging level is enabled")
        .def("log", &Logger::log, py::arg("level"), py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
             "Log message with file/line/function information")
        .def("debug", &Logger::debug, py::arg("message"), py::arg("file") = "", py::arg("line") = <br>0, py::arg("function") = "",
             "Log debug message")
        .def("info", &Logger::info, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
             "Log info message")
        .def("warning", &Logger::warning, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
             "Log warning message")
        .def("error", &Logger::error, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
             "Log error message")
        .def("critical", &Logger::critical, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
             "Log critical message");

    // Neuron - single spiking neuron
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Single spiking neuron implementing Leaky Integrate-and-Fire dynamics)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"), "Create neuron with ID")
        .def("getId", &Neuron::getId, "Get neuron ID")
        .def("getType", &Neuron::getType, "Get neuron type")
        .def("setType", &Neuron::setType, py::arg("type"), "Set neuron type")
        .def("getMembranePotential", &Neuron::getMembranePotential, "Get membrane potential (mV)")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"), "Set membrane potential (mV)")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"), "Add to membrane potential")
        .def("getThreshold", &Neuron::getThreshold, "Get firing threshold (mV)")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"), "Set firing threshold (mV)")
        .def("isFiring", &Neuron::isFiring, "Check if neuron is firing")
        .def("isRefractory", &Neuron::isRefractory, "Check if neuron is in refractory period")
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"), "Set firing state")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"), "Set refractory period")
        .def("decrementRefractory", &Neuron::decrementRefractory, "Decrement refractory counter")
        .def("getFiringRate", &Neuron::getFiringRate, "Get firing rate (Hz)")
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"), "Set firing rate (Hz)")
        .def("checkThreshold", &Neuron::checkThreshold, "Check if neuron has reached firing threshold")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime, "Get timestamp of last spike (-1 if none)")
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt") = 0.001, 
             "Advance neuron state using LIF dynamics (returns true if fired)")
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"), "Receive excitatory input")
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"), "Receive inhibitory input")
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"), "Receive modulatory input")
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"), "Inject current from external source")
        .def("getTotalCurrent", &Neuron::getTotalCurrent, "Get total injected current")
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent, "Clear injected current")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"), "Record spike at given timestamp")
        .def("getSpikeHistory", &Neuron::getSpikeHistory, "Get spike history vector")
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory, "Clear spike history")
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("handle"), "Add incoming synapse")
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("handle"), "Add outgoing synapse")
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses, "Get incoming synapse handles")
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses, "Get outgoing synapse handles")
        .def("getState", &Neuron::getState, "Get neuron state (const)")
        .def("getMutableState", &Neuron::getState, "Get mutable neuron state")
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"), "Set region ID")
        .def("getRegionId", &Neuron::getRegionId, "Get region ID")
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"), "Set population ID")
        .def("getPopulationId", &Neuron::getPopulationId, "Get population ID")
        .def("step", &Neuron::step, py::arg("currentTime"), "Advance neuron for one simulation step")
        .def("reset", &Neuron::reset, "Reset neuron to initial state")
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"), "Initialize neuron with random parameters");

    // NeuralRegion - brain region containing multiple populations
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Brain region containing multiple populations and local connectivity)pbdoc")
        .def(py::init<RegionId>(), py::arg("id"), "Create region with ID")
        .def(py::init<RegionId, std::string>(), py::arg("id"), py::arg("name"), "Create region with ID and name")
        .def("getId", &NeuralRegion::getId, "Get region ID")
        .def("getName", &NeuralRegion::getName, "Get region name")
        .def("setName", &NeuralRegion::setName, py::arg("name"), "Set region name")
        .def("setNextNeuronIndex", &NeuralRegion::setNextNeuronIndex, py::arg("next"), 
             "Set next neuron ID to allocate (for global ID uniqueness)")
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("size"), py::arg("type") = NeuronType::Internal,
             "Add a new population with specified size and neuron type")
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"), "Get population by ID")
        .def("getPopulationCount", &NeuralRegion::getPopulationCount, "Get number of populations")
        .def("getPopulations", &NeuralRegion::getPopulations, "Get all populations (const)")
        .def("getAllPopulations", &NeuralRegion::getAllPopulations, "Get all populations (mutable)")
        .def("addSynapse", &NeuralRegion::addSynapse, py::arg("source"), py::arg("destination"), 
             py::arg("weight") = 0.0f, py::arg("delay") = 1, "Add local synapse within region")
        .def("getSynapse", &NeuralRegion::getSynapse, py::arg("id"), "Get synapse by ID")
        .def("getSynapseCount", &NeuralRegion::getSynapseCount, "Get number of synapses")
        .def("getSynapses", &NeuralRegion::getSynapses, "Get all synapses (const)")
        .def("getSynapsesFrom", &NeuralRegion::getSynapsesFrom, py::arg("neuron"), 
             "Get all synapses originating from neuron")
        .def("getSynapsesTo", &NeuralRegion::getSynapsesTo, py::arg("neuron"), 
             "Get all synapses terminating at neuron")
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount, "Get total neuron count")
        .def("getActiveNeuronCount", &NeuralRegion::getActiveNeuronCount, "Get active neuron count")
        .def("getFiringNeuronCount", &NeuralRegion::getFiringNeuronCount, "Get firing neuron count")
        .def("getAverageFiringRate", &NeuralRegion::getAverageFiringRate, "Get average firing rate")
        .def("getAverageSynapticWeight", &NeuralRegion::getAverageSynapticWeight, "Get average synaptic weight")
        .def("getSynapticDensity", &NeuralRegion::getSynapticDensity, "Get synaptic density (fraction of possible connections)")
        .def("step", &NeuralRegion::step, py::arg("currentTime"), "Step all populations and synapses")
        .def("reset", &NeuralRegion::reset, "Reset all neurons and synapses")
        .def("initializeRandomConnectivity", &NeuralRegion::initializeRandomConnectivity,
             py::arg("rng"), py::arg("connectionProbability"), py::arg("meanWeight"), py::arg("weightVariance"),
             "Initialize random connectivity")
        .def("getAllNeurons", &NeuralRegion::getAllNeurons, "Get all neurons in region (mutable)")
        .def("getAllNeuronsConst", &NeuralRegion::getAllNeurons, "Get all neurons in region (const)");

    // NeuralPopulation - group of neurons with shared properties
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Group of neurons with shared properties)pbdoc")
        .def(py::init<PopulationId, size_t>(), py::arg("id"), py::arg("size"), "Create population with ID and size")
        .def("getId", &NeuralPopulation::getId, "Get population ID")
        .def("getSize", &NeuralPopulation::getSize, "Get population size")
        .def("isEmpty", &NeuralPopulation::isEmpty, "Check if population is empty")
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("index"), "Get neuron at index")
        .def("getMutableNeuron", &NeuralPopulation::getNeuron, py::arg("index"), "Get mutable neuron at index")
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"), "Add neuron to population (caller retains ownership)")
        .def("createNeurons", &NeuralPopulation::createNeurons, py::arg("startId"), 
             "Create `size` neurons owned by this population with sequential IDs")
        .def("getNeurons", &NeuralPopulation::getNeurons, "Get all neurons in population (const)")
        .def("getNeuronType", &NeuralPopulation::getNeuronType, "Get neuron type for all neurons in population")
        .def("setNeuronType", &NeuralPopulation::setNeuronType, py::arg("type"), "Set neuron type for all neurons")
        .def("getActiveCount", &NeuralPopulation::getActiveCount, "Get number of active neurons")
        .def("getFiringCount", &NeuralPopulation::getFiringCount, "Get number of firing neurons")
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate, "Get average firing rate")
        .def("getActivityLevel", &NeuralPopulation::getActivityLevel, "Get activity level (fraction of active neurons)")
        .def("initializeRandom", &NeuralPopulation::initializeRandom, py::arg("rng"), 
             "Initialize all neurons randomly")
        .def("step", &NeuralPopulation::step, py::arg("currentTime"), "Step all neurons")
        .def("reset", &NeuralPopulation::reset, "Reset all neurons")
        .def("getNeuronIds", &NeuralPopulation::getNeuronIds, "Get all neuron IDs in population")
        .def("recordActivity", &NeuralPopulation::recordActivity, "Record current state for statistics")
        .def("getMeanMembranePotential", &NeuralPopulation::getMeanMembranePotential, "Get mean membrane potential")
        .def("getVarianceMembranePotential", &NeuralPopulation::getVarianceMembranePotential, "Get variance of membrane potentials");

    // Synapse - connection between neurons
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Connection between neurons with transmission dynamics and plasticity)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"), 
             "Create synapse with IDs")
        .def("getId", &Synapse::getId, "Get synapse ID")
        .def("getSourceNeuron", &Synapse::getSourceNeuron, "Get source neuron ID")
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron, "Get destination neuron ID")
        .def("getWeight", &Synapse::getWeight, "Get synaptic weight")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"), "Set synaptic weight")
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"), "Add to synaptic weight")
        .def("getDelay", &Synapse::getDelay, "Get synaptic delay (in simulation steps)")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"), "Set synaptic delay (in simulation steps)")
        .def("getType", &Synapse::getType, "Get synapse type")
        .def("setType", &Synapse::setType, py::arg("type"), "Set synapse type")
        .def("isExcitatory", &Synapse::isExcitatory, "Check if synapse is excitatory")
        .def("isInhibitory", &Synapse::isInhibitory, "Check if synapse is inhibitory")
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"), "Record presynaptic spike")
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"), "Record postsynaptic spike")
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory, "Get presynaptic spike history")
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory, "Get postsynaptic spike history")
        .def("clearHistory", &Synapse::clearHistory, "Clear spike history")
        .def("setPreSpikeHistory", &Synapse::setPreSpikeHistory, py::arg("history"), 
             "Restore presynaptic spike history (for checkpoint)")
        .def("setPostSpikeHistory", &Synapse::setPostSpikeHistory, py::arg("history"), 
             "Restore postsynaptic spike history (for checkpoint)")
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags, "Get plasticity flags (const)")
        .def("getMutablePlasticityFlags", &Synapse::getPlasticityFlags, "Get mutable plasticity flags")
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"),
             "Enable plasticity rules (Hebbian, STDP, reward-modulated)")
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace, "Get eligibility trace value")
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"), "Set eligibility trace")
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"), "Decay eligibility trace")
        .def("getEfficacy", &Synapse::getEfficacy, "Get synaptic efficacy (use-dependent modulation)")
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"), "Set synaptic efficacy")
        .def("step", &Synapse::step, py::arg("currentTime"), "Update synapse for one simulation step")
        .def("reset", &Synapse::reset, "Reset synapse to initial state")
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"), 
             "Initialize synapse with random parameters");

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
