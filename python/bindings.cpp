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
        
        This module provides:
        - Context managers for safe resource management
        - Helper functions for common simulation patterns
        - Enhanced error handling with Python-specific exceptions
        - Convenience methods for easier integration
    )pbdoc";

    // Enhanced error handling for all bound methods
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            if (!self.loadFromFile(filepath)) {
                throw std::runtime_error("Failed to load configuration from file: " + filepath);
            }
            return true;
        }, py::arg("filepath"), "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            if (!self.loadFromArgs(argc, argv)) {
                throw std::runtime_error("Failed to load configuration from command line arguments");
            }
            return true;
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            if (!self.saveToFile(filepath)) {
                throw std::runtime_error("Failed to save configuration to file: " + filepath);
            }
            return true;
        }, py::arg("filepath"), "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) {
                return *value;
            }
            throw std::out_of_range("Configuration key not found: " + key);
        }, py::arg("key"), "Get configuration value (string)")
        .def("getInt", [](const Config& self, const std::string& key) {
            auto value = self.get<int64_t>(key);
            if (value) {
                return *value;
            }
            throw std::out_of_range("Configuration key not found: " + key);
        }, py::arg("key"), "Get configuration value (integer)")
        .def("getFloat", [](const Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            if (value) {
                return *value;
            }
            throw std::out_of_range("Configuration key not found: " + key);
        }, py::arg("key"), "Get configuration value (float)")
        .def("getBool", [](const Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            if (value) {
                return *value;
            }
            throw std::out_of_range("Configuration key not found: " + key);
        }, py::arg("key"), "Get configuration value (boolean)")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__enter__", [](Config& self) { return &self; })
        .def("__exit__", [](Config&, std::exception_ptr) {});

    // Enhanced AgentBrain with better error handling
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", [](AgentBrain& self, const SensoryPercept& percept) {
            if (!self.getBrain()) {
                throw std::runtime_error("AgentBrain not properly initialized - brain is null");
            }
            self.processSensoryInput(percept);
        }, py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", [](AgentBrain& self) -> MotorCommand {
            if (!self.getBrain()) {
                throw std::runtime_error("AgentBrain not properly initialized - brain is null");
            }
            return self.decodeMotorCommand();
        }, "Decode brain motor activity into motor command")
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

    // Enhanced Brain with memory management and error handling
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", [](Brain& self) {
            if (!self.initialize()) {
                throw std::runtime_error("Failed to initialize brain");
            }
            return true;
        }, "Initialize the brain with configuration")
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
        .def("produceAction", []() -> std::unique_ptr<Action> {
            throw std::runtime_error("Error: Invalid binding for produceAction() - needs proper memory management");
        }, "Produce motor action based on neural activity")
        .def("reset", [](Brain& self) {
            if (!self.getConfig()) {
                throw std::runtime_error("Cannot reset brain - configuration is null");
            }
            self.reset();
        }, "Reset brain state")
        .def("save", [](Brain& self, const std::string& filepath) {
            if (!self.save(filepath)) {
                throw std::runtime_error("Failed to save brain state to file: " + filepath);
            }
            return true;
        }, py::arg("filepath"), "Save brain state to file")
        .def("load", [](Brain& self, const std::string& filepath) {
            if (!self.load(filepath)) {
                throw std::runtime_error("Failed to load brain state from file: " + filepath);
            }
            return true;
        }, py::arg("filepath"), "Load brain state from file")
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get pending spike event count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("getSpikeSystem", [](const Brain& self) -> SpikeSystem* {
            auto* spikeSystem = self.getSpikeSystem();
            if (!spikeSystem) {
                throw std::runtime_error("Spike system is not initialized");
            }
            return spikeSystem;
        }, "Get spike system")
        .def("getSTDP", [](const Brain& self) -> STDP* {
            auto* stdp = self.getSTDP();
            if (!stdp) {
                throw std::runtime_error("STDP plasticity system is not initialized");
            }
            return stdp;
        }, "Get STDP plasticity system")
        .def("getHebbian", [](const Brain& self) -> Hebbian* {
            auto* hebbian = self.getHebbian();
            if (!hebbian) {
                throw std::runtime_error("Hebbian plasticity system is not initialized");
            }
            return hebbian;
        }, "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", [](const Brain& self) -> StructuralPlasticity* {
            auto* sp = self.getStructuralPlasticity();
            if (!sp) {
                throw std::runtime_error("Structural plasticity system is not initialized");
            }
            return sp;
        }, "Get structural plasticity system")
        .def("getWorkingMemory", [](const Brain& self) -> NeuralWorkingMemory* {
            auto* wm = self.getWorkingMemory();
            if (!wm) {
                throw std::runtime_error("Working memory system is not initialized");
            }
            return wm;
        }, "Get working memory system")
        .def("getEpisodicMemory", [](const Brain& self) -> NeuralEpisodicMemory* {
            auto* em = self.getEpisodicMemory();
            if (!em) {
                throw std::runtime_error("Episodic memory system is not initialized");
            }
            return em;
        }, "Get episodic memory system")
        .def("getAssociativeMemory", [](const Brain& self) -> NeuralAssociativeMemory* {
            auto* am = self.getAssociativeMemory();
            if (!am) {
                throw std::runtime_error("Associative memory system is not initialized");
            }
            return am;
        }, "Get associative memory system")
        .def("getPredictionSystem", [](const Brain& self) -> PredictionSystem* {
            auto* ps = self.getPredictionSystem();
            if (!ps) {
                throw std::runtime_error("Prediction system is not initialized");
            }
            return ps;
        }, "Get prediction system")
        .def("getPlanner", [](const Brain& self) -> NeuralPlanner* {
            auto* planner = self.getPlanner();
            if (!planner) {
                throw std::runtime_error("Neural planner system is not initialized");
            }
            return planner;
        }, "Get neural planner system")
        .def("getConceptFormation", [](const Brain& self) -> ConceptFormation* {
            auto* cf = self.getConceptFormation();
            if (!cf) {
                throw std::runtime_error("Concept formation system is not initialized");
            }
            return cf;
        }, "Get concept formation system")
        .def("getAttention", [](const Brain& self) -> AttentionalSelection* {
            auto* attention = self.getAttention();
            if (!attention) {
                throw std::runtime_error("Attentional selection system is not initialized");
            }
            return attention;
        }, "Get attentional selection system")
        .def("getDevelopmentSystem", [](const Brain& self) -> DevelopmentSystem* {
            auto* dev = self.getDevelopmentSystem();
            if (!dev) {
                throw std::runtime_error("Development system is not initialized");
            }
            return dev;
        }, "Get development system")
        .def("getDopamine", [](const Brain& self) -> Dopamine* {
            auto* dopamine = self.getDopamine();
            if (!dopamine) {
                throw std::runtime_error("Dopamine neuromodulation system is not initialized");
            }
            return dopamine;
        }, "Get dopamine neuromodulation system")
        .def("getCuriosity", [](const Brain& self) -> Curiosity* {
            auto* curiosity = self.getCuriosity();
            if (!curiosity) {
                throw std::runtime_error("Curiosity neuromodulation system is not initialized");
            }
            return curiosity;
        }, "Get curiosity neuromodulation system")
        .def("getNovelty", [](const Brain& self) -> Novelty* {
            auto* novelty = self.getNovelty();
            if (!novelty) {
                throw std::runtime_error("Novelty neuromodulation system is not initialized");
            }
            return novelty;
        }, "Get novelty neuromodulation system")
        .def("getPredictionErrorSignal", [](const Brain& self) -> PredictionError* {
            auto* pes = self.getPredictionErrorSignal();
            if (!pes) {
                throw std::runtime_error("Prediction error system is not initialized");
            }
            return pes;
        }, "Get prediction error system")
        .def("getRandomGenerator", [](const Brain& self) -> RandomGenerator* {
            auto* rng = self.getRandomGenerator();
            if (!rng) {
                throw std::runtime_error("Random generator is not initialized");
            }
            return rng;
        }, "Get random generator")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"))
        .def("updatePlasticity", [](Brain& self) {
            if (!self.getConfig()) {
                throw std::runtime_error("Cannot update plasticity - configuration is null");
            }
            self.updatePlasticity();
        }, "Update plasticity rules")
        .def("develop", [](Brain& self) {
            if (!self.getConfig()) {
                throw std::runtime_error("Cannot develop - configuration is null");
            }
            self.develop();
        }, "Apply developmental changes")
        .def("__repr__", [](const Brain& b) {
            return "<Brain with " + std::to_string(b.getTotalNeuronCount()) + " neurons>";
        });

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

    // SimpleWorld context manager for safe resource management
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("__enter__", [](SimpleWorld& self) { return &self; })
        .def("__exit__", [](SimpleWorld&, std::exception_ptr) {});

    // Config context manager
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
        })
        .def("__enter__", [](Config& self) { return &self; })
        .def("__exit__", [](Config&, std::exception_ptr) {});

    // SensoryInput base class with helper methods
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("as_vision", &SensoryInput::getVision, "Get as vision data (if applicable)")
        .def("as_audio", &SensoryInput::getAudio, "Get as audio data (if applicable)")
        .def("as_internal", &SensoryInput::getInternal, "Get as internal signals (if applicable)");

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

// Context manager for Brain
    class_<Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize)
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step), py::arg("currentStep"))
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step), py::arg("currentStep"), py::arg("currentTime"))
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"))
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"))
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"))
        .def("produceAction", &Brain::produceAction)
        .def("reset", &Brain::reset)
        .def("save", &Brain::save, py::arg("filepath"))
        .def("load", &Brain::load, py::arg("filepath"))
        .def("addRegion", &Brain::addRegion, py::arg("name") = "")
        .def("getRegion", &Brain::getRegion, py::arg("id"), py::return_value_policy::reference_internal)
        .def("getRegionCount", &Brain::getRegionCount)
        .def("getRegionIds", &Brain::getRegionIds)
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal)
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount)
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount)
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &Brain::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount)
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount)
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage)
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"))
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal)
        .def("getSpikeSystem", &Brain::getSpikeSystem)
        .def("getSTDP", &Brain::getSTDP)
        .def("getHebbian", &Brain::getHebbian)
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity)
        .def("getWorkingMemory", &Brain::getWorkingMemory)
        .def("getEpisodicMemory", &Brain::getEpisodicMemory)
        .def("getAssociativeMemory", &Brain::getAssociativeMemory)
        .def("getPredictionSystem", &Brain::getPredictionSystem)
        .def("getPlanner", &Brain::getPlanner)
        .def("getConceptFormation", &Brain::getConceptFormation)
        .def("getAttention", &Brain::getAttention)
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem)
        .def("getDopamine", &Brain::getDopamine)
        .def("getCuriosity", &Brain::getCuriosity)
        .def("getNovelty", &Brain::getNovelty)
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal)
        .def("getRandomGenerator", &Brain::getRandomGenerator)
        .def("logStatus", &Brain::logStatus)
        .def("applyNeuromodulation", &Brain::applyNeuromodulation, py::arg("signal"))
        .def("updatePlasticity", &Brain::updatePlasticity)
        .def("develop", &Brain::develop)
        .def("__repr__", [](const Brain& b) {
            return "<Brain with " + std::to_string(b.getTotalNeuronCount()) + " neurons>";
        });

    // Context manager helper functions for simulation workflows
    m.def("run_episode", [](SimpleWorld& world, Brain& brain, AgentBrain& agent, int steps) {
        """Run a complete simulation episode with automatic state management."""
        world.reset();
        brain.reset();
        brain.initialize();
        agent.reset();
        
        for (int step = 0; step < steps; ++step) {
            // Update world
            world.update(0.1);
            
            // Get sensory input
            auto percept = world.getSensoryPercept();
            
            // Process in brain
            agent.processSensoryInput(percept);
            brain.step(step);
            
            // Get and apply action
            auto action = agent.decodeMotorCommand();
            world.applyMotorCommand(action, world.getSimulationTime());
        }
        
        return std::make_tuple(
            brain.getTotalSpikeCount(),
            brain.getAverageFiringRate(),
            agent.getCuriosityLevel()
        );
    }, py::arg("world"), py::arg("brain"), py::arg("agent"), py::arg("steps"),
       "Run a complete simulation episode");

    // Create vision sensory input with convenient defaults
    m.def("create_vision", [](size_t width, size_t height, size_t channels) {
        auto vision = std::make_shared<Vision>(width, height, channels);
        vision->setData(std::vector<float>(width * height * channels, 0.0f));
        return vision;
    }, py::arg("width") = 64, py::arg("height") = 64, py::arg("channels") = 3,
       "Create vision sensory input with default data");

    // Create audio sensory input
    m.def("create_audio", [](size_t sampleRate, size_t numSamples) {
        auto audio = std::make_shared<Audio>(sampleRate, numSamples);
        audio->setData(std::vector<float>(numSamples, 0.0f));
        return audio;
    }, py::arg("sampleRate") = 44100, py::arg("numSamples") = 1024,
       "Create audio sensory input with default silent data");

    // Create internal signals
    m.def("create_internal_signals", []() {
        auto signals = std::make_shared<InternalSignals>();
        return signals;
    }, "Create internal signals sensory input");

    // Convenience function for running simulations with stats collection
    m.def("run_simulation", [](Brain& brain, int steps, bool trackStats = true) {
        """Run a simulation with optional statistics tracking."""
        std::vector<float> firingRates;
        std::vector<size_t> spikeCounts;
        
        for (int step = 0; step < steps; ++step) {
            brain.step(step);
            
            if (trackStats) {
                firingRates.push_back(brain.getFiringNeuronCount());
                spikeCounts.push_back(brain.getTotalSpikeCount());
            }
        }
        
        return std::make_tuple(
            firingRates,
            spikeCounts,
            brain.getAverageFiringRate(),
            brain.getTotalSpikeCount()
        );
    }, py::arg("brain"), py::arg("steps"), py::arg("trackStats") = true,
       "Run a simulation and optionally collect statistics");

    // Convenience function for loading/saving configurations
    m.def("load_config", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(filepath)) {
            return config;
        } else {
            PyErr_SetString(PyExc_IOError, ("Failed to load config from " + filepath).c_str());
            py::error_already_set();
        }
        return config; // Never reached if error thrown
    }, py::arg("filepath"), "Load configuration from file");

    // Create brain with default config
    m.def("create_brain", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create brain with default configuration");

    // Create agent with world
    m.def("create_agent_world", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100, 100, 16, 16);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, "Create complete agent system with brain, world, and agent");

    // Python-friendly sensor data creation
    m.def("vision_data", [](size_t width, size_t height, const std::vector<float>& data) {
        if (data.size() != width * height * 3) {
            throw std::invalid_argument("Vision data size must be width * height * 3");
        }
        auto vision = std::make_shared<Vision>(width, height, 3);
        vision->setData(data);
        return vision;
    }, py::arg("width"), py::arg("height"), py::arg("data"), 
       "Create vision data from raw pixel values");

    // Create random configuration with common defaults
    m.def("create_default_config", []() {
        auto config = std::make_shared<Config>();
        
        // Set common defaults for research
        config->set("brain.neuron_count", static_cast<int64_t>(1000));
        config->set("brain.synapse_density", 0.1f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.001f);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        
        return config;
    }, "Create a configuration with common research defaults");

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
