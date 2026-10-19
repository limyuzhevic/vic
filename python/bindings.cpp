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
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/development/DevelopmentSystem.hpp"

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        Also exposes all memory systems: NeuralWorkingMemory, NeuralEpisodicMemory, and NeuralAssociativeMemory.
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
        // ========== ADVANCED CONFIGURATION MANAGEMENT ==========
        
        // Schema validation
        .def("hasValidSchema", &Config::hasValidSchema,
             "Check if the configuration has a valid schema")
        .def("getSchema", &Config::getSchema,
             "Get the configuration schema")
        .def("validateConfiguration", &Config::validateConfiguration,
             "Validate the configuration against its schema")
        .def("setValidationEnabled", &Config::setValidationEnabled,
             py::arg("enabled"), "Enable or disable schema validation")
        .def("isValidationEnabled", &Config::isValidationEnabled,
             "Check if schema validation is enabled")
        .def("getValidationErrors", &Config::getValidationErrors,
             "Get validation errors")
        .def("clearValidationErrors", &Config::clearValidationErrors,
             "Clear validation errors")
        
        // Configuration templates
        .def("getTemplateCount", &Config::getTemplateCount,
             "Get the number of saved templates")
        .def("getTemplateNames", &Config::getTemplateNames,
             "Get names of all saved templates")
        .def("getTemplate", [](const Config& self, const std::string& templateName) {
            std::shared_ptr<Config> templateConfig = self.getTemplate(templateName);
            return templateConfig;
        }, py::arg("templateName"), "Get a configuration template by name")
        .def("addTemplate", &Config::addTemplate,
             py::arg("templateName"), py::arg("templateConfig"), "Add a configuration template")
        .def("removeTemplate", &Config::removeTemplate,
             py::arg("templateName"), "Remove a configuration template")
        .def("loadTemplate", &Config::loadTemplate,
             py::arg("templateName"), py::arg("filepath"), "Load a configuration template from file")
        
        // Configuration monitoring
        .def("hasMonitor", &Config::hasMonitor,
             "Check if configuration monitoring is enabled")
        .def("getMonitor", [](const Config& self) {
            std::shared_ptr<ConfigMonitor> monitor = self.getMonitor();
            return monitor;
        }, "Get the configuration monitor")
        .def("setMonitor", &Config::setMonitor,
             py::arg("monitor"), "Set the configuration monitor")
        .def("onConfigurationChanged", &Config::onConfigurationChanged,
             py::arg("key"), py::arg("oldValue"), py::arg("newValue"), py::arg("source"),
             "Handle configuration change event")
        .def("getConfigurationHistory", &Config::getConfigurationHistory,
             "Get configuration change history")
        .def("getRecentChanges", &Config::getRecentChanges,
             py::arg("maxChanges") = 10, "Get recent configuration changes")
        .def("rollback", &Config::rollback,
             py::arg("changeIndex"), "Rollback to a specific configuration change")
        .def("getDifferences", &Config::getDifferences,
             py::arg("otherConfig"), "Get differences between this and another configuration")
        
        // Configuration comparison
        .def("compareConfigurations", &Config::compareConfigurations,
             py::arg("config1"), py::arg("config2"), "Compare two configurations")
        .def("getConfigurationDifferences", &Config::getConfigurationDifferences,
             py::arg("config1"), py::arg("config2"), "Get differences between two configurations")
        .def("mergeConfigurations", [](Config& self, const Config& config1, const Config& config2) {
            Config result;
            Config::mergeConfigurations(config1, config2, result);
            return result;
        }, py::arg("config1"), py::arg("config2"), "Merge two configurations into a new one")
        
        // Configuration encryption/security
        .def("encryptConfiguration", &Config::encryptConfiguration,
             py::arg("key"), "Encrypt the configuration")
        .def("decryptConfiguration", &Config::decryptConfiguration,
             py::arg("encryptedData"), py::arg("key"), "Decrypt configuration data")
        .def("validateSignature", &Config::validateSignature,
             py::arg("signature"), py::arg("data"), "Validate configuration signature")
        .def("setEncryptionKey", &Config::setEncryptionKey,
             py::arg("key"), "Set encryption key for configuration")
        .def("getEncryptionStatus", &Config::getEncryptionStatus,
             "Get encryption status")
        
        // Configuration export/import
        .def("exportConfiguration", &Config::exportConfiguration,
             py::arg("filepath"), py::arg("format") = "json", "Export configuration to file")
        .def("importConfiguration", &Config::importConfiguration,
             py::arg("filepath"), py::arg("format") = "json", "Import configuration from file")
        .def("getExportFormats", &Config::getExportFormats,
             "Get available export formats")
        .def("setExportPath", &Config::setExportPath,
             py::arg("path"), "Set export directory path")
        .def("getExportPath", &Config::getExportPath,
             "Get export directory path")
        
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
             "Log brain status")
        
        // ========== PLASTICITY SYSTEMS ==========
        
        // Development system for neural development and critical periods
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        
        // Spike-timing dependent plasticity (STDP)
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP system")
        
        // Hebbian learning rules
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian system")
        
        // Structural plasticity for synapse and neuron creation/removal
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
        
        // ========== MEMORY SYSTEMS ==========
        
        // Working memory - transient active information
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        
        // Episodic memory - experience storage
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        
        // Associative memory - pattern associations
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system");

    // WorkingMemory class - wraps NeuralWorkingMemory for Python API
    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory system for temporary active storage of information)pbdoc")
        .def(py::init<>())
        .def("initialize", &WorkingMemory::initialize, "Initialize working memory")
        .def("setCapacity", &WorkingMemory::setCapacity, py::arg("capacity"),
             "Set working memory capacity")
        .def("store", &WorkingMemory::store, py::arg("neuron"), py::arg("value"),
             "Store information in working memory")
        .def("recall", &WorkingMemory::retrieve, py::arg("neuron"),
             "Retrieve information from working memory")
        .def("update", &WorkingMemory::decay, py::arg("decayRate"),
             "Update working memory with decay")
        .def("getActiveTraces", &WorkingMemory::getCurrentSize,
             "Get number of active memory traces")
        .def("getMemoryNeurons", &WorkingMemory::getCurrentSize,
             "Get memory neurons count")
        .def("clear", &WorkingMemory::clear, "Clear working memory")
        .def_readwrite("capacity", &WorkingMemory::getCapacity,
                      "Working memory capacity")
        .def_readwrite("activeTraces", &WorkingMemory::getCurrentSize,
                      "Number of active memory traces")
        .def_readwrite("memoryNeurons", &WorkingMemory::getCurrentSize,
                      "Memory neurons count");

    // EpisodicMemory class - wraps NeuralEpisodicMemory for Python API
    py::class_<EpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory system for storing experiences and events)pbdoc")
        .def(py::init<>())
        .def("initialize", &EpisodicMemory::initialize, "Initialize episodic memory")
        .def("setMaxEpisodes", &EpisodicMemory::setMaxEpisodes, py::arg("maxEpisodes"),
             "Set maximum number of episodes")
        .def("storeEpisode", &EpisodicMemory::storeEpisode, py::arg("episode"),
             "Store an episode in episodic memory")
        .def("getEpisodes", &EpisodicMemory::getEpisodeCount,
             "Get number of stored episodes")
        .def("replayEpisode", &EpisodicMemory::replayEpisode, py::arg("episode"),
             "Replay a stored episode")
        .def("getEpisodesForReplay", &EpisodicMemory::getEpisodesForReplay,
             py::arg("count"), "Get episodes suitable for replay")
        .def("consolidate", &EpisodicMemory::consolidate, py::arg("relevanceThreshold"),
             "Consolidate memory by removing irrelevant episodes")
        .def("clear", &EpisodicMemory::clear, "Clear episodic memory")
        .def_readwrite("maxEpisodes", &EpisodicMemory::getMaxEpisodes,
                      "Maximum number of episodes")
        .def_readwrite("episodeCount", &EpisodicMemory::getEpisodeCount,
                      "Current number of episodes");

    // AssociativeMemory class - wraps NeuralAssociativeMemory for Python API
    py::class_<AssociativeMemory>(m, "AssociativeMemory", R"pbdoc(Associative memory system for creating relationships between neural representations)pbdoc")
        .def(py::init<>())
        .def("initialize", &AssociativeMemory::initialize, "Initialize associative memory")
        .def("addAssociation", &AssociativeMemory::associate, py::arg("a"), py::arg("b"), py::arg("strength"),
             "Create association between two neurons")
        .def("recall", &AssociativeMemory::retrieve, py::arg("pattern"),
             "Recall patterns associated with a query pattern")
        .def("getAssociations", &AssociativeMemory::getAssociations, py::arg("neuron"),
             "Get neurons associated with a specific neuron")
        .def("clear", &AssociativeMemory::clear, "Clear associative memory")
        .def_readwrite("associationCount", &AssociativeMemory::getAssociationCount,
                      "Number of associations");

    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Neuromodulator base class
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base class for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName, "Get neuromodulator name")
        .def("getLevel", &Neuromodulator::getLevel, "Get current neuromodulator level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set neuromodulator level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor, "Get plasticity factor for this neuromodulator")
        .def("update", &Neuromodulator::update, py::arg("dt"), "Update neuromodulator state")
        .def_readonly("level", &Neuromodulator::getLevel)
        .def_readonly("enabled", [](const Neuromodulator& self) { return self.getLevel() > 0.0f; })
        .def_readonly("type", &Neuromodulator::getName);

    // Dopamine neuromodulator
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator for reward and reinforcement)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Dopamine::getLevel, "Get current dopamine level")
        .def("setLevel", &Dopamine::setLevel, py::arg("level"), "Set dopamine level")
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor, "Get plasticity factor modulated by dopamine")
        .def("update", &Dopamine::update, py::arg("dt"), "Update dopamine level with decay")
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"), "Signal reward to dopamine system")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"), "Signal reward prediction error to dopamine system")
        .def_readonly("plasticityFactor", &Dopamine::getPlasticityFactor)
        .def_readonly("rewardPredictionError", [](const Dopamine& self) { return self.getLevel(); });

    // Novelty detection system
    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection system)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Novelty::getLevel, "Get current novelty level")
        .def("setLevel", &Novelty::setLevel, py::arg("level"), "Set novelty level")
        .def("update", &Novelty::update, py::arg("dt"), "Update novelty detection")
        .def("getHistory", &Novelty::getHistory, "Get novelty detection history")
        .def("clearHistory", &Novelty::clearHistory, "Clear novelty history")
        .def("detectNovelty", static_cast<void (Novelty::*)(const std::vector<float>&, const std::vector<float>&)>(&Novelty::detectNovelty), 
             py::arg("currentPattern"), py::arg("previousPattern"), "Detect novelty from sensory patterns")
        .def_readonly("detectionStrength", [](const Novelty& self) { return self.getLevel(); })
        .def_readonly("recentActivity", &Novelty::getHistory);

    // Curiosity system
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity drive system for exploration)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Curiosity::getLevel, "Get current curiosity level")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"), "Update curiosity based on novelty and prediction error")
        .def("getExplorationBias", &Curiosity::getExplorationDrive, "Get exploration bias (same as curiosity level)")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"), "Set weight of novelty in curiosity calculation")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"), "Set weight of prediction error in curiosity calculation")
        .def("reset", &Curiosity::reset, "Reset curiosity to zero")
        .def_readonly("learningRate", [](const Curiosity& self) { return 0.1f; });

    // Prediction error system
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal for learning)pbdoc")
        .def(py::init<>())
        .def("getLevel", &PredictionError::getError, "Get current prediction error level")
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"), "Compute prediction error from predicted and actual values")
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"), "Update prediction value")
        .def("getHistory", &PredictionError::getHistory, "Get prediction error history")
        .def("clearHistory", &PredictionError::clearHistory, "Clear prediction error history")
        .def("getMagnitude", &PredictionError::getMagnitude, "Get absolute magnitude of prediction error")
        .def_readonly("predictionError", &PredictionError::getError)
        .def_readonly("confidence", [](const PredictionError& self) { return 1.0f / (1.0f + std::abs(self.getError())); });

        // Neuromodulator factory functions
    m.def("createDopamine", []() { return std::make_shared<Dopamine>(); }, "Create a dopamine neuromodulator");
    m.def("createNovelty", []() { return std::make_shared<Novelty>(); }, "Create a novelty detection system");
    m.def("createCuriosity", []() { return std::make_shared<Curiosity>(); }, "Create a curiosity system");
    m.def("createPredictionError", []() { return std::make_shared<PredictionError>(); }, "Create a prediction error system");

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
    // ========== COGNITION SYSTEMS ==========
    
    // PredictionSystem - predicts future sensory states and consequences
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system for sensory prediction and error computation)pbdoc")
        .def(py::init<>())
        .def("predict", &PredictionSystem::predictNextState, py::arg("currentState"),
             "Make prediction for next timestep based on current sensory state")
        .def("getPrediction", [](const PredictionSystem& self) { return self.predictNextState({}); },
             "Get prediction (placeholder implementation)")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             "Get prediction error")
        .def("getPredictionConfidence", &PredictionSystem::getConfidence,
             "Get prediction confidence")
        .def("getPredictionAccuracy", &PredictionSystem::getConfidence,
             "Get prediction accuracy (alias for confidence)")
        .def("update", &PredictionSystem::updatePredictions, py::arg("predicted"), py::arg("actual"),
             "Update predictions based on actual observation")
        .def("update", [](PredictionSystem& self) { 
            // Simple update using default constructed SensoryInput objects
            SensoryInput predicted;
            SensoryInput actual;
            self.updatePredictions(predicted, actual);
        }, "Update prediction system state")
        .def("enable", [](PredictionSystem& self) {}, "Enable prediction system (placeholder)")
        .def("disable", [](PredictionSystem& self) {}, "Disable prediction system (placeholder)")
        .def("getErrorHistory", &PredictionSystem::getErrorHistory,
             "Get prediction error history")
        .def("clearHistory", &PredictionSystem::clearHistory,
             "Clear prediction error history")
        .def("train", &PredictionSystem::train, py::arg("observation"),
             "Train prediction model with new observation")
        .def_readonly("enabled", [](const PredictionSystem& self) { return true; },
             "Whether the prediction system is enabled (placeholder)")
        .def_readonly("predictionAccuracy", &PredictionSystem::getConfidence,
             "Current prediction accuracy")
        .def_readonly("predictionConfidence", &PredictionSystem::getConfidence,
             "Current prediction confidence");

    // NeuralPlanner - plans multi-step actions based on learned predictions
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner for action planning using predictive neural dynamics)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"),
             "Initialize planner with brain reference")
        .def("plan", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward"),
             "Plan next action given current state and target reward")
        .def("evaluateAction", &NeuralPlanner::evaluateAction, py::arg("action"), py::arg("state"),
             "Evaluate a single action from a given state")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence, py::arg("actions"), py::arg("startState"),
             "Evaluate a sequence of actions")
        .def("getBestAction", &NeuralPlanner::planAction, py::arg("currentState"),
             "Get best action from current state (simplified)")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"),
             "Set planning horizon (number of steps to look ahead)")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth,
             "Get current planning depth")
        .def("getPlanningHorizon", &NeuralPlanner::getPlanningDepth,
             "Alias for getPlanningDepth")
        .def("getActionQuality", &NeuralPlanner::getPlanningConfidence,
             "Get current planning confidence (action quality)")
        .def("update", &NeuralPlanner::updatePlanQuality, py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
             "Update plans based on actual outcomes")
        .def("clearCache", &NeuralPlanner::clearCache,
             "Clear planning cache")
        .def("setActionQuality", &NeuralPlanner::setActionQuality, py::arg("action"), py::arg("quality"),
             "Set quality of an action from experience")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal,
             "Get current planning goal")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"),
             "Set current planning goal")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful,
             "Check if recent planning was successful")
        .def_readonly("planningDepth", &NeuralPlanner::getPlanningDepth,
             "Planning depth (look-ahead horizon)")
        .def_readonly("planningHorizon", &NeuralPlanner::getPlanningDepth,
             "Planning horizon (alias for planningDepth)")
        .def_readonly("actionQuality", &NeuralPlanner::getPlanningConfidence,
             "Current action quality/confidence");

    // ConceptFormation - discovers recurring patterns from experience
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation system that discovers recurring patterns from experience)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"),
             "Initialize concept formation with brain reference")
        .def("learnPattern", &ConceptFormation::presentExperience, py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"),
             "Learn a pattern and form/update concepts")
        .def("getConcept", &ConceptFormation::getMatchingConcept, py::arg("pattern"), py::arg("similarityThreshold"),
             "Get the concept ID that best matches a pattern")
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("conceptId"),
             "Get stability of a specific concept (0-1)")
        .def("getConceptCount", &ConceptFormation::getConceptCount,
             "Get number of discovered concepts")
        .def("update", &ConceptFormation::updateConcept, py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"),
             "Update a concept with new instance")
        .def("computeSimilarity", &ConceptFormation::computeSimilarity, py::arg("patternA"), py::arg("patternB"),
             "Compute similarity between two patterns")
        .def("isNovel", &ConceptFormation::isNovel, py::arg("pattern"), py::arg("similarityThreshold"),
             "Check if a pattern is novel (doesn't belong to any existing concept)")
        .def("getAllConcepts", &ConceptFormation::getConcepts,
             "Get all discovered concepts")
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype, py::arg("conceptId"),
             "Get the prototype pattern for a concept")
        .def("getConceptInstances", &ConceptFormation::getConceptInstances, py::arg("conceptId"),
             "Get all instances of a concept")
        .def("mergeConcepts", &ConceptFormation::mergeConcepts, py::arg("conceptA"), py::arg("conceptB"),
             "Merge two concepts when discovered they're actually the same")
        .def("clear", &ConceptFormation::clear,
             "Clear all concepts")
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility, py::arg("conceptId"),
             "Get generalization ability of a concept (how well it handles variations)")
        .def("setFormationThreshold", &ConceptFormation::setFormationThreshold, py::arg("threshold"),
             "Set similarity threshold for concept formation")
        .def("setStabilityWindow", &ConceptFormation::setStabilityWindow, py::arg("window"),
             "Set stability window (number of observations to consider)")
        .def_readonly("stability", &ConceptFormation::getGeneralizationAbility,
             "Concept stability (generalization ability)")
        .def_readonly("learningRate", [](const ConceptFormation& self) { return 0.1f; },
             "Learning rate (placeholder - could be configurable)")
        .def_readonly("conceptCount", &ConceptFormation::getConceptCount,
             "Number of discovered concepts");

    // AttentionalSelection - selects and focuses on relevant neural populations
    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Attentional selection system for focusing on relevant neural populations)pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"),
             "Initialize attentional selection with brain reference")
        .def("focus", &AttentionalSelection::focusOnRegion, py::arg("region"),
             "Focus attention on a specific brain region")
        .def("getFocusedNeurons", &AttentionalSelection::processCompetition, py::arg("competitors"), py::arg("globalInhibition"),
             "Process competing neurons and return winners (focused neurons)")
        .def("getFocusedNeurons", static_cast<std::vector<NeuronId> (AttentionalSelection::*)() const>(&AttentionalSelection::getWinners),
             "Get currently focused neurons (winners from last competition)")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"),
             "Set inhibition strength for attention competition")
        .def("getInhibitionStrength", &AttentionalSelection::getInhibitionStrength,
             "Get current inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"),
             "Set excitation strength for attended neurons")
        .def("getExcitationStrength", &AttentionalSelection::getExcitationStrength,
             "Get current excitation strength")
        .def("update", &AttentionalSelection::update, py::arg("dt"),
             "Update attentional selection dynamics")
        .def("isFocused", &AttentionalSelection::isAttended, py::arg("neuron"),
             "Check if a neuron is currently attended/focused")
        .def("releaseAttention", &AttentionalSelection::releaseAttention,
             "Release attention (allow all regions equal processing)")
        .def("reset", &AttentionalSelection::reset,
             "Reset attentional selection state")
        .def_readonly("inhibitionStrength", &AttentionalSelection::getInhibitionStrength,
             "Current inhibition strength")
        .def_readonly("excitationStrength", &AttentionalSelection::getExcitationStrength,
             "Current excitation strength")
        .def_readonly("attentionFocus", &AttentionalSelection::getAttendedRegions,
             "Current attention focus regions");

    m.def("createPredictionSystem", []() { return std::make_shared<PredictionSystem>(); },
         "Create a prediction system");

    m.def("createNeuralPlanner", []() { return std::make_shared<NeuralPlanner>(); },
         "Create a neural planner");

    m.def("createConceptFormation", []() { return std::make_shared<ConceptFormation>(); },
         "Create a concept formation system");

    m.def("createAttentionalSelection", []() { return std::make_shared<AttentionalSelection>(); },
         "Create an attentional selection system");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // DevelopmentSystem class
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Neural development system orchestrating developmental stages, plasticity modulation, and structural changes)pbdoc")
        .def(py::init<>())
        .def("getCurrentStage", &DevelopmentSystem::getStage,
             "Get current developmental stage")
        .def("setCurrentStage", &DevelopmentSystem::setStage, py::arg("stage"),
             "Set current developmental stage")
        .def("update", static_cast<void (DevelopmentSystem::*)(Brain* head, SimulationStep currentStep)>(&DevelopmentSystem::update),
             py::arg("brain"), py::arg("currentStep"),
             "Update development system with simulation step")
        .def("update", static_cast<void (DevelopmentSystem::*)(Brain* head, RandomGenerator& rng, TimestepDuration dt)>(&DevelopmentSystem::update),
             py::arg("brain"), py::arg("rng"), py::arg("dt"),
             "Update development system with time duration")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier,
             "Get plasticity modifier for current stage")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod,
             "Check if currently in critical period")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress,
             "Get progress through critical period")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge,
             "Get developmental age in simulation time")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"),
             "Set developmental age")
        .def("getStageName", &DevelopmentSystem::getStageName,
             "Get name of current developmental stage")
        .def("enable", [](DevelopmentSystem& self) {
            self.setStage(DevelopmentalStage::CriticalPeriod);
        }, "Enable development system (set to critical period)")
        .def("disable", [](DevelopmentSystem& self) {
            self.setStage(DevelopmentalStage::Adult);
        }, "Disable development system (set to adult stage)")
        .def("isEnabled", &DevelopmentSystem::isCriticalPeriod,
             "Check if development system is enabled (in critical period)");

    // STDP class
    py::class_<STDP, PlasticityRule>(m, "STDP", R"pbdoc(Spike-timing dependent plasticity for synaptic learning)pbdoc")
        .def(py::init<>())
        .def("update", &STDP::update,
             py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"),
             "Update STDP synaptic weights based on spike timing")
        .def("applyWeightChange", &STDP::applyWeightChange,
             py::arg("synapse"), py::arg("delta"),
             "Apply weight change to synapse")
        .def("getName", &STDP::getName,
             "Get STDP system name")
        .def("configure", &STDP::configure, py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"),
             "Configure STDP parameters")
        .def_readwrite("ltpWeight", &STDP::getLTPWeight, &STDP::setLTPWeight,
             "LTP weight parameter")
        .def_readwrite("ltdWeight", &STDP::getLTDWeight, &STDP::setLTDWeight,
             "LTD weight parameter")
        .def_readwrite("timeConstant", &STDP::getTimeConstant, &STDP::setTimeConstant,
             "STDP time constant (tau) in milliseconds")
        .def_readwrite("learningRate", &STDP::getTimeConstant, &STDP::setTimeConstant,
             "Learning rate factor")
        .def_readwrite("isEnabled", [](const STDP& self) { return true; }, [](STDP& self) {},
             "Whether STDP is enabled (placeholder)")
        .def("updatePlasticityModifier", [](STDP& self, float modifier) {},
             "Update plasticity modifier (placeholder)")
        .def("getLearningWindow", []() { return std::vector<float>(); },
             "Get learning window function (placeholder)")
        .def("setLearningWindow", [](STDP& self, const std::vector<float>& window) {},
             "Set learning window function (placeholder)")
        .def("getLearningRate", &STDP::getTimeConstant,
             "Get learning rate")
        .def("setLearningRate", &STDP::setTimeConstant, py::arg("rate"),
             "Set learning rate")
        .def("getPlasticityFactors", []() { return std::vector<float>(); },
             "Get plasticity factors (placeholder)")
        .def("getSpikeHistoryDuration", []() { return 100.0f; },
             "Get spike history duration (placeholder)")
        .def("setSpikeHistoryDuration", [](STDP& self, float duration) {},
             "Set spike history duration (placeholder)");

    // Hebbian class
    py::class_<Hebbian, PlasticityRule>(m, "Hebbian", R"pbdoc(Hebbian learning rule for synaptic strengthening based on correlated activity)pbdoc")
        .def(py::init<>())
        .def("update", &Hebbian::update,
             py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"),
             "Update Hebbian synaptic weights based on correlated activity")
        .def("applyWeightChange", &Hebbian::applyWeightChange,
             py::arg("synapse"), py::arg("delta"),
             "Apply weight change to synapse")
        .def("getName", &Hebbian::getName,
             "Get Hebbian system name")
        .def_readwrite("learningRate", &Hebbian::getLearningRate, &Hebbian::setLearningRate,
             "Hebbian learning rate")
        .def_readwrite("maxWeight", &Hebbian::getMaxWeight, &Hebbian::setMaxWeight,
             "Maximum synaptic weight")
        .def_readwrite("isEnabled", [](const Hebbian& self) { return true; }, [](Hebbian& self) {},
             "Whether Hebbian learning is enabled (placeholder)")
        .def("getStrengthHistory", []() { return std::vector<float>(); },
             "Get Hebbian strength history (placeholder)")
        .def("setHebbianRule", [](Hebbian& self, const std::string& rule) {},
             "Set Hebbian learning rule (placeholder)")
        .def("getHebbianRule", []() { return std::string("DeltaRule"); },
             "Get current Hebbian rule (placeholder)")
        .def("computeHebbianChange", [](const Hebbian& self, float preActivity, float postActivity) { return preActivity * postActivity; },
             "Compute Hebbian change from pre and post synaptic activities")
        .def("getHebbianStrength", &Hebbian::getLearningRate,
             "Get current Hebbian strength")
        .def("getHebbianDecayRate", []() { return 0.01f; },
             "Get Hebbian decay rate (placeholder)")
        .def("setHebbianStrength", &Hebbian::setLearningRate, py::arg("strength"),
             "Set Hebbian strength")
        .def("setHebbianDecayRate", [](Hebbian& self, float decayRate) {},
             "Set Hebbian decay rate (placeholder)")
        .def("reset", [](Hebbian& self) {},
             "Reset Hebbian state (placeholder)");

    // StructuralPlasticity class
    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity system for synapse and neuron creation and removal)pbdoc")
        .def(py::init<>())
        .def("update", &StructuralPlasticity::update,
             py::arg("brain"), py::arg("rng"),
             "Update structural plasticity dynamics")
        .def_readwrite("synaptogenesisRate", &StructuralPlasticity::getSynaptogenesisRate, &StructuralPlasticity::setSynaptogenesisRate,
             "Synaptogenesis (formation) rate")
        .def_readwrite("pruningRate", &StructuralPlasticity::getPruningRate, &StructuralPlasticity::setPruningRate,
             "Synaptic pruning rate")
        .def_readwrite("isEnabled", [](const StructuralPlasticity& self) { return true; }, [](StructuralPlasticity& self) {},
             "Whether structural plasticity is enabled (placeholder)")
        .def("getConnectionCount", &StructuralPlasticity::getSynaptogenesisRate,
             "Get current connection count (placeholder - uses synaptogenesis rate)")
        .def("isConnectionValid", [](const StructuralPlasticity& self, Brain* brain, NeuronId source, NeuronId destination) { return true; },
             "Check if connection between neurons is valid (placeholder)")
        .def("pruneWeakConnections", [](StructuralPlasticity& self, Brain* brain, float threshold) {},
             "Prune weak synaptic connections (placeholder)")
        .def("createNewConnection", [](StructuralPlasticity& self, Brain* brain, NeuronId source, NeuronId destination, SynapticWeight weight) { return SynapseId(); },
             "Create new synaptic connection (placeholder)")
        .def("getConnectionStats", []() { return std::unordered_map<std::string, float>(); },
             "Get connection statistics (placeholder)")
        .def("getGrowthCurve", []() { return std::vector<float>(); },
             "Get growth curve data (placeholder)")
        .def("getPruneCurve", []() { return std::vector<float>(); },
             "Get pruning curve data (placeholder)")
        .def("getConnectionProbability", []() { return 0.5f; },
             "Get connection probability (placeholder)")
        .def("setConnectionProbability", [](StructuralPlasticity& self, float probability) {},
             "Set connection probability (placeholder)")
        .def("getMaxConnections", []() { return 1000; },
             "Get maximum allowed connections (placeholder)")
        .def("setMaxConnections", [](StructuralPlasticity& self, size_t maxConnections) {},
             "Set maximum allowed connections (placeholder)")
        .def("getEnergyCost", []() { return 1.0f; },
             "Get energy cost per connection (placeholder)")
        .def("setEnergyCost", [](StructuralPlasticity& self, float cost) {},
             "Set energy cost per connection (placeholder)")
        .def("getSurvivalRate", []() { return 0.95f; },
             "Get connection survival rate (placeholder)")
        .def("setSurvivalRate", [](StructuralPlasticity& self, float rate) {},
             "Set connection survival rate (placeholder)")
        .def("getAdaptiveRate", []() { return 0.1f; },
             "Get adaptive rate (placeholder)")
        .def("setAdaptiveRate", [](StructuralPlasticity& self, float rate) {},
             "Set adaptive rate (placeholder)")
        .def("reset", [](StructuralPlasticity& self) {},
             "Reset structural plasticity state (placeholder)");

} // namespace nlm
