#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Include all core headers that Python bindings need
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive bindings for neural computation, memory systems,
        prediction, cognition, and neuromodulation.
        
        Key Features:
        - Brain simulation with event-driven neural dynamics
        - Integrated memory systems (working, episodic, semantic, procedural, associative)
        - Prediction and cognition modules
        - Neuromodulation (dopamine, curiosity, novelty, prediction error, reward)
        - Agent framework for interaction with environments
        - Visualization and monitoring tools
        
        Usage Examples:
            import pynlm
            
            # Create brain with default configuration
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            # Run simulation
            for step in range(100):
                brain.step(step)
            
            # Create world and agent
            world = pynlm.createSimpleWorld()
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            for step in range(50):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Neuron and synapse identifiers
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

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Type of motor action)pbdoc")
        .value("None", ActionType::None)
        .value("Movement", ActionType::Movement)
        .value("Gaze", ActionType::Gaze)
        .value("Manipulation", ActionType::Manipulation)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(Type of world object)pbdoc")
        .value("Static", WorldObjectType::Static)
        .value("Dynamic", WorldObjectType::Dynamic)
        .value("Agent", WorldObjectType::Agent)
        .value("Goal", WorldObjectType::Goal)
        .value("Obstacle", WorldObjectType::Obstacle)
        .export_values();

    // Sensor and motor signals
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Input signal from environment)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryInput::getVision)
        .def("setVision", &SensoryInput::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryInput::getVisionWidth)
        .def("getVisionHeight", &SensoryInput::getVisionHeight)
        .def("getTouch", &SensoryInput::getTouch)
        .def("setTouch", &SensoryInput::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryInput::getInternal)
        .def("setInternal", &SensoryInput::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryInput::getProprioception)
        .def("setProprioception", &SensoryInput::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryInput::getAudio)
        .def("setAudio", &SensoryInput::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryInput::getAllSignals)
        .def("getTimestamp", &SensoryInput::getTimestamp)
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"));

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

    // Configuration system
    py::class_<Config>(m, "Config", R"pbdoc(Configuration system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile,
             py::arg("filepath"), "Load configuration from JSON file")
        .def("loadFromArgs", &Config::loadFromArgs,
             py::arg("argc"), py::arg("argv"), "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile,
             py::arg("filepath"), "Save configuration to file")
        .def("get", &Config::get<int>, py::arg("key"), "Get integer configuration value")
        .def("get", &Config::get<int64_t>, py::arg("key"), "Get int64 configuration value")
        .def("get", &Config::get<double>, py::arg("key"), "Get double configuration value")
        .def("get", &Config::get<bool>, py::arg("key"), "Get boolean configuration value")
        .def("get", &Config::get<std::string>, py::arg("key"), "Get string configuration value")
        .def("getOr", &Config::getOr<int>, py::arg("key"), py::arg("defaultValue"), "Get integer with default")
        .def("getOr", &Config::getOr<int64_t>, py::arg("key"), py::arg("defaultValue"), "Get int64 with default")
        .def("getOr", &Config::getOr<double>, py::arg("key"), py::arg("defaultValue"), "Get double with default")
        .def("getOr", &Config::getOr<bool>, py::arg("key"), py::arg("defaultValue"), "Get boolean with default")
        .def("getOr", &Config::getOr<std::string>, py::arg("key"), py::arg("defaultValue"), "Get string with default")
        .def("set", &Config::set, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set configuration value")
        .def("set", &Config::set<std::string>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set string configuration value")
        .def("set", &Config::set<int>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set integer configuration value")
        .def("set", &Config::set<double>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set double configuration value")
        .def("set", &Config::set<bool>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set boolean configuration value")
        .def("has", &Config::has, py::arg("key"), "Check if configuration key exists")
        .def("remove", &Config::remove, py::arg("key"), "Remove configuration parameter")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration parameters")
        .def("summary", &Config::summary, "Get configuration summary");

    // Memory systems
    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory for temporary information storage)pbdoc")
        .def(py::init<>())
        .def("initialize", &WorkingMemory::initialize, py::arg("capacity") = 100, "Initialize with capacity")
        .def("store", &WorkingMemory::store, py::arg("neuron"), py::arg("value"), "Store item")
        .def("retrieve", &WorkingMemory::retrieve, py::arg("neuron"), "Retrieve item")
        .def("contains", &WorkingMemory::contains, py::arg("neuron"), "Check if item exists")
        .def("clear", &WorkingMemory::clear, "Clear all items")
        .def("getCapacity", &WorkingMemory::getCapacity, "Get capacity")
        .def("getCurrentSize", &WorkingMemory::getCurrentSize, "Get current size")
        .def("setCapacity", &WorkingMemory::setCapacity, py::arg("capacity"), "Set capacity")
        .def("decay", &WorkingMemory::decay, py::arg("decayRate"), "Decay all items");

    py::class_<EpisodicMemoryItem>(m, "EpisodicMemoryItem", R"pbdoc(Episodic memory item (experience))pbdoc")
        .def(py::init<>())
        .def_readwrite("timestamp", &EpisodicMemoryItem::timestamp)
        .def_readwrite("neurons", &EpisodicMemoryItem::neurons)
        .def_readwrite("values", &EpisodicMemoryItem::values)
        .def_readwrite("metadata", &EpisodicMemoryItem::metadata);

    py::class_<EpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory for experience storage)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", &EpisodicMemory::storeEpisode, py::arg("episode"), "Store episode")
        .def("retrieveEpisode", &EpisodicMemory::retrieveEpisode, py::arg("index"), "Retrieve episode")
        .def("getEpisodeCount", &EpisodicMemory::getEpisodeCount, "Get episode count")
        .def("getRecentEpisodes", &EpisodicMemory::getRecentEpisodes, py::arg("count"), "Get recent episodes")
        .def("clear", &EpisodicMemory::clear, "Clear all episodes")
        .def("consolidate", &EpisodicMemory::consolidate, py::arg("relevanceThreshold"), "Consolidate memory");

    py::class_<SemanticMemory>(m, "SemanticMemory", R"pbdoc(Semantic memory for facts and knowledge)pbdoc")
        .def(py::init<>())
        .def("storeFact", &SemanticMemory::storeFact, py::arg("key"), py::arg("value"), "Store fact")
        .def("retrieveFact", &SemanticMemory::retrieveFact, py::arg("key"), "Retrieve fact")
        .def("hasFact", &SemanticMemory::hasFact, py::arg("key"), "Check if fact exists")
        .def("getAllFacts", &SemanticMemory::getAllFacts, "Get all facts")
        .def("clear", &SemanticMemory::clear, "Clear all facts");

    py::class_<Skill>(m, "Skill", R"pbdoc(Learned skill/procedural memory)pbdoc")
        .def(py::init<>())
        .def_readwrite("name", &Skill::name)
        .def_readwrite("neuralPattern", &Skill::neuralPattern)
        .def_readwrite("proficiency", &Skill::proficiency);

    py::class_<ProceduralMemory>(m, "ProceduralMemory", R"pbdoc(Procedural memory for skills and habits)pbdoc")
        .def(py::init<>())
        .def("learnSkill", &ProceduralMemory::learnSkill, py::arg("name"), py::arg("pattern"), "Learn skill")
        .def("getSkill", &ProceduralMemory::getSkill, py::arg("name"), "Get skill")
        .def("getAllSkills", &ProceduralMemory::getAllSkills, "Get all skills")
        .def("updateProficiency", &ProceduralMemory::updateProficiency, py::arg("name"), py::arg("delta"), "Update proficiency")
        .def("clear", &ProceduralMemory::clear, "Clear all skills");

    py::class_<AssociativeMemory>(m, "AssociativeMemory", R"pbdoc(Associative memory for pattern associations)pbdoc")
        .def(py::init<>())
        .def("associate", &AssociativeMemory::associate, py::arg("a"), py::arg("b"), py::arg("strength"), "Create association")
        .def("getAssociations", &AssociativeMemory::getAssociations, py::arg("neuron"), "Get associated neurons")
        .def("getAssociationStrength", &AssociativeMemory::getAssociationStrength, py::arg("a"), py::arg("b"), "Get association strength")
        .def("updateAssociation", &AssociativeMemory::updateAssociation, py::arg("a"), py::arg("b"), py::arg("delta"), "Update association")
        .def("clear", &AssociativeMemory::clear, "Clear all associations");

    // Neuromodulation systems
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Base neuromodulator class)pbdoc")
        .def(py::init<>())
        .def_readwrite("level", &Neuromodulator::level);

    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
        .def(py::init<>())
        .def_readwrite("level", &Dopamine::level)
        .def_readwrite("predictionError", &Dopamine::predictionError);

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)pbdoc")
        .def(py::init<>())
        .def_readwrite("level", &Curiosity::level)
        .def_readwrite("explorationDrive", &Curiosity::explorationDrive);

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection system)pbdoc")
        .def(py::init<>())
        .def_readwrite("level", &Novelty::level)
        .def_readwrite("sensorySurprise", &Novelty::sensorySurprise);

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal)pbdoc")
        .def(py::init<>())
        .def_readwrite("error", &PredictionError::error)
        .def_readwrite("confidence", &PredictionError::confidence);

    py::class_<Reward>(m, "Reward", R"pbdoc(Reward signal)pbdoc")
        .def(py::init<>())
        .def_readwrite("level", &Reward::level)
        .def_readwrite("predictionError", &Reward::predictionError);

    // World simulation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"), "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), "Set agent start position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent body")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove world object")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get max energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set max energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed");

    // Agent framework
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"), "Process sensory percept")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"), "Apply reward modulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"), "Enable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"), "Enable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"), "Enable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"), "Enable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, "Check if reward modulation enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, "Check if structural plasticity enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, "Check if development enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, "Check if curiosity enabled");

    // Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize, "Initialize the brain")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"), "Receive sensory input")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"), "Inject current into neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"), "Inject current to neurons")
        .def("produceAction", &Brain::produceAction, "Produce motor action")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"), py::return_value_policy::reference_internal, "Get region")
        .def("getRegionCount", &Brain::getRegionCount, "Get region count")
        .def("getRegionIds", &Brain::getRegionIds, "Get region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal, "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get active neuron count")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get firing neuron count")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get E/I ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("getWorkingMemory", &Brain::getWorkingMemory, "Get working memory")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory, "Get episodic memory")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory, "Get associative memory")
        .def("getPredictionSystem", &Brain::getPredictionSystem, "Get prediction system")
        .def("getPlanner", &Brain::getPlanner, "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation, "Get concept formation")
        .def("getAttention", &Brain::getAttention, "Get attentional selection")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem, "Get development system")
        .def("getDopamine", &Brain::getDopamine, "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity, "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty, "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal, "Get prediction error signal")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, "Get configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status");

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

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Configuration source enum
    py::enum_<ConfigSource>(m, "ConfigSource")
        .value("Default", ConfigSource::Default)
        .value("File", ConfigSource::File)
        .value("CommandLine", ConfigSource::CommandLine)
        .value("Runtime", ConfigSource::Runtime)
        .export_values();

    // Logging convenience functions
    m.def("logDebug", [](const std::string& message) {
        Logger::getGlobal().debug(message);
    }, py::arg("message"), "Log debug message");

    m.def("logInfo", [](const std::string& message) {
        Logger::getGlobal().info(message);
    }, py::arg("message"), "Log info message");

    m.def("logWarning", [](const std::string& message) {
        Logger::getGlobal().warning(message);
    }, py::arg("message"), "Log warning message");

    m.def("logError", [](const std::string& message) {
        Logger::getGlobal().error(message);
    }, py::arg("message"), "Log error message");

    m.def("logCritical", [](const std::string& message) {
        Logger::getGlobal().critical(message);
    }, py::arg("message"), "Log critical message");

    // Stream-style logging
    py::class_<LogStream>(m, "LogStream")
        .def(py::init<>())  // No constructor needed for the static convenience
        .def("__enter__", [](LogStream& self) { return &self; })
        .def("__exit__", [](LogStream& self, py::args args) {})
        .def("__lshift__", [](LogStream& self, const std::string& value) {
            self.oss_ << value;
            return &self;
        }, py::arg("value"))
        .def("__lshift__", [](LogStream& self, int value) {
            self.oss_ << value;
            return &self;
        }, py::arg("value"))
        .def("__lshift__", [](LogStream& self, float value) {
            self.oss_ << value;
            return &self;
        }, py::arg("value"))
        .def("__lshift__", [](LogStream& self, double value) {
            self.oss_ << value;
            return &self;
        }, py::arg("value"))
        .def("getMessage", [](LogStream& self) {
            return self.oss_.str();
        });

    // Convenience macros (as Python functions)
    m.def("log_debug", [](const std::string& message) {
        NLM_LOG_DEBUG(message);
    }, py::arg("message"), "Debug logging convenience function");

    m.def("log_info", [](const std::string& message) {
        NLM_LOG_INFO(message);
    }, py::arg("message"), "Info logging convenience function");

    m.def("log_warning", [](const std::string& message) {
        NLM_LOG_WARNING(message);
    }, py::arg("message"), "Warning logging convenience function");

    m.def("log_error", [](const std::string& message) {
        NLM_LOG_ERROR(message);
    }, py::arg("message"), "Error logging convenience function");

    m.def("log_critical", [](const std::string& message) {
        NLM_LOG_CRITICAL(message);
    }, py::arg("message"), "Critical logging convenience function");

    // Logger class for advanced usage
    py::class_<Logger>(m, "Logger", R"pbdoc(Global logger for NLM")
        .def_static("getGlobal", &Logger::getGlobal, "Get global logger instance")
        .def_static("setGlobal", &Logger::setGlobal, py::arg("logger"), "Set global logger")
        .def("addLogger", &Logger::addLogger, py::arg("logger"), "Add logger sink")
        .def("clearLoggers", &Logger::clearLoggers, "Remove all loggers")
        .def("setLevel", &Logger::setLevel, py::arg("level"), "Set log level for all loggers")
        .def("debug", &Logger::debug, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "", "Log debug message")
        .def("info", &Logger::info, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "", "Log info message")
        .def("warning", &Logger::warning, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "", "Log warning message")
        .def("error", &Logger::error, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "", "Log error message")
        .def("critical", &Logger::critical, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "", "Log critical message")
        .def("isEnabled", &Logger::isEnabled, py::arg("level"), "Check if level is enabled");

} // namespace nlm