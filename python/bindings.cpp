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
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/motor/MotorSystem.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Random/Random.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/memory/WorkingMemory.hpp"
#include "../src/memory/EpisodicMemory.hpp"
#include "../src/memory/AssociativeMemory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/dynamics/NeuralDynamics.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Metrics.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"
#include "../src/environment/Environment.hpp"
#include "../src/environment/Observation.hpp"
#include "../src/environment/Action.hpp"
#include "../src/visualization/VisualizationInterface.hpp"
#include "../src/world/WorldObject.hpp"
#include "../src/agent/SelfModel.hpp"
#include "../src/agent/SocialLearning.hpp"

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

    // Phase 6 Memory Systems
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Working memory system with persistent activity)pbdoc")
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
             "Initialize working memory with brain reference")
        .def("update", &NeuralWorkingMemory::update, py::arg("timestep"),
             "Update working memory over timestep")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron,
             py::arg("neuronId"), py::arg("activation"),
             "Store activation to neuron in working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             "Get neurons currently in working memory")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity,
             "Get working memory capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("capacity"),
             "Set working memory capacity")
        .def("clear", &NeuralWorkingMemory::clear,
             "Clear all working memory traces");

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Episodic memory system for experience storage)pbdoc")
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"),
             "Initialize episodic memory with brain reference")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"),
             "Store an episodic memory item")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             "Get number of stored episodes")
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay,
             py::arg("maxEpisodes"), "Get episodes for replay")
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"),
             "Replay an episodic memory")
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar,
             py::arg("sensoryState"), py::arg("maxResults"),
             "Retrieve similar episodes to given sensory state")
        .def("getMaxEpisodes", &NeuralEpisodicMemory::getMaxEpisodes,
             "Get maximum number of episodes")
        .def("setMaxEpisodes", &NeuralEpisodicMemory::setMaxEpisodes, py::arg("max"),
             "Set maximum number of episodes")
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("threshold"),
             "Consolidate important memories, remove weak ones")
        .def("clear", &NeuralEpisodicMemory::clear,
             "Clear all episodic memory");

    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Associative memory system for pattern associations)pbdoc")
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"),
             "Initialize associative memory with brain reference")
        .def("storeAssociation", &NeuralAssociativeMemory::storeAssociation,
             py::arg("pattern1"), py::arg("pattern2"), py::arg("strength"),
             "Store an associative memory link")
        .def("recall", &NeuralAssociativeMemory::recall, py::arg("pattern"),
             "Recall associated pattern");

    // Phase 6 Neuromodulation Systems
    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator for reward prediction error)pbdoc")
        .def("initialize", &Dopamine::initialize, py::arg("brain"),
             "Initialize dopamine system with brain reference")
        .def("update", &Dopamine::update, py::arg("timestep"),
             "Update dopamine level over timestep")
        .def("getLevel", &Dopamine::getLevel,
             "Get current dopamine level")
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor,
             "Get dopamine effect on plasticity");

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detector for surprise and curiosity)pbdoc")
        .def("initialize", &Novelty::initialize, py::arg("brain"),
             "Initialize novelty detector with brain reference")
        .def("update", &Novelty::update, py::arg("timestep"),
             "Update novelty detection over timestep")
        .def("getLevel", &Novelty::getLevel,
             "Get current novelty level")
        .def("setThreshold", &Novelty::setThreshold, py::arg("threshold"),
             "Set novelty detection threshold");

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity system for exploration behavior)pbdoc")
        .def("initialize", &Curiosity::initialize, py::arg("brain"),
             "Initialize curiosity system with brain reference")
        .def("update", &Curiosity::update, py::arg("timestep"),
             "Update curiosity level over timestep")
        .def("getLevel", &Curiosity::getLevel,
             "Get current curiosity level")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive,
             "Get current exploration drive")
        .def("enableExploration", &Curiosity::enableExploration, py::arg("enable"),
             "Enable or disable exploration behavior");

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal for learning)pbdoc")
        .def("initialize", &PredictionError::initialize, py::arg("brain"),
             "Initialize prediction error system with brain reference")
        .def("update", &PredictionError::update, py::arg("timestep"),
             "Update prediction error over timestep")
        .def("getLevel", &PredictionError::getLevel,
             "Get current prediction error level")
        .def("setPredictor", &PredictionError::setPredictor, py::arg("predictor"),
             "Set prediction system reference");

    // Phase 6 Cognition Systems
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planning system for action sequences)pbdoc")
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"),
             "Initialize planner with brain reference")
        .def("planAction", &NeuralPlanner::planAction,
             py::arg("state"), py::arg("confidence"),
             "Plan action based on current state")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality,
             py::arg("planned"), py::arg("actual"), py::arg("outcome"),
             "Update plan quality based on outcome")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence,
             "Get planning confidence level")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"),
             "Set planning depth (number of steps ahead)")
        .def("getLastPlannedAction", &NeuralPlanner::getLastPlannedAction,
             "Get last planned action");

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation from experience patterns)pbdoc")
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"),
             "Initialize concept formation with brain reference")
        .def("presentExperience", &ConceptFormation::presentExperience,
             py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("timestamp"),
             "Present experience to concept formation system")
        .def("getConceptCount", &ConceptFormation::getConceptCount,
             "Get number of formed concepts")
        .def("getConcepts", &ConceptFormation::getConcepts,
             "Get all formed concepts")
        .def("getConceptStability", &ConceptFormation::getConceptStability,
             py::arg("conceptId"), "Get stability of a specific concept")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept,
             py::arg("pattern"), "Find concept matching given pattern")
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility,
             py::arg("conceptId"), "Get generalization ability of concept");

    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Attention system with competitive selection)pbdoc")
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"),
             "Initialize attention system with brain reference")
        .def("update", &AttentionalSelection::update, py::arg("timestep"),
             "Update attention dynamics over timestep")
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience,
             py::arg("neuronId"), py::arg("salience"),
             "Apply bottom-up salience to neuron")
        .def("applyTopDownAttention", &AttentionalSelection::applyTopDownAttention,
             py::arg("neuronId"), py::arg("attention"),
             "Apply top-down attention to neuron")
        .def("processCompetition", &AttentionalSelection::processCompetition,
             py::arg("competitors"), "Process competitive attention")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"),
             "Set inhibition strength between competitors")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"),
             "Set excitation strength between competitors")
        .def("getSelectedNeuron", &AttentionalSelection::getSelectedNeuron,
             "Get currently selected neuron")
        .def("reset", &AttentionalSelection::reset,
             "Reset attention system");

    // Phase 6 Development System
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Developmental system affecting plasticity)pbdoc")
        .def("initialize", &DevelopmentSystem::initialize, py::arg("brain"),
             "Initialize development system with brain reference")
        .def("update", &DevelopmentSystem::update, py::arg("brain"), py::arg("rng"), py::arg("dt"),
             "Update developmental state over time")
        .def("getDevelopmentalStage", &DevelopmentSystem::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &DevelopmentSystem::setDevelopmentalStage, py::arg("stage"),
             "Set developmental stage")
        .def("getAge", &DevelopmentSystem::getAge,
             "Get current developmental age")
        .def("getPlasticityRate", &DevelopmentSystem::getPlasticityRate,
             "Get current plasticity rate")
        .def("isInCriticalPeriod", &DevelopmentSystem::isInCriticalPeriod,
             "Check if in critical developmental period");

    // Phase 6 Prediction System
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system for forward modeling)pbdoc")
        .def("initialize", &PredictionSystem::initialize, py::arg("brain"),
             "Initialize prediction system with brain reference")
        .def("recordSensoryState", &PredictionSystem::recordSensoryState,
             py::arg("state"), py::arg("timestamp"),
             "Record sensory state for prediction")
        .def("generatePrediction", &PredictionSystem::generatePrediction,
             py::arg("timestamp"), "Generate prediction for future state")
        .def("updateWithObservation", &PredictionSystem::updateWithObservation,
             py::arg("observedState"), py::arg("timestamp"),
             "Update prediction with observed state")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             "Get current prediction error")
        .def("getPredictionConfidence", &PredictionSystem::getPredictionConfidence,
             "Get prediction confidence level")
        .def("setPredictionHorizon", &PredictionSystem::setPredictionHorizon, py::arg("horizon"),
             "Set prediction horizon (number of steps ahead)");

    // Phase 6 Experiment System
    py::class_<Phase6IntegratedExperiment>(m, "Phase6IntegratedExperiment", R"pbdoc(Phase 6 integration experiment runner)pbdoc")
        .def(py::init<>())
        .def("run", &Phase6IntegratedExperiment::run, py::arg("config"),
             "Run complete Phase 6 integration experiment")
        .def("verifyIntegration", &Phase6IntegratedExperiment::verifyIntegration,
             "Verify that all systems are properly integrated")
        .def("testMemoryIntegration", &Phase6IntegratedExperiment::testMemoryIntegration,
             "Test memory system integration")
        .def("testNeuromodulationIntegration", &Phase6IntegratedExperiment::testNeuromodulationIntegration,
             "Test neuromodulation system integration")
        .def("testCheckpointing", &Phase6IntegratedExperiment::testCheckpointing,
             "Test checkpoint save/load functionality")
        .def("testReplay", &Phase6IntegratedExperiment::testReplay,
             "Test memory replay functionality");

    // Phase 6 Experiment Configuration
    py::class_<Phase6Config>(m, "Phase6Config", R"pbdoc(Configuration for Phase 6 integration experiment)pbdoc")
        .def(py::init<>())
        .def_readwrite("maxSteps", &Phase6Config::maxSteps)
        .def_readwrite("neuronCount", &Phase6Config::neuronCount)
        .def_readwrite("regionCount", &Phase6Config::regionCount)
        .def_readwrite("connectionProbability", &Phase6Config::connectionProbability)
        .def_readwrite("enableCheckpointing", &Phase6Config::enableCheckpointing)
        .def_readwrite("enableReplay", &Phase6Config::enableReplay)
        .def_readwrite("enableDevelopment", &Phase6Config::enableDevelopment)
        .def_readwrite("checkpointPath", &Phase6Config::checkpointPath);

    // Phase 6 Integration Result
    py::class_<Phase6IntegrationResult>(m, "Phase6IntegrationResult", R"pbdoc(Result from Phase 6 integration experiment)pbdoc")
        .def(py::init<>())
        .def_readwrite("memoryWorkingMemoryIntegrated", &Phase6IntegrationResult::memoryWorkingMemoryIntegrated)
        .def_readwrite("memoryEpisodicMemoryIntegrated", &Phase6IntegrationResult::memoryEpisodicMemoryIntegrated)
        .def_readwrite("neuromodulationIntegrated", &Phase6IntegrationResult::neuromodulationIntegrated)
        .def_readwrite("predictionIntegrated", &Phase6IntegrationResult::predictionIntegrated)
        .def_readwrite("developmentIntegrated", &Phase6IntegrationResult::developmentIntegrated)
        .def_readwrite("checkpointingWorks", &Phase6IntegrationResult::checkpointingWorks)
        .def_readwrite("replayWorks", &Phase6IntegrationResult::replayWorks)
        .def_readwrite("totalReward", &Phase6IntegrationResult::totalReward)
        .def_readwrite("avgFiringRate", &Phase6IntegrationResult::avgFiringRate)
        .def_readwrite("avgSynapticWeight", &Phase6IntegrationResult::avgSynapticWeight)
        .def_readwrite("memoryEpisodesStored", &Phase6IntegrationResult::memoryEpisodesStored)
        .def_readwrite("noveltyLevel", &Phase6IntegrationResult::noveltyLevel)
        .def_readwrite("curiosityLevel", &Phase6IntegrationResult::curiosityLevel)
        .def_readwrite("dopamineLevel", &Phase6IntegrationResult::dopamineLevel)
        .def_readwrite("totalWallClockTime", &Phase6IntegrationResult::totalWallClockTime);

    // Factory functions for Phase 6 systems
    m.def("createNeuralWorkingMemory", []() -> std::unique_ptr<NeuralWorkingMemory> {
        return std::make_unique<NeuralWorkingMemory>();
    }, "Create a new neural working memory system");

    m.def("createNeuralEpisodicMemory", []() -> std::unique_ptr<NeuralEpisodicMemory> {
        return std::make_unique<NeuralEpisodicMemory>();
    }, "Create a new neural episodic memory system");

    m.def("createNeuralAssociativeMemory", []() -> std::unique_ptr<NeuralAssociativeMemory> {
        return std::make_unique<NeuralAssociativeMemory>();
    }, "Create a new neural associative memory system");

    m.def("createDopamine", []() -> std::unique_ptr<Dopamine> {
        return std::make_unique<Dopamine>();
    }, "Create a new dopamine neuromodulator");

    m.def("createNovelty", []() -> std::unique_ptr<Novelty> {
        return std::make_unique<Novelty>();
    }, "Create a new novelty detector");

    m.def("createCuriosity", []() -> std::unique_ptr<Curiosity> {
        return std::make_unique<Curiosity>();
    }, "Create a new curiosity system");

    m.def("createPredictionError", []() -> std::unique_ptr<PredictionError> {
        return std::make_unique<PredictionError>();
    }, "Create a new prediction error system");

    m.def("createNeuralPlanner", []() -> std::unique_ptr<NeuralPlanner> {
        return std::make_unique<NeuralPlanner>();
    }, "Create a new neural planner");

    m.def("createConceptFormation", []() -> std::unique_ptr<ConceptFormation> {
        return std::make_unique<ConceptFormation>();
    }, "Create a new concept formation system");

    m.def("createAttentionalSelection", []() -> std::unique_ptr<AttentionalSelection> {
        return std::make_unique<AttentionalSelection>();
    }, "Create a new attentional selection system");

    m.def("createDevelopmentSystem", []() -> std::unique_ptr<DevelopmentSystem> {
        return std::make_unique<DevelopmentSystem>();
    }, "Create a new development system");

    m.def("createPredictionSystem", []() -> std::unique_ptr<PredictionSystem> {
        return std::make_unique<PredictionSystem>();
    }, "Create a new prediction system");

    m.def("createPhase6Experiment", []() -> std::unique_ptr<Phase6IntegratedExperiment> {
        return std::make_unique<Phase6IntegratedExperiment>();
    }, "Create a new Phase 6 integration experiment runner");

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
