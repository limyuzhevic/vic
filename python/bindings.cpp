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

namespace py = pybind11;
namespace nlm {

// Helper functions for parameter validation
inline void validatePositive(int value, const std::string& name) {
    if (value <= 0) {
        throw std::invalid_argument(name + " must be positive, got " + std::to_string(value));
    }
}

inline void validateNonNegative(float value, const std::string& name) {
    if (value < 0.0f) {
        throw std::invalid_argument(name + " must be non-negative, got " + std::to_string(value));
    }
}

// Convenience factory functions
inline std::shared_ptr<Config> createConfigWithValidation(const std::vector<std::string>& args) {
    auto config = std::make_shared<Config>();
    config->loadFromArgs(static_cast<int>(args.size()), args.data());
    return config;
}

inline std::shared_ptr<Brain> createBrainWithValidation(const std::shared_ptr<Config>& config) {
    if (!config) {
        throw std::invalid_argument("Config cannot be null");
    }
    return std::make_shared<Brain>(config);
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This API provides access to advanced neural simulation capabilities including:
        - Leaky Integrate-and-Fire (LIF) neurons
        - Event-driven spike propagation
        - STDP and Hebbian plasticity rules
        - Structural plasticity (synaptogenesis/pruning)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Memory systems (working, episodic, associative)
        - Prediction and cognitive systems
        - Developmental stages
        
        For detailed examples and tutorials, visit: https://github.com/nlm-project/nlm
    )pbdoc";

    // Error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // ============================================================================
    // CORE TYPES AND IDENTIFIERS
    // ============================================================================
    
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

    // ============================================================================
    // ENUMERATIONS
    // ============================================================================
    
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

    // ============================================================================
    // CONFIGURATION CLASS
    // ============================================================================
    
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", &createConfigWithValidation,
             py::arg("args"), "Load configuration from command line arguments")
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
        .def("set", [](Config& self, const std::string& key, const ConfigValue& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set a configuration value (generic)")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set an integer configuration value")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set a float/double configuration value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set a boolean configuration value")
        .def("get", &Config::template get<int>, py::arg("key"),
             "Get an integer configuration value")
        .def("get", &Config::template get<double>, py::arg("key"),
             "Get a float/double configuration value")
        .def("get", &Config::template get<bool>, py::arg("key"),
             "Get a boolean configuration value")
        .def("get", &Config::template get<std::string>, py::arg("key"),
             "Get a string configuration value")
        .def("getOr", &Config::template getOr<int>, py::arg("key"), py::arg("defaultValue"),
             "Get an integer configuration value with default")
        .def("getOr", &Config::template getOr<double>, py::arg("key"), py::arg("defaultValue"),
             "Get a float/double configuration value with default")
        .def("getOr", &Config::template getOr<bool>, py::arg("key"), py::arg("defaultValue"),
             "Get a boolean configuration value with default")
        .def("getOr", &Config::template getOr<std::string>, py::arg("key"), py::arg("defaultValue"),
             "Get a string configuration value with default");

    // ============================================================================
    // SENSORY INPUT CLASSES
    // ============================================================================
    
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

    // ============================================================================
    // ACTION CLASS
    // ============================================================================
    
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

    // ============================================================================
    // WORLD OBJECTS
    // ============================================================================
    
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

    // ============================================================================
    // AGENT BODY
    // ============================================================================
    
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

    // ============================================================================
    // ACTION RESULT
    // ============================================================================
    
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
              py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // ============================================================================
    // SENSORY PERCEPT
    // ============================================================================
    
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

    // ============================================================================
    // SIMPLE WORLD
    // ============================================================================
    
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        .def("getObjectAt", [](SimpleWorld& self, float x, float y) {
            WorldObject* obj = self.getObjectAt(x, y);
            return obj ? py::cast(*obj) : py::none();
        }, py::arg("x"), py::arg("y"), R"pbdoc(Get world object at position (returns None if no object)"pbdoc");

    // ============================================================================
    // AGENT BRAIN
    // ============================================================================
    
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
def("getDevelopmentalState", &AgentBrain::getDevelopmentalStage,
              "Get current developmental state")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("getMotorGroupActivities", &AgentBrain::getMotorGroupActivities,
              "Get motor group activity diagnostics")
        .def("getThoughtTrace", &AgentBrain::getThoughtTrace,
              "Get thought trace (Phase 5)")
        .def("getLearnedPreference", &AgentBrain::getLearnedPreference,
              "Get learned preference trait")
        .def("getReflexBonusCount", &AgentBrain::getReflexBonusCount,
              "Get vision-gated reflex bonus count")
        .def("getReflexSuppressCount", &AgentBrain::getReflexSuppressCount,
              "Get hazard suppression count")
        .def("getWtaPulseCount", &AgentBrain::getWtaPulseCount,
              "Get winner-take-all pulse count")
        .def("getPlannerOverrideCount", &AgentBrain::getPlannerOverrideCount,
              "Get planner override count")
        .def("getPlannerAgreementCount", &AgentBrain::getPlannerAgreementCount,
              "Get planner agreement count")
        .def("getPlannerDecisionCount", &AgentBrain::getPlannerDecisionCount,
              "Get planner decision count")
        .def("getConceptBiasCount", &AgentBrain::getConceptBiasCount,
              "Get concept bias count")
        .def("getSelfBiasCount", &AgentBrain::getSelfBiasCount,
              "Get self-bias count")
        .def("getSocialBiasCount", &AgentBrain::getSocialBiasCount,
              "Get social-bias count")
        .def("getConceptCount", &AgentBrain::getConceptCount,
              "Get concept count")
        .def("getSelfModelConfidence", &AgentBrain::getSelfModelConfidence,
              "Get self-model confidence")
        .def("getSocialObservationCount", &AgentBrain::getSocialObservationCount,
              "Get social observation count")
        .def("observeOtherAgent", &AgentBrain::observeOtherAgent,
              py::arg("observed"), py::arg("beforeVision"), py::arg("afterVision"),
              "Observe another agent for social learning")
        .def("getInternalFiringRates", &AgentBrain::getInternalFiringRates,
              "Get internal firing rates")
        .def_static("encodeText", &AgentBrain::encodeText,
              py::arg("text"), "Encode text into sparse neuron pattern");

    // ============================================================================
    // BRAIN CLASS
    // ============================================================================
    
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
              py::arg("id"),
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
        .def("getSpikeSystem", &Brain::getSpikeSystem,
              py::return_value_policy::reference_internal,
              "Get spike system")
        .def("getSTDP", &Brain::getSTDP,
              py::return_value_policy::reference_internal,
              "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
              py::return_value_policy::reference_internal,
              "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
              py::return_value_policy::reference_internal,
              "Get structural plasticity system")
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
              "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
              py::return_value_policy::reference_internal,
              "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
              py::return_value_policy::reference_internal,
              "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
              py::return_value_policy::reference_internal,
              "Get prediction error signal")
        .def("getConfig", &Brain::getConfig,
              py::return_value_policy::reference_internal,
              "Get brain configuration")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
              py::return_value_policy::reference_internal,
              "Get random generator")
        .def("getLearnedPreference", &Brain::getLearnedPreference,
              "Get learned preference trait")
        .def("setLearnedPreference", &Brain::setLearnedPreference,
              py::arg("preference"),
              "Set learned preference trait")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
              "Get total spike count")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
              "Get pending spike event count")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
              "Get active neuron count")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
              "Get firing neuron count")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
              "Get average firing rate")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
              "Get excitation/inhibition ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
              "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
              "Get developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
              py::arg("stage"), "Set developmental stage");

    // ============================================================================
    // FACTORY FUNCTIONS AND UTILITIES
    // ============================================================================
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible defaults for Phase 6")

    m.def("createBrain", &createBrainWithValidation,
          py::arg("config"), "Create a new brain with configuration (with validation)")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world for agent simulations")

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")

    m.def("version", []() {
        return "pynlm 0.1.0 (Phase 6)";
    }, "Get the version string")

    // ============================================================================
    // INVALID CONSTANTS
    // ============================================================================
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ============================================================================
    // ERROR CLASSES
    // ============================================================================
    
    py::class_<std::exception>(m, "Exception")
        .def(py::init<const char*>())  // Allow construction from C-string
        .def("what", &std::exception::what);

    // ============================================================================
    // ADD ERROR ALIASES
    // ============================================================================
    
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // ============================================================================
    // HELPER FUNCTIONS
    // ============================================================================
    
    // Additional validation functions
    m.def("validate_positive", &validatePositive,
          py::arg("value"), py::arg("name"),
          "Validate that a value is positive")

    m.def("validate_non_negative", &validateNonNegative,
          py::arg("value"), py::arg("name"),
          "Validate that a value is non-negative")

    // ============================================================================
    // QUICK START EXAMPLES (provided as reference)
    // ============================================================================
    
    m.def("example_minimal_brain", []() {
        py::module_ examples = py::module_::import("builtins");
        return R"python(
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)
print(f"Total neurons: {brain.getTotalNeuronCount()}")
"python";
    }, "Minimal brain example")

    m.def("example_agent_simulation", []() {
        return R"python(
import pynlm

# Create everything
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation complete!")
"python";
    }, "Complete agent simulation example")

    m.def("example_brain_configuration", []() {
        return R"python(
import pynlm

config = pynlm.createDefaultConfig()

# Configure specific parameters
config.set("brain.neuron_count", 500)
config.set("brain.connection_probability", 0.05)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.curiosity.enable", True)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain configured with {brain.getTotalNeuronCount()} neurons")
print(f"Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
"python";
    }, "Brain configuration example");

} // namespace nlm
