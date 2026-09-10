#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "SystemIntegrationResult.hpp"
#include "VerificationFunctions.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
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
             "Log brain status");

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

    py::class_<SelfModel>(m, "SelfModel", R"pbdoc(Self model: represents the agent's internal model of itself)pbdoc")
        .def(py::init<>())
        .def("initialize", &SelfModel::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("recordSelfAction", &SelfModel::recordSelfAction, py::arg("action"), py::arg("beforeState"), py::arg("afterState"), "Record that taking an action caused a specific sensory change")
        .def("predictActionConsequence", &SelfModel::predictActionConsequence, py::arg("action"), py::arg("currentState"), "Predict sensory consequence of an action")
        .def("getSelfModelConfidence", &SelfModel::getSelfModelConfidence, py::arg("action"), "Get confidence in self-model for a given action")
        .def("computeSelfGeneratedLikeness", &SelfModel::computeSelfGeneratedLikeness, py::arg("beforeState"), py::arg("afterState"), py::arg("action"), "Is this change likely caused by self?")
        .def("getPreferredAction", &SelfModel::getPreferredAction, py::arg("state"), "Get the body schema (preferred actions)")
        .def("updateSelfModel", &SelfModel::updateSelfModel, py::arg("predicted"), py::arg("actual"), py::arg("action"), "Update self-model based on prediction error")
        .def("getCapabilityLevel", &SelfModel::getCapabilityLevel, "Get current capability level")
        .def("getBodyAwareness", &SelfModel::getBodyAwareness, "Get body awareness")
        .def("clear", &SelfModel::clear, "Clear self-model")
        .def("hasSelfModel", &SelfModel::hasSelfModel, "Has self-model formed?");

    py::class_<SocialLearning>(m, "SocialLearning", R"pbdoc(Social learning: enables learning from observing other agents)pbdoc")
        .def(py::init<>())
        .def("initialize", &SocialLearning::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("observeAgentAction", &SocialLearning::observeAgentAction, py::arg("observedAction"), py::arg("observerState"), py::arg("resultingState"), "Record observation of another agent's action")
        .def("canImitate", &SocialLearning::canImitate, py::arg("observedAction"), "Can I imitate this observed action?")
        .def("getImitationAction", &SocialLearning::getImitationAction, py::arg("currentState"), "Get the best action to imitate")
        .def("learnCommunicationSignal", &SocialLearning::learnCommunicationSignal, py::arg("signalPattern"), py::arg("signalReward"), "Learn simple communication signal")
        .def("detectSignal", &SocialLearning::detectSignal, py::arg("neuralPattern"), "Detect if another agent is signaling")
        .def("getSignalPattern", &SocialLearning::getSignalPattern, "Get learned signal pattern")
        .def("getSignalMeaning", &SocialLearning::getSignalMeaning, "Get signal meaning")
        .def("updateSocialKnowledge", &SocialLearning::updateSocialKnowledge, py::arg("interactionReward"), "Update social knowledge")
        .def("clear", &SocialLearning::clear, "Clear social learning")
        .def("hasSocialKnowledge", &SocialLearning::hasSocialKnowledge, "Has learned from others?")
        .def("getObservationCount", &SocialLearning::getObservationCount, "Get observation count");

    py::class_<SpatialRepresentation>(m, "SpatialRepresentation", R"pbdoc(Spatial representation: learns spatial relationships from experience)pbdoc")
        .def(py::init<>())
        .def("initialize", &SpatialRepresentation::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("recordPosition", &SpatialRepresentation::recordPosition, py::arg("x"), py::arg("y"), py::arg("sensoryCues"), "Record current position experience")
        .def("getPredictedPosition", &SpatialRepresentation::getPredictedPosition, "Get predicted position based on path integration")
        .def("integrateMovement", &SpatialRepresentation::integrateMovement, py::arg("dx"), py::arg("dy"), "Update position estimate based on movement")
        .def("getPositionActivation", &SpatialRepresentation::getPositionActivation, py::arg("x"), py::arg("y"), "Get position neuron activation")
        .def("getPlaceNeuronsNear", &SpatialRepresentation::getPlaceNeuronsNear, py::arg("x"), py::arg("y"), py::arg("radius"), "Find place neurons near a position")
        .def("getGridSpacing", &SpatialRepresentation::getGridSpacing, "Get grid spacing")
        .def("clear", &SpatialRepresentation::clear, "Clear spatial representations")
        .def("hasPlaceCells", &SpatialRepresentation::hasPlaceCells, "Have place cells formed?");

    // Performance Infrastructure
    py::class_<CheckpointSystem>(m, "CheckpointSystem", R"pbdoc(Checkpoint system: brain state serialization and checkpointing)pbdoc")
        .def(py::init<>())
        .def("save", &CheckpointSystem::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &CheckpointSystem::load, py::arg("filepath"), "Load brain state from file");

    // MemoryPool is a template class, we need to expose a specific type
    m.def("createMemoryPool", []() -> std::shared_ptr<MemoryPool<NeuronId>> {
        return std::make_shared<MemoryPool<NeuronId>>();
    }, "Create a memory pool for NeuronId");

    m.def("createMemoryPoolSynapse", []() -> std::shared_ptr<MemoryPool<SynapseId>> {
        return std::make_shared<MemoryPool<SynapseId>>();
    }, "Create a memory pool for SynapseId");

    py::class_<EventQueue>(m, "EventQueue", R"pbdoc(Event queue: lock-free event queue for spike processing)pbdoc")
        .def(py::init<>())
        .def("push", &EventQueue::push, py::arg("event"), "Push an event to the queue")
        .def("pop", &EventQueue::pop, "Pop an event from the queue")
        .def("size", &EventQueue::size, "Get queue size")
        .def("empty", &EventQueue::empty, "Check if queue is empty")
        .def("clear", &EventQueue::clear, "Clear all events");

    py::class_<SparseConnectivity>(m, "SparseConnectivity", R"pbdoc(Sparse connectivity: memory-efficient neural connectivity)pbdoc")
        .def(py::init<>())
        .def("addSynapse", &SparseConnectivity::addSynapse, py::arg("source"), py::arg("dest"), py::arg("weight"), py::arg("delay"), py::arg("synapseType"), "Add a synapse")
        .def("removeSynapse", &SparseConnectivity::removeSynapse, py::arg("index"), "Remove a synapse")
        .def("getSynapse", &SparseConnectivity::getSynapse, py::arg("index"), "Get synapse by index")
        .def("getSynapsesFrom", &SparseConnectivity::getSynapsesFrom, py::arg("source"), "Get synapses from source neuron")
        .def("getSynapsesTo", &SparseConnectivity::getSynapsesTo, py::arg("dest"), "Get synapses to destination neuron")
        .def("getSynapseCountFrom", &SparseConnectivity::getSynapseCountFrom, py::arg("source"), "Get synapse count from source")
        .def("getSynapseCountTo", &SparseConnectivity::getSynapseCountTo, py::arg("dest"), "Get synapse count to destination")
        .def("getSynapseCount", &SparseConnectivity::getSynapseCount, "Get total synapse count")
        .def("getNeuronCount", &SparseConnectivity::getNeuronCount, "Get neuron count")
        .def("getSynapsesWithDelay", &SparseConnectivity::getSynapsesWithDelay, py::arg("maxDelay"), "Get synapses with delay <= max")
        .def("getStats", &SparseConnectivity::getStats, "Get connectivity statistics")
        .def("clear", &SparseConnectivity::clear, "Clear all synapses")
        .def("memoryUsage", &SparseConnectivity::memoryUsage, "Get memory usage")
        .def("forEachSynapse", &SparseConnectivity::forEachSynapse, "Iterate over all synapses");

    py::class_<SIMDVectorization>(m, "SIMDVectorization", R"pbdoc(SIMD vectorization: SIMD operations for neural computation)pbdoc")
        .def(py::init<>())
        .def("isSIMDAvailable", &SIMDVectorization::isSIMDAvailable, "Check if SIMD is available")
        .def("getSIMDType", &SIMDVectorization::getSIMDType, "Get SIMD type name")
        .def("getSIMDWidth", &SIMDVectorization::getSIMDWidth, "Get SIMD width")
        .def("updateLIF", &SIMDVectorization::updateLIF, "Update membrane potentials using SIMD")
        .def("updateSTD", &SIMDVectorization::updateSTD, "Update short-term plasticity using SIMD")
        .def("updateSTDP", &SIMDVectorization::updateSTDP, "Update STDP using SIMD")
        .def("computeStats", &SIMDVectorization::computeStats, "Compute statistics using SIMD")
        .def("clamp", &SIMDVectorization::clamp, "Clamp values using SIMD");

    // Comprehensive system integration verification
    py::class_<SystemIntegrationResult>(m, "SystemIntegrationResult", R"pbdoc(System integration verification result)pbdoc")
        .def_readwrite("overallStatus", &SystemIntegrationResult::overallStatus)
        .def_readwrite("memorySystemStatus", &SystemIntegrationResult::memorySystemStatus)
        .def_readwrite("cognitiveSystemStatus", &SystemIntegrationResult::cognitiveSystemStatus)
        .def_readwrite("neuromodulationSystemStatus", &SystemIntegrationResult::neuromodulationSystemStatus)
        .def_readwrite("performanceInfrastructureStatus", &SystemIntegrationResult::performanceInfrastructureStatus)
        .def_readwrite("sleepSystemStatus", &SystemIntegrationResult::sleepSystemStatus)
        .def_readwrite("developmentSystemStatus", &SystemIntegrationResult::developmentSystemStatus)
        .def_readwrite("errors", &SystemIntegrationResult::errors)
        .def_readwrite("warnings", &SystemIntegrationResult::warnings)
        .def_readwrite("metrics", &SystemIntegrationResult::metrics)
        .def("toDict", [](const SystemIntegrationResult& self) {
            py::dict result;
            result["overallStatus"] = self.overallStatus;
            result["memorySystemStatus"] = self.memorySystemStatus;
            result["cognitiveSystemStatus"] = self.cognitiveSystemStatus;
            result["neuromodulationSystemStatus"] = self.neuromodulationSystemStatus;
            result["performanceInfrastructureStatus"] = self.performanceInfrastructureStatus;
            result["sleepSystemStatus"] = self.sleepSystemStatus;
            result["developmentSystemStatus"] = self.developmentSystemStatus;
            result["errors"] = self.errors;
            result["warnings"] = self.warnings;
            return result;
        }, R"pbdoc(Convert integration result to Python dictionary)pbdoc");

    m.def("verifyMemorySystem", &verifyMemorySystem,
          py::arg("brain"), py::arg("detailed"), py::arg("includeReplayTests"),
          "Verify memory system integration and functionality");

    m.def("verifyCognitiveIntegration", &verifyCognitiveIntegration,
          py::arg("brain"), py::arg("detailed"), py::arg("includePlanningTests"),
          "Verify cognitive system integration and functionality");

    m.def("verifyNeuromodulationIntegration", &verifyNeuromodulationIntegration,
          py::arg("brain"), py::arg("detailed"), py::arg("includeLearningTests"),
          "Verify neuromodulation system integration and functionality");

    m.def("verifyPerformanceInfrastructure", &verifyPerformanceInfrastructure,
          py::arg("brain"), py::arg("detailed"),
          "Verify performance infrastructure integration and functionality");

    m.def("verifySleepRestCycle", &verifySleepRestCycle,
          py::arg("brain"), py::arg("detailed"), py::arg("includeConsolidationTests"),
          "Verify sleep/rest cycle integration and functionality");

    m.def("verifyDevelopment", &verifyDevelopment,
          py::arg("brain"), py::arg("detailed"),
          "Verify development system integration and functionality");

    m.def("verifyCompleteSystemIntegration", &verifyCompleteSystemIntegration,
          py::arg("brain"), py::arg("detailed"),
          "Perform complete system integration verification and return comprehensive status report");

    m.def("testSystemIntegration", &testSystemIntegration,
          py::arg("brain"), py::arg("testSuite"),
          "Run specific integration test suite");

 } // namespace nlm
