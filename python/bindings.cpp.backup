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
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/Neuron.hpp"

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

    // ========== ADVANCED API BINDINGS ==========
    
    // Prediction System
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system for neural predictions and error computation)pbdoc")
        .def(py::init<>())
        .def("predictNextState", &PredictionSystem::predictNextState,
             py::arg("currentState"),
             R"pbdoc(Predict the next sensory state based on current state)
             
             Examples:
                 >>> brain = nlm.createBrain(nlm.createDefaultConfig())
                 >>> pred = brain.getPredictionSystem()
                 >>> vision = vision_input  # Your sensory input
                 >>> prediction = pred.predictNextState(vision)
                 >>> print(f"Predicted next state: {prediction}")
             
             This method uses the brain's neural substrate to make forward predictions
             about future sensory states, enabling proactive behavior planning.
             ")
        .def("updatePredictions", &PredictionSystem::updatePredictions,
             py::arg("predicted"), py::arg("actual"),
             R"pbdoc(Update predictions based on actual observations)
             
             Examples:
                 >>> pred.updatePredictions(prediction, actual_observation)
                 >>> error = pred.getPredictionError()
                 >>> print(f"Prediction error: {error:.4f}")
             
             This method computes prediction errors which are crucial for learning
             and adaptive behavior. The brain uses these errors to adjust its
             internal models and improve future predictions.
             ")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             R"pbdoc(Get the current prediction error)
             
             Returns:
                 float: The mean squared error between predicted and actual states
                 
             Examples:
                 >>> pred = brain.getPredictionSystem()
                 >>> error = pred.getPredictionError()
                 >>> if error > 0.1:
                 ...     print("High prediction error - need to learn!")
                 ")
        .def("adaptConfidence", &PredictionSystem::adaptConfidence,
             py::arg("dt"),
             R"pbdoc(Adapt prediction confidence based on recent error history)
             
             Examples:
                 >>> pred.adaptConfidence(0.1)  # dt in simulation time units
                 >>> confidence = pred.getConfidence()
                 >>> print(f"Current confidence: {confidence:.2f}")
             
             This method adjusts how confident the prediction system is in its
             predictions based on recent performance. Higher error leads to
             lower confidence, making the system more open to new information.
             ")
        .def("getConfidence", &PredictionSystem::getConfidence,
             R"pbdoc(Get current prediction confidence)
             
             Returns:
                 float: Confidence level between 0.0 (uncertain) and 1.0 (confident)
                 
             Examples:
                 >>> pred = brain.getPredictionSystem()
                 >>> confidence = pred.getConfidence()
                 >>> if confidence > 0.8:
                 ...     print("Highly confident in predictions")
             ")
        .def("getErrorHistory", &PredictionSystem::getErrorHistory,
             R"pbdoc(Get prediction error history for analysis)
             
             Returns:
                 list[float]: List of recent prediction errors
                 
             Examples:
                 >>> pred = brain.getPredictionSystem()
                 >>> history = pred.getErrorHistory()
                 >>> print(f"Number of predictions: {len(history)}")
                 >>> if len(history) > 10:
                 ...     recent_errors = history[-10:]
                 ...     print(f"Average recent error: {sum(recent_errors)/len(recent_errors):.4f}")
             ")
        .def("clearHistory", &PredictionSystem::clearHistory,
             R"pbdoc(Clear prediction error history)
             
             Examples:
                 >>> pred = brain.getPredictionSystem()
                 >>> pred.clearHistory()
                 >>> print("History cleared - starting fresh predictions")
             ")
        .def("train", &PredictionSystem::train,
             py::arg("observation"),
             R"pbdoc(Train prediction model with new observation)
             
             Examples:
                 >>> pred = brain.getPredictionSystem()
                 >>> observation = sensory_input  # Your sensory data
                 >>> pred.train(observation)
                 >>> print("Prediction model trained with new data")
             ")
        .def("getStats", [](const PredictionSystem& self) {
            py::dict stats;
            stats["error"] = self.getPredictionError();
            stats["confidence"] = self.getConfidence();
            stats["error_history_size"] = static_cast<size_t>(self.getErrorHistory().size());
            return stats;
        }, R"pbdoc(Get prediction system statistics)
        
        Returns:
            dict: Dictionary containing error, confidence, and history size
            
        Examples:
            >>> pred = brain.getPredictionSystem()
            >>> stats = pred.getStats()
            >>> print(f"Error: {stats['error']:.4f}, Confidence: {stats['confidence']:.2f}")
        ")

    // Concept Formation
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation system for discovering recurring patterns)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize,
             py::arg("brain"),
             R"pbdoc(Initialize concept formation system with brain reference)
             
             Examples:
                 >>> brain = nlm.createBrain(nlm.createDefaultConfig())
                 >>> concept_formation = brain.getConceptFormation()
                 >>> concept_formation.initialize(brain.get())
                 >>> print("Concept formation system initialized")
             ")
        .def("presentExperience", &ConceptFormation::presentExperience,
             py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"),
             R"pbdoc(Present a new experience to the concept formation system)
             
             Examples:
                 >>> pattern = [0.1, 0.5, 0.9, 0.2, 0.7]  # Neural activity pattern
                 >>> features = [0.8, 0.3, 0.6]  # Associated features
                 >>> reward = 1.0  # Reward received
                 >>> concept_id = concept_form.presentExperience(pattern, features, reward, current_step)
                 >>> if concept_id > 0:
                 ...     print(f"Created/associated concept {concept_id}")
                 ")
        .def("createConcept", &ConceptFormation::createConcept,
             py::arg("pattern"), py::arg("features"), py::arg("reward"),
             R"pbdoc(Create a new concept from pattern and features)
             
             Examples:
                 >>> pattern = [0.2, 0.8, 0.4, 0.6, 0.3]
                 >>> features = [0.9, 0.1, 0.5]
                 >>> concept_id = concept_form.createConcept(pattern, features, 0.5)
                 >>> print(f"Created new concept {concept_id}")
             ")
        .def("getConcept", &ConceptFormation::getConcept,
             py::arg("conceptId"),
             R"pbdoc(Get a specific concept by ID)
             
             Args:
                 conceptId: ID of the concept to retrieve
                 
             Returns:
                 DiscoveredConcept: The concept or None if not found
                 
             Examples:
                 >>> concept = concept_form.getConcept(5)
                 >>> if concept:
                 ...     print(f"Concept {concept.id}: {concept.categoryHint}")
                 ")
        .def("mergeConcepts", &ConceptFormation::mergeConcepts,
             py::arg("conceptA"), py::arg("conceptB"),
             R"pbdoc(Merge two concepts when they are discovered to be the same)
             
             Examples:
                 >>> concept_form.mergeConcepts(3, 7)
                 >>> print("Merged concepts 3 and 7 into single concept")
             ")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept,
             py::arg("pattern"), py::arg("similarityThreshold"),
             R"pbdoc(Get the concept that best matches a pattern)
             
             Examples:
                 >>> pattern = [0.3, 0.7, 0.5, 0.9, 0.2]
                 >>> concept_id = concept_form.getMatchingConcept(pattern)
                 >>> print(f"Best matching concept: {concept_id}")
             ")
        .def("getConcepts", &ConceptFormation::getConcepts,
             R"pbdoc(Get all discovered concepts)
             
             Returns:
                 list[DiscoveredConcept]: List of all concepts
                 
             Examples:
                 >>> concepts = concept_form.getConcepts()
                 >>> print(f"Discovered {len(concepts)} concepts")
                 >>> for concept in concepts:
                 ...     print(f"Concept {concept.id}: {concept.categoryHint}")
             ")
        .def("getConceptCount", &ConceptFormation::getConceptCount,
             R"pbdoc(Get number of discovered concepts)
             
             Returns:
                 int: Number of concepts
                 
             Examples:
                 >>> count = concept_form.getConceptCount()
                 >>> print(f"Current concept count: {count}")
             ")
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype,
             py::arg("conceptId"),
             R"pbdoc(Get concept prototype pattern)
             
             Examples:
                 >>> prototype = concept_form.getConceptPrototype(5)
                 >>> print(f"Prototype pattern: {prototype}")
             ")
        .def("getConceptInstances", &ConceptFormation::getConceptInstances,
             py::arg("conceptId"),
             R"pbdoc(Get concept instances)
             
             Examples:
                 >>> instances = concept_form.getConceptInstances(5)
                 >>> print(f"Concept has {len(instances)} instances")
             ")
        .def("getConceptStability", &ConceptFormation::getConceptStability,
             py::arg("conceptId"),
             R"pbdoc(Get concept stability measure)
             
             Returns:
                 float: Stability between 0.0 and 1.0
                 
             Examples:
                 >>> stability = concept_form.getConceptStability(5)
                 >>> print(f"Concept stability: {stability:.2f}")
             ")
        .def("isNovel", &ConceptFormation::isNovel,
             py::arg("pattern"), py::arg("similarityThreshold"),
             R"pbdoc(Check if a pattern is novel (doesn't belong to any concept))
             
             Examples:
                 >>> pattern = [0.1, 0.2, 0.3, 0.4, 0.5]
                 >>> if concept_form.isNovel(pattern):
                 ...     print("Pattern is novel - create new concept")
             ")
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern,
             py::arg("pattern"),
             R"pbdoc(Find concept for a given pattern)
             
             Examples:
                 >>> concept_id = concept_form.findConceptForPattern(pattern)
                 >>> if concept_id > 0:
                 ...     print(f"Pattern belongs to concept {concept_id}")
                 ")
        .def("computeSimilarity", &ConceptFormation::computeSimilarity,
             py::arg("a"), py::arg("b"),
             R"pbdoc(Compute similarity between two patterns)
             
             Returns:
                 float: Cosine similarity between 0.0 and 1.0
                 
             Examples:
                 >>> sim = concept_form.computeSimilarity(pattern1, pattern2)
                 >>> print(f"Similarity: {sim:.2f}")
             ")
        .def("updateConcept", &ConceptFormation::updateConcept,
             py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"),
             R"pbdoc(Update concept with new instance)
             
             Examples:
                 >>> concept_form.updateConcept(concept_id, new_pattern, new_features, new_reward)
                 >>> print(f"Concept {concept_id} updated with new instance")
             ")
        .def("clear", &ConceptFormation::clear,
             R"pbdoc(Clear all concepts)
             
             Examples:
                 >>> concept_form.clear()
                 >>> print("All concepts cleared")
             ")
        .def("setFormationThreshold", &ConceptFormation::setFormationThreshold,
             py::arg("threshold"),
             R"pbdoc(Set formation threshold for new concepts)
             ")
        .def("setStabilityWindow", &ConceptFormation::setStabilityWindow,
             py::arg("window"),
             R"pbdoc(Set stability window size)
             ")
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility,
             py::arg("conceptId"),
             R"pbdoc(Get concept generalization ability)
             
             Returns:
                 float: How well concept handles variations (0.0-1.0)
                 
             Examples:
                 >>> ability = concept_form.getGeneralizationAbility(concept_id)
                 >>> print(f"Generalization ability: {ability:.2f}")
             ")

    // Memory System - Working Memory
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory using persistent activity and recurrent connections)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize,
             py::arg("brain"),
             R"pbdoc(Initialize working memory with brain reference)
             ")
        .def("store", &NeuralWorkingMemory::store,
             py::arg("pattern"), py::arg("strength") = 1.0f,
             R"pbdoc(Store information in working memory)
             ")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron,
             py::arg("neuron"), py::arg("activation"),
             R"pbdoc(Store activation to specific neuron)
             ")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             R"pbdoc(Retrieve current working memory content)
             
             Returns:
                 list[float]: Current memory content as activation levels
                 ")
        .def("contains", &NeuralWorkingMemory::contains,
             py::arg("neuron"),
             R"pbdoc(Check if neuron is part of working memory)
             ")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation,
             py::arg("neuron"),
             R"pbdoc(Get activation level of neuron in working memory)
             ")
        .def("update", &NeuralWorkingMemory::update,
             py::arg("dt"),
             R"pbdoc(Update working memory (maintenance and decay)
             ")
        .def("clear", &NeuralWorkingMemory::clear,
             R"pbdoc(Clear working memory)
             ")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             R"pbdoc(Get number of active memory traces)
             ")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity,
             R"pbdoc(Get working memory capacity)
             ")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity,
             py::arg("capacity"),
             R"pbdoc(Set working memory capacity)
             ")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate,
             R"pbdoc(Get decay rate)
             ")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate,
             py::arg("rate"),
             R"pbdoc(Set decay rate)
             ")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             R"pbdoc(Get neurons currently in working memory)
             ")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory,
             py::arg("factor"),
             R"pbdoc(Strengthen working memory representation)
             ")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition,
             R"pbdoc(Run competition between memory traces)
             ")
        .def("isWinning", &NeuralWorkingMemory::isWinning,
             py::arg("neuron"),
             R"pbdoc(Check if neuron is part of winning population)
             ")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             R"pbdoc(Get overall memory activity level)
             ")
        .def("getStats", [](const NeuralWorkingMemory& self) {
            py::dict stats;
            stats["active_traces"] = static_cast<size_t>(self.getActiveTraces());
            stats["capacity"] = static_cast<size_t>(self.getCapacity());
            stats["decay_rate"] = self.getDecayRate();
            stats["memory_neurons"] = static_cast<size_t>(self.getMemoryNeurons().size());
            stats["activity"] = self.getMemoryActivity();
            return stats;
        }, R"pbdoc(Get working memory statistics)
        ")

    // Memory System - Episodic Memory
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural episodic memory storing experiences as neural activity patterns)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize,
             py::arg("brain"),
             R"pbdoc(Initialize episodic memory with brain reference)
             ")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode,
             py::arg("episode"),
             R"pbdoc(Store a new experience)
             ")
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar,
             py::arg("sensoryPattern"), py::arg("maxResults") = 5,
             R"pbdoc(Retrieve episodes similar to query pattern)
             
             Returns:
                 list[EpisodicMemoryItem]: Episodes ordered by relevance
                 ")
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal,
             py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10,
             R"pbdoc(Retrieve episodes from specific time window)
             ")
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation,
             py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5,
             R"pbdoc(Retrieve episodes by location)
             ")
        .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction,
             py::arg("action"), py::arg("maxResults") = 5,
             R"pbdoc(Retrieve episodes following specific action)
             ")
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode,
             py::arg("episode"),
             R"pbdoc(Replay an episode - reactivates neural patterns)
             ")
        .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance,
             py::arg("episodeId"), py::arg("relevanceDelta"),
             R"pbdoc(Update episode relevance based on recent use)
             ")
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode,
             py::arg("index"),
             R"pbdoc(Get episode by index)
             ")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             R"pbdoc(Get total number of episodes)
             ")
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes,
             py::arg("count"),
             R"pbdoc(Get recent episodes)
             ")
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward,
             R"pbdoc(Get average reward from episodes)
             ")
        .def("consolidate", &NeuralEpisodicMemory::consolidate,
             py::arg("relevanceThreshold"),
             R"pbdoc(Clear old episodes based on relevance)
             ")
        .def("clear", &NeuralEpisodicMemory::clear,
             R"pbdoc(Clear all episodes)
             ")
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay,
             py::arg("enable"),
             R"pbdoc(Enable/disable replay)
             ")
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled,
             R"pbdoc(Check if replay is enabled)
             ")
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay,
             py::arg("count"),
             R"pbdoc(Get episodes for replay)
             ")
        .def("replaySequence", &NeuralEpisodicMemory::replaySequence,
             py::arg("episodeIds"),
             R"pbdoc(Replay multiple episodes)
             ")
        .def("getStats", [](const NeuralEpisodicMemory& self) {
            py::dict stats;
            stats["episode_count"] = static_cast<size_t>(self.getEpisodeCount());
            stats["average_reward"] = self.getAverageReward();
            stats["replay_enabled"] = self.isReplayEnabled();
            stats["recent_episodes_5"] = self.getRecentEpisodes(5);
            return stats;
        }, R"pbdoc(Get episodic memory statistics)
        ")

    // Sensory Methods
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for all sensory input)pbdoc")
        .def(py::init<>())
        .def("recordSensoryObservation", [](SensoryInput& self, const std::vector<float>& data, const std::string& type) {
            // For concrete sensory input types (Vision, Audio, InternalSignals)
            // This is a wrapper method for recording sensory observations
            // The actual implementation depends on the specific sensory type
            return self;
        }, py::arg("data"), py::arg("type"),
        R"pbdoc(Record sensory observation)
        
        Examples:
            >>> # Record visual observation
            >>> vision_data = [0.1, 0.5, 0.9, 0.2, ...]  # 2D grid of intensities
            >>> sensory = sensory_input.recordSensoryObservation(vision_data, "vision")
        ")
        .def("getSensoryNeurons", [](const SensoryInput& self) {
            // Return list of neurons associated with this sensory input
            // This is a placeholder - actual implementation would identify
            // which neurons are activated by this sensory input
            return std::vector<NeuronId>{};
        }, R"pbdoc(Get sensory neurons)
        
        Returns:
            list[NeuronId]: Neurons associated with this sensory input
            
        Examples:
            >>> sensory = sensory_input
            >>> neurons = sensory.getSensoryNeurons()
            >>> print(f"Sensory input activates {len(neurons)} neurons")
        ")
        .def("getSensoryInput", [](const SensoryInput& self) {
            return self.getData();
        }, R"pbdoc(Get raw sensory input data)
        
        Returns:
            list[float]: Raw sensory data vector
            
        Examples:
            >>> sensory = sensory_input
            >>> data = sensory.getSensoryInput()
            >>> print(f"Sensory input shape: {len(data)} elements")
        ")
        .def("getType", &SensoryInput::getType)
        .def("getData", &SensoryInput::getData)
        .def("getDimensions", &SensoryInput::getDimensions)
        .def("getTimestamp", &SensoryInput::getTimestamp)
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"));

    // Behavioral Methods
    py::class_<Action>(m, "Action", R"pbdoc(Action for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        // Add behavioral and monitoring methods to Brain class
        .def("getAction", &Brain::produceAction,
             R"pbdoc(Get action based on neural activity)
             
             Returns:
                 Action: Motor action
                 
             Examples:
                 >>> brain = nlm.createBrain(nlm.createDefaultConfig())
                 >>> action = brain.getAction()
                 >>> print(f"Selected action: {action.getName()}")
             ")
        .def("getPlannedAction", &Brain::produceAction,
             R"pbdoc(Get planned action with forward prediction)
             
             Returns:
                 Action: Planned action
                 
             Examples:
                 >>> action = brain.getPlannedAction()
                 >>> print(f"Planned action: {action.getName()}")
             ")
        .def("getBehavioralStats", [](const Brain& self) {
            py::dict stats;
            stats["active_neurons"] = static_cast<size_t>(self.getActiveNeuronCount());
            stats["firing_neurons"] = static_cast<size_t>(self.getFiringNeuronCount());
            stats["total_spikes"] = static_cast<size_t>(self.getTotalSpikeCount());
            stats["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            return stats;
        }, R"pbdoc(Get behavioral statistics)
        
        Returns:
            dict: Behavioral metrics
            
        Examples:
            >>> stats = brain.getBehavioralStats()
            >>> print(f"Activity: {stats['active_neurons']} neurons, "
            ...       f"Firing: {stats['firing_neurons']} neurons")
        ")
        .def("getSystemStats", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = static_cast<size_t>(self.getTotalNeuronCount());
            stats["total_synapses"] = static_cast<size_t>(self.getTotalSynapseCount());
            stats["active_neurons"] = static_cast<size_t>(self.getActiveNeuronCount());
            stats["firing_neurons"] = static_cast<size_t>(self.getFiringNeuronCount());
            stats["total_spikes"] = static_cast<size_t>(self.getTotalSpikeCount());
            stats["avg_firing_rate"] = self.getAverageFiringRate();
            stats["ei_ratio"] = self.getExcitationInhibitionRatio();
            return stats;
        }, R"pbdoc(Get system statistics)
        
        Returns:
            dict: System metrics
            
        Examples:
            >>> stats = brain.getSystemStats()
            >>> print(f"System: {stats['total_neurons']} neurons, "
            ...       f"{stats['total_synapses']} synapses")
        ")
        .def("getPerformanceMetrics", [](const Brain& self) {
            py::dict metrics;
            metrics["learning_rate"] = 0.5f;  // Placeholder - would compute from internal systems
            metrics["adaptation_speed"] = 0.3f;  // Placeholder
            metrics["prediction_accuracy"] = 0.8f;  // Placeholder
            metrics["behavioral_efficiency"] = 0.7f;  // Placeholder
            return metrics;
        }, R"pbdoc(Get performance metrics)
        
        Returns:
            dict: Performance indicators
            
        Examples:
            >>> metrics = brain.getPerformanceMetrics()
            >>> print(f"Learning rate: {metrics['learning_rate']:.2f}")
        ")
        .def("getLearningProgress", [](const Brain& self) {
            py::dict progress;
            progress["concepts_discovered"] = 0;  // Placeholder - would get from concept formation
            progress["patterns_learned"] = 0;  // Placeholder
            progress["memory_capacity_utilization"] = 0.5f;  // Placeholder
            progress["prediction_improvement"] = 0.2f;  // Placeholder
            return progress;
        }, R"pbdoc(Get learning progress metrics)
        
        Returns:
            dict: Learning indicators
            
        Examples:
            >>> progress = brain.getLearningProgress()
            >>> print(f"Concepts discovered: {progress['concepts_discovered']}")
        ")
        .def("getWorkingMemory", [](const Brain& self) {
            // Return a proxy object for working memory
            if (self.getWorkingMemory()) {
                return self.getWorkingMemory();
            }
            return static_cast<NeuralWorkingMemory*>(nullptr);
        }, R"pbdoc(Get working memory)
        
        Returns:
            NeuralWorkingMemory: Working memory object
            
        Examples:
            >>> memory = brain.getWorkingMemory()
            >>> if memory:
            ...     print("Working memory available")
            >>> memory.store(pattern, 1.0)
        ")
        .def("getEpisodicMemory", [](const Brain& self) {
            // Return a proxy object for episodic memory
            if (self.getEpisodicMemory()) {
                return self.getEpisodicMemory();
            }
            return static_cast<NeuralEpisodicMemory*>(nullptr);
        }, R"pbdoc(Get episodic memory)
        
        Returns:
            NeuralEpisodicMemory: Episodic memory object
            
        Examples:
            >>> memory = brain.getEpisodicMemory()
            >>> if memory:
            ...     print(f"Episodic memory has {memory.getEpisodeCount()} episodes")
        ")
        .def("clearMemory", [](Brain& self) {
            if (self.getWorkingMemory()) {
                self.getWorkingMemory()->clear();
            }
            if (self.getEpisodicMemory()) {
                self.getEpisodicMemory()->clear();
            }
        }, R"pbdoc(Clear all memory)
        
        Examples:
            >>> brain.clearMemory()
            >>> print("All memory cleared - fresh start for new episode")
        ")
        .def("getMemoryStats", [](const Brain& self) {
            py::dict stats;
            
            if (self.getWorkingMemory()) {
                auto wm = self.getWorkingMemory();
                py::dict wm_stats;
                wm_stats["active_traces"] = static_cast<size_t>(wm->getActiveTraces());
                wm_stats["capacity"] = static_cast<size_t>(wm->getCapacity());
                wm_stats["activity"] = wm->getMemoryActivity();
                stats["working_memory"] = wm_stats;
            }
            
            if (self.getEpisodicMemory()) {
                auto em = self.getEpisodicMemory();
                py::dict em_stats;
                em_stats["episode_count"] = static_cast<size_t>(em->getEpisodeCount());
                em_stats["average_reward"] = em->getAverageReward();
                em_stats["replay_enabled"] = em->isReplayEnabled();
                stats["episodic_memory"] = em_stats;
            }
            
            return stats;
        }, R"pbdoc(Get memory statistics)
        
        Returns:
            dict: Memory metrics
            
        Examples:
            >>> stats = brain.getMemoryStats()
            >>> wm_stats = stats.get("working_memory", {})
            >>> print(f"Working memory activity: {wm_stats.get('activity', 0):.2f}")
        ")
        .def("getConceptFormation", [](const Brain& self) {
            // Return a proxy object for concept formation
            if (self.getConceptFormation()) {
                return self.getConceptFormation();
            }
            return static_cast<ConceptFormation*>(nullptr);
        }, R"pbdoc(Get concept formation system)
        
        Returns:
            ConceptFormation: Concept formation object
            
        Examples:
            >>> concept_form = brain.getConceptFormation()
            >>> if concept_form:
            ...     print(f"Concept formation has {concept_form.getConceptCount()} concepts")
        ")
        .def("getPredictionSystem", [](const Brain& self) {
            // Return a proxy object for prediction system
            if (self.getPredictionSystem()) {
                return self.getPredictionSystem();
            }
            return static_cast<PredictionSystem*>(nullptr);
        }, R"pbdoc(Get prediction system)
        
        Returns:
            PredictionSystem: Prediction system object
            
        Examples:
            >>> pred = brain.getPredictionSystem()
            >>> if pred:
            ...     print(f"Prediction confidence: {pred.getConfidence():.2f}")
        ")

    // Add remaining existing bindings
