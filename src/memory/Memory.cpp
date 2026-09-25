#include "Memory.hpp"
#include <cmath>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <queue>
#include <unordered_map>

namespace nlm {

// Working Memory Implementation - Biological constraints and dynamics
struct WorkingMemory::Impl {
    // Memory slots for items (neuron ID -> memory trace)
    std::vector<std::pair<NeuronId, MemoryTrace>> memorySlots;
    
    // Capacity constraints based on biological limitations
    size_t maxSlots;
    float decayRate;
    float interferenceStrength;
    
    // Activity-dependent updates
    float totalActivation;
    size_t activeSlots;
    
    // Hemispheric lateral inhibition
    float leftInhibition;
    float rightInhibition;
    
    // Encoding strength modifiers
    float noveltyGain;
    float attentionModulator;
    
    Impl(size_t cap) : 
        maxSlots(cap),
        decayRate(0.01f),           // ~1% decay per timestep
        interferenceStrength(0.1f),
        totalActivation(0.0f),
        activeSlots(0),
        leftInhibition(1.0f),
        rightInhibition(1.0f),
        noveltyGain(0.5f),
        attentionModulator(1.0f) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Biological encoding with synaptic integration
    encodeMemory(neuron, value);
}

void WorkingMemory::encodeMemory(NeuronId neuron, float value) {
    if (!pImpl) return;
    
    // Check for existing memory slot
    size_t existingSlot = findMemorySlot(neuron);
    
    if (existingSlot != pImpl->memorySlots.size()) {
        // Update existing memory
        MemoryTrace& trace = pImpl->memorySlots[existingSlot].second;
        updateMemoryTrace(trace, value);
    } else {
        // Create new memory slot with biological constraints
        createMemorySlot(neuron, value);
    }
    
    // Apply lateral inhibition
    applyLateralInhibition(neuron);
    
    // Update activation metrics
    pImpl->totalActivation += std::abs(value);
    pImpl->activeSlots++;
}

void WorkingMemory::createMemorySlot(NeuronId neuron, float value) {
    if (!pImpl) return;
    
    // Check capacity before creating new slot
    if (pImpl->memorySlots.size() >= pImpl->maxSlots) {
        // Find weakest slot for replacement
        size_t weakestSlot = findWeakestSlot();
        if (weakestSlot != pImpl->memorySlots.size()) {
            pImpl->memorySlots[weakestSlot] = {neuron, createMemoryTrace(value, neuron)};
        }
        return;
    }
    
    // Create new memory trace with biological properties
    MemoryTrace trace = createMemoryTrace(value, neuron);
    pImpl->memorySlots.emplace_back(neuron, trace);
}

MemoryTrace WorkingMemory::createMemoryTrace(float value, NeuronId neuron) const {
    MemoryTrace trace;
    
    // Encode based on synaptic properties
    trace.value = value;
    trace.encodingStrength = calculateEncodingStrength(value);
    trace.stability = calculateStability(trace.encodingStrength);
    trace.decayRate = pImpl->decayRate * (1.0f - stabilityDecayModifier(trace.stability));
    trace.interference = calculateInterferenceLevel(neuron);
    trace.formationTime = 0.0f;
    
    // Add biological time constant
    trace.timeConstant = calculateTimeConstant(trace.stability);
    
    return trace;
}

void WorkingMemory::updateMemoryTrace(MemoryTrace& trace, float newValue) {
    // Update trace with new information using Hebbian integration
    float delta = newValue - trace.value;
    
    // Synaptic-like update rule
    trace.value = trace.value + delta * 0.5f;  // Partial update
    trace.encodingStrength += std::abs(delta) * 0.1f;
    trace.stability = std::clamp(trace.stability + 0.05f, 0.1f, 1.0f);
    trace.formationTime = 0.0f;
    
    // Decay effect reduces with stability
    trace.decayRate = pImpl->decayRate * (1.0f - stabilityDecayModifier(trace.stability));
}

float WorkingMemory::calculateEncodingStrength(float value) const {
    // Biological encoding depends on magnitude and novelty
    return std::min(std::abs(value) * 0.1f + 0.5f, 1.0f);
}

float WorkingMemory::calculateStability(float encodingStrength) const {
    // Stability depends on encoding strength
    return 0.5f + encodingStrength * 0.5f;
}

float WorkingMemory::stabilityDecayModifier(float stability) const {
    // More stable memories decay slower
    return stability * 0.8f;
}

float WorkingMemory::calculateInterferenceLevel(NeuronId neuron) const {
    // Calculate interference from similar neural patterns
    float interference = 0.0f;
    for (const auto& slot : pImpl->memorySlots) {
        if (slot.first != neuron) {
            // Calculate neural similarity
            float similarity = calculateNeuralSimilarity(slot.first, neuron);
            interference += similarity * pImpl->interferenceStrength;
        }
    }
    return std::min(interference, 1.0f);
}

float WorkingMemory::calculateNeuralSimilarity(NeuronId a, NeuronId b) const {
    // Simple distance-based similarity
    int aIndex = static_cast<int>(a.index());
    int bIndex = static_cast<int>(b.index());
    int distance = std::abs(aIndex - bIndex);
    
    return std::max(0.0f, 1.0f - static_cast<float>(distance) / 100.0f);
}

float WorkingMemory::calculateTimeConstant(float stability) const {
    // Biological time constant depends on stability
    return 1.0f / (pImpl->decayRate * (1.0f + stability));
}

void WorkingMemory::applyLateralInhibition(NeuronId activeNeuron) {
    // Hemispheric competition
    float totalActivity = calculateTotalActivity();
    if (totalActivity > 0.0f) {
        float leftShare = calculateLeftShare();
        float rightShare = calculateRightShare();
        
        // Update inhibition based on activity
        pImpl->leftInhibition = std::max(0.1f, 1.0f - leftShare * pImpl->interferenceStrength);
        pImpl->rightInhibition = std::max(0.1f, 1.0f - rightShare * pImpl->interferenceStrength);
    }
}

void WorkingMemory::updateLateralInhibition() {
    if (!pImpl) return;
    
    // Adaptive inhibition based on memory load
    float utilization = static_cast<float>(pImpl->activeSlots) / static_cast<float>(pImpl->maxSlots);
    
    pImpl->leftInhibition = 1.0f - utilization * 0.3f;
    pImpl->rightInhibition = 1.0f - utilization * 0.3f;
}

float WorkingMemory::calculateTotalActivity() const {
    if (!pImpl) return 0.0f;
    
    float total = 0.0f;
    for (const auto& slot : pImpl->memorySlots) {
        total += std::abs(slot.second.value);
    }
    return total;
}

float WorkingMemory::calculateLeftShare() const {
    if (!pImpl) return 0.5f;
    
    float leftActivity = 0.0f;
    for (const auto& slot : pImpl->memorySlots) {
        int neuronIndex = static_cast<int>(slot.first.index());
        if (neuronIndex % 2 == 0) {  // Left hemisphere
            leftActivity += std::abs(slot.second.value);
        }
    }
    
    float totalActivity = calculateTotalActivity();
    return totalActivity > 0.0f ? leftActivity / totalActivity : 0.5f;
}

float WorkingMemory::calculateRightShare() const {
    return 1.0f - calculateLeftShare();
}

size_t WorkingMemory::findMemorySlot(NeuronId neuron) const {
    for (size_t i = 0; i < pImpl->memorySlots.size(); ++i) {
        if (pImpl->memorySlots[i].first == neuron) {
            return i;
        }
    }
    return pImpl->memorySlots.size();
}

size_t WorkingMemory::findWeakestSlot() const {
    if (pImpl->memorySlots.empty()) return pImpl->memorySlots.size();
    
    size_t weakestSlot = 0;
    float weakestStrength = pImpl->memorySlots[0].second.encodingStrength;
    
    for (size_t i = 1; i < pImpl->memorySlots.size(); ++i) {
        if (pImpl->memorySlots[i].second.encodingStrength < weakestStrength) {
            weakestSlot = i;
            weakestStrength = pImpl->memorySlots[i].second.encodingStrength;
        }
    }
    
    return weakestSlot;
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    size_t slotIndex = findMemorySlot(neuron);
    if (slotIndex < pImpl->memorySlots.size()) {
        const MemoryTrace& trace = pImpl->memorySlots[slotIndex].second;
        
        // Apply decay
        float retrievedValue = trace.value * (1.0f - trace.decayRate);
        
        // Apply interference
        retrievedValue *= (1.0f - trace.interference * 0.5f);
        
        return std::clamp(retrievedValue, -10.0f, 10.0f);
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    return findMemorySlot(neuron) < pImpl->memorySlots.size();
}

void WorkingMemory::decay(float decayRate) {
    if (!pImpl) return;
    
    // Apply global decay to all memory slots
    for (auto& slot : pImpl->memorySlots) {
        slot.second.value *= (1.0f - decayRate * slot.second.decayRate);
        slot.second.formationTime += decayRate * 100.0f;
    }
    
    updateLateralInhibition();
}

size_t WorkingMemory::getCapacity() const {
    return pImpl ? pImpl->maxSlots : 100;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl ? pImpl->memorySlots.size() : 0;
}

void WorkingMemory::clear() {
    if (pImpl) {
        pImpl->memorySlots.clear();
        pImpl->totalActivation = 0.0f;
        pImpl->activeSlots = 0;
    }
}

std::vector<std::pair<NeuronId, MemoryTrace>> WorkingMemory::getAllMemory() const {
    return pImpl ? pImpl->memorySlots : std::vector<std::pair<NeuronId, MemoryTrace>>();
}

// Episodic Memory Implementation - Event recording and consolidation
struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItem> episodes;
    size_t maxEpisodes;
    
    // Consolidation queue
    std::queue<EpisodicMemoryItem> consolidationQueue;
    
    // Time-based consolidation
    float consolidationRate;
    float temporalStability;
    
    // Pattern completion
    std::unordered_map<NeuronId, std::vector<size_t>> patternIndex;
    
    Impl(size_t max) : 
        maxEpisodes(max),
        consolidationRate(0.05f),
        temporalStability(0.9f) {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Encode episode with biological constraints
    EpisodicMemoryItem encodedEpisode = encodeEpisodicMemoryItem(episode);
    
    // Add to consolidation queue
    pImpl->consolidationQueue.push(encodedEpisode);
    
    // Add to main storage
    if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
        // Remove oldest episode
        pImpl->episodes.erase(pImpl->episodes.begin());
    }
    pImpl->episodes.push_back(encodedEpisode);
    
    // Update pattern index
    updatePatternIndex(encodedEpisode);
    
    // Trigger consolidation if needed
    if (pImpl->consolidationQueue.size() > pImpl->maxEpisodes * 0.1f) {
        consolidateMemory();
    }
}

EpisodicMemoryItem EpisodicMemory::encodeEpisodicMemoryItem(const EpisodicMemoryItem& episode) {
    EpisodicMemoryItem encoded;
    
    // Biological encoding parameters
    encoded.uniqueId = generateUniqueId();
    encoded.timestamp = getCurrentTimestamp();
    encoded.context = episode.context;
    
    // Encode with synaptic plasticity rules
    encoded.encodingStrength = calculateEncodingStrength(episode.context);
    encoded.stability = calculateStability(encoded.encodingStrength);
    encoded.conversionThreshold = calculateConversionThreshold(encoded.stability);
    
    // Add neuromodulatory signals
    encoded.rewardPrediction = 0.0f;  // TODO: Set from neuromodulation system
    encoded.arousal = calculateArousal(episode.context);
    encoded.encodingQuality = calculateEncodingQuality(episode, encoded);
    
    // Time-based encoding
    encoded.formationTime = 0.0f;
    encoded.recallProbability = calculateRecallProbability(encoded);
    
    return encoded;
}

float EpisodicMemory::calculateEncodingStrength(const MemoryContext& context) const {
    // Complexity-based encoding strength
    float complexity = calculateContextComplexity(context);
    float novelty = calculateNovelty(context);
    
    return std::min(1.0f, (complexity * 0.7f + novelty * 0.3f));
}

float EpisodicMemory::calculateStability(float encodingStrength) const {
    // Stability grows with encoding strength
    return 0.5f + encodingStrength * 0.5f;
}

float EpisodicMemory::calculateConversionThreshold(float stability) const {
    // Conversion threshold depends on stability
    return 0.3f + stability * 0.5f;
}

float EpisodicMemory::calculateArousal(const MemoryContext& context) const {
    // Arousal based on context intensity
    return calculateContextComplexity(context) * 0.8f;
}

float EpisodicMemory::calculateEncodingQuality(const EpisodicMemoryItem& episode, const EpisodicMemoryItem& encoded) const {
    // Quality based on multiple factors
    float temporalCoherence = calculateTemporalCoherence(episode, encoded);
    float semanticRelevance = calculateSemanticRelevance(episode, encoded);
    float neuromodulatoryIntegration = calculateNeuromodulatoryIntegration(episode, encoded);
    
    return (temporalCoherence + semanticRelevance + neuromodulatoryIntegration) / 3.0f;
}

float EpisodicMemory::calculateRecallProbability(const EpisodicMemoryItem& episode) const {
    // Probability of recall depends on time and strength
    float timeSinceFormation = getCurrentTimestamp() - episode.timestamp;
    float timeFactor = std::exp(-timeSinceFormation * 0.01f);  // Decay over time
    
    return timeFactor * episode.encodingStrength * episode.stability;
}

float EpisodicMemory::calculateContextComplexity(const MemoryContext& context) const {
    // Calculate complexity of context
    size_t totalFeatures = 0;
    float totalInteraction = 0.0f;
    
    for (const auto& feature : context.neuralPattern) {
        totalFeatures++;
        // Calculate interactions with other features
        // Simplified: assume some interactions
        totalInteraction += 1.0f;
    }
    
    return std::min(1.0f, static_cast<float>(totalFeatures) / 100.0f + totalInteraction / 1000.0f);
}

float EpisodicMemory::calculateNovelty(const MemoryContext& context) const {
    // Novelty based on deviation from existing patterns
    // Simplified: assume novelty is random based on complexity
    return static_cast<float>(rand()) / RAND_MAX;
}

float EpisodicMemory::calculateTemporalCoherence(const EpisodicMemoryItem& episode, const EpisodicMemoryItem& encoded) const {
    // Coherence based on temporal continuity
    float timeDiff = std::abs(encoded.timestamp - episode.timestamp);
    return std::exp(-timeDiff * 0.01f);
}

float EpisodicMemory::calculateSemanticRelevance(const EpisodicMemoryItem& episode, const EpisodicMemoryItem& encoded) const {
    // Semantic relevance based on context similarity
    float semanticSimilarity = calculateContextSimilarity(episode.context, encoded.context);
    return semanticSimilarity * 0.5f + 0.5f;
}

float EpisodicMemory::calculateNeuromodulatoryIntegration(const EpisodicMemoryItem& episode, const EpisodicMemoryItem& encoded) const {
    // Integration of neuromodulatory signals
    float rewardSignal = episode.rewardPrediction * encoded.rewardPrediction;
    float arousalSignal = episode.arousal * encoded.arousal;
    
    return (rewardSignal + arousalSignal) * 0.5f;
}

float EpisodicMemory::calculateContextSimilarity(const MemoryContext& a, const MemoryContext& b) const {
    // Simple similarity measure
    float similarity = 0.0f;
    
    for (const auto& patternA : a.neuralPattern) {
        for (const auto& patternB : b.neuralPattern) {
            if (patternA == patternB) {
                similarity += 1.0f;
            }
        }
    }
    
    return std::min(1.0f, similarity / std::max(1.0f, static_cast<float>(a.neuralPattern.size())));
}

size_t EpisodicMemory::getEpisodeCount() const {
    return pImpl ? pImpl->episodes.size() : 0;
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    if (!pImpl || index >= pImpl->episodes.size()) {
        return EpisodicMemoryItem();
    }
    
    // Apply recall dynamics
    const EpisodicMemoryItem& episode = pImpl->episodes[index];
    
    EpisodicMemoryItem recalled = episode;
    recalled.recallStrength = episode.encodingStrength * episode.stability * 
                               calculateRecallProbability(episode) * 0.8f;
    
    return recalled;
}

std::vector<EpisodicMemoryItem> EpisodicMemory::getRecentEpisodes(size_t count) const {
    if (!pImpl) return std::vector<EpisodicMemoryItem>();
    
    std::vector<EpisodicMemoryItem> result;
    size_t start = count > pImpl->episodes.size() ? 0 : pImpl->episodes.size() - count;
    
    for (size_t i = start; i < pImpl->episodes.size(); ++i) {
        result.push_back(retrieveEpisode(i));
    }
    
    return result;
}

void EpisodicMemory::clear() {
    if (pImpl) {
        pImpl->episodes.clear();
        while (!pImpl->consolidationQueue.empty()) {
            pImpl->consolidationQueue.pop();
        }
        pImpl->patternIndex.clear();
    }
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    if (!pImpl) return;
    
    // Consolidate high-priority episodes
    std::vector<EpisodicMemoryItem> toConsolidate;
    
    for (const auto& episode : pImpl->episodes) {
        if (shouldConsolidate(episode, relevanceThreshold)) {
            toConsolidate.push_back(episode);
        }
    }
    
    // Promote consolidated episodes to long-term storage
    for (const auto& episode : toConsolidate) {
        pImpl->episodes.erase(std::remove(pImpl->episodes.begin(), pImpl->episodes.end(), episode), 
                             pImpl->episodes.end());
        pImpl->episodes.push_back(episode);
    }
}

bool EpisodicMemory::shouldConsolidate(const EpisodicMemoryItem& episode, float threshold) const {
    // Criteria for consolidation
    float priority = calculateEpisodePriority(episode);
    return priority > threshold;
}

float EpisodicMemory::calculateEpisodePriority(const EpisodicMemoryItem& episode) const {
    // Priority based on encoding strength, arousal, and time since formation
    float timeSinceFormation = getCurrentTimestamp() - episode.timestamp;
    float timePriority = std::exp(-timeSinceFormation * 0.001f);
    
    float encodingPriority = episode.encodingStrength * 0.4f;
    float arousalPriority = episode.arousal * 0.3f;
    float stabilityPriority = episode.stability * 0.3f;
    
    return (timePriority + encodingPriority + arousalPriority + stabilityPriority) * 0.25f;
}

void EpisodicMemory::updatePatternIndex(const EpisodicMemoryItem& episode) {
    if (!pImpl) return;
    
    // Update pattern index for efficient retrieval
    for (const auto& pattern : episode.context.neuralPattern) {
        pImpl->patternIndex[pattern].push_back(pImpl->episodes.size() - 1);
    }
}

size_t EpisodicMemory::generateUniqueId() {
    static size_t nextId = 0;
    return nextId++;
}

float EpisodicMemory::getCurrentTimestamp() const {
    // Return current simulation time (simplified)
    return static_cast<float>(std::time(nullptr)) * 0.001f;  // Convert to seconds
}

} // namespace nlm
