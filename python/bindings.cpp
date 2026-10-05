#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/development/DevelopmentSystem.hpp"

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

    // Logger class for system-wide logging
    py::class_<nlm::Logger> logger_class(m, "Logger", R"pbdoc(System-wide logger for NLM)pbdoc");
    logger_class
        .def(py::init<>())
        .def("addLogger", &nlm::Logger::addLogger, py::arg("logger"),
             "Add a logger sink")
        .def("clearLoggers", &nlm::Logger::clearLoggers,
             "Remove all logger sinks")
        .def("setLevel", &nlm::Logger::setLevel, py::arg("level"),
             "Set minimum log level for all loggers")
        .def("log", &nlm::Logger::log, py::arg("level"), py::arg("message"),
             py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
             "Log a message with specified level")
        .def("debug", &nlm::Logger::debug, py::arg("message"), py::arg("file") = "",
             py::arg("line") = 0, py::arg("function") = "",
             "Log debug message")
        .def("info", &nlm::Logger::info, py::arg("message"), py::arg("file") = "",
             py::arg("line") = 0, py::arg("function") = "",
             "Log info message")
        .def("warning", &nlm::Logger::warning, py::arg("message"), py::arg("file") = "",
             py::arg("line") = 0, py::arg("function") = "",
             "Log warning message")
        .def("error", &nlm::Logger::error, py::arg("message"), py::arg("file") = "",
             py::arg("line") = 0, py::arg("function") = "",
             "Log error message")
        .def("critical", &nlm::Logger::critical, py::arg("message"),
             py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
             "Log critical message")
        .def("isEnabled", &nlm::Logger::isEnabled, py::arg("level"),
             "Check if a log level is enabled")
        .def("getGlobal", &nlm::Logger::getGlobal,
             py::return_value_policy::reference,
             "Get global logger instance")
        .def("setGlobal", &nlm::Logger::setGlobal, py::arg("logger"),
             "Set global logger instance")
        .def("__repr__", [](const nlm::Logger& logger) {
            return "<NLM Logger>";
        });

    // Console logger class for terminal output
    py::class_<nlm::ConsoleLogger, nlm::ILogger>(m, "ConsoleLogger", R"pbdoc(Console logger for terminal output)pbdoc")
        .def(py::init<>())
        .def(py::init<nlm::LogLevel>(), py::arg("level"),
             "Create console logger with specific log level")
        .def("setLevel", &nlm::ConsoleLogger::setLevel, py::arg("level"),
             "Set console log level")
        .def("getLevel", &nlm::ConsoleLogger::getLevel,
             "Get current log level")
        .def("setUseColors", &nlm::ConsoleLogger::setUseColors, py::arg("useColors"),
             "Enable/disable colored output")
        .def("getUseColors", &nlm::ConsoleLogger::getUseColors,
             "Get whether colors are enabled")
        .def("setOutputFile", &nlm::ConsoleLogger::setOutputFile, py::arg("filepath"),
             "Set output file (in addition to console)")
        .def("closeOutputFile", &nlm::ConsoleLogger::closeOutputFile,
             "Close output file");

    // SimulationClock class for time management
    py::class_<nlm::SimulationClock> simulation_clock_class(m, "SimulationClock", R"pbdoc(Simulation clock for deterministic time management)pbdoc");
    simulation_clock_class
        .def(py::init<nlm::TimestepDuration>(), py::arg("timestep") = 0.001,
             "Create simulation clock with specified timestep")
        .def("reset", &nlm::SimulationClock::reset,
             "Reset simulation clock")
        .def("advance", &nlm::SimulationClock::advance,
             "Advance simulation by one timestep")
        .def("getStep", &nlm::SimulationClock::getStep,
             "Get current step number")
        .def("setStep", &nlm::SimulationClock::setStep, py::arg("step"),
             "Set current step number")
        .def("getTime", &nlm::SimulationClock::getTime,
             "Get current simulation time")
        .def("setTime", &nlm::SimulationClock::setTime, py::arg("time"),
             "Set current simulation time")
        .def("getTimestep", &nlm::SimulationClock::getTimestep,
             "Get simulation timestep")
        .def("setTimestep", &nlm::SimulationClock::setTimestep, py::arg("dt"),
             "Set simulation timestep")
        .def("isFirstStep", &nlm::SimulationClock::isFirstStep,
             "Check if this is the first step")
        .def("isLastStep", &nlm::SimulationClock::isLastStep, py::arg("totalSteps"),
             "Check if this is the last step given total steps")
        .def("stepToTime", &nlm::SimulationClock::stepToTime, py::arg("step"),
             "Convert step number to time")
        .def("timeToStep", &nlm::SimulationClock::timeToStep, py::arg("time"),
             "Convert time to step number")
        .def("advanceBy", &nlm::SimulationClock::advanceBy, py::arg("steps"),
             "Advance by multiple steps")
        .def("timeAtFutureStep", &nlm::SimulationClock::timeAtFutureStep,
             py::arg("stepsAhead"), "Get time at a future step from now")
        .def("getRealTimeElapsed", &nlm::SimulationClock::getRealTimeElapsed,
             "Get real time elapsed since start")
        .def("setRealTimeStart", &nlm::SimulationClock::setRealTimeStart,
             "Set real time start point");

    // LogEntry structure for logging
    py::class_<nlm::LogEntry>(m, "LogEntry", R"pbdoc(Log entry structure)pbdoc")
        .def(py::init<>())
        .def_readwrite("level", &nlm::LogEntry::level)
        .def_readwrite("message", &nlm::LogEntry::message)
        .def_readwrite("file", &nlm::LogEntry::file)
        .def_readwrite("line", &nlm::LogEntry::line)
        .def_readwrite("function", &nlm::LogEntry::function)
        .def_readwrite("timestamp", &nlm::LogEntry::timestamp);

    // LogStream for stream-style logging
    py::class_<nlm::LogStream>(m, "LogStream", R"pbdoc(Stream-style logging helper)pbdoc")
        .def(py::init<nlm::LogLevel, const std::string&, int, const std::string&>(),
             py::arg("level"), py::arg("file"), py::arg("line"), py::arg("function"),
             "Create log stream with file/line/function context");

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

    // Global logger initialization functions
    m.def("initializeGlobalLogger", []() {
        auto logger = std::make_shared<nlm::Logger>();
        auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
        logger->addLogger(consoleLogger);
        nlm::Logger::setGlobal(logger);
    }, "Initialize global logger with console output");

    m.def("getGlobalLogger", []() {
        return nlm::Logger::getGlobal();
    }, py::return_value_policy::reference,
         "Get the global logger instance");

    m.def("setGlobalLogger", [](std::shared_ptr<nlm::Logger> logger) {
        nlm::Logger::setGlobal(logger);
    }, py::arg("logger"), "Set the global logger instance");

    m.def("createLogStream", [](nlm::LogLevel level, const std::string& file, 
                             int line, const std::string& function) {
        return nlm::LogStream(level, file, line, function);
    }, py::arg("level"), py::arg("file"), py::arg("line"), py::arg("function"),
       "Create a log stream for stream-style logging");

    // Helper functions for common logging patterns
    m.def("logDebug", [](const std::string& message) {
        nlm::Logger::getGlobal().debug(message);
    }, py::arg("message"), "Log debug message using global logger");

    m.def("logInfo", [](const std::string& message) {
        nlm::Logger::getGlobal().info(message);
    }, py::arg("message"), "Log info message using global logger");

    m.def("logWarning", [](const std::string& message) {
        nlm::Logger::getGlobal().warning(message);
    }, py::arg("message"), "Log warning message using global logger");

    m.def("logError", [](const std::string& message) {
        nlm::Logger::getGlobal().error(message);
    }, py::arg("message"), "Log error message using global logger");

    m.def("logCritical", [](const std::string& message) {
        nlm::Logger::getGlobal().critical(message);
    }, py::arg("message"), "Log critical message using global logger");

    // Simulation utilities
    m.def("simulate", [](std::shared_ptr<Brain> brain, SimulationStep steps, 
                       bool verbose = false) {
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step);
            if (verbose && step % 100 == 0) {
                nlm::Logger::getGlobal().info("Step " + std::to_string(step) + ": " +
                                             std::to_string(brain->getFiringNeuronCount()) +
                                             " firing neurons");
            }
        }
    }, py::arg("brain"), py::arg("steps"), py::arg("verbose") = false,
       "Run brain simulation for specified number of steps");

    m.def("simulateWithTime", [](std::shared_ptr<Brain> brain, SimulationStep steps,
                               double timestep = 0.001, bool verbose = false) {
        nlm::SimulationClock clock(timestep);
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, clock.getTime());
            clock.advance();
            if (verbose && step % 100 == 0) {
                nlm::Logger::getGlobal().info("Step " + std::to_string(step) + " (t=" +
                                             std::to_string(clock.getTime()) + "): " +
                                             std::to_string(brain->getFiringNeuronCount()) +
                                             " firing neurons");
            }
        }
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.001,
       py::arg("verbose") = false,
       "Run brain simulation with time tracking");

    // Utility functions for brain analysis
    m.def("analyzeBrainState", [](std::shared_ptr<Brain> brain) {
        auto& logger = nlm::Logger::getGlobal();
        logger.info("=== Brain State Analysis ===");
        logger.info("Total Neurons: " + std::to_string(brain->getTotalNeuronCount()));
        logger.info("Total Synapses: " + std::to_string(brain->getTotalSynapseCount()));
        logger.info("Active Neurons: " + std::to_string(brain->getActiveNeuronCount()));
        logger.info("Firing Neurons: " + std::to_string(brain->getFiringNeuronCount()));
        logger.info("Total Spikes: " + std::to_string(brain->getTotalSpikeCount()));
        logger.info("E/I Ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
        logger.info("Average Firing Rate: " + std::to_string(brain->getAverageFiringRate()));
        logger.info("Developmental Stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
    }, py::arg("brain"), "Analyze and log brain state information");

    // Performance monitoring
    m.def("startPerformanceTimer", []() {
        static auto start = std::chrono::high_resolution_clock::now();
        return start;
    }, "Start performance timer");

    m.def("stopPerformanceTimer", [](auto start) {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        nlm::Logger::getGlobal().info("Operation took " + std::to_string(duration.count()) + " ms");
    }, py::arg("start"), "Stop performance timer and log duration");

    // Export constants for commonly used values
    m.attr("SIMULATION_DEFAULT_TIMESTEP") = py::float_(0.001);
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_REGION_COUNT") = 2;
    m.attr("DEFAULT_CONNECTION_PROBABILITY") = 0.1;
    m.attr("LOG_LEVEL_DEBUG") = nlm::LogLevel::Debug;
    m.attr("LOG_LEVEL_INFO") = nlm::LogLevel::Info;
    m.attr("LOG_LEVEL_WARNING") = nlm::LogLevel::Warning;
    m.attr("LOG_LEVEL_ERROR") = nlm::LogLevel::Error;
    m.attr("LOG_LEVEL_CRITICAL") = nlm::LogLevel::Critical;

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Export advanced utility functions
    m.def("runBenchmark", [](std::shared_ptr<Brain> brain, SimulationStep steps) {
        auto profiler = std::make_shared<Profiler>();
        auto timer = profiler->startTimer("total");
        
        profiler->startTimer("brain_step");
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step);
            profiler->stopTimer(timer); // Stop current timer and start new one
            timer = profiler->startTimer("brain_step");
        }
        profiler->stopTimer(timer);
        
        return profiler->getProfileSummary();
    }, py::arg("brain"), py::arg("steps"), "Run a benchmark test on the brain");

    m.def("createQuickTestSetup", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 100);
        config->set("brain.region_count", 1);
        config->set("brain.connection_probability", 0.05);
        config->set("random_seed", 42);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        nlm::Logger::getGlobal().info("Quick test setup complete");
        nlm::Logger::getGlobal().info("Neurons: " + std::to_string(brain->getTotalNeuronCount()));
        nlm::Logger::getGlobal().info("Synapses: " + std::to_string(brain->getTotalSynapseCount()));
        
        return brain;
    }, "Create a quick test setup with optimized default parameters");

    m.def("validateBrainConfiguration", [](std::shared_ptr<Brain> brain) {
        std::vector<std::string> issues;
        auto config = brain->getConfig();
        
        if (!config->has("brain.neuron_count")) {
            issues.push_back("Missing brain.neuron_count");
        } else if (config->getOr<int64_t>("brain.neuron_count", 0) < 10) {
            issues.push_back("brain.neuron_count too low (< 10)");
        }
        
        if (!config->has("brain.region_count")) {
            issues.push_back("Missing brain.region_count");
        }
        
        if (!config->has("brain.connection_probability")) {
            issues.push_back("Missing brain.connection_probability");
        } else if (config->getOr<double>("brain.connection_probability", 0) > 1.0) {
            issues.push_back("brain.connection_probability > 1.0");
        }
        
        if (issues.empty()) {
            return "✓ Configuration is valid";
        } else {
            std::stringstream ss;
            ss << "✗ Configuration has issues:\n";
            for (const auto& issue : issues) {
                ss << "  - " << issue << "\n";
            }
            return ss.str();
        }
    }, py::arg("brain"), "Validate brain configuration for common issues");

    // Export advanced system statistics and utilities
    m.def("getSystemStats", []() {
        std::stringstream ss;
        ss << "=== NLM System Statistics ===\n";
        ss << "Version: 0.1.0\n";
        ss << "Architecture: Artificial Developmental Brain\n";
        ss << "Phase: 6 - Final Integration\n";
        ss << "Language: C++20 with Python bindings\n";
        ss << "Core Components:\n";
        ss << "  - Neural Core (LIF neurons, synapses, plasticity)\n";
        ss << "  - Memory Systems (Working, Episodic, Associative)\n";
        ss << "  - Neuromodulation (Dopamine, Curiosity, Novelty)\n";
        ss << "  - Cognitive Systems (Planner, Concept Formation)\n";
        ss << "  - Prediction System\n";
        ss << "  - Development System\n";
        ss << "Python API Features:\n";
        ss << "  - Comprehensive logging and monitoring\n";
        ss << "  - Performance profiling tools\n";
        ss << "  - Simulation control utilities\n";
        ss << "  - Memory management tools\n";
        ss << "  - Batch processing capabilities\n";
        ss << "  - Debug and inspection tools\n";
        ss << "  - Configuration optimization\n";
        return ss.str();
    }, "Get system-wide statistics and information");

    m.def("createAdvancedUsageExample", []() {
        nlm::Logger::getGlobal().info("=== Advanced Usage Example ===");
        nlm::Logger::getGlobal().info("Creating comprehensive test scenario...");
        
        // Create optimized configuration
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 500);
        config->set("brain.region_count", 2);
        config->set("brain.connection_probability", 0.15);
        config->set("plasticity.stdp.learning_rate", 0.001);
        config->set("plasticity.hebbian.learning_rate", 0.01);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 1.0);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        
        nlm::Logger::getGlobal().info("Configuration created with optimized parameters for complex learning scenarios");
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        nlm::Logger::getGlobal().info("Brain initialized with " + std::to_string(brain->getTotalNeuronCount()) + " neurons");
        
        // Create world and agent
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        
        nlm::Logger::getGlobal().info("World created and agent positioned");
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        nlm::Logger::getGlobal().info("Agent brain interface created and initialized");
        
        // Enable advanced features
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        nlm::Logger::getGlobal().info("Advanced features enabled: reward modulation, structural plasticity, development, curiosity");
        
        // Run complex simulation
        nlm::Logger::getGlobal().info("Starting complex learning simulation...");
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step);
            
            // Get action
            auto action = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation
            float reward = 0.0f;
            auto internal = world->getSensoryPercept().getInternal();
            if (!internal.empty()) {
                reward = internal[0];
            }
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1);
            
            // Log progress periodically
            if (step % 100 == 0) {
                nlm::Logger::getGlobal().info("Step " + std::to_string(step) + ": " +
                                             std::to_string(brain->getFiringNeuronCount()) + " firing, " +
                                             std::to_string(agent->getCuriosityLevel()) + " curiosity, " +
                                             std::to_string(agent->getNoveltyLevel()) + " novelty");
            }
        }
        
        nlm::Logger::getGlobal().info("=== Advanced Usage Example Complete ===");
        nlm::Logger::getGlobal().info("Simulation completed successfully with all advanced features working together!");
        
        return brain;
    }, "Run advanced usage example demonstrating all features");
}

} // namespace nlm
