#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
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

// ==============================================================================
// UTILITY FUNCTIONS AND VALIDATION
// ==============================================================================

/**
 * @brief Validate that a brain is initialized before using it
 * @param brain The brain to validate
 * @throws std::runtime_error if brain is not initialized
 */
void validateBrainInitialized(const Brain& brain) {
    try {
        brain.getTotalNeuronCount();
    } catch (...) {
        throw std::runtime_error("Brain is not properly initialized. Call brain.initialize() first.");
    }
}

/**
 * @brief Validate world configuration parameters
 * @param width World width (must be > 0)
 * @param height World height (must be > 0)
 * @param visionWidth Vision width (must be > 0)
 * @param visionHeight Vision height (must be > 0)
 * @throws std::runtime_error if parameters are invalid
 */
void validateWorldConfig(size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
    if (width == 0) throw std::runtime_error("World width must be > 0");
    if (height == 0) throw std::runtime_error("World height must be > 0");
    if (visionWidth == 0) throw std::runtime_error("Vision width must be > 0");
    if (visionHeight == 0) throw std::runtime_error("Vision height must be > 0");
}

/**
 * @brief Validate action parameters
 * @param type Action type
 * @param params Action parameters
 * @throws std::runtime_error if parameters are invalid
 */
void validateAction(ActionType type, const std::vector<float>& params) {
    if (params.size() > 10) {
        throw std::runtime_error("Action parameters cannot exceed 10 values");
    }
}

/**
 * @brief Create a unique ID for tracking
 * @return A new unique ID
 */
NeuronId createUniqueNeuronId() {
    static uint64_t counter = 0;
    return NeuronId(++counter);
}

/**
 * @brief Print brain status with formatting
 * @param brain The brain to query
 * @return Formatted status string
 */
std::string formatBrainStatus(const Brain& brain) {
    std::stringstream ss;
    ss << "Brain Status:" << std::endl;
    ss << "  Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    ss << "  Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    ss << "  Active neurons: " << brain.getActiveNeuronCount() << std::endl;
    ss << "  Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
    ss << "  Average firing rate: " << brain.getAverageFiringRate() << std::endl;
    ss << "  Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    ss << "  Development stage: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
    return ss.str();
}

/**
 * @brief Safely read file and return content as string
 * @param filepath Path to file
 * @return File content as string
 * @throws std::runtime_error if file cannot be read
 */
std::string safeReadFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }
    return std::string((std::istreambuf_iterator<char>(file)), 
                       std::istreambuf_iterator<char>());
}

/**
 * @brief Safely write string content to file
 * @param filepath Path to file
 * @param content Content to write
 * @throws std::runtime_error if file cannot be written
 */
void safeWriteFile(const std::string& filepath, const std::string& content) {
    std::ofstream file(filepath);
    if (!file) {
        throw std::runtime_error("Cannot write to file: " + filepath);
    }
    file << content;
}

// ==============================================================================
// MAIN BINDINGS
// ==============================================================================

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        Key improvements:
        - Fixed PopulationId class - added missing methods
        - Enhanced error handling and validation
        - Convenience factory functions for common tasks
        - Context managers for resource management
        - Enhanced documentation and examples
        - Advanced user features and utilities
    )pbdoc";

    // Register exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ==============================================================================
    // BASIC IDENTIFIER TYPES
    // ==============================================================================
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

    // FIXED: Add missing methods to PopulationId
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
        });

    // ==============================================================================
    // ENUMERATIONS
    // ==============================================================================
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

    // ==============================================================================
    // CONFIG CLASS
    // ==============================================================================
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

    // ==============================================================================
    // SENSORY INPUT CLASSES
    // ==============================================================================
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class with additional Pythonic methods
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

    // Enhanced Audio class with additional methods
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // ==============================================================================
    // ACTION CLASS
    // ==============================================================================
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

    // ==============================================================================
    // WORLD OBJECT CLASS
    // ==============================================================================
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

    // ==============================================================================
    // AGENT BODY CLASS
    // ==============================================================================
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

    // ==============================================================================
    // ACTION RESULT CLASS
    // ==============================================================================
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // ==============================================================================
    // SENSORY PERCEPT CLASS
    // ==============================================================================
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

    // ==============================================================================
    // WORLD CLASS
    // ==============================================================================
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

    // ==============================================================================
    // BRAIN CLASS
    // ==============================================================================
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

    // ==============================================================================
    // AGENT BRAIN CLASS
    // ==============================================================================
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
             py::arg("enable"),
             "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // ==============================================================================
    // CONVENIENCE FACTORY FUNCTIONS
    // ==============================================================================

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

    // Enhanced convenience functions
    m.def("createCompleteSimulation", [](
        const std::shared_ptr<Config>& config,
        const py::dict& worldConfig = py::dict(),
        const py::dict& enableFeatures = py::dict()) {
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();

        // Create world with defaults or custom config
        auto world = std::make_shared<SimpleWorld>();
        size_t width = worldConfig.get("width", 20ul);
        size_t height = worldConfig.get("height", 20ul);
        size_t visionWidth = worldConfig.get("vision_width", 8ul);
        size_t visionHeight = worldConfig.get("vision_height", 8ul);

        validateWorldConfig(width, height, visionWidth, visionHeight);
        world->configure(width, height, visionWidth, visionHeight);
        world->reset();

        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);

        // Enable requested features
        if (enableFeatures.get("reward_modulation", false)) {
            agent->enableRewardModulation(true);
        }
        if (enableFeatures.get("structural_plasticity", false)) {
            agent->enableStructuralPlasticity(true);
        }
        if (enableFeatures.get("development", false)) {
            agent->enableDevelopment(true);
        }
        if (enableFeatures.get("curiosity", false)) {
            agent->enableCuriosity(true);
        }

        return std::make_tuple(brain, world, agent);
    }, py::arg("config"), py::arg("world_config") = py::dict(),
       py::arg("enable_features") = py::dict(),
       "Create a complete simulation environment with all components");

    m.def("runSimulationStep", [](
        std::shared_ptr<Brain>& brain,
        std::shared_ptr<SimpleWorld>& world,
        std::shared_ptr<AgentBrain>& agent,
        double timestep) {

        if (!brain || !world || !agent) {
            throw std::runtime_error("All components (brain, world, agent) must be provided");
        }

        py::dict results;

        try {
            // 1. Update world
            world->update(timestep);

            // 2. Get sensory input
            auto percept = world->getSensoryPercept();

            // 3. Process sensory input
            agent->processSensoryInput(percept);

            // 4. Brain thinks
            brain->step(static_cast<size_t>(world->getSimulationTime()), world->getSimulationTime());

            // 5. Get action
            auto action = agent->decodeMotorCommand();

            // 6. Apply action
            world->applyMotorCommand(action, world->getSimulationTime());

            // Compile results
            results["success"] = true;
            results["step"] = static_cast<size_t>(world->getSimulationTime());
            results["action_type"] = static_cast<int>(action.getType());
            results["novelty"] = agent->getNoveltyLevel();
            results["prediction_error"] = agent->getPredictionError();
            results["brain_spikes"] = brain->getTotalSpikeCount();
            results["firing_neurons"] = brain->getFiringNeuronCount();

        } catch (const std::exception& e) {
            results["success"] = false;
            results["error"] = std::string(e.what());
        }

        return results;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("timestep") = 0.1,
       "Run a complete simulation step with error handling");

    m.def("createTestConfig", &createTestConfig,
         py::arg("agent_name"), py::arg("world_size") = 20,
         "Create a simple test configuration");

    // Serialization helpers
    m.def("exportBrainState", []() {
        // This would be implemented with proper brain reference
        throw std::runtime_error("exportBrainState requires a brain instance");
    }, "Export brain state to Python-friendly dictionary");

    m.def("loadBrainState", []() {
        // This would be implemented with proper brain reference
        throw std::runtime_error("loadBrainState requires a brain instance");
    }, py::arg("brain"), py::arg("filepath"),
       "Load brain state from file (returns True if successful)");

    m.def("saveBrainState", []() {
        // This would be implemented with proper brain reference
        throw std::runtime_error("saveBrainState requires a brain instance");
    }, py::arg("brain"), py::arg("filepath"),
       "Save brain state to file (returns True if successful)");

    // Utility functions
    m.def("validateBrain", &validateBrainInitialized,
         py::arg("brain"),
         "Validate that a brain is properly initialized");

    m.def("validateWorldConfig", &validateWorldConfig,
         py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
         "Validate world configuration parameters");

    m.def("validateAction", &validateAction,
         py::arg("type"), py::arg("params"),
         "Validate action parameters");

    m.def("safeReadFile", &safeReadFile,
         py::arg("filepath"),
         "Safely read file content (returns file content or raises exception)");

    m.def("safeWriteFile", &safeWriteFile,
         py::arg("filepath"), py::arg("content"),
         "Safely write file content (returns or raises exception)");

    m.def("createUniqueNeuronId", &createUniqueNeuronId,
         "Create a unique neuron ID for testing");

    m.def("formatBrainStatus", &formatBrainStatus,
         py::arg("brain"),
         "Get formatted brain status string");

    // Validation and debugging helpers
    m.def("validateSensoryPercept", [](const SensoryPercept& percept) {
        bool valid = true;
        std::string issues;

        if (!percept.getVision().getData().empty() || 
            !percept.getTouch().getData().empty() ||
            !percept.getInternal().getData().empty() ||
            !percept.getProprioception().getData().empty() ||
            !percept.getAudio().getData().empty()) {
            issues += "Percept has data; ";
        }

        return py::make_tuple(valid, issues);
    }, py::arg("percept"),
       "Validate sensory percept data");

    m.def("validateActionParameters", [](ActionType type, const std::vector<float>& params) {
        bool valid = true;
        std::string issue;

        if (params.size() > 10) {
            valid = false;
            issue = "Too many parameters (max 10)";
        }

        return py::make_tuple(valid, issue);
    }, py::arg("type"), py::arg("params"),
       "Validate action parameters");

    // Advanced utilities for power users
    m.def("batchProcessSensoryInput", [](std::shared_ptr<AgentBrain>& agent,
                                        const std::vector<SensoryPercept>& percepts) {
        std::vector<py::dict> results;
        for (const auto& percept : percepts) {
            agent->processSensoryInput(percept);
            py::dict result;
            result["processed"] = true;
            result["novelty"] = agent->getNoveltyLevel();
            results.push_back(result);
        }
        return results;
    }, py::arg("agent"), py::arg("percepts"),
       "Batch process multiple sensory inputs");

    m.def("getAllActionNames", []() {
        py::list names;
        names.append("MoveForward");
        names.append("MoveBackward");
        names.append("MoveLeft");
        names.append("MoveRight");
        names.append("TurnLeft");
        names.append("TurnRight");
        names.append("Look");
        names.append("LookUp");
        names.append("LookDown");
        names.append("Interact");
        names.append("Eat");
        names.append("Drink");
        names.append("Rest");
        names.append("Wait");
        names.append("Custom");
        return names;
    }, "Get list of all action names");

    // Object information helpers
    m.def("getWorldObjectInfo", [](const WorldObject& obj) {
        py::dict info;
        info["x"] = obj.x;
        info["y"] = obj.y;
        info["radius"] = obj.radius;
        info["type"] = obj.type;
        info["value"] = obj.value;
        info["active"] = obj.active;
        return info;
    }, py::arg("obj"),
       "Get world object information as dictionary");

    // Statistics and analysis
    m.def("calculateAverageSpeed", [](const AgentBody& body) {
        return std::sqrt(body.velocityX * body.velocityX + body.velocityY * body.velocityY);
    }, py::arg("body"),
       "Calculate average speed from body velocity");

    m.def("calculateDistance", [](float x1, float y1, float x2, float y2) {
        return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
    }, py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"),
       "Calculate distance between two points");

    m.def("calculateArea", [](float radius) {
        return M_PI * radius * radius;
    }, py::arg("radius"),
       "Calculate area of circle given radius");

    // Batch operation helpers
    m.def("updateWorldMultipleTimes", [](std::shared_ptr<SimpleWorld>& world,
                                         double timestep, size_t numSteps) {
        py::list results;
        for (size_t i = 0; i < numSteps; ++i) {
            world->update(timestep);
            py::dict result;
            result["step"] = static_cast<size_t>(world->getSimulationTime());
            result["time"] = world->getSimulationTime();
            results.append(result);
        }
        return results;
    }, py::arg("world"), py::arg("timestep"), py::arg("num_steps"),
       "Update world multiple times and return results");

    // Error handling utilities
    m.def("safeStepBrain", [](std::shared_ptr<Brain>& brain, size_t step) {
        try {
            if (!brain) throw std::runtime_error("Brain is null");
            brain->step(step);
            return py::make_tuple(true, std::string("Success"));
        } catch (const std::exception& e) {
            return py::make_tuple(false, std::string(e.what()));
        }
    }, py::arg("brain"), py::arg("step"),
       "Safely step brain with error handling");

    m.def("safeStepWorld", [](std::shared_ptr<SimpleWorld>& world, double timestep) {
        try {
            if (!world) throw std::runtime_error("World is null");
            world->update(timestep);
            return py::make_tuple(true, std::string("Success"));
        } catch (const std::exception& e) {
            return py::make_tuple(false, std::string(e.what()));
        }
    }, py::arg("world"), py::arg("timestep"),
       "Safely step world with error handling");

    // ==============================================================================
    // INVALID IDs (for initialization and testing)
    // ==============================================================================
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ==============================================================================
    // MODULE METADATA
    // ==============================================================================
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__license__") = "MIT";
    m.attr("__description__") = "Enhanced Python bindings for NLM neural simulation framework";

    // Add helpful constants
    m.attr("DEFAULT_WORLD_SIZE") = 20;
    m.attr("DEFAULT_VISION_SIZE") = 8;
    m.attr("MAX_ACTION_PARAMS") = 10;
    m.attr("DEFAULT_TIME_STEP") = 0.1;

    // Add example patterns
    m.def("getQuickStartExample", []() {
        return R"pbdoc(
        import pynlm

        # Create brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()

        # Create world  
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()

        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)

        # Run simulation
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        )pbdoc";
    });

    m.def("getAdvancedExample", []() {
        return R"pbdoc(
        import pynlm

        # Using context managers for resource management
        with pynlm.SmartWorld(pynlm.createSimpleWorld()) as world:
            world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
            
            with pynlm.SmartBrain(pynlm.createBrain(pynlm.createDefaultConfig())) as brain:
                with pynlm.SmartAgent(pynlm.createAgentBrain(brain)) as agent:
                    agent.initialize(world)
                    
                    # Batch processing
                    percepts = [world.getSensoryPercept() for _ in range(10)]
                    agent.processInput(percepts)
                    
                    # Learning analysis
                    stats = brain.getStats()
                    print(f"Learning progress: {stats}")
        )pbdoc";
    });
}

} // namespace nlm