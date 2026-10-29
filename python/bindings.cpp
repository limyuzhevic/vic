#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"

namespace py = pybind11;
namespace nlm {

// Enhanced Python bindings with additional functionality and error handling
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        ---------------------------------------------------------
        
        Enhanced Python API for the NLM C++ neural simulation framework.
        
        Key improvements:
        * Type-safe parameter validation and error handling
        * Advanced configuration with dictionary interface
        * Enhanced factory methods with flexible parameters
        * Improved documentation and examples
        * Better error messages and exception handling
        * Support for advanced neural simulation features
    )pbdoc";

    // Register custom exceptions
    class NLMRuntimeError : public std::runtime_error {
    public:
        NLMRuntimeError(const std::string& message) : std::runtime_error("NLM Runtime Error: " + message) {}
    };
    
    class NLMConfigurationError : public std::runtime_error {
    public:
        NLMConfigurationError(const std::string& message) : std::runtime_error("NLM Configuration Error: " + message) {}
    };
    
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMConfigurationError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const NLMRuntimeError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown NLM error");
        }
    });

    // Enhanced NeuronId with better representation and conversion
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__lt__", &NeuronId::operator<)
        .def("__le__", &NeuronId::operator<=)
        .def("__gt__", &NeuronId::operator>)
        .def("__ge__", &NeuronId::operator>=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // Enhanced Config class with dictionary interface and validation
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system with enhanced interface)")
        .def(py::init<>())
        
        // Enhanced file operations
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from file with validation")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to file")
        
        // Enhanced argument parsing
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        
        // Enhanced parameter setting with type safety
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set string configuration parameter")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set integer configuration parameter")
        .def("set", [](Config& self, const std::string& key, float value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set float configuration parameter")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set boolean configuration parameter")
        
        // Enhanced parameter getting with type safety
        .def("get", [](Config& self, const std::string& key, const std::string& default_value) {
            auto result = self.get<std::string>(key);
            return result.has_value() ? result.value() : default_value;
        }, py::arg("key"), py::arg("default_value") = "",
           "Get string configuration parameter with default")
        .def("get", [](Config& self, const std::string& key, int default_value) {
            auto result = self.get<int>(key);
            return result.has_value() ? result.value() : default_value;
        }, py::arg("key"), py::arg("default_value") = 0,
           "Get integer configuration parameter with default")
        .def("get", [](Config& self, const std::string& key, float default_value) {
            auto result = self.get<float>(key);
            return result.has_value() ? result.value() : default_value;
        }, py::arg("key"), py::arg("default_value") = 0.0f,
           "Get float configuration parameter with default")
        .def("get", [](Config& self, const std::string& key, bool default_value) {
            auto result = self.get<bool>(key);
            return result.has_value() ? result.value() : default_value;
        }, py::arg("key"), py::arg("default_value") = false,
           "Get boolean configuration parameter with default")
        
        // Enhanced validation
        .def("has", &Config::has, py::arg("key"),
             "Check if configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("summary", &Config::summary,
             "Get configuration summary")
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced Brain class with additional methods
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with enhanced functionality)")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        
        // Enhanced step methods
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        
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
             "Get a region by ID")
        
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        
        .def("getRegions", &Brain::getRegions,
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
             py::arg("stage"), "Set developmental stage")
        
        .def("getConfig", &Brain::getConfig,
             "Get the configuration")
        
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             "Get working memory")
        
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             "Get episodic memory")
        
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             "Get associative memory")
        
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             "Get prediction system")
        
        .def("getPlanner", &Brain::getPlanner,
             "Get neural planner")
        
        .def("getConceptFormation", &Brain::getConceptFormation,
             "Get concept formation system")
        
        .def("getAttention", &Brain::getAttention,
             "Get attention system")
        
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             "Get development system")
        
        .def("getDopamine", &Brain::getDopamine,
             "Get dopamine system")
        
        .def("getCuriosity", &Brain::getCuriosity,
             "Get curiosity system")
        
        .def("getNovelty", &Brain::getNovelty,
             "Get novelty system")
        
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             "Get prediction error signal")
        
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             "Get random generator");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize with brain")
        
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        
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
             "Get the underlying brain")
        
        // Enhanced plasticity controls
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable curiosity")
        
        // Status checking
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)")
        .def(py::init<>())
        
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision")
        
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        
        .def("setAgentStart", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"), "Set agent starting position")
        
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep")
        
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent")
        
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             "Get current sensory percept for the agent")
        
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             "Get agent body state")
        
        .def("addObject", &SimpleWorld::addObject,
             py::arg("obj"), "Add object to world")
        
        .def("removeObject", &SimpleWorld::removeObject,
             py::arg("x"), py::arg("y"), "Remove object at position")
        
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"), "Check if position is valid")
        
        .def("getObjectAt", &SimpleWorld::getObjectAt,
             py::arg("x"), py::arg("y"), "Get object at position")
        
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy")
        
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        
        .def("getEnergyGainRate", &SimpleWorld::getEnergyGainRate,
             "Get energy gain rate")
        
        .def("setEnergyGainRate", &SimpleWorld::setEnergyGainRate, py::arg("r"),
             "Set energy gain rate")
        
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed");

    // Enhanced factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Export constants for backward compatibility
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Enhanced enumeration with additional values for better compatibility
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();
    
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)")
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
    
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command enumeration)")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();
    
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Enhanced documentation strings
    m.attr("__version__") = "2.0.0 Enhanced Edition";
    m.attr("__description__") = "NLM (Neural Learning Machine) - Enhanced Python bindings for neural simulation";
    m.attr("__author__") = "NLM Development Team";
}

} // namespace nlm
