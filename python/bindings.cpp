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

    // ==============================================
    // ADVANCED FEATURE CREATORS (Phase 4+ COGNITION)
    // ==============================================

    m.def("createNeuralWorkingMemory", []() -> std::unique_ptr<NeuralWorkingMemory> {
        return std::make_unique<NeuralWorkingMemory>();
    }, "Create a neural working memory system");

    m.def("createPredictionSystem", []() -> std::unique_ptr<PredictionSystem> {
        return std::make_unique<PredictionSystem>();
    }, "Create a prediction system for temporal sequence learning");

    m.def("createNeuralPlanner", []() -> std::unique_ptr<NeuralPlanner> {
        return std::make_unique<NeuralPlanner>();
    }, "Create a neural planner for action selection");

    m.def("createConceptFormation", []() -> std::unique_ptr<ConceptFormation> {
        return std::make_unique<ConceptFormation>();
    }, "Create a concept formation system for pattern discovery");

    m.def("createAttentionalSelection", []() -> std::unique_ptr<AttentionalSelection> {
        return std::make_unique<AttentionalSelection>();
    }, "Create an attentional selection system for competitive selection");

    m.def("createAttentionalSelectionSystem", []() -> std::unique_ptr<AttentionalSelection> {
        return std::make_unique<AttentionalSelection>();
    }, "Create an attentional selection system (alternative name)");

    m.def("createEpisodicMemory", []() -> std::unique_ptr<NeuralEpisodicMemory> {
        return std::make_unique<NeuralEpisodicMemory>();
    }, "Create an episodic memory system for experience storage");

    m.def("createAssociativeMemory", []() -> std::unique_ptr<NeuralAssociativeMemory> {
        return std::make_unique<NeuralAssociativeMemory>();
    }, "Create an associative memory system for pattern associations");

    // ==============================================
    // CONFIGURE AND INITIALIZATION HELPERS
    // ==============================================

    m.def("configureBrainAdvanced", [](std::shared_ptr<Config> config, size_t neuronCount,
                                       size_t regionCount, float connectionProbability,
                                       const std::string& plasticityMode = "stdp",
                                       bool enableMemory = true,
                                       bool enablePrediction = true,
                                       bool enableCognition = true) {
        // Set basic parameters
        config->set("brain.neuron_count", neuronCount);
        config->set("brain.region_count", regionCount);
        config->set("brain.connection_probability", connectionProbability);
        
        // Configure plasticity systems
        if (plasticityMode == "stdp") {
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.001f);
        } else if (plasticityMode == "hebbian") {
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.hebbian.learning_rate", 0.01f);
        } else if (plasticityMode == "both") {
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.001f);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.hebbian.learning_rate", 0.01f);
        }
        
        // Configure memory systems
        if (enableMemory) {
            config->set("memory.working_memory.enabled", true);
            config->set("memory.episodic.enabled", true);
            config->set("memory.associative.enabled", true);
        }
        
        // Configure prediction and cognition
        if (enablePrediction) {
            config->set("prediction.enabled", true);
            config->set("prediction.horizon", 10);
        }
        
        if (enableCognition) {
            config->set("cognition.planning.enabled", true);
            config->set("cognition.attention.enabled", true);
            config->set("cognition.concept_formation.enabled", true);
        }
        
        return config;
    }, py::arg("config"), py::arg("neuronCount"), py::arg("regionCount"),
         py::arg("connectionProbability"), py::arg("plasticityMode") = "stdp",
         py::arg("enableMemory") = true,
         py::arg("enablePrediction") = true,
         py::arg("enableCognition") = true,
         "Advanced brain configuration helper function");

    m.def("configureAgentAdvanced", [](std::shared_ptr<AgentBrain> agent,
                                       bool enableRewardModulation = true,
                                       bool enableStructuralPlasticity = true,
                                       bool enableDevelopment = true,
                                       bool enableCuriosity = true) {
        agent->enableRewardModulation(enableRewardModulation);
        agent->enableStructuralPlasticity(enableStructuralPlasticity);
        agent->enableDevelopment(enableDevelopment);
        agent->enableCuriosity(enableCuriosity);
        return agent;
    }, py::arg("agent"), py::arg("enableRewardModulation") = true,
         py::arg("enableStructuralPlasticity") = true,
         py::arg("enableDevelopment") = true,
         py::arg("enableCuriosity") = true,
         "Configure agent brain with advanced features");

    // ==============================================
    // EXPERIMENT HELPERS
    // ==============================================

    m.def("runPhase3Experiment", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent,
                                   size_t duration = 1000) {
        brain->initialize();
        agent->initialize(*world);
        
        for (size_t step = 0; step < duration; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_tuple(brain->getTotalSpikeCount(),
                              brain->getAverageFiringRate(),
                              agent->getCuriosityLevel());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("duration") = 1000,
         "Run Phase 3 (World Interaction) experiment");

    m.def("runPhase4Experiment", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent,
                                   size_t duration = 1000) {
        brain->initialize();
        agent->initialize(*world);
        
        // Enable advanced cognitive features
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        for (size_t step = 0; step < duration; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply neuromodulation
            float reward = 0.1f;
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1);
        }
        
        return std::make_tuple(brain->getTotalSpikeCount(),
                              brain->getAverageFiringRate(),
                              agent->getCuriosityLevel(),
                              agent->getNoveltyLevel(),
                              agent->getPredictionError());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("duration") = 1000,
         "Run Phase 4 (Emerging Cognition) experiment with full cognitive stack");

    m.def("runCompleteAgentSimulation", [](std::shared_ptr<Brain> brain,
                                           std::shared_ptr<SimpleWorld> world,
                                           std::shared_ptr<AgentBrain> agent,
                                           size_t duration = 1000,
                                           const std::string& mode = "survival") {
        brain->initialize();
        agent->initialize(*world);
        
        // Enable all cognitive features based on mode
        if (mode == "survival") {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
            agent->enableStructuralPlasticity(true);
        } else if (mode == "exploration") {
            agent->enableCuriosity(true);
            agent->enableNoveltyDetection(true);
        } else if (mode == "social") {
            // Social learning would require additional components
            agent->enableRewardModulation(true);
            agent->enableDevelopment(true);
        } else {
            // Full mode
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
            agent->enableStructuralPlasticity(true);
        }
        
        for (size_t step = 0; step < duration; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Dynamic reward based on mode
            float reward = 0.0f;
            if (mode == "survival") {
                // Simple survival reward
                auto body = world->getAgentBody();
                reward = body.energy / 100.0f;
            } else if (mode == "exploration") {
                // Exploration reward for novelty
                reward = agent->getNoveltyLevel() * 0.1f;
            } else if (mode == "social") {
                // Social reward (placeholder - would need social features)
                reward = 0.05f;
            }
            
            // Apply neuromodulation
            if (agent->isRewardModulationEnabled()) {
                agent->applyRewardModulation(reward, 0.0f);
            }
            
            // Update development
            if (agent->isDevelopmentEnabled()) {
                agent->updateDevelopment(0.1);
            }
        }
        
        return std::make_tuple(brain->getTotalSpikeCount(),
                              brain->getAverageFiringRate(),
                              agent->getCuriosityLevel(),
                              agent->getNoveltyLevel(),
                              agent->getPredictionError(),
                              world->getAgentBody().energy);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("duration") = 1000,
         py::arg("mode") = "survival",
         "Run complete agent simulation with different behavioral modes");

    // ==============================================
    // ADVANCED COGNITIVE SYSTEM API FUNCTIONS
    // ==============================================

    m.def("createWorkingMemoryAdvanced", [](std::shared_ptr<Brain> brain, size_t capacity = 1000,
                                          float decayRate = 0.01f) {
        auto workingMemory = std::make_unique<NeuralWorkingMemory>();
        workingMemory->initialize(brain.get());
        workingMemory->setCapacity(capacity);
        workingMemory->setDecayRate(decayRate);
        return workingMemory;
    }, py::arg("brain"), py::arg("capacity") = 1000, py::arg("decayRate") = 0.01f,
         "Create and configure a neural working memory system");

    m.def("createEpisodicMemoryAdvanced", [](size_t maxEpisodes = 1000, float consolidationThreshold = 0.3f) {
        auto episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        episodicMemory->setMaxEpisodes(maxEpisodes);
        episodicMemory->setConsolidationThreshold(consolidationThreshold);
        return episodicMemory;
    }, py::arg("maxEpisodes") = 1000, py::arg("consolidationThreshold") = 0.3f,
         "Create and configure an episodic memory system");

    m.def("createPredictionSystemAdvanced", [](size_t horizon = 10, float predictionThreshold = 0.5f) {
        auto predictionSystem = std::make_unique<PredictionSystem>();
        // Configure prediction system parameters (if available)
        return predictionSystem;
    }, py::arg("horizon") = 10, py::arg("predictionThreshold") = 0.5f,
         "Create and configure a prediction system for temporal sequence learning");

    m.def("createNeuralPlannerAdvanced", [](std::shared_ptr<Brain> brain, size_t planningDepth = 5,
                                          float explorationRate = 0.1f) {
        auto planner = std::make_unique<NeuralPlanner>();
        planner->initialize(brain.get());
        planner->setPlanningDepth(planningDepth);
        planner->setExplorationRate(explorationRate);
        return planner;
    }, py::arg("brain"), py::arg("planningDepth") = 5, py::arg("explorationRate") = 0.1f,
         "Create and configure a neural planner for action planning");

    m.def("createConceptFormationAdvanced", [](std::shared_ptr<Brain> brain, size_t conceptCount = 50,
                                             float stabilityThreshold = 0.8f) {
        auto conceptFormation = std::make_unique<ConceptFormation>();
        conceptFormation->initialize(brain.get());
        conceptFormation->setConceptCount(conceptCount);
        conceptFormation->setStabilityThreshold(stabilityThreshold);
        return conceptFormation;
    }, py::arg("brain"), py::arg("conceptCount") = 50, py::arg("stabilityThreshold") = 0.8f,
         "Create and configure a concept formation system for pattern discovery");

    m.def("createAttentionalSelectionAdvanced", [](std::shared_ptr<Brain> brain,
                                                  float inhibitionStrength = 0.5f,
                                                  float excitationStrength = 1.5f) {
        auto attention = std::make_unique<AttentionalSelection>();
        attention->initialize(brain.get());
        attention->setInhibitionStrength(inhibitionStrength);
        attention->setExcitationStrength(excitationStrength);
        return attention;
    }, py::arg("brain"), py::arg("inhibitionStrength") = 0.5f,
         py::arg("excitationStrength") = 1.5f,
         "Create and configure an attentional selection system");

    m.def("createAssociativeMemoryAdvanced", [](std::shared_ptr<Brain> brain, size_t maxAssociations = 1000,
                                              float associationStrength = 0.1f) {
        auto associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        associativeMemory->initialize(brain.get());
        associativeMemory->setMaxAssociations(maxAssociations);
        associativeMemory->setAssociationStrength(associationStrength);
        return associativeMemory;
    }, py::arg("brain"), py::arg("maxAssociations") = 1000,
         py::arg("associationStrength") = 0.1f,
         "Create and configure an associative memory system");

    // ==============================================
    // ADVANCED BRAIN COGNITIVE CONFIGURATION
    // ==============================================

    m.def("configureBrainForCognition", [](std::shared_ptr<Config> config,
                                          bool enableWorkingMemory = true,
                                          bool enableEpisodicMemory = true,
                                          bool enableAssociativeMemory = true,
                                          bool enablePrediction = true,
                                          bool enableCognition = true,
                                          bool enableNeuromodulation = true,
                                          bool enableDevelopment = true) {
        // Configure cognitive systems
        if (enableWorkingMemory) {
            config->set("cognitive.working_memory.enabled", true);
            config->set("cognitive.working_memory.capacity", 1000);
        }
        
        if (enableEpisodicMemory) {
            config->set("cognitive.episodic_memory.enabled", true);
            config->set("cognitive.episodic_memory.max_episodes", 1000);
        }
        
        if (enableAssociativeMemory) {
            config->set("cognitive.associative_memory.enabled", true);
            config->set("cognitive.associative_memory.max_associations", 1000);
        }
        
        if (enablePrediction) {
            config->set("cognitive.prediction.enabled", true);
            config->set("cognitive.prediction.horizon", 10);
        }
        
        if (enableCognition) {
            config->set("cognitive.planning.enabled", true);
            config->set("cognitive.attention.enabled", true);
            config->set("cognitive.concept_formation.enabled", true);
        }
        
        if (enableNeuromodulation) {
            config->set("neuromodulation.enabled", true);
            config->set("neuromodulation.dopamine.scale", 1.0f);
            config->set("neuromodulation.curiosity.enabled", true);
            config->set("neuromodulation.novelty.enabled", true);
        }
        
        if (enableDevelopment) {
            config->set("development.enabled", true);
            config->set("development.stage_transition_rate", 0.01f);
        }
        
        return config;
    }, py::arg("config"), py::arg("enableWorkingMemory") = true,
         py::arg("enableEpisodicMemory") = true,
         py::arg("enableAssociativeMemory") = true,
         py::arg("enablePrediction") = true,
         py::arg("enableCognition") = true,
         py::arg("enableNeuromodulation") = true,
         py::arg("enableDevelopment") = true,
         "Configure brain for advanced cognitive capabilities");

    // ==============================================
    // ADVANCED SIMULATION HELPERS
    // ==============================================

    m.def("runPhase5Experiment", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent,
                                   size_t duration = 1000,
                                   const std::string& mode = "memory") {
        brain->initialize();
        agent->initialize(*world);
        
        // Enable advanced features based on mode
        if (mode == "memory") {
            agent->enableRewardModulation(true);
            agent->enableDevelopment(true);
            // Enable memory systems
        } else if (mode == "prediction") {
            agent->enableCuriosity(true);
            agent->enableNoveltyDetection(true);
            agent->enablePrediction(true);
        } else if (mode == "cognition") {
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
            agent->enableStructuralPlasticity(true);
            // Enable all cognitive systems
        }
        
        for (size_t step = 0; step < duration; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Mode-specific processing
            if (mode == "memory") {
                // Memory-based learning
                float reward = 0.05f;
                agent->applyRewardModulation(reward, 0.0f);
                agent->updateDevelopment(0.1);
            } else if (mode == "prediction") {
                // Prediction-based learning
                float novelty = agent->getNoveltyLevel();
                agent->applyRewardModulation(novelty * 0.1f, 0.0f);
            } else if (mode == "cognition") {
                // Full cognitive processing
                float curiosity = agent->getCuriosityLevel();
                float predictionError = agent->getPredictionError();
                agent->applyRewardModulation(curiosity + predictionError, 0.0f);
                agent->updateDevelopment(0.1);
            }
        }
        
        return std::make_tuple(brain->getTotalSpikeCount(),
                              brain->getAverageFiringRate(),
                              agent->getCuriosityLevel(),
                              agent->getNoveltyLevel(),
                              agent->getPredictionError());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("duration") = 1000,
         py::arg("mode") = "memory",
         "Run Phase 5 (Advanced Learning) experiment with different cognitive modes");

    m.def("runPhase6Experiment", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent,
                                   size_t duration = 1000,
                                   float learningRate = 0.01f,
                                   float explorationRate = 0.1f) {
        brain->initialize();
        agent->initialize(*world);
        
        // Enable all advanced cognitive features
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(true);
        
        // Configure advanced systems if accessible
        // (Note: Some systems may require direct brain access)
        
        for (size_t step = 0; step < duration; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Advanced learning with curiosity and prediction
            float curiosity = agent->getCuriosityLevel();
            float novelty = agent->getNoveltyLevel();
            float predictionError = agent->getPredictionError();
            
            // Adaptive reward based on learning signals
            float reward = (curiosity * learningRate) + (novelty * explorationRate) + predictionError;
            agent->applyRewardModulation(reward, 0.0f);
            
            // Continuous development
            agent->updateDevelopment(0.1);
        }
        
        return std::make_tuple(brain->getTotalSpikeCount(),
                              brain->getAverageFiringRate(),
                              agent->getCuriosityLevel(),
                              agent->getNoveltyLevel(),
                              agent->getPredictionError(),
                              brain->getDevelopmentalStage());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("duration") = 1000,
         py::arg("learningRate") = 0.01f, py::arg("explorationRate") = 0.1f,
         "Run Phase 6 (Advanced Integration) experiment with full cognitive integration");

    m.def("runHybridExperiment", [](std::shared_ptr<Brain> brain,
                                    std::shared_ptr<SimpleWorld> world,
                                    std::shared_ptr<AgentBrain> agent,
                                    size_t duration = 1000,
                                    const std::vector<std::string>& modes = {"memory", "prediction", "cognition"},
                                    float modeTransitionRate = 0.1f) {
        brain->initialize();
        agent->initialize(*world);
        
        // Enable all features
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(true);
        
        size_t modeStep = 0;
        for (size_t step = 0; step < duration; ++step) {
            // Cycle through modes
            std::string currentMode = modes[modeStep % modes.size()];
            
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Mode-specific learning
            if (currentMode == "memory") {
                float reward = 0.05f;
                agent->applyRewardModulation(reward, 0.0f);
                agent->updateDevelopment(0.1);
            } else if (currentMode == "prediction") {
                float novelty = agent->getNoveltyLevel();
                agent->applyRewardModulation(novelty * 0.1f, 0.0f);
            } else if (currentMode == "cognition") {
                float curiosity = agent->getCuriosityLevel();
                float predictionError = agent->getPredictionError();
                agent->applyRewardModulation(curiosity + predictionError, 0.0f);
                agent->updateDevelopment(0.1);
            }
            
            modeStep++;
        }
        
        return std::make_tuple(brain->getTotalSpikeCount(),
                              brain->getAverageFiringRate(),
                              agent->getCuriosityLevel(),
                              agent->getNoveltyLevel(),
                              agent->getPredictionError());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("duration") = 1000,
         py::arg("modes") = std::vector<std::string>{"memory", "prediction", "cognition"},
         py::arg("modeTransitionRate") = 0.1f,
         "Run hybrid experiment cycling through different cognitive modes");

    // ==============================================
    // ADVANCED UTILITY AND DIAGNOSTIC FUNCTIONS
    // ==============================================

    m.def("analyzeBrainState", [](std::shared_ptr<Brain> brain) {
        brain->logStatus();
        
        // Analyze brain state health
        bool isHealthy = true;
        std::string healthReport;
        
        // Check neuron counts
        size_t totalNeurons = brain->getTotalNeuronCount();
        if (totalNeurons < 100) {
            isHealthy = false;
            healthReport += "WARNING: Low neuron count (" + std::to_string(totalNeurons) + "). ";
        }
        
        // Check synapse count
        size_t totalSynapses = brain->getTotalSynapseCount();
        if (totalSynapses < 10) {
            healthReport += "WARNING: Low synapse count (" + std::to_string(totalSynapses) + "). ";
        }
        
        // Check firing rate
        float firingRate = brain->getAverageFiringRate();
        if (firingRate > 200.0f) {
            healthReport += "WARNING: High firing rate may indicate instability. ";
        } else if (firingRate < 0.1f) {
            healthReport += "INFO: Low firing rate - brain may be inactive. ";
        }
        
        // Check E/I ratio
        float eiRatio = brain->getExcitationInhibitionRatio();
        if (eiRatio > 10.0f) {
            healthReport += "WARNING: High E/I ratio may indicate imbalance. ";
        } else if (eiRatio < 0.1f && eiRatio > 0.0f) {
            healthReport += "WARNING: Low E/I ratio may indicate excessive inhibition. ";
        }
        
        // Check developmental stage
        auto stage = brain->getDevelopmentalStage();
        if (stage == DevelopmentalStage::Initial && totalNeurons > 1000) {
            healthReport += "INFO: Brain initialized but not developed. ";
        }
        
        return std::make_tuple(isHealthy, healthReport.empty() ? "System healthy" : healthReport);
    }, py::arg("brain"), "Analyze brain state and provide health recommendations");

    m.def("compareAgentBrains", [](std::shared_ptr<AgentBrain> agent1,
                                   std::shared_ptr<AgentBrain> agent2,
                                   std::shared_ptr<Brain> brain1,
                                   std::shared_ptr<Brain> brain2) {
        auto stage1 = agent1->getDevelopmentalStage();
        auto stage2 = agent2->getDevelopmentalStage();
        
        float curiosity1 = agent1->getCuriosityLevel();
        float curiosity2 = agent2->getCuriosityLevel();
        
        float novelty1 = agent1->getNoveltyLevel();
        float novelty2 = agent2->getNoveltyLevel();
        
        float predictionError1 = agent1->getPredictionError();
        float predictionError2 = agent2->getPredictionError();
        
        float dopamine1 = agent1->getNeuromodulationLevel();
        float dopamine2 = agent2->getNeuromodulationLevel();
        
        std::string comparison;
        
        if (stage1 != stage2) {
            comparison += "Different developmental stages. ";
        }
        
        if (std::abs(curiosity1 - curiosity2) > 0.2f) {
            comparison += "Significant curiosity difference. ";
        }
        
        if (std::abs(novelty1 - novelty2) > 0.3f) {
            comparison += "Significant novelty difference. ";
        }
        
        if (std::abs(predictionError1 - predictionError2) > 0.5f) {
            comparison += "Different prediction errors. ";
        }
        
        if (std::abs(dopamine1 - dopamine2) > 0.2f) {
            comparison += "Different neuromodulation levels. ";
        }
        
        if (comparison.empty()) {
            comparison = "Agents are similarly configured. ";
        }
        
        return comparison;
    }, py::arg("agent1"), py::arg("agent2"), py::arg("brain1"), py::arg("brain2"),
         "Compare two agent brains and identify differences");

    m.def("benchmarkBrainPerformance", [](std::shared_ptr<Brain> brain,
                                         size_t testDuration = 100,
                                         const std::string& benchmarkType = "spiking") {
        brain->initialize();
        
        size_t spikeCount = 0;
        float maxFiringRate = 0.0f;
        
        for (size_t step = 0; step < testDuration; ++step) {
            brain->step(step);
            spikeCount += brain->getFiringNeuronCount();
            maxFiringRate = std::max(maxFiringRate, brain->getAverageFiringRate());
        }
        
        std::string benchmarkReport;
        
        if (benchmarkType == "spiking") {
            benchmarkReport = "Spike count: " + std::to_string(spikeCount) + " over " + std::to_string(testDuration) + " steps. ";
        } else if (benchmarkType == "stability") {
            float stability = 1.0f - (maxFiringRate / 100.0f); // Simple stability metric
            benchmarkReport = "Stability: " + std::to_string(std::max(0.0f, stability)) + ". ";
        } else if (benchmarkType == "efficiency") {
            float efficiency = spikeCount / static_cast<float>(testDuration);
            benchmarkReport = "Spikes per step: " + std::to_string(efficiency) + ". ";
        }
        
        return std::make_tuple(spikeCount, maxFiringRate, benchmarkReport);
    }, py::arg("brain"), py::arg("testDuration") = 100, py::arg("benchmarkType") = "spiking",
         "Benchmark brain performance over a test duration");

    // ==============================================
    // LEGACY COMPATIBILITY
    // ==============================================

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
