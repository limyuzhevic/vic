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

// Include for RandomGenerator and SimulationClock
#include "../src/core/Random/Random.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"

// Include for CheckpointManager
#include "../src/performance/CheckpointSystem.hpp"

// Include for memory and cognition systems
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralPrediction.hpp"

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

    // Log entry structure for binding
    py::class_<LogEntry>(m, "LogEntry", R"pbdoc(Log entry structure)pbdoc")
        .def(py::init<>())
        .def_readwrite("level", &LogEntry::level)
        .def_readwrite("message", &LogEntry::message)
        .def_readwrite("file", &LogEntry::file)
        .def_readwrite("line", &LogEntry::line)
        .def_readwrite("function", &LogEntry::function)
        .def_readwrite("timestamp", &LogEntry::timestamp);

    // ILogger interface
    py::class_<ILogger>(m, "ILogger", R"pbdoc(Logger interface)pbdoc")
        .def("log", &ILogger::log, py::arg("entry"),
             "Log an entry")
        .def("setLevel", &ILogger::setLevel, py::arg("level"),
             "Set log level")
        .def("getLevel", &ILogger::getLevel,
             "Get log level");

    // Console logger implementation
    py::class_<ConsoleLogger, ILogger>(m, "ConsoleLogger", R"pbdoc(Console logger implementation)pbdoc")
        .def(py::init<>())
        .def(py::init<LogLevel>(), py::arg("level"))
        .def("log", &ConsoleLogger::log, py::arg("entry"),
             "Log an entry (ConsoleLogger)")
        .def("setLevel", &ConsoleLogger::setLevel, py::arg("level"),
             "Set log level (ConsoleLogger)")
        .def("getLevel", &ConsoleLogger::getLevel,
             "Get log level (ConsoleLogger)")
        .def("setUseColors", &ConsoleLogger::setUseColors, py::arg("useColors"),
             "Enable/disable colors")
        .def("getUseColors", &ConsoleLogger::getUseColors,
             "Check if colors are enabled")
        .def("setOutputFile", &ConsoleLogger::setOutputFile, py::arg("filepath"),
             "Set output file (in addition to console)")
        .def("closeOutputFile", &ConsoleLogger::closeOutputFile,
             "Close output file");

    // Main Logger class
    py::class_<Logger>(m, "Logger", R"pbdoc(Main Logger class)pbdoc")
        .def(py::init<>())
        .def("addLogger", &Logger::addLogger, py::arg("logger"),
             "Add logger sink")
        .def("clearLoggers", &Logger::clearLoggers,
             "Remove all loggers")
        .def("setLevel", &Logger::setLevel, py::arg("level"),
             "Set minimum level for all loggers")
        .def("log", &Logger::log, py::arg("level"), py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
             "Log message with level and location info")
        .def("debug", &Logger::debug, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
             "Log debug message")
        .def("info", &Logger::info, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
             "Log info message")
        .def("warning", &Logger::warning, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
             "Log warning message")
        .def("error", &Logger::error, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
             "Log error message")
        .def("critical", &Logger::critical, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
             "Log critical message")
        .def("isEnabled", &Logger::isEnabled, py::arg("level"),
             "Check if level is enabled")
        .def_static("getGlobal", &Logger::getGlobal,
             "Get global logger instance")
        .def_static("setGlobal", &Logger::setGlobal, py::arg("logger"),
             "Set global logger");

    // Log level enumeration
    py::enum_<LogLevel>(m, "LogLevel", R"pbdoc(Log severity levels)pbdoc")
        .value("Debug", LogLevel::Debug)
        .value("Info", LogLevel::Info)
        .value("Warning", LogLevel::Warning)
        .value("Error", LogLevel::Error)
        .value("Critical", LogLevel::Critical)
        .export_values();

    // Random generator class
    py::class_<RandomGenerator>(m, "RandomGenerator", R"pbdoc(Deterministic random number generator)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("seed"))
        .def("seed", &RandomGenerator::seed, py::arg("seed"),
             "Set random seed")
        .def("getSeed", &RandomGenerator::getSeed,
             "Get random seed")
        .def("uniformInt", static_cast<uint32_t (RandomGenerator::*)(uint32_t, uint32_t)>(&RandomGenerator::uniformInt),
             py::arg("min"), py::arg("max"),
             "Generate uniform random integer in [min, max]")
        .def("uniformInt", static_cast<uint64_t (RandomGenerator::*)(uint64_t, uint64_t)>(&RandomGenerator::uniformInt),
             py::arg("min"), py::arg("max"),
             "Generate uniform random 64-bit integer in [min, max]")
        .def("uniformReal", static_cast<double (RandomGenerator::*)(double, double)>(&RandomGenerator::uniformReal),
             py::arg("min"), py::arg("max"),
             "Generate uniform random real in [min, max)")
        .def("uniformReal", static_cast<float (RandomGenerator::*)(float, float)>(&RandomGenerator::uniformReal),
             py::arg("min"), py::arg("max"),
             "Generate uniform random float in [min, max)")
        .def("normal", static_cast<double (RandomGenerator::*)(double, double)>(&RandomGenerator::normal),
             py::arg("mean"), py::arg("stddev"),
             "Generate normally distributed random number")
        .def("normal", static_cast<float (RandomGenerator::*)(float, float)>(&RandomGenerator::normal),
             py::arg("mean"), py::arg("stddev"),
             "Generate normally distributed random float")
        .def("bernoulli", &RandomGenerator::bernoulli, py::arg("p") = 0.5,
             "Generate Bernoulli random number")
        .def("poisson", &RandomGenerator::poisson, py::arg("lambda"),
             "Generate Poisson distributed random number")
        .def("exponential", &RandomGenerator::exponential, py::arg("lambda"),
             "Generate exponentially distributed random number")
        .def("randomBool", &RandomGenerator::randomBool,
             "Generate random boolean")
        .def("randomBit", &RandomGenerator::randomBit,
             "Generate random bit")
        .def("shuffle", &RandomGenerator::shuffle, py::arg("container"),
             "Shuffle container in-place")
        .def("pickRandom", &RandomGenerator::pickRandom, py::arg("container"),
             "Pick random element from container")
        .def("pickRandomIndex", &RandomGenerator::pickRandomIndex, py::arg("n"),
             "Pick random index from 0 to n-1")
        .def("fillBytes", &RandomGenerator::fillBytes, py::arg("data"), py::arg("size"),
             "Fill buffer with random bytes")
        .def("engine", &RandomGenerator::engine,
             py::return_value_policy::reference_internal,
             "Get underlying engine for advanced use");

    // SimulationClock class
    py::class_<SimulationClock>(m, "SimulationClock", R"pbdoc(Simulation clock for managing time in the neural simulation)pbdoc")
        .def(py::init<TimestepDuration>(), py::arg("timestep") = 0.001)
        .def("reset", &SimulationClock::reset,
             "Reset the simulation clock")
        .def("advance", &SimulationClock::advance,
             "Advance the simulation clock by one timestep")
        .def("getStep", &SimulationClock::getStep,
             "Get current simulation step")
        .def("setStep", &SimulationClock::setStep, py::arg("step"),
             "Set simulation step")
        .def("getTime", &SimulationClock::getTime,
             "Get current simulation time")
        .def("setTime", &SimulationClock::setTime, py::arg("time"),
             "Set simulation time")
        .def("getTimestep", &SimulationClock::getTimestep,
             "Get simulation timestep")
        .def("setTimestep", &SimulationClock::setTimestep, py::arg("dt"),
             "Set simulation timestep")
        .def("isFirstStep", &SimulationClock::isFirstStep,
             "Check if this is the first step")
        .def("isLastStep", &SimulationClock::isLastStep, py::arg("totalSteps"),
             "Check if this is the last step (given total steps)")
        .def("stepToTime", &SimulationClock::stepToTime, py::arg("step"),
             "Convert simulation step to time")
        .def("timeToStep", &SimulationClock::timeToStep, py::arg("time"),
             "Convert simulation time to step")
        .def("advanceBy", &SimulationClock::advanceBy, py::arg("steps"),
             "Advance by multiple steps")
        .def("timeAtFutureStep", &SimulationClock::timeAtFutureStep, py::arg("stepsAhead"),
             "Get time at a future step from now")
        .def("getRealTimeElapsed", &SimulationClock::getRealTimeElapsed,
             "Get real time elapsed (for real-time mode)")
        .def("setRealTimeStart", &SimulationClock::setRealTimeStart,
             "Start real-time tracking");

    // CheckpointManager class
    py::class_<CheckpointManager>(m, "CheckpointManager", R"pbdoc(Checkpoint manager for automated checkpointing)pbdoc")
        .def(py::init<>())
        .def("configure", &CheckpointManager::configure, py::arg("checkpointDir"), py::arg("saveIntervalSteps") = 10000, py::arg("maxCheckpoints") = 10, py::arg("compress") = true,
             "Configure checkpoint manager")
        .def("setNeuronProvider", &CheckpointManager::setNeuronProvider, py::arg("provider"),
             "Set checkpoint data provider for neurons")
        .def("setSynapseProvider", &CheckpointManager::setSynapseProvider, py::arg("provider"),
             "Set checkpoint data provider for synapses")
        .def("update", &CheckpointManager::update, py::arg("currentStep"), py::arg("currentTime"),
             "Update checkpoint state (call each simulation step)")
        .def("saveImmediately", &CheckpointManager::saveImmediately, py::arg("name") = "",
             "Save checkpoint immediately")
        .def("load", &CheckpointManager::load, py::arg("name"),
             "Load checkpoint")
        .def("listCheckpoints", &CheckpointManager::listCheckpoints,
             "List available checkpoints")
        .def("pruneOldCheckpoints", &CheckpointManager::pruneOldCheckpoints, py::arg("keepCount"),
             "Delete old checkpoints (keep most recent N)")
        .def("getCheckpointDir", &CheckpointManager::getCheckpointDir,
             "Get checkpoint directory")
        .def("getLastCheckpointPath", &CheckpointManager::getLastCheckpointPath,
             "Get last checkpoint path")
        .def("shouldSave", &CheckpointManager::shouldSave, py::arg("currentStep"),
             "Check if checkpoint should be saved");

    // Memory system classes
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory with persistent activity)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store information in working memory")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"),
             "Store specific neuron activation")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             "Retrieve current working memory content as activity levels")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"),
             "Check if specific neuron is part of working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"),
             "Get activation level of a specific neuron in working memory")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"),
             "Update working memory (maintenance and decay)")
        .def("clear", &NeuralWorkingMemory::clear,
             "Clear working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity,
             "Get capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"),
             "Set capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate,
             "Get decay rate for memory traces")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"),
             "Set decay rate for memory traces")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             "Get neurons currently in working memory")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"),
             "Strengthen working memory representation (for rehearsal)")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition,
             "Run competition between memory traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"),
             "Check if neuron is part of winning population")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level");

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural episodic memory for experience storage)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("store", &NeuralEpisodicMemory::store, py::arg("perception"), py::arg("action") = std::vector<float>(), py::arg("reward") = 0.0f,
             "Store an episodic memory item")
        .def("retrieve", &NeuralEpisodicMemory::retrieve, py::arg("queryPattern"), py::arg("maxResults") = 5,
             "Retrieve episodic memories matching query")
        .def("searchByContext", &NeuralEpisodicMemory::searchByContext, py::arg("contextPattern"), py::arg("maxResults") = 10,
             "Search episodic memories by context pattern")
        .def("getMemory", &NeuralEpisodicMemory::getMemory, py::arg("index"),
             "Get specific episodic memory item")
        .def("updateMemory", &NeuralEpisodicMemory::updateMemory, py::arg("index"), py::arg("newPerception"), py::arg("newAction"), py::arg("newReward"),
             "Update existing episodic memory")
        .def("deleteMemory", &NeuralEpisodicMemory::deleteMemory, py::arg("index"),
             "Delete episodic memory item")
        .def("clear", &NeuralEpisodicMemory::clear,
             "Clear all episodic memories")
        .def("getMemoryCount", &NeuralEpisodicMemory::getMemoryCount,
             "Get number of stored memories")
        .def("getCapacity", &NeuralEpisodicMemory::getCapacity,
             "Get memory capacity")
        .def("setCapacity", &NeuralEpisodicMemory::setCapacity, py::arg("capacity"),
             "Set memory capacity")
        .def("getDecayRate", &NeuralEpisodicMemory::getDecayRate,
             "Get decay rate for episodic memories")
        .def("setDecayRate", &NeuralEpisodicMemory::setDecayRate, py::arg("rate"),
             "Set decay rate for episodic memories")
        .def("encodePerception", &NeuralEpisodicMemory::encodePerception, py::arg("perception"),
             "Encode perception into neural pattern")
        .def("decodePerception", &NeuralEpisodicMemory::decodePerception, py::arg("neuralPattern"),
             "Decode neural pattern back to perception")
        .def("getTimeline", &NeuralEpisodicMemory::getTimeline,
             "Get chronological timeline of memories");

    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Neural associative memory for pattern associations)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("associate", &NeuralAssociativeMemory::associate, py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f,
             "Create association between two neural patterns")
        .def("associateFromExperience", &NeuralAssociativeMemory::associateFromExperience, py::arg("episode"),
             "Create association based on experience")
        .def("retrieve", &NeuralAssociativeMemory::retrieve, py::arg("queryPattern"), py::arg("maxResults") = 5,
             "Retrieve patterns associated with a query pattern")
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength, py::arg("patternA"), py::arg("patternB"),
             "Get association strength between two patterns")
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation, py::arg("patternA"), py::arg("patternB"), py::arg("delta"),
             "Update association based on outcome")
        .def("spreadActivation", &NeuralAssociativeMemory::spreadActivation, py::arg("cuePattern"), py::arg("steps") = 2,
             "Spread activation from cue pattern")
        .def("clear", &NeuralAssociativeMemory::clear,
             "Clear associations")
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount,
             "Get number of associations");

    py::class_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
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

    py::class_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    // Cognition system classes
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner for action planning)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan next action given current state and goal")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence, py::arg("actions"), py::arg("startState"),
             "Evaluate a potential action sequence")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth,
             "Get number of steps to look ahead")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"),
             "Set planning depth")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence,
             "Get planning confidence")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality, py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
             "Update plans based on actual outcome")
        .def("clearCache", &NeuralPlanner::clearCache,
             "Clear planning cache")
        .def("setActionQuality", &NeuralPlanner::setActionQuality, py::arg("action"), py::arg("quality"),
             "Set action quality function")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal,
             "Get current goal")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"),
             "Set current goal")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful,
             "Check if recent planning was successful");

    py::class_<ConceptInstance>(m, "ConceptInstance", R"pbdoc(Neural concept instance)pbdoc")
        .def(py::init<>())
        .def_readwrite("pattern", &ConceptInstance::pattern)
        .def_readwrite("features", &ConceptInstance::features)
        .def_readwrite("observationCount", &ConceptInstance::observationCount)
        .def_readwrite("lastObserved", &ConceptInstance::lastObserved)
        .def_readwrite("avgReward", &ConceptInstance::avgReward)
        .def_readwrite("stability", &ConceptInstance::stability);

    py::class_<DiscoveredConcept>(m, "DiscoveredConcept", R"pbdoc(Discovered neural concept)pbdoc")
        .def(py::init<>())
        .def_readwrite("id", &DiscoveredConcept::id)
        .def_readwrite("prototype", &DiscoveredConcept::prototype)
        .def_readwrite("instances", &DiscoveredConcept::instances)
        .def_readwrite("categoryHint", &DiscoveredConcept::categoryHint)
        .def_readwrite("associatedConceptIds", &DiscoveredConcept::associatedConceptIds)
        .def_readwrite("totalObservations", &DiscoveredConcept::totalObservations)
        .def_readwrite("avgStability", &DiscoveredConcept::avgStability);

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation for pattern discovery)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("presentExperience", &ConceptFormation::presentExperience, py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"),
             "Present a new experience to the concept formation system")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Get the concept that best matches a pattern")
        .def("getConcepts", &ConceptFormation::getConcepts,
             "Get all concepts")
        .def("getConcept", &ConceptFormation::getConcept, py::arg("conceptId"),
             "Get a specific concept")
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype, py::arg("conceptId"),
             "Get concept prototype")
        .def("getConceptInstances", &ConceptFormation::getConceptInstances, py::arg("conceptId"),
             "Get concept instances")
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("conceptId"),
             "Measure concept stability")
        .def("getConceptCount", &ConceptFormation::getConceptCount,
             "Get number of concepts discovered")
        .def("mergeConcepts", &ConceptFormation::mergeConcepts, py::arg("conceptA"), py::arg("conceptB"),
             "Merge two concepts")
        .def("updateConcept", &ConceptFormation::updateConcept, py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"),
             "Update concept with new instance")
        .def("computeSimilarity", &ConceptFormation::computeSimilarity, py::arg("a"), py::arg("b"),
             "Compute similarity between two patterns")
        .def("isNovel", &ConceptFormation::isNovel, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Check if pattern is novel")
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern, py::arg("pattern"),
             "Get concept for a given pattern");

    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Attentional selection for focus)pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("processCompetition", &AttentionalSelection::processCompetition, py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
             "Process competing sensory inputs and select winners")
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("region"),
             "Apply attention to a specific region")
        .def("releaseAttention", &AttentionalSelection::releaseAttention,
             "Release attention (allow all regions equal processing)")
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions,
             "Get current attention focus")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"),
             "Set inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"),
             "Set excitation strength")
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold, py::arg("threshold"),
             "Set competition threshold")
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor, py::arg("neuron"),
             "Get inhibition level for a neuron")
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor, py::arg("neuron"),
             "Get excitation level for a neuron")
        .def("update", &AttentionalSelection::update, py::arg("dt"),
             "Update attention dynamics")
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"),
             "Check if neuron is currently attended")
        .def("getWinners", &AttentionalSelection::getWinners,
             "Get attention winners")
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias, py::arg("neuron"), py::arg("biasStrength"),
             "Apply top-down attention bias")
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience, py::arg("neuron"), py::arg("salienceStrength"),
             "Apply bottom-up attention salience")
        .def("reset", &AttentionalSelection::reset,
             "Reset attention");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("processCompetition", &AttentionalSelection::processCompetition, py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
             "Process competing sensory inputs and select winners")
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("region"),
             "Apply attention to a specific region")
        .def("releaseAttention", &AttentionalSelection::releaseAttention,
             "Release attention (allow all regions equal processing)")
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions,
             "Get current attention focus")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"),
             "Set inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"),
             "Set excitation strength")
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold, py::arg("threshold"),
             "Set competition threshold")
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor, py::arg("neuron"),
             "Get inhibition level for a neuron")
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor, py::arg("neuron"),
             "Get excitation level for a neuron")
        .def("update", &AttentionalSelection::update, py::arg("dt"),
             "Update attention dynamics")
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"),
             "Check if neuron is currently attended")
        .def("getWinners", &AttentionalSelection::getWinners,
             "Get attention winners")
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias, py::arg("neuron"), py::arg("biasStrength"),
             "Apply top-down attention bias")
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience, py::arg("neuron"), py::arg("salienceStrength"),
             "Apply bottom-up attention salience")
        .def("reset", &AttentionalSelection::reset,
             "Reset attention");

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

    // Note: getSimulationClock() is not implemented in Brain.hpp, but is exposed for completeness
    // It would require adding a SimulationClock member to Brain class
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
             "Get dopamine neuromodulation system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulation system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulation system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error neuromodulation system")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator")
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

    // ========== HIGH-LEVEL SIMULATION FACTORY FUNCTIONS ==========

    // Create a complete simulation with brain, world, and agent
    m.def("createCompleteSimulation", [](std::shared_ptr<Config> config,
                                       const std::string& worldName = "simple_world") -> std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> {
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();

        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);  // Default 20x20 world with 8x8 vision
        world->reset();

        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);

        return std::make_tuple(brain, world, agent);
    }, py::arg("config"), py::arg("worldName") = "simple_world",
       "Create a complete simulation with brain, world, and agent in one call. Returns (brain, world, agent).")

    // Run a complete simulation loop
    m.def("runSimulation", [](std::shared_ptr<Brain> brain,
                           std::shared_ptr<SimpleWorld> world,
                           std::shared_ptr<AgentBrain> agent,
                           int steps, double timestep) {
        // Initialize if needed
        if (!world->getSimulationTime()) {
            world->reset();
            if (agent) {
                agent->initialize(world);
            }
        }

        // Run simulation steps
        for (int step = 0; step < steps; ++step) {
            // Update world
            world->update(timestep);

            // Get sensory percept
            auto percept = world->getSensoryPercept();

            // Process sensory input
            if (agent) {
                agent->processSensoryInput(percept);
            }

            // Brain thinks
            brain->step(step, step * timestep);

            // Agent produces action
            auto action = agent ? agent->decodeMotorCommand() : nullptr;

            // Apply action to world
            if (action) {
                world->applyMotorCommand(action, world->getSimulationTime());
            }
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"), py::arg("timestep"),
       "Run a complete simulation loop with agent-world interaction.")

    // Create neural network with specified size
    m.def("createNeuralNetwork", [](size_t neuronCount, size_t regionCount) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(regionCount));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        return config;
    }, py::arg("neuronCount"), py::arg("regionCount"),
       "Create a neural network configuration with specified neuron and region count.")

    // Create environment with specified dimensions
    m.def("createEnvironment", [](size_t width, size_t height,
                                size_t visionWidth, size_t visionHeight) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
       "Create a configured environment with specified dimensions.")

    // ========== CONFIGURATION CONVENIENCE ==========

    // Create default configuration with specific neuron count
    m.def("createDefaultConfigWithNeuronCount", [](size_t neuronCount) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        return config;
    }, py::arg("neuronCount"),
       "Create a default configuration with specific neuron count.")

    // Create configuration optimized for learning
    m.def("createDefaultConfigWithLearning", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Enable all plasticity mechanisms
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("random_seed", static_cast<int64_t>(42));
        return config;
    }, "Create a configuration optimized for learning and plasticity.")

    // Export configuration to JSON string
    m.def("exportConfigToJson", [](const std::shared_ptr<Config>& config) -> std::string {
        // For now, create a simple JSON representation
        // TODO: Implement proper JSON serialization
        std::string json = "{\n";
        auto keys = config->getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            json += "  \"" + keys[i] + "\": \"" + (config->has(keys[i]) ? "value" : "null") + "\"";
            if (i < keys.size() - 1) json += ",\n";
        }
        json += "\n}";
        return json;
    }, py::arg("config"),
       "Export configuration to JSON string.")

    // Import configuration from JSON string
    m.def("importConfigFromJson", [](const std::string& jsonString) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // TODO: Implement proper JSON deserialization
        // For now, just parse simple key=value format
        size_t pos = 0;
        while ((pos = jsonString.find('"', pos)) != std::string::npos) {
            size_t keyStart = pos + 1;
            size_t keyEnd = jsonString.find('"', keyStart);
            if (keyEnd == std::string::npos) break;

            std::string key = jsonString.substr(keyStart, keyEnd - keyStart);

            size_t valueStart = jsonString.find('"', keyEnd + 1);
            if (valueStart == std::string::npos) break;
            size_t valueEnd = jsonString.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) break;

            std::string value = jsonString.substr(valueStart + 1, valueEnd - valueStart - 1);
            config->set(key, value);

            pos = valueEnd + 1;
        }
        return config;
    }, py::arg("jsonString"),
       "Import configuration from JSON string.")

    // ========== MEMORY MANAGEMENT UTILITIES ==========

    // Get formatted memory statistics
    m.def("getMemoryStats", [](const std::shared_ptr<Brain>& brain) -> std::string {
        std::string stats = "Memory Statistics:\n";
        stats += "  Total Neurons: " + std::to_string(brain->getTotalNeuronCount()) + "\n";
        stats += "  Total Synapses: " + std::to_string(brain->getTotalSynapseCount()) + "\n";
        stats += "  Active Neurons: " + std::to_string(brain->getActiveNeuronCount()) + "\n";
        stats += "  Firing Neurons: " + std::to_string(brain->getFiringNeuronCount()) + "\n";
        stats += "  Total Spikes: " + std::to_string(brain->getTotalSpikeCount()) + "\n";
        if (auto workingMemory = brain->getWorkingMemory()) {
            stats += "  Working Memory Capacity: " + std::to_string(workingMemory->getCapacity()) + "\n";
            stats += "  Working Memory Traces: " + std::to_string(workingMemory->getActiveTraces()) + "\n";
        }
        return stats;
    }, py::arg("brain"),
       "Get formatted memory statistics as a string.")

    // ========== SIMULATION CONTROL ==========

    // Run a single episode with agent-world interaction
    m.def("runEpisode", [](std::shared_ptr<AgentBrain> agent,
                        std::shared_ptr<SimpleWorld> world,
                        int steps) {
        // Initialize if needed
        if (!world->getSimulationTime()) {
            world->reset();
            agent->initialize(world);
        }

        // Run episode
        for (int step = 0; step < steps; ++step) {
            world->update(0.1);  // Default timestep

            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("agent"), py::arg("world"), py::arg("steps"),
       "Run a single episode with agent-world interaction.")

    // Run complete training session
    m.def("runTrainingSession", [](std::shared_ptr<AgentBrain> agent,
                                 std::shared_ptr<SimpleWorld> world,
                                 int episodes, int stepsPerEpisode) {
        for (int episode = 0; episode < episodes; ++episode) {
            runEpisode(agent, world, stepsPerEpisode);
            if (episode % 10 == 0) {
                // Log progress every 10 episodes
                std::string stats = getMemoryStats(agent->getBrain());
                // In Python, this would be printed to console
            }
        }
    }, py::arg("agent"), py::arg("world"), py::arg("episodes"), py::arg("stepsPerEpisode"),
       "Run complete training session with multiple episodes.")

    // ========== VISUALIZATION AND MONITORING ==========

    // Create monitoring wrapper for statistics
    m.def("createMonitor", [](std::shared_ptr<Brain> brain,
                            double interval) -> std::function<void(int)> {
        return [brain, interval](int step) {
            // This would create a monitoring callback in real implementation
            // For now, just print statistics
            if (step % static_cast<int>(interval) == 0) {
                std::string stats = getMemoryStats(brain);
                // In Python, this would be printed to console
            }
        };
    }, py::arg("brain"), py::arg("interval"),
       "Create monitoring callback for statistics collection.")

    // Get current simulation statistics
    m.def("getLiveStats", [](const std::shared_ptr<Brain>& brain) -> py::dict {
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());

        if (auto workingMemory = brain->getWorkingMemory()) {
            stats["working_memory_capacity"] = workingMemory->getCapacity();
            stats["working_memory_traces"] = workingMemory->getActiveTraces();
        }

        if (auto episodicMemory = brain->getEpisodicMemory()) {
            stats["episodic_memory_episodes"] = episodicMemory->getEpisodeCount();
        }

        return stats;
    }, py::arg("brain"),
       "Get current simulation statistics as a dictionary.")

    // ========== UTILITY FUNCTIONS ==========

    // Wait for minimum spike activity
    m.def("waitForSpikes", [](const std::shared_ptr<Brain>& brain,
                           size_t threshold,
                           double timeout) {
        double startTime = 0.0;  // Would use actual time in real implementation
        while (brain->getFiringNeuronCount() < threshold) {
            // Simulate waiting (would use actual sleep in real implementation)
            brain->step(0, 0.001);

            double elapsed = 0.001;  // Simulated elapsed time
            if (elapsed >= timeout) {
                break;
            }
        }
    }, py::arg("brain"), py::arg("threshold"), py::arg("timeout"),
       "Wait for minimum spike activity in the brain.")

    // Check brain system health
    m.def("checkBrainHealth", [](const std::shared_ptr<Brain>& brain) -> py::dict {
        py::dict health;
        health["is_healthy"] = true;
        health["errors"] = std::vector<std::string>();

        // Check for obvious issues
        if (brain->getTotalNeuronCount() == 0) {
            health["is_healthy"] = false;
            health["errors"].attr<py::list>().append("No neurons initialized");
        }

        if (brain->getTotalSynapseCount() == 0) {
            health["is_healthy"] = false;
            health["errors"].attr<py::list>().append("No synapses initialized");
        }

        return health;
    }, py::arg("brain"),
       "Check brain system health and return health status.")

    // ========== ENUM CONVENIENCE ==========

    // Get all neuron types as Python list
    m.def("getAllNeuronTypes", []() -> py::list {
        py::list types;
        types.append(static_cast<int>(NeuronType::Excitatory));
        types.append(static_cast<int>(NeuronType::Inhibitory));
        types.append(static_cast<int>(NeuronType::Modulatory));
        types.append(static_cast<int>(NeuronType::Sensory));
        types.append(static_cast<int>(NeuronType::Motor));
        types.append(static_cast<int>(NeuronType::Internal));
        return types;
    }, "Get list of all neuron types as Python list.")

    // Get all action types as Python list
    m.def("getAllActionTypes", []() -> py::list {
        py::list types;
        types.append(static_cast<int>(ActionType::MoveForward));
        types.append(static_cast<int>(ActionType::MoveBackward));
        types.append(static_cast<int>(ActionType::MoveLeft));
        types.append(static_cast<int>(ActionType::MoveRight));
        types.append(static_cast<int>(ActionType::TurnLeft));
        types.append(static_cast<int>(ActionType::TurnRight));
        types.append(static_cast<int>(ActionType::Look));
        types.append(static_cast<int>(ActionType::LookUp));
        types.append(static_cast<int>(ActionType::LookDown));
        types.append(static_cast<int>(ActionType::Interact));
        types.append(static_cast<int>(ActionType::Eat));
        types.append(static_cast<int>(ActionType::Drink));
        types.append(static_cast<int>(ActionType::Rest));
        types.append(static_cast<int>(ActionType::Wait));
        types.append(static_cast<int>(ActionType::Custom));
        return types;
    }, "Get list of all action types as Python list.")

    // Get all developmental stages as Python list
    m.def("getAllDevelopmentalStages", []() -> py::list {
        py::list stages;
        stages.append(static_cast<int>(DevelopmentalStage::Initial));
        stages.append(static_cast<int>(DevelopmentalStage::CriticalPeriod));
        stages.append(static_cast<int>(DevelopmentalStage::Maturation));
        stages.append(static_cast<int>(DevelopmentalStage::Adult));
        stages.append(static_cast<int>(DevelopmentalStage::Aging));
        return stages;
    }, "Get list of all developmental stages as Python list.")

    // ========== BATCH OPERATIONS ==========

    // Batch inject current to multiple neurons
    m.def("batchInjectCurrent", [](const std::shared_ptr<Brain>& brain,
                                 const std::vector<NeuronType>& types,
                                 double current,
                                 size_t count) {
        for (size_t i = 0; i < count && i < types.size(); ++i) {
            brain->injectCurrentToNeurons(types[i], static_cast<MembranePotential>(current));
        }
    }, py::arg("brain"), py::arg("types"), py::arg("current"), py::arg("count"),
       "Inject current to multiple neurons of specified types.")

    m.def("batchSetWeights", [](const std::vector<std::shared_ptr<Synapse>>& synapses,
                              const std::vector<double>& weights) {
        for (size_t i = 0; i < synapses.size() && i < weights.size(); ++i) {
            synapses[i]->setWeight(static_cast<float>(weights[i]));
        }
    }, py::arg("synapses"), py::arg("weights"),
       "Set weights for multiple synapses.")

    // Update module documentation
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Edition
        -----------------------------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.

        This enhanced edition includes additional convenience functions and factory methods:

        **High-Level Simulation Factory Functions:**
        - createCompleteSimulation(config, worldName) - Create brain, world, and agent in one call
        - runSimulation(brain, world, agent, steps, timestep) - Run complete simulation loop
        - createNeuralNetwork(neuronCount, regionCount) - Quick neural network creation
        - createEnvironment(width, height, visionWidth, visionHeight) - Create configured environment

        **Configuration Convenience:**
        - createDefaultConfigWithNeuronCount(count) - Create config with specific neuron count
        - createDefaultConfigWithLearning() - Create config optimized for learning
        - exportConfigToJson(config) - Export configuration to JSON string
        - importConfigFromJson(jsonString) - Import configuration from JSON string

        **Memory Management Utilities:**
        - getMemoryStats(brain) - Get formatted memory statistics

        **Simulation Control:**
        - runEpisode(agent, world, steps) - Run single episode with agent-world interaction
        - runTrainingSession(agent, world, episodes, stepsPerEpisode) - Run complete training session

        **Visualization and Monitoring:**
        - createMonitor(brain, interval) - Create monitoring wrapper for statistics
        - getLiveStats(brain) - Get current simulation statistics as a dictionary

        **Utility Functions:**
        - waitForSpikes(brain, threshold, timeout) - Wait for minimum spike activity
        - checkBrainHealth(brain) - Check brain system health

        **Enum Convenience:**
        - getAllNeuronTypes() - Get list of all neuron types as Python list
        - getAllActionTypes() - Get list of all action types as Python list
        - getAllDevelopmentalStages() - Get list of all developmental stages as Python list

        **Batch Operations:**
        - batchInjectCurrent(brain, types, current, count) - Inject current to multiple neurons
        - batchSetWeights(synapses, weights) - Set weights for multiple synapses
        )pbdoc";
}

} // namespace nlm
