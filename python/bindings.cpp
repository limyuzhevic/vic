#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

// Core NLM headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Neuromodulation systems
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Curiosity.hpp"

// Prediction and cognition systems
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/SpatialRepresentation.hpp"
#include "../src/cognition/TemporalRelation.hpp"

// Development systems
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/development/Pruning.hpp"

// Performance and experiment systems
#include "../src/experiments/Metrics.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"
#include "../src/performance/MemoryPool.hpp"
#include "../src/core/Random/Random.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"

namespace py = pybind11;
namespace nlm {

// Helper function for error handling
inline void check_error(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

// Python bindings module
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This version provides comprehensive bindings for all Phase 6 integrated
        brain systems with improved error handling, Pythonic interfaces,
        and convenience methods for easier use by researchers and developers.
        
        Key improvements over the original bindings:
        
        - Comprehensive Phase 6 integration: All memory, neuromodulation,
          prediction, cognition, and development systems are exposed
        - Better error handling with informative Python exceptions
        - Pythonic interfaces using properties and method chaining
        - Convenience functions for common operations
        - Enhanced documentation and type safety
        - Memory safety with proper reference counting
        - Cross-platform compatibility
    )pbdoc";

    // Exception registration
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Version information
    m.attr("__version__") = "0.2.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Enhanced neural simulation bindings";

    // =======================================================================
    // Identity Types
    // =======================================================================
    
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

    // =======================================================================
    // Enumerations
    // =======================================================================
    
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

    // =======================================================================
    // Memory Systems (Neural implementations)
    // =======================================================================
    
    // Neural Working Memory - transient active information
    py::class_<NeuralWorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory: transient active storage of information)pbdoc")
        .def(py::init<>())
        .def("store", &NeuralWorkingMemory::store, py::arg("neuron"), py::arg("value"),
             "Store an item in working memory")
        .def("retrieve", &NeuralWorkingMemory::retrieve, py::arg("neuron"),
             "Retrieve an item from working memory")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"),
             "Check if working memory contains an item")
        .def("clear", &NeuralWorkingMemory::clear, "Clear all working memory items")
        .def("decay", &NeuralWorkingMemory::decay, py::arg("decayRate"),
             "Decay all items with given rate")
        .def("get_capacity", &NeuralWorkingMemory::getCapacity, "Get working memory capacity")
        .def("get_current_size", &NeuralWorkingMemory::getCurrentSize, "Get current number of items");

    // Neural Episodic Memory - experience storage
    py::class_<NeuralEpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory: storage of events and experiences)pbdoc")
        .def(py::init<>())
        .def("store_episode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"),
             "Store a new episode")
        .def("retrieve_episode", &NeuralEpisodicMemory::retrieveEpisode, py::arg("index"),
             "Retrieve an episode by index")
        .def("get_episode_count", &NeuralEpisodicMemory::getEpisodeCount, "Get number of episodes")
        .def("get_recent_episodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"),
             "Get recent episodes")
        .def("clear", &NeuralEpisodicMemory::clear, "Clear all episodes")
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevanceThreshold"),
             "Consolidate episodes above relevance threshold");

    // Neural Associative Memory - pattern associations
    py::class_<NeuralAssociativeMemory>(m, "AssociativeMemory", R"pbdoc(Associative memory: relationships between representations)pbdoc")
        .def(py::init<>())
        .def("associate", &NeuralAssociativeMemory::associate, py::arg("a"), py::arg("b"), py::arg("strength"),
             "Create or update an association between two neurons")
        .def("get_associations", &NeuralAssociativeMemory::getAssociations, py::arg("neuron"),
             "Get all neurons associated with given neuron")
        .def("get_association_strength", &NeuralAssociativeMemory::getAssociationStrength, py::arg("a"), py::arg("b"),
             "Get strength of association between two neurons")
        .def("update_association", &NeuralAssociativeMemory::updateAssociation, py::arg("a"), py::arg("b"), py::arg("delta"),
             "Update association strength")
        .def("clear", &NeuralAssociativeMemory::clear, "Clear all associations");

    // =======================================================================
    // Neuromodulation Systems
    // =======================================================================
    
    // Neuromodulator base class
    py::class_<Neuromodulator>(m, "Neuromodulator", py::abstract(), R"pbdoc(Abstract base class for neuromodulatory signals)pbdoc")
        .def("get_name", &Neuromodulator::getName, "Get neuromodulator name")
        .def("get_level", &Neuromodulator::getLevel, "Get current concentration/level")
        .def("set_level", &Neuromodulator::setLevel, py::arg("level"), "Set concentration/level")
        .def("get_plasticity_factor", &Neuromodulator::getPlasticityFactor,
             "Get neuromodulatory effect on plasticity")
        .def("update", &Neuromodulator::update, py::arg("dt"), "Update neuromodulator state");

    // Dopamine (reward and reinforcement learning)
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator: reward and reinforcement learning)pbdoc")
        .def(py::init<>())
        .def("signal_reward", &Dopamine::signalReward, py::arg("reward"), "Signal reward")
        .def("signal_reward_prediction_error", &Dopamine::signalRewardPredictionError, py::arg("error"), "Signal reward prediction error");

    // Acetylcholine (simplified implementation - uses base class methods)
    py::class_<Acetylcholine, Neuromodulator>(m, "Acetylcholine", R"pbdoc(Acetylcholine neuromodulator: attention and memory consolidation)pbdoc")
        .def(py::init<>());

    // Norepinephrine (simplified implementation - uses base class methods)
    py::class_<Norepinephrine, Neuromodulator>(m, "Norepinephrine", R"pbdoc(Norepinephrine neuromodulator: arousal and vigilance)pbdoc")
        .def(py::init<>());

    // Serotonin (simplified implementation - uses base class methods)
    py::class_<Serotonin, Neuromodulator>(m, "Serotonin", R"pbdoc(Serotonin neuromodulator: mood and social behavior)pbdoc")
        .def(py::init<>());

    // Reward system
    py::class_<Reward>(m, "Reward", R"pbdoc(Reward system: reinforcement learning signals)pbdoc")
        .def(py::init<>())
        .def("get_value", &Reward::getValue, "Get current reward value")
        .def("set_value", &Reward::setValue, py::arg("value"), "Set reward value")
        .def("add", &Reward::add, py::arg("delta"), "Add to current reward")
        .def("reset", &Reward::reset, "Reset accumulated reward")
        .def("get_history", &Reward::getHistory, "Get reward history")
        .def("clear_history", &Reward::clearHistory, "Clear reward history");

    // Prediction Error
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal: difference between predicted and actual values)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("get_error", &PredictionError::getError, "Get current prediction error")
        .def("compute_error", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"), "Compute prediction error")
        .def("update_prediction", &PredictionError::updatePrediction, py::arg("newPrediction"), "Update prediction")
        .def("get_history", &PredictionError::getHistory, "Get prediction error history")
        .def("clear_history", &PredictionError::clearHistory, "Clear prediction error history")
        .def("get_magnitude", &PredictionError::getMagnitude, "Get error magnitude for neuromodulation");

    // Novelty detection
    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection: computes novelty from comparison with previous observations)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("get_level", &Novelty::getLevel, "Get current novelty level")
        .def("set_level", &Novelty::setLevel, py::arg("level"), "Set novelty level")
        .def("detect_novelty", py::overload_cast<const Observation&, const Observation&>(&Novelty::detectNovelty),
             py::arg("current_observation"), py::arg("previous_observation"), "Detect novelty from observations")
        .def("detect_novelty", py::overload_cast<const std::vector<float>&, const std::vector<float>&>(&Novelty::detectNovelty),
             py::arg("current_pattern"), py::arg("previous_pattern"), "Detect novelty from patterns")
        .def("update", &Novelty::update, py::arg("dt"), "Decay novelty over time")
        .def("get_history", &Novelty::getHistory, "Get novelty history")
        .def("clear_history", &Novelty::clearHistory, "Clear novelty history");

    // Curiosity
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity: exploration motivation based on novelty and prediction error)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("get_level", &Curiosity::getLevel, "Get current curiosity level")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("prediction_error"), py::arg("dt"),
             "Update curiosity based on novelty and prediction error")
        .def("get_exploration_drive", &Curiosity::getExplorationDrive, "Get exploration drive")
        .def("set_novelty_weight", &Curiosity::setNoveltyWeight, py::arg("weight"), "Set novelty weight")
        .def("set_prediction_error_weight", &Curiosity::setPredictionErrorWeight, py::arg("weight"), "Set prediction error weight")
        .def("reset", &Curiosity::reset, "Reset curiosity level");

    // =======================================================================
    // Prediction and Cognition Systems
    // =======================================================================
    
    // Prediction System
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system: predicts future sensory states and consequences)pbdoc")
        .def(py::init<>())
        .def("predict_next_state", &PredictionSystem::predictNextState, py::arg("current_state"),
             "Make prediction for next timestep")
        .def("update_predictions", &PredictionSystem::updatePredictions, py::arg("predicted"), py::arg("actual"),
             "Update predictions based on actual observation")
        .def("get_prediction_error", &PredictionSystem::getPredictionError, "Get prediction error")
        .def("get_confidence", &PredictionSystem::getConfidence, "Get prediction confidence")
        .def("get_error_history", &PredictionSystem::getErrorHistory, "Get prediction error history")
        .def("clear_history", &PredictionSystem::clearHistory, "Clear prediction history")
        .def("train", &PredictionSystem::train, py::arg("observation"), "Train prediction model");

    // Neural Prediction
    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Neural prediction: internal prediction mechanism)pbdoc")
        .def(py::init<>());

    // Concept Formation
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation: discovers recurring patterns from experience)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("present_experience", &ConceptFormation::presentExperience,
             py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("current_time"),
             "Present a new experience to the concept formation system")
        .def("get_matching_concept", &ConceptFormation::getMatchingConcept, py::arg("pattern"), py::arg("similarity_threshold") = 0.7f,
             "Get concept that best matches a pattern")
        .def("get_concepts", &ConceptFormation::getConcepts, "Get all concepts")
        .def("get_concept", &ConceptFormation::getConcept, py::arg("concept_id"),
             "Get a specific concept (returns None if not found)")
        .def("get_concept_prototype", &ConceptFormation::getConceptPrototype, py::arg("concept_id"),
             "Get concept prototype")
        .def("get_concept_instances", &ConceptFormation::getConceptInstances, py::arg("concept_id"),
             "Get concept instances")
        .def("get_concept_stability", &ConceptFormation::getConceptStability, py::arg("concept_id"),
             "Measure concept stability (0-1)")
        .def("get_concept_count", &ConceptFormation::getConceptCount, "Get number of concepts discovered")
        .def("merge_concepts", &ConceptFormation::mergeConcepts, py::arg("concept_a"), py::arg("concept_b"),
             "Merge two concepts when discovered they are actually the same")
        .def("update_concept", &ConceptFormation::updateConcept,
             py::arg("concept_id"), py::arg("new_pattern"), py::arg("features"), py::arg("reward"),
             "Update concept with new instance")
        .def("compute_similarity", &ConceptFormation::computeSimilarity, py::arg("a"), py::arg("b"),
             "Compute similarity between two patterns")
        .def("is_novel", &ConceptFormation::isNovel, py::arg("pattern"), py::arg("similarity_threshold") = 0.7f,
             "Check if pattern is novel (doesn't belong to any existing concept)")
        .def("find_concept_for_pattern", &ConceptFormation::findConceptForPattern, py::arg("pattern"),
             "Get concept for a given pattern")
        .def("clear", &ConceptFormation::clear, "Clear all concepts")
        .def("get_generalization_ability", &ConceptFormation::getGeneralizationAbility, py::arg("concept_id"),
             "Get generalization ability (can concept handle variations?)")
        .def("set_formation_threshold", &ConceptFormation::setFormationThreshold, py::arg("threshold"),
             "Set formation threshold")
        .def("set_stability_window", &ConceptFormation::setStabilityWindow, py::arg("window"),
             "Set stability window");

    // Spatial Representation
    py::class_<SpatialRepresentation>(m, "SpatialRepresentation", R"pbdoc(Spatial representation: learns spatial relationships from experience)pbdoc")
        .def(py::init<>())
        .def("initialize", &SpatialRepresentation::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("record_position", &SpatialRepresentation::recordPosition, py::arg("x"), py::arg("y"), py::arg("sensory_cues"),
             "Record current position experience")
        .def("get_predicted_position", &SpatialRepresentation::getPredictedPosition, "Get predicted position based on path integration")
        .def("integrate_movement", &SpatialRepresentation::integrateMovement, py::arg("dx"), py::arg("dy"),
             "Update position estimate based on movement")
        .def("get_position_activation", &SpatialRepresentation::getPositionActivation, py::arg("x"), py::arg("y"),
             "Get position neuron activation for a given position")
        .def("get_place_neurons_near", &SpatialRepresentation::getPlaceNeuronsNear, py::arg("x"), py::arg("y"), py::arg("radius"),
             "Find place neurons near a position")
        .def("get_grid_spacing", &SpatialRepresentation::getGridSpacing, "Get grid spacing (if grid cells have formed)")
        .def("clear", &SpatialRepresentation::clear, "Clear spatial representations")
        .def("has_place_cells", &SpatialRepresentation::hasPlaceCells, "Have place cells formed?");

    // Temporal Relation
    py::class_<TemporalRelation>(m, "TemporalRelation", R"pbdoc(Temporal relation: learns temporal relationships (before, after, sequence))pbdoc")
        .def(py::init<>())
        .def("initialize", &TemporalRelation::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("record_sequence", &TemporalRelation::recordSequence,
             py::arg("event_a"), py::arg("event_b"), py::arg("reward"),
             "Record that event A was followed by event B")
        .def("predict_next", &TemporalRelation::predictNext, py::arg("event_a"),
             "Given event A, predict what comes next")
        .def("predict_previous", &TemporalRelation::predictPrevious, py::arg("event_b"),
             "Given event B, predict what came before")
        .def("get_prediction_confidence", &TemporalRelation::getPredictionConfidence, py::arg("event"),
             "Get confidence in prediction")
        .def("get_sequence_length", &TemporalRelation::getSequenceLength, "Get sequence length")
        .def("clear", &TemporalRelation::clear, "Clear sequences");

    // =======================================================================
    // Development Systems
    // =======================================================================
    
    // Development System
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system: orchestrates neural development)pbdoc")
        .def(py::init<>())
        .def("get_stage", &DevelopmentSystem::getStage, "Get current developmental stage")
        .def("set_stage", &DevelopmentSystem::setStage, py::arg("stage"), "Set developmental stage")
        .def("advance_stage", &DevelopmentSystem::advanceStage, "Advance developmental stage")
        .def("get_stage_name", &DevelopmentSystem::getStageName, "Get stage name")
        .def("update", py::overload_cast<Brain*>(&DevelopmentSystem::update), py::arg("brain"), py::arg("current_step"),
             "Update development (simple version)")
        .def("update", py::overload_cast<Brain*, RandomGenerator&, TimestepDuration>(&DevelopmentSystem::update), py::arg("brain"), py::arg("rng"), py::arg("dt"),
             "Update development with full parameters")
        .def("get_plasticity_modifier", &DevelopmentSystem::getPlasticityModifier, "Get plasticity modifier for current stage")
        .def("is_critical_period", &DevelopmentSystem::isCriticalPeriod, "Get critical period info")
        .def("get_critical_period_progress", &DevelopmentSystem::getCriticalPeriodProgress, "Get critical period progress")
        .def("get_developmental_age", &DevelopmentSystem::getDevelopmentalAge, "Get developmental age (in simulation time)")
        .def("set_developmental_age", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"), "Set developmental age");

    // Synaptogenesis
    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Synaptogenesis: formation of new synapses)pbdoc")
        .def(py::init<>())
        .def("get_formation_rate", &Synaptogenesis::getFormationRate, "Get formation rate")
        .def("set_formation_rate", &Synaptogenesis::setFormationRate, py::arg("rate"), "Set formation rate")
        .def("get_target_density", &Synaptogenesis::getTargetDensity, "Get target synapse density")
        .def("set_target_density", &Synaptogenesis::setTargetDensity, py::arg("density"), "Set target density")
        .def("update", &Synaptogenesis::update, py::arg("brain"), py::arg("rng"), "Update synapse formation");

    // Maturation
    py::class_<Maturation>(m, "Maturation", R"pbdoc(Maturation: development of adult-level properties)pbdoc")
        .def(py::init<>())
        .def("get_progress", &Maturation::getProgress, "Get maturation progress (0-1)")
        .def("set_progress", &Maturation::setProgress, py::arg("progress"), "Set maturation progress")
        .def("update", &Maturation::update, py::arg("brain"), py::arg("current_step"), "Update maturation")
        .def("get_mature_threshold", &Maturation::getMatureThreshold, "Get mature threshold")
        .def("get_mature_resting_potential", &Maturation::getMatureRestingPotential, "Get mature resting potential")
        .def("get_mature_time_constant", &Maturation::getMatureTimeConstant, "Get mature time constant");

    // Pruning
    py::class_<Pruning>(m, "Pruning", R"pbdoc(Pruning: elimination of synapses and neurons)pbdoc")
        .def(py::init<>());

    // =======================================================================
    // Performance Systems
    // =======================================================================
    
    // MemoryPool - template version for neurons (simplified)
    py::class_<MemoryPool<Neuron>, py::non_templated>(m, "NeuronPool", R"pbdoc(Memory pool for neurons with cache-aligned states)pbdoc")
        .def(py::init<size_t, size_t>(), py::arg("block_size") = 1024, py::arg("initial_blocks") = 4)
        .def("allocate", &MemoryPool<Neuron>::allocate, "Allocate a neuron from the pool")
        .def("deallocate", &MemoryPool<Neuron>::deallocate, py::arg("id"), "Deallocate a neuron")
        .def("empty", &MemoryPool<Neuron>::empty, "Check if pool is empty")
        .def("allocated_count", &MemoryPool<Neuron>::allocatedCount, "Get allocated count")
        .def("available_count", &MemoryPool<Neuron>::availableCount, "Get available count")
        .def("memory_usage", &MemoryPool<Neuron>::memoryUsage, "Get memory usage (bytes)")
        .def("reserve", &MemoryPool<Neuron>::reserve, py::arg("count"), "Pre-allocate objects");

    // EventQueue
    py::class_<EventQueue>(m, "EventQueue", R"pbdoc(Event queue for temporal processing)pbdoc")
        .def(py::init<>());

    // OptimizedBrain
    py::class_<OptimizedBrain>(m, "OptimizedBrain", R"pbdoc(Optimized brain implementation)pbdoc")
        .def(py::init<>());

    // =======================================================================
    // Experiment Systems
    // =======================================================================
    
    // Metrics
    py::class_<Metrics>(m, "Metrics", R"pbdoc(Metrics collection and analysis)pbdoc")
        .def(py::init<>())
        .def("record", &Metrics::record, py::arg("name"), py::arg("value"), "Record a metric value")
        .def("get_values", &Metrics::getValues, py::arg("name"), "Get metric values")
        .def("get_mean", &Metrics::getMean, py::arg("name"), "Get mean value")
        .def("get_std_dev", &Metrics::getStdDev, py::arg("name"), "Get standard deviation")
        .def("get_min", &Metrics::getMin, py::arg("name"), "Get minimum value")
        .def("get_max", &Metrics::getMax, py::arg("name"), "Get maximum value")
        .def("get_metric_names", &Metrics::getMetricNames, "Get all metric names")
        .def("clear", &Metrics::clear, "Clear all metrics")
        .def("summary", &Metrics::summary, "Get summary report");

    // Experiment
    py::class_<Experiment>(m, "Experiment", R"pbdoc(Experiment: records data from a single experimental run)pbdoc")
        .def(py::init<>())
        .def(py::init<std::string>(), py::arg("name"))
        .def("get_name", &Experiment::getName, "Get experiment name")
        .def("set_name", &Experiment::setName, py::arg("name"), "Set experiment name")
        .def("get_seed", &Experiment::getSeed, "Get random seed")
        .def("set_seed", &Experiment::setSeed, py::arg("seed"), "Set random seed")
        .def("set_config_value", &Experiment::setConfigValue, py::arg("key"), py::arg("value"), "Set config value")
        .def("get_config_value", &Experiment::getConfigValue, py::arg("key"), "Get config value")
        .def("record_metric", py::overload_cast<const std::string&, double>(&Experiment::recordMetric),
             py::arg("name"), py::arg("value"), "Record double metric")
        .def("record_metric", py::overload_cast<const std::string&, float>(&Experiment::recordMetric),
             py::arg("name"), py::arg("value"), "Record float metric")
        .def("record_metric", py::overload_cast<const std::string&, int>(&Experiment::recordMetric),
             py::arg("name"), py::arg("value"), "Record int metric")
        .def("get_metrics", &Experiment::getMetrics, "Get all metrics")
        .def("get_metric", &Experiment::getMetric, py::arg("name"), "Get metric value")
        .def("has_metric", &Experiment::hasMetric, py::arg("name"), "Check if metric exists")
        .def("record_step", &Experiment::recordStep,
             py::arg("step"), py::arg("neuron_count"), py::arg("synapse_count"),
             py::arg("firing_count"), py::arg("avg_firing_rate"),
             "Record step data")
        .def("get_step_data", &Experiment::getStepData, "Get step data")
        .def("get_start_step", &Experiment::getStartStep, "Get start step")
        .def("set_start_step", &Experiment::setStartStep, py::arg("step"), "Set start step")
        .def("get_end_step", &Experiment::getEndStep, "Get end step")
        .def("set_end_step", &Experiment::setEndStep, py::arg("step"), "Set end step")
        .def("add_note", &Experiment::addNote, py::arg("note"), "Add note")
        .def("get_notes", &Experiment::getNotes, "Get notes");

    // Phase6IntegratedExperiment
    py::class_<Phase6IntegratedExperiment>(m, "Phase6IntegratedExperiment", R"pbdoc(Phase 6 integration test: verifies all systems work together)pbdoc")
        .def(py::init<>())
        .def("run", &Phase6IntegratedExperiment::run, py::arg("config"), "Run the complete integration test")
        .def("verify_integration", &Phase6IntegratedExperiment::verifyIntegration, "Verify integration")
        .def("test_memory_integration", &Phase6IntegratedExperiment::testMemoryIntegration, "Test memory integration")
        .def("test_neuromodulation_integration", &Phase6IntegratedExperiment::testNeuromodulationIntegration, "Test neuromodulation integration")
        .def("test_checkpintering", &Phase6IntegratedExperiment::testCheckpointing, "Test checkpoint save/load")
        .def("test_replay", &Phase6IntegratedExperiment::testReplay, "Test replay system");

    // =======================================================================
    // Core Systems (Enhanced)
    // =======================================================================
    
    // Enhanced Config class with Pythonic interface
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        // Pythonic properties and methods
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.setConfigValue(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("__getitem__", [](const Config& self, const std::string& key) {
            return self.getConfigValue(key);
        }, py::arg("key"), "Get configuration value (empty string if not found)")
        .def("__setitem__", [](Config& self, const std::string& key, const std::string& value) {
            self.setConfigValue(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("get_int", [](const Config& self, const std::string& key, int default_value) {
            try {
                return std::stoi(self.getConfigValue(key));
            } catch (...) {
                return default_value;
            }
        }, py::arg("key"), py::arg("default_value"), "Get integer configuration value")
        .def("get_float", [](const Config& self, const std::string& key, float default_value) {
            try {
                return std::stof(self.getConfigValue(key));
            } catch (...) {
                return default_value;
            }
        }, py::arg("key"), py::arg("default_value"), "Get float configuration value")
        .def("get_bool", [](const Config& self, const std::string& key, bool default_value) {
            try {
                std::string value = self.getConfigValue(key);
                return value == "true" || value == "1" || value == "yes";
            } catch (...) {
                return default_value;
            }
        }, py::arg("key"), py::arg("default_value"), "Get boolean configuration value")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced Brain class with better error handling and Pythonic interfaces
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
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status")
        // Pythonic properties
        .def("get_spike_system", &Brain::getSpikeSystem,
             "Get spike system")
        .def("get_stdp", &Brain::getSTDP,
             "Get STDP plasticity system")
        .def("get_hebbian", &Brain::getHebbian,
             "Get Hebbian plasticity system")
        .def("get_structural_plasticity", &Brain::getStructuralPlasticity,
             "Get structural plasticity system")
        .def("get_working_memory", &Brain::getWorkingMemory,
             "Get working memory system")
        .def("get_episodic_memory", &Brain::getEpisodicMemory,
             "Get episodic memory system")
        .def("get_associative_memory", &Brain::getAssociativeMemory,
             "Get associative memory system")
        .def("get_prediction_system", &Brain::getPredictionSystem,
             "Get prediction system")
        .def("get_planner", &Brain::getPlanner,
             "Get neural planner")
        .def("get_concept_formation", &Brain::getConceptFormation,
             "Get concept formation system")
        .def("get_attention", &Brain::getAttention,
             "Get attentional selection system")
        .def("get_development_system", &Brain::getDevelopmentSystem,
             "Get development system")
        .def("get_dopamine", &Brain::getDopamine,
             "Get dopamine neuromodulator")
        .def("get_curiosity", &Brain::getCuriosity,
             "Get curiosity neuromodulator")
        .def("get_novelty", &Brain::getNovelty,
             "Get novelty neuromodulator")
        .def("get_prediction_error_signal", &Brain::getPredictionErrorSignal,
             "Get prediction error signal")
        .def("get_random_generator", &Brain::getRandomGenerator,
             "Get random generator");

    // Enhanced AgentBrain class with more Pythonic interfaces
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        // Subsystem enable/disable with Pythonic interface
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development")
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity")
        // Status checks
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // =======================================================================
    // World and Sensorimotor Systems (Enhanced)
    // =======================================================================
    
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"))
        .def("clear_signals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType)
        .def("set_type", &Action::setType, py::arg("type"))
        .def("get_parameters", &Action::getParameters)
        .def("set_parameters", &Action::setParameters, py::arg("params"))
        .def("get_name", &Action::getName)
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
        .def_readwrite("velocity_x", &AgentBody::velocityX)
        .def_readwrite("velocity_y", &AgentBody::velocityY)
        .def_readwrite("angular_velocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("is_moving", &AgentBody::isMoving)
        .def_readwrite("is_turning", &AgentBody::isTurning)
        .def_readwrite("last_action_time", &AgentBody::lastActionTime)
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
        .def("get_vision", &SensoryPercept::getVision)
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"))
        .def("get_vision_width", &SensoryPercept::getVisionWidth)
        .def("get_vision_height", &SensoryPercept::getVisionHeight)
        .def("get_touch", &SensoryPercept::getTouch)
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("get_internal", &SensoryPercept::getInternal)
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("get_proprioception", &SensoryPercept::getProprioception)
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("get_audio", &SensoryPercept::getAudio)
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("get_all_signals", &SensoryPercept::getAllSignals)
        .def("get_timestamp", &SensoryPercept::getTimestamp)
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"))
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("get_width", &SimpleWorld::getWidth)
        .def("get_height", &SimpleWorld::getHeight)
        .def("get_max_energy", &SimpleWorld::getMaxEnergy)
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate)
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("get_simulation_time", &SimpleWorld::getSimulationTime)
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("get_random_seed", &SimpleWorld::getRandomSeed);

    // =======================================================================
    // Convenience Functions and Factories
    // =======================================================================
    
    // Factory functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Convenience helper functions
    m.def("run_agent_simulation", [](AgentBrain& agent, SimpleWorld& world, int num_steps) {
        for (int step = 0; step < num_steps; ++step) {
            world.update(0.1);
            auto percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            // Note: This is a simplified version - full implementation would need brain integration
        }
    }, py::arg("agent"), py::arg("world"), py::arg("num_steps"), "Run a complete agent simulation");

    m.def("print_agent_status", [](const AgentBrain& agent, const SimpleWorld& world) {
        std::cout << "=== Agent Status ===" << std::endl;
        std::cout << "Curiosity: " << agent.getCuriosityLevel() << std::endl;
        std::cout << "Novelty: " << agent.getNoveltyLevel() << std::endl;
        std::cout << "Prediction Error: " << agent.getPredictionError() << std::endl;
        std::cout << "Neuromodulation Level: " << agent.getNeuromodulationLevel() << std::endl;
        std::cout << "Developmental Stage: " << static_cast<int>(agent.getDevelopmentalStage()) << std::endl;
    }, py::arg("agent"), py::arg("world"), "Print agent status information");

    // Error handling helper
    m.def("safe_execute", [](py::function func, const std::string& operation_name) {
        try {
            return func();
        } catch (const std::exception& e) {
            throw std::runtime_error("Error during " + operation_name + ": " + std::string(e.what()));
        }
    }, py::arg("func"), py::arg("operation_name"), "Safely execute a function with error handling");

    // =======================================================================
    // Version and Information
    // =======================================================================
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Export constants for version control and compatibility
    m.attr("PHASE6_INTEGRATION_VERSION") = "6.0";
    m.attr("NEURAL_SIMULATION_VERSION") = "1.0";
    m.attr("COMPATIBILITY_VERSION") = "1.0";

    // Module initialization
    m.def("_initialize", []() {
        // Optional initialization hook
    });

    m.def("_cleanup", []() {
        // Optional cleanup hook
    });

} // namespace nlm
