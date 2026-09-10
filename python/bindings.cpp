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

    // ========== SYSTEM CLASSES ==========

    // SpikeSystem - Event-driven neural computation system
    py::class_<SpikeSystem>(m, "SpikeSystem", R"pbdoc(Spike event processing system)pbdoc")
        .def(py::init<>())
        .def("queueSpike", [](SpikeSystem& self, const SpikeEvent& event) {
            self.queueSpike(event);
        }, py::arg("event"))
        .def("queueSpike", [](SpikeSystem& self, NeuronId neuron, Timestamp timestamp, SimulationStep step) {
            self.queueSpike(neuron, timestamp, step);
        }, py::arg("neuron"), py::arg("timestamp"), py::arg("step"))
        .def("queueDelayedSpike", &SpikeSystem::queueDelayedSpike, py::arg("event"))
        .def("processSpikes", &SpikeSystem::processSpikes, py::arg("currentStep"))
        .def("processDelayedSpikes", &SpikeSystem::processDelayedSpikes,
             py::arg("currentStep"), py::arg("currentTime"))
        .def("registerHandler", &SpikeSystem::registerHandler, py::arg("handler"))
        .def("registerDelayedHandler", &SpikeSystem::registerDelayedHandler, py::arg("handler"))
        .def("getSpikeHistory", &SpikeSystem::getSpikeHistory,
             py::return_value_policy::reference_internal)
        .def("clearHistory", &SpikeSystem::clearHistory)
        .def("getSpikeCount", &SpikeSystem::getSpikeCount)
        .def("getPendingSpikeCount", &SpikeSystem::getPendingSpikeCount)
        .def("getPendingDelayedCount", &SpikeSystem::getPendingDelayedCount)
        .def("getAverageSpikeRate", &SpikeSystem::getAverageSpikeRate)
        .def("getMostActiveNeurons", &SpikeSystem::getMostActiveNeurons, py::arg("count"))
        .def("reset", &SpikeSystem::reset);

    // SimulationClock - Time management for simulation
    py::class_<SimulationClock>(m, "SimulationClock", R"pbdoc(Simulation clock for time management)pbdoc")
        .def(py::init<TimestepDuration>(), py::arg("timestep") = 0.001)
        .def("reset", &SimulationClock::reset)
        .def("advance", &SimulationClock::advance)
        .def("getStep", &SimulationClock::getStep)
        .def("setStep", &SimulationClock::setStep, py::arg("step"))
        .def("getTime", &SimulationClock::getTime)
        .def("setTime", &SimulationClock::setTime, py::arg("time"))
        .def("getTimestep", &SimulationClock::getTimestep)
        .def("setTimestep", &SimulationClock::setTimestep, py::arg("dt"))
        .def("isFirstStep", &SimulationClock::isFirstStep)
        .def("isLastStep", &SimulationClock::isLastStep, py::arg("totalSteps"))
        .def("stepToTime", &SimulationClock::stepToTime, py::arg("step"))
        .def("timeToStep", &SimulationClock::timeToStep, py::arg("time"))
        .def("advanceBy", &SimulationClock::advanceBy, py::arg("steps"))
        .def("timeAtFutureStep", &SimulationClock::timeAtFutureStep, py::arg("stepsAhead"))
        .def("getRealTimeElapsed", &SimulationClock::getRealTimeElapsed)
        .def("setRealTimeStart", &SimulationClock::setRealTimeStart);

    // RandomGenerator - Deterministic random number generator
    py::class_<RandomGenerator>(m, "RandomGenerator", R"pbdoc(Random number generator)pbdoc")
        .def(py::init<uint64_t>(), py::arg("seed"))
        .def(py::init<>())
        .def("seed", &RandomGenerator::seed, py::arg("seed"))
        .def("getSeed", &RandomGenerator::getSeed)
        .def("uniformInt", [](RandomGenerator& self, uint32_t min, uint32_t max) {
            return self.uniformInt(min, max);
        }, py::arg("min"), py::arg("max"))
        .def("uniformInt", [](RandomGenerator& self, uint64_t min, uint64_t max) {
            return self.uniformInt(min, max);
        }, py::arg("min"), py::arg("max"))
        .def("uniformReal", [](RandomGenerator& self, double min, double max) {
            return self.uniformReal(min, max);
        }, py::arg("min"), py::arg("max"))
        .def("uniformReal", [](RandomGenerator& self, float min, float max) {
            return self.uniformReal(min, max);
        }, py::arg("min"), py::arg("max"))
        .def("normal", [](RandomGenerator& self, double mean, double stddev) {
            return self.normal(mean, stddev);
        }, py::arg("mean"), py::arg("stddev"))
        .def("normal", [](RandomGenerator& self, float mean, float stddev) {
            return self.normal(mean, stddev);
        }, py::arg("mean"), py::arg("stddev"))
        .def("bernoulli", &RandomGenerator::bernoulli, py::arg("p") = 0.5)
        .def("poisson", &RandomGenerator::poisson, py::arg("lambda"))
        .def("exponential", &RandomGenerator::exponential, py::arg("lambda"))
        .def("randomBool", &RandomGenerator::randomBool)
        .def("randomBit", &RandomGenerator::randomBit)
        .def("engine", &RandomGenerator::engine,
             py::return_value_policy::reference_internal);

    // Logger - Logging system
    py::class_<LogEntry>(m, "LogEntry", R"pbdoc(Log entry structure)pbdoc")
        .def_readwrite("level", &LogEntry::level)
        .def_readwrite("message", &LogEntry::message)
        .def_readwrite("file", &LogEntry::file)
        .def_readwrite("line", &LogEntry::line)
        .def_readwrite("function", &LogEntry::function);

    py::class_<Logger>(m, "Logger", R"pbdoc(Main logger class)pbdoc")
        .def(py::init<>())
        .def("addLogger", &Logger::addLogger, py::arg("logger"))
        .def("clearLoggers", &Logger::clearLoggers)
        .def("setLevel", &Logger::setLevel, py::arg("level"))
        .def("log", &Logger::log,
             py::arg("level"), py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "")
        .def("debug", &Logger::debug,
             py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "")
        .def("info", &Logger::info,
             py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "")
        .def("warning", &Logger::warning,
             py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "")
        .def("error", &Logger::error,
             py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "")
        .def("critical", &Logger::critical,
             py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "")
        .def("isEnabled", &Logger::isEnabled, py::arg("level"))
        .def("getGlobal", &Logger::getGlobal,
             py::return_value_policy::reference_internal)
        .def("setGlobal", &Logger::setGlobal, py::arg("logger"));

    // ========== MEMORY SYSTEMS ==========

    // NeuralWorkingMemory - Transient active information storage
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Working memory)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"))
        .def("store", &NeuralWorkingMemory::store, py::arg("neuron"), py::arg("value"))
        .def("retrieve", &NeuralWorkingMemory::retrieve, py::arg("neuron"))
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"))
        .def("clear", &NeuralWorkingMemory::clear)
        .def("getCapacity", &NeuralWorkingMemory::getCapacity)
        .def("getCurrentSize", &NeuralWorkingMemory::getCurrentSize)
        .def("decay", &NeuralWorkingMemory::decay, py::arg("decayRate"));

    // NeuralEpisodicMemory - Experience storage
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Episodic memory)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"))
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"))
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar,
             py::arg("sensoryPattern"), py::arg("maxResults") = 5)
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal,
             py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10)
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation,
             py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5)
        .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction,
             py::arg("action"), py::arg("maxResults") = 5)
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"))
        .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance,
             py::arg("episodeId"), py::arg("relevanceDelta"))
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode, py::arg("index"))
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount)
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"))
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward)
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevanceThreshold"))
        .def("clear", &NeuralEpisodicMemory::clear)
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay, py::arg("enable"))
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled)
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("count"))
        .def("replaySequence", &NeuralEpisodicMemory::replaySequence, py::arg("episodeIds"));

    // NeuralAssociativeMemory - Pattern associations
    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Associative memory)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"))
        .def("associate", &NeuralAssociativeMemory::associate,
             py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f)
        .def("associateFromExperience", &NeuralAssociativeMemory::associateFromExperience, py::arg("episode"))
        .def("retrieve", &NeuralAssociativeMemory::retrieve,
             py::arg("queryPattern"), py::arg("maxResults") = 5)
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength,
             py::arg("patternA"), py::arg("patternB"))
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation,
             py::arg("patternA"), py::arg("patternB"), py::arg("delta"))
        .def("spreadActivation", &NeuralAssociativeMemory::spreadActivation,
             py::arg("cuePattern"), py::arg("steps") = 2)
        .def("clear", &NeuralAssociativeMemory::clear)
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount);

    // ========== PLASTICITY ==========

    // SynapseHandle - Generic synapse handle
    py::class_<SynapseHandle>(m, "SynapseHandle", R"pbdoc(Synapse handle)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseHandle::value)
        .def("index", &SynapseHandle::index)
        .def("__eq__", &SynapseHandle::operator==)
        .def("__ne__", &SynapseHandle::operator!=)
        .def("__hash__", [](const SynapseHandle& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseHandle& id) {
            return "<SynapseHandle: " + std::to_string(id.value) + ">";
        });

    // PlasticityRule - Abstract base class for plasticity rules
    py::class_<PlasticityRule>(m, "PlasticityRule", py::module_local(), R"pbdoc(Abstract plasticity rule)pbdoc")
        .def("isEnabled", &PlasticityRule::isEnabled)
        .def("setEnabled", &PlasticityRule::setEnabled, py::arg("enabled"));

    // Hebbian - Hebbian learning implementation
    py::class_<Hebbian, PlasticityRule>(m, "Hebbian", R"pbdoc(Hebbian plasticity rule)pbdoc")
        .def(py::init<>())
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"))
        .def("getLearningRate", &Hebbian::getLearningRate)
        .def("setMaxWeight", &Hebbian::setMaxWeight, py::arg("maxWeight"))
        .def("getMaxWeight", &Hebbian::getMaxWeight);

    // StructuralPlasticity - Synapse and neuron formation/removal
    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity)pbdoc")
        .def(py::init<>())
        .def("createSynapse", &StructuralPlasticity::createSynapse,
             py::arg("brain"), py::arg("source"), py::arg("destination"), py::arg("weight"))
        .def("removeSynapse", &StructuralPlasticity::removeSynapse,
             py::arg("brain"), py::arg("synapse"))
        .def("createNeuron", &StructuralPlasticity::createNeuron,
             py::arg("brain"), py::arg("type"))
        .def("removeNeuron", &StructuralPlasticity::removeNeuron,
             py::arg("brain"), py::arg("neuron"))
        .def("getSynaptogenesisRate", &StructuralPlasticity::getSynaptogenesisRate)
        .def("setSynaptogenesisRate", &StructuralPlasticity::setSynaptogenesisRate, py::arg("rate"))
        .def("getPruningRate", &StructuralPlasticity::getPruningRate)
        .def("setPruningRate", &StructuralPlasticity::setPruningRate, py::arg("rate"))
        .def("update", &StructuralPlasticity::update, py::arg("brain"), py::arg("rng"));

    // ========== NEUROMODULATION ==========

    // Neuromodulator - Abstract base class for neuromodulatory signals
    py::class_<Neuromodulator>(m, "Neuromodulator", py::module_local(), R"pbdoc(Abstract neuromodulator)pbdoc")
        .def("getName", &Neuromodulator::getName)
        .def("getLevel", &Neuromodulator::getLevel)
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor)
        .def("update", &Neuromodulator::update, py::arg("dt"));

    // Dopamine - Reward and reinforcement learning signal
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"))
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"));

    // Curiosity - Exploration motivation
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"))
        .def("getLevel", &Curiosity::getLevel)
        .def("update", &Curiosity::update,
             py::arg("novelty"), py::arg("predictionError"), py::arg("dt"))
        .def("getExplorationDrive", &Curiosity::getExplorationDrive)
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"))
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"))
        .def("reset", &Curiosity::reset);

    // Novelty - Novelty detection
    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty neuromodulator)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"))
        .def("getLevel", &Novelty::getLevel)
        .def("setLevel", &Novelty::setLevel, py::arg("level"))
        .def("detectNovelty", [](Novelty& self, const std::vector<float>& currentPattern,
                               const std::vector<float>& previousPattern) {
            self.detectNovelty(currentPattern, previousPattern);
        }, py::arg("currentPattern"), py::arg("previousPattern"))
        .def("detectNovelty", [](Novelty& self, const class Observation& observation,
                               const class Observation& previousObservation) {
            self.detectNovelty(observation, previousObservation);
        }, py::arg("observation"), py::arg("previousObservation"))
        .def("update", &Novelty::update, py::arg("dt"))
        .def("getHistory", &Novelty::getHistory,
             py::return_value_policy::reference_internal)
        .def("clearHistory", &Novelty::clearHistory);

    // PredictionError - Prediction error signal
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"))
        .def("getError", &PredictionError::getError)
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"))
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"))
        .def("getHistory", &PredictionError::getHistory,
             py::return_value_policy::reference_internal)
        .def("clearHistory", &PredictionError::clearHistory)
        .def("getMagnitude", &PredictionError::getMagnitude);

    // Reward - Reward signal
    py::class_<Reward>(m, "Reward", R"pbdoc(Reward signal)pbdoc")
        .def(py::init<>())
        .def("getValue", &Reward::getValue)
        .def("setValue", &Reward::setValue, py::arg("value"))
        .def("add", &Reward::add, py::arg("delta"))
        .def("reset", &Reward::reset)
        .def("computeReward", &Reward::computeReward, py::arg("observation"))
        .def("getHistory", &Reward::getHistory,
             py::return_value_policy::reference_internal)
        .def("clearHistory", &Reward::clearHistory);

    // ========== PREDICTION ==========

    // PredictionSystem - Predicts future sensory states
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system)pbdoc")
        .def(py::init<>())
        .def("predictNextState", &PredictionSystem::predictNextState, py::arg("currentState"))
        .def("updatePredictions", &PredictionSystem::updatePredictions,
             py::arg("predicted"), py::arg("actual"))
        .def("getPredictionError", &PredictionSystem::getPredictionError)
        .def("getConfidence", &PredictionSystem::getConfidence)
        .def("getErrorHistory", &PredictionSystem::getErrorHistory,
             py::return_value_policy::reference_internal)
        .def("clearHistory", &PredictionSystem::clearHistory)
        .def("train", &PredictionSystem::train, py::arg("observation"));

    // NeuralPrediction - Temporal prediction using spiking substrate
    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Neural prediction)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPrediction::initialize, py::arg("brain"))
        .def("recordSensoryState", &NeuralPrediction::recordSensoryState,
             py::arg("sensoryState"), py::arg("currentStep"))
        .def("generatePrediction", &NeuralPrediction::generatePrediction, py::arg("currentStep"))
        .def("updateWithObservation", &NeuralPrediction::updateWithObservation,
             py::arg("actualState"), py::arg("currentStep"))
        .def("predictActionConsequence", &NeuralPrediction::predictActionConsequence,
             py::arg("action"), py::arg("currentState"))
        .def("getPredictionError", &NeuralPrediction::getPredictionError)
        .def("getPredictionConfidence", &NeuralPrediction::getPredictionConfidence)
        .def("predictMultipleSteps", &NeuralPrediction::predictMultipleSteps,
             py::arg("currentStep"), py::arg("numSteps"))
        .def("recordAction", &NeuralPrediction::recordAction,
             py::arg("action"), py::arg("step"))
        .def("getErrorHistory", &NeuralPrediction::getErrorHistory,
             py::return_value_policy::reference_internal)
        .def("clearHistory", &NeuralPrediction::clearHistory)
        .def("enableTemporalPrediction", &NeuralPrediction::enableTemporalPrediction, py::arg("enable"))
        .def("enableActionConsequencePrediction", &NeuralPrediction::enableActionConsequencePrediction, py::arg("enable"))
        .def("setSequenceMemorySize", &NeuralPrediction::setSequenceMemorySize, py::arg("size"))
        .def("setPredictionHorizon", &NeuralPrediction::setPredictionHorizon, py::arg("steps"))
        .def("getPredictionNeurons", &NeuralPrediction::getPredictionNeurons)
        .def("getSequenceNeurons", &NeuralPrediction::getSequenceNeurons);

    // ========== COGNITION ==========

    // NeuralPlanner - Plans multi-step actions
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"))
        .def("planAction", &NeuralPlanner::planAction,
             py::arg("currentState"), py::arg("targetReward") = 0.5f)
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence,
             py::arg("actions"), py::arg("startState"))
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth)
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"))
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence)
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality,
             py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"))
        .def("clearCache", &NeuralPlanner::clearCache)
        .def("setActionQuality", &NeuralPlanner::setActionQuality, py::arg("action"), py::arg("quality"))
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal)
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"))
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful);

    // ConceptFormation - Discovers recurring patterns
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"))
        .def("presentExperience", &ConceptFormation::presentExperience,
             py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"))
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f)
        .def("getConcepts", &ConceptFormation::getConcepts,
             py::return_value_policy::reference_internal)
        .def("getConcept", &ConceptFormation::getConcept, py::arg("conceptId"))
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype, py::arg("conceptId"))
        .def("getConceptInstances", &ConceptFormation::getConceptInstances, py::arg("conceptId"))
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("conceptId"))
        .def("getConceptCount", &ConceptFormation::getConceptCount)
        .def("mergeConcepts", &ConceptFormation::mergeConcepts, py::arg("conceptA"), py::arg("conceptB"))
        .def("updateConcept", &ConceptFormation::updateConcept,
             py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"))
        .def("computeSimilarity", &ConceptFormation::computeSimilarity,
             py::arg("a"), py::arg("b"))
        .def("isNovel", &ConceptFormation::isNovel,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f)
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern, py::arg("pattern"))
        .def("clear", &ConceptFormation::clear)
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility, py::arg("conceptId"))
        .def("setFormationThreshold", &ConceptFormation::setFormationThreshold, py::arg("t"))
        .def("setStabilityWindow", &ConceptFormation::setStabilityWindow, py::arg("w"));

    // AttentionalSelection - Selects what to focus on
    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Attentional selection)pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"))
        .def("selectFocus", &AttentionalSelection::selectFocus,
             py::arg("currentState"), py::arg("currentGoal"))
        .def("updateAttention", &AttentionalSelection::updateAttention,
             py::arg("currentState"), py::arg("target"))
        .def("getCurrentFocus", &AttentionalSelection::getCurrentFocus)
        .def("setFocus", &AttentionalSelection::setFocus, py::arg("focus"))
        .def("getAttentionStrength", &AttentionalSelection::getAttentionStrength, py::arg("target"))
        .def("clear", &AttentionalSelection::clear)
        .def("reset", &AttentionalSelection::reset);

    // ========== DEVELOPMENT ==========

    // DevelopmentSystem - Orchestrates neural development
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system)pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage)
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"))
        .def("advanceStage", &DevelopmentSystem::advanceStage)
        .def("getStageName", &DevelopmentSystem::getStageName)
        .def("update", [](DevelopmentSystem& self, Brain* brain, SimulationStep currentStep) {
            self.update(brain, currentStep);
        }, py::arg("brain"), py::arg("currentStep"))
        .def("update", [](DevelopmentSystem& self, Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
            self.update(brain, rng, dt);
        }, py::arg("brain"), py::arg("rng"), py::arg("dt"))
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier)
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod)
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress)
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge)
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"));

    // Synaptogenesis - Formation of new synapses
    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Synaptogenesis)pbdoc")
        .def(py::init<>())
        .def("getFormationRate", &Synaptogenesis::getFormationRate)
        .def("setFormationRate", &Synaptogenesis::setFormationRate, py::arg("rate"))
        .def("getTargetDensity", &Synaptogenesis::getTargetDensity)
        .def("setTargetDensity", &Synaptogenesis::setTargetDensity, py::arg("density"))
        .def("update", &Synaptogenesis::update, py::arg("brain"), py::arg("rng"));

    // Pruning - Synapse removal
    py::class_<Pruning>(m, "Pruning", R"pbdoc(Synapse pruning)pbdoc")
        .def(py::init<>())
        .def("getThreshold", &Pruning::getThreshold)
        .def("setThreshold", &Pruning::setThreshold, py::arg("threshold"))
        .def("getPruningRate", &Pruning::getPruningRate)
        .def("setPruningRate", &Pruning::setPruningRate, py::arg("rate"))
        .def("update", &Pruning::update, py::arg("brain"), py::arg("rng"))
        .def("pruneSynapse", &Pruning::pruneSynapse, py::arg("brain"), py::arg("synapse"));

    // Maturation - Neural maturation
    py::class_<Maturation>(m, "Maturation", R"pbdoc(Neural maturation)pbdoc")
        .def(py::init<>())
        .def("getProgress", &Maturation::getProgress)
        .def("setProgress", &Maturation::setProgress, py::arg("progress"))
        .def("update", &Maturation::update, py::arg("brain"), py::arg("currentStep"))
        .def("getMatureThreshold", &Maturation::getMatureThreshold)
        .def("getMatureRestingPotential", &Maturation::getMatureRestingPotential)
        .def("getMatureTimeConstant", &Maturation::getMatureTimeConstant);

    // ========== PERFORMANCE ==========

    // MemoryPool - Lock-free memory pool
    py::class_<MemoryPool<NeuronState>>(m, "NeuronPool", R"pbdoc(Neuron memory pool)pbdoc")
        .def(py::init<size_t, size_t>(), py::arg("blockSize") = 1024, py::arg("initialBlocks") = 4)
        .def("allocate", &MemoryPool<NeuronState>::allocate)
        .def("deallocate", &MemoryPool<NeuronState>::deallocate, py::arg("obj"))
        .def("empty", &MemoryPool<NeuronState>::empty)
        .def("allocatedCount", &MemoryPool<NeuronState>::allocatedCount)
        .def("availableCount", &MemoryPool<NeuronState>::availableCount)
        .def("memoryUsage", &MemoryPool<NeuronState>::memoryUsage)
        .def("reserve", &MemoryPool<NeuronState>::reserve, py::arg("count"));

    py::class_<MemoryPool<Synapse>>(m, "SynapsePool", R"pbdoc(Synapse memory pool)pbdoc")
        .def(py::init<size_t, size_t>(), py::arg("blockSize") = 1024, py::arg("initialBlocks") = 4)
        .def("allocate", &MemoryPool<Synapse>::allocate)
        .def("deallocate", &MemoryPool<Synapse>::deallocate, py::arg("obj"))
        .def("empty", &MemoryPool<Synapse>::empty)
        .def("allocatedCount", &MemoryPool<Synapse>::allocatedCount)
        .def("availableCount", &MemoryPool<Synapse>::availableCount)
        .def("memoryUsage", &MemoryPool<Synapse>::memoryUsage)
        .def("reserve", &MemoryPool<Synapse>::reserve, py::arg("count"));

    // EventQueue - High-performance event queue
    py::class_<EventQueue>(m, "EventQueue", R"pbdoc(Event queue)pbdoc")
        .def(py::init<>())
        .def("push", &EventQueue::push, py::arg("event"))
        .def("pop", &EventQueue::pop)
        .def("empty", &EventQueue::empty)
        .def("size", &EventQueue::size)
        .def("capacity", &EventQueue::capacity)
        .def("clear", &EventQueue::clear);

    // CheckpointManager - Automated checkpointing
    py::class_<CheckpointManager>(m, "CheckpointManager", R"pbdoc(Checkpoint manager)pbdoc")
        .def(py::init<>())
        .def("configure", &CheckpointManager::configure,
             py::arg("checkpointDir"), py::arg("saveIntervalSteps") = 10000, py::arg("maxCheckpoints") = 10, py::arg("compress") = true)
        .def("setNeuronProvider", &CheckpointManager::setNeuronProvider, py::arg("provider"))
        .def("setSynapseProvider", &CheckpointManager::setSynapseProvider, py::arg("provider"))
        .def("update", &CheckpointManager::update, py::arg("currentStep"), py::arg("currentTime"))
        .def("saveImmediately", &CheckpointManager::saveImmediately, py::arg("name") = "")
        .def("load", &CheckpointManager::load, py::arg("name"))
        .def("listCheckpoints", &CheckpointManager::listCheckpoints)
        .def("pruneOldCheckpoints", &CheckpointManager::pruneOldCheckpoints, py::arg("keepCount"))
        .def("getCheckpointDir", &CheckpointManager::getCheckpointDir)
        .def("getLastCheckpointPath", &CheckpointManager::getLastCheckpointPath)
        .def("shouldSave", &CheckpointManager::shouldSave, py::arg("currentStep"));

    // Checkpointer - Brain state checkpointing
    py::class_<Checkpointer>(m, "Checkpointer", R"pbdoc(Checkpointer)pbdoc")
        .def(py::init<>())
        .def("configure", &Checkpointer::configure,
             py::arg("checkpointDir"), py::arg("saveIntervalSteps") = 10000, py::arg("maxCheckpoints") = 10, py::arg("compress") = true)
        .def("setNeuronProvider", &Checkpointer::setNeuronProvider, py::arg("provider"))
        .def("setSynapseProvider", &Checkpointer::setSynapseProvider, py::arg("provider"))
        .def("update", &Checkpointer::update, py::arg("currentStep"), py::arg("currentTime"))
        .def("saveImmediately", &Checkpointer::saveImmediately, py::arg("name") = "")
        .def("load", &Checkpointer::load, py::arg("name"))
        .def("listCheckpoints", &Checkpointer::listCheckpoints)
        .def("pruneOldCheckpoints", &Checkpointer::pruneOldCheckpoints, py::arg("keepCount"))
        .def("getCheckpointDir", &Checkpointer::getCheckpointDir)
        .def("getLastCheckpointPath", &Checkpointer::getLastCheckpointPath)
        .def("shouldSave", &Checkpointer::shouldSave, py::arg("currentStep"));

    // ========== SENSORY ==========

    // Audio - Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // ========== MOLECULAR ==========

    // SynapseCheckpointData - Synapse state for checkpointing
    py::class_<SynapseCheckpointData>(m, "SynapseCheckpointData", R"pbdoc(Synapse checkpoint data)pbdoc")
        .def(py::init<>());

    // NeuronCheckpointData - Neuron state for checkpointing
    py::class_<NeuronCheckpointData>(m, "NeuronCheckpointData", R"pbdoc(Neuron checkpoint data)pbdoc")
        .def(py::init<>());

    // ========== EVENTS ==========

    // DetailedSpikeEvent - Spike event with metadata
    py::class_<DetailedSpikeEvent>(m, "DetailedSpikeEvent", R"pbdoc(Detailed spike event)pbdoc")
        .def(py::init<>())
        .def(py::init<NeuronId, Timestamp, SimulationStep, RegionId, PopulationId>())
        .def_readwrite("source", &DetailedSpikeEvent::source)
        .def_readwrite("timestamp", &DetailedSpikeEvent::timestamp)
        .def_readwrite("step", &DetailedSpikeEvent::step)
        .def_readwrite("regionId", &DetailedSpikeEvent::regionId)
        .def_readwrite("populationId", &DetailedSpikeEvent::populationId);

    // DelayedSpikeEvent - Spike event with delay
    py::class_<DelayedSpikeEvent>(m, "DelayedSpikeEvent", R"pbdoc(Delayed spike event)pbdoc")
        .def(py::init<>())
        .def(py::init<NeuronId, NeuronId, SynapseId, SynapticWeight, SynapseType, Timestamp, Timestamp, SimulationStep, SimulationStep>())
        .def_readwrite("sourceNeuron", &DelayedSpikeEvent::source_neuron)
        .def_readwrite("destinationNeuron", &DelayedSpikeEvent::destination_neuron)
        .def_readwrite("synapseId", &DelayedSpikeEvent::synapse_id)
        .def_readwrite("weight", &DelayedSpikeEvent::weight)
        .def_readwrite("synapseType", &DelayedSpikeEvent::synapse_type)
        .def_readwrite("timestamp", &DelayedSpikeEvent::timestamp)
        .def_readwrite("deliveryTime", &DelayedSpikeEvent::delivery_time)
        .def_readwrite("step", &DelayedSpikeEvent::step)
        .def_readwrite("deliveryStep", &DelayedSpikeEvent::delivery_step)
        .def_readwrite("isExcitatory", &DelayedSpikeEvent::is_excitatory);

    // ========== ENUMERATIONS ==========

    // LogLevel enum
    py::enum_<LogLevel>(m, "LogLevel", R"pbdoc(Log severity levels)pbdoc")
        .value("Debug", LogLevel::Debug)
        .value("Info", LogLevel::Info)
        .value("Warning", LogLevel::Warning)
        .value("Error", LogLevel::Error)
        .value("Critical", LogLevel::Critical)
        .export_values();

    // CheckpointReader - Checkpoint file reading
    py::class_<CheckpointReader>(m, "CheckpointReader", R"pbdoc(Checkpoint file reader)pbdoc")
        .def(py::init<>())
        .def("open", &CheckpointReader::open, py::arg("filepath"),
             "Open a checkpoint file for reading")
        .def("close", &CheckpointReader::close, "Close the checkpoint file")
        .def("isOpen", &CheckpointReader::isOpen, "Check if checkpoint is open")
        .def("getHeader", &CheckpointReader::getHeader,
             py::return_value_policy::reference_internal,
             "Get checkpoint header")
        .def("readNeurons", &CheckpointReader::readNeurons, py::arg("data"),
             "Read neuron checkpoint data")
        .def("readSynapses", &CheckpointReader::readSynapses, py::arg("data"),
             "Read synapse checkpoint data")
        .def("readSection", &CheckpointReader::readSection, py::arg("sectionType"),
             "Read raw section data")
        .def("validate", &CheckpointReader::validate,
             "Validate checkpoint integrity")
        .def("getError", &CheckpointReader::getError,
             py::return_value_policy::reference_internal,
             "Get validation error message")
        .def("getProgress", &CheckpointReader::getProgress,
             "Get reader progress (0.0 - 1.0)");

    // CheckpointWriter - Checkpoint file writing
    py::class_<CheckpointWriter>(m, "CheckpointWriter", R"pbdoc(Checkpoint file writer)pbdoc")
        .def(py::init<>())
        .def("create", &CheckpointWriter::create, py::arg("filepath"), py::arg("compression") = CompressionLevel::Balanced,
             "Create a new checkpoint file")
        .def("close", &CheckpointWriter::close, "Close the checkpoint file")
        .def("isOpen", &CheckpointWriter::isOpen, "Check if checkpoint is open")
        .def("setMetadata", &CheckpointWriter::setMetadata, py::arg("neuronCount"), py::arg("synapseCount"),
             py::arg("regionCount"), py::arg("step"), py::arg("simulationTime"),
             "Set checkpoint metadata")
        .def("writeNeurons", &CheckpointWriter::writeNeurons, py::arg("data"),
             "Write neuron checkpoint data")
        .def("writeSynapses", &CheckpointWriter::writeSynapses, py::arg("data"),
             "Write synapse checkpoint data")
        .def("writeSection", &CheckpointWriter::writeSection, py::arg("sectionType"), py::arg("data"), py::arg("size"),
             "Write raw section data")
        .def("finalize", &CheckpointWriter::finalize,
             "Finalize and write footer")
        .def("abort", &CheckpointWriter::abort,
             "Abort checkpoint (cleanup partial file)")
        .def("getBytesWritten", &CheckpointWriter::getBytesWritten,
             "Get total bytes written");

    // SPIKE_EVENT_TYPE enum (placeholder - needs to be defined in Types.hpp)
    // Note: We're adding the key spike event types from the requirements
    py::enum_<SpikeEventType>(m, "SpikeEventType", R"pbdoc(Spike event type enumeration)pbdoc")
        .value("Single", SpikeEventType::Single)
        .value("Delayed", SpikeEventType::Delayed)
        .value("Broadcast", SpikeEventType::Broadcast)
        .value("Propagated", SpikeEventType::Propagated)
        .export_values();

    // SPIKE_EVENT subtypes
    py::enum_<SpikeEventSubType>(m, "SpikeEventSubType", R"pbdoc(Spike event subtype enumeration)pbdoc")
        .value("Standard", SpikeEventSubType::Standard)
        .value("Neuromodulatory", SpikeEventSubType::Neuromodulatory)
        .value("Plasticity", SpikeEventSubType::Plasticity)
        .value("Prediction", SpikeEventSubType::Prediction)
        .export_values();

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
