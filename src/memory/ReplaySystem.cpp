#include "ReplaySystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <fstream>
#include <unordered_set>

namespace nlm {

struct ReplaySystem::Impl {
    std::mt19937 rng;
    Brain* brain;
    
    // Replay timing
    float replayTimeAccumulator;
    float reconsolidationTimeAccumulator;
    float compressionTimeAccumulator;
    
    // Event tracking
    size_t totalReplaySessions;
    size_t totalReconsolidationEvents;
    size_t successfulReplays;
    size_t failedReplays;
    
    // Statistics
    float averageReplayStrength;
    float totalReplayTime;
    float totalCompressionTime;
    
    // Event history
    std::deque<SimulationStep> recentReplaySteps;
    std::deque<SimulationStep> recentReconsolidationSteps;
};

ReplaySystem::ReplaySystem()
    : pImpl(std::make_unique<Impl>())
    , brain_(nullptr)
    , replayEnabled_(true)
    , replayCooldown_(0.0f)
    , spontaneousReplayProbability_(0.3f)
    , reconsolidationThreshold_(0.7f)
    , replayTimer_(0.0f)
    , reconsolidationTimer_(0.0f)
    , compressionTimer_(0.0f)
    , totalReplaySessions_(0)
    , totalReconsolidationEvents_(0)
    , successfulReplays_(0)
    , failedReplays_(0)
    , averageReplayStrength_(0.0f)
    , totalReplayTime_(0.0f)
    , totalCompressionTime_(0.0f)
{
    // Initialize random number generator
    std::random_device rd;
    pImpl->rng.seed(rd());
}

ReplaySystem::~ReplaySystem() = default;

void ReplaySystem::initialize(Brain* brain) {
    brain_ = brain;
    pImpl->brain = brain;
    
    // Initialize replay components
    initializeSpontaneousTriggers();
    initializeCompressionParameters();
    
    // Reset replay state
    replayTimer_ = 0.0f;
    reconsolidationTimer_ = 0.0f;
    compressionTimer_ = 0.0f;
    replayCooldown_ = 0.0f;
    
    // Clear event lists
    replaySessions_.clear();
    reconsolidationUpdates_.clear();
    spontaneousTriggers_.clear();
    targetedRequests_.clear();
    
    // Reset statistics
    pImpl->totalReplaySessions = 0;
    pImpl->totalReconsolidationEvents = 0;
    successfulReplays_ = 0;
    failedReplays_ = 0;
    averageReplayStrength_ = 0.0f;
    totalReplayTime_ = 0.0f;
    totalCompressionTime_ = 0.0f;
    
    NLM_LOG_INFO("ReplaySystem initialized");
}

void ReplaySystem::update(TimestepDuration dt) {
    // Update replay timers
    replayTimer_ += dt;
    reconsolidationTimer_ += dt;
    compressionTimer_ += dt;
    
    // Update cooldown
    if (replayCooldown_ > 0.0f) {
        replayCooldown_ -= dt;
    }
    
    // Process spontaneous replay triggers
    if (replayEnabled_ && !spontaneousTriggers_.empty()) {
        processSpontaneousTriggers();
    }
    
    // Process reconsolidation updates
    processReconsolidationUpdates();
    
    // Process targeted replay requests
    processTargetedRequests();
    
    // Schedule replay events
    scheduleReplayEvents();
}

void ReplaySystem::triggerSpontaneousReplay(const std::vector<SpontaneousReplayTrigger>& triggers) {
    spontaneousTriggers_ = triggers;
    
    if (!replayEnabled_ || triggers.empty() || replayCooldown_ > 0.0f) {
        return;
    }
    
    // Process each trigger
    for (const auto& trigger : triggers) {
        bool triggerActivated = false;
        
        switch (trigger.type) {
            case SpontaneousReplayTrigger::TriggerType::TimeBased:
                triggerActivated = (replayTimer_ > trigger.threshold);
                break;
            case SpontaneousReplayTrigger::TriggerType::RestBased:
                triggerActivated = (replayTimer_ > trigger.threshold && replayCooldown_ <= 0.0f);
                break;
            case SpontaneousReplayTrigger::TriggerType::NoveltyBased:
                if (brain_ && brain_->getEpisodicMemory()) {
                    auto episodes = brain_->getEpisodicMemory()->getRecentEpisodes(10);
                    if (!episodes.empty()) {
                        // Calculate average novelty
                        float totalNovelty = 0.0f;
                        for (const auto* episode : episodes) {
                            totalNovelty += episode->novelty;
                        }
                        float avgNovelty = totalNovelty / episodes.size();
                        triggerActivated = (avgNovelty > trigger.threshold);
                    }
                }
                break;
            case SpontaneousReplayTrigger::TriggerType::RewardBased:
                if (brain_ && brain_->getDopamine()) {
                    triggerActivated = (brain_->getDopamine()->getLevel() > trigger.threshold);
                }
                break;
            case SpontaneousReplayTrigger::TriggerType::AttentionBased:
                if (brain_ && brain_->getAttention()) {
                    triggerActivated = (brain_->getAttention()->getWinners().size() > trigger.threshold);
                }
                break;
            case SpontaneousReplayTrigger::TriggerType::Homeostatic:
                // Would use homeostatic system state
                triggerActivated = (replayTimer_ > trigger.threshold);
                break;
        }
        
        if (triggerActivated) {
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            if (dist(pImpl->rng) < trigger.probability) {
                // Trigger replay with trigger intensity
                triggerReplayEvent(trigger.intensity);
            }
        }
    }
}

void ReplaySystem::triggerReconsolidation(SimulationStep episodeId, const std::vector<float>& newInput) {
    if (!brain_ || !brain_->getEpisodicMemory()) {
        return;
    }
    
    pImpl->totalReconsolidationEvents++;
    
    // Check if episode exists
    auto* episode = brain_->getEpisodicMemory()->getEpisode(episodeId);
    if (!episode) {
        NLM_LOG_WARNING("Cannot reconsolidate: episode " + std::to_string(episodeId) + " not found");
        return;
    }
    
    // Create reconsolidation update
    ReconsolidationUpdate update;
    update.episodeId = episodeId;
    update.updatedStrength = episode->neuronActivations.empty() ? 1.0f : 
        std::accumulate(episode->neuronActivations.begin(), episode->neuronActivations.end(), 0.0f) / episode->neuronActivations.size();
    update.updateMagnitude = 0.0f;
    update.reconsolidationSuccess = 0.0f;
    
    // Calculate new activation pattern (simplified)
    update.updatedActivationPattern = episode->neuronActivations;
    for (size_t i = 0; i < update.updatedActivationPattern.size() && i < newInput.size(); ++i) {
        update.updatedActivationPattern[i] = (update.updatedActivationPattern[i] + newInput[i]) * 0.5f;
    }
    
    // Apply update magnitude based on similarity
    float similarity = calculatePatternSimilarity(episode->neuronActivations, newInput);
    update.updateMagnitude = (1.0f - similarity) * 0.1f;
    update.reconsolidationSuccess = 1.0f - (update.updateMagnitude * 2.0f);
    
    // Update episode with new activation pattern
    // In a full implementation, would update the actual episode
    
    reconsolidationUpdates_.push_back(update);
    
    reconsolidationTimer_ = 0.0f;  // Reset reconsolidation timer
    
    NLM_LOG_INFO("Triggered reconsolidation for episode " + std::to_string(episodeId) + 
                 " with success rate " + std::to_string(update.reconsolidationSuccess));
}

void ReplaySystem::requestTargetedReplay(const TargetedReplayRequest& request) {
    targetedRequests_.push_back(request);
    
    NLM_LOG_INFO("Received targeted replay request for episode " + std::to_string(request.episodeId) + 
                 " with priority " + std::to_string(request.priorityScore));
}

void ReplaySystem::executeReplaySession(const MemoryReplaySession& session) {
    if (!replayEnabled_ || replayCooldown_ > 0.0f) {
        failedReplays_++;
        NLM_LOG_WARNING("Replay session blocked: cooldown active or replay disabled");
        return;
    }
    
    // Calculate replay strength
    float replayStrength = session.replayStrength;
    
    // Check if episode exists in episodic memory
    if (brain_ && brain_->getEpisodicMemory()) {
        // Find the episode (simplified - would match by timestamp)
        auto episodes = brain_->getEpisodicMemory()->getRecentEpisodes(100);
        for (const auto* episode : episodes) {
            if (episode && std::abs(episode->timestamp - session.startTime) < 10) {
                // Trigger the actual replay
                brain_->getEpisodicMemory()->replayEpisode(episode);
                
                // Update replay strength based on episode characteristics
                replayStrength *= (1.0f + episode->novelty * 0.5f);
                replayStrength *= (1.0f + episode->reward * 0.3f);
                
                successfulReplays_++;
                break;
            }
        }
    }
    
    // Record replay session
    recordReplayEvent(session);
    
    // Set cooldown
    replayCooldown_ = 100.0f;  // 100 simulation steps
    
    NLM_LOG_INFO("Executed replay session for episode " + std::to_string(session.episodeId) + 
                 " with strength " + std::to_string(replayStrength));
}

void ReplaySystem::scheduleReplayEvents() {
    if (!replayEnabled_ || replayCooldown_ > 0.0f) {
        return;
    }
    
    // Process targeted requests first (higher priority)
    for (auto it = targetedRequests_.begin(); it != targetedRequests_.end();) {
        if (it->priorityScore > reconsolidationThreshold_) {
            // Create and execute replay session
            MemoryReplaySession session = createReplaySession(it->episodeId, it->priorityScore);
            session.context = it->context;
            
            executeReplaySession(session);
            
            it = targetedRequests_.erase(it);
        } else {
            ++it;
        }
    }
    
    // Process spontaneous replay if no targeted requests
    if (!targetedRequests_.empty()) {
        return;
    }
    
    // Spontaneous replay with probability
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    if (dist(pImpl->rng) < spontaneousReplayProbability_) {
        // Get recent episodes for spontaneous replay
        if (brain_ && brain_->getEpisodicMemory()) {
            auto episodes = brain_->getEpisodicMemory()->getRecentEpisodes(20);
            if (!episodes.empty()) {
                // Select random episode
                size_t idx = static_cast<size_t>(dist(pImpl->rng) * episodes.size());
                auto* episode = episodes[idx];
                
                if (episode) {
                    // Create replay session with episode characteristics
                    MemoryReplaySession session;
                    session.startTime = episode->timestamp;
                    session.episodeId = idx;
                    session.replayStrength = dist(pImpl->rng) * 0.5f + 0.5f;
                    session.temporalCompression = compressionParams_.compressionRatio;
                    session.synapticChange = session.replayStrength * 0.01f;
                    session.predictionError = episode->novelty;
                    session.noveltyValue = episode->novelty;
                    session.rewardAssociation = episode->reward;
                    session.context = "spontaneous_replay";
                    
                    executeReplaySession(session);
                }
            }
        }
    }
}

void ReplaySystem::initializeSpontaneousTriggers() {
    spontaneousTriggers_.clear();
    
    // Time-based trigger
    SpontaneousReplayTrigger timeTrigger;
    timeTrigger.type = SpontaneousReplayTrigger::TriggerType::TimeBased;
    timeTrigger.threshold = 1000.0f;  // 1000 simulation steps
    timeTrigger.probability = 0.2f;
    timeTrigger.cooldown = 100.0f;
    timeTrigger.intensity = 0.7f;
    spontaneousTriggers_.push_back(timeTrigger);
    
    // Novelty-based trigger
    SpontaneousReplayTrigger noveltyTrigger;
    noveltyTrigger.type = SpontaneousReplayTrigger::TriggerType::NoveltyBased;
    noveltyTrigger.threshold = 0.5f;
    noveltyTrigger.probability = 0.4f;
    noveltyTrigger.cooldown = 200.0f;
    noveltyTrigger.intensity = 0.9f;
    spontaneousTriggers_.push_back(noveltyTrigger);
    
    // Reward-based trigger
    SpontaneousReplayTrigger rewardTrigger;
    rewardTrigger.type = SpontaneousReplayTrigger::TriggerType::RewardBased;
    rewardTrigger.threshold = 0.3f;
    rewardTrigger.probability = 0.3f;
    rewardTrigger.cooldown = 150.0f;
    rewardTrigger.intensity = 0.6f;
    spontaneousTriggers_.push_back(rewardTrigger);
}

void ReplaySystem::initializeCompressionParameters() {
    // Set default compression parameters
    compressionParams_.compressionRatio = 0.5f;
    compressionParams_.decayFactor = 0.9f;
    compressionParams_.replayDuration = 100.0f;
    compressionParams_.strengthScaling = 1.0f;
    compressionParams_.noveltyAmplification = 1.5f;
    compressionParams_.rewardAmplification = 1.2f;
}

void ReplaySystem::processSpontaneousTriggers() {
    // This is called by the main update loop
}

void ReplaySystem::processReconsolidationUpdates() {
    // Process pending reconsolidation updates
    for (auto it = reconsolidationUpdates_.begin(); it != reconsolidationUpdates_.end();) {
        if (reconsolidationTimer_ > 50.0f) {  // Reconsolidation complete
            // Update memory based on reconsolidation
            // In a full implementation, would update episodic memory
            
            it = reconsolidationUpdates_.erase(it);
        } else {
            ++it;
        }
    }
}

void ReplaySystem::processTargetedRequests() {
    // This is called by the main update loop
}

void ReplaySystem::calculateReplayStrength(const MemoryReplaySession& session) {
    // Calculate replay strength based on session parameters
    // This is called during replay execution
}

void ReplaySystem::applyTemporalCompression(MemoryReplaySession& session) {
    // Apply temporal compression to replay
    // This makes replay faster than original experience
    
    session.replayDuration = compressionParams_.replayDuration * session.temporalCompression;
    session.replayStrength *= compressionParams_.strengthScaling;
    
    // Apply amplification based on novelty and reward
    if (compressionParams_.noveltyAmplification > 1.0f && session.noveltyValue > 0.0f) {
        session.replayStrength *= compressionParams_.noveltyAmplification;
    }
    
    if (compressionParams_.rewardAmplification > 1.0f && session.rewardAssociation > 0.0f) {
        session.replayStrength *= compressionParams_.rewardAmplification;
    }
}

void ReplaySystem::updateSynapticConnectivity(const MemoryReplaySession& session) {
    if (!brain_) {
        return;
    }
    
    // Update synaptic weights based on replay
    // This is a simplified version
    for (const auto& neuronId : session.reactivatedNeurons) {
        // In a real implementation, would update actual synapses
    }
}

void ReplaySystem::recordReplayEvent(const MemoryReplaySession& session) {
    replaySessions_.push_back(session);
    pImpl->totalReplaySessions++;
    
    // Update statistics
    averageReplayStrength_ = (averageReplayStrength_ * 0.9f) + (session.replayStrength * 0.1f);
    totalReplayTime_ += session.replayDuration;
    
    // Keep only recent sessions
    if (replaySessions_.size() > 100) {
        replaySessions_.erase(replaySessions_.begin());
    }
    
    // Record step for statistics
    if (brain_) {
        pImpl->recentReplaySteps.push_back(brain_->getCurrentStep());
        if (pImpl->recentReplaySteps.size() > 50) {
            pImpl->recentReplaySteps.pop_front();
        }
    }
}

void ReplaySystem::recordReconsolidationEvent(const ReconsolidationUpdate& update) {
    reconsolidationUpdates_.push_back(update);
    
    // Update statistics
    pImpl->totalReconsolidationEvents++;
    
    // Keep only recent updates
    if (reconsolidationUpdates_.size() > 50) {
        reconsolidationUpdates_.erase(reconsolidationUpdates_.begin());
    }
    
    // Record step for statistics
    if (brain_) {
        pImpl->recentReconsolidationSteps.push_back(brain_->getCurrentStep());
        if (pImpl->recentReconsolidationSteps.size() > 20) {
            pImpl->recentReconsolidationSteps.pop_front();
        }
    }
}

MemoryReplaySession ReplaySystem::createReplaySession(SimulationStep episodeId, float strength) {
    MemoryReplaySession session;
    session.startTime = brain_ ? brain_->getCurrentStep() : 0;
    session.endTime = session.startTime + compressionParams_.replayDuration;
    session.episodeId = episodeId;
    session.replayStrength = strength;
    session.temporalCompression = compressionParams_.compressionRatio;
    session.synapticChange = strength * 0.01f;
    session.context = "targeted_replay";
    
    // Get episode details for replay context
    if (brain_ && brain_->getEpisodicMemory()) {
        auto* episode = brain_->getEpisodicMemory()->getEpisode(episodeId);
        if (episode) {
            session.predictionError = episode->novelty;
            session.noveltyValue = episode->novelty;
            session.rewardAssociation = episode->reward;
            session.activationPattern = episode->neuronActivations;
            session.reactivatedNeurons = episode->activeNeurons;
        }
    }
    
    return session;
}

float ReplaySystem::calculateReplayBenefit(const MemoryReplaySession& session) const {
    // Calculate expected benefit of a replay session
    float benefit = session.replayStrength * 0.5f;
    
    if (session.predictionError > 0.0f) {
        benefit += session.predictionError * 0.3f;
    }
    
    if (session.noveltyValue > 0.0f) {
        benefit += session.noveltyValue * 0.2f;
    }
    
    if (session.rewardAssociation > 0.0f) {
        benefit += session.rewardAssociation * 0.2f;
    }
    
    if (session.temporalCompression > 1.0f) {
        benefit *= session.temporalCompression;
    }
    
    return benefit;
}

bool ReplaySystem::shouldReconsolidate(SimulationStep episodeId) const {
    // Check if episode should be reconsolidated
    // In a real implementation, would consider multiple factors
    
    if (brain_ && brain_->getEpisodicMemory()) {
        auto* episode = brain_->getEpisodicMemory()->getEpisode(episodeId);
        if (episode) {
            // Reconsolidate based on episode age and novelty
            float age = static_cast<float>(brain_->getCurrentStep()) - episode->timestamp;
            float ageFactor = 1.0f / (1.0f + age * 0.01f);
            float noveltyFactor = episode->novelty;
            
            return (ageFactor * noveltyFactor) > reconsolidationThreshold_;
        }
    }
    
    return false;
}

float ReplaySystem::calculatePatternSimilarity(const std::vector<float>& patternA, const std::vector<float>& patternB) const {
    if (patternA.empty() || patternB.empty()) {
        return 0.0f;
    }
    
    size_t minSize = std::min(patternA.size(), patternB.size());
    if (minSize == 0) {
        return 0.0f;
    }
    
    float dotProduct = 0.0f;
    float normA = 0.0f, normB = 0.0f;
    
    for (size_t i = 0; i < minSize; ++i) {
        dotProduct += patternA[i] * patternB[i];
        normA += patternA[i] * patternA[i];
        normB += patternB[i] * patternB[i];
    }
    
    if (normA > 0.0f && normB > 0.0f) {
        return dotProduct / std::sqrt(normA * normB);
    }
    
    return 0.0f;
}

void ReplaySystem::reset() {
    replaySessions_.clear();
    reconsolidationUpdates_.clear();
    spontaneousTriggers_.clear();
    targetedRequests_.clear();
    
    // Reset timers
    replayTimer_ = 0.0f;
    reconsolidationTimer_ = 0.0f;
    compressionTimer_ = 0.0f;
    replayCooldown_ = 0.0f;
    
    NLM_LOG_INFO("ReplaySystem reset");
}

} // namespace nlm
