#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

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

// Phase 1: Basic Enums and Types
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        An enhanced Python binding for the NLM C++ neural simulation framework.
        
        NEW ADVANCED FEATURES:
        - Enhanced AgentBrain with advanced neuromodulation and learning
        - Personalized development based on agent's unique characteristics  
        - Environmental adaptation and social learning capabilities
        - Emotional intelligence and moral development systems
        - Consciousness and self-awareness metrics
        - Autonomy development with goal setting and self-regulation
        - Advanced curiosity and exploration mechanisms
        - Reward prediction and adaptive learning
        - Personality and behavioral modeling
        - Memory optimization and consolidation
        - Social interaction and theory of mind capabilities
        
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, 
        SensoryInput, and Action with advanced functionality.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // === Basic Types ===
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

    // === Neuron and Synapse Types ===
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

    // === Configuration and Core Classes ===
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

    // === Sensory and Motor Systems ===
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

    // === Action and World Objects ===
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

    // === Agent Components ===
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

    // === Advanced AgentBrain Class ===
    py::enum_<AgentBrain::DevelopmentStyle>(m, "DevelopmentStyle", R"pbdoc(Development style enumeration)pbdoc")
        .value("Initial", AgentBrain::DevelopmentStyle::Initial)
        .value("CriticalPeriod", AgentBrain::DevelopmentStyle::CriticalPeriod)
        .value("Maturation", AgentBrain::DevelopmentStyle::Maturation)
        .value("Adult", AgentBrain::DevelopmentStyle::Adult)
        .value("Aging", AgentBrain::DevelopmentStyle::Aging)
        .export_values();

    py::enum_<AgentBrain::LearningStyle>(m, "LearningStyle", R"pbdoc(Learning style enumeration)pbdoc")
        .value("Exploratory", AgentBrain::LearningStyle::Exploratory)
        .value("TrialAndError", AgentBrain::LearningStyle::TrialAndError)
        .value("Balanced", AgentBrain::LearningStyle::Balanced)
        .value("Reflective", AgentBrain::LearningStyle::Reflective)
        .export_values();

    // === Enhanced AgentBrain with Advanced Features ===
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Advanced agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
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
        
        // === Advanced Features ===
        .def("enableAdvancedFeatures", &AgentBrain::enableAdvancedFeatures,
             "Enable advanced neuromodulation and learning features")
        .def("enableAdvancedLearning", &AgentBrain::enableAdvancedLearning,
             "Enable advanced learning mechanisms")
        .def("enablePersonalizedDevelopment", &AgentBrain::enablePersonalizedDevelopment,
             "Enable personalized development based on agent's unique characteristics")
        .def("enableEnvironmentalAdaptation", &AgentBrain::enableEnvironmentalAdaptation,
             "Enable adaptation to environmental conditions")
        .def("enableSocialLearning", &AgentBrain::enableSocialLearning,
             "Enable social learning capabilities")
        .def("enableEmotionalIntelligence", &AgentBrain::enableEmotionalIntelligence,
             "Enable emotional intelligence capabilities")
        .def("enableMoralDevelopment", &AgentBrain::enableMoralDevelopment,
             "Enable moral development capabilities")
        .def("enableConsciousnessDevelopment", &AgentBrain::enableConsciousnessDevelopment,
             "Enable consciousness development capabilities")
        .def("enableAutonomyDevelopment", &AgentBrain::enableAutonomyDevelopment,
             "Enable autonomy development capabilities")
        
        // === Configuration Setters ===
        .def("setExplorationRate", &AgentBrain::setExplorationRate,
             py::arg("rate"), "Set exploration rate")
        .def("setRewardPredictionHorizon", &AgentBrain::setRewardPredictionHorizon,
             py::arg("horizon"), "Set reward prediction horizon")
        .def("setDevelopmentStyle", &AgentBrain::setDevelopmentStyle,
             py::arg("style"), "Set development style")
        .def("setLearningStyle", &AgentBrain::setLearningStyle,
             py::arg("style"), "Set learning style")
        
        // === State Management ===
        .def("recordReward", &AgentBrain::recordReward,
             py::arg("reward"), "Record reward for adaptation calculations")
        .def("recordAction", &AgentBrain::recordAction,
             py::arg("action"), "Record action for learning and adaptation")
        .def("recordLearningOutcome", &AgentBrain::recordLearningOutcome,
             py::arg("success"), py::arg("reward"), "Record learning outcomes")
        .def("recordSocialInteraction", &AgentBrain::recordSocialInteraction,
             py::arg("interactionType"), py::arg("success"), py::arg("outcomeValue"), "Record social interaction")
        .def("recordEnvironmentalCondition", &AgentBrain::recordEnvironmentalCondition,
             py::arg("temperature"), py::arg("humidity"), py::arg("lightLevel"), "Record environmental conditions")
        .def("recordMoralDecision", &AgentBrain::recordMoralDecision,
             py::arg("decision"), py::arg("reasoning"), py::arg("ethical"), "Record moral decision")
        
        // === Goal Management ===
        .def("setGoals", &AgentBrain::setGoals,
             py::arg("goals"), "Set goals for autonomy development")
        .def("updateGoalProgress", &AgentBrain::updateGoalProgress,
             py::arg("goal"), py::arg("progress"), "Update progress on specific goals")
        
        // === State Queries ===
        .def("predictNextReward", &AgentBrain::predictNextReward,
             "Calculate predicted next reward based on recent rewards")
        .def("calculateLearningEfficiency", &AgentBrain::calculateLearningEfficiency,
             "Calculate learning efficiency based on success rates and rewards")
        .def("calculateActionNovelty", &AgentBrain::calculateActionNovelty,
             py::arg("currentAction"), "Calculate novelty of current action")
        .def("calculateEmotionalStability", &AgentBrain::calculateEmotionalStability,
             "Calculate emotional stability based on emotion history")
        .def("calculateEthicalConsistency", &AgentBrain::calculateEthicalConsistency,
             "Calculate ethical consistency over time")
        .def("calculateSocialSuccessRate", &AgentBrain::calculateSocialSuccessRate,
             "Calculate social success rate")
        .def("calculateSensoryAwareness", &AgentBrain::calculateSensoryAwareness,
             "Calculate sensory awareness based on input processing")
        .def("calculateMemoryAwareness", &AgentBrain::calculateMemoryAwareness,
             "Calculate memory awareness based on memory system usage")
        .def("calculateSocialAwareness", &AgentBrain::calculateSocialAwareness,
             "Calculate social awareness based on social interactions")
        .def("calculateSuccessRate", &AgentBrain::calculateSuccessRate,
             "Calculate success rate based on learning metrics")
        .def("calculateRewardConsistency", &AgentBrain::calculateRewardConsistency,
             "Calculate reward consistency over time")
        .def("calculatePlasticityVariance", &AgentBrain::calculatePlasticityVariance,
             "Calculate variance in plasticity modifier over time")
        .def("calculateAdaptation", &AgentBrain::calculateAdaptation,
             "Calculate adaptation factor based on recent performance")
        .def("calculateNoveltyLevel", &AgentBrain::calculateNoveltyLevel,
             "Calculate novelty based on sensory input changes")
        .def("calculateSynapticActivityLevel", &AgentBrain::calculateSynapticActivityLevel,
             "Calculate overall synaptic activity level")
        .def("calculateSynaptogenesisRate", &AgentBrain::calculateSynaptogenesisRate,
             "Calculate synaptogenesis rate based on developmental stage")
        .def("calculatePruningRate", &AgentBrain::calculatePruningRate,
             "Calculate pruning rate based on developmental stage and synaptic activity")
        .def("calculateActionVariety", &AgentBrain::calculateActionVariety,
             "Calculate variety of actions taken")
        .def("calculateEnvironmentalStress", &AgentBrain::calculateEnvironmentalStress,
             "Calculate environmental stress level")
        .def("calculateAutonomyLevel", &AgentBrain::calculateAutonomyLevel,
             "Calculate overall autonomy level")
        
        // === Emotional State Management ===
        .def("setEmotionalState", &AgentBrain::setEmotionalState,
             py::arg("fear"), py::arg("pleasure"), py::arg("surprise"), py::arg("anger"),
             "Set emotional state")
        
        // === Environmental Adaptation ===
        .def("adaptToEnvironment", &AgentBrain::adaptToEnvironment,
             "Adapt behavior based on environmental conditions")
        
        // === Social Learning ===
        .def("applySocialLearning", &AgentBrain::applySocialLearning,
             "Apply social learning principles to behavior")
        
        // === Emotional Intelligence ===
        .def("applyEmotionalStateToBehavior", &AgentBrain::applyEmotionalStateToBehavior,
             "Apply emotional state to influence behavior")
        
        // === Moral Development ===
        .def("applyMoralDevelopment", &AgentBrain::applyMoralDevelopment,
             "Apply moral development principles to behavior")
        
        // === Consciousness Development ===
        .def("updateAwarenessLevel", &AgentBrain::updateAwarenessLevel,
             "Update awareness level based on experience")
        .def("updateSelfAwareness", &AgentBrain::updateSelfAwareness,
             "Update self-awareness based on reflection and meta-cognition")
        .def("updateMetacognitiveAbility", &AgentBrain::updateMetacognitiveAbility,
             "Update metacognitive ability (thinking about thinking)")
        .def("updateConsciousnessMetrics", &AgentBrain::updateConsciousnessMetrics,
             "Update all consciousness-related metrics")
        
        // === Autonomy Development ===
        .def("applyAutonomyDevelopment", &AgentBrain::applyAutonomyDevelopment,
             "Apply autonomy development principles to behavior")
        
        // === Additional State Queries ===
        .def("getPersonalityProfile", &AgentBrain::getPersonalityProfile,
             "Get personality profile")
        .def("getAdaptabilityLevel", &AgentBrain::getAdaptabilityLevel,
             "Get adaptability level")
        .def("getInnovationRate", &AgentBrain::getInnovationRate,
             "Get innovation rate")
        .def("getConformityLevel", &AgentBrain::getConformityLevel,
             "Get conformity level")
        .def("getExplorationRate", &AgentBrain::getExplorationRate,
             "Get current exploration rate")
        .def("getDevelopmentStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("getLearningEfficiency", &AgentBrain::calculateLearningEfficiency,
             "Get learning efficiency")
        .def("getEmotionalStability", &AgentBrain::calculateEmotionalStability,
             "Get emotional stability")
        .def("getEthicalConsistency", &AgentBrain::calculateEthicalConsistency,
             "Get ethical consistency")
        .def("getSocialSuccessRate", &AgentBrain::calculateSocialSuccessRate,
             "Get social success rate")
        .def("getAwarenessLevel", &AgentBrain::calculateSensoryAwareness,
             "Get awareness level")
        .def("getSelfAwareness", &AgentBrain::updateSelfAwareness,
             "Get self-awareness")
        .def("getMetacognitiveAbility", &AgentBrain::updateMetacognitiveAbility,
             "Get metacognitive ability")
        .def("getAutonomyLevel", &AgentBrain::calculateAutonomyLevel,
             "Get autonomy level")
        .def("getActionSequenceLength", &AgentBrain::getActionSequenceLength,
             "Get action sequence length")
        .def("getRewardPredictionHorizon", &AgentBrain::getRewardPredictionHorizon,
             "Get reward prediction horizon")
        .def("getDevelopmentalAge", &AgentBrain::getDevelopmentalAge,
             "Get developmental age")
        .def("getPlasticityModifier", &AgentBrain::getPlasticityModifier,
             "Get plasticity modifier")
        .def("getExplorationRate", &AgentBrain::getExplorationRate,
             "Get exploration rate")
        .def("getAdaptationFactor", &AgentBrain::getAdaptationFactor,
             "Get adaptation factor")
        .def("getHabituationThreshold", &AgentBrain::getHabituationThreshold,
             "Get habituation threshold")
        .def("getAdvancedFeaturesEnabled", &AgentBrain::isAdvancedFeaturesEnabled,
             "Check if advanced features are enabled")
        .def("getPersonalizedDevelopmentEnabled", &AgentBrain::isPersonalizedDevelopmentEnabled,
             "Check if personalized development is enabled")
        .def("getEnvironmentalAdaptationEnabled", &AgentBrain::isEnvironmentalAdaptationEnabled,
             "Check if environmental adaptation is enabled")
        .def("getSocialLearningEnabled", &AgentBrain::isSocialLearningEnabled,
             "Check if social learning is enabled")
        .def("getEmotionalIntelligenceEnabled", &AgentBrain::isEmotionalIntelligenceEnabled,
             "Check if emotional intelligence is enabled")
        .def("getMoralDevelopmentEnabled", &AgentBrain::isMoralDevelopmentEnabled,
             "Check if moral development is enabled")
        .def("getConsciousnessDevelopmentEnabled", &AgentBrain::isConsciousnessDevelopmentEnabled,
             "Check if consciousness development is enabled")
        .def("getAutonomyDevelopmentEnabled", &AgentBrain::isAutonomyDevelopmentEnabled,
             "Check if autonomy development is enabled")
        
        // === Helper Methods ===
        .def("getTotalNeuronCount", &AgentBrain::getTotalNeuronCount,
             "Get total neuron count")
        .def("getTotalSynapseCount", &AgentBrain::getTotalSynapseCount,
             "Get total synapse count")
        .def("getActiveNeuronCount", &AgentBrain::getActiveNeuronCount,
             "Get active neuron count")
        .def("getFiringNeuronCount", &AgentBrain::getFiringNeuronCount,
             "Get firing neuron count")
        .def("getAverageFiringRate", &AgentBrain::getAverageFiringRate,
             "Get average firing rate")
        .def("getExcitationInhibitionRatio", &AgentBrain::getExcitationInhibitionRatio,
             "Get excitation/inhibition ratio")
        .def("getTotalSpikeCount", &AgentBrain::getTotalSpikeCount,
             "Get total spike count")
        .def("getPendingSpikeEventCount", &AgentBrain::getPendingSpikeEventCount,
             "Get pending spike event count")
        .def("getWorkingMemoryCapacity", &AgentBrain::getWorkingMemoryCapacity,
             "Get working memory capacity")
        .def("getEpisodicMemoryCapacity", &AgentBrain::getEpisodicMemoryCapacity,
             "Get episodic memory capacity")
        .def("getMemoryConsolidationRate", &AgentBrain::getMemoryConsolidationRate,
             "Get memory consolidation rate")
        .def("getLearningRate", &AgentBrain::getLearningRate,
             "Get learning rate")
        .def("getRetentionRate", &AgentBrain::getRetentionRate,
             "Get retention rate")
        .def("getEmpathyLevel", &AgentBrain::getEmpathyLevel,
             "Get empathy level")
        .def("getTheoryOfMindLevel", &AgentBrain::getTheoryOfMindLevel,
             "Get theory of mind level")
        .def("getEmotionalIntelligenceLevel", &AgentBrain::getEmotionalIntelligenceLevel,
             "Get emotional intelligence level")
        .def("getMoralReasoningLevel", &AgentBrain::getMoralReasoningLevel,
             "Get moral reasoning level")
        .def("getAdaptabilityLevel", &AgentBrain::getAdaptabilityLevel,
             "Get adaptability level")
        .def("getInnovationRate", &AgentBrain::getInnovationRate,
             "Get innovation rate")
        .def("getConformityLevel", &AgentBrain::getConformityLevel,
             "Get conformity level")
        .def("getTemperatureSensitivity", &AgentBrain::getTemperatureSensitivity,
             "Get temperature sensitivity")
        .def("getLightSensitivity", &AgentBrain::getLightSensitivity,
             "Get light sensitivity")
        .def("getHumiditySensitivity", &AgentBrain::getHumiditySensitivity,
             "Get humidity sensitivity")
        .def("getSelfReflectionEnabled", &AgentBrain::isSelfReflectionEnabled,
             "Check if self-reflection is enabled")
        .def("getIntrospectionLevel", &AgentBrain::getIntrospectionLevel,
             "Get introspection level")
        .def("getMetaAwarenessLevel", &AgentBrain::getMetaAwarenessLevel,
             "Get meta-awareness level")
        .def("getPersonalityProfile", &AgentBrain::getPersonalityProfile,
             "Get personality profile")
        .def("getDevelopmentStyle", &AgentBrain::getDevelopmentStyle,
             "Get development style")
        .def("getLearningStyle", &AgentBrain::getLearningStyle,
             "Get learning style")
        
        // === State History ===
        .def("getActionHistory", &AgentBrain::getActionHistory,
             "Get action history")
        .def("getRewardHistory", &AgentBrain::getRewardHistory,
             "Get reward history")
        .def("getLearningHistory", &AgentBrain::getLearningHistory,
             "Get learning history")
        .def("getSocialInteractionHistory", &AgentBrain::getSocialInteractionHistory,
             "Get social interaction history")
        .def("getEnvironmentalHistory", &AgentBrain::getEnvironmentalHistory,
             "Get environmental history")
        .def("getEmotionHistory", &AgentBrain::getEmotionHistory,
             "Get emotion history")
        .def("getMoralDecisionHistory", &AgentBrain::getMoralDecisionHistory,
             "Get moral decision history")
        .def("getGoalHistory", &AgentBrain::getGoalHistory,
             "Get goal history")
        .def("getConsciousnessHistory", &AgentBrain::getConsciousnessHistory,
             "Get consciousness history")
        .def("getAutonomyHistory", &AgentBrain::getAutonomyHistory,
             "Get autonomy history")
        
        // === Statistics ===
        .def("getStatistics", &AgentBrain::getStatistics,
             "Get comprehensive statistics")
        .def("getBrainStatus", &AgentBrain::getBrainStatus,
             "Get brain status")
        .def("getAdvancedStats", &AgentBrain::getAdvancedStats,
             "Get advanced statistics")
        .def("getDevelopmentStats", &AgentBrain::getDevelopmentStats,
             "Get development statistics")
        .def("getLearningStats", &AgentBrain::getLearningStats,
             "Get learning statistics")
        .def("getSocialStats", &AgentBrain::getSocialStats,
             "Get social statistics")
        .def("getEmotionalStats", &AgentBrain::getEmotionalStats,
             "Get emotional statistics")
        .def("getMoralStats", &AgentBrain::getMoralStats,
             "Get moral statistics")
        .def("getConsciousnessStats", &AgentBrain::getConsciousnessStats,
             "Get consciousness statistics")
        .def("getAutonomyStats", &AgentBrain::getAutonomyStats,
             "Get autonomy statistics")
        
        // === Reset ===
        .def("resetEnhanced", &AgentBrain::resetEnhanced,
             "Reset agent with enhanced features")
        .def("resetBasic", &AgentBrain::resetBasic,
             "Reset agent with basic features")
        .def("resetAll", &AgentBrain::resetAll,
             "Reset agent completely")
        
        // === Comparison Operators ===
        .def("__eq__", &AgentBrain::operator==,
             "Equality comparison")
        .def("__ne__", &AgentBrain::operator!=,
             "Inequality comparison")
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain: " + std::to_string(agent.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(agent.getCuriosityLevel()) + " curiosity, " +
                   std::to_string(agent.getEmotionalStability()) + " emotional stability, " +
                   std::to_string(agent.getLearningEfficiency()) + " learning efficiency>";
        });

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

    // Phase 1: Basic identity constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm

