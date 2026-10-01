#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include "../src/core/ErrorHandling/ErrorHandling.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/environment/Vision.hpp"
#include "../src/environment/Audio.hpp"
#include "../src/environment/InternalSignals.hpp"
#include "../src/environment/SensoryInput.hpp"
#include "../src/environment/Action.hpp"
#include "../src/world/AgentBody.hpp"
#include "../src/world/ActionResult.hpp"
#include "../src/world/SensoryPercept.hpp"
#include "../src/environment/WorldObject.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralSemanticMemory.hpp"
#include "../src/memory/NeuralProceduralMemory.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Acetylcholine.hpp"
#include "../src/neuromodulation/Norepinephrine.hpp"
#include "../src/neuromodulation/Serotonin.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/performance/PerformanceMonitor.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/experiments/ExperimentRunner.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Advanced Edition
        -------------------------------------------------------------
        Advanced Python bindings for the NLM C++ neural simulation framework.
        Provides comprehensive APIs for all NLM systems including advanced
        development, neuromodulation, memory, prediction, performance,
        export, tool, plugin, and error handling capabilities.
        
        Features:
        - Advanced Brain APIs (development, neuromodulation, memory, prediction, performance)
        - Advanced AgentBrain APIs (cognitive, integration, development)
        - Advanced Config APIs (schema, validation, analysis)
        - Advanced SimpleWorld APIs (metrics, environment, performance)
        - Advanced SensoryInput APIs (vision, audio, internal signals)
        - Advanced Error APIs (NLM-specific and Python errors)
        - Advanced Development APIs (stage, progression, tools)
        - Advanced Neuromodulation APIs (modulators, state, dynamics)
        - Advanced Memory APIs (working, episodic, semantic, procedural)
        - Advanced Prediction APIs (system, state, performance)
        - Advanced Performance APIs (metrics, monitoring, optimization)
        - Advanced Export APIs (state, reports, data export)
        - Advanced Tool APIs (execution, pipelines, output)
        - Advanced Plugin APIs (information, management, configuration)
        - Advanced Schema APIs (validation, examples, documentation)
    )pbdoc";

    // Register NLM exception hierarchy for Python
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<nlm::NLMException>(m, "NLMException", py::module_local());
    py::register_exception<nlm::SystemError>(m, "SystemError", py::module_local());
    py::register_exception<nlm::InitializationError>(m, "InitializationError", py::module_local());
    py::register_exception<nlm::ConfigurationError>(m, "ConfigurationError", py::module_local());
    py::register_exception<nlm::MemoryError>(m, "MemoryError", py::module_local());
    py::register_exception<nlm::WorkingMemoryError>(m, "WorkingMemoryError", py::module_local());
    py::register_exception<nlm::NeuralError>(m, "NeuralError", py::module_local());
    py::register_exception<nlm::NeuronError>(m, "NeuronError", py::module_local());
    py::register_exception<nlm::SynapseError>(m, "SynapseError", py::module_local());
    py::register_exception<nlm::EpisodicMemoryError>(m, "EpisodicMemoryError", py::module_local());
    py::register_exception<nlm::IntegrationError>(m, "IntegrationError", py::module_local());
    py::register_exception<nlm::BrainLoopError>(m, "BrainLoopError", py::module_local());
    py::register_exception<nlm::PersistenceError>(m, "PersistenceError", py::module_local());
    py::register_exception<nlm::CheckpointError>(m, "CheckpointError", py::module_local());
    py::register_exception<nlm::ValidationError>(m, "ValidationError", py::module_local());

    // Python basic errors
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::value_error>(m, "ValueError");
    py::register_exception<std::type_error>(m, "TypeError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // Unique identifiers
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

    // Enumerations
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

    // Config class with enhanced APIs
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)pbdoc")
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
        // Enhanced APIs
        .def("getSchema", &Config::getSchema,
             "Get configuration schema")
        .def("getDefaults", &Config::getDefaults,
             "Get default values")
        .def("getValidationRules", &Config::getValidationRules,
             "Get validation rules")
        .def("getConstraints", &Config::getConstraints,
             "Get constraints")
        .def("getDocumentation", &Config::getDocumentation,
             "Get configuration documentation")
        .def("validate", &Config::validate,
             "Validate configuration")
        .def("validateAll", &Config::validateAll,
             "Validate all parameters")
        .def("getValidationErrors", &Config::getValidationErrors,
             "Get validation errors")
        .def("getValidationWarnings", &Config::getValidationWarnings,
             "Get validation warnings")
        .def("fixConfiguration", &Config::fixConfiguration,
             "Fix configuration automatically")
        .def("getConfigurationStats", &Config::getConfigurationStats,
             "Get configuration statistics")
        .def("getParameterOptimizationHints", &Config::getParameterOptimizationHints,
             "Get optimization hints")
        .def("getParameterDependencies", &Config::getParameterDependencies,
             "Get parameter dependencies")
        .def("getParameterConflicts", &Config::getParameterConflicts,
             "Get parameter conflicts")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Advanced Config wrapper
    py::class_<ConfigAdvanced>(m, "ConfigAdvanced", R"pbdoc(Advanced configuration wrapper)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &ConfigAdvanced::loadFromFile, py::arg("filepath"))
        .def("saveToFile", &ConfigAdvanced::saveToFile, py::arg("filepath"))
        .def("has", &ConfigAdvanced::has, py::arg("key"))
        .def("getKeys", &ConfigAdvanced::getKeys)
        .def("clear", &ConfigAdvanced::clear)
        .def("summary", &ConfigAdvanced::summary)
        .def("getSchema", &ConfigAdvanced::getSchema)
        .def("getSchemaDocumentation", &ConfigAdvanced::getSchemaDocumentation)
        .def("getSchemaExamples", &ConfigAdvanced::getSchemaExamples)
        .def("getSchemaValidationRules", &ConfigAdvanced::getSchemaValidationRules)
        .def("validateSchema", &ConfigAdvanced::validateSchema, py::arg("data"))
        .def("validate", &ConfigAdvanced::validate)
        .def("validateAll", &ConfigAdvanced::validateAll)
        .def("getValidationErrors", &ConfigAdvanced::getValidationErrors)
        .def("getValidationWarnings", &ConfigAdvanced::getValidationWarnings)
        .def("addValidation", &ConfigAdvanced::addValidation, py::arg("key"), py::arg("validator"))
        .def("removeValidation", &ConfigAdvanced::removeValidation, py::arg("key"))
        .def("fixConfiguration", &ConfigAdvanced::fixConfiguration)
        .def("addRangeValidation", &ConfigAdvanced::addRangeValidation,
             py::arg("key"), py::arg("min"), py::arg("max"), py::arg("description")="")
        .def("addIntegerRangeValidation", &ConfigAdvanced::addIntegerRangeValidation,
             py::arg("key"), py::arg("min"), py::arg("max"), py::arg("description")="")
        .def("addPositiveValidation", &ConfigAdvanced::addPositiveValidation,
             py::arg("key"), py::arg("description")="")
        .def("addNonNegativeValidation", &ConfigAdvanced::addNonNegativeValidation,
             py::arg("key"), py::arg("description")="")
        .def("addNeuronCountValidation", &ConfigAdvanced::addNeuronCountValidation,
             py::arg("max_neurons")=1000000)
        .def("addConnectionProbabilityValidation", &ConfigAdvanced::addConnectionProbabilityValidation,
             py::arg("min")=0.0, py::arg("max")=1.0)
        .def("addMemoryCapacityValidation", &ConfigAdvanced::addMemoryCapacityValidation,
             py::arg("max_working_memory")=10000)
        .def("addTimeStepValidation", &ConfigAdvanced::addTimeStepValidation,
             py::arg("min")=0.0, py::arg("max")=1.0)
        .def("addRegionValidation", &ConfigAdvanced::addRegionValidation,
             py::arg("max_regions")=100)
        .def("getConfigurationStats", &ConfigAdvanced::getConfigurationStats)
        .def("getParameterOptimizationHints", &ConfigAdvanced::getParameterOptimizationHints)
        .def("getParameterDependencies", &ConfigAdvanced::getParameterDependencies)
        .def("getParameterConflicts", &ConfigAdvanced::getParameterConflicts)
        .def("getParameterRelationships", &ConfigAdvanced::getParameterRelationships)
        .def("getConfigurationReport", &ConfigAdvanced::getConfigurationReport)
        .def("getPythonBindingsVersion", &ConfigAdvanced::getPythonBindingsVersion)
        .def("getPythonBindingsInfo", &ConfigAdvanced::getPythonBindingsInfo)
        .def("getMemoryUsage", &ConfigAdvanced::getMemoryUsage)
        .def("exportConfig", &ConfigAdvanced::exportConfig, py::arg("filepath"))
        .def("importConfig", &ConfigAdvanced::importConfig, py::arg("filepath"))
        .def("getConfigAsDict", &ConfigAdvanced::getConfigAsDict)
        .def("fromDict", &ConfigAdvanced::fromDict, py::arg("config_dict"))
        .def("equals", &ConfigAdvanced::equals, py::arg("other_config"))
        .def("copy", &ConfigAdvanced::copy)
        .def("__repr__", [](const ConfigAdvanced& cfg) {
            return "<ConfigAdvanced: " + cfg.summary() + ">";
        });

    // SensoryInput class with enhanced APIs
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class
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

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Advanced SensoryInput wrapper
    py::class_<SensoryInputAdvanced>(m, "SensoryInputAdvanced", R"pbdoc(Advanced sensory input wrapper)pbdoc")
        .def(py::init<>(), py::arg("sensory_input"))
        .def("getType", &SensoryInputAdvanced::getType)
        .def("getData", &SensoryInputAdvanced::getData)
        .def("getDimensions", &SensoryInputAdvanced::getDimensions)
        .def("getTimestamp", &SensoryInputAdvanced::getTimestamp)
        .def("setTimestamp", &SensoryInputAdvanced::setTimestamp, py::arg("timestamp"))
        .def("getSensoryStats", &SensoryInputAdvanced::getSensoryStats)
        .def("getInputQuality", &SensoryInputAdvanced::getInputQuality)
        .def("getSignalNoiseRatio", &SensoryInputAdvanced::getSignalNoiseRatio)
        .def("getSensoryLoad", &SensoryInputAdvanced::getSensoryLoad)
        .def("getVisualFieldStats", &SensoryInputAdvanced::getVisualFieldStats)
        .def("getVisualAcuity", &SensoryInputAdvanced::getVisualAcuity)
        .def("getVisualSensitivity", &SensoryInputAdvanced::getVisualSensitivity)
        .def("getVisualProcessingSpeed", &SensoryInputAdvanced::getVisualProcessingSpeed)
        .def("getAudioStats", &SensoryInputAdvanced::getAudioStats)
        .def("getSoundClarity", &SensoryInputAdvanced::getSoundClarity)
        .def("getAudioFrequencyRange", &SensoryInputAdvanced::getAudioFrequencyRange)
        .def("getAudioProcessingSpeed", &SensoryInputAdvanced::getAudioProcessingSpeed)
        .def("getInternalStats", &SensoryInputAdvanced::getInternalStats)
        .def("getSignalStrength", &SensoryInputAdvanced::getSignalStrength)
        .def("getSignalReliability", &SensoryInputAdvanced::getSignalReliability)
        .def("getInternalProcessingSpeed", &SensoryInputAdvanced::getInternalProcessingSpeed)
        .def("createPercept", &SensoryInputAdvanced::createPercept,
             py::arg("vision")=nullptr, py::arg("touch")=nullptr, py::arg("internal")=nullptr,
             py::arg("proprioception")=nullptr, py::arg("audio")=nullptr)
        .def("getAllSignals", &SensoryInputAdvanced::getAllSignals, py::arg("percept"))
        .def("exportSensoryData", &SensoryInputAdvanced::exportSensoryData, py::arg("filepath"))
        .def("importSensoryData", &SensoryInputAdvanced::importSensoryData, py::arg("filepath"))
        .def("getPythonBindingsVersion", &SensoryInputAdvanced::getPythonBindingsVersion)
        .def("getPythonBindingsInfo", &SensoryInputAdvanced::getPythonBindingsInfo)
        .def("getMemoryUsage", &SensoryInputAdvanced::getMemoryUsage)
        .def("__repr__", [](const SensoryInputAdvanced& sensory) {
            return "<SensoryInputAdvanced: " + sensory.getType() + ">";
        });

    // SimpleWorld class with enhanced APIs
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Advanced 2D world for NLM simulation)pbdoc")
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

    // Advanced SimpleWorld wrapper
    py::class_<SimpleWorldAdvanced>(m, "SimpleWorldAdvanced", R"pbdoc(Advanced world wrapper)pbdoc")
        .def(py::init<>(), py::arg("world"))
        .def("configure", &SimpleWorldAdvanced::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorldAdvanced::reset)
        .def("setAgentStart", &SimpleWorldAdvanced::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorldAdvanced::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorldAdvanced::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorldAdvanced::getSensoryPercept)
        .def("getAgentBody", &SimpleWorldAdvanced::getAgentBody)
        .def("addObject", &SimpleWorldAdvanced::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorldAdvanced::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorldAdvanced::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorldAdvanced::getWidth)
        .def("getHeight", &SimpleWorldAdvanced::getHeight)
        .def("getMaxEnergy", &SimpleWorldAdvanced::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorldAdvanced::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorldAdvanced::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorldAdvanced::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorldAdvanced::getSimulationTime)
        .def("setRandomSeed", &SimpleWorldAdvanced::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorldAdvanced::getRandomSeed)
        .def("getWorldMetrics", &SimpleWorldAdvanced::getWorldMetrics)
        .def("getAgentMetrics", &SimpleWorldAdvanced::getAgentMetrics)
        .def("getEnvironmentMetrics", &SimpleWorldAdvanced::getEnvironmentMetrics)
        .def("getSimulationStats", &SimpleWorldAdvanced::getSimulationStats)
        .def("getPerformanceStats", &SimpleWorldAdvanced::getPerformanceStats)
        .def("getEnvironmentalComplexity", &SimpleWorldAdvanced::getEnvironmentalComplexity)
        .def("getEnvironmentalStability", &SimpleWorldAdvanced::getEnvironmentalStability)
        .def("getResourceAvailability", &SimpleWorldAdvanced::getResourceAvailability)
        .def("getChallengeLevel", &SimpleWorldAdvanced::getChallengeLevel)
        .def("getPerformanceMetrics", &SimpleWorldAdvanced::getPerformanceMetrics)
        .def("getFrameRate", &SimpleWorldAdvanced::getFrameRate)
        .def("getSimulationEfficiency", &SimpleWorldAdvanced::getSimulationEfficiency)
        .def("getMemoryUsage", &SimpleWorldAdvanced::getMemoryUsage)
        .def("validateWorldState", &SimpleWorldAdvanced::validateWorldState)
        .def("getWorldErrors", &SimpleWorldAdvanced::getWorldErrors)
        .def("clearWorldErrors", &SimpleWorldAdvanced::clearWorldErrors)
        .def("diagnoseWorld", &SimpleWorldAdvanced::diagnoseWorld)
        .def("executeTool", &SimpleWorldAdvanced::executeTool, py::arg("tool_name"), py::arg("parameters"))
        .def("runToolPipeline", &SimpleWorldAdvanced::runToolPipeline, py::arg("tools"), py::arg("parameters"))
        .def("exportToolList", &SimpleWorldAdvanced::exportToolList, py::arg("filepath"))
        .def("getPythonBindingsVersion", &SimpleWorldAdvanced::getPythonBindingsVersion)
        .def("getPythonBindingsInfo", &SimpleWorldAdvanced::getPythonBindingsInfo)
        .def("getSystemResources", &SimpleWorldAdvanced::getSystemResources)
        .def("getWorldLoad", &SimpleWorldAdvanced::getWorldLoad)
        .def("__repr__", [](const SimpleWorldAdvanced& world) {
            return "<SimpleWorldAdvanced: " + std::to_string(world.getWidth()) + "x" + std::to_string(world.getHeight()) + ">";
        });

    // WorldObject class
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

    // AgentBody class
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

    // Action class
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

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class
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

    // Brain class with enhanced APIs
    py::class_<Brain>(m, "Brain", R"pbdoc(Advanced neural simulation brain class)pbdoc")
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

    // Advanced Brain wrapper
    py::class_<BrainAdvanced>(m, "BrainAdvanced", R"pbdoc(Advanced brain wrapper)pbdoc")
        .def(py::init<>(), py::arg("brain"))
        .def("initialize", &BrainAdvanced::initialize)
        .def("step", &BrainAdvanced::step, py::arg("timestep"))
        .def("stepWithTime", &BrainAdvanced::stepWithTime, py::arg("timestep"), py::arg("current_time"))
        .def("reset", &BrainAdvanced::reset)
        .def("getBrainId", &BrainAdvanced::getBrainId)
        .def("getRegionCount", &BrainAdvanced::getRegionCount)
        .def("getTotalNeuronCount", &BrainAdvanced::getTotalNeuronCount)
        .def("getTotalSynapseCount", &BrainAdvanced::getTotalSynapseCount)
        .def("getActiveNeuronCount", &BrainAdvanced::getActiveNeuronCount)
        .def("getFiringNeuronCount", &BrainAdvanced::getFiringNeuronCount)
        .def("getAverageFiringRate", &BrainAdvanced::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &BrainAdvanced::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &BrainAdvanced::getTotalSpikeCount)
        .def("getDevelopmentStage", &BrainAdvanced::getDevelopmentStage)
        .def("getDevelopmentStageName", &BrainAdvanced::getDevelopmentStageName)
        .def("setDevelopmentStage", &BrainAdvanced::setDevelopmentStage, py::arg("stage"))
        .def("getLearningPotential", &BrainAdvanced::getLearningPotential)
        .def("isOptimalStageForLearning", &BrainAdvanced::isOptimalStageForLearning)
        .def("getCriticalPeriodStatus", &BrainAdvanced::getCriticalPeriodStatus)
        .def("getDevelopmentalAge", &BrainAdvanced::getDevelopmentalAge)
        .def("getNeuromodulationLevel", &BrainAdvanced::getNeuromodulationLevel)
        .def("getDopamineLevel", &BrainAdvanced::getDopamineLevel)
        .def("getAcetylcholineLevel", &BrainAdvanced::getAcetylcholineLevel)
        .def("getNorepinephrineLevel", &BrainAdvanced::getNorepinephrineLevel)
        .def("getSerotoninLevel", &BrainAdvanced::getSerotoninLevel)
        .def("getCuriosityLevel", &BrainAdvanced::getCuriosityLevel)
        .def("getNoveltyLevel", &BrainAdvanced::getNoveltyLevel)
        .def("getPredictionError", &BrainAdvanced::getPredictionError)
        .def("getRewardLevel", &BrainAdvanced::getRewardLevel)
        .def("getNeuromodulatorSummary", &BrainAdvanced::getNeuromodulatorSummary)
        .def("getNeuromodulatorNames", &BrainAdvanced::getNeuromodulatorNames)
        .def("getNeuromodulatorEffects", &BrainAdvanced::getNeuromodulatorEffects, py::arg("name"))
        .def("getWorkingMemoryStats", &BrainAdvanced::getWorkingMemoryStats)
        .def("getEpisodicMemoryStats", &BrainAdvanced::getEpisodicMemoryStats)
        .def("getSemanticMemoryStats", &BrainAdvanced::getSemanticMemoryStats)
        .def("getProceduralMemoryStats", &BrainAdvanced::getProceduralMemoryStats)
        .def("getMemoryConsolidationStats", &BrainAdvanced::getMemoryConsolidationStats)
        .def("getMemoryStats", &BrainAdvanced::getMemoryStats)
        .def("getMemoryPerformanceMetrics", &BrainAdvanced::getMemoryPerformanceMetrics)
        .def("getMemoryEfficiency", &BrainAdvanced::getMemoryEfficiency)
        .def("getMemoryCapacity", &BrainAdvanced::getMemoryCapacity)
        .def("getMemoryUtilization", &BrainAdvanced::getMemoryUtilization)
        .def("getPredictionAccuracy", &BrainAdvanced::getPredictionAccuracy)
        .def("getPredictionError", &BrainAdvanced::getPredictionError)
        .def("getPredictionConfidence", &BrainAdvanced::getPredictionConfidence)
        .def("getPredictionHistory", &BrainAdvanced::getPredictionHistory)
        .def("getPredictionPerformance", &BrainAdvanced::getPredictionPerformance)
        .def("getPredictionSystemNames", &BrainAdvanced::getPredictionSystemNames)
        .def("getPredictionTypes", &BrainAdvanced::getPredictionTypes, py::arg("name"))
        .def("getPredictionAlgorithms", &BrainAdvanced::getPredictionAlgorithms, py::arg("name"))
        .def("getPerformanceMetrics", &BrainAdvanced::getPerformanceMetrics)
        .def("getBrainLoopStats", &BrainAdvanced::getBrainLoopStats)
        .def("getIntegrationMetrics", &BrainAdvanced::getIntegrationMetrics)
        .def("getMemoryPerformance", &BrainAdvanced::getMemoryPerformance)
        .def("getPredictionPerformance", &BrainAdvanced::getPredictionPerformance)
        .def("getDevelopmentPerformance", &BrainAdvanced::getDevelopmentPerformance)
        .def("getDevelopmentalEffects", &BrainAdvanced::getDevelopmentalEffects)
        .def("getAgeEffects", &BrainAdvanced::getAgeEffects)
        .def("getStageEffects", &BrainAdvanced::getStageEffects)
        .def("getSystemSync", &BrainAdvanced::getSystemSync)
        .def("getCrossSystemCoordination", &BrainAdvanced::getCrossSystemCoordination)
        .def("validateBrainState", &BrainAdvanced::validateBrainState)
        .def("getBrainErrors", &BrainAdvanced::getBrainErrors)
        .def("clearBrainErrors", &BrainAdvanced::clearBrainErrors)
        .def("exportState", &BrainAdvanced::exportState, py::arg("filepath"))
        .def("generateReport", &BrainAdvanced::generateReport, py::arg("report_type")="comprehensive")
        .def("exportMemoryState", &BrainAdvanced::exportMemoryState, py::arg("filepath"))
        .def("exportPredictionState", &BrainAdvanced::exportPredictionState, py::arg("filepath"))
        .def("exportDevelopmentState", &BrainAdvanced::exportDevelopmentState, py::arg("filepath"))
        .def("executeTool", &BrainAdvanced::executeTool, py::arg("tool_name"), py::arg("parameters"))
        .def("runToolPipeline", &BrainAdvanced::runToolPipeline, py::arg("tools"), py::arg("parameters"))
        .def("getToolOutput", &BrainAdvanced::getToolOutput, py::arg("tool_name"))
        .def("getToolLogs", &BrainAdvanced::getToolLogs, py::arg("tool_name"))
        .def("loadPlugin", &BrainAdvanced::loadPlugin, py::arg("plugin_name"), py::arg("config")=py::none())
        .def("unloadPlugin", &BrainAdvanced::unloadPlugin, py::arg("plugin_name"))
        .def("getPluginState", &BrainAdvanced::getPluginState, py::arg("plugin_name"))
        .def("setPluginConfig", &BrainAdvanced::setPluginConfig, py::arg("plugin_name"), py::arg("config"))
        .def("getAvailablePlugins", &BrainAdvanced::getAvailablePlugins)
        .def("getSchema", &BrainAdvanced::getSchema)
        .def("getSchemaDocumentation", &BrainAdvanced::getSchemaDocumentation)
        .def("getSchemaExamples", &BrainAdvanced::getSchemaExamples)
        .def("getSchemaValidationRules", &BrainAdvanced::getSchemaValidationRules)
        .def("validateSchema", &BrainAdvanced::validateSchema, py::arg("data"))
        .def("getConfig", &BrainAdvanced::getConfig)
        .def("getConfigSchema", &BrainAdvanced::getConfigSchema)
        .def("validateConfig", &BrainAdvanced::validateConfig)
        .def("getOptimizationRecommendations", &BrainAdvanced::getOptimizationRecommendations)
        .def("getPerformanceBottlenecks", &BrainAdvanced::getPerformanceBottlenecks)
        .def("getOptimizationSuggestions", &BrainAdvanced::getOptimizationSuggestions)
        .def("applyOptimizations", &BrainAdvanced::applyOptimizations)
        .def("getPythonBindingsVersion", &BrainAdvanced::getPythonBindingsVersion)
        .def("getPythonBindingsInfo", &BrainAdvanced::getPythonBindingsInfo)
        .def("getMemoryUsage", &BrainAdvanced::getMemoryUsage)
        .def("getPerformanceMonitoring", &BrainAdvanced::getPerformanceMonitoring)
        .def("getSystemHealth", &BrainAdvanced::getSystemHealth)
        .def("getSystemStatus", &BrainAdvanced::getSystemStatus)
        .def("getSystemDiagnostics", &BrainAdvanced::getSystemDiagnostics)
        .def("_getBrain", &BrainAdvanced::_getBrain)
        .def("__repr__", [](const BrainAdvanced& brain) {
            return "<BrainAdvanced: " + std::to_string(brain.getBrainId()) + ">";
        });

    // AgentBrain class with enhanced APIs
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Advanced agent brain interface)pbdoc")
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
             "Get neuromodulation level")
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

    // Advanced AgentBrain wrapper
    py::class_<AgentBrainAdvanced>(m, "AgentBrainAdvanced", R"pbdoc(Advanced agent brain wrapper)pbdoc")
        .def(py::init<>(), py::arg("agent_brain"))
        .def("initialize", &AgentBrainAdvanced::initialize, py::arg("world"))
        .def("getSensoryInputSize", &AgentBrainAdvanced::getSensoryInputSize)
        .def("getMotorOutputSize", &AgentBrainAdvanced::getMotorOutputSize)
        .def("processSensoryInput", &AgentBrainAdvanced::processSensoryInput, py::arg("percept"))
        .def("decodeMotorCommand", &AgentBrainAdvanced::decodeMotorCommand)
        .def("applyRewardModulation", &AgentBrainAdvanced::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"))
        .def("updateDevelopment", &AgentBrainAdvanced::updateDevelopment, py::arg("timestep"))
        .def("getDevelopmentalStage", &AgentBrainAdvanced::getDevelopmentalStage)
        .def("getDevelopmentalStageName", &AgentBrainAdvanced::getDevelopmentalStageName)
        .def("getNeuromodulationLevel", &AgentBrainAdvanced::getNeuromodulationLevel)
        .def("getCuriosityLevel", &AgentBrainAdvanced::getCuriosityLevel)
        .def("getNoveltyLevel", &AgentBrainAdvanced::getNoveltyLevel)
        .def("getPredictionError", &AgentBrainAdvanced::getPredictionError)
        .def("getAcetylcholineLevel", &AgentBrainAdvanced::getAcetylcholineLevel)
        .def("getNorepinephrineLevel", &AgentBrainAdvanced::getNorepinephrineLevel)
        .def("getSerotoninLevel", &AgentBrainAdvanced::getSerotoninLevel)
        .def("getRewardLevel", &AgentBrainAdvanced::getRewardLevel)
        .def("enableRewardModulation", &AgentBrainAdvanced::enableRewardModulation, py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrainAdvanced::enableStructuralPlasticity, py::arg("enable"))
        .def("enableDevelopment", &AgentBrainAdvanced::enableDevelopment, py::arg("enable"))
        .def("enableCuriosity", &AgentBrainAdvanced::enableCuriosity, py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrainAdvanced::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrainAdvanced::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrainAdvanced::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrainAdvanced::isCuriosityEnabled)
        .def("getBrain", &AgentBrainAdvanced::getBrain)
        .def("getBrainId", &AgentBrainAdvanced::getBrainId)
        .def("getAttentionScore", &AgentBrainAdvanced::getAttentionScore)
        .def("getPlanningScore", &AgentBrainAdvanced::getPlanningScore)
        .def("getConceptFormationScore", &AgentBrainAdvanced::getConceptFormationScore)
        .def("getSelfModelScore", &AgentBrainAdvanced::getSelfModelScore)
        .def("getSocialLearningScore", &AgentBrainAdvanced::getSocialLearningScore)
        .def("getCognitiveMetrics", &AgentBrainAdvanced::getCognitiveMetrics)
        .def("getIntegrationQuality", &AgentBrainAdvanced::getIntegrationQuality)
        .def("getSystemSync", &AgentBrainAdvanced::getSystemSync)
        .def("getCrossSystemCoordination", &AgentBrainAdvanced::getCrossSystemCoordination)
        .def("getIntegrationMetrics", &AgentBrainAdvanced::getIntegrationMetrics)
        .def("validateAgentState", &AgentBrainAdvanced::validateAgentState)
        .def("getAgentErrors", &AgentBrainAdvanced::getAgentErrors)
        .def("clearAgentErrors", &AgentBrainAdvanced::clearAgentErrors)
        .def("diagnoseAgent", &AgentBrainAdvanced::diagnoseAgent)
        .def("executeTool", &AgentBrainAdvanced::executeTool, py::arg("tool_name"), py::arg("parameters"))
        .def("runToolPipeline", &AgentBrainAdvanced::runToolPipeline, py::arg("tools"), py::arg("parameters"))
        .def("loadPlugin", &AgentBrainAdvanced::loadPlugin, py::arg("plugin_name"), py::arg("config")=py::none())
        .def("unloadPlugin", &AgentBrainAdvanced::unloadPlugin, py::arg("plugin_name"))
        .def("getPluginState", &AgentBrainAdvanced::getPluginState, py::arg("plugin_name"))
        .def("setPluginConfig", &AgentBrainAdvanced::setPluginConfig, py::arg("plugin_name"), py::arg("config"))
        .def("getAvailablePlugins", &AgentBrainAdvanced::getAvailablePlugins)
        .def("getPythonBindingsVersion", &AgentBrainAdvanced::getPythonBindingsVersion)
        .def("getPythonBindingsInfo", &AgentBrainAdvanced::getPythonBindingsInfo)
        .def("getMemoryUsage", &AgentBrainAdvanced::getMemoryUsage)
        .def("__repr__", [](const AgentBrainAdvanced& agent) {
            return "<AgentBrainAdvanced: " + std::to_string(agent.getBrainId()) + ">";
        });

    // Factory functions
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

    // Python integration helper functions
    m.def("createBrainAdvanced", []() -> std::shared_ptr<BrainAdvanced> {
        auto brain = std::make_shared<BrainAdvanced>();
        return brain;
    }, "Create an advanced brain instance");

    m.def("createAgentBrainAdvanced", []() -> std::shared_ptr<AgentBrainAdvanced> {
        auto agent_brain = std::make_shared<AgentBrainAdvanced>();
        return agent_brain;
    }, "Create an advanced agent brain instance");

    m.def("createConfigAdvanced", []() -> std::shared_ptr<ConfigAdvanced> {
        auto config = std::make_shared<ConfigAdvanced>();
        return config;
    }, "Create an advanced configuration instance");

    m.def("createWorldAdvanced", []() -> std::shared_ptr<SimpleWorldAdvanced> {
        auto world = std::make_shared<SimpleWorldAdvanced>();
        return world;
    }, "Create an advanced world instance");

    m.def("createSensoryInputAdvanced", []() -> std::shared_ptr<SensoryInputAdvanced> {
        auto sensory = std::make_shared<SensoryInputAdvanced>();
        return sensory;
    }, "Create an advanced sensory input instance");

    // Version and metadata
    m.attr("VERSION") = "0.1.0-advanced";
    m.attr("NLM_VERSION") = "0.1.0";
    m.attr("PLATFORM") = "linux";

    // Error types for user convenience
    py::class_<NLMError>(m, "NLMError")
        .def(py::init<const std::string&>(), py::arg("message"))
        .def("getErrorType", &NLMError::getErrorType)
        .def("getErrorMessage", &NLMError::getErrorMessage)
        .def("getErrorCode", &NLMError::getErrorCode)
        .def("getErrorContext", &NLMError::getErrorContext)
        .def("getErrorDetails", &NLMError::getErrorDetails);

    py::class_<ConfigurationError, NLMError>(m, "ConfigurationError")
        .def(py::init<const std::string&>(), py::arg("message"));

    py::class_<NeuralError, NLMError>(m, "NeuralError")
        .def(py::init<const std::string&>(), py::arg("message"));

    py::class_<MemoryError, NLMError>(m, "MemoryError")
        .def(py::init<const std::string&>(), py::arg("message"));

    py::class_<IntegrationError, NLMError>(m, "IntegrationError")
        .def(py::init<const std::string&>(), py::arg("message"));

    py::class_<DevelopmentError, NLMError>(m, "DevelopmentError")
        .def(py::init<const std::string&>(), py::arg("message"));

    py::class_<PersistenceError, NLMError>(m, "PersistenceError")
        .def(py::init<const std::string&>(), py::arg("message"));

    // Convenience constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Summary
    m.def("getPythonBindingsVersion", []() {
        return "0.1.0-advanced";
    }, "Get Python bindings version");

    m.def("getPythonBindingsInfo", []() {
        return {
            "version": "0.1.0-advanced",
            "platform": "linux",
            "features": [
                "advanced_brain_apis",
                "advanced_agentbrain_apis",
                "advanced_config_apis",
                "advanced_simpleworld_apis",
                "advanced_sensoryinput_apis",
                "advanced_error_apis",
                "advanced_development_apis",
                "advanced_neuromodulation_apis",
                "advanced_memory_apis",
                "advanced_prediction_apis",
                "advanced_performance_apis",
                "advanced_export_apis",
                "advanced_tool_apis",
                "advanced_plugin_apis",
                "advanced_schema_apis",
            ],
        };
    }, "Get Python bindings information");
}

} // namespace nlm