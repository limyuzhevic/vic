#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/cli/AdvancedCLI.hpp"
#include "../src/cli/ConfigurationManager.hpp"
#include "../src/cli/SimulationController.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Enhanced with AdvancedCLI, ConfigurationManager, and SimulationController support.
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

    // Enhanced Config class with validation support
    py::class_<Config>(m, "Config", R"pbdoc(Advanced Configuration class for NLM system)pbdoc")
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
        .def("getOr", &Config::getOr<int64_t>, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default (int64_t)")
        .def("getOr", &Config::getOr<double>, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default (double)")
        .def("getOr", &Config::getOr<float>, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default (float)")
        .def("getOr", &Config::getOr<bool>, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default (bool)")
        .def("getOr", &Config::getOr<std::string>, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default (string)")
        .def("set", (void (Config::*)(const std::string&, const ConfigValue&, ConfigSource)) &Config::set,
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value with source")
        .def("set", (void (Config::*)(const std::string&, int)) &Config::set,
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value (int)")
        .def("set", (void (Config::*)(const std::string&, double)) &Config::set,
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value (double)")
        .def("set", (void (Config::*)(const std::string&, bool)) &Config::set,
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value (bool)")
        .def("set", (void (Config::*)(const std::string&, const std::string&)) &Config::set,
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value (string)")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Advanced ConfigurationManager class
    py::class_<ConfigurationManager>(m, "ConfigurationManager", R"pbdoc(Advanced configuration manager with JSON/YAML support)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &ConfigurationManager::loadFromFile, py::arg("filepath"),
             "Load configuration from file (auto-detects format)")
        .def("loadFromJSON", &ConfigurationManager::loadFromJSON, py::arg("filepath"),
             "Load configuration from JSON file")
        .def("loadFromYAML", &ConfigurationManager::loadFromYAML, py::arg("filepath"),
             "Load configuration from YAML file")
        .def("saveToFile", &ConfigurationManager::saveToFile, py::arg("filepath"), py::arg("format") = 0,
             "Save configuration to file with specified format")
        .def("saveToJSON", &ConfigurationManager::saveToJSON, py::arg("filepath"),
             "Save configuration to JSON file")
        .def("saveToYAML", &ConfigurationManager::saveToYAML, py::arg("filepath"),
             "Save configuration to YAML file")
        .def("validate", &ConfigurationManager::validate,
             "Validate configuration and return validation result")
        .def("compareWith", &ConfigurationManager::compareWith, py::arg("other"),
             "Compare with another config and return diff")
        .def("getConfig", &ConfigurationManager::getConfig,
             "Get the underlying config object")
        .def("mergeWithArgs", &ConfigurationManager::mergeWithArgs, py::arg("argc"), py::arg("argv"),
             "Merge with command line arguments")
        .def("getDefaultConfig", &ConfigurationManager::getDefaultConfig,
             "Get default configuration template")
        .def("loadFromEnvironment", &ConfigurationManager::loadFromEnvironment,
             "Load configuration from environment variables")
        .def("applyTemplate", &ConfigurationManager::applyTemplate, py::arg("templateName"),
             "Apply a configuration template")
        .def("getAvailableTemplates", &ConfigurationManager::getAvailableTemplates,
             "Get list of available configuration templates")
        .def("generateReport", &ConfigurationManager::generateReport,
             "Generate a configuration report");

    // Enhanced SimulationController class
    py::class_<SimulationController>(m, "SimulationController", R"pbdoc(Advanced simulation controller with pause/resume, step-through, and speed control)pbdoc")
        .def(py::init<>())
        .def("initialize", &SimulationController::initialize, py::arg("config"), py::arg("clock"),
             "Initialize simulation controller")
        .def("start", &SimulationController::start,
             "Start simulation")
        .def("pause", &SimulationController::pause,
             "Pause simulation")
        .def("resume", &SimulationController::resume,
             "Resume simulation")
        .def("stop", &SimulationController::stop,
             "Stop simulation")
        .def("step", &SimulationController::step,
             "Execute single simulation step")
        .def("setSpeed", &SimulationController::setSpeed, py::arg("factor"),
             "Set simulation speed factor")
        .def("getSpeed", &SimulationController::getSpeed,
             "Get current simulation speed")
        .def("setTimestep", &SimulationController::setTimestep, py::arg("timestep"),
             "Set simulation timestep")
        .def("getTimestep", &SimulationController::getTimestep,
             "Get current simulation timestep")
        .def("setControlMode", &SimulationController::setControlMode, py::arg("mode"),
             "Set control mode")
        .def("getControlMode", &SimulationController::getControlMode,
             "Get current control mode")
        .def("getStats", &SimulationController::getStats,
             "Get simulation statistics")
        .def("getState", &SimulationController::getState,
             "Get current simulation state")
        .def("isPaused", &SimulationController::isPaused,
             "Check if simulation is paused")
        .def("isRunning", &SimulationController::isRunning,
             "Check if simulation is running")
        .def("isStopped", &SimulationController::isStopped,
             "Check if simulation is stopped");

    // Advanced CLI class
    py::class_<AdvancedCLI>(m, "AdvancedCLI", R"pbdoc(Advanced command-line interface with subcommand support)pbdoc")
        .def(py::init<>())
        .def("parseArgs", &AdvancedCLI::parseArgs, py::arg("argc"), py::arg("argv"),
             "Parse command line arguments")
        .def("getOptions", &AdvancedCLI::getOptions,
             "Get parsed command line options")
        .def("execute", &AdvancedCLI::execute,
             "Execute the requested subcommand")
        .def("printHelp", &AdvancedCLI::printHelp,
             "Print help information")
        .def("printVersion", &AdvancedCLI::printVersion,
             "Print version information")
        .def("getConfig", &AdvancedCLI::getConfig,
             "Get current config")
        .def("setConfig", &AdvancedCLI::setConfig, py::arg("config"),
             "Set config");

    // Helper functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createConfigurationManager", []() -> std::shared_ptr<ConfigurationManager> {
        return std::make_shared<ConfigurationManager>();
    }, "Create a configuration manager");

    m.def("createSimulationController", []() -> std::shared_ptr<SimulationController> {
        return std::make_shared<SimulationController>();
    }, "Create a simulation controller");

    m.def("createAdvancedCLI", []() -> std::shared_ptr<AdvancedCLI> {
        return std::make_shared<AdvancedCLI>();
    }, "Create an advanced CLI");

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

    // Configuration format enum
    py::enum_<ConfigurationManager::ConfigFormat>(m, "ConfigFormat")
        .value("PlainText", ConfigurationManager::ConfigFormat::PlainText)
        .value("JSON", ConfigurationManager::ConfigFormat::JSON)
        .value("YAML", ConfigurationManager::ConfigFormat::YAML)
        .export_values();

    // Simulation state enum
    py::enum_<SimulationController::SimulationState>(m, "SimulationState")
        .value("Stopped", SimulationController::SimulationState::Stopped)
        .value("Running", SimulationController::SimulationState::Running)
        .value("Paused", SimulationController::SimulationState::Paused)
        .value("Stepping", SimulationController::SimulationState::Stepping)
        .value("Finished", SimulationController::SimulationState::Finished)
        .export_values();

    // Control mode enum
    py::enum_<SimulationController::ControlMode>(m, "ControlMode")
        .value("RealTime", SimulationController::ControlMode::RealTime)
        .value("FixedStep", SimulationController::ControlMode::FixedStep)
        .value("VariableStep", SimulationController::ControlMode::VariableStep)
        .value("SpeedControl", SimulationController::ControlMode::SpeedControl)
        .export_values();

    // Config source enum
    py::enum_<ConfigSource>(m, "ConfigSource")
        .value("Default", ConfigSource::Default)
        .value("File", ConfigSource::File)
        .value("CommandLine", ConfigSource::CommandLine)
        .value("Runtime", ConfigSource::Runtime)
        .export_values();

    // Validation result structure
    py::class_<ValidationResult>(m, "ValidationResult")
        .def(py::init<>())
        .def_readwrite("valid", &ValidationResult::valid)
        .def_readwrite("errors", &ValidationResult::errors)
        .def_readwrite("warnings", &ValidationResult::warnings)
        .def("addError", &ValidationResult::addError, py::arg("error"))
        .def("addWarning", &ValidationResult::addWarning, py::arg("warning"));

    // Config diff structure
    py::class_<ConfigDiff>(m, "ConfigDiff")
        .def(py::init<>())
        .def_readwrite("changes", &ConfigDiff::changes)
        .def_readwrite("addedKeys", &ConfigDiff::addedKeys)
        .def_readwrite("removedKeys", &ConfigDiff::removedKeys)
        .def("hasChanges", &ConfigDiff::hasChanges);

    // Simulation stats structure
    py::class_<SimulationStats>(m, "SimulationStats")
        .def(py::init<>())
        .def_readwrite("totalSteps", &SimulationStats::totalSteps)
        .def_readwrite("currentStep", &SimulationStats::currentStep)
        .def_readwrite("elapsedTime", &SimulationStats::elapsedTime)
        .def_readwrite("simulatedTime", &SimulationStats::simulatedTime)
        .def_readwrite("spikeCount", &SimulationStats::spikeCount)
        .def_readwrite("averageFiringRate", &SimulationStats::averageFiringRate)
        .def_readwrite("state", &SimulationStats::state)
        .def_readwrite("controlMode", &SimulationStats::controlMode)
        .def_readwrite("speedFactor", &SimulationStats::speedFactor)
        .def_readwrite("timestep", &SimulationStats::timestep);
}

} // namespace nlm