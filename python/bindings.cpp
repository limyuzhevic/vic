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

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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

    // Phase 6: Memory systems for storing experiences and learned information
    py::class_<EpisodicMemoryItem>(m, "EpisodicMemoryItem", R"pbdoc(Episodic memory item representing an event)pbdoc")
        .def(py::init<>())
        .def_readwrite("timestamp", &EpisodicMemoryItem::timestamp)
        .def_readwrite("neurons", &EpisodicMemoryItem::neurons)
        .def_readwrite("values", &EpisodicMemoryItem::values)
        .def_readwrite("metadata", &EpisodicMemoryItem::metadata);

    py::class_<EpisodicMemory, std::shared_ptr<EpisodicMemory>>(m, "EpisodicMemory", R"pbdoc(Episodic memory for storing events and experiences)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", &EpisodicMemory::storeEpisode, py::arg("episode"),
             "Store an episode in episodic memory")
        .def("retrieveEpisode", &EpisodicMemory::retrieveEpisode, py::arg("index"),
             "Retrieve an episode by index")
        .def("getEpisodeCount", &EpisodicMemory::getEpisodeCount,
             "Get the number of stored episodes")
        .def("getRecentEpisodes", &EpisodicMemory::getRecentEpisodes, py::arg("count"),
             "Get the most recent N episodes")
        .def("clear", &EpisodicMemory::clear,
             "Clear all episodes")
        .def("consolidate", &EpisodicMemory::consolidate, py::arg("relevanceThreshold"),
             "Consolidate episodes above relevance threshold");

    py::class_<WorkingMemory, std::shared_ptr<WorkingMemory>>(m, "WorkingMemory", R"pbdoc(Working memory for temporary information storage)pbdoc")
        .def(py::init<>())
        .def("store", &WorkingMemory::store, py::arg("neuron"), py::arg("value"),
             "Store a value for a neuron")
        .def("retrieve", &WorkingMemory::retrieve, py::arg("neuron"),
             "Retrieve a value for a neuron")
        .def("contains", &WorkingMemory::contains, py::arg("neuron"),
             "Check if neuron has stored value")
        .def("clear", &WorkingMemory::clear,
             "Clear all stored values")
        .def("getCapacity", &WorkingMemory::getCapacity,
             "Get working memory capacity")
        .def("getCurrentSize", &WorkingMemory::getCurrentSize,
             "Get current number of stored items")
        .def("decay", &WorkingMemory::decay, py::arg("decayRate"),
             "Decay all stored values at given rate");

    py::class_<SemanticMemory, std::shared_ptr<SemanticMemory>>(m, "SemanticMemory", R"pbdoc(Semantic memory for learned facts and knowledge)pbdoc")
        .def(py::init<>())
        .def("storeFact", &SemanticMemory::storeFact, py::arg("key"), py::arg("value"),
             "Store a fact with given key and value")
        .def("retrieveFact", &SemanticMemory::retrieveFact, py::arg("key"),
             "Retrieve a fact by key")
        .def("hasFact", &SemanticMemory::hasFact, py::arg("key"),
             "Check if a fact exists")
        .def("getAllFacts", &SemanticMemory::getAllFacts,
             "Get all facts as list of (key, value) pairs")
        .def("clear", &SemanticMemory::clear,
             "Clear all facts");

    py::class_<Skill>(m, "Skill", R"pbdoc(A learned skill with neural pattern and proficiency)pbdoc")
        .def(py::init<>())
        .def_readwrite("name", &Skill::name)
        .def_readwrite("neuralPattern", &Skill::neuralPattern)
        .def_readwrite("proficiency", &Skill::proficiency);

    py::class_<ProceduralMemory, std::shared_ptr<ProceduralMemory>>(m, "ProceduralMemory", R"pbdoc(Procedural memory for learned skills and habits)pbdoc")
        .def(py::init<>())
        .def("learnSkill", &ProceduralMemory::learnSkill, py::arg("name"), py::arg("pattern"),
             "Learn a new skill with neural pattern")
        .def("getSkill", &ProceduralMemory::getSkill, py::arg("name"),
             "Get skill by name")
        .def("getAllSkills", &ProceduralMemory::getAllSkills,
             "Get all learned skills")
        .def("updateProficiency", &ProceduralMemory::updateProficiency, py::arg("name"), py::arg("delta"),
             "Update skill proficiency")
        .def("clear", &ProceduralMemory::clear,
             "Clear all skills");

    py::class_<AssociativeMemory, std::shared_ptr<AssociativeMemory>>(m, "AssociativeMemory", R"pbdoc(Associative memory for relationships between representations)pbdoc")
        .def(py::init<>())
        .def("associate", &AssociativeMemory::associate, py::arg("a"), py::arg("b"), py::arg("strength"),
             "Create association between two neurons with given strength")
        .def("getAssociations", &AssociativeMemory::getAssociations, py::arg("neuron"),
             "Get all associated neurons")
        .def("getAssociationStrength", &AssociativeMemory::getAssociationStrength, py::arg("a"), py::arg("b"),
             "Get association strength between two neurons")
        .def("updateAssociation", &AssociativeMemory::updateAssociation, py::arg("a"), py::arg("b"), py::arg("delta"),
             "Update association strength")
        .def("clear", &AssociativeMemory::clear,
             "Clear all associations");

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

    // Neuromodulation system for learning and behavior
    py::class_<Neuromodulator, std::shared_ptr<Neuromodulator>>(m, "Neuromodulator", R"pbdoc(Abstract base class for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName,
             "Get neuromodulator name")
        .def("getLevel", &Neuromodulator::getLevel,
             "Get current concentration/level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"),
             "Set current concentration/level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor,
             "Get plasticity factor applied to synapses")
        .def("update", &Neuromodulator::update, py::arg("dt"),
             "Update neuromodulator state");

    py::class_<Dopamine, std::shared_ptr<Dopamine>>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator for reward and reinforcement learning)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"),
             "Signal reward reception")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"),
             "Signal reward prediction error");

    py::class_<Acetylcholine, std::shared_ptr<Acetylcholine>>(m, "Acetylcholine", R"pbdoc(Acetylcholine neuromodulator for attention and memory)pbdoc")
        .def(py::init<>());

    py::class_<Norepinephrine, std::shared_ptr<Norepinephrine>>(m, "Norepinephrine", R"pbdoc(Norepinephrine neuromodulator for arousal and vigilance)pbdoc")
        .def(py::init<>());

    py::class_<Serotonin, std::shared_ptr<Serotonin>>(m, "Serotonin", R"pbdoc(Serotonin neuromodulator for mood and social behavior)pbdoc")
        .def(py::init<>());

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

    // Prediction system for future state and reward prediction
    py::class_<PredictionSystem, std::shared_ptr<PredictionSystem>>(m, "PredictionSystem", R"pbdoc(Prediction system for future state and reward prediction)pbdoc")
        .def(py::init<>())
        .def("predictNextState", &PredictionSystem::predictNextState, py::arg("currentState"),
             "Make prediction for next timestep")
        .def("updatePredictions", &PredictionSystem::updatePredictions, py::arg("predicted"), py::arg("actual"),
             "Update predictions based on actual observation")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             "Get prediction error")
        .def("getConfidence", &PredictionSystem::getConfidence,
             "Get prediction confidence")
        .def("getErrorHistory", &PredictionSystem::getErrorHistory,
             "Get prediction error history")
        .def("clearHistory", &PredictionSystem::clearHistory,
             "Clear prediction history")
        .def("train", &PredictionSystem::train, py::arg("observation"),
             "Train prediction model with observation");

    py::class_<DevelopmentSystem, std::shared_ptr<DevelopmentSystem>>(m, "DevelopmentSystem", R"pbdoc(Development system for neural maturation and plasticity)pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage,
             "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"),
             "Set developmental stage")
        .def("advanceStage", &DevelopmentSystem::advanceStage,
             "Advance to next developmental stage")
        .def("getStageName", &DevelopmentSystem::getStageName,
             "Get name of current developmental stage")
        .def("update", &DevelopmentSystem::update, py::arg("brain"), py::arg("currentStep"),
             "Update development system (simple version)")
        .def("updateWithParams", &DevelopmentSystem::update, py::arg("brain"), py::arg("rng"), py::arg("dt"),
             "Update development system with full parameters")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier,
             "Get plasticity modifier for current stage")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod,
             "Check if in critical period")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress,
             "Get critical period progress (0-1)")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge,
             "Get developmental age in simulation time")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"),
             "Set developmental age");

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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

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
        // getRegion is const/non-const overloaded; bind the mutable one.
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
             "Log brain status");

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

    // Phase 6 cognition systems
    py::class_<NeuralPlanner, std::shared_ptr<NeuralPlanner>>(m, "NeuralPlanner", R"pbdoc(Neural planner for action sequence evaluation)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan next action given current state and goal")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence, py::arg("actions"), py::arg("startState"),
             "Evaluate a potential action sequence")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth,
             "Get number of steps to look ahead")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"),
             "Set planning depth")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence,
             "Get planning confidence")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality, py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
             "Update plans based on actual outcome")
        .def("clearCache", &NeuralPlanner::clearCache,
             "Clear planning cache")
        .def("setActionQuality", &NeuralPlanner::setActionQuality, py::arg("action"), py::arg("quality"),
             "Set action quality learned from experience")
        .def("getActionQuality", &NeuralPlanner::getActionQuality, py::arg("action"),
             "Get action quality from experience")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal,
             "Get current goal state")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"),
             "Set current goal state")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful,
             "Check if recent planning was successful");

    py::class_<SelfModel, std::shared_ptr<SelfModel>>(m, "SelfModel", R"pbdoc(Self-model for sensorimotor self-awareness)pbdoc")
        .def(py::init<>())
        .def("initialize", &SelfModel::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("recordSelfAction", &SelfModel::recordSelfAction, py::arg("action"), py::arg("beforeState"), py::arg("afterState"),
             "Record that taking an action caused a specific sensory change")
        .def("predictActionConsequence", &SelfModel::predictActionConsequence, py::arg("action"), py::arg("currentState"),
             "Predict sensory consequence of an action")
        .def("getSelfModelConfidence", &SelfModel::getSelfModelConfidence, py::arg("action"),
             "Get confidence in self-model for a given action")
        .def("computeSelfGeneratedLikeness", &SelfModel::computeSelfGeneratedLikeness, py::arg("beforeState"), py::arg("afterState"), py::arg("action"),
             "Is this change likely caused by self (action) vs external?")
        .def("getPreferredAction", &SelfModel::getPreferredAction, py::arg("state"),
             "Get the body schema (preferred actions in different states)")
        .def("updateSelfModel", &SelfModel::updateSelfModel, py::arg("predicted"), py::arg("actual"), py::arg("action"),
             "Update self-model based on prediction error")
        .def("getCapabilityLevel", &SelfModel::getCapabilityLevel,
             "Get current capability level (0-1)")
        .def("getBodyAwareness", &SelfModel::getBodyAwareness,
             "Get body awareness (how accurately can predict consequences)")
        .def("clear", &SelfModel::clear,
             "Clear self-model")
        .def("hasSelfModel", &SelfModel::hasSelfModel,
             "Has self-model formed?");

    py::class_<SocialLearning, std::shared_ptr<SocialLearning>>(m, "SocialLearning", R"pbdoc(Social learning from observing other agents)pbdoc")
        .def(py::init<>())
        .def("initialize", &SocialLearning::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("observeAgentAction", &SocialLearning::observeAgentAction, py::arg("observedAction"), py::arg("observerState"), py::arg("resultingState"),
             "Record observation of another agent's action and effect")
        .def("canImitate", &SocialLearning::canImitate, py::arg("observedAction"),
             "Can I imitate this observed action?")
        .def("getImitationActionAndSimilarity", &SocialLearning::getImitationActionAndSimilarity, py::arg("currentState"),
             "Phase 6: best imitation action for a state with similarity")
        .def("getImitationAction", &SocialLearning::getImitationAction, py::arg("currentState"),
             "Get the best action to imitate given current state")
        .def("learnCommunicationSignal", &SocialLearning::learnCommunicationSignal, py::arg("signalPattern"), py::arg("signalReward"),
             "Learn simple communication signal from another agent")
        .def("detectSignal", &SocialLearning::detectSignal, py::arg("neuralPattern"),
             "Detect if another agent is signaling")
        .def("getSignalPattern", &SocialLearning::getSignalPattern,
             "Get learned signal pattern")
        .def("getSignalMeaning", &SocialLearning::getSignalMeaning,
             "Get signal meaning (associated reward)")
        .def("updateSocialKnowledge", &SocialLearning::updateSocialKnowledge, py::arg("interactionReward"),
             "Update social knowledge based on interactions")
        .def("computeSimilarity", &SocialLearning::computeSimilarity, py::arg("a"), py::arg("b"),
             "Similarity between two patterns (cosine)")
        .def("clear", &SocialLearning::clear,
             "Clear social learning")
        .def("hasSocialKnowledge", &SocialLearning::hasSocialKnowledge,
             "Has learned from others?")
        .def("getObservationCount", &SocialLearning::getObservationCount,
             "Get observation count");

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

    // Phase 6 text processing: text-as-a-sense
    m.def("encodeText", &AgentBrain::encodeText, py::arg("text"),
         "Encode text into sparse sensory pattern (Phase 6 text-as-a-sense)");

    // Diagnostic and thought trace functions
    m.def("getThoughtTrace", &AgentBrain::getThoughtTrace,
         "Get thought trace (human-readable record of most recent loop)");

    m.def("getInternalFiringRates", &AgentBrain::getInternalFiringRates,
         "Get internal (hidden) neuron firing rates");

    // Brain statistics and counters
    m.def("getPlannerOverrideCount", &AgentBrain::getPlannerOverrideCount,
         "Phase 6: planner override count");
    m.def("getPlannerAgreementCount", &AgentBrain::getPlannerAgreementCount,
         "Phase 6: planner agreement count");
    m.def("getPlannerDecisionCount", &AgentBrain::getPlannerDecisionCount,
         "Phase 6: planner decision count");
    m.def("getConceptBiasCount", &AgentBrain::getConceptBiasCount,
         "Phase 6: concept bias count");
    m.def("getSelfBiasCount", &AgentBrain::getSelfBiasCount,
         "Phase 6: self bias count");
    m.def("getSocialBiasCount", &AgentBrain::getSocialBiasCount,
         "Phase 6: social bias count");
    m.def("getConceptCount", &AgentBrain::getConceptCount,
         "Phase 6: concept count");
    m.def("getSelfModelConfidence", &AgentBrain::getSelfModelConfidence,
         "Phase 6: self-model's mean confidence");
    m.def("getSocialObservationCount", &AgentBrain::getSocialObservationCount,
         "Phase 6: social observation count");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
