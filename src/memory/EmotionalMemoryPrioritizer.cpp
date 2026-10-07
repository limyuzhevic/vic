#include "EmotionalMemoryPrioritizer.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct EmotionalMemoryPrioritizer::Impl {
    // Attention dynamics
    std::vector<float> attentionWeights;
    std::vector<float> emotionalStates;
    
    // Memory saturation tracking
    float currentMemoryLoad;
    float saturationThreshold;
    
    // Neuromodulation history
    std::vector<float> dopamineHistory;
    std::vector<float> noveltyHistory;
    std::vector<float> curiosityHistory;
    
    Impl() : currentMemoryLoad(0.0f), saturationThreshold(0.8f) {
        attentionWeights.reserve(100);
        emotionalStates.reserve(100);
        dopamineHistory.reserve(1000);
        noveltyHistory.reserve(1000);
        curiosityHistory.reserve(1000);
    }
};

EmotionalMemoryPrioritizer::EmotionalMemoryPrioritizer()
    : pImpl(new Impl)
    , brain_(nullptr)
    , dopamine_(nullptr)
    , novelty_(nullptr)
    , curiosity_(nullptr)
    , emotionalWeight_(0.4f)
    , recencyWeight_(0.2f)
    , rewardWeight_(0.2f)
    , noveltyWeight_(0.1f)
    , priorityThreshold_(0.5f)
    , consolidationThreshold_(0.7f)
    , currentEmotionalInfluence_(0.0f)
    , averageNeuromodulation_(0.0f)
    , lastConsolidationStep_(0)
    , totalPrioritized_(0)
    , accumulatedValenceScore_(0.0f)
    , highPriorityCount_(0)
    , mediumPriorityCount_(0)
    , lowPriorityCount_(0)
    , accumulatedEmotionalInfluence_(0.0f)
{
}

EmotionalMemoryPrioritizer::~EmotionalMemoryPrioritizer() = default;

void EmotionalMemoryPrioritizer::initialize(Brain* brain, Dopamine* dopamine, Novelty* novelty, Curiosity* curiosity) {
    pImpl->brain = brain;
    brain_ = brain;
    dopamine_ = dopamine;
    novelty_ = novelty;
    curiosity_ = curiosity;
    
    // Initialize attention weights
    pImpl->attentionWeights.resize(100, 1.0f);
    pImpl->emotionalStates.resize(100, 0.0f);
    
    NLM_LOG_INFO("EmotionalMemoryPrioritizer initialized with neuromodulation integration");
}

std::vector<const class EpisodicMemoryItem*> EmotionalMemoryPrioritizer::prioritizeEpisodes(
    const std::vector<const class EpisodicMemoryItem*>& episodes,
    size_t maxResults,
    float timeWindowFactor) const {

    totalPrioritized_++;
    
    std::vector<PrioritizedEpisode> prioritized;
    prioritized.reserve(episodes.size());
    
    // Convert to prioritized episodes
    for (const auto* episode : episodes) {
        prioritized.emplace_back(episode);
    }
    
    // Calculate priority scores
    for (auto& prioritizedEp : prioritized) {
        prioritizedEp.valenceScore = calculateEmotionalValence(*prioritizedEp.episode);
        prioritizedEp.recencyScore = calculateRecencyScore(*prioritizedEp.episode);
        prioritizedEp.rewardScore = calculateRewardScore(*prioritizedEp.episode);
        prioritizedEp.noveltyScore = calculateNoveltyScore(*prioritizedEp.episode);
        
        // Calculate final weighted score
        float baseScore = 
            prioritizedEp.valenceScore * emotionalWeight_ +
            prioritizedEp.recencyScore * recencyWeight_ +
            prioritizedEp.rewardScore * rewardWeight_ +
            prioritizedEp.noveltyScore * noveltyWeight_;
        
        // Apply emotional modulation
        applyEmotionalModulation(prioritized, prioritizedEp);
        applyAttentionBias(prioritized, prioritizedEp);
        applyMemorySaturation(prioritized, prioritizedEp);
        
        prioritizedEp.finalScore = baseScore * currentEmotionalInfluence_;
    }
    
    // Sort by priority score (descending)
    std::sort(prioritized.begin(), prioritized.end(),
              [](const PrioritizedEpisode& a, const PrioritizedEpisode& b) {
                  return a.finalScore > b.finalScore;
              });
    
    // Apply dynamic threshold filtering
    float dynamicThreshold = getDynamicPriorityThreshold();
    
    std::vector<const class EpisodicMemoryItem*> results;
    results.reserve(std::min(maxResults, prioritized.size()));
    
    for (size_t i = 0; i < std::min(maxResults, prioritized.size()); ++i) {
        if (prioritized[i].finalScore >= dynamicThreshold) {
            results.push_back(prioritized[i].episode);
            
            // Update statistics
            accumulatedValenceScore_ += prioritized[i].finalScore;
            accumulatedEmotionalInfluence_ += currentEmotionalInfluence_;
            
            if (prioritized[i].finalScore >= consolidationThreshold_) {
                highPriorityCount_++;
            } else if (prioritized[i].finalScore >= priorityThreshold_) {
                mediumPriorityCount_++;
            } else {
                lowPriorityCount_++;
            }
        }
    }
    
    return results;
}

float EmotionalMemoryPrioritizer::calculateEmotionalValence(const class EpisodicMemoryItem& episode) const {
    // Base valence from reward and emotional content
    float baseValence = episode.reward * 0.6f + (episode.novelty > 0.5f ? 0.3f : -0.2f);
    
    // Factor in action type (positive actions increase valence)
    switch (episode.action) {
        case ActionType::Explore:
            baseValence += 0.2f;
            break;
        case ActionType::Interact:
            baseValence += 0.1f;
            break;
        case ActionType::Wait:
            baseValence += 0.0f;
            break;
        case ActionType::Fight:
            baseValence -= 0.3f;
            break;
        default:
            break;
    }
    
    // Apply neuromodulation scaling
    float dopamineFactor = getDopamineModulation();
    float noveltyFactor = getNoveltyModulation();
    
    float finalValence = baseValence * 
                         (1.0f + dopamineFactor * 0.3f) * 
                         (1.0f + noveltyFactor * 0.2f);
    
    // Clamp to reasonable range
    return std::clamp(finalValence, -1.0f, 1.0f);
}

float EmotionalMemoryPrioritizer::calculateRecencyScore(const class EpisodicMemoryItem& episode) const {
    // Recency decays exponentially
    float recencyDecay = std::exp(-episode.age * 0.001f); // Decay constant
    
    // Recent episodes with positive valence get boosted
    float score = recencyDecay * (1.0f + std::max(0.0f, episode.reward) * 0.2f);
    
    return std::clamp(score, 0.0f, 1.0f);
}

float EmotionalMemoryPrioritizer::calculateRewardScore(const class EpisodicMemoryItem& episode) const {
    // Direct reward influence
    float rewardScore = std::min(episode.reward, 1.0f);
    
    // Reward prediction error influence
    if (dopamine_ && episode.age > 0) {
        float predictionError = episode.reward - 0.5f; // Assuming 0.5f was expected reward
        rewardScore *= (1.0f + std::abs(predictionError) * 0.5f);
    }
    
    return std::clamp(rewardScore, 0.0f, 1.0f);
}

float EmotionalMemoryPrioritizer::calculateNoveltyScore(const class EpisodicMemoryItem& episode) const {
    if (!novelty_) return 0.0f;
    
    // Use novelty detector's current state
    float currentNovelty = novelty_->getLevel();
    
    // Episodes from novel time periods get higher scores
    float noveltyScore = episode.novelty * currentNovelty;
    
    // Curiosity amplification for novel experiences
    if (curiosity_) {
        float curiosityLevel = curiosity_->getLevel();
        noveltyScore *= (1.0f + curiosityLevel * 0.5f);
    }
    
    return std::clamp(noveltyScore, 0.0f, 1.0f);
}

float EmotionalMemoryPrioritizer::calculateEmotionalValenceComponent(const class EpisodicMemoryItem& episode) const {
    // Additional emotional processing
    float stressInfluence = calculateStressInfluence(episode);
    float empathyInfluence = calculateEmpathyInfluence(episode);
    
    float emotionalComponent = 
        episode.reward * 0.4f +
        stressInfluence * 0.3f +
        empathyInfluence * 0.3f;
    
    return std::clamp(emotionalComponent, -1.0f, 1.0f);
}

float EmotionalMemoryPrioritizer::calculateStressInfluence(const class EpisodicMemoryItem& episode) const {
    // Simplified stress calculation
    float stress = 0.0f;
    
    if (episode.action == ActionType::Fight) {
        stress += 0.5f;
    }
    
    if (episode.reward < 0.0f) {
        stress += 0.3f;
    }
    
    if (episode.novelty > 0.8f) {
        stress += 0.2f;
    }
    
    return std::clamp(stress, 0.0f, 1.0f);
}

float EmotionalMemoryPrioritizer::calculateEmpathyInfluence(const class EpisodicMemoryItem& episode) const {
    // Empathy influence based on episode characteristics
    float empathy = 0.0f;
    
    // Episodes involving interaction with others increase empathy potential
    if (episode.action == ActionType::Interact) {
        empathy += 0.4f;
    }
    
    // High reward episodes increase empathy through positive association
    if (episode.reward > 0.7f) {
        empathy += 0.3f;
    }
    
    // Novel experiences increase empathy through learning
    if (episode.novelty > 0.6f) {
        empathy += 0.2f;
    }
    
    return std::clamp(empathy, 0.0f, 1.0f);
}

void EmotionalMemoryPrioritizer::updateNeuromodulationState() {
    float totalModulation = 0.0f;
    int count = 0;
    
    if (dopamine_) {
        float dopamineLevel = dopamine_->getLevel();
        totalModulation += dopamineLevel;
        count++;
        
        // Update history
        pImpl->dopamineHistory.push_back(dopamineLevel);
        if (pImpl->dopamineHistory.size() > 1000) {
            pImpl->dopamineHistory.erase(pImpl->dopamineHistory.begin());
        }
    }
    
    if (novelty_) {
        float noveltyLevel = novelty_->getLevel();
        totalModulation += noveltyLevel;
        count++;
        
        pImpl->noveltyHistory.push_back(noveltyLevel);
        if (pImpl->noveltyHistory.size() > 1000) {
            pImpl->noveltyHistory.erase(pImpl->noveltyHistory.begin());
        }
    }
    
    if (curiosity_) {
        float curiosityLevel = curiosity_->getLevel();
        totalModulation += curiosityLevel;
        count++;
        
        pImpl->curiosityHistory.push_back(curiosityLevel);
        if (pImpl->curiosityHistory.size() > 1000) {
            pImpl->curiosityHistory.erase(pImpl->curiosityHistory.begin());
        }
    }
    
    if (count > 0) {
        averageNeuromodulation_ = totalModulation / count;
    }
    
    // Update emotional influence based on neuromodulation
    currentEmotionalInfluence_ = std::min(averageNeuromodulation_ * 1.5f, 1.0f);
}

float EmotionalMemoryPrioritizer::getDopamineModulation() const {
    if (!dopamine_) return 0.0f;
    return dopamine_->getPlasticityFactor();
}

float EmotionalMemoryPrioritizer::getNoveltyModulation() const {
    if (!novelty_) return 0.0f;
    return novelty_->getLevel();
}

float EmotionalMemoryPrioritizer::getCuriosityModulation() const {
    if (!curiosity_) return 0.0f;
    return curiosity_->getLevel();
}

float EmotionalMemoryPrioritizer::adjustThresholds(TimestepDuration dt) const {
    // Dynamic threshold adjustment based on brain state
    float adjustment = std::sin(static_cast<float>(dt) * 0.001f) * 0.1f;
    
    // Adjust based on memory load
    float loadFactor = pImpl->currentMemoryLoad / pImpl->saturationThreshold;
    float loadAdjustment = (loadFactor - 0.5f) * 0.2f;
    
    return priorityThreshold_ + adjustment + loadAdjustment;
}

float EmotionalMemoryPrioritizer::getDynamicPriorityThreshold() const {
    return adjustThresholds(1); // Use default timestep
}

float EmotionalMemoryPrioritizer::getDynamicConsolidationThreshold() const {
    float base = consolidationThreshold_;
    
    // Increase consolidation threshold during high neuromodulation
    float neuromodulationBonus = averageNeuromodulation_ * 0.3f;
    
    // Decrease during low memory load
    float loadFactor = 1.0f - pImpl->currentMemoryLoad / pImpl->saturationThreshold;
    
    return std::clamp(base + neuromodulationBonus + loadFactor * 0.1f, 0.3f, 0.9f);
}

void EmotionalMemoryPrioritizer::applyEmotionalModulation(std::vector<PrioritizedEpisode>& episodes, PrioritizedEpisode& current) const {
    // Apply current emotional state to all episodes
    for (auto& ep : episodes) {
        // Emotional contagion - similar emotional states
        float emotionalSimilarity = std::abs(current.valenceScore - ep.valenceScore);
        ep.finalScore *= (1.0f - emotionalSimilarity * 0.1f);
    }
}

void EmotionalMemoryPrioritizer::applyAttentionBias(std::vector<PrioritizedEpisode>& episodes, PrioritizedEpisode& current) const {
    // Attention bias based on current emotional state
    for (auto& ep : episodes) {
        // High valence episodes get attention boost
        if (current.valenceScore > 0.5f && ep.valenceScore > 0.0f) {
            ep.finalScore *= 1.1f;
        }
        
        // Low valence episodes get attention boost when current is negative
        if (current.valenceScore < -0.5f && ep.valenceScore < 0.0f) {
            ep.finalScore *= 1.1f;
        }
    }
}

void EmotionalMemoryPrioritizer::applyMemorySaturation(std::vector<PrioritizedEpisode>& episodes, PrioritizedEpisode& current) const {
    // Memory saturation effect - limit number of high-priority memories
    int highPriorityCount = 0;
    for (const auto& ep : episodes) {
        if (ep.finalScore >= consolidationThreshold_) {
            highPriorityCount++;
        }
    }
    
    float saturationEffect = 1.0f - (static_cast<float>(highPriorityCount) / episodes.size()) * 0.2f;
    
    for (auto& ep : episodes) {
        ep.finalScore *= saturationEffect;
    }
}

void EmotionalMemoryPrioritizer::updatePriorities(TimestepDuration dt) {
    // Update neuromodulation state
    updateNeuromodulationState();
    
    // Adjust memory load based on working memory
    if (brain_) {
        auto* workingMemory = brain_->getWorkingMemory();
        if (workingMemory) {
            pImpl->currentMemoryLoad = static_cast<float>(workingMemory->getActiveTraces()) / 
                                     static_cast<float>(workingMemory->getCapacity());
        }
    }
    
    // Apply consolidation timing
    lastConsolidationStep_++;
    if (lastConsolidationStep_ > 1000) {
        // Periodic consolidation based on emotional valence
        lastConsolidationStep_ = 0;
        NLM_LOG_INFO("Emotional memory prioritization performing consolidation");
    }
}

EmotionalMemoryPrioritizer::PriorityStats EmotionalMemoryPrioritizer::getStats() const {
    PriorityStats stats;
    stats.totalPrioritized = totalPrioritized_;
    stats.highPriorityCount = highPriorityCount_;
    stats.mediumPriorityCount = mediumPriorityCount_;
    stats.lowPriorityCount = lowPriorityCount_;
    
    if (totalPrioritized_ > 0) {
        stats.averageValenceScore = accumulatedValenceScore_ / totalPrioritized_;
        stats.emotionalInfluenceAverage = accumulatedEmotionalInfluence_ / totalPrioritized_;
    }
    
    return stats;
}

void EmotionalMemoryPrioritizer::resetStats() {
    totalPrioritized_ = 0;
    accumulatedValenceScore_ = 0.0f;
    highPriorityCount_ = 0;
    mediumPriorityCount_ = 0;
    lowPriorityCount_ = 0;
    accumulatedEmotionalInfluence_ = 0.0f;
}

} // namespace nlm
