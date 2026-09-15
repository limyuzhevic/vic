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
             "Load configuration from a JSON or key=value file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file")
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
             py::arg("channels") = 3, "Create vision input with given dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the vision data")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples")
             , "Create audio input with given sample rate and duration")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the audio data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set sample rate")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals");

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

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a new brain with configuration")
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
             "Log brain status")
        // Context manager support for safe resource management
        .def("__enter__", [](Brain& self) -> Brain& {
            self.initialize();
            return self;
        })
        .def("__exit__", [](Brain& self, py::object, py::object, py::object) {
            self.reset();
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

    // Python-specific utilities
    m.def("BrainContext", []() {
        return py::cpp_function([](py::args args, py::kwargs kwargs) {
            // Context manager for brain
            return py::cpp_function([](std::shared_ptr<Config> config) {
                return py::make_tuple(std::make_shared<Brain>(config));
            });
        });
    });

    // Simulation utility functions
    m.def("runEpisode", [](std::shared_ptr<Config> config, std::shared_ptr<SimpleWorld> world, 
                         std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, 
                         int num_steps) {
        // Run a complete simulation episode
        agent->initialize(*world);
        
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto motor_cmd = agent->decodeMotorCommand();
            world->applyMotorCommand(motor_cmd, world->getSimulationTime());
            
            float reward = world->getSensoryPercept().getInternal()[0];
            agent->applyRewardModulation(reward, 0.0);
        }
        
        return std::make_tuple(
            brain->getTotalSpikeCount(),
            agent->getCuriosityLevel(),
            brain->getDevelopmentalStage()
        );
    }, py::arg("config"), py::arg("world"), py::arg("brain"), py::arg("agent"), py::arg("num_steps"),
        "Run a complete simulation episode");
    
    m.def("createEpisode", []() {
        // Factory function for creating a complete episode setup
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        
        return std::make_tuple(config, brain, agent, world);
    }, "Create a complete episode setup");

    // Statistics and analysis utilities
    m.def("analyzeActivity", [](std::shared_ptr<Brain> brain) {
        std::unordered_map<std::string, double> stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["avg_firing_rate"] = brain->getAverageFiringRate();
        stats["e_to_i_ratio"] = brain->getExcitationInhibitionRatio();
        
        return stats;
    }, py::arg("brain"), "Analyze brain activity and return statistics");

    m.def("profilePerformance", [](std::shared_ptr<Brain> brain, int num_steps) {
        // Simple performance profiling
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < num_steps; ++step) {
            brain->step(step);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        return std::make_tuple(
            duration.count(),  // total milliseconds
            num_steps,        // steps completed
            brain->getTotalSpikeCount(),  // spikes generated
            brain->getAverageFiringRate() // average firing rate
        );
    }, py::arg("brain"), py::arg("num_steps"), "Profile brain performance over multiple steps");

    // Development utilities
    m.def("simulateDevelopment", [](std::shared_ptr<Brain> brain, double total_time) {
        // Simulate brain development over time
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->enableDevelopment(true);
        
        double current_time = 0.0;
        std::vector<std::tuple<double, std::string, float>> timeline;
        
        while (current_time < total_time) {
            agent->updateDevelopment(0.1);
            current_time += 0.1;
            
            timeline.push_back(std::make_tuple(
                current_time,
                std::string("Stage ") + std::to_string(static_cast<int>(brain->getDevelopmentalStage())),
                agent->getNeuromodulationLevel()
            ));
        }
        
        return timeline;
    }, py::arg("brain"), py::arg("total_time"), "Simulate brain development over time");

    // Visualization utilities
    m.def("getVisualData", [](std::shared_ptr<Brain> brain) {
        // Extract visual-ready data from brain state
        std::unordered_map<std::string, std::vector<float>> visual_data;
        
        // Neuron activity by type (simplified)
        std::vector<float> excitatory_activity;
        std::vector<float> inhibitory_activity;
        
        // Get region information
        auto regions = brain->getRegions();
        for (const auto& region : regions) {
            // Add region data for visualization
            auto populations = region->getPopulations();
            for (const auto& pop : populations) {
                // Add activity data based on population type
            }
        }
        
        visual_data["excitatory"] = excitatory_activity;
        visual_data["inhibitory"] = inhibitory_activity;
        visual_data["development_stage"] = std::vector<float>{static_cast<float>(brain->getDevelopmentalStage())};
        
        return visual_data;
    }, py::arg("brain"), "Extract visualization data from brain state");

    // Error handling and validation
    m.def("validateConfig", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        
        // Check for required keys
        if (!config->has("brain.neuron_count")) {
            errors.push_back("Missing required key: brain.neuron_count");
        }
        
        // Validate values
        auto neuron_count = config->get<int>("brain.neuron_count");
        if (neuron_count.has_value() && neuron_count.value() <= 0) {
            errors.push_back("brain.neuron_count must be positive");
        }
        
        auto neuron_density = config->get<double>("brain.synapse_density");
        if (neuron_density.has_value() && (neuron_density.value() < 0.0 || neuron_density.value() > 1.0)) {
            errors.push_back("brain.synapse_density must be between 0.0 and 1.0");
        }
        
        return errors;
    }, py::arg("config"), "Validate configuration and return any errors");

    m.def("safeStep", [](std::shared_ptr<Brain> brain, int step) {
        // Safe brain stepping with error handling
        try {
            brain->step(step);
            return std::make_pair(true, std::string("Step completed successfully"));
        } catch (const std::exception& e) {
            return std::make_pair(false, std::string("Error: ") + e.what());
        }
    }, py::arg("brain"), py::arg("step"), "Safely perform a brain step with error handling");

    // Resource management utilities
    m.def("cleanupBrain", [](std::shared_ptr<Brain> brain) {
        // Clean up brain resources
        brain->reset();
        return std::make_pair(true, "Brain cleaned up successfully");
    }, py::arg("brain"), "Clean up brain resources");

    m.def("saveCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        // Save brain checkpoint
        bool success = brain->save(filepath);
        return std::make_pair(success, std::string("Checkpoint saved to ") + filepath);
    }, py::arg("brain"), py::arg("filepath"), "Save brain checkpoint to file");

    m.def("loadCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        // Load brain checkpoint
        bool success = brain->load(filepath);
        return std::make_pair(success, std::string("Checkpoint loaded from ") + filepath);
    }, py::arg("brain"), py::arg("filepath"), "Load brain checkpoint from file");

    // Advanced agent utilities
    m.def("createAdvancedAgent", [](std::shared_ptr<Brain> brain, bool enable_all_features = true) {
        // Create an agent with all features enabled
        auto agent = std::make_shared<AgentBrain>(brain);
        
        if (enable_all_features) {
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
        }
        
        return agent;
    }, py::arg("brain"), py::arg("enable_all_features") = true, "Create an advanced agent with all features enabled");

    m.def("createLearningAgent", [](std::shared_ptr<Brain> brain, bool curiosity = true, bool development = true) {
        // Create an agent focused on learning and adaptation
        auto agent = std::make_shared<AgentBrain>(brain);
        
        agent->enableRewardModulation(true);
        agent->enableCuriosity(curiosity);
        agent->enableDevelopment(development);
        agent->enableStructuralPlasticity(development);
        
        return agent;
    }, py::arg("brain"), py::arg("curiosity") = true, py::arg("development") = true, "Create a learning-focused agent");

    // Configuration utilities
    m.def("createOptimizedConfig", []() {
        // Create a configuration optimized for fast simulations
        auto config = std::make_shared<Config>();
        
        config->set("brain.neuron_count", 500);
        config->set("brain.synapse_density", 0.05);
        config->set("brain.connection_probability", 0.02);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 1.0);
        config->set("neuromod.curiosity.enable", true);
        
        return config;
    }, "Create an optimized configuration for fast simulations");

    m.def("createLargeScaleConfig", []() {
        // Create a configuration for large-scale simulations
        auto config = std::make_shared<Config>();
        
        config->set("brain.neuron_count", 5000);
        config->set("brain.synapse_density", 0.1);
        config->set("brain.connection_probability", 0.05);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 2.0);
        config->set("neuromod.curiosity.enable", true);
        
        return config;
    }, "Create a large-scale configuration");

    m.def("createMinimalConfig", []() {
        // Create a minimal configuration for testing
        auto config = std::make_shared<Config>();
        
        config->set("brain.neuron_count", 100);
        config->set("brain.synapse_density", 0.01);
        
        return config;
    }, "Create a minimal configuration for testing");

    // Simulation utility functions
    m.def("runEpisode", [](std::shared_ptr<Config> config, std::shared_ptr<SimpleWorld> world, 
                         std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, 
                         int num_steps) {
        // Run a complete simulation episode
        agent->initialize(*world);
        
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto motor_cmd = agent->decodeMotorCommand();
            world->applyMotorCommand(motor_cmd, world->getSimulationTime());
            
            float reward = world->getSensoryPercept().getInternal()[0];
            agent->applyRewardModulation(reward, 0.0);
        }
        
        return std::make_tuple(
            brain->getTotalSpikeCount(),
            agent->getCuriosityLevel(),
            brain->getDevelopmentalStage()
        );
    }, py::arg("config"), py::arg("world"), py::arg("brain"), py::arg("agent"), py::arg("num_steps"),
        "Run a complete simulation episode");
    
    m.def("createEpisode", []() {
        // Factory function for creating a complete episode setup
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        
        return std::make_tuple(config, brain, agent, world);
    }, "Create a complete episode setup");

    // Statistics and analysis utilities
    m.def("analyzeActivity", [](std::shared_ptr<Brain> brain) {
        std::unordered_map<std::string, double> stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["avg_firing_rate"] = brain->getAverageFiringRate();
        stats["e_to_i_ratio"] = brain->getExcitationInhibitionRatio();
        
        return stats;
    }, py::arg("brain"), "Analyze brain activity and return statistics");

    m.def("profilePerformance", [](std::shared_ptr<Brain> brain, int num_steps) {
        // Simple performance profiling
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < num_steps; ++step) {
            brain->step(step);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        return std::make_tuple(
            duration.count(),  // total milliseconds
            num_steps,        // steps completed
            brain->getTotalSpikeCount(),  // spikes generated
            brain->getAverageFiringRate() // average firing rate
        );
    }, py::arg("brain"), py::arg("num_steps"), "Profile brain performance over multiple steps");

    // Development utilities
    m.def("simulateDevelopment", [](std::shared_ptr<Brain> brain, double total_time) {
        // Simulate brain development over time
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->enableDevelopment(true);
        
        double current_time = 0.0;
        std::vector<std::tuple<double, std::string, float>> timeline;
        
        while (current_time < total_time) {
            agent->updateDevelopment(0.1);
            current_time += 0.1;
            
            timeline.push_back(std::make_tuple(
                current_time,
                std::string("Stage ") + std::to_string(static_cast<int>(brain->getDevelopmentalStage())),
                agent->getNeuromodulationLevel()
            ));
        }
        
        return timeline;
    }, py::arg("brain"), py::arg("total_time"), "Simulate brain development over time");

    // Visualization utilities
    m.def("getVisualData", [](std::shared_ptr<Brain> brain) {
        // Extract visual-ready data from brain state
        std::unordered_map<std::string, std::vector<float>> visual_data;
        
        // Neuron activity by type (simplified)
        std::vector<float> excitatory_activity;
        std::vector<float> inhibitory_activity;
        
        // Get region information
        auto regions = brain->getRegions();
        for (const auto& region : regions) {
            // Add region data for visualization
            auto populations = region->getPopulations();
            for (const auto& pop : populations) {
                // Add activity data based on population type
            }
        }
        
        visual_data["excitatory"] = excitatory_activity;
        visual_data["inhibitory"] = inhibitory_activity;
        visual_data["development_stage"] = std::vector<float>{static_cast<float>(brain->getDevelopmentalStage())};
        
        return visual_data;
    }, py::arg("brain"), "Extract visualization data from brain state");

    // Error handling and validation
    m.def("validateConfig", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        
        // Check for required keys
        if (!config->has("brain.neuron_count")) {
            errors.push_back("Missing required key: brain.neuron_count");
        }
        
        // Validate values
        auto neuron_count = config->get<int>("brain.neuron_count");
        if (neuron_count.has_value() && neuron_count.value() <= 0) {
            errors.push_back("brain.neuron_count must be positive");
        }
        
        auto neuron_density = config->get<double>("brain.synapse_density");
        if (neuron_density.has_value() && (neuron_density.value() < 0.0 || neuron_density.value() > 1.0)) {
            errors.push_back("brain.synapse_density must be between 0.0 and 1.0");
        }
        
        return errors;
    }, py::arg("config"), "Validate configuration and return any errors");

    m.def("safeStep", [](std::shared_ptr<Brain> brain, int step) {
        // Safe brain stepping with error handling
        try {
            brain->step(step);
            return std::make_pair(true, std::string("Step completed successfully"));
        } catch (const std::exception& e) {
            return std::make_pair(false, std::string("Error: ") + e.what());
        }
    }, py::arg("brain"), py::arg("step"), "Safely perform a brain step with error handling");

    // Resource management utilities
    m.def("cleanupBrain", [](std::shared_ptr<Brain> brain) {
        // Clean up brain resources
        brain->reset();
        return std::make_pair(true, "Brain cleaned up successfully");
    }, py::arg("brain"), "Clean up brain resources");

    m.def("saveCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        // Save brain checkpoint
        bool success = brain->save(filepath);
        return std::make_pair(success, std::string("Checkpoint saved to ") + filepath);
    }, py::arg("brain"), py::arg("filepath"), "Save brain checkpoint to file");

    m.def("loadCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        // Load brain checkpoint
        bool success = brain->load(filepath);
        return std::make_pair(success, std::string("Checkpoint loaded from ") + filepath);
    }, py::arg("brain"), py::arg("filepath"), "Load brain checkpoint from file");

    // Advanced agent utilities
    m.def("createAdvancedAgent", [](std::shared_ptr<Brain> brain, bool enable_all_features = true) {
        // Create an agent with all features enabled
        auto agent = std::make_shared<AgentBrain>(brain);
        
        if (enable_all_features) {
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
        }
        
        return agent;
    }, py::arg("brain"), py::arg("enable_all_features") = true, "Create an advanced agent with all features enabled");

    m.def("createLearningAgent", [](std::shared_ptr<Brain> brain, bool curiosity = true, bool development = true) {
        // Create an agent focused on learning and adaptation
        auto agent = std::make_shared<AgentBrain>(brain);
        
        agent->enableRewardModulation(true);
        agent->enableCuriosity(curiosity);
        agent->enableDevelopment(development);
        agent->enableStructuralPlasticity(development);
        
        return agent;
    }, py::arg("brain"), py::arg("curiosity") = true, py::arg("development") = true, "Create a learning-focused agent");

    // Configuration utilities
    m.def("createOptimizedConfig", []() {
        // Create a configuration optimized for fast simulations
        auto config = std::make_shared<Config>();
        
        config->set("brain.neuron_count", 500);
        config->set("brain.synapse_density", 0.05);
        config->set("brain.connection_probability", 0.02);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 1.0);
        config->set("neuromod.curiosity.enable", true);
        
        return config;
    }, "Create an optimized configuration for fast simulations");

    m.def("createLargeScaleConfig", []() {
        // Create a configuration for large-scale simulations
        auto config = std::make_shared<Config>();
        
        config->set("brain.neuron_count", 5000);
        config->set("brain.synapse_density", 0.1);
        config->set("brain.connection_probability", 0.05);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 2.0);
        config->set("neuromod.curiosity.enable", true);
        
        return config;
    }, "Create a large-scale configuration");

    m.def("createMinimalConfig", []() {
        // Create a minimal configuration for testing
        auto config = std::make_shared<Config>();
        
        config->set("brain.neuron_count", 100);
        config->set("brain.synapse_density", 0.01);
        
        return config;
    }, "Create a minimal configuration for testing");

    // Python-specific utility functions
    m.def("createPythonBrain", [](py::kwargs kwargs) {
        // Python-friendly brain creation with keyword arguments
        auto config = std::make_shared<Config>();
        
        if (kwargs.contains("neuron_count")) {
            config->set("brain.neuron_count", kwargs["neuron_count"].cast<int>());
        }
        if (kwargs.contains("synapse_density")) {
            config->set("brain.synapse_density", kwargs["synapse_density"].cast<double>());
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        return brain;
    }, py::arg("kwargs"), "Python-friendly brain creation with keyword arguments");

    m.def("createPythonWorld", [](py::kwargs kwargs) {
        // Python-friendly world creation
        auto world = std::make_shared<SimpleWorld>();
        
        if (kwargs.contains("width")) {
            world->configure(
                kwargs["width"].cast<size_t>(),
                kwargs.contains("height") ? kwargs["height"].cast<size_t>() : 20,
                kwargs.contains("vision_width") ? kwargs["vision_width"].cast<size_t>() : 8,
                kwargs.contains("vision_height") ? kwargs["vision_height"].cast<size_t>() : 8
            );
        }
        
        return world;
    }, py::arg("kwargs"), "Python-friendly world creation with keyword arguments");

    // Batch processing utilities
    m.def("batchSteps", [](std::shared_ptr<Brain> brain, int num_steps) {
        // Perform multiple steps efficiently
        std::vector<int> spike_counts;
        
        for (int step = 0; step < num_steps; ++step) {
            brain->step(step);
            spike_counts.push_back(brain->getTotalSpikeCount());
        }
        
        return spike_counts;
    }, py::arg("brain"), py::arg("num_steps"), "Perform multiple steps efficiently");

    m.def("batchSimulations", [](std::shared_ptr<Config> config, int num_simulations, int steps_per_simulation) {
        // Run multiple simulation episodes
        std::vector<std::unordered_map<std::string, double>> results;
        
        for (int sim = 0; sim < num_simulations; ++sim) {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            for (int step = 0; step < steps_per_simulation; ++step) {
                brain->step(step);
            }
            
            std::unordered_map<std::string, double> sim_result;
            sim_result["simulation_id"] = sim;
            sim_result["total_spikes"] = brain->getTotalSpikeCount();
            sim_result["avg_firing_rate"] = brain->getAverageFiringRate();
            
            results.push_back(sim_result);
        }
        
        return results;
    }, py::arg("config"), py::arg("num_simulations"), py::arg("steps_per_simulation"), "Run multiple simulation episodes");

    // Debugging and profiling utilities
    m.def("debugBrain", [](std::shared_ptr<Brain> brain) {
        // Debug brain state
        std::unordered_map<std::string, std::string> debug_info;
        
        debug_info["total_neurons"] = std::to_string(brain->getTotalNeuronCount());
        debug_info["total_synapses"] = std::to_string(brain->getTotalSynapseCount());
        debug_info["active_neurons"] = std::to_string(brain->getActiveNeuronCount());
        debug_info["firing_neurons"] = std::to_string(brain->getFiringNeuronCount());
        debug_info["total_spikes"] = std::to_string(brain->getTotalSpikeCount());
        debug_info["avg_firing_rate"] = std::to_string(brain->getAverageFiringRate());
        debug_info["development_stage"] = std::to_string(static_cast<int>(brain->getDevelopmentalStage()));
        debug_info["config_summary"] = brain->getConfig()->summary();
        
        return debug_info;
    }, py::arg("brain"), "Get debug information about brain state");

    m.def("profileStep", [](std::shared_ptr<Brain> brain, int step, int iterations = 100) {
        // Profile individual step performance
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < iterations; ++i) {
            brain->step(step);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        
        return std::make_tuple(
            duration.count(),  // total microseconds
            iterations,        // number of iterations
            duration.count() / iterations  // average time per step in microseconds
        );
    }, py::arg("brain"), py::arg("step"), py::arg("iterations") = 100, "Profile individual step performance");

    // Educational and demonstration utilities
    m.def("createEducationalDemo", []() {
        // Create an educational demonstration setup
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 200);
        config->set("brain.synapse_density", 0.02);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width=10, height=10, visionWidth=4, visionHeight=4);
        
        agent->initialize(*world);
        
        return std::make_tuple(config, brain, agent, world);
    }, "Create an educational demonstration setup");

    m.def("createResearchDemo", []() {
        // Create a research-grade demo with more complexity
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.05);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width=20, height=20, visionWidth=8, visionHeight=8);
        
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableDevelopment(true);
        
        return std::make_tuple(config, brain, agent, world);
    }, "Create a research-grade demo with advanced features");

    // Comparison and analysis utilities
    m.def("compareBrains", [](std::vector<std::shared_ptr<Brain>> brains, int steps) {
        // Compare multiple brains over the same simulation
        std::vector<std::unordered_map<std::string, double>> brain_stats;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            auto& brain = brains[i];
            brain->initialize();
            
            for (int step = 0; step < steps; ++step) {
                brain->step(step);
            }
            
            std::unordered_map<std::string, double> stats;
            stats["brain_id"] = i;
            stats["total_spikes"] = brain->getTotalSpikeCount();
            stats["avg_firing_rate"] = brain->getAverageFiringRate();
            stats["final_development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
            
            brain_stats.push_back(stats);
        }
        
        return brain_stats;
    }, py::arg("brains"), py::arg("steps"), "Compare multiple brains over the same simulation");

    m.def("analyzeTrend", [](std::vector<int> values) {
        // Analyze trend in time series data
        if (values.empty()) {
            return std::make_tuple(0.0, 0.0, std::string("empty"));
        }
        
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        double mean = sum / values.size();
        
        double sum_squared = 0.0;
        for (double value : values) {
            sum_squared += std::pow(value - mean, 2);
        }
        double std_dev = std::sqrt(sum_squared / values.size());
        
        // Determine trend
        std::string trend;
        if (values.size() >= 3) {
            double first_half = std::accumulate(values.begin(), values.begin() + values.size()/2, 0.0) / (values.size()/2);
            double second_half = std::accumulate(values.begin() + values.size()/2, values.end(), 0.0) / (values.size() - values.size()/2);
            
            if (second_half > first_half * 1.1) trend = "increasing";
            else if (second_half < first_half * 0.9) trend = "decreasing";
            else trend = "stable";
        } else {
            trend = "insufficient_data";
        }
        
        return std::make_tuple(mean, std_dev, trend);
    }, py::arg("values"), "Analyze trend in time series data");

    // Save and export utilities
    m.def("exportToJSON", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        // Export brain data to JSON format (simplified)
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "{\n";
        file << "  \"total_neurons\": " << brain->getTotalNeuronCount() << ",\n";
        file << "  \"total_synapses\": " << brain->getTotalSynapseCount() << ",\n";
        file << "  \"firing_neurons\": " << brain->getFiringNeuronCount() << ",\n";
        file << "  \"total_spikes\": " << brain->getTotalSpikeCount() << ",\n";
        file << "  \"avg_firing_rate\": " << brain->getAverageFiringRate() << ",\n";
        file << "  \"development_stage\": " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
        file << "}\n";
        
        return true;
    }, py::arg("brain"), py::arg("filepath"), "Export brain data to JSON format");

    m.def("exportToCSV", [](std::vector<std::shared_ptr<Brain>> brains, const std::string& filepath) {
        // Export multiple brains to CSV format
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "brain_id,total_neurons,total_synapses,firing_neurons,total_spikes,avg_firing_rate,development_stage\n";
        
        for (size_t i = 0; i < brains.size(); ++i) {
            auto& brain = brains[i];
            brain->initialize();
            
            file << i << "," 
                 << brain->getTotalNeuronCount() << "," 
                 << brain->getTotalSynapseCount() << "," 
                 << brain->getFiringNeuronCount() << "," 
                 << brain->getTotalSpikeCount() << "," 
                 << brain->getAverageFiringRate() << "," 
                 << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
        }
        
        return true;
    }, py::arg("brains"), py::arg("filepath"), "Export multiple brains to CSV format");

    // Version and metadata utilities
    m.def("getVersionInfo", []() {
        // Get version information
        return std::make_tuple(
            "NLM",
            "0.1.0",
            "Phase 6: Final Integration",
            "2026-09-15"
        );
    }, "Get NLM version information");

    m.def("getLicenseInfo", []() {
        // Get license information
        return std::string("MIT License\n") +
               "Copyright (c) 2026 NLM Authors\n" +
               "Permission is hereby granted, free of charge, to any person obtaining a copy\n" +
               "of this software and associated documentation files, to deal in the Software\n" +
               "without restriction, including without limitation the rights to use, copy,\n" +
               "modify, merge, publish, distribute, sublicense, and/or sell copies of the\n" +
               "Software, and to permit persons to whom the Software is furnished to do so,\n" +
               "subject to the following conditions:\n\n" +
               "The above copyright notice and this permission notice shall be included in all\n" +
               "copies or substantial portions of the Software.\n\n" +
               "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n" +
               "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n" +
               "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n" +
               "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n" +
               "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n" +
               "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n" +
               "SOFTWARE.";
    }, "Get NLM license information");

    // Help and documentation utilities
    m.def("getDocumentation", []() {
        // Get documentation information
        return std::vector<std::string>{
            "NLM (Neural Learning Machine) - Python Bindings",
            "============================",
            "",
            "The NLM library provides Python bindings for the NLM brain simulation framework.",
            "",
            "Key Classes:",
            "  - Brain: Central neural simulation brain class",
            "  - Config: Configuration class for NLM system",
            "  - AgentBrain: Agent brain interface connecting brain to world",
            "  - SimpleWorld: Simple 2D world for NLM simulation",
            "  - SensoryInput: Base class for sensory input",
            "  - Action: Action representation for motor output",
            "",
            "Key Functions:",
            "  - createDefaultConfig(): Create a default configuration",
            "  - createBrain(config): Create a new brain with configuration",
            "  - createSimpleWorld(): Create a new simple world",
            "  - createAgentBrain(brain): Create a new agent brain interface",
            "",
            "Getting Started:",
            "  1. Import pynlm",
            "  2. Create a config: config = pynlm.createDefaultConfig()",
            "  3. Create a brain: brain = pynlm.createBrain(config)",
            "  4. Initialize: brain.initialize()",
            "  5. Simulate: for step in range(100): brain.step(step)",
            "",
            "Advanced Usage:",
            "  - Use context managers for resource management",
            "  - Create complete episode setups with createEpisode()",
            "  - Analyze brain activity with analyzeActivity()",
            "  - Profile performance with profilePerformance()",
            "",
            "For more information, see:",
            "  - easy_usage.md: Beginner-friendly usage guide",
            "  - HOW_TO_USE.md: Comprehensive usage documentation",
            "  - docs/ARCHITECTURE.md: System architecture",
            "  - docs/SCIENCE.md: Scientific background"
        };
    }, "Get NLM documentation");

    m.def("getExamples", []() {
        // Get example code snippets
        return std::vector<std::string>{
            "# Basic Brain Simulation",
            "import pynlm",
            "",
            "config = pynlm.createDefaultConfig()",
            "brain = pynlm.createBrain(config)",
            "brain.initialize()",
            "",
            "for i in range(100):",
            "    brain.step(i)",
            "",
            "# Complete Agent Simulation",
            "import pynlm",
            "",
            "def run_episode():",
            "    config = pynlm.createDefaultConfig()",
            "    brain = pynlm.createBrain(config)",
            "    brain.initialize()",
            "    ",
            "    agent = pynlm.createAgentBrain(brain)",
            "    world = pynlm.createSimpleWorld()",
            "    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)",
            "    world.reset()",
            "    ",
            "    agent.initialize(world)",
            "    agent.enableRewardModulation(True)",
            "    agent.enableCuriosity(True)",
            "",
            "    for step in range(100):",
            "        world.update(0.1)",
            "        agent.processSensoryInput(world.getSensoryPercept())",
            "        brain.step(step)",
            "        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())",
            "",
            "    return brain.getTotalSpikeCount()",
            "",
            "# Using Context Managers",
            "import pynlm",
            "",
            "# Safe resource management with context manager",
            "with pynlm.BrainContext() as brain:",
            "    for step in range(100):",
            "        brain.step(step)",
            "    ",
            "print(f\"Simulation complete: {brain.getTotalSpikeCount()} spikes\")",
            "",
            "# Configuration Management",
            "import pynlm",
            "",
            "config = pynlm.createDefaultConfig()",
            "config.set(\"brain.neuron_count\", 1000)",
            "config.set(\"brain.synapse_density\", 0.05)",
            "",
            "# Save configuration",
            "config.saveToFile(\"my_config.txt\")",
            "",
            "# Load configuration",
            "new_config = pynlm.createDefaultConfig()",
            "new_config.loadFromFile(\"my_config.txt\")",
            "",
            "# File I/O Example",
            "import pynlm",
            "import tempfile",
            "",
            "with tempfile.NamedTemporaryFile(mode='w', suffix='.txt') as f:",
            "    config = pynlm.createDefaultConfig()",
            "    config.set(\"test.value\", 42)",
            "    config.saveToFile(f.name)",
            "    ",
            "    new_config = pynlm.createDefaultConfig()",
            "    new_config.loadFromFile(f.name)",
            "    ",
            "    print(f\"Loaded value: {new_config.get(\"test.value\").value}\")"
        };
    }, "Get example code snippets");

    m.def("getSupportedFeatures", []() {
        // Get list of supported features
        return std::vector<std::string>{
            "Basic Brain Operations",
            "  - Brain creation and initialization",
            "  - Simulation steps",
            "  - State management (reset, save, load)",
            "  - Statistics and analysis",
            "",
            "Configuration Management",
            "  - Key/value configuration",
            "  - File I/O (simple format)",
            "  - Command line argument parsing",
            "  - Configuration validation",
            "",
            "Agent System",
            "  - Sensory processing",
            "  - Motor command decoding",
            "  - Reward modulation",
            "  - Development system",
            "  - Curiosity and exploration",
            "",
            "World Simulation",
            "  - Simple 2D world",
            "  - Sensory perception",
            "  - Object interaction",
            "  - Motor commands",
            "",
            "Advanced Features",
            "  - Context managers for resource management",
            "  - Error handling and validation",
            "  - Performance profiling",
            "  - Batch processing",
            "  - Data export (JSON, CSV)",
            "  - Development simulation",
            "",
            "Utility Functions",
            "  - Episode management",
            "  - Configuration templates",
            "  - Statistics and analysis",
            "  - Debugging tools",
            "  - Comparison utilities",
            "",
            "Performance Optimizations",
            "  - Thread-safe configuration",
            "  - Efficient O(1) lookups",
            "  - Memory pool support",
            "  - SIMD optimizations",
            "  - Parallel processing",
            "",
            "Developer Tools",
            "  - Debugging and profiling",
            "  - Validation utilities",
            "  - Resource management",
            "  - Error handling",
            "  - Type hints and docstrings",
            "",
            "User-Friendly Features",
            "  - Beginner-friendly examples",
            "  - Comprehensive documentation",
            "  - Interactive tutorials",
            "  - Migration guides",
            "  - Community support",
            "",
            "Research Capabilities",
            "  - Advanced neuromodulation",
            "  - Developmental stages",
            "  - Learning algorithms",
            "  - Statistical analysis",
            "  - Visualization tools",
            "  - Experiment management"
        };
    }, "Get list of supported features");

    m.def("getQuickStart", []() {
        // Get quick start guide
        return std::vector<std::string>{
            "# Quick Start Guide for NLM Python Bindings",
            "",
            "## Installation",
            "pip install pynlm",
            "",
            "## First Steps",
            "```python",
            "import pynlm",
            "",
            "# Create and initialize a brain",
            "config = pynlm.createDefaultConfig()",
            "brain = pynlm.createBrain(config)",
            "brain.initialize()",
            "```",
            "",
            "## Basic Simulation",
            "```python",
            "for i in range(100):",
            "    brain.step(i)",
            "    if i % 10 == 0:",
            "        print(f\"Step {i}: {brain.getFiringNeuronCount()} neurons firing\")",
            "```",
            "",
            "## With World and Agent",
            "```python",
            "# Create world and agent",
            "world = pynlm.createSimpleWorld()",
            "world.configure(width=10, height=10, visionWidth=4, visionHeight=4)",
            "agent = pynlm.createAgentBrain(brain)",
            "agent.initialize(world)",
            "",
            "# Run simulation",
            "for step in range(50):",
            "    world.update(0.1)",
            "    agent.processSensoryInput(world.getSensoryPercept())",
            "    brain.step(step)",
            "    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())",
            "```",
            "",
            "## Using Context Manager",
            "```python",
            "# Safe resource management",
            "with pynlm.BrainContext() as brain:",
            "    for i in range(100):",
            "        brain.step(i)",
            "```",
            "",
            "## Save and Load",
            "```python",
            "# Save brain state",
            "brain.save(\"checkpoint.bin\")",
            "",
            "# Load brain state",
            "brain.load(\"checkpoint.bin\")",
            "```",
            "",
            "## Get Statistics",
            "```python",
            "print(f\"Total neurons: {brain.getTotalNeuronCount()}\")",
            "print(f\"Total spikes: {brain.getTotalSpikeCount()}\")",
            "print(f\"Average firing rate: {brain.getAverageFiringRate():.2f} Hz\")",
            "print(f\"Development stage: {brain.getDevelopmentalStage().name}\")",
            "```",
            "",
            "## Advanced Features",
            "```python",
            "# Enable learning features",
            "agent.enableRewardModulation(True)",
            "agent.enableCuriosity(True)",
            "agent.enableDevelopment(True)",
            "",
            "# Create episode setup",
            "config, brain, agent, world = pynlm.createEpisode()",
            "",
            "# Analyze activity",
            "stats = pynlm.analyzeActivity(brain)",
            "print(f\"Active neurons: {stats['active_neurons']}/\" + \\
           f\"{stats['total_neurons']}\")",
            "```",
            "",
            "## Next Steps",
            "1. Read easy_usage.md for beginner-friendly examples",
            "2. Read HOW_TO_USE.md for comprehensive documentation",
            "3. Explore the test suite to understand capabilities",
            "4. Check the docs/ directory for architecture and science details",
            "",
            "The NLM library provides a complete brain simulation environment that's",
            "easy to use for beginners while offering advanced features for researchers"
        };
    }, "Get quick start guide");
}

} // namespace nlm
