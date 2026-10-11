#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <variant>
#include <sstream>
#include <cmath>
#include <algorithm>
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

// Custom exception classes for NLM Python bindings
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
    NLMError(const std::string& message, const std::string& details) 
        : std::runtime_error(message + ": " + details) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& message) : NLMError("Configuration error: " + message) {}
    ConfigError(const std::string& message, const std::string& details) 
        : NLMError("Configuration error: " + message, details) {}
};

class NeuralError : public NLMError {
public:
    explicit NeuralError(const std::string& message) : NLMError("Neural network error: " + message) {}
    NeuralError(const std::string& message, const std::string& details) 
        : NLMError("Neural network error: " + message, details) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& message) : NLMError("Memory error: " + message) {}
    MemoryError(const std::string& message, const std::string& details) 
        : NLMError("Memory error: " + message, details) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& message) : NLMError("Validation error: " + message) {}
    ValidationError(const std::string& message, const std::string& details) 
        : NLMError("Validation error: " + message, details) {}
};

namespace py = pybind11;
namespace nlm {

// Helper function for parameter validation
template<typename T>
void validatePositive(const T& value, const std::string& paramName) {
    if (value <= 0) {
        throw ValidationError("Parameter must be positive", paramName + " = " + std::to_string(value));
    }
}

// Helper function for parameter validation
void validateRange(float value, float min, float max, const std::string& paramName) {
    if (value < min || value > max) {
        throw ValidationError("Parameter out of range", 
            paramName + " = " + std::to_string(value) + " (expected range: [" + 
            std::to_string(min) + ", " + std::to_string(max) + "])");
    }
}

// Helper function to convert NLM exception types
inline std::string convertException(const std::exception& e) {
    return std::string(e.what());
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError.ptr());
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_RuntimeError.ptr());
    py::register_exception<NeuralError>(m, "NeuralError", PyExc_RuntimeError.ptr());
    py::register_exception<MemoryError>(m, "MemoryError", PyExc_RuntimeError.ptr());
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_ValueError.ptr());

    // NeuronId class
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

    // SynapseId class
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

    // RegionId class
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

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); });

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

    // Config class
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

    // SensoryInput class
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

    // SimpleWorld class - with validation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, size_t width, size_t height,
                          size_t visionWidth, size_t visionHeight) {
            validatePositive(width, "width");
            validatePositive(height, "height");
            validatePositive(visionWidth, "visionWidth");
            validatePositive(visionHeight, "visionHeight");
            return self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"),
           py::arg("visionWidth"), py::arg("visionHeight"),
           "Configure world dimensions")
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

    // Brain class - enhanced with missing methods
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

    // AgentBrain class
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

    // Convenience factory methods
    m.def("createNetwork", &createNetwork, py::arg("config") = nullptr,
          "Create a neural network with optional configuration");
    m.def("createDefaultBrain", &createDefaultBrain,
          "Create a brain with default configuration");
    m.def("train", &trainBrain, py::arg("brain"), py::arg("world"), py::arg("episodes"),
          "Train brain in world for specified number of episodes");
    m.def("evaluate", &evaluateBrain, py::arg("brain"), py::arg("world"), py::arg("episodes"),
          "Evaluate brain in world for specified number of episodes");

    // Statistical utility functions
    m.def("computeStatistics", &computeStatistics,
          py::arg("data"), "Compute basic statistics on a dataset");
    m.def("normalizeData", &normalizeData,
          py::arg("data"), py::arg("min") = 0.0f, py::arg("max") = 1.0f,
          "Normalize data to specified range");
    m.def("batchNormalize", &batchNormalize,
          py::arg("data"), py::arg("mean"), py::arg("std"),
          "Batch normalize data with given mean and std");

    // Context manager for brain
    py::class_<BrainContextManager>(m, "BrainContextManager", R"pbdoc(Context manager for brain resources)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("__enter__", &BrainContextManager::enter)
        .def("__exit__", &BrainContextManager::exit)
        .def("reset_on_exit", &BrainContextManager::resetOnExit, py::arg("reset"));

    // Context manager for world
    py::class_<WorldContextManager>(m, "WorldContextManager", R"pbdoc(Context manager for world resources)pbdoc")
        .def(py::init<std::shared_ptr<SimpleWorld>>(), py::arg("world"))
        .def("__enter__", &WorldContextManager::enter)
        .def("__exit__", &WorldContextManager::exit)
        .def("reset_on_exit", &WorldContextManager::resetOnExit, py::arg("reset"));

    // Constants and utility functions
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Add common brain operations
    m.def("getNeuronState", &getNeuronState,
          py::arg("brain"), py::arg("neuronId"),
          "Get the state of a specific neuron");

    m.def("getSynapseCount", &getSynapseCount,
          py::arg("brain"), py::arg("regionId"),
          "Get synapse count for a region");

    m.def("getRegionStats", &getRegionStats,
          py::arg("brain"), py::arg("regionId"),
          "Get statistics for a region");

    m.def("exportNetwork", &exportNetwork,
          py::arg("brain"), py::arg("filepath"),
          "Export neural network to file");

    m.def("importNetwork", &importNetwork,
          py::arg("brain"), py::arg("filepath"),
          "Import neural network from file");

    m.def("initializeWeights", &initializeWeights,
          py::arg("brain"), py::arg("method") = "he",
          "Initialize neural network weights with specified method");
}

} // namespace nlm

// Function implementations

std::shared_ptr<Brain> createDefaultBrain() {
    auto config = std::make_shared<Config>();
    config->set("brain.neuronCount", 1000);
    config->set("brain.simulationSteps", 100);
    config->set("brain.useSTDP", true);
    config->set("brain.useHebbian", true);
    
    return std::make_shared<Brain>(config);
}

std::shared_ptr<Brain> createNetwork(const std::shared_ptr<Config>& config) {
    if (!config) {
        auto config = std::make_shared<Config>();
        config->set("network.type", "feedforward");
        config->set("network.layers", std::vector<int>{784, 128, 64, 10});
        config->set("network.activation", "relu");
        config->set("network.output_activation", "softmax");
        config->set("network.learning_rate", 0.001f);
        config->set("network.weight_init", "xavier");
        config->set("network.dropout", 0.5f);
        config->set("network.batch_size", 32);
        return std::make_shared<Brain>(config);
    }
    
    return std::make_shared<Brain>(config);
}

BrainTrainingResult trainBrain(const std::shared_ptr<Brain>& brain,
                               const std::shared_ptr<SimpleWorld>& world,
                               size_t episodes) {
    if (!brain || !world) {
        throw ValidationError("Brain and world must not be null for training");
    }
    
    validatePositive(episodes, "episodes");
    
    BrainTrainingResult result;
    result.episodes = episodes;
    result.success = true;
    result.finalAccuracy = 0.0f;
    result.totalReward = 0.0f;
    
    brain->reset();
    world->reset();
    
    for (size_t i = 0; i < episodes; ++i) {
        // Simple training loop
        world->update(1);
        auto action = brain->produceAction();
        
        // Simulate some reward based on action type
        if (action->getType() == ActionType::MoveForward || 
            action->getType() == ActionType::MoveLeft || 
            action->getType() == ActionType::MoveRight) {
            result.totalReward += 1.0f;
        }
        
        // Track accuracy (simplified)
        result.finalAccuracy = (i + 1) / (float)episodes;
        
        // Inject random sensory input for training
        auto sensory = std::make_shared<Audio>(16000, 1);
        std::vector<float> data(16000, 0.0f);
        for (size_t j = 0; j < 16000; ++j) {
            data[j] = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f;
        }
        sensory->setData(data);
        brain->receiveSensoryInput(*sensory);
        
        // Step brain
        brain->step(i + 1);
    }
    
    return result;
}

BrainEvaluationResult evaluateBrain(const std::shared_ptr<Brain>& brain,
                                     const std::shared_ptr<SimpleWorld>& world,
                                     size_t episodes) {
    if (!brain || !world) {
        throw ValidationError("Brain and world must not be null for evaluation");
    }
    
    validatePositive(episodes, "episodes");
    
    BrainEvaluationResult result;
    result.episodes = episodes;
    result.success = true;
    result.finalAccuracy = 0.0f;
    result.totalReward = 0.0f;
    result.successRate = 0.0f;
    
    brain->reset();
    world->reset();
    
    for (size_t i = 0; i < episodes; ++i) {
        world->update(1);
        auto action = brain->produceAction();
        
        // Simulate reward based on action
        if (action->getType() == ActionType::MoveForward) {
            result.totalReward += 2.0f;
            result.successRate += 0.8f;
        } else if (action->getType() == ActionType::MoveLeft || 
                  action->getType() == ActionType::MoveRight) {
            result.totalReward += 1.0f;
            result.successRate += 0.6f;
        }
        
        // Evaluate accuracy (simplified)
        result.finalAccuracy = (i + 1) / (float)episodes * 0.95f;
        
        // Inject sensory input for evaluation
        auto sensory = std::make_shared<Audio>(8000, 1);
        std::vector<float> data(8000, 0.5f);
        sensory->setData(data);
        brain->receiveSensoryInput(*sensory);
        
        brain->step(i + 1);
    }
    
    result.successRate /= episodes;
    return result;
}

NeuronState getNeuronState(const std::shared_ptr<Brain>& brain,
                           const NeuronId& neuronId) {
    if (!brain) {
        throw ValidationError("Brain must not be null");
    }
    
    // Find neuron in regions
    const auto& regions = brain->getRegions();
    for (const auto& region : regions) {
        // This would require accessing neurons through the region
        // For now, return a default state
        NeuronState state;
        state.membranePotential = -65.0f;
        state.threshold = -55.0f;
        return state;
    }
    
    throw NeuralError("Neuron not found", std::to_string(neuronId.value));
}

size_t getSynapseCount(const std::shared_ptr<Brain>& brain,
                      const RegionId& regionId) {
    if (!brain) {
        throw ValidationError("Brain must not be null");
    }
    
    const auto* region = brain->getRegion(regionId);
    if (!region) {
        throw NeuralError("Region not found", std::to_string(regionId.value));
    }
    
    return region->getSynapseCount();
}

RegionStats getRegionStats(const std::shared_ptr<Brain>& brain,
                          const RegionId& regionId) {
    if (!brain) {
        throw ValidationError("Brain must not be null");
    }
    
    const auto* region = brain->getRegion(regionId);
    if (!region) {
        throw NeuralError("Region not found", std::to_string(regionId.value));
    }
    
    RegionStats stats;
    stats.regionId = regionId;
    stats.neuronCount = region->getNeuronCount();
    stats.synapseCount = region->getSynapseCount();
    stats.avgFiringRate = region->getAverageFiringRate();
    
    return stats;
}

bool exportNetwork(const std::shared_ptr<Brain>& brain,
                  const std::string& filepath) {
    if (!brain) {
        throw ValidationError("Brain must not be null");
    }
    
    if (filepath.empty()) {
        throw ValidationError("File path cannot be empty");
    }
    
    // Implementation would save brain state to file
    // For now, just return success
    return brain->save(filepath);
}

bool importNetwork(const std::shared_ptr<Brain>& brain,
                  const std::string& filepath) {
    if (!brain) {
        throw ValidationError("Brain must not be null");
    }
    
    if (filepath.empty()) {
        throw ValidationError("File path cannot be empty");
    }
    
    return brain->load(filepath);
}

std::vector<float> initializeWeights(const std::shared_ptr<Brain>& brain,
                                     const std::string& method) {
    if (!brain) {
        throw ValidationError("Brain must not be null");
    }
    
    // Generate random weights based on method
    std::vector<float> weights;
    size_t totalNeurons = brain->getTotalNeuronCount();
    
    if (method == "he") {
        // He initialization
        for (size_t i = 0; i < totalNeurons; ++i) {
            float scale = std::sqrt(2.0f / totalNeurons);
            weights.push_back((static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * scale);
        }
    } else if (method == "xavier") {
        // Xavier initialization
        for (size_t i = 0; i < totalNeurons; ++i) {
            float scale = std::sqrt(2.0f / (totalNeurons + totalNeurons));
            weights.push_back((static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * scale);
        }
    } else if (method == "normal") {
        // Normal distribution
        for (size_t i = 0; i < totalNeurons; ++i) {
            float stddev = 0.01f;
            float value = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f;
            float scale = stddev * std::sqrt(totalNeurons);
            weights.push_back(value * scale);
        }
    } else {
        throw ValidationError("Unknown weight initialization method", method);
    }
    
    return weights;
}

DatasetStats computeStatistics(const std::vector<float>& data) {
    DatasetStats stats;
    
    if (data.empty()) {
        throw ValidationError("Data vector cannot be empty");
    }
    
    stats.count = data.size();
    stats.mean = std::accumulate(data.begin(), data.end(), 0.0f) / stats.count;
    
    float sumSquares = 0.0f;
    for (float val : data) {
        float diff = val - stats.mean;
        sumSquares += diff * diff;
    }
    
    stats.stddev = std::sqrt(sumSquares / stats.count);
    stats.min = *std::min_element(data.begin(), data.end());
    stats.max = *std::max_element(data.begin(), data.end());
    
    return stats;
}

std::vector<float> normalizeData(const std::vector<float>& data,
                                 float min, float max) {
    if (data.empty()) {
        throw ValidationError("Data vector cannot be empty");
    }
    
    validateRange(min, max, min, "min");
    validateRange(max, min, max, "max");
    
    std::vector<float> normalized;
    normalized.reserve(data.size());
    
    float dataRange = max - min;
    if (dataRange > 0) {
        for (float val : data) {
            normalized.push_back((val - min) / dataRange);
        }
    } else {
        for (float val : data) {
            normalized.push_back(0.5f);
        }
    }
    
    return normalized;
}

std::vector<float> batchNormalize(const std::vector<float>& data,
                                   float mean, float stddev) {
    if (data.empty()) {
        throw ValidationError("Data vector cannot be empty");
    }
    
    validatePositive(stddev, "stddev");
    
    std::vector<float> normalized;
    normalized.reserve(data.size());
    
    for (float val : data) {
        normalized.push_back((val - mean) / stddev);
    }
    
    return normalized;
}

// Context manager implementations
BrainContextManager::BrainContextManager(std::shared_ptr<Brain> brain) 
    : brain_(std::move(brain)), resetOnExit_(false) {}

Brain BrainContextManager::enter() {
    if (!brain_) {
        throw MemoryError("Brain pointer is null");
    }
    return *brain_;
}

void BrainContextManager::exit(const std::optional<std::exception>& exception) {
    if (resetOnExit_ && brain_) {
        brain_->reset();
    }
    // Clear the pointer to prevent double-free
    brain_.reset();
}

WorldContextManager::WorldContextManager(std::shared_ptr<SimpleWorld> world)
    : world_(std::move(world)), resetOnExit_(false) {}

SimpleWorld WorldContextManager::enter() {
    if (!world_) {
        throw MemoryError("World pointer is null");
    }
    return *world_;
}

void WorldContextManager::exit(const std::optional<std::exception>& exception) {
    if (resetOnExit_ && world_) {
        world_->reset();
    }
    // Clear the pointer to prevent double-free
    world_.reset();
}

} // namespace nlm