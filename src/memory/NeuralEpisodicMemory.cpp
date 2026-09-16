#include "NeuralEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <functional>
#include <mutex>
#include <limits>
#include <random>
#include <chrono>

namespace nlm {

struct MemorySchema {
    std::string schemaName;
    std::vector<std::string> patternTemplates;
    std::vector<SimulationStep> occurrenceTimestamps;
    float schemaStrength;
    
    MemorySchema(const std::string& name) : schemaName(name), schemaStrength(0.0f) {}
};

struct EventSequence {
    std::vector<size_t> episodeIndices;
    float sequenceCoherence;
    SimulationStep sequenceStart;
    SimulationStep sequenceEnd;
    
    EventSequence() : sequenceCoherence(0.0f), sequenceStart(0), sequenceEnd(0) {}
};

struct TemporalContext {
    SimulationStep temporalWindowStart;
    SimulationStep temporalWindowEnd;
    std::unordered_map<std::string, float> contextualWeight;
    
    TemporalContext(SimulationStep start, SimulationStep end) : 
        temporalWindowStart(start), temporalWindowEnd(end) {}
};

struct ProbabilisticInference {
    std::vector<std::pair<float, size_t>> retrievalProbabilities;
    float totalWeight;
    
    ProbabilisticInference() : totalWeight(0.0f) {}
    
    float getProbability(size_t index) const {
        if (totalWeight > 0.0f) {
            for (const auto& prob : retrievalProbabilities) {
                if (prob.second == index) {
                    return prob.first / totalWeight;
                }
            }
        }
        return 0.0f;
    }
};

struct MemoryGap {
    SimulationStep startTime;
    SimulationStep endTime;
    float gapSize;
    std::string gapType; // "temporal", "contextual", "sensory"
    float coherenceCost;
    
    MemoryGap(SimulationStep start, SimulationStep end) : 
        startTime(start), endTime(end), gapSize(end - start) {}
};

struct MetaMemoryInfo {
    std::vector<SimulationStep> retrievalTimestamps;
    std::vector<float> retrievalConfidence;
    std::vector<float> memoryStrength;
    std::vector<float> relevanceScore;
    std::vector<float> agingScore;
    std::vector<size_t> consolidationCount;
    
    float getAverageConfidence() const {
        if (retrievalConfidence.empty()) return 0.0f;
        float sum = 0.0f;
        for (float conf : retrievalConfidence) sum += conf;
        return sum / retrievalConfidence.size();
    }
    
    float getAverageStrength() const {
        if (memoryStrength.empty()) return 0.0f;
        float sum = 0.0f;
        for (float str : memoryStrength) sum += str;
        return sum / memoryStrength.size();
    }
};

struct MemoryBinding {
    std::vector<NeuronId> sensoryNeurons;
    std::vector<NeuronId> motorNeurons;
    std::vector<NeuronId> contextualNeurons;
    float bindingStrength;
    SimulationStep bindingTimestamp;
    
    MemoryBinding() : bindingStrength(0.0f), bindingTimestamp(0) {}
};

struct IntegrationWeights {
    std::unordered_map<std::string, float> workingMemoryWeight;
    std::unordered_map<std::string, float> predictionSystemWeight;
    std::unordered_map<std::string, float> neuromodulationWeight;
    std::unordered_map<std::string, float> attentionWeight;
    
    IntegrationWeights() {
        workingMemoryWeight["storage"] = 0.3f;
        workingMemoryWeight["retrieval"] = 0.4f;
        predictionSystemWeight["prediction"] = 0.5f;
        neuromodulationWeight["updating"] = 0.6f;
        neuromodulationWeight["suppression"] = 0.3f;
        attentionWeight["gating"] = 0.7f;
    }
};

struct NeuralEpisodicMemory::Impl {
    Brain* brain;
    
    // Pattern neurons for episodic recall
    std::vector<NeuronId> episodeNeurons;
    
    // Advanced memory consolidation
    std::vector<std::vector<NeuronId>> hippocampalPatterns;
    std::vector<std::vector<NeuronId>> neocorticalPatterns;
    std::deque<std::pair<SimulationStep, SimulationStep>> consolidationWindow;
    float consolidationThreshold;
    bool systemsConsolidationEnabled;
    
    // Memory indexing and search optimization
    std::unordered_map<std::string, std::vector<size_t>> temporalIndex;
    std::unordered_map<float, std::vector<size_t>> spatialIndex;
    std::unordered_map<float, std::vector<size_t>> rewardIndex;
    std::unordered_map<std::string, std::vector<size_t>> schemaIndex;
    mutable std::mutex indexMutex;
    
    // Memory categorization
    std::vector<MemorySchema> schemas;
    std::unordered_map<std::string, std::vector<size_t>> temporalCategory;
    std::unordered_map<std::string, std::vector<size_t>> spatialCategory;
    std::unordered_map<std::string, std::vector<size_t>> rewardCategory;
    
    // Memory compression
    std::vector<std::vector<float>> compressedPatterns;
    std::vector<size_t> compressionIndices;
    float compressionRatio;
    
    // Multimodal memory binding
    std::vector<MemoryBinding> multimodalBindings;
    std::unordered_map<size_t, std::vector<size_t>> bindingMap;
    
    // Memory sequencing
    std::vector<EventSequence> eventSequences;
    std::vector<MemoryGap> memoryGaps;
    
    // Probabilistic inference for retrieval
    std::unordered_map<size_t, ProbabilisticInference> probabilisticRetrievals;
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    
    // Memory reconsolidation
    std::unordered_map<size_t, std::vector<float>> reconsolidationTriggers;
    std::vector<std::pair<size_t, SimulationStep>> reconsolidationQueue;
    
    // Memory suppression and inhibition
    std::unordered_map<size_t, float> suppressionStrengths;
    std::vector<size_t> inhibitedMemories;
    
    // Memory interference resolution
    std::vector<std::pair<size_t, size_t>> interferencePairs;
    std::vector<float> interferenceWeights;
    
    // Meta-memory and monitoring
    std::unordered_map<size_t, MetaMemoryInfo> metaMemory;
    
    // Integration with other systems
    IntegrationWeights integrationWeights;
    std::vector<std::vector<float>> workingMemoryPatterns;
    std::vector<float> predictionErrors;
    std::vector<float> neuromodulationSignals;
    std::vector<NeuronId> attentionGatingNeurons;
    
    Impl() : brain(nullptr), consolidationThreshold(0.7f), systemsConsolidationEnabled(true), 
             compressionRatio(0.5f) {
        rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
        dist = std::uniform_real_distribution<float>(0.0f, 1.0f);
    }
};

void NeuralEpisodicMemory::updateMemoryIndices(const EpisodicMemoryItem& episode) {
    std::lock_guard<std::mutex> lock(pImpl->indexMutex);
    
    // Build temporal index
    std::string timeKey = "time_" + std::to_string(episode.timestamp / 1000); // Group by seconds
    pImpl->temporalIndex[timeKey].push_back(episodes_.size() - 1);
    
    // Build spatial index
    float spatialKey = (episode.positionX + episode.positionY) / 2.0f;
    pImpl->spatialIndex[spatialKey].push_back(episodes_.size() - 1);
    
    // Build reward index
    pImpl->rewardIndex[episode.reward].push_back(episodes_.size() - 1);
    
    // Build schema index
    if (auto* schema = AdvancedMemoryFeatures::getSchemaForEpisode(episode, pImpl->schemas)) {
        pImpl->schemaIndex[schema->schemaName].push_back(episodes_.size() - 1);
    }
    
    // Cleanup old indices
    if (episodes_.size() > maxEpisodes_ * 2) {
        cleanupOldIndices();
    }
}

void NeuralEpisodicMemory::updateSystemsConsolidation() {
    if (!pImpl->systemsConsolidationEnabled) return;
    
    // Hippocampal-neocortical interaction
    static SimulationStep consolidationCounter = 0;
    consolidationCounter++;
    
    // Perform systems consolidation every N steps
    if (consolidationCounter % 1000 == 0) {
        SimulationStep currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        
        // Transfer recent hippocampal patterns to neocortical storage
        SimulationStep consolidationWindowEnd = currentTime;
        SimulationStep consolidationWindowStart = currentTime - 10000; // 10 seconds window
        
        AdvancedMemoryFeatures::systemsConsolidation(
            pImpl->hippocampalPatterns, 
            pImpl->neocorticalPatterns,
            pImpl->consolidationThreshold,
            consolidationWindowStart
        );
        
        pImpl->consolidationWindow.push_back(
            std::make_pair(consolidationWindowStart, consolidationWindowEnd)
        );
        
        if (pImpl->consolidationWindow.size() > 100) {
            pImpl->consolidationWindow.pop_front();
        }
    }
}

void NeuralEpisodicMemory::updateMemoryCategoricalInformation(const EpisodicMemoryItem& episode) {
    // Categorize by temporal patterns
    std::string temporalCategory = "normal";
    if (episode.age > 100) temporalCategory = "old"; // More than 100 simulation steps
    else if (episode.age < 10) temporalCategory = "recent";
    
    pImpl->temporalCategory[temporalCategory].push_back(episodes_.size() - 1);
    
    // Categorize by spatial context
    std::string spatialCategory = "central";
    float distanceFromCenter = std::sqrt(episode.positionX * episode.positionX + 
                                        episode.positionY * episode.positionY);
    if (distanceFromCenter > 100.0f) spatialCategory = "distant";
    else if (distanceFromCenter < 10.0f) spatialCategory = "near";
    
    pImpl->spatialCategory[spatialCategory].push_back(episodes_.size() - 1);
    
    // Categorize by reward
    std::string rewardCategory = "neutral";
    if (episode.reward > 0.5f) rewardCategory = "high_reward";
    else if (episode.reward < -0.5f) rewardCategory = "negative";
    
    pImpl->rewardCategory[rewardCategory].push_back(episodes_.size() - 1);
    
    // Update schemas based on patterns
    if (auto* schema = AdvancedMemoryFeatures::getSchemaForEpisode(episode, pImpl->schemas)) {
        // Increment schema strength
        schema->schemaStrength = std::min(1.0f, schema->schemaStrength + 0.01f);
        schema->occurrenceTimestamps.push_back(episode.timestamp);
    } else {
        // Create new schema if needed
        MemorySchema newSchema("auto_schema_" + std::to_string(pImpl->schemas.size()));
        newSchema.patternTemplates.push_back("#sensory_pattern#");
        newSchema.schemaStrength = 0.1f;
        newSchema.occurrenceTimestamps.push_back(episode.timestamp);
        pImpl->schemas.push_back(newSchema);
    }
}

void NeuralEpisodicMemory::updateMemoryCompression(const EpisodicMemoryItem& episode) {
    // Apply compression to reduce memory footprint
    if (pImpl->compressionRatio <= 0.0f) return;
    
    std::vector<float> compressedPattern;
    float compressionFactor = pImpl->compressionRatio;
    
    // Simple compression: sample every Nth element
    size_t sampleInterval = std::max(1, static_cast<int>(1.0f / compressionFactor));
    
    for (size_t i = 0; i < episode.sensoryState.size(); i += sampleInterval) {
        compressedPattern.push_back(episode.sensoryState[i]);
    }
    
    // Store compressed pattern
    pImpl->compressionIndices.push_back(episodes_.size() - 1);
    pImpl->compressedPatterns.push_back(compressedPattern);
    
    // Limit compression storage
    if (pImpl->compressedPatterns.size() > maxEpisodes_ * 2) {
        pImpl->compressedPatterns.erase(pImpl->compressedPatterns.begin());
        pImpl->compressionIndices.erase(pImpl->compressionIndices.begin());
    }
}

void NeuralEpisodicMemory::updateMultimodalBindings(const EpisodicMemoryItem& episode) {
    MemoryBinding binding;
    
    // Extract multimodal features
    binding.sensoryNeurons = episode.activeNeurons; // Simplification: use active neurons as proxy
    
    // Create motor pattern from action
    std::vector<NeuronId> motorPattern;
    motorPattern.reserve(episode.activeNeurons.size());
    
    // Simplified motor encoding
    for (size_t i = 0; i < std::min(episode.activeNeurons.size(), (size_t)5); ++i) {
        motorPattern.push_back(episode.activeNeurons[i] + 1000); // Offset for motor region
    }
    binding.motorNeurons = motorPattern;
    
    // Create contextual binding
    std::vector<NeuronId> contextualPattern;
    contextualPattern.reserve(3);
    contextualPattern.push_back(episode.activeNeurons.empty() ? NeuronId(0) : episode.activeNeurons[0]);
    contextualPattern.push_back(static_cast<NeuronId>(episode.reward > 0.0f ? 1 : 0));
    contextualPattern.push_back(static_cast<NeuronId>(episode.novelty > 0.5f ? 1 : 0));
    binding.contextualNeurons = contextualPattern;
    
    binding.bindingStrength = 0.5f + (episode.novelty * 0.5f) + (episode.reward * 0.3f);
    binding.bindingTimestamp = episode.timestamp;
    
    pImpl->multimodalBindings.push_back(binding);
    pImpl->bindingMap[episodes_.size() - 1].push_back(pImpl->multimodalBindings.size() - 1);
    
    // Limit bindings storage
    if (pImpl->multimodalBindings.size() > maxEpisodes_ * 3) {
        pImpl->multimodalBindings.erase(pImpl->multimodalBindings.begin());
        // Cleanup bindingMap
        for (auto& pair : pImpl->bindingMap) {
            for (auto it = pair.second.begin(); it != pair.second.end(); ) {
                if (*it >= pImpl->multimodalBindings.size()) {
                    it = pair.second.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }
}

void NeuralEpisodicMemory::updateEventSequencing(const EpisodicMemoryItem& episode) {
    // Create new event sequence if this is a potential sequence start
    if (episode.action == ActionType::Interact || episode.novelty > 0.7f) {
        EventSequence sequence;
        sequence.episodeIndices.push_back(episodes_.size() - 1);
        sequence.sequenceStart = episode.timestamp;
        sequence.sequenceCoherence = 0.8f;
        pImpl->eventSequences.push_back(sequence);
    } else {
        // Add to existing sequence if possible
        for (auto& seq : pImpl->eventSequences) {
            // Simple sequence extension: add if within time window
            if (episode.timestamp - seq.sequenceEnd < 1000 && 
                seq.sequenceCoherence > 0.3f) {
                seq.episodeIndices.push_back(episodes_.size() - 1);
                seq.sequenceCoherence = std::min(1.0f, seq.sequenceCoherence + 0.05f);
                seq.sequenceEnd = episode.timestamp;
                break;
            }
        }
    }
    
    // Check for memory gaps
    if (!pImpl->memoryGaps.empty()) {
        for (auto it = pImpl->memoryGaps.begin(); it != pImpl->memoryGaps.end(); ) {
            if (episode.timestamp >= it->endTime) {
                it = pImpl->memoryGaps.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void NeuralEpisodicMemory::updateMetaMemoryInfo(const EpisodicMemoryItem& episode) {
    MetaMemoryInfo metaInfo;
    metaInfo.retrievalTimestamps.push_back(episode.timestamp);
    metaInfo.retrievalConfidence.push_back(0.5f); // Initial confidence
    metaInfo.memoryStrength.push_back(0.5f); // Initial strength
    metaInfo.relevanceScore.push_back(episode.reward); // Use reward as proxy for relevance
    metaInfo.agingScore.push_back(std::min(1.0f, static_cast<float>(episode.age) / 1000.0f)); // Aging score
    metaInfo.consolidationCount.push_back(0);
    
    pImpl->metaMemory[episodes_.size() - 1] = metaInfo;
}

const MemorySchema* NeuralEpisodicMemory::getSchema(const std::string& schemaName) const {
    for (const auto& schema : pImpl->schemas) {
        if (schema.schemaName == schemaName) {
            return &schema;
        }
    }
    return nullptr;
}

void NeuralEpisodicMemory::createSchema(const std::string& schemaName, 
                                       const std::vector<std::string>& patternTemplates) {
    MemorySchema schema(schemaName);
    schema.patternTemplates = patternTemplates;
    schema.schemaStrength = 0.0f;
    pImpl->schemas.push_back(schema);
}

std::vector<const EventSequence*> NeuralEpisodicMemory::getEventSequencesForEpisode(
    size_t episodeIndex) const {
    std::vector<const EventSequence*> sequences;
    
    for (const auto& seq : pImpl->eventSequences) {
        if (std::find(seq.episodeIndices.begin(), seq.episodeIndices.end(), episodeIndex) != seq.episodeIndices.end()) {
            sequences.push_back(&seq);
        }
    }
    
    return sequences;
}

void NeuralEpisodicMemory::fillMemoryGaps() {
    // Identify gaps in episode timeline
    if (episodes_.size() < 2) return;
    
    std::vector<MemoryGap> newGaps;
    
    // Sort episodes by timestamp
    std::vector<const EpisodicMemoryItem*> sortedEpisodes;
    for (size_t i = 0; i < episodes_.size(); ++i) {
        sortedEpisodes.push_back(&episodes_[i]);
    }
    
    std::sort(sortedEpisodes.begin(), sortedEpisodes.end(),
              [](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
                  return a->timestamp < b->timestamp;
              });
    
    // Find gaps between consecutive episodes
    for (size_t i = 1; i < sortedEpisodes.size(); ++i) {
        SimulationStep gapStart = sortedEpisodes[i-1]->timestamp;
        SimulationStep gapEnd = sortedEpisodes[i]->timestamp;
        SimulationStep gapSize = gapEnd - gapStart;
        
        // Only consider significant gaps
        if (gapSize > 100.0f) { // 100 simulation steps threshold
            MemoryGap gap(gapStart, gapEnd);
            gap.gapType = "temporal";
            gap.coherenceCost = gapSize / 1000.0f;
            newGaps.push_back(gap);
        }
    }
    
    pImpl->memoryGaps.insert(pImpl->memoryGaps.end(), newGaps.begin(), newGaps.end());
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::probabilisticRetrieve(
    const std::vector<float>& queryPattern,
    float confidenceThreshold) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    // Build probabilistic inference
    ProbabilisticInference inference;
    inference.retrievalProbabilities.clear();
    inference.totalWeight = 0.0f;
    
    // Compute similarities and probabilities
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float similarity = computeSimilarity(queryPattern, episodes_[i]);
        
        if (similarity > 0.1f) {
            float probability = similarity * (0.5f + pImpl->metaMemory.at(i).getAverageConfidence() * 0.5f);
            inference.retrievalProbabilities.emplace_back(probability, i);
            inference.totalWeight += probability;
        }
    }
    
    // Store inference for meta-memory
    pImpl->probabilisticRetrievals[episodes_.size()] = inference;
    
    // Filter by confidence threshold
    for (const auto& probPair : inference.retrievalProbabilities) {
        if (probPair.first / inference.totalWeight > confidenceThreshold) {
            results.push_back(&episodes_[probPair.second]);
        }
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::patternComplete(
    const std::vector<float>& partialPattern,
    size_t maxResults) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    // Find best matching patterns
    std::vector<std::pair<float, size_t>> matches;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float similarity = computeSimilarity(partialPattern, episodes_[i]);
        
        if (similarity > 0.3f) {
            matches.emplace_back(similarity, i);
        }
    }
    
    // Sort by similarity
    std::sort(matches.begin(), matches.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    for (size_t i = 0; i < std::min(maxResults, matches.size()); ++i) {
        results.push_back(&episodes_[matches[i].second]);
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::reconstructSequence(
    SimulationStep startTime,
    SimulationStep endTime,
    size_t minSequenceLength) const {
    std::vector<const EpisodicMemoryItem*> sequence;
    
    // Find episodes within the time window
    for (const auto& episode : episodes_) {
        if (episode.timestamp >= startTime && episode.timestamp <= endTime) {
            sequence.push_back(&episode);
        }
    }
    
    // Sort by timestamp
    std::sort(sequence.begin(), sequence.end(),
              [](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
                  return a->timestamp < b->timestamp;
              });
    
    // Filter by minimum sequence length
    if (sequence.size() < minSequenceLength) {
        return {};
    }
    
    return sequence;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::contextSensitiveRetrieve(
    const std::vector<float>& sensoryQuery,
    SimulationStep* startTime,
    SimulationStep* endTime,
    ActionType* actionQuery,
    float contextWeight) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    for (const auto& episode : episodes_) {
        bool matches = true;
        
        // Check sensory similarity
        if (!sensoryQuery.empty()) {
            float similarity = computeSimilarity(sensoryQuery, episode);
            if (similarity < 0.3f) {
                matches = false;
            }
        }
        
        // Check time window
        if (startTime && endTime) {
            if (episode.timestamp < *startTime || episode.timestamp > *endTime) {
                matches = false;
            }
        }
        
        // Check action
        if (actionQuery && episode.action != *actionQuery) {
            matches = false;
        }
        
        if (matches) {
            results.push_back(&episode);
        }
    }
    
    return results;
}

void NeuralEpisodicMemory::updateMemoryWithNewInformation(
    const EpisodicMemoryItem& originalEpisode,
    const EpisodicMemoryItem& updatedEpisode) {
    size_t originalIndex = 0;
    for (size_t i = 0; i < episodes_.size(); ++i) {
        if (&episodes_[i] == &originalEpisode) {
            originalIndex = i;
            break;
        }
    }
    
    // Store reconsolidation trigger
    std::vector<float> triggerInfo;
    triggerInfo.push_back(updatedEpisode.reward - originalEpisode.reward);
    triggerInfo.push_back(updatedEpisode.novelty - originalEpisode.novelty);
    triggerInfo.push_back(updatedEpisode.energy - originalEpisode.energy);
    
    pImpl->reconsolidationTriggers[originalIndex] = triggerInfo;
    pImpl->reconsolidationQueue.emplace_back(originalIndex, updatedEpisode.timestamp);
    
    // Update the episode with new information
    EpisodicMemoryItem& storedEpisode = episodes_[originalIndex];
    storedEpisode = updatedEpisode;
}

void NeuralEpisodicMemory::triggerReconsolidation(
    size_t episodeIndex,
    const std::vector<float>& newInformation) {
    if (episodeIndex >= episodes_.size()) return;
    
    // Update episode with new information
    EpisodicMemoryItem& episode = episodes_[episodeIndex];
    
    // Apply new information
    if (!newInformation.empty()) {
        if (newInformation.size() > 0 && newInformation[0] > 0.0f) {
            episode.reward = std::min(1.0f, episode.reward + newInformation[0] * 0.1f);
        }
        
        if (newInformation.size() > 1 && newInformation[1] > 0.0f) {
            episode.novelty = std::min(1.0f, episode.novelty + newInformation[1] * 0.1f);
        }
        
        if (newInformation.size() > 2 && newInformation[2] > 0.0f) {
            episode.energy = std::min(1.0f, episode.energy + newInformation[2] * 0.1f);
        }
    }
    
    // Update meta-memory for reconsolidation
    if (pImpl->metaMemory.find(episodeIndex) != pImpl->metaMemory.end()) {
        pImpl->metaMemory[episodeIndex].consolidationCount.back()++;
    }
}

void NeuralEpisodicMemory::suppressMemory(
    size_t episodeIndex,
    float suppressionStrength) {
    if (episodeIndex >= episodes_.size()) return;
    
    pImpl->suppressionStrengths[episodeIndex] = suppressionStrength;
    pImpl->inhibitedMemories.push_back(episodeIndex);
    
    // Apply suppression effects
    EpisodicMemoryItem& episode = episodes_[episodeIndex];
    episode.reward *= (1.0f - suppressionStrength);
    episode.energy *= (1.0f - suppressionStrength);
    
    // Update meta-memory
    if (pImpl->metaMemory.find(episodeIndex) != pImpl->metaMemory.end()) {
        pImpl->metaMemory[episodeIndex].memoryStrength.back() *= (1.0f - suppressionStrength);
    }
}

void NeuralEpisodicMemory::inhibitMemory(
    size_t episodeIndex,
    float inhibitionDuration) {
    if (episodeIndex >= episodes_.size()) return;
    
    pImpl->inhibitedMemories.push_back(episodeIndex);
    
    // Mark episode as inhibited
    EpisodicMemoryItem& episode = episodes_[episodeIndex];
    episode.age += static_cast<SimulationStep>(inhibitionDuration);
    
    // Update meta-memory
    if (pImpl->metaMemory.find(episodeIndex) != pImpl->metaMemory.end()) {
        pImpl->metaMemory[episodeIndex].agingScore.back() = 1.0f;
    }
}

void NeuralEpisodicMemory::resolveMemoryInterference(
    size_t episodeIndexA,
    size_t episodeIndexB,
    float interferenceStrength) {
    if (episodeIndexA >= episodes_.size() || episodeIndexB >= episodes_.size()) return;
    
    pImpl->interferencePairs.emplace_back(episodeIndexA, episodeIndexB);
    pImpl->interferenceWeights.push_back(interferenceStrength);
    
    // Resolve interference by adjusting episode attributes
    EpisodicMemoryItem& episodeA = episodes_[episodeIndexA];
    EpisodicMemoryItem& episodeB = episodes_[episodeIndexB];
    
    // Reduce similarity between the two episodes
    float currentSimilarity = computeSimilarity(episodeA.sensoryState, episodeB.sensoryState);
    
    if (currentSimilarity > 0.5f) {
        // Apply interference resolution
        float adjustmentFactor = 1.0f - interferenceStrength;
        
        episodeA.reward *= adjustmentFactor;
        episodeA.energy *= adjustmentFactor;
        episodeA.novelty *= adjustmentFactor;
        
        episodeB.reward *= adjustmentFactor;
        episodeB.energy *= adjustmentFactor;
        episodeB.novelty *= adjustmentFactor;
    }
}

void NeuralEpisodicMemory::updateMetaMemoryMonitoring(
    size_t episodeIndex,
    float retrievalConfidence,
    float memoryStrength,
    float relevanceScore,
    float agingScore) {
    if (pImpl->metaMemory.find(episodeIndex) == pImpl->metaMemory.end()) {
        MetaMemoryInfo metaInfo;
        pImpl->metaMemory[episodeIndex] = metaInfo;
    }
    
    MetaMemoryInfo& metaInfo = pImpl->metaMemory[episodeIndex];
    metaInfo.retrievalTimestamps.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
    metaInfo.retrievalConfidence.push_back(retrievalConfidence);
    metaInfo.memoryStrength.push_back(memoryStrength);
    metaInfo.relevanceScore.push_back(relevanceScore);
    metaInfo.agingScore.push_back(agingScore);
    metaInfo.consolidationCount.push_back(0);
    
    // Limit history size
    if (metaInfo.retrievalTimestamps.size() > 100) {
        metaInfo.retrievalTimestamps.erase(metaInfo.retrievalTimestamps.begin());
        metaInfo.retrievalConfidence.erase(metaInfo.retrievalConfidence.begin());
        metaInfo.memoryStrength.erase(metaInfo.memoryStrength.begin());
        metaInfo.relevanceScore.erase(metaInfo.relevanceScore.begin());
        metaInfo.agingScore.erase(metaInfo.agingScore.begin());
        metaInfo.consolidationCount.erase(metaInfo.consolidationCount.begin());
    }
}

float NeuralEpisodicMemory::getMetaMemoryAverageConfidence() const {
    float totalConfidence = 0.0f;
    int count = 0;
    
    for (const auto& pair : pImpl->metaMemory) {
        totalConfidence += pair.second.getAverageConfidence();
        count++;
    }
    
    return count > 0 ? totalConfidence / count : 0.0f;
}

float NeuralEpisodicMemory::getMetaMemoryAverageStrength() const {
    float totalStrength = 0.0f;
    int count = 0;
    
    for (const auto& pair : pImpl->metaMemory) {
        totalStrength += pair.second.getAverageStrength();
        count++;
    }
    
    return count > 0 ? totalStrength / count : 0.0f;
}

std::vector<size_t> NeuralEpisodicMemory::getPrioritizedEpisodesForConsolidation(
    size_t maxEpisodes) const {
    std::vector<std::pair<float, size_t>> prioritized;
    
    for (const auto& pair : pImpl->metaMemory) {
        float priorityScore = 0.0f;
        
        if (!pair.second.retrievalConfidence.empty()) {
            priorityScore += pair.second.getAverageConfidence() * 0.4f;
        }
        
        if (!pair.second.memoryStrength.empty()) {
            priorityScore += pair.second.getAverageStrength() * 0.3f;
        }
        
        if (!pair.second.relevanceScore.empty()) {
            float avgRelevance = 0.0f;
            for (float rel : pair.second.relevanceScore) avgRelevance += rel;
            avgRelevance /= pair.second.relevanceScore.size();
            priorityScore += avgRelevance * 0.2f;
        }
        
        if (!pair.second.agingScore.empty()) {
            float avgAging = 0.0f;
            for (float age : pair.second.agingScore) avgAging += age;
            avgAging /= pair.second.agingScore.size();
            priorityScore += (1.0f - avgAging) * 0.1f; // Older memories have lower priority
        }
        
        prioritized.emplace_back(priorityScore, pair.first);
    }
    
    // Sort by priority (descending)
    std::sort(prioritized.begin(), prioritized.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Extract indices
    std::vector<size_t> result;
    for (size_t i = 0; i < std::min(maxEpisodes, prioritized.size()); ++i) {
        result.push_back(prioritized[i].second);
    }
    
    return result;
}

void NeuralEpisodicMemory::integrateWithWorkingMemory(
    const std::vector<float>& workingMemoryPattern) {
    // Store working memory pattern for integration
    pImpl->workingMemoryPatterns.push_back(workingMemoryPattern);
    
    // Limit storage
    if (pImpl->workingMemoryPatterns.size() > 100) {
        pImpl->workingMemoryPatterns.erase(pImpl->workingMemoryPatterns.begin());
    }
    
    // Use working memory pattern to influence episodic memory
    if (!workingMemoryPattern.empty() && brain_) {
        // Apply working memory influence to recent episodes
        size_t influenceCount = std::min(size_t(10), episodes_.size());
        for (size_t i = 0; i < influenceCount; ++i) {
            size_t episodeIndex = episodes_.size() - 1 - i;
            
            if (pImpl->metaMemory.find(episodeIndex) != pImpl->metaMemory.end()) {
                // Update meta-memory based on working memory pattern
                float patternSimilarity = computeSimilarity(workingMemoryPattern, episodes_[episodeIndex].sensoryState);
                pImpl->metaMemory[episodeIndex].relevanceScore.back() = 
                    std::max(pImpl->metaMemory[episodeIndex].relevanceScore.back(),
                            patternSimilarity * pImpl->integrationWeights.workingMemoryWeight["storage"]);
            }
        }
    }
}

void NeuralEpisodicMemory::integrateWithPredictionSystem(
    float predictionError,
    const std::vector<float>& predictedPattern) {
    // Store prediction error
    pImpl->predictionErrors.push_back(predictionError);
    
    // Store predicted pattern
    pImpl->workingMemoryPatterns.push_back(predictedPattern);
    
    // Limit storage
    if (pImpl->predictionErrors.size() > 100) {
        pImpl->predictionErrors.erase(pImpl->predictionErrors.begin());
    }
    
    if (pImpl->workingMemoryPatterns.size() > 100) {
        pImpl->workingMemoryPatterns.erase(pImpl->workingMemoryPatterns.begin());
    }
    
    // Use prediction error to influence memory updating
    if (predictionError > pImpl->integrationWeights.predictionSystemWeight["prediction"] * 2.0f) {
        // High prediction error triggers memory updating
        for (size_t i = 0; i < episodes_.size(); ++i) {
            if (pImpl->metaMemory.find(i) != pImpl->metaMemory.end()) {
                // Update based on prediction error
                float updateWeight = predictionError * pImpl->integrationWeights.predictionSystemWeight["prediction"];
                pImpl->metaMemory[i].memoryStrength.back() = 
                    std::min(1.0f, pImpl->metaMemory[i].memoryStrength.back() + updateWeight * 0.1f);
            }
        }
    }
}

void NeuralEpisodicMemory::integrateWithNeuromodulation(
    float neuromodulationSignal,
    const std::string& modulationType) {
    // Store neuromodulation signal
    pImpl->neuromodulationSignals.push_back(neuromodulationSignal);
    
    // Limit storage
    if (pImpl->neuromodulationSignals.size() > 100) {
        pImpl->neuromodulationSignals.erase(pImpl->neuromodulationSignals.begin());
    }
    
    // Apply neuromodulation effects based on type
    if (modulationType == "updating") {
        // Update strength
        for (size_t i = 0; i < episodes_.size(); ++i) {
            if (pImpl->metaMemory.find(i) != pImpl->metaMemory.end()) {
                float updateStrength = neuromodulationSignal * pImpl->integrationWeights.neuromodulationWeight["updating"];
                pImpl->metaMemory[i].memoryStrength.back() = 
                    std::min(1.0f, pImpl->metaMemory[i].memoryStrength.back() + updateStrength * 0.05f);
            }
        }
    } else if (modulationType == "suppression") {
        // Suppress memories
        float suppressionEffect = neuromodulationSignal * pImpl->integrationWeights.neuromodulationWeight["suppression"];
        for (size_t i = 0; i < episodes_.size() && i < 10; ++i) { // Affect most recent memories
            suppressMemory(i, suppressionEffect);
        }
    }
}

void NeuralEpisodicMemory::integrateWithAttention(
    const std::vector<NeuronId>& attendedNeurons,
    float attentionLevel) {
    // Store attended neurons
    pImpl->attentionGatingNeurons = attendedNeurons;
    
    // Apply attention gating effects
    for (size_t i = 0; i < episodes_.size(); ++i) {
        // Check if episode involves attended neurons
        bool isAttended = false;
        for (const auto& neuron : pImpl->attentionGatingNeurons) {
            if (std::find(episodes_[i].activeNeurons.begin(), episodes_[i].activeNeurons.end(), neuron) != episodes_[i].activeNeurons.end()) {
                isAttended = true;
                break;
            }
        }
        
        if (isAttended && pImpl->metaMemory.find(i) != pImpl->metaMemory.end()) {
            // Update meta-memory based on attention
            float attentionWeight = attentionLevel * pImpl->integrationWeights.attentionWeight["gating"];
            pImpl->metaMemory[i].relevanceScore.back() = 
                std::max(pImpl->metaMemory[i].relevanceScore.back(),
                        attentionWeight);
        }
    }
}

void NeuralEpisodicMemory::cleanupOldIndices() {
    // Remove oldest entries from indices to prevent memory leak
    for (auto& pair : pImpl->temporalIndex) {
        if (pair.second.size() > maxEpisodes_) {
            pair.second.erase(pair.second.begin());
        }
    }
    
    for (auto& pair : pImpl->spatialIndex) {
        if (pair.second.size() > maxEpisodes_) {
            pair.second.erase(pair.second.begin());
        }
    }
    
    for (auto& pair : pImpl->rewardIndex) {
        if (pair.second.size() > maxEpisodes_) {
            pair.second.erase(pair.second.begin());
        }
    }
    
    for (auto& pair : pImpl->schemaIndex) {
        if (pair.second.size() > maxEpisodes_) {
            pair.second.erase(pair.second.begin());
        }
    }
}

NeuralEpisodicMemory::NeuralEpisodicMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , maxEpisodes_(1000)
    , replayEnabled_(true)
{
    NLM_LOG_INFO("NeuralEpisodicMemory created with advanced features");
}

NeuralEpisodicMemory::~NeuralEpisodicMemory() = default;

void NeuralEpisodicMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralEpisodicMemory initialized");
}

void NeuralEpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Create episode copy with age 0
    EpisodicMemoryItem stored = episode;
    stored.age = 0;
    
    episodes_.push_back(stored);
    
    // Create episode neuron for pattern completion
    if (brain_ && !episode.sensoryState.empty()) {
        NeuronId epNeuron(episodes_.size() + 20000);
        pImpl->episodeNeurons.push_back(epNeuron);
        
        // Store episode index in the neuron
        // (This is a simplified approach - real implementation would use more distributed encoding)
    }
    
    // Remove old episodes if over capacity
    while (episodes_.size() > maxEpisodes_) {
        episodes_.erase(episodes_.begin());
        if (!pImpl->episodeNeurons.empty()) {
            pImpl->episodeNeurons.erase(pImpl->episodeNeurons.begin());
        }
    }
    
    // Age all episodes
    for (auto& ep : episodes_) {
        ep.age++;
    }
    
    // Update indices and memory systems
    updateMemoryIndices(stored);
    updateSystemsConsolidation();
    updateMemoryCategoricalInformation(stored);
    updateMemoryCompression(stored);
    updateMultimodalBindings(stored);
    updateEventSequencing(stored);
    updateMetaMemoryInfo(stored);
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveSimilar(
    const std::vector<float>& sensoryPattern,
    size_t maxResults) const {
    
    std::vector<std::pair<float, size_t>> scores;  // (similarity, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float sim = computeSimilarity(sensoryPattern, episodes_[i]);
        scores.emplace_back(sim, i);
    }
    
    // Sort by similarity (descending)
    std::sort(scores.begin(), scores.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(maxResults, scores.size()); ++i) {
        if (scores[i].first > 0.3f) {  // Threshold
            results.push_back(&episodes_[scores[i].second]);
        }
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveTemporal(
    SimulationStep startTime,
    SimulationStep endTime,
    size_t maxResults) const {
    
    std::vector<const EpisodicMemoryItem*> results;
    
    for (const auto& ep : episodes_) {
        if (ep.timestamp >= startTime && ep.timestamp <= endTime) {
            results.push_back(&ep);
        }
    }
    
    // Return most recent first
    std::sort(results.begin(), results.end(),
        [](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
            return a->timestamp > b->timestamp;
        });
    
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveByLocation(
    float x, float y, float radius, size_t maxResults) const {
    
    std::vector<std::pair<float, size_t>> distances;  // (distance, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float dx = episodes_[i].positionX - x;
        float dy = episodes_[i].positionY - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist <= radius) {
            distances.emplace_back(dist, i);
        }
    }
    
    // Sort by distance
    std::sort(distances.begin(), distances.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });
    
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(maxResults, distances.size()); ++i) {
        results.push_back(&episodes_[distances[i].second]);
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveAfterAction(
    ActionType action, size_t maxResults) const {
    
    std::vector<const EpisodicMemoryItem*> results;
    
    for (const auto& ep : episodes_) {
        if (ep.action == action) {
            results.push_back(&ep);
        }
    }
    
    // Most recent first
    std::sort(results.begin(), results.end(),
        [](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
            return a->timestamp > b->timestamp;
        });
    
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

void NeuralEpisodicMemory::replayEpisode(const EpisodicMemoryItem* episode) {
    if (!episode || !brain_) return;
    
    // Reactivate neurons that were active during this episode
    for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
        NeuronId neuron = episode->activeNeurons[i];
        float activation = i < episode->neuronActivations.size() ? 
                          episode->neuronActivations[i] : 0.5f;
        
        // Inject current to reawaken this pattern
        brain_->injectCurrent(neuron, activation * 3.0f);
    }
    
    // Also inject sensory pattern if available
    if (!episode->sensoryState.empty()) {
        // This would go through sensory channels - simplified here
    }
}

void NeuralEpisodicMemory::updateRelevance(SimulationStep episodeId, float relevanceDelta) {
    if (episodeId < episodes_.size()) {
        // In a full implementation, episodes would have a relevance field
        // For now, this would age the episode differently
    }
}

const EpisodicMemoryItem* NeuralEpisodicMemory::getEpisode(size_t index) const {
    if (index < episodes_.size()) {
        return &episodes_[index];
    }
    return nullptr;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    size_t start = count > episodes_.size() ? 0 : episodes_.size() - count;
    for (size_t i = start; i < episodes_.size(); ++i) {
        results.push_back(&episodes_[i]);
    }
    
    return results;
}

float NeuralEpisodicMemory::getAverageReward() const {
    if (episodes_.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& ep : episodes_) {
        sum += ep.reward;
    }
    return sum / episodes_.size();
}

void NeuralEpisodicMemory::consolidate(float relevanceThreshold) {
    // Remove episodes below relevance threshold
    // In a full implementation, episodes would have relevance scores
    // For now, just enforce max capacity
    while (episodes_.size() > maxEpisodes_ * 0.8f) {
        episodes_.erase(episodes_.begin());
    }
}

void NeuralEpisodicMemory::clear() {
    episodes_.clear();
    pImpl->episodeNeurons.clear();
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getEpisodesForReplay(
    size_t count) const {
    
    // Select episodes for replay based on:
    // 1. Recency (more recent episodes)
    // 2. Reward (higher reward episodes)
    // 3. Unpredictability (higher prediction error episodes)
    
    std::vector<std::tuple<float, float, size_t>> scored;  // (score, reward, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float recencyScore = 1.0f / (episodes_[i].age + 1);
        float rewardScore = episodes_[i].reward;
        float score = recencyScore * 0.5f + rewardScore * 0.5f;
        scored.emplace_back(score, rewardScore, i);
    }
    
    // Sort by score
    std::sort(scored.begin(), scored.end(),
        [](const auto& a, const auto& b) {
            return std::get<0>(a) > std::get<0>(b);
        });
    
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(count, scored.size()); ++i) {
        results.push_back(&episodes_[std::get<2>(scored[i])]);
    }
    
    return results;
}

void NeuralEpisodicMemory::replaySequence(const std::vector<size_t>& episodeIds) {
    for (size_t id : episodeIds) {
        if (id < episodes_.size()) {
            replayEpisode(&episodes_[id]);
            
            // Small delay between replays
            // (In simulation, this would be steps)
        }
    }
}

float NeuralEpisodicMemory::computeSimilarity(const std::vector<float>& query,
                                             const EpisodicMemoryItem& episode) const {
    if (query.empty() || episode.sensoryState.empty()) return 0.0f;
    if (query.size() != episode.sensoryState.size()) return 0.0f;
    
    // Cosine similarity
    float dot = 0.0f, normQ = 0.0f, normE = 0.0f;
    for (size_t i = 0; i < query.size(); ++i) {
        dot += query[i] * episode.sensoryState[i];
        normQ += query[i] * query[i];
        normE += episode.sensoryState[i] * episode.sensoryState[i];
    }
    
    if (normQ < 0.0001f || normE < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normQ) * std::sqrt(normE));
}

bool NeuralEpisodicMemory::matchesCriteria(
    const EpisodicMemoryItem& episode,
    const std::vector<float>* sensoryQuery,
    SimulationStep* startTime,
    SimulationStep* endTime,
    ActionType* actionQuery) const {
    
    if (sensoryQuery) {
        if (computeSimilarity(*sensoryQuery, episode) < 0.3f) {
            return false;
        }
    }
    
    if (startTime && endTime) {
        if (episode.timestamp < *startTime || episode.timestamp > *endTime) {
            return false;
        }
    }
    
    if (actionQuery && episode.action != *actionQuery) {
        return false;
    }
    
    return true;
}

// NeuralAssociativeMemory Implementation
struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
{
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                        const std::vector<float>& patternB,
                                        float strength) {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA != INVALID_NEURON_ID && neuronB != INVALID_NEURON_ID) {
        // Check if association exists
        for (auto& assoc : associations_) {
            if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
                (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
                // Update strength
                std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + strength * 0.1f, 0.0f, 1.0f);
                return;
            }
        }
        
        // Create new association
        associations_.emplace_back(neuronA, neuronB, strength);
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!episode.sensoryState.empty() && !episode.resultingSensoryState.empty()) {
        associate(episode.sensoryState, episode.resultingSensoryState, 0.8f);
    }
    
    // Associate action with resulting state
    if (!episode.sensoryState.empty()) {
        // Create action-specific pattern
        std::vector<float> actionPattern(episode.sensoryState.size(), 0.0f);
        actionPattern[0] = static_cast<float>(episode.action) / 10.0f;
        
        associate(actionPattern, episode.resultingSensoryState, 0.7f);
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern,
    size_t maxResults) const {
    
    NeuronId queryNeuron;
    float bestSim = 0.0f;
    
    // Find best matching pattern neuron
    for (size_t i = 0; i < patternNeurons_.size(); ++i) {
        float sim = computeSimilarity(queryPattern, patternNeurons_[i].second);
        if (sim > bestSim) {
            bestSim = sim;
            queryNeuron = patternNeurons_[i].first;
        }
    }
    
    if (bestSim < 0.3f) {
        return {};
    }
    
    // Find associated patterns
    std::vector<std::vector<float>> results;
    std::vector<std::pair<float, std::vector<float>>> scored;
    
    for (const auto& assoc : associations_) {
        NeuronId otherNeuron;
        
        if (std::get<0>(assoc) == queryNeuron) {
            otherNeuron = std::get<1>(assoc);
        } else if (std::get<1>(assoc) == queryNeuron) {
            otherNeuron = std::get<0>(assoc);
        }
        
        if (otherNeuron != INVALID_NEURON_ID) {
            for (const auto& pn : patternNeurons_) {
                if (pn.first == otherNeuron) {
                    float strength = std::get<2>(assoc);
                    scored.emplace_back(strength, pn.second);
                    break;
                }
            }
        }
    }
    
    // Sort by strength
    std::sort(scored.begin(), scored.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    for (size_t i = 0; i < std::min(maxResults, scored.size()); ++i) {
        results.push_back(scored[i].second);
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    
    NeuronId neuronA, neuronB;
    float bestSimA = 0.0f, bestSimB = 0.0f;
    
    for (const auto& pn : patternNeurons_) {
        float simA = computeSimilarity(patternA, pn.second);
        float simB = computeSimilarity(patternB, pn.second);
        
        if (simA > bestSimA) {
            bestSimA = simA;
            neuronA = pn.first;
        }
        if (simB > bestSimB) {
            bestSimB = simB;
            neuronB = pn.first;
        }
    }
    
    if (bestSimA < 0.3f || bestSimB < 0.3f) {
        return 0.0f;
    }
    
    for (const auto& assoc : associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            return std::get<2>(assoc);
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(const std::vector<float>& patternA,
                                                const std::vector<float>& patternB,
                                                float delta) {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    for (auto& assoc : associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            return;
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern,
    size_t steps) const {
    
    std::vector<std::vector<float>> activated;
    std::set<NeuronId> visited;
    
    // Find initial pattern neuron
    NeuronId currentNeuron;
    for (const auto& pn : patternNeurons_) {
        if (computeSimilarity(cuePattern, pn.second) > 0.5f) {
            currentNeuron = pn.first;
            activated.push_back(pn.second);
            visited.insert(currentNeuron);
            break;
        }
    }
    
    if (currentNeuron == INVALID_NEURON_ID) {
        return activated;
    }
    
    // Spread through association network
    for (size_t step = 0; step < steps; ++step) {
        std::vector<NeuronId> nextNeurons;
        
        for (const auto& assoc : associations_) {
            NeuronId otherNeuron = INVALID_NEURON_ID;
            
            if (std::get<0>(assoc) == currentNeuron && visited.find(std::get<1>(assoc)) == visited.end()) {
                otherNeuron = std::get<1>(assoc);
            } else if (std::get<1>(assoc) == currentNeuron && visited.find(std::get<0>(assoc)) == visited.end()) {
                otherNeuron = std::get<0>(assoc);
            }
            
            if (otherNeuron != INVALID_NEURON_ID && std::get<2>(assoc) > 0.3f) {
                nextNeurons.push_back(otherNeuron);
                visited.insert(otherNeuron);
                
                // Find pattern for this neuron
                for (const auto& pn : patternNeurons_) {
                    if (pn.first == otherNeuron) {
                        activated.push_back(pn.second);
                        break;
                    }
                }
            }
        }
        
        if (nextNeurons.empty()) break;
        
        // Pick highest associated next neuron
        currentNeuron = nextNeurons[0];
    }
    
    return activated;
}

void NeuralAssociativeMemory::clear() {
    patternNeurons_.clear();
    associations_.clear();
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    // Check if we already have a similar pattern
    for (const auto& pn : patternNeurons_) {
        if (computeSimilarity(pattern, pn.second) > 0.8f) {
            return pn.first;
        }
    }
    
    // Create new pattern neuron
    NeuronId newId(patternNeurons_.size() + 30000);
    patternNeurons_.push_back({newId, pattern});
    return newId;
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
                                                 const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

} // namespace nlm