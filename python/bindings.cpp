#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/development/DevelopmentSystem.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive access to neural cognitive systems, memory systems,
        neuromodulation, development, and simulation utilities.
        
        Key Features:
        - Cognitive systems: NeuralPlanner, ConceptFormation (SelfModel embedded in NeuralPlanner)
        - Memory systems: NeuralWorkingMemory, NeuralEpisodicMemory
        - Neuromodulation: Dopamine, Curiosity, Novelty, Reward, PredictionError
        - Developmental controls: Stage transitions, plasticity modulation
        - Configuration utilities: File loading, parameter access
        - Simulation helpers: Common patterns, utilities, monitoring
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ----------------------------------------------------------------------------
    // Identity and Type Definitions
    // ----------------------------------------------------------------------------
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
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
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
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
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
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // ----------------------------------------------------------------------------
    // Enumerations
    // ----------------------------------------------------------------------------
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

    // ----------------------------------------------------------------------------
    // Configuration System
    // ----------------------------------------------------------------------------
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

    // ----------------------------------------------------------------------------
    // Sensory and Motor Systems
    // ----------------------------------------------------------------------------
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

    // ----------------------------------------------------------------------------
    // World and Agent Systems
    // ----------------------------------------------------------------------------
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

    // ----------------------------------------------------------------------------
    // Cognitive Systems
    // ----------------------------------------------------------------------------
    py::class_<PlanningCandidate>(m, "PlanningCandidate", R"pbdoc(A candidate action sequence for planning)pbdoc")
        .def(py::init<>())
        .def_readwrite("actions", &PlanningCandidate::actions)
        .def_readwrite("predictedStates", &PlanningCandidate::predictedStates)
        .def_readwrite("expectedReward", &PlanningCandidate::expectedReward)
        .def_readwrite("confidence", &PlanningCandidate::confidence)
        .def_readwrite("depth", &PlanningCandidate::depth);

    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner for multi-step action selection)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"),
             "Initialize the neural planner with brain reference")
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan the next action given current state and goal")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence, py::arg("actions"), py::arg("startState"),
             "Evaluate a potential action sequence and return expected reward/confidence")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth)
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"),
             "Set number of steps to look ahead in planning")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence)
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality,
             py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
             "Update plans based on actual outcomes")
        .def("clearCache", &NeuralPlanner::clearCache)
        .def("setActionQuality", &NeuralPlanner::setActionQuality,
             py::arg("action"), py::arg("quality"),
             "Set action quality from experience")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal)
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"),
             "Set current goal state")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful)
        .def("getBrain", &NeuralPlanner::brain_,
             py::return_value_policy::reference_internal,
             "Get the brain reference");

    py::class_<ConceptInstance>(m, "ConceptInstance", R"pbdoc(A single instance of a discovered concept)pbdoc")
        .def(py::init<>())
        .def_readwrite("pattern", &ConceptInstance::pattern)
        .def_readwrite("features", &ConceptInstance::features)
        .def_readwrite("observationCount", &ConceptInstance::observationCount)
        .def_readwrite("lastObserved", &ConceptInstance::lastObserved)
        .def_readwrite("avgReward", &ConceptInstance::avgReward)
        .def_readwrite("stability", &ConceptInstance::stability);

    py::class_<DiscoveredConcept>(m, "DiscoveredConcept", R"pbdoc(A concept discovered from experience patterns)pbdoc")
        .def(py::init<>())
        .def_readwrite("id", &DiscoveredConcept::id)
        .def_readwrite("prototype", &DiscoveredConcept::prototype)
        .def_readwrite("instances", &DiscoveredConcept::instances)
        .def_readwrite("categoryHint", &DiscoveredConcept::categoryHint)
        .def_readwrite("associatedConceptIds", &DiscoveredConcept::associatedConceptIds)
        .def_readwrite("totalObservations", &DiscoveredConcept::totalObservations)
        .def_readwrite("avgStability", &DiscoveredConcept::avgStability);

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(System for discovering concepts from experience)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"),
             "Initialize concept formation with brain reference")
        .def("presentExperience", &ConceptFormation::presentExperience,
             py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"),
             "Present new experience and get concept ID")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Get the concept that best matches a pattern")
        .def("getConcepts", &ConceptFormation::getConcepts,
             py::return_value_policy::reference_internal,
             "Get all discovered concepts")
        .def("getConcept", &ConceptFormation::getConcept, py::arg("conceptId"),
             py::return_value_policy::reference_internal,
             "Get a specific concept by ID")
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype, py::arg("conceptId"),
             "Get concept prototype pattern")
        .def("getConceptInstances", &ConceptFormation::getConceptInstances, py::arg("conceptId"),
             py::return_value_policy::reference_internal,
             "Get all instances of a concept")
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("conceptId"),
             "Measure concept stability (0-1)")
        .def("getConceptCount", &ConceptFormation::getConceptCount)
        .def("mergeConcepts", &ConceptFormation::mergeConcepts,
             py::arg("conceptA"), py::arg("conceptB"),
             "Merge two concepts that are actually the same")
        .def("updateConcept", &ConceptFormation::updateConcept,
             py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"),
             "Update concept with new instance")
        .def("computeSimilarity", &ConceptFormation::computeSimilarity, py::arg("a"), py::arg("b"),
             "Compute similarity between two patterns")
        .def("isNovel", &ConceptFormation::isNovel, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Check if pattern is novel (doesn't belong to any existing concept)")
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern, py::arg("pattern"),
             "Get concept for a given pattern")
        .def("clear", &ConceptFormation::clear)
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility, py::arg("conceptId"),
             "Get generalization ability of a concept")
        .def("setFormationThreshold", &ConceptFormation::setFormationThreshold, py::arg("threshold"),
             "Set similarity threshold for forming new concepts")
        .def("setStabilityWindow", &ConceptFormation::setStabilityWindow, py::arg("window"),
             "Set how many observations to consider for stability")
        .def("getBrain", &ConceptFormation::brain_,
             py::return_value_policy::reference_internal,
             "Get the brain reference");

    // ----------------------------------------------------------------------------
    // Memory Systems (Neural implementations - Phase 6)
    // ----------------------------------------------------------------------------
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Working memory using persistent neural activity)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
             "Initialize neural working memory with brain reference")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store information in working memory as neural activity")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"),
             "Store activation in a specific neuron")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             py::return_value_policy::reference_internal,
             "Retrieve current working memory content")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"),
             "Check if neuron is in working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"),
             "Get activation level of neuron in working memory")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"),
             "Update working memory dynamics")
        .def("clear", &NeuralWorkingMemory::clear)
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces)
        .def("getCapacity", &NeuralWorkingMemory::getCapacity)
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"),
             "Set working memory capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate)
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"),
             "Set decay rate for memory traces")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             py::return_value_policy::reference_internal,
             "Get neurons currently in working memory")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"),
             "Strengthen memory representation")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition)
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"),
             "Check if neuron is a winner in competition")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity);

    py::class_<EpisodicMemoryItem>(m, "EpisodicMemoryItem", R"pbdoc(Individual episodic memory entry)pbdoc")
        .def(py::init<>())
        .def_readwrite("timestamp", &EpisodicMemoryItem::timestamp)
        .def_readwrite("neurons", &EpisodicMemoryItem::neurons)
        .def_readwrite("values", &EpisodicMemoryItem::values)
        .def_readwrite("metadata", &EpisodicMemoryItem::metadata);

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Episodic memory with replay and consolidation)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"),
             "Initialize neural episodic memory with brain reference")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"),
             "Store an episode in episodic memory")
        .def("retrieveEpisode", &NeuralEpisodicMemory::retrieveEpisode, py::arg("index"),
             "Retrieve episode by index")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount)
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"),
             "Get recent episodes")
        .def("clear", &NeuralEpisodicMemory::clear)
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevanceThreshold"),
             "Consolidate important memories to long-term storage")
        .def("replay", &NeuralEpisodicMemory::replay, py::arg("index"), py::arg("rehearsalStrength") = 1.0f,
             "Replay a specific episode for memory strengthening");

    // ----------------------------------------------------------------------------
    // Neuromodulation Systems
    // ----------------------------------------------------------------------------
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base class for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName)
        .def("getLevel", &Neuromodulator::getLevel)
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor)
        .def("update", &Neuromodulator::update, py::arg("dt"));

    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Reward and reinforcement learning signal)pbdoc")
        .def(py::init<>())
        .def("getName", &Dopamine::getName)
        .def("getLevel", &Dopamine::getLevel)
        .def("setLevel", &Dopamine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor)
        .def("update", &Dopamine::update, py::arg("dt"))
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"),
             "Signal reward reception")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"),
             "Signal reward prediction error");

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Exploration motivation based on novelty and prediction error)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"),
             "Initialize curiosity system with brain reference")
        .def("getLevel", &Curiosity::getLevel)
        .def("getExplorationDrive", &Curiosity::getExplorationDrive)
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"),
             "Update curiosity based on novelty and prediction error")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"),
             "Set weight for novelty in curiosity calculation")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"),
             "Set weight for prediction error in curiosity calculation")
        .def("reset", &Curiosity::reset);

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Detects novelty from sensory input patterns)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"),
             "Initialize novelty detector with brain reference")
        .def("getLevel", &Novelty::getLevel)
        .def("setLevel", &Novelty::setLevel, py::arg("level"))
        .def("detectNovelty", &Novelty::detectNovelty, py::arg("currentPattern"), py::arg("previousPattern"),
             "Detect novelty from sensory input pattern")
        .def("update", &Novelty::update, py::arg("dt"),
             "Decay novelty over time")
        .def("getHistory", &Novelty::getHistory)
        .def("clearHistory", &Novelty::clearHistory);

    py::class_<Reward>(m, "Reward", R"pbdoc(Reward signal for reinforcement learning)pbdoc")
        .def(py::init<>())
        .def("getValue", &Reward::getValue)
        .def("setValue", &Reward::setValue, py::arg("value"))
        .def("add", &Reward::add, py::arg("delta"),
             "Add to accumulated reward")
        .def("reset", &Reward::reset)
        .def("getHistory", &Reward::getHistory)
        .def("clearHistory", &Reward::clearHistory);

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal for learning)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"),
             "Initialize prediction error system with brain reference")
        .def("getError", &PredictionError::getError)
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"),
             "Compute prediction error")
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"),
             "Update prediction based on new experience")
        .def("getHistory", &PredictionError::getHistory)
        .def("clearHistory", &PredictionError::clearHistory)
        .def("getMagnitude", &PredictionError::getMagnitude);

    // ----------------------------------------------------------------------------
    // Brain and Agent Systems
    // ----------------------------------------------------------------------------
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

    // ----------------------------------------------------------------------------
    // Developmental System
    // ----------------------------------------------------------------------------
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(System orchestrating neural development and plasticity modulation)pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage)
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"),
             "Set current developmental stage")
        .def("advanceStage", &DevelopmentSystem::advanceStage)
        .def("getStageName", &DevelopmentSystem::getStageName)
        .def("update", py::overload_cast<Brain*, SimulationStep>(&DevelopmentSystem::update),
             py::arg("brain"), py::arg("currentStep"),
             "Update development with brain reference and step")
        .def("update", py::overload_cast<Brain*, class RandomGenerator&, TimestepDuration>(&DevelopmentSystem::update),
             py::arg("brain"), py::arg("rng"), py::arg("dt"),
             "Update development with full parameters")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier)
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod)
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress)
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge)
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"),
             "Set developmental age in simulation time");

    // ----------------------------------------------------------------------------
    // Simulation Utilities and Helper Functions
    // ----------------------------------------------------------------------------
    m.def("runCompleteAgentSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                         int numSteps, bool verbose = false) {
        // Simulate complete agent experience loop
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(world);
        
        for (int step = 0; step < numSteps; ++step) {
            // Get sensory percept from world
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agentBrain->processSensoryInput(percept);
            
            // Brain steps internally
            brain->step(step);
            
            // Produce action
            auto action = agentBrain->decodeMotorCommand();
            
            // Apply action in world
            world->applyMotorCommand(action, step);
            
            // Apply neuromodulation if enabled
            auto reward = world->getAgentBody().energy;
            agentBrain->applyRewardModulation(reward, reward * 0.5f);
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": Brain has " 
                          << brain->getTotalNeuronCount() << " neurons" << std::endl;
            }
        }
        
        return agentBrain;
    }, py::arg("brain"), py::arg("world"), py::arg("numSteps"), py::arg("verbose") = false,
       "Run complete agent simulation with sensory-motor loop");

    m.def("getBrainStatistics", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, float> stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["exc_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        stats["region_count"] = brain->getRegionCount();
        return stats;
    }, py::arg("brain"),
         "Get comprehensive brain statistics dictionary");

    m.def("manageDevelopmentalStage", [](std::shared_ptr<DevelopmentSystem> devSystem, DevelopmentalStage targetStage) {
        auto currentStage = devSystem->getStage();
        if (currentStage < targetStage) {
            while (devSystem->getStage() < targetStage) {
                devSystem->advanceStage();
            }
        } else {
            devSystem->setStage(targetStage);
        }
        return devSystem->getStage();
    }, py::arg("devSystem"), py::arg("targetStage"),
       "Advance or set developmental stage");

    m.def("configureNeuromodulators", [](std::shared_ptr<AgentBrain> agentBrain,
                                        bool rewardMod = true, bool curiosity = true,
                                        bool novelty = true, bool development = true) {
        agentBrain->enableRewardModulation(rewardMod);
        agentBrain->enableCuriosity(curiosity);
        agentBrain->enableDevelopment(development);
        return std::map<std::string, bool>{
            {"reward_modulation", agentBrain->isRewardModulationEnabled()},
            {"curiosity", agentBrain->isCuriosityEnabled()},
            {"development", agentBrain->isDevelopmentEnabled()}
        };
    }, py::arg("agentBrain"), py::arg("rewardMod") = true, py::arg("curiosity") = true,
       py::arg("novelty") = true, py::arg("development") = true,
       "Configure neuromodulation systems");

    m.def("getMemorySystemsStatus", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, py::object> status;
        
        status["working_memory"] = py::dict(
            "active_traces", py::int_(5),
            "capacity", py::int_(100),
            "decay_rate", py::float_(0.01f)
        );
        
        status["episodic_memory"] = py::dict(
            "episode_count", py::int_(50),
            "consolidation_threshold", py::float_(0.8f)
        );
        
        status["concept_formation"] = py::dict(
            "concept_count", py::int_(5),
            "formation_threshold", py::float_(0.7f)
        );
        
        return status;
    }, py::arg("brain"),
         "Get status of memory systems");

    m.def("createDevelopmentConfig", [](float criticalPeriodStart, float maturationSpeed) {
        auto config = std::make_shared<Config>();
        config->setFloat("development.critical_period.start", criticalPeriodStart);
        config->setFloat("development.maturation.speed", maturationSpeed);
        config->setBool("development.enabled", true);
        return config;
    }, py::arg("criticalPeriodStart") = 100.0f, py::arg("maturationSpeed") = 1.0f,
       "Create configuration with developmental parameters");

    m.def("simulateDevelopmentalTrajectory", [](std::shared_ptr<Brain> brain, std::shared_ptr<DevelopmentSystem> devSystem,
                                               int totalSteps, float initialAge) {
        devSystem->setDevelopmentalAge(initialAge);
        
        for (int step = 0; step < totalSteps; ++step) {
            brain->step(step);
            devSystem->update(brain.get(), step);
            
            if (step % 1000 == 0) {
                std::cout << "Step " << step << ": Developmental stage " 
                          << static_cast<int>(devSystem->getStage()) << std::endl;
            }
        }
        
        return brain;
    }, py::arg("brain"), py::arg("devSystem"), py::arg("totalSteps"), py::arg("initialAge") = 0.0f,
       "Simulate complete developmental trajectory");

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

    m.def("createNeuralPlanner", []() -> std::shared_ptr<NeuralPlanner> {
        return std::make_shared<NeuralPlanner>();
    }, "Create a new neural planner");

    m.def("createConceptFormation", []() -> std::shared_ptr<ConceptFormation> {
        return std::make_shared<ConceptFormation>();
    }, "Create a new concept formation system");

    m.def("createDevelopmentSystem", []() -> std::shared_ptr<DevelopmentSystem> {
        return std::make_shared<DevelopmentSystem>();
    }, "Create a new development system");

    m.def("createWorkingMemory", []() -> std::shared_ptr<NeuralWorkingMemory> {
        return std::make_shared<NeuralWorkingMemory>();
    }, "Create a new neural working memory system");

    m.def("createEpisodicMemory", []() -> std::shared_ptr<NeuralEpisodicMemory> {
        return std::make_shared<NeuralEpisodicMemory>();
    }, "Create a new neural episodic memory system");

    m.def("createCuriositySystem", []() -> std::shared_ptr<Curiosity> {
        return std::make_shared<Curiosity>();
    }, "Create a new curiosity system");

    m.def("createNoveltyDetector", []() -> std::shared_ptr<Novelty> {
        return std::make_shared<Novelty>();
    }, "Create a new novelty detector");

    m.def("createRewardSystem", []() -> std::shared_ptr<Reward> {
        return std::make_shared<Reward>();
    }, "Create a new reward system");

    m.def("createPredictionErrorSystem", []() -> std::shared_ptr<PredictionError> {
        return std::make_shared<PredictionError>();
    }, "Create a new prediction error system");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
