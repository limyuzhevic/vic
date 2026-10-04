// Memory integration implementation
// This file contains the implementation of the comprehensive memory integration pipeline

#include "MemoryIntegration.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Reward.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <sstream>

namespace nlm {

struct MemoryIntegration::Impl {
    class Brain* brain;
    MemoryIntegrationConfig config;
    
    // Memory state tracking
    size_t stepsSinceLastReplay;
    size_t stepsSinceConsolidation;
    double lastReplayTime;
    
    // Current experience state
    SensoryPercept currentPercept;
    MotorCommand currentAction;
    float currentReward;
    float currentPredictionError;
    double experienceTimestamp;
    
    // Memory statistics
    size_t totalExperiencesCaptured;
    size_t totalReplayEvents;
    size_t totalConsolidations;
    
    // Performance tracking
    float averageWorkingMemoryLoad;
    float averageEpisodicMemoryStrength;
    float averageAssociationStrength;
    
    Impl() : brain(nullptr), stepsSinceLastReplay(0), stepsSinceConsolidation(0), 
             lastReplayTime(0.0), currentReward(0.0f), currentPredictionError(0.0f),
             experienceTimestamp(0.0), totalExperiencesCaptured(0), 
             totalReplayEvents(0), totalConsolidations(0),
             averageWorkingMemoryLoad(0.0f), averageEpisodicMemoryStrength(0.0f),
             averageAssociationStrength(0.0f) {
        initializeDefaultConfig();
    }
    
    void initializeDefaultConfig() {
        // Working memory configuration
        config.workingMemory.capacity = 100;
        config.workingMemory.decayRate = 0.01f;
        config.workingMemory.competitionStrength = 1.0f;
        config.workingMemory.maintenanceCurrent = 0.5f;
        config.workingMemory.threshold = 0.3f;
        
        // Episodic memory configuration
        config.episodicMemory.maxEpisodes = 1000;
        config.episodicMemory.consolidationThreshold = 0.5f;
        config.episodicMemory.replayInterval = 100;
        config.episodicMemory.decayRate = 0.001f;
        config.episodicMemory.integrationWeight = 0.3f;
        
        // Associative memory configuration
        config.associativeMemory.associationStrength = 0.5f;
        config.associativeMemory.patternOverlap = 0.7f;
        config.associativeMemory.reinforcementRate = 0.1f;
        config.associativeMemory.inhibitionRate = 0.05f;
        
        // Integration configuration
        config.integration.attentionBoost = 0.2f;
        config.integration.conceptFormationWeight = 0.4f;
        config.integration.planningInfluence = 0.3f;
        config.integration.noveltyNovelty = 0.5f;
    }
};

MemoryIntegration::MemoryIntegration() : pImpl(new Impl) {}

MemoryIntegration::~MemoryIntegration() = default;

bool MemoryIntegration::initialize(class Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize MemoryIntegration with null brain");
        return false;
    }
    
    pImpl->brain = brain;
    
    // Initialize memory systems
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    auto* associativeMemory = brain->getAssociativeMemory();
    
    if (!workingMemory || !episodicMemory || !associativeMemory) {
        NLM_LOG_ERROR("Failed to initialize memory systems");
        return false;
    }
    
    // Configure memory systems
    workingMemory->setCapacity(pImpl->config.workingMemory.capacity);
    workingMemory->setDecayRate(pImpl->config.workingMemory.decayRate);
    episodicMemory->setMaxEpisodes(pImpl->config.episodicMemory.maxEpisodes);
    episodicMemory->setDecayRate(pImpl->config.episodicMemory.decayRate);
    associativeMemory->setAssociationStrength(pImpl->config.associativeMemory.associationStrength);
    
    NLM_LOG_INFO("MemoryIntegration initialized with brain reference");
    return true;
}

void MemoryIntegration::configure(class Config* config) {
    if (!config) {
        NLM_LOG_WARNING("No configuration provided, using defaults");
        return;
    }
    
    // Working memory configuration
    pImpl->config.workingMemory.capacity = 
        static_cast<size_t>(config->getOr<size_t>("working_memory_capacity", 
                                                 pImpl->config.workingMemory.capacity));
    pImpl->config.workingMemory.decayRate = 
        config->getOr<float>("working_memory_decay_rate", 
                            pImpl->config.workingMemory.decayRate);
    pImpl->config.workingMemory.competitionStrength = 
        config->getOr<float>("working_memory_competition_strength", 
                            pImpl->config.workingMemory.competitionStrength);
    
    // Episodic memory configuration
    pImpl->config.episodicMemory.maxEpisodes = 
        static_cast<size_t>(config->getOr<size_t>("episodic_memory_max_episodes",
                                                 pImpl->config.episodicMemory.maxEpisodes));
    pImpl->config.episodicMemory.consolidationThreshold = 
        config->getOr<float>("episodic_memory_consolidation_threshold",
                            pImpl->config.episodicMemory.consolidationThreshold);
    pImpl->config.episodicMemory.replayInterval = 
        static_cast<size_t>(config->getOr<size_t>("episodic_memory_replay_interval",
                                                 pImpl->config.episodicMemory.replayInterval));
    
    // Integration configuration
    pImpl->config.integration.attentionBoost = 
        config->getOr<float>("memory_integration_attention_boost",
                           pImpl->config.integration.attentionBoost);
    pImpl->config.integration.conceptFormationWeight = 
        config->getOr<float>("memory_integration_concept_formation_weight",
                           pImpl->config.integration.conceptFormationWeight);
    
    NLM_LOG_INFO("MemoryIntegration configured from configuration file");
}

void MemoryIntegration::update(class Brain* brain, double timestep) {
    if (!brain) return;
    
    // Update working memory
    updateWorkingMemory(brain, timestep);
    
    // Update episodic memory
    updateEpisodicMemory(brain, timestep);
    
    // Update associative memory
    updateAssociativeMemory(brain, timestep);
    
    // Integrate with cognition systems
    updateAttention(brain);
    updateConceptFormation(brain);
    updatePlanning(brain);
    
    // Track replay and consolidation
    pImpl->stepsSinceLastReplay++;
    pImpl->stepsSinceConsolidation++;
    
    // Trigger replay if needed
    if (pImpl->stepsSinceLastReplay >= pImpl->config.episodicMemory.replayInterval) {
        triggerReplay(brain);
        pImpl->stepsSinceLastReplay = 0;
    }
    
    // Trigger consolidation if needed
    if (pImpl->stepsSinceConsolidation >= 1000) {  // Consolidate every 1000 steps
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            episodicMemory->consolidate(pImpl->config.episodicMemory.consolidationThreshold);
        }
        pImpl->stepsSinceConsolidation = 0;
        pImpl->totalConsolidations++;
    }
}

void MemoryIntegration::processSensoryInput(class Brain* brain, const SensoryPercept& percept) {
    if (!brain) return;
    
    pImpl->currentPercept = percept;
    
    // Store sensory input in working memory
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        std::vector<float> sensoryPattern = MemoryIntegrationHelpers::encodeSensoryInput(percept);
        workingMemory->store(sensoryPattern, 1.0f);
    }
    
    // Also store in episodic memory as a new experience
    // This will be done when action and reward are available
}

void MemoryIntegration::updateWorkingMemory(class Brain* brain, double timestep) {
    auto* workingMemory = brain->getWorkingMemory();
    if (!workingMemory) return;
    
    // Update working memory traces
    workingMemory->update(static_cast<float>(timestep));
    
    // Track performance
    float memoryLoad = workingMemory->getMemoryActivity();
    pImpl->averageWorkingMemoryLoad = 
        (pImpl->averageWorkingMemoryLoad * 0.9f) + (memoryLoad * 0.1f);
}

void MemoryIntegration::updateEpisodicMemory(class Brain* brain, double timestep) {
    auto* episodicMemory = brain->getEpisodicMemory();
    if (!episodicMemory) return;
    
    // If we have a current experience, store it
    if (pImpl->currentAction != MotorCommand::Wait) {
        captureExperience(brain, pImpl->currentPercept, pImpl->currentAction,
                         pImpl->currentReward, pImpl->currentPredictionError);
        pImpl->currentAction = MotorCommand::Wait; // Reset for next experience
    }
    
    // Decay episodic memories
    episodicMemory->update(static_cast<float>(timestep));
    
    // Track performance
    float memoryStrength = episodicMemory->getAverageMemoryStrength();
    pImpl->averageEpisodicMemoryStrength = 
        (pImpl->averageEpisodicMemoryStrength * 0.9f) + (memoryStrength * 0.1f);
}

void MemoryIntegration::updateAssociativeMemory(class Brain* brain, double timestep) {
    auto* associativeMemory = brain->getAssociativeMemory();
    if (!associativeMemory) return;
    
    // Update associations based on current experience
    if (pImpl->currentAction != MotorCommand::Wait) {
        std::vector<float> perceptPattern = MemoryIntegrationHelpers::encodeSensoryInput(pImpl->currentPercept);
        std::vector<float> actionPattern = MemoryIntegrationHelpers::encodeMotorCommand(pImpl->currentAction);
        
        float reward = pImpl->currentReward;
        
        // Create association between percept and action
        associativeMemory->createAssociation(perceptPattern, actionPattern, reward);
    }
    
    // Update association strengths
    associativeMemory->update(static_cast<float>(timestep));
    
    // Track performance
    float associationStrength = associativeMemory->getAverageAssociationStrength();
    pImpl->averageAssociationStrength = 
        (pImpl->averageAssociationStrength * 0.9f) + (associationStrength * 0.1f);
}

void MemoryIntegration::updateAttention(class Brain* brain) {
    auto* attention = brain->getAttention();
    if (!attention) return;
    
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
        std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
        attention->processCompetition(competitors);
        
        // Apply attention boost to working memory winners
        for (const auto& neuron : attention->getWinners()) {
            attention->applyBottomUpSalience(neuron, pImpl->config.integration.attentionBoost);
        }
    }
}

void MemoryIntegration::updateConceptFormation(class Brain* brain) {
    auto* conceptFormation = brain->getConceptFormation();
    if (!conceptFormation) return;
    
    // Use episodic memory to form concepts
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        // Get recent episodes to form concepts
        auto recentEpisodes = episodicMemory->getRecentEpisodes(10);
        
        for (const auto* episode : recentEpisodes) {
            // Extract patterns from the episode
            std::vector<float> pattern;
            for (float activation : episode->neuronActivations) {
                pattern.push_back(activation);
            }
            
            // Form concept from the pattern
            conceptFormation->processPattern(pattern);
        }
    }
}

void MemoryIntegration::updatePlanning(class Brain* brain) {
    auto* planner = brain->getPlanner();
    if (!planner) return;
    
    // Get current state and action from memory systems
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    auto* associativeMemory = brain->getAssociativeMemory();
    
    if (workingMemory && episodicMemory && associativeMemory) {
        std::vector<NeuronId> workingMemoryNeurons = workingMemory->getMemoryNeurons();
        auto recentEpisodes = episodicMemory->getRecentEpisodes(5);
        
        // Create planning context from memory systems
        std::vector<float> planningContext;
        
        // Add working memory activity
        for (const auto& neuron : workingMemoryNeurons) {
            planningContext.push_back(workingMemory->getNeuronActivation(neuron));
        }
        
        // Add episodic memory strength
        float episodicStrength = 0.0f;
        for (const auto* episode : recentEpisodes) {
            episodicStrength += episode->strength;
        }
        episodicStrength /= recentEpisodes.size();
        planningContext.push_back(episodicStrength);
        
        // Add associative memory strength
        float associationStrength = associativeMemory->getAverageAssociationStrength();
        planningContext.push_back(associationStrength);
        
        // Plan action using memory systems
        planner->planAction(planningContext, pImpl->config.integration.planningInfluence);
    }
}

void MemoryIntegration::captureExperience(class Brain* brain, 
                                           const SensoryPercept& percept,
                                           const MotorCommand& action,
                                           float reward,
                                           float predictionError) {
    auto* episodicMemory = brain->getEpisodicMemory();
    auto* associativeMemory = brain->getAssociativeMemory();
    
    if (!episodicMemory || !associativeMemory) return;
    
    // Capture sensory pattern
    std::vector<float> sensoryPattern = MemoryIntegrationHelpers::encodeSensoryInput(percept);
    
    // Capture action pattern
    std::vector<float> actionPattern = MemoryIntegrationHelpers::encodeMotorCommand(action);
    
    // Create episode
    EpisodicMemoryItem episode;
    episode.timestamp = static_cast<SimulationStep>(pImpl->experienceTimestamp);
    episode.sensoryPattern = sensoryPattern;
    episode.actionPattern = actionPattern;
    episode.reward = reward;
    episode.predictionError = predictionError;
    episode.strength = 1.0f - std::abs(predictionError);
    
    // Store neural activity if available
    if (brain) {
        for (const auto& region : brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (Neuron* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || 
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                        episode.activeNeurons.push_back(neuron->getId());
                        episode.neuronActivations.push_back(
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                    }
                }
            }
        }
    }
    
    // Store in episodic memory
    episodicMemory->storeEpisode(episode);
    pImpl->totalExperiencesCaptured++;
    
    // Update associative memory
    associativeMemory->createAssociation(sensoryPattern, actionPattern, reward);
}

void MemoryIntegration::triggerReplay(class Brain* brain) {
    auto* episodicMemory = brain->getEpisodicMemory();
    if (!episodicMemory) return;
    
    // Get episodes for replay
    auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
    
    for (const auto* episode : episodesToReplay) {
        episodicMemory->replayEpisode(episode);
    }
    
    pImpl->totalReplayEvents++;
    NLM_LOG_INFO("Triggered memory replay with " + std::to_string(episodesToReplay.size()) + " episodes");
}

void MemoryIntegration::reset(class Brain* brain) {
    if (!brain) return;
    
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    auto* associativeMemory = brain->getAssociativeMemory();
    
    if (workingMemory) workingMemory->clear();
    if (episodicMemory) episodicMemory->clear();
    if (associativeMemory) associativeMemory->clear();
    
    pImpl->stepsSinceLastReplay = 0;
    pImpl->stepsSinceConsolidation = 0;
    pImpl->totalExperiencesCaptured = 0;
    pImpl->totalReplayEvents = 0;
    pImpl->totalConsolidations = 0;
}

void MemoryIntegration::logMemoryStatus(class Brain* brain) const {
    NLM_LOG_INFO("=== Memory Integration Status ===");
    NLM_LOG_INFO("Working memory capacity: " + std::to_string(pImpl->config.workingMemory.capacity));
    NLM_LOG_INFO("Working memory decay rate: " + std::to_string(pImpl->config.workingMemory.decayRate));
    NLM_LOG_INFO("Episodic memory max episodes: " + std::to_string(pImpl->config.episodicMemory.maxEpisodes));
    NLM_LOG_INFO("Episodic memory consolidation threshold: " + std::to_string(pImpl->config.episodicMemory.consolidationThreshold));
    NLM_LOG_INFO("Associative memory association strength: " + std::to_string(pImpl->config.associativeMemory.associationStrength));
    
    NLM_LOG_INFO("Experiences captured: " + std::to_string(pImpl->totalExperiencesCaptured));
    NLM_LOG_INFO("Replay events: " + std::to_string(pImpl->totalReplayEvents));
    NLM_LOG_INFO("Consolidations: " + std::to_string(pImpl->totalConsolidations));
    
    NLM_LOG_INFO("Average working memory load: " + std::to_string(pImpl->averageWorkingMemoryLoad));
    NLM_LOG_INFO("Average episodic memory strength: " + std::to_string(pImpl->averageEpisodicMemoryStrength));
    NLM_LOG_INFO("Average association strength: " + std::to_string(pImpl->averageAssociationStrength));
}

// Memory integration helper functions
namespace MemoryIntegrationHelpers {
    
    std::vector<float> encodeSensoryInput(const SensoryPercept& percept) {
        std::vector<float> pattern;
        
        // Vision (16x16)
        const auto& vision = percept.getVision();
        pattern.insert(pattern.end(), vision.begin(), vision.end());
        
        // Touch (8)
        const auto& touch = percept.getTouch();
        pattern.insert(pattern.end(), touch.begin(), touch.end());
        
        // Internal (4)
        const auto& intern = percept.getInternal();
        pattern.insert(pattern.end(), intern.begin(), intern.end());
        
        // Proprioception (6)
        const auto& proprio = percept.getProprioception();
        pattern.insert(pattern.end(), proprio.begin(), proprio.end());
        
        // Normalize pattern
        float maxVal = 0.0f;
        for (float val : pattern) {
            maxVal = std::max(maxVal, std::abs(val));
        }
        
        if (maxVal > 0.0f) {
            for (size_t i = 0; i < pattern.size(); ++i) {
                pattern[i] /= maxVal;
            }
        }
        
        return pattern;
    }
    
    std::vector<float> encodeMotorCommand(const MotorCommand& action) {
        // One-hot encoding of motor commands
        std::vector<float> pattern(6, 0.0f);
        
        switch (action) {
            case MotorCommand::MoveForward: pattern[0] = 1.0f; break;
            case MotorCommand::MoveBackward: pattern[1] = 1.0f; break;
            case MotorCommand::TurnLeft: pattern[2] = 1.0f; break;
            case MotorCommand::TurnRight: pattern[3] = 1.0f; break;
            case MotorCommand::LookLeft: pattern[4] = 1.0f; break;
            case MotorCommand::LookRight: pattern[5] = 1.0f; break;
            case MotorCommand::Interact: pattern[0] = 0.5f; pattern[1] = 0.5f; break;
            case MotorCommand::Wait: /* Already zero */ break;
        }
        
        return pattern;
    }
    
    float computeSimilarity(const std::vector<float>& pattern1, 
                           const std::vector<float>& pattern2) {
        if (pattern1.size() != pattern2.size() || pattern1.empty()) {
            return 0.0f;
        }
        
        float dotProduct = 0.0f;
        for (size_t i = 0; i < pattern1.size(); ++i) {
            dotProduct += pattern1[i] * pattern2[i];
        }
        
        return std::abs(dotProduct) / static_cast<float>(pattern1.size());
    }
    
    size_t generateEpisodicKey(const SensoryPercept& percept,
                               const MotorCommand& action,
                               float reward) {
        // Simple hash based on percept and action
        std::hash<float> hasher;
        
        size_t hash = hasher(static_cast<float>(action));
        
        const auto& vision = percept.getVision();
        for (size_t i = 0; i < std::min(vision.size(), static_cast<size_t>(10)); ++i) {
            hash ^= hasher(vision[i]);
        }
        
        return hash;
    }
    
    float computeConsolidationStrength(const EpisodicMemoryItem& episode,
                                        float currentTime) {
        float strength = episode.strength;
        
        // Decay based on time
        float timeDecay = std::exp(-(currentTime - episode.timestamp) * 0.001f);
        strength *= timeDecay;
        
        // Boost based on prediction error
        strength *= (1.0f + std::abs(episode.predictionError));
        
        // Boost based on reward
        strength *= (1.0f + episode.reward);
        
        return std::min(1.0f, strength);
    }
    
} // namespace MemoryIntegrationHelpers

} // namespace nlm
