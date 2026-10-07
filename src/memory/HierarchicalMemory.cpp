#include "HierarchicalMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <unordered_map>

namespace nlm {

struct HierarchicalMemory::Impl {
    // Pattern similarity cache for performance
    mutable std::unordered_map<uint64_t, float> similarityCache;
    
    // Consolidation tracking
    struct ConsolidationState {
        SimulationStep lastConsolidation;
        size_t consolidationCount;
        float totalConsolidationScore;
    };
    
    std::vector<ConsolidationState> consolidationStates;
    
    // Pruning tracking
    struct PruningStats {
        size_t totalPruned;
        float averageRelevance;
        SimulationStep lastPrune;
    } pruningStats;
    
    Impl() {
        similarityCache.reserve(50000);
        consolidationStates.resize(4); // One per level
        pruningStats = {0, 0.0f, 0};
    }
};

HierarchicalMemory::HierarchicalMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , prioritizer_(nullptr)
    , neuromodulationBoost_(1.0f)
    , attentionAmplification_(1.0f)
    , stressSensitivity_(1.0f)
    , empathyInfluence_(1.0f)
    , dopamineModulation_(1.0f)
{
    // Initialize level configurations with neuromodulatory considerations
    levelConfigs_.resize(4);
    
    // Working memory: fast decay, high capacity, low threshold
    levelConfigs_[static_cast<size_t>(MemoryLevel::WORKING)].capacity = 200;
    levelConfigs_[static_cast<size_t>(MemoryLevel::WORKING)].decayRate = 0.02f;
    levelConfigs_[static_cast<size_t>(MemoryLevel::WORKING)].consolidationThreshold = 0.3f;
    levelConfigs_[static_cast<size_t>(MemoryLevel::WORKING)].relevanceThreshold = 0.01f;
    
    // Episodic memory: medium decay, medium capacity, medium threshold
    levelConfigs_[static_cast<size_t>(MemoryLevel::EPISODIC)].capacity = 1000;
    levelConfigs_[static_cast<size_t>(MemoryLevel::EPISODIC)].decayRate = 0.005f;
    levelConfigs_[static_cast<size_t>(MemoryLevel::EPISODIC)].consolidationThreshold = 0.6f;
    levelConfigs_[static_cast<size_t>(MemoryLevel::EPISODIC)].relevanceThreshold = 0.05f;
    
    // Semantic memory: slow decay, medium capacity, high threshold
    levelConfigs_[static_cast<size_t>(MemoryLevel::SEMANTIC)].capacity = 500;
    levelConfigs_[static_cast<size_t>(MemoryLevel::SEMANTIC)].decayRate = 0.001f;
    levelConfigs_[static_cast<size_t>(MemoryLevel::SEMANTIC)].consolidationThreshold = 0.8f;
    levelConfigs_[static_cast<size_t>(MemoryLevel::SEMANTIC)].relevanceThreshold = 0.1f;
    
    // Long-term memory: very slow decay, low capacity, very high threshold
    levelConfigs_[static_cast<size_t>(MemoryLevel::LONG_TERM)].capacity = 200;
    levelConfigs_[static_cast<size_t>(MemoryLevel::LONG_TERM)].decayRate = 0.0001f;
    levelConfigs_[static_cast<size_t>(MemoryLevel::LONG_TERM)].consolidationThreshold = 0.95f;
    levelConfigs_[static_cast<size_t>(MemoryLevel::LONG_TERM)].relevanceThreshold = 0.5f;
}

HierarchicalMemory::~HierarchicalMemory() = default;

void HierarchicalMemory::initialize(Brain* brain, EmotionalMemoryPrioritizer* prioritizer) {
    pImpl->brain = brain;
    brain_ = brain;
    prioritizer_ = prioritizer;
    
    // Initialize neuromodulation parameters
    neuromodulationBoost_ = 1.0f;
    attentionAmplification_ = 1.0f;
    stressSensitivity_ = 1.0f;
    empathyInfluence_ = 1.0f;
    dopamineModulation_ = 1.0f;
    
    NLM_LOG_INFO("HierarchicalMemory initialized with multi-level architecture");
}

void HierarchicalMemory::store(const std::vector<float>& pattern, MemoryLevel level,
                              float strength, float relevance) {
    if (pattern.empty() || !brain_) return;
    
    MemoryItem item;
    item.level = level;
    item.pattern = pattern;
    item.timestamp = SimulationStep(); // Use current step
    item.strength = std::clamp(strength, 0.0f, 1.0f);
    item.relevance = std::clamp(relevance, 0.0f, 1.0f);
    
    // Add neurons that are currently active for this pattern
    item.associatedNeurons = findActiveNeuronsForPattern(pattern);
    
    // Update level-specific metadata
    switch (level) {
        case MemoryLevel::WORKING:
            item.working.isActive = true;
            item.working.decayRate = levelConfigs_[static_cast<size_t>(level)].decayRate;
            item.working.maintenanceCurrent = strength * 5.0f;
            break;
            
        case MemoryLevel::EPISODIC:
            item.episodic.reward = relevance * 0.8f;
            item.episodic.energy = (1.0f - relevance) * 0.5f;
            item.episodic.novelty = 1.0f - relevance;
            item.episodic.action = ActionType::Wait;
            item.episodic.resultingReward = relevance * 0.5f;
            break;
            
        case MemoryLevel::SEMANTIC:
            item.semantic.conceptId = generateConceptId(pattern);
            item.semantic.stability = strength;
            item.semantic.relatedConcepts = findRelatedConcepts(pattern);
            item.semantic.abstractionLevel = 1.0f - relevance;
            break;
            
        case MemoryLevel::LONG_TERM:
            item.longTerm.consolidationScore = strength;
            item.longTerm.emotionalValence = relevance;
            item.longTerm.importance = strength * relevance;
            item.longTerm.lastAccess = SimulationStep();
            item.longTerm.accessCount = 1;
            break;
    }
    
    addToLevel(item);
}

void HierarchicalMemory::storeEpisode(const std::vector<float>& sensoryState, ActionType action,
                                    float reward, float energy, float novelty,
                                    const std::vector<float>& resultingSensoryState,
                                    float resultingReward, const std::vector<NeuronId>& activeNeurons) {
    // Store in episodic memory with full metadata
    MemoryItem episodicItem;
    episodicItem.level = MemoryLevel::EPISODIC;
    episodicItem.pattern = sensoryState;
    episodicItem.timestamp = SimulationStep();
    episodicItem.strength = 0.8f + std::min(reward, 1.0f) * 0.2f;
    episodicItem.relevance = 0.7f + (reward * 0.3f) + (novelty * 0.2f);
    episodicItem.associatedNeurons = activeNeurons;
    
    episodicItem.episodic.sensoryState = sensoryState;
    episodicItem.episodic.action = action;
    episodicItem.episodic.reward = reward;
    episodicItem.episodic.energy = energy;
    episodicItem.episodic.novelty = novelty;
    episodicItem.episodic.resultingSensoryState = resultingSensoryState;
    episodicItem.episodic.resultingReward = resultingReward;
    
    addToLevel(episodicItem);
    
    // Store semantic representation if novelty is high
    if (novelty > 0.7f) {
        // Create semantic abstraction
        std::vector<float> semanticPattern = abstractPattern(sensoryState);
        store(semanticPattern, MemoryLevel::SEMANTIC, 0.7f, novelty * 0.8f);
    }
    
    // Store in long-term memory if reward is high
    if (reward > 0.8f) {
        // Create long-term consolidation
        std::vector<float> longTermPattern = consolidatePattern(sensoryState);
        store(longTermPattern, MemoryLevel::LONG_TERM, reward, reward * novelty);
    }
}

std::vector<HierarchicalMemory::MemoryItem> HierarchicalMemory::retrieveLevel(MemoryLevel level, size_t maxResults) const {
    std::vector<MemoryItem> results;
    
    switch (level) {
        case MemoryLevel::WORKING:
            results = workingMemory_;
            break;
        case MemoryLevel::EPISODIC:
            results = episodicMemory_;
            break;
        case MemoryLevel::SEMANTIC:
            results = semanticMemory_;
            break;
        case MemoryLevel::LONG_TERM:
            results = longTermMemory_;
            break;
    }
    
    // Sort by strength and relevance
    std::sort(results.begin(), results.end(),
              [](const MemoryItem& a, const MemoryItem& b) {
                  return a.strength * a.relevance > b.strength * b.relevance;
              });
    
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

std::vector<HierarchicalMemory::MemoryItem> HierarchicalMemory::retrieveHierarchical(
    const std::vector<float>& pattern, float similarityThreshold, size_t maxResults) const {
    
    std::vector<MemoryItem> allResults;
    
    // Search each level
    for (int level = 0; level < 4; ++level) {
        MemoryLevel memLevel = static_cast<MemoryLevel>(level);
        
        std::vector<MemoryItem> levelResults = findSimilarItems(pattern, memLevel, similarityThreshold);
        
        // Filter by relevance threshold
        float relevanceThreshold = levelConfigs_[level].relevanceThreshold;
        std::vector<MemoryItem> filteredResults;
        for (const auto& item : levelResults) {
            if (item.relevance >= relevanceThreshold) {
                filteredResults.push_back(item);
            }
        }
        
        // Add to all results with level weighting
        for (auto& item : filteredResults) {
            item.relevance *= (1.0f + static_cast<float>(level) * 0.2f); // Higher levels weighted more
            allResults.push_back(item);
        }
    }
    
    // Sort by final relevance score
    std::sort(allResults.begin(), allResults.end(),
              [](const MemoryItem& a, const MemoryItem& b) {
                  return a.relevance > b.relevance;
              });
    
    if (allResults.size() > maxResults) {
        allResults.resize(maxResults);
    }
    
    return allResults;
}

void HierarchicalMemory::consolidate(float consolidationThreshold) {
    // Consolidate between adjacent levels
    consolidateLevel(MemoryLevel::WORKING, MemoryLevel::EPISODIC, consolidationThreshold);
    consolidateLevel(MemoryLevel::EPISODIC, MemoryLevel::SEMANTIC, consolidationThreshold);
    consolidateLevel(MemoryLevel::SEMANTIC, MemoryLevel::LONG_TERM, consolidationThreshold);
    
    // Update consolidation states
    for (auto& state : pImpl->consolidationStates) {
        state.lastConsolidation = SimulationStep();
        state.consolidationCount++;
    }
}

std::vector<float> HierarchicalMemory::completePattern(const std::vector<float>& partialPattern) const {
    // Use pattern completion across all levels
    std::vector<MemoryItem> similarItems = retrieveHierarchical(partialPattern, 0.2f, 20);
    
    if (similarItems.empty()) {
        return partialPattern; // No completion possible
    }
    
    // Average the patterns for completion
    std::vector<float> completedPattern(similarItems[0].pattern.size(), 0.0f);
    float totalWeight = 0.0f;
    
    for (const auto& item : similarItems) {
        float weight = item.strength * item.relevance;
        totalWeight += weight;
        
        for (size_t i = 0; i < item.pattern.size() && i < completedPattern.size(); ++i) {
            completedPattern[i] += item.pattern[i] * weight;
        }
    }
    
    if (totalWeight > 0.0f) {
        for (float& value : completedPattern) {
            value /= totalWeight;
        }
    }
    
    return completedPattern;
}

void HierarchicalMemory::prune(float retentionThreshold) {
    // Prune weak items from each level
    pruneLevel(MemoryLevel::WORKING, retentionThreshold);
    pruneLevel(MemoryLevel::EPISODIC, retentionThreshold);
    pruneLevel(MemoryLevel::SEMANTIC, retentionThreshold);
    pruneLevel(MemoryLevel::LONG_TERM, retentionThreshold);
    
    // Update pruning stats
    pImpl->pruningStats.totalPruned++;
    pImpl->pruningStats.lastPrune = SimulationStep();
}

HierarchicalMemory::MemoryStats HierarchicalMemory::getStats() const {
    MemoryStats stats;
    
    stats.workingMemoryItems = workingMemory_.size();
    stats.episodicMemoryItems = episodicMemory_.size();
    stats.semanticMemoryItems = semanticMemory_.size();
    stats.longTermMemoryItems = longTermMemory_.size();
    
    stats.totalItems = stats.workingMemoryItems + stats.episodicMemoryItems + 
                      stats.semanticMemoryItems + stats.longTermMemoryItems;
    
    // Calculate average strength and relevance
    if (stats.totalItems > 0) {
        float totalStrength = 0.0f;
        float totalRelevance = 0.0f;
        
        for (const auto& item : allItems_) {
            totalStrength += item.strength;
            totalRelevance += item.relevance;
        }
        
        stats.averageStrength = totalStrength / stats.totalItems;
        stats.averageRelevance = totalRelevance / stats.totalItems;
    }
    
    // Calculate memory efficiency (items retained / capacity used)
    float totalCapacity = 0.0f;
    float totalUtilization = 0.0f;
    
    for (size_t level = 0; level < 4; ++level) {
        totalCapacity += levelConfigs_[level].capacity;
        switch (level) {
            case 0: totalUtilization += workingMemory_.size(); break;
            case 1: totalUtilization += episodicMemory_.size(); break;
            case 2: totalUtilization += semanticMemory_.size(); break;
            case 3: totalUtilization += longTermMemory_.size(); break;
        }
    }
    
    stats.memoryEfficiency = totalCapacity > 0.0f ? totalUtilization / totalCapacity : 0.0f;
    
    return stats;
}

void HierarchicalMemory::setLevelCapacity(MemoryLevel level, size_t capacity) {
    levelConfigs_[static_cast<size_t>(level)].capacity = capacity;
}

size_t HierarchicalMemory::getLevelCapacity(MemoryLevel level) const {
    return levelConfigs_[static_cast<size_t>(level)].capacity;
}

void HierarchicalMemory::setLevelDecayRate(MemoryLevel level, float decayRate) {
    levelConfigs_[static_cast<size_t>(level)].decayRate = decayRate;
}

float HierarchicalMemory::getLevelDecayRate(MemoryLevel level) const {
    return levelConfigs_[static_cast<size_t>(level)].decayRate;
}

void HierarchicalMemory::applyWorkingMemoryToEpisodic() {
    // Promote strong working memory items to episodic memory
    for (const auto& item : workingMemory_) {
        if (item.strength > levelConfigs_[static_cast<size_t>(MemoryLevel::EPISODIC)].consolidationThreshold) {
            // Create episodic version with additional metadata
            MemoryItem episodicItem = item;
            episodicItem.level = MemoryLevel::EPISODIC;
            episodicItem.episodic.reward = item.relevance * 0.8f;
            episodicItem.episodic.action = ActionType::Wait; // Would be set from actual experience
            episodicItem.episodic.energy = (1.0f - item.relevance) * 0.5f;
            episodicItem.episodic.novelty = 1.0f - item.relevance;
            
            // Remove from working memory and add to episodic
            removeFromLevel(item);
            addToLevel(episodicItem);
        }
    }
}

void HierarchicalMemory::applyEpisodicToSemantic() {
    // Abstract episodic patterns into semantic concepts
    for (const auto& item : episodicMemory_) {
        if (item.relevance > levelConfigs_[static_cast<size_t>(MemoryLevel::SEMANTIC)].consolidationThreshold) {
            // Create semantic abstraction
            MemoryItem semanticItem;
            semanticItem.level = MemoryLevel::SEMANTIC;
            semanticItem.pattern = abstractPattern(item.pattern);
            semanticItem.timestamp = item.timestamp;
            semanticItem.strength = item.strength * 0.8f;
            semanticItem.relevance = item.relevance * 0.9f;
            semanticItem.associatedNeurons = item.associatedNeurons;
            semanticItem.semantic.conceptId = generateConceptId(semanticItem.pattern);
            semanticItem.semantic.stability = semanticItem.strength;
            semanticItem.semantic.relatedConcepts = findRelatedConcepts(semanticItem.pattern);
            semanticItem.semantic.abstractionLevel = 0.7f;
            
            removeFromLevel(item);
            addToLevel(semanticItem);
        }
    }
}

void HierarchicalMemory::applySemanticToLongTerm() {
    // Consolidate semantic patterns into long-term memory
    for (const auto& item : semanticMemory_) {
        if (item.relevance > levelConfigs_[static_cast<size_t>(MemoryLevel::LONG_TERM)].consolidationThreshold) {
            // Create long-term consolidation
            MemoryItem longTermItem;
            longTermItem.level = MemoryLevel::LONG_TERM;
            longTermItem.pattern = consolidatePattern(item.pattern);
            longTermItem.timestamp = item.timestamp;
            longTermItem.strength = item.strength * 0.9f;
            longTermItem.relevance = item.relevance * 0.95f;
            longTermItem.associatedNeurons = item.associatedNeurons;
            longTermItem.longTerm.consolidationScore = longTermItem.strength;
            longTermItem.longTerm.emotionalValence = longTermItem.relevance;
            longTermItem.longTerm.importance = longTermItem.strength * longTermItem.relevance;
            longTermItem.longTerm.lastAccess = SimulationStep();
            longTermItem.longTerm.accessCount = 1;
            
            removeFromLevel(item);
            addToLevel(longTermItem);
        }
    }
}

void HierarchicalMemory::updateWithNeuromodulation(float emotionalInfluence, float attentionFocus, 
                                                 float stressLevel, float empathyLevel) {
    // Enhanced neuromodulatory update with multiple factors
    float totalNeuromodulation = neuromodulationBoost_ * (1.0f + emotionalInfluence * 0.2f);
    
    // Apply stress sensitivity
    float stressFactor = 1.0f + stressLevel * stressSensitivity_ * 0.3f;
    
    // Apply empathy influence
    float empathyFactor = 1.0f + empathyLevel * empathyInfluence_ * 0.2f;
    
    // Apply dopamine modulation
    float dopamineFactor = 1.0f + dopamineModulation_ * 0.1f;
    
    float attentionFactor = 1.0f + attentionFocus * attentionAmplification_ * 0.3f;
    
    for (auto& item : allItems_) {
        // Enhanced emotional valence influence with stress interaction
        item.relevance *= (1.0f + emotionalInfluence * 0.2f * stressFactor);
        
        // Strength modulation with attention, empathy, and dopamine
        item.strength *= attentionFactor * empathyFactor * dopamineFactor;
        
        // Level-specific decay with neuromodulatory enhancement
        size_t levelIndex = static_cast<size_t>(item.level);
        item.strength *= (1.0f - levelConfigs_[levelIndex].decayRate * totalNeuromodulation);
        
        // Enhanced relevance calculation
        item.relevance = std::clamp(item.relevance, 0.0f, 1.0f);
        item.strength = std::clamp(item.strength, 0.0f, 1.0f);
        
        // Cross-level integration during high neuromodulation
        if (totalNeuromodulation > 1.5f) {
            enhanceCrossLevelIntegration(item);
        }
        
        // Update in the correct level storage
        removeFromLevel(item);
        addToLevel(item);
    }
}

void HierarchicalMemory::enhanceCrossLevelIntegration(MemoryItem& item) {
    // Enhanced cross-level integration during high neuromodulation
    if (brain_ && item.level != MemoryLevel::LONG_TERM) {
        // Boost relevance for potential long-term consolidation
        item.relevance *= 1.3f;
        
        // Enhance strength for better cross-level transfer
        item.strength *= 1.2f;
        
        // Add to potential consolidation queue
        if (!pImpl->consolidationQueue.empty() && item.relevance > levelConfigs_[static_cast<size_t>(MemoryLevel::LONG_TERM)].consolidationThreshold) {
            pImpl->consolidationQueue.push_back(item);
        }
    }
}

void HierarchicalMemory::processConsolidationQueue() {
    // Process queued items for enhanced cross-level consolidation
    if (!pImpl->consolidationQueue.empty()) {
        std::vector<MemoryItem> consolidatedItems;
        
        for (const auto& item : pImpl->consolidationQueue) {
            // Enhanced consolidation with multi-factor scoring
            float consolidationScore = calculateEnhancedConsolidationScore(item);
            
            if (consolidationScore > levelConfigs_[static_cast<size_t>(MemoryLevel::LONG_TERM)].consolidationThreshold) {
                // Create enhanced long-term item
                MemoryItem enhancedItem = item;
                enhancedItem.level = MemoryLevel::LONG_TERM;
                enhancedItem.strength = consolidationScore * 0.9f;
                enhancedItem.relevance = item.relevance * 1.1f;
                enhancedItem.longTerm.consolidationScore = consolidationScore;
                enhancedItem.longTerm.emotionalValence = item.relevance * 0.8f;
                enhancedItem.longTerm.importance = item.strength * item.relevance * 1.2f;
                
                consolidatedItems.push_back(enhancedItem);
            }
        }
        
        // Clear queue and add consolidated items
        pImpl->consolidationQueue.clear();
        for (auto& item : consolidatedItems) {
            addToLevel(item);
        }
        
        NLM_LOG_INFO("Processed " + std::to_string(consolidatedItems.size()) + 
                     " items through enhanced consolidation queue");
    }
}

float HierarchicalMemory::calculateEnhancedConsolidationScore(const MemoryItem& item) const {
    // Multi-factor consolidation scoring with neuromodulatory influence
    float baseScore = item.strength * item.relevance;
    
    // Add age factor (older items consolidate better)
    float ageFactor = std::min(1.0f, static_cast<float>(item.timestamp) * 0.0001f);
    baseScore *= (1.0f + ageFactor * 0.3f);
    
    // Add level-based modulation
    float levelFactor = 1.0f;
    switch (item.level) {
        case MemoryLevel::WORKING:
            levelFactor = 0.7f * neuromodulationBoost_;
            break;
        case MemoryLevel::EPISODIC:
            levelFactor = 0.85f * neuromodulationBoost_;
            break;
        case MemoryLevel::SEMANTIC:
            levelFactor = 0.95f * neuromodulationBoost_;
            break;
        case MemoryLevel::LONG_TERM:
            levelFactor = 1.0f * neuromodulationBoost_;
            break;
    }
    
    // Add novelty bonus
    float noveltyBonus = 1.0f + item.episodic.novelty * 0.2f;
    
    return std::clamp(baseScore * levelFactor * noveltyBonus, 0.0f, 1.0f);
}

void HierarchicalMemory::setNeuromodulationParameters(float boost, float attention, 
                                                     float stress, float empathy, float dopamine) {
    neuromodulationBoost_ = boost;
    attentionAmplification_ = attention;
    stressSensitivity_ = stress;
    empathyInfluence_ = empathy;
    dopamineModulation_ = dopamine;
    
    NLM_LOG_INFO("Neuromodulation parameters updated - Boost: " + std::to_string(boost) +
                 ", Attention: " + std::to_string(attention) + ", Stress: " + std::to_string(stress) +
                 ", Empathy: " + std::to_string(empathy) + ", Dopamine: " + std::to_string(dopamine));
}

void HierarchicalMemory::clear() {
    workingMemory_.clear();
    episodicMemory_.clear();
    semanticMemory_.clear();
    longTermMemory_.clear();
    allItems_.clear();
}

void HierarchicalMemory::addToLevel(MemoryItem item) {
    // Add to central storage
    allItems_.push_back(item);
    
    // Add to level-specific storage
    switch (item.level) {
        case MemoryLevel::WORKING:
            workingMemory_.push_back(item);
            break;
        case MemoryLevel::EPISODIC:
            episodicMemory_.push_back(item);
            break;
        case MemoryLevel::SEMANTIC:
            semanticMemory_.push_back(item);
            break;
        case MemoryLevel::LONG_TERM:
            longTermMemory_.push_back(item);
            break;
    }
    
    // Apply capacity limits
    applyLevelCapacityLimit(item.level);
}

void HierarchicalMemory::removeFromLevel(MemoryItem item) {
    // Remove from central storage
    auto it = std::find(allItems_.begin(), allItems_.end(), item);
    if (it != allItems_.end()) {
        allItems_.erase(it);
    }
    
    // Remove from level-specific storage
    switch (item.level) {
        case MemoryLevel::WORKING:
            it = std::find(workingMemory_.begin(), workingMemory_.end(), item);
            if (it != workingMemory_.end()) {
                workingMemory_.erase(it);
            }
            break;
        case MemoryLevel::EPISODIC:
            it = std::find(episodicMemory_.begin(), episodicMemory_.end(), item);
            if (it != episodicMemory_.end()) {
                episodicMemory_.erase(it);
            }
            break;
        case MemoryLevel::SEMANTIC:
            it = std::find(semanticMemory_.begin(), semanticMemory_.end(), item);
            if (it != semanticMemory_.end()) {
                semanticMemory_.erase(it);
            }
            break;
        case MemoryLevel::LONG_TERM:
            it = std::find(longTermMemory_.begin(), longTermMemory_.end(), item);
            if (it != longTermMemory_.end()) {
                longTermMemory_.erase(it);
            }
            break;
    }
}

void HierarchicalMemory::updateItemRelevance(MemoryItem& item) {
    // Update relevance based on various factors
    float baseRelevance = item.relevance;
    
    // Factor in strength
    baseRelevance *= item.strength;
    
    // Factor in age (recent items more relevant)
    float ageFactor = std::exp(-static_cast<float>(item.timestamp) * 0.0001f);
    baseRelevance *= ageFactor;
    
    item.relevance = std::clamp(baseRelevance, 0.0f, 1.0f);
}

float HierarchicalMemory::computePatternSimilarity(const std::vector<float>& pattern1,
                                                 const std::vector<float>& pattern2) const {
    // Use cache for performance
    size_t hash = 0;
    for (size_t i = 0; i < std::min(pattern1.size(), pattern2.size()); ++i) {
        hash ^= std::hash<float>{}(pattern1[i]) + std::hash<float>{}(pattern2[i]) + 
                0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    
    auto cacheIt = pImpl->similarityCache.find(hash);
    if (cacheIt != pImpl->similarityCache.end()) {
        return cacheIt->second;
    }
    
    // Compute cosine similarity
    float dotProduct = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    size_t minSize = std::min(pattern1.size(), pattern2.size());
    for (size_t i = 0; i < minSize; ++i) {
        dotProduct += pattern1[i] * pattern2[i];
        norm1 += pattern1[i] * pattern1[i];
        norm2 += pattern2[i] * pattern2[i];
    }
    
    float similarity = dotProduct / (std::sqrt(norm1) * std::sqrt(norm2) + 0.0001f);
    
    // Cache result
    pImpl->similarityCache[hash] = similarity;
    
    return similarity;
}

std::vector<HierarchicalMemory::MemoryItem> HierarchicalMemory::findSimilarItems(
    const std::vector<float>& pattern, MemoryLevel level,
    float similarityThreshold) const {
    
    std::vector<MemoryItem> results;
    
    switch (level) {
        case MemoryLevel::WORKING:
            for (const auto& item : workingMemory_) {
                float similarity = computePatternSimilarity(pattern, item.pattern);
                if (similarity >= similarityThreshold) {
                    results.push_back(item);
                }
            }
            break;
            
        case MemoryLevel::EPISODIC:
            for (const auto& item : episodicMemory_) {
                float similarity = computePatternSimilarity(pattern, item.pattern);
                if (similarity >= similarityThreshold) {
                    results.push_back(item);
                }
            }
            break;
            
        case MemoryLevel::SEMANTIC:
            for (const auto& item : semanticMemory_) {
                float similarity = computePatternSimilarity(pattern, item.pattern);
                if (similarity >= similarityThreshold) {
                    results.push_back(item);
                }
            }
            break;
            
        case MemoryLevel::LONG_TERM:
            for (const auto& item : longTermMemory_) {
                float similarity = computePatternSimilarity(pattern, item.pattern);
                if (similarity >= similarityThreshold) {
                    results.push_back(item);
                }
            }
            break;
    }
    
    return results;
}

void HierarchicalMemory::consolidateLevel(MemoryLevel sourceLevel, MemoryLevel targetLevel,
                                         float threshold) {
    std::vector<MemoryItem>* source = nullptr;
    std::vector<MemoryItem>* target = nullptr;
    
    switch (sourceLevel) {
        case MemoryLevel::WORKING: source = &workingMemory_; break;
        case MemoryLevel::EPISODIC: source = &episodicMemory_; break;
        case MemoryLevel::SEMANTIC: source = &semanticMemory_; break;
        case MemoryLevel::LONG_TERM: source = &longTermMemory_; break;
    }
    
    switch (targetLevel) {
        case MemoryLevel::WORKING: target = &workingMemory_; break;
        case MemoryLevel::EPISODIC: target = &episodicMemory_; break;
        case MemoryLevel::SEMANTIC: target = &semanticMemory_; break;
        case MemoryLevel::LONG_TERM: target = &longTermMemory_; break;
    }
    
    if (!source || !target) return;
    
    for (const auto& sourceItem : *source) {
        if (sourceItem.relevance >= threshold) {
            // Create target item with consolidation
            MemoryItem targetItem = sourceItem;
            targetItem.level = targetLevel;
            targetItem.strength = calculateConsolidationScore(sourceItem);
            targetItem.relevance = sourceItem.relevance * 0.9f; // Slight decay during consolidation
            
            // Remove from source and add to target
            removeFromLevel(sourceItem);
            addToLevel(targetItem);
        }
    }
}

float HierarchicalMemory::calculateConsolidationScore(const MemoryItem& item) const {
    // Multi-factor consolidation scoring
    float baseScore = item.strength;
    
    // Add factors based on level
    size_t sourceLevel = static_cast<size_t>(item.level);
    
    switch (item.level) {
        case MemoryLevel::WORKING:
            baseScore *= 0.7f; // Working memory items lose strength during consolidation
            break;
        case MemoryLevel::EPISODIC:
            baseScore *= 0.85f;
            break;
        case MemoryLevel::SEMANTIC:
            baseScore *= 0.9f;
            break;
        case MemoryLevel::LONG_TERM:
            baseScore *= 0.95f;
            break;
    }
    
    // Add age factor (older items consolidate better)
    float ageFactor = std::min(1.0f, static_cast<float>(item.timestamp) * 0.0001f);
    baseScore *= (1.0f + ageFactor * 0.2f);
    
    return std::clamp(baseScore, 0.0f, 1.0f);
}

void HierarchicalMemory::applyLevelCapacityLimit(MemoryLevel level) {
    std::vector<MemoryItem>* storage = nullptr;
    
    switch (level) {
        case MemoryLevel::WORKING: storage = &workingMemory_; break;
        case MemoryLevel::EPISODIC: storage = &episodicMemory_; break;
        case MemoryLevel::SEMANTIC: storage = &semanticMemory_; break;
        case MemoryLevel::LONG_TERM: storage = &longTermMemory_; break;
    }
    
    if (!storage) return;
    
    size_t capacity = levelConfigs_[static_cast<size_t>(level)].capacity;
    if (storage->size() > capacity) {
        // Remove lowest relevance items
        std::partial_sort(storage->begin(), storage->begin() + capacity, storage->end(),
                         [](const MemoryItem& a, const MemoryItem& b) {
                             return a.relevance > b.relevance;
                         });
        storage->resize(capacity);
        
        // Remove from central storage
        std::vector<MemoryItem> updatedAll;
        for (const auto& item : allItems_) {
            if (item.level != level || 
                std::find(storage->begin(), storage->end(), item) != storage->end()) {
                updatedAll.push_back(item);
            }
        }
        allItems_ = updatedAll;
    }
}

void HierarchicalMemory::pruneLevel(MemoryLevel level, float retentionThreshold) {
    std::vector<MemoryItem>* storage = nullptr;
    
    switch (level) {
        case MemoryLevel::WORKING: storage = &workingMemory_; break;
        case MemoryLevel::EPISODIC: storage = &episodicMemory_; break;
        case MemoryLevel::SEMANTIC: storage = &semanticMemory_; break;
        case MemoryLevel::LONG_TERM: storage = &longTermMemory_; break;
    }
    
    if (!storage) return;
    
    std::vector<MemoryItem> prunedItems;
    float sumRelevance = 0.0f;
    
    // Collect items above threshold and calculate statistics
    for (const auto& item : *storage) {
        if (item.relevance >= retentionThreshold) {
            prunedItems.push_back(item);
            sumRelevance += item.relevance;
        }
    }
    
    // Update pruning stats
    pImpl->pruningStats.totalPruned += storage->size() - prunedItems.size();
    if (!prunedItems.empty()) {
        pImpl->pruningStats.averageRelevance = sumRelevance / prunedItems.size();
    }
    
    // Replace storage
    *storage = prunedItems;
    
    // Update central storage
    std::vector<MemoryItem> updatedAll;
    for (const auto& item : allItems_) {
        if (item.level != level || 
            std::find(storage->begin(), storage->end(), item) != storage->end()) {
            updatedAll.push_back(item);
        }
    }
    allItems_ = updatedAll;
}

void HierarchicalMemory::updateMemoryEfficiency() {
    // Simple efficiency calculation
    float totalCapacity = 0.0f;
    float totalUtilization = 0.0f;
    
    for (size_t level = 0; level < 4; ++level) {
        totalCapacity += levelConfigs_[level].capacity;
        switch (level) {
            case 0: totalUtilization += workingMemory_.size(); break;
            case 1: totalUtilization += episodicMemory_.size(); break;
            case 2: totalUtilization += semanticMemory_.size(); break;
            case 3: totalUtilization += longTermMemory_.size(); break;
        }
    }
    
    // Update efficiency in stats (would need to be exposed)
}

std::vector<NeuronId> HierarchicalMemory::findActiveNeuronsForPattern(const std::vector<float>& pattern) const {
    // This would normally query the brain's neural populations
    // For this implementation, we'll return an empty set
    return std::vector<NeuronId>();
}

std::string HierarchicalMemory::generateConceptId(const std::vector<float>& pattern) const {
    // Generate a simple concept ID based on pattern
    std::hash<float> hasher;
    size_t hash = 0;
    for (float value : pattern) {
        hash ^= hasher(value) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return "concept_" + std::to_string(hash);
}

std::vector<std::string> HierarchicalMemory::findRelatedConcepts(const std::vector<float>& pattern) const {
    // Find related semantic concepts
    std::vector<std::string> related;
    
    for (const auto& item : semanticMemory_) {
        float similarity = computePatternSimilarity(pattern, item.pattern);
        if (similarity > 0.7f) {
            related.push_back(item.semantic.conceptId);
        }
    }
    
    return related;
}

std::vector<float> HierarchicalMemory::abstractPattern(const std::vector<float>& pattern) const {
    // Create semantic abstraction by simplifying the pattern
    std::vector<float> abstracted(pattern.size(), 0.0f);
    
    // Apply averaging over local regions
    for (size_t i = 1; i < pattern.size() - 1; ++i) {
        abstracted[i] = (pattern[i-1] + pattern[i] + pattern[i+1]) / 3.0f;
    }
    
    // Add some noise for robustness
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-0.1f, 0.1f);
    
    for (float& value : abstracted) {
        value += dis(gen);
        value = std::clamp(value, 0.0f, 1.0f);
    }
    
    return abstracted;
}

std::vector<float> HierarchicalMemory::consolidatePattern(const std::vector<float>& pattern) const {
    // Create long-term consolidation by further abstraction
    std::vector<float> consolidated(pattern.size() / 2 + 1, 0.0f);
    
    for (size_t i = 0; i < pattern.size(); i += 2) {
        size_t targetIdx = i / 2;
        if (targetIdx < consolidated.size()) {
            consolidated[targetIdx] = pattern[i];
        }
    }
    
    // Apply smoothing
    for (size_t i = 1; i < consolidated.size() - 1; ++i) {
        consolidated[i] = (consolidated[i-1] + consolidated[i] + consolidated[i+1]) / 3.0f;
    }
    
    return consolidated;
}

} // namespace nlm
