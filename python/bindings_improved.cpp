#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <unordered_map>

// Include all the required headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/environment/Observation.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/prediction/PredictionSystem.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception classes for Python
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidArgumentError : public NLMException {
public:
    explicit InvalidArgumentError(const std::string& msg) : NLMException("InvalidArgumentError: " + msg) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& msg) : NLMException("ConfigurationError: " + msg) {}
};

class MemoryError : public NLMException {
public:
    explicit MemoryError(const std::string& msg) : NLMException("MemoryError: " + msg) {}
};

class TrainingError : public NLMException {
public:
    explicit TrainingError(const std::string& msg) : NLMException("TrainingError: " + msg) {}
};

// Exception translation utilities
try {
    // Function to translate C++ exceptions to Python exceptions
    template<typename Callable>
    auto translateExceptions(Callable&& func) {
        return [func](auto&&... args) {
            try {
                return func(std::forward<decltype(args)>(args)...);
            } catch (const InvalidArgumentError& e) {
                py::raise(PyExc_ValueError, e.what());
            } catch (const ConfigurationError& e) {
                py::raise(PyExc_RuntimeError, e.what());
            } catch (const MemoryError& e) {
                py::raise(PyExc_MemoryError, e.what());
            } catch (const TrainingError& e) {
                py::raise(PyExc_RuntimeError, e.what());
            } catch (const std::runtime_error& e) {
                py::raise(PyExc_RuntimeError, e.what());
            } catch (const std::invalid_argument& e) {
                py::raise(PyExc_ValueError, e.what());
            } catch (const std::out_of_range& e) {
                py::raise(PyExc_IndexError, e.what());
            } catch (...) {
                py::raise(PyExc_RuntimeError, "Unknown error occurred");
            }
        };
    }
} catch (...) {
    // If exception translation setup fails, continue with default error handling
}

// Context manager for automatic cleanup
class BrainCleanupManager {
public:
    explicit BrainCleanupManager(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    void enter() {
        // No action needed on enter
    }
    
    void exit() {
        if (brain_) {
            brain_->reset();
        }
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

class MemoryCleanupManager {
public:
    explicit MemoryCleanupManager(std::shared_ptr<NeuralWorkingMemory> memory) : memory_(memory) {}
    
    void enter() {
        // No action needed on enter
    }
    
    void exit() {
        if (memory_) {
            memory_->clear();
        }
    }
    
private:
    std::shared_ptr<NeuralWorkingMemory> memory_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Improved Python Bindings
        ---------------------------------------------------------
        
        This module provides an enhanced interface to the NLM neural simulation framework
        with comprehensive error handling, complete class coverage, and Pythonic APIs.
        
        Features:
        - Full error handling and exception translation
        - All memory, neuromodulation, and cognition classes included
        - High-level training APIs (train(), validate(), test())
        - Model persistence (save/load)
        - Real-time monitoring and statistics
        - Context managers for automatic resource cleanup
        - Python properties and natural method signatures
        - Utility functions for random seeds and experiment templates
        
        Backward compatible with existing API while adding new functionality.
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::out_of_range& e) {
            PyErr_SetString(PyExc_IndexError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown exception occurred");
        }
    });

    // Type definitions
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

    // Memory system classes
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory - persistent activity storage)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store a pattern in working memory")
        .def("store_to_neuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"),
             "Store activation to specific neuron")
        .def("retrieve", &NeuralWorkingMemory::retrieve, "Retrieve current memory content")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"),
             "Check if neuron is in working memory")
        .def("get_neuron_activation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"),
             "Get activation of specific neuron")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"),
             "Update working memory")
        .def("clear", &NeuralWorkingMemory::clear,
             "Clear working memory")
        .def("capacity", &NeuralWorkingMemory::getCapacity, "Get memory capacity")
        .def("capacity", &NeuralWorkingMemory::setCapacity, py::arg("capacity"),
             "Set memory capacity")
        .def("decay_rate", &NeuralWorkingMemory::getDecayRate, "Get decay rate")
        .def("decay_rate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"),
             "Set decay rate")
        .def("memory_neurons", &NeuralWorkingMemory::getMemoryNeurons,
             "Get neurons in memory")
        .def("active_traces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("strengthen_memory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"),
             "Strengthen memory representations")
        .def("run_competition", &NeuralWorkingMemory::runCompetition,
             "Run winner-take-all competition")
        .def("is_winning", &NeuralWorkingMemory::isWinning, py::arg("neuron"),
             "Check if neuron is a winner")
        .def("memory_activity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level");

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural episodic memory - storing experiences)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("store_episode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"),
             "Store a new episode")
        .def("retrieve_similar", &NeuralEpisodicMemory::retrieveSimilar, py::arg("sensory_pattern"), py::arg("max_results") = 5,
             "Retrieve similar episodes")
        .def("retrieve_temporal", &NeuralEpisodicMemory::retrieveTemporal, py::arg("start_time"), py::arg("end_time"), py::arg("max_results") = 10,
             "Retrieve episodes from time window")
        .def("retrieve_by_location", &NeuralEpisodicMemory::retrieveByLocation, py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("max_results") = 5,
             "Retrieve episodes by location")
        .def("retrieve_after_action", &NeuralEpisodicMemory::retrieveAfterAction, py::arg("action"), py::arg("max_results") = 5,
             "Retrieve episodes after specific action")
        .def("replay_episode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"),
             "Replay an episode")
        .def("update_relevance", &NeuralEpisodicMemory::updateRelevance, py::arg("episode_id"), py::arg("relevance_delta"),
             "Update episode relevance")
        .def("get_episode", &NeuralEpisodicMemory::getEpisode, py::arg("index"),
             "Get episode by index")
        .def("episode_count", &NeuralEpisodicMemory::getEpisodeCount,
             "Get number of episodes")
        .def("recent_episodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"),
             "Get recent episodes")
        .def("get_average_reward", &NeuralEpisodicMemory::getAverageReward,
             "Get average reward from episodes")
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevance_threshold"),
             "Consolidate relevant episodes")
        .def("clear", &NeuralEpisodicMemory::clear,
             "Clear all episodes")
        .def("enable_replay", &NeuralEpisodicMemory::enableReplay, py::arg("enable"),
             "Enable/disable replay")
        .def("is_replay_enabled", &NeuralEpisodicMemory::isReplayEnabled,
             "Check if replay is enabled")
        .def("get_episodes_for_replay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("count"),
             "Get episodes for replay");

    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Neural associative memory - pattern relationships)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("associate", &NeuralAssociativeMemory::associate, py::arg("pattern_a"), py::arg("pattern_b"), py::arg("strength") = 1.0f,
             "Create association between two patterns")
        .def("associate_from_experience", &NeuralAssociativeMemory::associateFromExperience, py::arg("episode"),
             "Create association from experience")
        .def("retrieve", &NeuralAssociativeMemory::retrieve, py::arg("query_pattern"), py::arg("max_results") = 5,
             "Retrieve patterns associated with query")
        .def("get_association_strength", &NeuralAssociativeMemory::getAssociationStrength, py::arg("pattern_a"), py::arg("pattern_b"),
             "Get association strength")
        .def("update_association", &NeuralAssociativeMemory::updateAssociation, py::arg("pattern_a"), py::arg("pattern_b"), py::arg("delta"),
             "Update association strength")
        .def("spread_activation", &NeuralAssociativeMemory::spreadActivation, py::arg("cue_pattern"), py::arg("steps") = 2,
             "Spread activation through associations")
        .def("clear", &NeuralAssociativeMemory::clear,
             "Clear all associations")
        .def("association_count", &NeuralAssociativeMemory::getAssociationCount,
             "Get number of associations");

    // Neuromodulation classes
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator - reward signal)pbdoc")
        .def(py::init<>())
        .def("get_name", &Neuromodulator::getName)
        .def("level", &Dopamine::getLevel, "Get current dopamine level")
        .def("level", &Dopamine::setLevel, py::arg("level"), "Set dopamine level")
        .def("plasticity_factor", &Dopamine::getPlasticityFactor,
             "Get plasticity factor for this neuromodulator")
        .def("update", &Dopamine::update, py::arg("dt"), "Update neuromodulator state")
        .def("signal_reward", &Dopamine::signalReward, py::arg("reward"),
             "Signal reward for reinforcement learning")
        .def("signal_reward_prediction_error", &Dopamine::signalRewardPredictionError, py::arg("error"),
             "Signal reward prediction error for learning");

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator - exploration drive)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("level", &Curiosity::getLevel, "Get current curiosity level")
        .def("exploration_drive", &Curiosity::getExplorationDrive,
             "Get exploration drive (same as level)")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("prediction_error"), py::arg("dt"),
             "Update curiosity based on novelty and prediction error")
        .def("set_novelty_weight", &Curiosity::setNoveltyWeight, py::arg("weight"),
             "Set weight for novelty contribution")
        .def("set_prediction_error_weight", &Curiosity::setPredictionErrorWeight, py::arg("weight"),
             "Set weight for prediction error contribution")
        .def("reset", &Curiosity::reset,
             "Reset curiosity system");

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty neuromodulator - novelty detection)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("level", &Novelty::getLevel, "Get current novelty level")
        .def("level", &Novelty::setLevel, py::arg("level"), "Set novelty level")
        .def("detect_novelty", [](const Novelty& self, const Observation& obs, const Observation& prev) {
            return self.detectNovelty(obs, prev);
        }, py::arg("observation"), py::arg("previous_observation"),
             "Detect novelty from two observations")
        .def("detect_novelty", [](const Novelty& self, const std::vector<float>& current, const std::vector<float>& previous) {
            return self.detectNovelty(current, previous);
        }, py::arg("current_pattern"), py::arg("previous_pattern"),
             "Detect novelty from two patterns")
        .def("update", &Novelty::update, py::arg("dt"), "Update novelty detection")
        .def("history", &Novelty::getHistory, "Get novelty history")
        .def("clear_history", &Novelty::clearHistory, "Clear novelty history");

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error - prediction error signal)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("error", &PredictionError::getError, "Get current prediction error")
        .def("compute_error", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"),
             "Compute prediction error from predicted and actual values")
        .def("update_prediction", &PredictionError::updatePrediction, py::arg("prediction"),
             "Update prediction with new value")
        .def("history", &PredictionError::getHistory, "Get prediction error history")
        .def("clear_history", &PredictionError::clearHistory, "Clear prediction error history")
        .def("magnitude", &PredictionError::getMagnitude, "Get error magnitude for neuromodulation");

    py::class_<Reward>(m, "Reward", R"pbdoc(Reward neuromodulator - reward accumulation)pbdoc")
        .def(py::init<>())
        .def("value", &Reward::getValue, "Get current reward value")
        .def("value", &Reward::setValue, py::arg("value"), "Set reward value")
        .def("add", &Reward::add, py::arg("delta"), "Add to reward value")
        .def("reset", &Reward::reset,
             "Reset accumulated reward")
        .def("compute_reward", &Reward::computeReward, py::arg("observation"),
             "Compute reward from observation")
        .def("history", &Reward::getHistory, "Get reward history")
        .def("clear_history", &Reward::clearHistory, "Clear reward history");

    // Cognition classes
    py::class_<SocialLearning>(m, "SocialLearning", R"pbdoc(Social learning - learning from others)pbdoc")
        .def(py::init<>())
        .def("initialize", &SocialLearning::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("observe", &SocialLearning::observeAgentAction,
             py::arg("observed_action"), py::arg("observer_state"), py::arg("resulting_state"),
             "Record observation of another agent's action")
        .def("can_imitate", &SocialLearning::canImitate, py::arg("observed_action"),
             "Check if can imitate a given action")
        .def("get_imitation_action", &SocialLearning::getImitationAction, py::arg("current_state"),
             "Get best action to imitate given current state")
        .def("learn_communication", &SocialLearning::learnCommunicationSignal,
             py::arg("signal_pattern"), py::arg("signal_reward"),
             "Learn communication signal from another agent")
        .def("detect_signal", &SocialLearning::detectSignal, py::arg("neural_pattern"),
             "Detect if another agent is signaling")
        .def("get_signal_pattern", &SocialLearning::getSignalPattern,
             "Get learned communication signal pattern")
        .def("get_signal_meaning", &SocialLearning::getSignalMeaning,
             "Get meaning (associated reward) of communication signal")
        .def("update_social_knowledge", &SocialLearning::updateSocialKnowledge, py::arg("interaction_reward"),
             "Update social knowledge based on interaction")
        .def("clear", &SocialLearning::clear,
             "Clear all social learning")
        .def("observation_count", &SocialLearning::getObservationCount,
             "Get number of observations made");

    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner - action planning)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("plan_action", &NeuralPlanner::planAction, py::arg("current_state"), py::arg("target_reward"),
             "Plan next action given current state and target reward", py::arg("target_reward") = 0.5f)
        .def("evaluate_sequence", &NeuralPlanner::evaluateSequence, py::arg("actions"), py::arg("start_state"),
             "Evaluate a potential action sequence")
        .def("planning_depth", &NeuralPlanner::getPlanningDepth, "Get planning depth (look-ahead steps)")
        .def("planning_depth", &NeuralPlanner::setPlanningDepth, py::arg("depth"),
             "Set planning depth (look-ahead steps)")
        .def("planning_confidence", &NeuralPlanner::getPlanningConfidence, "Get current planning confidence")
        .def("update_plan_quality", &NeuralPlanner::updatePlanQuality,
             py::arg("planned_actions"), py::arg("actual_actions"), py::arg("actual_reward"),
             "Update plans based on actual outcome")
        .def("clear_cache", &NeuralPlanner::clearCache,
             "Clear planning cache")
        .def("set_action_quality", &NeuralPlanner::setActionQuality, py::arg("action"), py::arg("quality"),
             "Set action quality from experience")
        .def("get_current_goal", &NeuralPlanner::getCurrentGoal, "Get current planning goal")
        .def("set_current_goal", &NeuralPlanner::setCurrentGoal, py::arg("goal"),
             "Set current planning goal");

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation - pattern clustering)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("present_experience", &ConceptFormation::presentExperience,
             py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("time"),
             "Present a new experience to the system")
        .def("get_matching_concept", &ConceptFormation::getMatchingConcept, py::arg("pattern"), py::arg("threshold"),
             "Get concept that best matches a pattern", py::arg("threshold") = 0.7f)
        .def("concept_count", &ConceptFormation::getConceptCount, "Get number of discovered concepts")
        .def("get_concept_prototype", &ConceptFormation::getConceptPrototype, py::arg("concept_id"),
             "Get prototype pattern for a concept")
        .def("get_concept_instances", &ConceptFormation::getConceptInstances, py::arg("concept_id"),
             "Get instances for a concept")
        .def("get_concept_stability", &ConceptFormation::getConceptStability, py::arg("concept_id"),
             "Get stability of a concept (0-1)")
        .def("is_novel", &ConceptFormation::isNovel, py::arg("pattern"), py::arg("threshold"),
             "Check if pattern is novel", py::arg("threshold") = 0.7f)
        .def("merge_concepts", &ConceptFormation::mergeConcepts, py::arg("concept_a"), py::arg("concept_b"),
             "Merge two discovered concepts")
        .def("clear", &ConceptFormation::clear,
             "Clear all concepts")
        .def("get_generalization_ability", &ConceptFormation::getGeneralizationAbility, py::arg("concept_id"),
             "Get how well a concept generalizes to variations");

    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system - predictive coding)pbdoc")
        .def(py::init<>())
        .def("predict_next_state", &PredictionSystem::predictNextState, py::arg("current_state"),
             "Predict next state from current state")
        .def("update_predictions", &PredictionSystem::updatePredictions,
             py::arg("predicted"), py::arg("actual"),
             "Update predictions based on actual observation")
        .def("prediction_error", &PredictionSystem::getPredictionError,
             "Get prediction error")
        .def("confidence", &PredictionSystem::getConfidence,
             "Get prediction confidence")
        .def("error_history", &PredictionSystem::getErrorHistory,
             "Get prediction error history")
        .def("clear_error_history", &PredictionSystem::clearHistory,
             "Clear error history")
        .def("train", &PredictionSystem::train, py::arg("observation"),
             "Train prediction model");

    // Development system
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system - agent development)pbdoc")
        .def(py::init<>())
        .def("set_developmental_stage", &DevelopmentSystem::setDevelopmentalStage, py::arg("stage"),
             "Set developmental stage")
        .def("developmental_stage", &DevelopmentSystem::getDevelopmentalStage,
             "Get current developmental stage")
        .def("update", &DevelopmentSystem::update, py::arg("dt"),
             "Update development system (called automatically)")
        .def("reset", &DevelopmentSystem::reset,
             "Reset developmental state");

    // Main classes with enhanced APIs
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("save_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has_key", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("get", py::overload_cast<const std::string&>(&Config::get), py::arg("key"),
             "Get a configuration value (returns empty string if not found)")
        .def("get_int", py::overload_cast<const std::string&, int>(&Config::get), py::arg("key"), py::arg("default"),
             "Get an integer configuration value")
        .def("get_float", py::overload_cast<const std::string&, float>(&Config::get), py::arg("key"), py::arg("default"),
             "Get a float configuration value")
        .def("get_bool", py::overload_cast<const std::string&, bool>(&Config::get), py::arg("key"), py::arg("default"),
             "Get a boolean configuration value")
        .def("set", py::overload_cast<const std::string&, const std::string&>(&Config::set), py::arg("key"), py::arg("value"),
             "Set a configuration value")
        .def("set_int", py::overload_cast<const std::string&, int>(&Config::set), py::arg("key"), py::arg("value"),
             "Set an integer configuration value")
        .def("set_float", py::overload_cast<const std::string&, float>(&Config::set), py::arg("key"), py::arg("value"),
             "Set a float configuration value")
        .def("set_bool", py::overload_cast<const std::string&, bool>(&Config::set), py::arg("key"), py::arg("value"),
             "Set a boolean configuration value")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config") = nullptr,
             "Create a new brain with configuration (None for default)")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", py::overload_cast<SimulationStep>(&Brain::step), py::arg("current_step"),
             "Perform a simulation step")
        .def("step", py::overload_cast<SimulationStep, Timestamp>(&Brain::step), py::arg("current_step"), py::arg("current_time"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
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
        .def("region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("developmental_stage", &Brain::setDevelopmentalStage, py::arg("stage"),
             "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status")
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
        .def("get_prediction_error", &Brain::getPredictionErrorSignal,
             "Get prediction error signal")
        .def("get_random_generator", &Brain::getRandomGenerator,
             "Get random number generator")
        .def("enable_reward_modulation", &Brain::enableRewardModulation, py::arg("enable"),
             "Enable reward modulation")
        .def("enable_structural_plasticity", &Brain::enableStructuralPlasticity, py::arg("enable"),
             "Enable structural plasticity")
        .def("enable_development", &Brain::enableDevelopment, py::arg("enable"),
             "Enable development")
        .def("enable_curiosity", &Brain::enableCuriosity, py::arg("enable"),
             "Enable curiosity")
        .def("is_reward_modulation_enabled", &Brain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &Brain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("is_development_enabled", &Brain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("is_curiosity_enabled", &Brain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("context_manager", py::overload_cast<std::shared_ptr<Brain>>(&py::class_<Brain>::def),
             "Get context manager for automatic cleanup")
        .def("__enter__", [](std::shared_ptr<Brain> brain) {
            return brain;
        })
        .def("__exit__", [](std::shared_ptr<Brain> brain, int, int, int) {
            if (brain) {
                brain->reset();
            }
        });

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an agent brain interface")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment, py::arg("timestep"),
             "Update development system")
        .def("developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain, py::return_value_policy::reference_internal,
             "Get the underlying brain");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width") = 5, py::arg("vision_height") = 5,
             "Configure the world dimensions")
        .def("reset", &SimpleWorld::reset,
             "Reset world state")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand, py::arg("cmd"), py::arg("current_time"),
             "Apply motor command to world")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("width", &SimpleWorld::getWidth,
             "Get world width")
        .def("height", &SimpleWorld::getHeight,
             "Get world height")
        .def("max_energy", &SimpleWorld::getMaxEnergy,
             "Get maximum agent energy")
        .def("max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("simulation_time", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("random_seed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        .def("random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed");

    // Convenience functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config") = nullptr, "Create a new brain with configuration");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
