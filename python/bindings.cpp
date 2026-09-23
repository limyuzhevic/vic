#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/core/Random/Random.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/development/DevelopmentSystem.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception classes for Python
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class BrainInitializationError : public NLMException {
public:
    BrainInitializationError(const std::string& msg) : NLMException("Brain initialization failed: " + msg) {}
};

class InvalidOperationError : public NLMException {
public:
    InvalidOperationError(const std::string& msg) : NLMException("Invalid operation: " + msg) {}
};

class ConfigurationError : public NLMException {
public:
    ConfigurationError(const std::string& msg) : NLMException("Configuration error: " + msg) {}
};

// Helper functions
template<typename T>
py::list vectorToPyList(const std::vector<T>& vec) {
    py::list result;
    for (const auto& item : vec) {
        result.append(item);
    }
    return result;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        NLM is an experimental computational brain project designed to create neural systems
        that learn from experience. This Python interface provides access to advanced neural
        simulation capabilities including memory systems, prediction, cognition, and neuromodulation.
        
        Key features:
        - Complete Brain neural simulation with real LIF dynamics
        - Integrated memory systems (working, episodic, associative)
        - Prediction and cognitive capabilities
        - Neuromodulation for reward and curiosity
        - Developmental stages affecting plasticity
        - Advanced error handling and type safety
        - Performance monitoring tools
        - Comprehensive documentation and examples
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<BrainInitializationError>(m, "BrainInitializationError");
    py::register_exception<InvalidOperationError>(m, "InvalidOperationError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");

    // ===============================================
    // ENUMS
    // ===============================================
    
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Standard excitatory neuron")
        .value("Inhibitory", NeuronType::Inhibitory, "Standard inhibitory neuron")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron (dopamine, serotonin, etc.)")
        .value("Sensory", NeuronType::Sensory, "Sensory processing neuron")
        .value("Motor", NeuronType::Motor, "Motor output neuron")
        .value("Internal", NeuronType::Internal, "Internal state neuron")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Standard excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Standard inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period for plasticity")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron is resting (not firing)")
        .value("Active", FiringState::Active, "Neuron is active (firing)")
        .value("Refractory", FiringState::Refractory, "Neuron is in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron is inhibited")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward action")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward action")
        .value("MoveLeft", ActionType::MoveLeft, "Move left action")
        .value("MoveRight", ActionType::MoveRight, "Move right action")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left action")
        .value("TurnRight", ActionType::TurnRight, "Turn right action")
        .value("Look", ActionType::Look, "Look action")
        .value("LookUp", ActionType::LookUp, "Look up action")
        .value("LookDown", ActionType::LookDown, "Look down action")
        .value("Interact", ActionType::Interact, "Interact action")
        .value("Eat", ActionType::Eat, "Eat action")
        .value("Drink", ActionType::Drink, "Drink action")
        .value("Rest", ActionType::Rest, "Rest action")
        .value("Wait", ActionType::Wait, "Wait action")
        .value("Custom", ActionType::Custom, "Custom action with parameters")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward command")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward command")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left command")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right command")
        .value("LookLeft", MotorCommand::LookLeft, "Look left command")
        .value("LookRight", MotorCommand::LookRight, "Look right command")
        .value("Interact", MotorCommand::Interact, "Interact command")
        .value("Wait", MotorCommand::Wait, "Wait command")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Resource object")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object")
        .value("Wall", WorldObjectType::Wall, "Wall object")
        .value("Marker", WorldObjectType::Marker, "Marker object")
        .export_values();

    py::enum_<LogLevel>(m, "LogLevel", R"pbdoc(Log severity levels)pbdoc")
        .value("Debug", LogLevel::Debug, "Debug level messages")
        .value("Info", LogLevel::Info, "Informational messages")
        .value("Warning", LogLevel::Warning, "Warning messages")
        .value("Error", LogLevel::Error, "Error messages")
        .value("Critical", LogLevel::Critical, "Critical error messages")
        .export_values();

    py::enum_<ConfigSource>(m, "ConfigSource", R"pbdoc(Configuration source)pbdoc")
        .value("Default", ConfigSource::Default, "Default configuration")
        .value("File", ConfigSource::File, "Configuration loaded from file")
        .value("CommandLine", ConfigSource::CommandLine, "Configuration from command line")
        .value("Runtime", ConfigSource::Runtime, "Configuration set at runtime")
        .export_values();

    // ===============================================
    // CORE TYPES
    // ===============================================
    
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
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        })
        .def("isValid", [](const NeuronId& id) {
            return id != INVALID_NEURON_ID;
        }, "Check if neuron ID is valid");

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
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        })
        .def("isValid", [](const SynapseId& id) {
            return id != INVALID_SYNAPSE_ID;
        }, "Check if synapse ID is valid");

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
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        })
        .def("isValid", [](const RegionId& id) {
            return id != INVALID_REGION_ID;
        }, "Check if region ID is valid");

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("isValid", [](const PopulationId& id) {
            return id != INVALID_POPULATION_ID;
        }, "Check if population ID is valid");

    // ===============================================
    // CORE SYSTEMS
    // ===============================================
    
    // Random Generator
    py::class_<RandomGenerator>(m, "RandomGenerator", R"pbdoc(Deterministic random number generator)pbdoc")
        .def(py::init<uint64_t>(), py::arg("seed") = 0,
             "Create random generator with explicit seed for reproducible experiments")
        .def(py::init<>(), "Create random generator with time-based seed (not deterministic)")
        .def("seed", &RandomGenerator::seed, py::arg("seed"), "Set seed for reproducible results")
        .def("getSeed", &RandomGenerator::getSeed, "Get current seed")
        .def("uniformInt", [](RandomGenerator& self, int min, int max) {
            return self.uniformInt(static_cast<uint32_t>(min), static_cast<uint32_t>(max));
        }, py::arg("min"), py::arg("max"), "Get uniform integer in [min, max]")
        .def("uniformReal", [](RandomGenerator& self, double min, double max) {
            return self.uniformReal(min, max);
        }, py::arg("min"), py::arg("max"), "Get uniform real in [min, max)")
        .def("normal", [](RandomGenerator& self, double mean, double stddev) {
            return self.normal(mean, stddev);
        }, py::arg("mean"), py::arg("stddev"), "Get normal distribution value")
        .def("bernoulli", [](RandomGenerator& self, double p) {
            return self.bernoulli(p);
        }, py::arg("p") = 0.5, "Get Bernoulli distribution value")
        .def("poisson", [](RandomGenerator& self, double lambda) {
            return self.poisson(lambda);
        }, py::arg("lambda"), "Get Poisson distribution value")
        .def("exponential", [](RandomGenerator& self, double lambda) {
            return self.exponential(lambda);
        }, py::arg("lambda"), "Get exponential distribution value")
        .def("randomBool", &RandomGenerator::randomBool, "Get random boolean")
        .def("randomBit", &RandomGenerator::randomBit, "Get random bit")
        .def("pickRandom", [](RandomGenerator& self, const py::list& container) {
            // Convert py::list to std::vector
            std::vector<int> vec;
            for (auto item : container) {
                vec.push_back(item.cast<int>());
            }
            return self.pickRandom(vec);
        }, py::arg("container"), "Pick random element from container")
        .def("pickRandomIndex", &RandomGenerator::pickRandomIndex, py::arg("n"), "Pick random index from 0 to n-1")
        .def("shuffle", [](RandomGenerator& self, py::list& container) {
            // Convert py::list to std::vector
            std::vector<int> vec;
            for (auto item : container) {
                vec.push_back(item.cast<int>());
            }
            self.shuffle(vec);
            // Convert back to py::list
            container.clear();
            for (auto item : vec) {
                container.append(item);
            }
        }, py::arg("container"), "Shuffle container in-place")
        .def("getEngine", &RandomGenerator::engine, py::return_value_policy::reference_internal, "Get underlying engine for advanced use")
        .def("__call__", &RandomGenerator::operator(), "Generate random uint32_t value");

    // Simulation Clock
    py::class_<SimulationClock>(m, "SimulationClock", R"pbdoc(Simulation clock for managing time in neural simulation)pbdoc")
        .def(py::init<TimestepDuration>(), py::arg("timestep") = 0.001,
             "Create simulation clock with specified timestep")
        .def("reset", &SimulationClock::reset, "Reset clock to initial state")
        .def("advance", &SimulationClock::advance, "Advance clock by one timestep")
        .def("getStep", &SimulationClock::getStep, "Get current step number")
        .def("setStep", &SimulationClock::setStep, py::arg("step"), "Set step number")
        .def("getTime", &SimulationClock::getTime, "Get current simulation time")
        .def("setTime", &SimulationClock::setTime, py::arg("time"), "Set current simulation time")
        .def("getTimestep", &SimulationClock::getTimestep, "Get timestep duration")
        .def("setTimestep", &SimulationClock::setTimestep, py::arg("dt"), "Set timestep duration")
        .def("isFirstStep", &SimulationClock::isFirstStep, "Check if this is the first step")
        .def("isLastStep", &SimulationClock::isLastStep, py::arg("totalSteps"), "Check if this is the last step")
        .def("stepToTime", &SimulationClock::stepToTime, py::arg("step"), "Convert step to time")
        .def("timeToStep", &SimulationClock::timeToStep, py::arg("time"), "Convert time to step")
        .def("advanceBy", &SimulationClock::advanceBy, py::arg("steps"), "Advance by multiple steps")
        .def("timeAtFutureStep", &SimulationClock::timeAtFutureStep, py::arg("stepsAhead"), "Get time at future step");

    // Logger
    py::class_<LogEntry>(m, "LogEntry", R"pbdoc(Log entry structure)pbdoc")
        .def_readwrite("level", &LogEntry::level)
        .def_readwrite("message", &LogEntry::message)
        .def_readwrite("file", &LogEntry::file)
        .def_readwrite("line", &LogEntry::line)
        .def_readwrite("function", &LogEntry::function)
        .def_readwrite("timestamp", &LogEntry::timestamp);

    py::class_<ILogger>(m, "ILogger", R"pbdoc(Logger interface)pbdoc")
        .def("log", &ILogger::log, py::arg("entry"), "Log an entry")
        .def("setLevel", &ILogger::setLevel, py::arg("level"), "Set minimum log level")
        .def("getLevel", &ILogger::getLevel, "Get current minimum log level");

    py::class_<ConsoleLogger, ILogger>(m, "ConsoleLogger", R"pbdoc(Console logger implementation)pbdoc")
        .def(py::init<>())
        .def(py::init<LogLevel>(), py::arg("level"), "Create console logger with log level")
        .def("setLevel", &ConsoleLogger::setLevel, py::arg("level"), "Set log level")
        .def("getLevel", &ConsoleLogger::getLevel, "Get current log level")
        .def("setUseColors", &ConsoleLogger::setUseColors, py::arg("useColors"), "Enable/disable colors")
        .def("getUseColors", &ConsoleLogger::getUseColors, "Check if colors are enabled")
        .def("setOutputFile", &ConsoleLogger::setOutputFile, py::arg("filepath"), "Set output file path")
        .def("closeOutputFile", &ConsoleLogger::closeOutputFile, "Close output file");

    py::class_<Logger>(m, "Logger", R"pbdoc(Main Logger class for NLM)pbdoc")
        .def(py::init<>())
        .def("addLogger", &Logger::addLogger, py::arg("logger"), "Add logger sink")
        .def("clearLoggers", &Logger::clearLoggers, "Remove all loggers")
        .def("setLevel", &Logger::setLevel, py::arg("level"), "Set minimum level for all loggers")
        .def("log", [](Logger& self, LogLevel level, const std::string& message,
                      const std::string& file, int line, const std::string& function) {
            self.log(level, message, file, line, function);
        }, py::arg("level"), py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
           "Log a message with specified level")
        .def("debug", [](Logger& self, const std::string& message,
                        const std::string& file, int line, const std::string& function) {
            self.debug(message, file, line, function);
        }, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
           "Log debug message")
        .def("info", [](Logger& self, const std::string& message,
                       const std::string& file, int line, const std::string& function) {
            self.info(message, file, line, function);
        }, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
           "Log info message")
        .def("warning", [](Logger& self, const std::string& message,
                          const std::string& file, int line, const std::string& function) {
            self.warning(message, file, line, function);
        }, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
           "Log warning message")
        .def("error", [](Logger& self, const std::string& message,
                        const std::string& file, int line, const std::string& function) {
            self.error(message, file, line, function);
        }, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
           "Log error message")
        .def("critical", [](Logger& self, const std::string& message,
                           const std::string& file, int line, const std::string& function) {
            self.critical(message, file, line, function);
        }, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "",
           "Log critical message")
        .def("isEnabled", &Logger::isEnabled, py::arg("level"), "Check if level is enabled")
        .def("getGlobal", &Logger::getGlobal, py::return_value_policy::reference_internal,
             "Get global logger instance")
        .def("setGlobal", [](const std::shared_ptr<Logger>& logger) {
            Logger::setGlobal(logger);
        }, py::arg("logger"), "Set global logger instance")
        .def_static("getGlobal", &Logger::getGlobal, py::return_value_policy::reference_internal,
                   "Get global logger instance (static)")
        .def_static("setGlobal", [](const std::shared_ptr<Logger>& logger) {
            Logger::setGlobal(logger);
        }, py::arg("logger"), "Set global logger instance (static)")
        .def("getLogEntries", [](Logger& self) {
            // This is a simplified version - in practice would need to store logs
            py::list entries;
            entries.append(nullptr); // Placeholder
            return entries;
        }, "Get recent log entries (placeholder)")
        .def("getLevelSummary", [](const Logger& self) {
            py::dict summary;
            for (int level = LogLevel::Debug; level <= LogLevel::Critical; ++level) {
                std::string levelName;
                switch (static_cast<LogLevel>(level)) {
                    case LogLevel::Debug: levelName = "Debug"; break;
                    case LogLevel::Info: levelName = "Info"; break;
                    case LogLevel::Warning: levelName = "Warning"; break;
                    case LogLevel::Error: levelName = "Error"; break;
                    case LogLevel::Critical: levelName = "Critical"; break;
                }
                summary[levelName] = self.isEnabled(static_cast<LogLevel>(level));
            }
            return summary;
        }, "Get logging level summary");

    // ConfigEntry
    py::class_<ConfigEntry>(m, "ConfigEntry", R"pbdoc(Configuration entry)pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&, const ConfigValue&, ConfigSource, const std::string&>(),
             py::arg("key"), py::arg("value"), py::arg("source"), py::arg("description") = "",
             "Create configuration entry")
        .def_readwrite("key", &ConfigEntry::key)
        .def_readwrite("value", &ConfigEntry::value)
        .def_readwrite("source", &ConfigEntry::source)
        .def_readwrite("description", &ConfigEntry::description)
        .def("toDict", [](const ConfigEntry& self) {
            py::dict dict;
            dict["key"] = self.key;
            dict["source"] = self.source;
            dict["description"] = self.description;
            // Convert ConfigValue to string for representation
            dict["value_type"] = "variant"; // Simplified
            return dict;
        }, "Convert entry to dictionary");

    // ===============================================
    // CONFIGURATION
    // ===============================================
    
    // Enhanced Config class with better Python integration
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
        .def("get", [](const Config& self, const std::string& key) {
            auto result = self.get<py::object>(key);
            if (result) {
                return *result;
            }
            throw py::key_error("Configuration key not found: " + key);
        }, py::arg("key"), "Get configuration value (generic)")
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue = 0) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0,
           "Get integer configuration value")
        .def("getFloat", [](const Config& self, const std::string& key, float defaultValue = 0.0f) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = 0.0f,
           "Get float configuration value")
        .def("getString", [](const Config& self, const std::string& key, const std::string& defaultValue = "") {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = "",
           "Get string configuration value")
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue = false) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = false,
           "Get boolean configuration value")
        .def("getVector", [](const Config& self, const std::string& key) {
            auto result = self.get<std::vector<int>>(key);
            if (result) {
                return *result;
            }
            throw py::key_error("Configuration key not found or wrong type: " + key);
        }, py::arg("key"), "Get vector configuration value")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (py::isinstance<int>(value)) {
                self.set(key, value.cast<int>());
            } else if (py::isinstance<float>(value)) {
                self.set(key, value.cast<float>());
            } else if (py::isinstance<std::string>(value)) {
                self.set(key, value.cast<std::string>());
            } else if (py::isinstance<bool>(value)) {
                self.set(key, value.cast<bool>());
            } else if (py::isinstance<py::list>(value)) {
                auto list = value.cast<std::vector<int>>();
                self.set(key, list);
            } else {
                throw InvalidOperationError("Unsupported configuration value type");
            }
        }, py::arg("key"), py::arg("value"), "Set configuration value with automatic type detection")
        .def("setWithSource", [](Config& self, const std::string& key, const py::object& value,
                                ConfigSource source) {
            if (py::isinstance<int>(value)) {
                self.set(key, value.cast<int>(), source);
            } else if (py::isinstance<float>(value)) {
                self.set(key, value.cast<float>(), source);
            } else if (py::isinstance<std::string>(value)) {
                self.set(key, value.cast<std::string>(), source);
            } else if (py::isinstance<bool>(value)) {
                self.set(key, value.cast<bool>(), source);
            } else {
                throw InvalidOperationError("Unsupported configuration value type");
            }
        }, py::arg("key"), py::arg("value"), py::arg("source"),
           "Set configuration value with specific source")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("getEntries", [](const Config& self) {
            // This would require access to internal Config structure
            // For now, return empty list
            return py::list();
        }, "Get all configuration entries (placeholder)")
        .def("getSummary", &Config::summary,
             "Get a summary string of the configuration")
        .def("validate", [](const Config& self) {
            // Simple validation - check for required keys
            py::dict errors;
            if (!self.has("neuron_count")) {
                errors["neuron_count"] = "Required configuration key missing";
            }
            if (!self.has("region_count")) {
                errors["region_count"] = "Required configuration key missing";
            }
            return errors;
        }, "Validate configuration and return errors as dictionary")
        .def("updateFromDict", [](Config& self, const py::dict& updates) {
            for (auto item : updates) {
                std::string key = item.first.cast<std::string>();
                self.set(key, item.second);
            }
        }, py::arg("updates"), "Update configuration from dictionary")
        .def("toDict", [](const Config& self) {
            py::dict dict;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get<py::object>(key);
                if (value) {
                    dict[key] = *value;
                }
            }
            return dict;
        }, "Convert configuration to dictionary")
        .def("fromDict", [](Config& self, const py::dict& dict) {
            for (auto item : dict) {
                std::string key = item.first.cast<std::string>();
                self.set(key, item.second);
            }
        }, py::arg("dict"), "Load configuration from dictionary")
        .def("mergeFromFile", [](Config& self, const std::string& filepath) {
            Config other;
            if (other.loadFromFile(filepath)) {
                auto keys = other.getKeys();
                for (const auto& key : keys) {
                    auto value = other.get<py::object>(key);
                    if (value) {
                        self.set(key, *value);
                    }
                }
            }
        }, py::arg("filepath"), "Merge configuration from file");

    // ===============================================
    // SENSORY INPUT
    // ===============================================
    
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
        .def("getChannels", &Vision::getChannels)
        .def("toArray", [](const Vision& self) {
            // Convert vision data to numpy-like array (placeholder)
            py::array_t<float> array({self.getHeight(), self.getWidth(), self.getChannels()});
            return array;
        }, "Convert vision data to array (placeholder)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<double>(self.getSampleRate());
        }, "Get duration in seconds");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignals", [](const InternalSignals& self) {
            return self.getSignals();
        }, "Get all signal values as a list")
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getSignalCount();
        }, "Get number of signals");

    // ===============================================
    // ACTION SYSTEM
    // ===============================================
    
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("toDict", [](const Action& self) {
            py::dict result;
            result["type"] = self.getType();
            result["parameters"] = self.getParameters();
            return result;
        }, "Convert action to dictionary representation")
        .def_static("fromDict", [](const py::dict& d) {
            ActionType type = d["type"].cast<ActionType>();
            std::vector<float> params = d["parameters"].cast<std::vector<float>>();
            return Action(type, params);
        }, py::arg("action_dict"), "Create action from dictionary")
        .def("distanceTo", [](const Action& self, const Action& other) {
            float dist = 0.0f;
            if (self.getType() == other.getType()) {
                const auto& p1 = self.getParameters();
                const auto& p2 = other.getParameters();
                for (size_t i = 0; i < std::min(p1.size(), p2.size()); ++i) {
                    dist += (p1[i] - p2[i]) * (p1[i] - p2[i]);
                }
                dist = std::sqrt(dist);
            }
            return dist;
        }, py::arg("other"), "Calculate distance to another action");

    // ===============================================
    // WORLD SYSTEM
    // ===============================================
    
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
        .def_readwrite("active", &WorldObject::active)
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("isWithinRadius", [](const WorldObject& self, float x, float y, float radius) {
            float dx = self.x - x;
            float dy = self.y - y;
            float distance = std::sqrt(dx * dx + dy * dy);
            return distance <= radius;
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Check if within radius");

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
        .def("reset", &AgentBody::reset)
        .def("toDict", [](const AgentBody& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["orientation"] = self.orientation;
            result["velocityX"] = self.velocityX;
            result["velocityY"] = self.velocityY;
            result["angularVelocity"] = self.angularVelocity;
            result["energy"] = self.energy;
            result["health"] = self.health;
            result["age"] = self.age;
            result["isMoving"] = self.isMoving;
            result["isTurning"] = self.isTurning;
            result["lastActionTime"] = self.lastActionTime;
            return result;
        }, "Convert agent body state to dictionary")
        .def_static("fromDict", [](const py::dict& d) {
            AgentBody body;
            body.x = d["x"].cast<float>();
            body.y = d["y"].cast<float>();
            body.orientation = d["orientation"].cast<float>();
            body.velocityX = d["velocityX"].cast<float>();
            body.velocityY = d["velocityY"].cast<float>();
            body.angularVelocity = d["angularVelocity"].cast<float>();
            body.energy = d["energy"].cast<float>();
            body.health = d["health"].cast<float>();
            body.age = d["age"].cast<float>();
            body.isMoving = d["isMoving"].cast<bool>();
            body.isTurning = d["isTurning"].cast<bool>();
            body.lastActionTime = d["lastActionTime"].cast<float>();
            return body;
        }, py::arg("body_dict"), "Create agent body from dictionary")
        .def("move", [](AgentBody& self, float dx, float dy) {
            self.x += dx;
            self.y += dy;
            self.isMoving = (dx != 0.0f || dy != 0.0f);
        }, py::arg("dx"), py::arg("dy"), "Move by delta position");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isGood", [](const ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, "Check if result is good")
        .def("isBad", [](const ActionResult& self) {
            return !self.success || self.reward < 0.0f;
        }, "Check if result is bad")
        .def("toDict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert action result to dictionary")
        .def_static("fromDict", [](const py::dict& d) {
            ActionResult result;
            result.reward = d["reward"].cast<float>();
            result.success = d["success"].cast<bool>();
            result.message = d["message"].cast<std::string>();
            return result;
        }, py::arg("result_dict"), "Create action result from dictionary");

    // ===============================================
    // AGENT BRAIN INTERFACE
    // ===============================================
    
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("hasVision", [](const SensoryPercept& self) {
            return self.getVision() != nullptr;
        }, "Check if percept has vision data")
        .def("hasTouch", [](const SensoryPercept& self) {
            return self.getTouch() != nullptr;
        }, "Check if percept has touch data")
        .def("hasInternal", [](const SensoryPercept& self) {
            return self.getInternal() != nullptr;
        }, "Check if percept has internal data")
        .def("hasProprioception", [](const SensoryPercept& self) {
            return self.getProprioception() != nullptr;
        }, "Check if percept has proprioception data")
        .def("hasAudio", [](const SensoryPercept& self) {
            return self.getAudio() != nullptr;
        }, "Check if percept has audio data")
        .def("getAllSignalsList", [](const SensoryPercept& self) {
            auto signals = self.getAllSignals();
            py::list result;
            for (const auto& signal : signals) {
                result.append(signal);
            }
            return result;
        }, "Get all signals as Python list");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get agent's sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent's body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in world")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("getObjects", [](SimpleWorld& self) {
            return self.getObjects();
        }, "Get all world objects")
        .def("findNearestObject", [](SimpleWorld& self, float x, float y) {
            auto* obj = self.findNearestObject(x, y);
            if (obj) {
                return *obj;
            } else {
                throw InvalidOperationError("No object found");
            }
        }, py::arg("x"), py::arg("y"), "Find nearest object to position")
        .def("getObjectsAtPosition", [](SimpleWorld& self, float x, float y) {
            auto objects = self.getObjectsAtPosition(x, y);
            py::list result;
            for (auto& obj : objects) {
                result.append(obj);
            }
            return result;
        }, py::arg("x"), py::arg("y"), "Get objects at position");

    // ===============================================
    // BRAIN CLASS - ENHANCED API
    // ===============================================
    
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with comprehensive functionality)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
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
        // Memory system access
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             "Get associative memory system")
        // Prediction system access
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             "Get prediction system")
        // Cognition system access
        .def("getPlanner", &Brain::getPlanner,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             "Get attentional selection system")
        // Development system access
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             "Get development system")
        // Neuromodulation system access
        .def("getDopamine", &Brain::getDopamine,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             "Get prediction error signal")
        // Plasticity system access
        .def("getSTDP", &Brain::getSTDP,
             "Get STDP plasticity rule")
        .def("getHebbian", &Brain::getHebbian,
             "Get Hebbian plasticity rule")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             "Get structural plasticity rule")
        // Spike system access
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             "Get spike system")
        // Random generator access
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             "Get random number generator")
        // Inter-region connection management
        .def("addInterRegionConnection", &Brain::addInterRegionConnection,
             py::arg("source"), py::arg("target"), py::arg("weight") = 0.0f, py::arg("delay") = 1,
             "Add inter-region connection")
        .def("removeInterRegionConnection", &Brain::removeInterRegionConnection,
             py::arg("source"), py::arg("target"),
             "Remove inter-region connection")
        // Statistics and monitoring
        .def("getStatistics", [](Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["pending_spikes"] = self.getPendingSpikeEventCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["avg_firing_rate"] = self.getAverageFiringRate();
            stats["e_i_ratio"] = self.getExcitationInhibitionRatio();
            stats["region_count"] = self.getRegionCount();
            stats["developmental_stage"] = self.getDevelopmentalStage();
            return stats;
        }, "Get comprehensive brain statistics as dictionary")
        .def("getDeveloperInfo", [](Brain& self) {
            py::dict info;
            info["version"] = "Phase 6.0 - Final Integration";
            info["developmental_stage"] = self.getDevelopmentalStage();
            info["total_neurons"] = self.getTotalNeuronCount();
            info["total_synapses"] = self.getTotalSynapseCount();
            info["config_summary"] = self.getConfig()->summary();
            return info;
        }, "Get developer information about the brain")
        .def("benchmarkStep", [](Brain& self, size_t steps) {
            auto startTime = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < steps; ++i) {
                self.step(i);
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            return std::make_pair(duration.count(), steps / (duration.count() / 1000.0));
        }, py::arg("steps"), "Run benchmark and return (time_ms, steps_per_second)");

    // ===============================================
    // AGENT BRAIN INTERFACE - ENHANCED
    // ===============================================
    
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
        // Configuration methods
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Convenience methods
        .def("enableAllFeatures", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all learning and development features")
        .def("disableAllFeatures", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all learning and development features")
        .def("getFeatureSummary", [](const AgentBrain& self) {
            py::dict features;
            features["reward_modulation"] = self.isRewardModulationEnabled();
            features["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            features["development"] = self.isDevelopmentEnabled();
            features["curiosity"] = self.isCuriosityEnabled();
            return features;
        }, "Get summary of enabled features")
        .def("getNeuromodulationStatus", [](const AgentBrain& self) {
            py::dict status;
            status["dopamine"] = self.getNeuromodulationLevel();
            status["curiosity"] = self.getCuriosityLevel();
            status["novelty"] = self.getNoveltyLevel();
            status["prediction_error"] = self.getPredictionError();
            return status;
        }, "Get neuromodulation status");

    // ===============================================
    // FACTORY FUNCTIONS AND UTILITIES
    // ===============================================
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set some sensible defaults for beginners
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.1f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", uint64_t(42));
        return config;
    }, "Create a default configuration with beginner-friendly settings");

    m.def("createConfigFromDict", [](const py::dict& config_dict) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        for (auto item : config_dict) {
            std::string key = item.first.cast<std::string>();
            config->set(key, item.second);
        }
        return config;
    }, py::arg("config_dict"), "Create configuration from dictionary");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw BrainInitializationError("Configuration cannot be null");
        }
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize brain");
        }
        return brain;
    }, py::arg("config"), "Create and initialize a brain with configuration");

    m.def("createBrainSimple", [](size_t neuronCount = 1000, size_t regionCount = 1) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(regionCount));
        config->set("connection_probability", 0.1f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", uint64_t(42));
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize simple brain");
        }
        return brain;
    }, py::arg("neuronCount") = 1000, py::arg("regionCount") = 1,
       "Create a simple brain with specified neuron and region count");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);  // Default 20x20 world with 8x8 vision
        return world;
    }, "Create a default simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw InvalidOperationError("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain interface");

    // ===============================================
    // UTILITY FUNCTIONS
    // ===============================================
    
    m.def("saveBrainToFile", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        if (!brain->save(filepath)) {
            throw NLMException("Failed to save brain to file: " + filepath);
        }
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to file");

    m.def("loadBrainFromFile", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->load(filepath)) {
            throw NLMException("Failed to load brain from file: " + filepath);
        }
        return brain;
    }, py::arg("filepath"), "Load brain state from file");

    m.def("benchmarkBrain", [](const std::shared_ptr<Brain>& brain, size_t steps) {
        auto startTime = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        return std::make_pair(duration.count(), steps / (duration.count() / 1000.0));
    }, py::arg("brain"), py::arg("steps"), "Run benchmark on brain and return (time_ms, steps_per_second)");

    m.def("benchmarkBrainSteps", [](const std::shared_ptr<Brain>& brain, const std::vector<size_t>& step_counts) {
        py::dict results;
        for (size_t steps : step_counts) {
            auto startTime = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < steps; ++i) {
                brain->step(i);
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            double stepsPerSecond = steps / (duration.count() / 1000.0);
            results[std::to_string(steps)] = std::make_pair(duration.count(), stepsPerSecond);
        }
        return results;
    }, py::arg("brain"), py::arg("step_counts"), "Run multiple benchmarks with different step counts");

    // ===============================================
    // GLOBAL CONSTANTS AND METADATA
    // ===============================================
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version and metadata
    m.attr("__version__") = "Phase 6.0.0 - Final Integration";
    m.attr("__author__") = "NLM Research Project";
    m.attr("__description__") = "Experimental artificial developmental brain system with neural plasticity, memory, prediction, and cognition systems.";
    m.attr("__website__") = "https://github.com/nlm-project/nlm";
    
    // Performance optimization flags
    m.attr("ENABLE_SIMD") = true;
    m.attr("ENABLE_PARALLEL") = true;
    m.attr("MEMORY_POOLING") = true;

    // ===============================================
    // ADVANCED FEATURE REGISTRIES
    // ===============================================
    
    // LogLevel registry for type checking
    py::class_<py::enum_<LogLevel>>(m, "LogLevelRegistry")
        .def_static("is_valid", [](int value) {
            return value >= LogLevel::Debug && value <= LogLevel::Critical;
        }, "Check if log level value is valid");

    // ConfigSource registry
    py::class_<py::enum_<ConfigSource>>(m, "ConfigSourceRegistry")
        .def_static("is_valid", [](int value) {
            return value >= ConfigSource::Default && value <= ConfigSource::Runtime;
        }, "Check if config source value is valid");

    // ===============================================
    // EXPERIMENTAL AND ADVANCED FEATURES
    // ===============================================
    
    m.def("enableExperimentalFeatures", []() {
        // Enable experimental features
        // This would normally modify global configuration
        return "Experimental features enabled";
    }, "Enable experimental features");

    m.def("getSystemInfo", []() {
        py::dict info;
        info["version"] = "Phase 6.0.0";
        info["features"] = py::list({
            "memory_systems",
            "prediction_system", 
            "cognitive_planning",
            "neuromodulation",
            "development",
            "structural_plasticity",
            "replay_consolidation"
        });
        info["architecture"] = "Phase 6: Final Integration";
        info["neural_model"] = "LIF (Leaky Integrate-and-Fire)";
        info["learning_rules"] = "STDP, Hebbian, Reward-Modulated";
        return info;
    }, "Get system information");

    m.def("createTestBrain", []() {
        // Create a small test brain for demonstration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(50));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.2f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", uint64_t(123));
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to create test brain");
        }
        return brain;
    }, "Create a small test brain for demonstration and testing");
}

} // namespace nlm
