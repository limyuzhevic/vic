// NLM Convenience API - High-level abstractions for common brain operations
// This file provides convenient functions for common NLM brain operations,
// making it easier for users to perform typical tasks without understanding
// the low-level implementation details.

#pragma once

#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/motor/Action.hpp"
#include "src/cognition/NeuralPlanner.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <sstream>
#include <numeric>
#include <tuple>

namespace nlm {

// ===============================================
// BRAIN CREATION AND INITIALIZATION HELPERS
// ===============================================

/**
 * @brief Create a fully configured NLM brain with all systems
 * 
 * This convenience function creates a brain with all integrated systems
 * (memory, prediction, cognition, neuromodulation) enabled, ready for simulation.
 * 
 * @param neuronCount Number of neurons (default: 500)
 * @param regionCount Number of brain regions (default: 1)
 * @param timestep Simulation timestep (default: 0.001)
 * @return Shared pointer to initialized brain
 */
inline std::shared_ptr<Brain> createBrainWithAllSystems(
    size_t neuronCount = 500,
    size_t regionCount = 1,
 float timestep = 0.001) {
    
    auto config = std::make_shared<Config>();
    
    // Configure basic brain parameters
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
    config->set("simulation_timestep", timestep, ConfigSource::Default);
    
    // Configure advanced systems
    config->set("plasticity_learning_rate", 0.01f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_time_constant", 20. acceler_f, ConfigSource::Default);
    
    // Memory system configuration
    config->set("working_memory_capacity", neuronCount / 10, ConfigSource::Default);
    config->set("episodic_memory_max_episodes", 1000, ConfigSource::Default);
    
    // Neuromodulation configuration
    config->set("dopamine_baseline", 0.0f, ConfigSource::Default);
    config->set("curiosity_baseline", 0.0f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    return brain;
}

/**
 * @brief Create a minimal NLM brain for testing and prototyping
 * 
 * This creates a brain with essential systems only (memory, neuromodulation)
 * but excludes complex cognition, making it faster for prototyping.
 * 
 * @param neuronCount Number of neurons (default: 100)
 * @return Shared pointer to initialized minimal brain
 */
inline std::shared_ptr<Brain> createMinimalBrain(
    size_t neuronCount = 100) {
    
    auto config = std::make_shared<Config>();
    
    // Minimal configuration - focus on basic functionality
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    return brain;
}

// ===============================================
// ENVIRONMENT AND AGENT HELPERS
// ===============================================

/**
 * @brief Create a simple world environment for NLM simulation
 * 
 * @param width Width of the world (default: 16)
 * @param height Height of the world (default: 16)
 * @param visionWidth Vision width (default: 16)
 * @param visionHeight Vision height (default: 16)
 * @return Shared pointer to SimpleWorld
 */
inline std::shared_ptr<SimpleWorld> createWorld(
    size_t width = 16,
    size_t height = 16,
    size_t visionWidth = 16,
    size_t visionHeight = 16) {
    
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width, height, visionWidth, visionHeight);
    world->reset();
    
    return world;
}

/**
 * @brief Create an agent-brain interface with a given brain
 * 
 * @param brain The brain to interface with
 * @param enableNeuromodulation Whether to enable neuromodulation (default: true)
 * @return Shared pointer to AgentBrain
 */
inline stdolicitud<std::shared_ptr<AgentBrain>> createAgentBrain(
    std::shared_ptr<Brain> brain,
    bool enableNeuromodulation = true) {
    
    auto agent = std::make_shared<AgentBrain>(brain);
    
    if (enableNeuromodulation) {
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableNovelty(true);
    }
    
    return agent;
}

// ===============================================
// SIMULATION HELPERS
// ===============================================

/**
 * @brief Run a complete simulation loop with brain, agent, and world
 * 
 * @param brain The brain to simulate
 * @param world The world environment
 * @param agent The agent interface (can be nullptr)
 * @param steps Number of simulation steps (default: 1000)
 * @param timestep Simulation timestep (default: 0.001)
 * @param verbose Whether to output progress information (default: false)
 * @return SimulationResults structure with metrics
 */
struct SimulationResults {
    float totalReward;
    float avgFiringRate;
    float avgProcessingTime;
    size_t stepsCompleted;
    size_t memoryEpisodes;
    float curiosityLevel;
    float noveltyLevel;
    float dopamineLevel;
    
    SimulationResults() : totalReward(0.0f), avgFiringRate(0.0f), avgProcessingTime(0.0f),
                          stepsCompleted(0), memoryEpisodes(0), curiosityLevel(0.0f),
                          noveltyLevel(0.0f), dopamineLevel(0.0f) {}
};

inline SimulationResults runCompleteSimulation(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world,
    std::shared_ptr<AgentBrain> agent,
    uint64_t steps = 1000,
    float timestep = 0.001,
    bool verbose = false) {
    
    SimulationResults results;
    results.stepsCompleted = steps;
    
    for (uint64_t step = 0; step < steps; ++step) {
        // Update world
        world->update(timestep);
        
        // Get sensory input
        SensoryPercept percept = world->getSensoryPercept();
        
        // Process through agent if available
        if (agent) {
            agent->processSensoryInput(percept);
        }
        
        // Brain step
        brain->step(step, step * timestep);
        
        // Get motor command through agent if available
        if (agent) {
            MotorCommand cmd = agent->decodeMotorCommand();
            ActionResult actionResult = world->applyMotorCommand(cmd, step * timestep);
        }
        
        // Collect metrics
        if (step % 100 == 0 && verbose) {
            std::cout << "Step " << step << ": Reward = " << (results.totalReward / (step + 1)) 
                      << ", Firing Rate = " << brain->getAverageFiringRate() << std::endl;
        }
    }
    
    // Compile final results
    results.totalReward = 0.0f; // Would need to track actual reward
    results.avgFiringRate = brain->getAverageFiringRate();
    results.stepsCompleted = steps;
    
    if (agent) {
        results.curiosityLevel = agent->getCuriosityLevel();
        results.noveltyLevel = agent->getNoveltyLevel();
        results.dopamineLevel = agent->getNeuromodulationLevel();
    }
    
    if (brain->getEpisodicMemory()) {
        results.memoryEpisodes = brain->getEpisodicMemory()->getEpisodeCount();
    }
    
    return results;
}

// ===============================================
// MEMORY HELPERS
// ===============================================

/**
 * @brief Get working memory traces with filtering and analysis
 * 
 * @param brain The brain to query
 * @param minActivation Minimum activation threshold (default: 0.0)
 * @param maxTraces Maximum number of traces to return (default: all)
 * @return Vector of neuron IDs with activation levels
 */
inline std::vector<std::pair<NeuronId, float>> getActiveWorkingMemory(
    std::shared_ptr<Brain> brain,
    float minActivation = 0.0f,
    size_t maxTraces = 0) {
    
    if (!brain->getWorkingMemory()) return {};
    
    auto neurons = brain->getWorkingMemory()->getMemoryNeurons();
    auto activations = brain->getWorkingMemory()->getMemoryActivations();
    
    std::vector<std::pair<NeuronId, float>> result;
    result.reserve(neurons.size());
    
    for (size_t i = 0; i < neurons.size(); ++i) {
        if (activations[i] >= minActivation) {
            if (maxTraces == 0 || result.size() < maxTraces) {
                result.emplace_back(neurons[i], activations[i]);
            }
        }
    }
    
    // Sort by activation level (descending)
    std::sort(result.begin(), result.end(),
              [](const std::pair<NeuronId, float>& a, const std::pair<NeuronId, float>& b) {
                  return a.second > b.second;
              });
    
    return result;
}

/**
 * @brief Get recent episodes from episodic memory with filtering
 * 
 * @param brain The brain to query
 * @param maxEpisodes Maximum number of episodes to return (default: all)
 * @param minReward Minimum reward threshold (default: -inf)
 * @return Vector of episodic memory items
 */
inline std::vector<EpisodicMemoryItem> getRecentEpisodes(
    std::shared_ptr<Brain> brain,
    size_t maxEpisodes = 0,
    float minReward = std::numeric_limits<float>::lowest()) {
    
    if (!brain->getEpisodicMemory()) return {};
    
    auto allEpisodes = brain->getEpisodicMemory()->getEpisodes();
    
    std::vector<EpisodicMemoryItem> result;
    result.reserve(allEpisodes.size());
    
    for (const auto* episode : allEpisodes) {
        if (episode->reward >= minReward) {
            if (maxEpisodes == 0 || result.size() < maxEpisodes) {
                result.push_back(*episode);
            }
        }
    }
    
    // Sort by timestamp (newest first)
    std::sort(result.begin(), result.end(),
              [](const EpisodicMemoryItem& a, const EpisodicMemoryItem& b) {
                  return a.timestamp > b.timestamp;
              });
    
    return result;
}

// ===============================================
// PREDICTION AND COGNITION HELPERS
// ===============================================

/**
 * @brief Get prediction system confidence metrics
 * 
 * @param brain The brain to query
 * @return Prediction confidence metrics (mean, stddev, min, max)
 */
inline std::tuple<float, float, float, float> getPredictionConfidence(
    std::shared_ptr<Brain> brain) {
    
    if (!brain->getPredictionSystem()) {
        return std::make_tuple(0.0f, 0.0f, 0.0f, 0.0f);
    }
    
    // This would need to be implemented based on actual prediction system API
    float mean = 0.5f; // Placeholder
    float stddev = 0.1f; // Placeholder
    float min = 0.0f; // Placeholder
    float max = 1.0f; // Placeholder
    
    return std::make_tuple(mean, stddev, min, max);
}

/**
 * @brief Get concept formation similarity metrics
 * 
 * @param brain The brain to query
 * @param concept1 First concept ID
 * @param concept2 Second concept ID
 * @return Cosine similarity between concepts (0.0 = dissimilar, 1.0 = identical)
 */
inline float getConceptSimilarity(
    std::shared_ptr<Brain> brain,
    uint64_t concept1,
    uint64_t concept2) {
    
    if (!brain->getConceptFormation()) {
        return 0.0f;
    }
    
    // This would need to be implemented based on actual concept formation API
    return 0.5f; // Placeholder
}

// ===============================================
// NEUROMODULATION HELPERS
// ===============================================

/**
 * @brief Get current neuromodulation state as a summary
 * 
 * @param agent The agent to query
 * @return String describing neuromodulation state
 */
inline std::string getNeuromodulationState(
    std::shared_ptr<AgentBrain> agent) {
    
    if (!agent) return "No agent available";
    
    std::stringstream ss;
    ss << "Neuromodulation State:" << std::endl;
    ss << "  Dopamine: " << agent->getNeuromodulationLevel() << std::endl;
    ss << "  Curiosity: " << agent->getCuriosityLevel() << std::endl;
    ss << "  Novelty: " << agent->getNoveltyLevel() << std::endl;
    ss << "  Prediction Error: " << agent->getPredictionError() << std::endl;
    
    return ss.str();
}

/**
 * @brief Apply external reward signal with optional prediction error
 * 
 * @param agent The agent to modulate
 * @param reward External reward signal
 * @param expectedReward Expected reward for prediction error calculation
 * @param novelty External novelty signal (0.0 = familiar, 1.0 = novel)
 */
inline void applyExternalReward(
    std::shared_ptr<AgentBrain> agent,
    float reward,
    float expectedReward = 0.0f,
    float novelty = 0.0f) {
    
    if (!agent) return;
    
    agent->applyRewardModulation(reward, expectedReward);
    
    if (novelty > 0.0f) {
        // Apply novelty signal directly
        auto* noveltySystem = agent->getAgent()->getNovelty();
        if (noveltySystem) {
            noveltySystem->setLevel(novelty);
        }
    }
}

// ===============================================
// EXPORT AND ANALYSIS HELPERS
// ===============================================

/**
 * @brief Export brain state to JSON format for analysis
 * 
 * @param brain The brain to export
 * @param filepath Output file path
 * @param includeNeurons Whether to include detailed neuron data (default: false)
 * @return true if export succeeded, false otherwise
 */
inline bool exportBrainToJSON(
    std::shared_ptr<Brain> brain,
    const std::string& filepath,
    bool includeNeurons = false) {
    
    // This would need to be implemented with JSON library (e.g., nlohmann::json)
    // For now, return false to indicate not implemented
    return false;
}

/**
 * @brief Generate brain activity report
 * 
 * @param brain The brain to analyze
 * @param includeMemory Whether to include memory system analysis (default: true)
 * @param includeNeuromodulation Whether to include neuromodulation analysis (default: true)
 * @return Formatted report string
 */
inline std::string generateBrainReport(
    std::shared_ptr<Brain> brain,
    bool includeMemory = true,
    bool includeNeuromodulation = true) {
    
    std::stringstream ss;
    ss << "=== NLM Brain Report ===" << std::endl;
    ss << std::endl;
    
    ss << "Basic Statistics:" << std::endl;
    ss << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
    ss << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
    ss << "  Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
    ss << "  Firing Neuron Count: " << brain->getFiringNeuronCount() << std::endl;
    ss << std::endl;
    
    if (includeMemory && brain->getWorkingMemory()) {
        ss << "Working Memory:" << std::endl;
        ss << "  Active Traces: " << brain->getWorkingMemory()->getActiveTraces() << std::endl;
        ss << "  Capacity: " << brain->getWorkingMemory()->getCapacity() << std::endl;
        ss << std::endl;
    }
    
    if (includeMemory && brain->getEpisodicMemory()) {
        ss << "Episodic Memory:" << std::endl;
        ss << "  Episode Count: " << brain->getEpisodicMemory()->getEpisodeCount() << std::endl;
        ss << std::endl;
    }
    
    if (includeNeuromodulation) {
        // Note: Brain doesn't directly expose neuromodulators, would need agent
        ss << "Neuromodulation: Available (via AgentBrain interface)" << std::endl;
        ss << std::endl;
    }
    
    ss << "=== End Report ===" << std::endl;
    
    return ss.str();
}

// ===============================================
// EXPERIMENT HELPERS
// ===============================================

/**
 * @brief Run a standard NLM learning experiment
 * 
 * @param brain The brain to use
 * @param steps Number of learning steps (default: 1000)
 * @param verbose Whether to output progress (default: false)
 * @return Learning experiment results
 */
struct LearningExperimentResults {
    float initialMeanWeight;
    float finalMeanWeight;
    float weightChange;
    float learningScore;
    size_t totalSpikes;
    size_t strengthenedConnections;
    size_t weakenedConnections;
    
    LearningExperimentResults() : initialMeanWeight(0.0f), finalMeanWeight(0.0f),
                                  weightChange(0.0f), learningScore(0.0f),
                                  totalSpikes(0), strengthenedConnections(0),
                                  weakenedConnections(0) {}
};

inline LearningExperimentResults runLearningExperiment(
    std::shared_ptr<Brain> brain,
    uint64_t steps = 1000,
    bool verbose = false) {
    
    LearningExperimentResults results;
    
    // This is a simplified version of the learning experiment from main.cpp
    // In a real implementation, this would be more sophisticated
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        // Record initial weights
        for (const auto& syn : region->getSynapses()) {
            results.initialMeanWeight += syn->getWeight();
        }
        if (!region->getSynapses().empty()) {
            results.initialMeanWeight /= region->getSynapses().size();
        }
        
        // Run learning steps
        for (SimulationStep step = 0; step < steps; ++step) {
            // Inject pattern into sensory neurons
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Sensory && rand() % 20 == 0) {
                    neuron->injectCurrent(30.0f);
                }
            }
            
            brain->step(step, step * 0.001);
        }
        
        // Record final weights
        float finalSum = 0.0f;
        for (const auto& syn : region->getSynapses()) {
            finalSum += syn->getWeight();
        }
        if (!region->getSynapses().empty()) {
            finalSum /= region->getSynapses().size();
            results.finalMeanWeight = finalSum;
            results.weightChange = finalSum - results.initialMeanWeight;
        }
        
        results.totalSpikes = brain->getTotalSpikeCount();
    }
    
    // Compute learning score
    results.learningScore = std::abs(results.weightChange) / std::max(results.initialMeanWeight, 0.01f);
    
    return results;
}

} // namespace nlm
