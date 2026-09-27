#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
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

namespace py = pybind11;
namespace nlm {

// Custom exception wrapper for Python
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

// Helper functions for type conversion and validation
namespace helpers {
    // Validate neuron count
    inline void validateNeuronCount(size_t count) {
        if (count < 1) {
            throw NLMException("Neuron count must be positive");
        }
    }
    
    // Validate complexity
    inline void validateComplexity(double complexity) {
        if (complexity < 0.0 || complexity > 1.0) {
            throw NLMException("Complexity must be in range [0.0, 1.0]");
        }
    }
    
    // Validate world size
    inline void validateWorldSize(const std::pair<size_t, size_t>& size) {
        if (size.first < 1 || size.second < 1) {
            throw NLMException("World dimensions must be positive");
        }
    }
    
    // Convert string to NeuronType
    inline NeuronType stringToNeuronType(const std::string& str) {
        static const std::unordered_map<std::string, NeuronType> typeMap = {
            {"Excitatory", NeuronType::Excitatory},
            {"Inhibitory", NeuronType::Inhibitory},
            {"Modulatory", NeuronType::Modulatory},
            {"Sensory", NeuronType::Sensory},
            {"Motor", NeuronType::Motor},
            {"Internal", NeuronType::Internal}
        };
        
        auto it = typeMap.find(str);
        if (it == typeMap.end()) {
            throw NLMException("Invalid neuron type: " + str);
        }
        return it->second;
    }
    
    // Convert string to ActionType
    inline ActionType stringToActionType(const std::string& str) {
        static const std::unordered_map<std::string, ActionType> typeMap = {
            {"MoveForward", ActionType::MoveForward},
            {"MoveBackward", ActionType::MoveBackward},
            {"MoveLeft", ActionType::MoveLeft},
            {"MoveRight", ActionType::MoveRight},
            {"TurnLeft", ActionType::TurnLeft},
            {"TurnRight", ActionType::TurnRight},
            {"Look", ActionType::Look},
            {"LookUp", ActionType::LookUp},
            {"LookDown", ActionType::LookDown},
            {"Interact", ActionType::Interact},
            {"Eat", ActionType::Eat},
            {"Drink", ActionType::Drink},
            {"Rest", ActionType::Rest},
            {"Wait", ActionType::Wait},
            {"Custom", ActionType::Custom}
        };
        
        auto it = typeMap.find(str);
        if (it == typeMap.end()) {
            throw NLMException("Invalid action type: " + str);
        }
        return it->second;
    }
    
    // Convert NeuronType to string
    inline std::string neuronTypeToString(NeuronType type) {
        static const std::vector<std::pair<NeuronType, std::string>> typeStrings = {
            {NeuronType::Excitatory, "Excitatory"},
            {NeuronType::Inhibitory, "Inhibitory"},
            {NeuronType::Modulatory, "Modulatory"},
            {NeuronType::Sensory, "Sensory"},
            {NeuronType::Motor, "Motor"},
            {NeuronType::Internal, "Internal"}
        };
        
        for (const auto& pair : typeStrings) {
            if (pair.first == type) {
                return pair.second;
            }
        }
        return "Unknown";
    }
    
    // Convert ActionType to string
    inline std::string actionTypeToString(ActionType type) {
        static const std::vector<std::pair<ActionType, std::string>> typeStrings = {
            {ActionType::MoveForward, "MoveForward"},
            {ActionType::MoveBackward, "MoveBackward"},
            {ActionType::MoveLeft, "MoveLeft"},
            {ActionType::MoveRight, "MoveRight"},
            {ActionType::TurnLeft, "TurnLeft"},
            {ActionType::TurnRight, "TurnRight"},
            {ActionType::Look, "Look"},
            {ActionType::LookUp, "LookUp"},
            {ActionType::LookDown, "LookDown"},
            {ActionType::Interact, "Interact"},
            {ActionType::Eat, "Eat"},
            {ActionType::Drink, "Drink"},
            {ActionType::Rest, "Rest"},
            {ActionType::Wait, "Wait"},
            {ActionType::Custom, "Custom"}
        };
        
        for (const auto& pair : typeStrings) {
            if (pair.first == type) {
                return pair.second;
            }
        }
        return "Unknown";
    }
}

// Python module definitions
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Simplified Python API
        -------------------------------------------------------
        
        This module provides a simplified, user-friendly interface to the NLM
        neural simulation framework. It includes high-level classes for working
        with brains, experiments, and visualization, along with convenience
        functions for common operations.
        
        Key Features:
        - High-level classes: NLMBrain, NLMExperiment, NLMVisualizer
        - Convenience factory functions: create_simple_brain, create_standard_experiment, etc.
        - Training and optimization utilities
        - Comprehensive error handling and validation
        - Detailed documentation and examples
        
        For the original detailed API, import pynlm directly.
    )pbdoc";

    // Register custom Python exceptions
    py::register_exception<NLMException>(m, "NLMError", PyExc_RuntimeError);

    // Core identifier types
    py::class_<NeuronId>(m, "NeuronId", "Unique identifier for a neuron")
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

    py::class_<SynapseId>(m, "SynapseId", "Unique identifier for a synapse")
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

    py::class_<RegionId>(m, "RegionId", "Unique identifier for a brain region")
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

    py::class_<PopulationId>(m, "PopulationId", "Unique identifier for a neuron population")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enumerations
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType")
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

    py::enum_<MotorCommand>(m, "MotorCommand")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Core classes
    py::class_<Config>(m, "Config", "Configuration class for NLM system")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"), "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"), "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"), "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"), "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", "Base class for sensory input")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", "Vision sensory input")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", "Audio sensory input")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", "Internal signals sensory input")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", "Action representation for motor output")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", "World object representation")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    py::class_<AgentBody>(m, "AgentBody", "Agent body state")
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

    py::class_<ActionResult>(m, "ActionResult", "Action result from world")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", "Sensory percept data")
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

    py::class_<SimpleWorld>(m, "SimpleWorld", "Simple 2D world for NLM simulation")
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

    py::class_<Brain>(m, "Brain", "Central neural simulation brain class")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
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
        .def("produceAction", &Brain::produceAction, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal, "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status");

    py::class_<AgentBrain>(m, "AgentBrain", "Agent brain interface connecting NLM brain to world")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal, "Get the underlying brain")
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

    // Convenience factory functions (module-level)
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

    // Module-level convenience functions
    m.def("create_simple_brain", [](size_t neuronCount, double complexity, py::kwargs kwargs) -> std::shared_ptr<Brain> {
        // Helper function to create brain with enhanced parameters
        auto config = std::make_shared<Config>();
        
        // Validate inputs
        if (neuronCount < 1) {
            throw NLMException("Neuron count must be positive");
        }
        if (complexity < 0.0 || complexity > 1.0) {
            throw NLMException("Complexity must be in range [0.0, 1.0]");
        }
        
        // Set up brain configuration based on complexity
        config->set("brain.neuron_count", static_cast<int>(neuronCount));
        
        if (complexity > 0.7) {
            config->set("brain.excitatory_ratio", 0.8);
            config->set("brain.inhibitory_ratio", 0.2);
        } else if (complexity > 0.3) {
            config->set("brain.excitatory_ratio", 0.6);
            config->set("brain.inhibitory_ratio", 0.4);
        } else {
            config->set("brain.excitatory_ratio", 0.5);
            config->set("brain.inhibitory_ratio", 0.5);
        }
        
        // Apply additional kwargs
        for (auto item : kwargs) {
            std::string key = item.first.cast<std::string>();
            // Handle nested keys (e.g., "brain.excitatory_neurons")
            if (key.find('.') != std::string::npos) {
                config->set(key, item.second.cast<double>());
            } else {
                config->set(key, item.second.cast<double>());
            }
        }
        
        return std::make_shared<Brain>(config);
    }, py::arg("neuronCount"), py::arg("complexity"), "Create a simple brain with specified neuron count and complexity");

    m.def("create_standard_experiment", [](std::pair<size_t, size_t> worldSize) -> py::dict {
        auto brain = create_simple_brain(1000, 0.5);
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(static_cast<float>(worldSize.first), static_cast<float>(worldSize.second), 8, 8);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        py::dict result;
        result["brain"] = brain;
        result["world"] = world;
        result["agent"] = agent;
        return result;
    }, py::arg("worldSize"), "Create a standard experiment setup with brain, world, and agent");

    m.def("quick_test", [](std::string mode, size_t steps) -> py::dict {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        py::dict results;
        results["passed"] = true;
        results["mode"] = mode;
        results["steps"] = steps;
        results["metrics"] = py::dict();
        results["errors"] = py::list();
        
        try {
            if (mode == "connectivity" || mode == "all") {
                size_t maxActive = 0;
                for (size_t i = 0; i < steps; ++i) {
                    brain->step(i);
                    size_t active = brain->getActiveNeuronCount();
                    maxActive = std::max(maxActive, active);
                    if (active == 0 && i > 100) {
                        results["passed"] = false;
                        results["errors"].attr("append")(std::string("No active neurons at step " + std::to_string(i)));
                        break;
                    }
                }
                results["metrics"]["max_active_neurons"] = static_cast<double>(maxActive);
            }
            
            if (mode == "firing" || mode == "all") {
                std::vector<double> firingRates;
                for (size_t i = 0; i < steps; ++i) {
                    brain->step(i);
                    firingRates.push_back(brain->getAverageFiringRate());
                }
                
                if (!firingRates.empty()) {
                    double sum = 0.0, sumSq = 0.0;
                    for (double rate : firingRates) {
                        sum += rate;
                        sumSq += rate * rate;
                    }
                    double mean = sum / firingRates.size();
                    double stddev = std::sqrt((sumSq / firingRates.size()) - (mean * mean));
                    
                    results["metrics"]["firing_rate_mean"] = mean;
                    results["metrics"]["firing_rate_std"] = stddev;
                    results["metrics"]["firing_rate_min"] = *std::min_element(firingRates.begin(), firingRates.end());
                    results["metrics"]["firing_rate_max"] = *std::max_element(firingRates.begin(), firingRates.end());
                }
            }
            
            if (mode == "learning" || mode == "all") {
                auto initialStage = brain->getDevelopmentalStage();
                for (size_t i = 0; i < steps; ++i) {
                    brain->step(i);
                    auto currentStage = brain->getDevelopmentalStage();
                    if (currentStage != initialStage && currentStage != DevelopmentalStage::Adult) {
                        results["passed"] = false;
                        results["errors"].attr("append")(
                            std::string("Unexpected developmental stage: " + std::to_string(static_cast<int>(currentStage)) + " at step " + std::to_string(i)));
                        break;
                    }
                }
            }
            
        } catch (const std::exception& e) {
            results["passed"] = false;
            results["errors"].attr("append")(std::string("Exception: ") + e.what());
        }
        
        return results;
    }, py::arg("mode") = "connectivity", py::arg("steps") = 1000, "Quick test of NLM functionality");

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Module-level helper functions
    m.def("validate_params", [](size_t neuronCount, double complexity, std::pair<size_t, size_t> worldSize) {
        helpers::validateNeuronCount(neuronCount);
        helpers::validateComplexity(complexity);
        helpers::validateWorldSize(worldSize);
    }, py::arg("neuronCount"), py::arg("complexity"), py::arg("worldSize"),
       "Validate NLM parameters (neuron count, complexity, world size)");

    // Document the new high-level classes
    py::class_<NLMBrain>(m, "NLMBrain", "Simplified brain interface for common operations")
        .def(py::init<size_t, double, py::kwargs>(), py::arg("neuronCount") = 1000, py::arg("complexity") = 0.5,
             "Initialize NLMBrain with neuron count and complexity")
        .def("initialize", &NLMBrain::initialize, "Initialize the brain for simulation")
        .def("step", &NLMBrain::step, py::arg("stepNumber"), py::arg("currentTime") = nullptr,
             "Perform one simulation step")
        .def("reset", &NLMBrain::reset, "Reset the brain to initial state")
        .def("get_total_neurons", &NLMBrain::getTotalNeuronCount, "Get total neuron count")
        .def("get_total_synapses", &NLMBrain::getTotalSynapseCount, "Get total synapse count")
        .def("get_active_neurons", &NLMBrain::getActiveNeuronCount, "Get count of active neurons")
        .def("get_firing_neurons", &NLMBrain::getFiringNeuronCount, "Get count of firing neurons")
        .def("get_average_firing_rate", &NLMBrain::getAverageFiringRate, "Get average firing rate")
        .def("get_total_spikes", &NLMBrain::getTotalSpikeCount, "Get total spike count")
        .def("get_neuron_count", &NLMBrain::getNeuronCount, "Get number of neurons")
        .def("get_complexity", &NLMBrain::getComplexity, "Get brain complexity (0.0-1.0)");

    py::class_<NLMExperiment>(m, "NLMExperiment", "High-level experiment runner")
        .def(py::init<size_t, std::pair<size_t, size_t>, py::kwargs>(),
             py::arg("neuronCount") = 1000, py::arg("worldSize") = std::make_pair<size_t, size_t>(10, 10),
             "Create NLMExperiment with specified neuron count and world size")
        .def_static("create_simple", [](size_t neuronCount, std::pair<size_t, size_t> worldSize) {
            return NLMExperiment(neuronCount, worldSize);
        }, py::arg("neuronCount"), py::arg("worldSize"), "Create a simple experiment")
        .def_static("create_standard", [](std::pair<size_t, size_t> worldSize) {
            return NLMExperiment(1000, worldSize);
        }, py::arg("worldSize"), "Create a standard experiment")
        .def("run", &NLMExperiment::run, py::arg("numSteps"), "Run the experiment")
        .def("enable_features", &NLMExperiment::enableFeatures,
             py::arg("rewardModulation") = false, py::arg("structuralPlasticity") = false,
             py::arg("development") = false, py::arg("curiosity") = false,
             "Enable optional brain features");

    py::class_<NLMVisualizer>(m, "NLMVisualizer", "Simple visualization interface")
        .def(py::init<std::string>(), py::arg("backend") = "console", "Initialize visualizer")
        .def("enable", &NLMVisualizer::enable, "Enable visualization output")
        .def("disable", &NLMVisualizer::disable, "Disable visualization output")
        .def("show_firing_rate", &NLMVisualizer::showFiringRate,
             py::arg("firingRate"), py::arg("step") = 0, "Display firing rate")
        .def("show_neuron_stats", &NLMVisualizer::showNeuronStats, py::arg("brain"), "Show brain statistics")
        .def("show_simulation_progress", &NLMVisualizer::showSimulationProgress,
             py::arg("currentStep"), py::arg("totalSteps"), py::arg("metrics") = nullptr, "Show simulation progress")
        .def("show", &NLMVisualizer::show, "Display visualizations");
}

} // namespace nlm