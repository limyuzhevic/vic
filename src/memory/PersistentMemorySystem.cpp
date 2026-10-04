#include "PersistentMemorySystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../prediction/PredictionSystem.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <fstream>
#include <sstream>
#include <unordered_set>

namespace nlm {

struct PersistentMemorySystem::Impl {
    std::mt19937 rng;
    Brain* brain;
    
    // Memory statistics tracking
    std::unordered_map<SimulationStep, size_t> accessFrequency;
    std::unordered_map<SimulationStep, float> lastAccessTime;
    
    // Integration state
    std::vector<std::pair<SimulationStep, SimulationStep>> traceIntegrations;
    
    // Statistics
    size_t totalTraceCreations;
    size_t successfulConsolidations;
    size_t failedConsolidations;
    float totalIntegrationStrength;
    float averageTraceAge;
};

PersistentMemorySystem::PersistentMemorySystem()
    : pImpl(std::make_unique<Impl>())
    , brain_(nullptr)
    , persistenceEnabled_(true)
    , consolidationInterval_(1000.0f)  // Consolidate every 1000 simulation steps
    , maxMemoryTraces_(10000)
    , integrationStrength_(0.5f)
    , totalConsolidations_(0)
    , successfulIntegrations_(0)
    , memoryEvictions_(0)
    , totalMemoryStrength_(0.0f)
{
    // Initialize random number generator
    std::random_device rd;
    pImpl->rng.seed(rd());
    
    pImpl->totalTraceCreations = 0;
    pImpl->successfulConsolidations = 0;
    pImpl->failedConsolidations = 0;
    pImpl->totalIntegrationStrength = 0.0f;
    pImpl->averageTraceAge = 0.0f;
}

PersistentMemorySystem::~PersistentMemorySystem() = default;

void PersistentMemorySystem::initialize(Brain* brain) {
    brain_ = brain;
    pImpl->brain = brain;
    
    // Reset memory traces
    memoryTraces_.clear();
    consolidationEvents_.clear();
    pImpl->accessFrequency.clear();
    pImpl->lastAccessTime.clear();
    pImpl->traceIntegrations.clear();
    
    NLM_LOG_INFO("PersistentMemorySystem initialized");
}

void PersistentMemorySystem::update(TimestepDuration dt) {
    // Apply memory decay over time
    applyDecay(dt);
    
    // Schedule consolidation events based on time
    static float timeAccumulator = 0.0f;
    timeAccumulator += dt;
    
    if (timeAccumulator >= consolidationInterval_) {
        scheduleConsolidationEvents();
        timeAccumulator = 0.0f;
    }
}

void PersistentMemorySystem::createMemoryTrace(const std::vector<float>& activationPattern,
                                              const std::string& semanticContent) {
    if (!brain_ || memoryTraces_.size() >= maxMemoryTraces_) {
        return;
    }
    
    LongTermMemoryTrace trace;
    trace.creationTime = brain_ ? brain_->getCurrentStep() : 0;
    trace.lastAccessTime = trace.creationTime;
    trace.lastConsolidationTime = trace.creationTime;
    trace.stabilityScore = 0.5f + (pImpl->rng() / static_cast<float>(pImpl->rng.max())) * 0.5f;
    trace.strength = 1.0f;
    trace.associatedNeurons.clear();
    trace.activationPattern = activationPattern;
    trace.semanticContent = semanticContent;
    
    // Get associated neurons from pattern (simplified)
    if (!activationPattern.empty()) {
        trace.associatedNeurons.resize(std::min<size_t>(10, activationPattern.size()));
        for (size_t i = 0; i < trace.associatedNeurons.size(); ++i) {
            trace.associatedNeurons[i] = NeuronId(static_cast<uint64_t>(i + 1000));
        }
    }
    
    memoryTraces_.push_back(trace);
    pImpl->totalTraceCreations++;
    
    NLM_LOG_INFO("Created long-term memory trace with " + std::to_string(activationPattern.size()) + 
                 " activation values, strength: " + std::to_string(trace.strength));
}

LongTermMemoryTrace* PersistentMemorySystem::retrieveMemoryTrace(SimulationStep episodeId) {
    for (auto& trace : memoryTraces_) {
        // In a real implementation, would match based on episodeId
        // For now, use simple pattern matching based on age
        if (std::abs(trace.creationTime - episodeId) < 10) {
            trace.lastAccessTime = brain_ ? brain_->getCurrentStep() : 0;
            pImpl->lastAccessTime[episodeId]++;
            
            return &trace;
        }
    }
    return nullptr;
}

void PersistentMemorySystem::updateMemoryTrace(SimulationStep episodeId,
                                             const std::vector<float>& newActivationPattern) {
    for (auto& trace : memoryTraces_) {
        if (std::abs(trace.creationTime - episodeId) < 10) {
            // Update activation pattern with weighted average
            trace.lastAccessTime = brain_ ? brain_->getCurrentStep() : 0;
            
            // Blend old and new patterns
            if (!newActivationPattern.empty()) {
                float blendFactor = 0.3f;
                trace.activationPattern.resize(std::max(trace.activationPattern.size(), newActivationPattern.size()));
                
                for (size_t i = 0; i < trace.activationPattern.size(); ++i) {
                    float oldVal = (i < trace.activationPattern.size()) ? trace.activationPattern[i] : 0.0f;
                    float newVal = (i < newActivationPattern.size()) ? newActivationPattern[i] : 0.0f;
                    trace.activationPattern[i] = (1.0f - blendFactor) * oldVal + blendFactor * newVal;
                }
            }
            
            // Update strength based on access frequency
            pImpl->accessFrequency[episodeId]++;
            trace.strength = std::min(2.0f, trace.strength * 1.01f);
            
            break;
        }
    }
}

void PersistentMemorySystem::consolidateMemories(const std::vector<MemoryPrioritizationInfo>& priorities) {
    if (priorities.empty() || !brain_) {
        return;
    }
    
    totalConsolidations_++;
    
    // Sort priorities by consolidation need
    std::vector<MemoryPrioritizationInfo> sortedPriorities = priorities;
    std::sort(sortedPriorities.begin(), sortedPriorities.end(),
              [](const MemoryPrioritizationInfo& a, const MemoryPrioritizationInfo& b) {
                  return a.consolidationNeed > b.consolidationNeed;
              });
    
    // Consolidate top N memories
    size_t consolidateCount = std::min<size_t>(5, sortedPriorities.size());
    
    for (size_t i = 0; i < consolidateCount; ++i) {
        const auto& priority = sortedPriorities[i];
        
        // Find or create source trace
        LongTermMemoryTrace* sourceTrace = retrieveMemoryTrace(priority.episodeId);
        if (!sourceTrace) {
            // Create a new trace for this episode
            if (brain_->getEpisodicMemory()) {
                auto* episode = brain_->getEpisodicMemory()->getEpisode(priority.episodeId);
                if (episode) {
                    generateMemoryTraceFromEpisode(*episode);
                    sourceTrace = retrieveMemoryTrace(episode->timestamp);
                }
            }
        }
        
        if (sourceTrace) {
            // Create consolidation event
            MemoryConsolidationEvent event;
            event.timestamp = brain_ ? brain_->getCurrentStep() : 0;
            event.sourceEpisodeId = priority.episodeId;
            event.sourceStrength = sourceTrace->strength;
            event.targetTraceStrength = 0.0f;  // Would find target in real implementation
            event.integrationStrength = priority.consolidationNeed * 0.2f;
            event.temporalCompression = 1.0f / (1.0f + priority.temporalRecency);
            event.weightChange = priority.consolidationNeed * 0.01f;
            
            // Update trace stability
            sourceTrace->lastConsolidationTime = event.timestamp;
            sourceTrace->stabilityScore = (sourceTrace->stabilityScore * 0.7f) + (priority.consolidationNeed * 0.3f);
            
            // Update source trace strength based on consolidation
            sourceTrace->strength = std::min(2.0f, sourceTrace->strength + event.weightChange);
            
            // Add to consolidation events
            consolidationEvents_.push_back(event);
            
            pImpl->successfulConsolidations++;
            pImpl->totalIntegrationStrength += event.integrationStrength;
            
            NLM_LOG_INFO("Consolidated memory trace " + std::to_string(priority.episodeId) + 
                         " with strength " + std::to_string(event.sourceStrength) + 
                         " -> new strength " + std::to_string(sourceTrace->strength));
        }
    }
}

void PersistentMemorySystem::integrateMemories(const MemoryIntegrationWeights& weights) {
    if (memoryTraces_.size() < 2) {
        return;
    }
    
    // Calculate integration weights based on current state
    MemoryIntegrationWeights integrationWeights = weights;
    calculateIntegrationWeights(integrationWeights);
    
    // Find similar memory traces for integration
    for (size_t i = 0; i < memoryTraces_.size(); ++i) {
        for (size_t j = i + 1; j < memoryTraces_.size(); ++j) {
            // Check if traces are similar (simplified)
            float similarity = calculateTraceSimilarity(memoryTraces_[i], memoryTraces_[j]);
            
            if (similarity > 0.3f) {
                // Integrate traces
                updateTraceIntegration(memoryTraces_[i], memoryTraces_[j]);
                
                // Record integration
                pImpl->traceIntegrations.push_back(std::make_pair(memoryTraces_[i].creationTime, 
                                                                 memoryTraces_[j].creationTime));
                
                successfulIntegrations_++;
                integrationWeights.crossSystemConsolidation *= 1.1f;  // Boost future integration
                
                NLM_LOG_INFO("Integrated memory traces " + std::to_string(memoryTraces_[i].creationTime) + 
                             " and " + std::to_string(memoryTraces_[j].creationTime) + 
                             " with similarity " + std::to_string(similarity));
            }
        }
    }
}

void PersistentMemorySystem::applyDecay(float timeDelta) {
    if (timeDelta <= 0.0f) {
        return;
    }
    
    float totalStrength = 0.0f;
    
    for (auto& trace : memoryTraces_) {
        // Calculate decay based on decay mechanism
        float effectiveDecayRate = decayParams_.baseDecayRate;
        
        switch (decayParams_.type) {
            case MemoryDecayMechanism::DecayType::Exponential:
                effectiveDecayRate *= timeDelta;
                break;
            case MemoryDecayMechanism::DecayType::PowerLaw:
                effectiveDecayRate *= std::pow(timeDelta, 0.5f);
                break;
            case MemoryDecayMechanism::DecayType::Logistic:
                effectiveDecayRate *= (1.0f / (1.0f + std::exp(-timeDelta + 5.0f)));
                break;
            case MemoryDecayMechanism::DecayType::Adaptive:
                effectiveDecayRate *= decayParams_.adaptationFactor * (1.0f - trace.strength / 2.0f);
                break;
        }
        
        // Apply strength-dependent factor
        if (decayParams_.strengthDependentFactor > 0.0f) {
            effectiveDecayRate *= (1.0f - decayParams_.strengthDependentFactor * (trace.strength - 0.5f));
        }
        
        // Apply consolidation protection
        float protection = decayParams_.consolidationProtection * trace.stabilityScore;
        effectiveDecayRate *= (1.0f - protection);
        
        // Update strength
        trace.strength *= std::max(0.0f, 1.0f - effectiveDecayRate);
        
        // Update activation pattern
        for (auto& val : trace.activationPattern) {
            val *= (1.0f - effectiveDecayRate * 0.1f);
        }
        
        // Update age
        trace.lastAccessTime += timeDelta;
        
        totalStrength += trace.strength;
    }
    
    totalMemoryStrength_ = totalStrength;
    
    // Perform trace pruning if we have too many traces
    performTracePruning();
    
    // Apply adaptive decay if needed
    if (decayParams_.type == MemoryDecayMechanism::DecayType::Adaptive) {
        applyAdaptiveDecay(timeDelta);
    }
    
    // Update average trace age
    float totalAge = 0.0f;
    for (const auto& trace : memoryTraces_) {
        totalAge += trace.lastAccessTime;
    }
    
    if (!memoryTraces_.empty()) {
        pImpl->averageTraceAge = totalAge / memoryTraces_.size();
    }
    
    NLM_LOG_INFO("Applied decay to " + std::to_string(memoryTraces_.size()) + " traces, " + 
                 "average strength: " + std::to_string(totalStrength / memoryTraces_.size()));
}

std::vector<MemoryPrioritizationInfo> PersistentMemorySystem::prioritizeMemories() const {
    std::vector<MemoryPrioritizationInfo> priorities;
    
    // Get access frequency for prioritization
    std::unordered_map<SimulationStep, size_t> accessFreq = pImpl->accessFrequency;
    
    for (const auto& trace : memoryTraces_) {
        MemoryPrioritizationInfo priority;
        priority.episodeId = trace.creationTime;
        priority.replayFrequency = 1.0f / (1.0f + trace.lastAccessTime * 0.01f);  // More recent = higher priority
        priority.noveltyValue = 1.0f - trace.strength;  // Weaker memories = more novel
        priority.rewardValue = trace.rewardAssociation;  // Would need to track this
        priority.predictionError = trace.predictionError;
        priority.temporalRecency = 1.0f / (1.0f + (brain_ ? brain_->getCurrentStep() - trace.creationTime : 1.0f));
        priority.emotionalValence = 1.0f;  // Would need to track this
        priority.contextualRelevance = trace.strength * 0.5f;  // Stronger memories = more relevant
        priority.integrationPotential = trace.stabilityScore * 0.5f;  // Stable memories = good for integration
        
        // Calculate consolidation need (weighted sum)
        priority.consolidationNeed = 
            priority.replayFrequency * 0.15f +
            priority.noveltyValue * 0.15f +
            priority.rewardValue * 0.15f +
            priority.predictionError * 0.15f +
            priority.temporalRecency * 0.10f +
            priority.emotionalValence * 0.10f +
            priority.contextualRelevance * 0.10f +
            priority.integrationPotential * 0.10f;
        
        priorities.push_back(priority);
    }
    
    // Sort by consolidation need
    std::sort(priorities.begin(), priorities.end(),
              [](const MemoryPrioritizationInfo& a, const MemoryPrioritizationInfo& b) {
                  return a.consolidationNeed > b.consolidationNeed;
              });
    
    return priorities;
}

void PersistentMemorySystem::generateMemoryTraceFromEpisode(const EpisodicMemoryItem& episode) {
    LongTermMemoryTrace trace;
    trace.creationTime = episode.timestamp;
    trace.lastAccessTime = trace.creationTime;
    trace.lastConsolidationTime = trace.creationTime;
    trace.stabilityScore = 0.5f + (pImpl->rng() / static_cast<float>(pImpl->rng.max())) * 0.5f;
    trace.strength = 1.0f;
    trace.decayRate = decayParams_.baseDecayRate;
    
    // Copy episode data
    trace.associatedNeurons = episode.activeNeurons;
    trace.activationPattern = episode.neuronActivations;
    
    // Set additional fields based on episode
    trace.predictionError = episode.novelty;
    trace.noveltyScore = episode.novelty;
    trace.rewardAssociation = episode.reward;
    
    // Create semantic content from episode (simplified)
    std::stringstream ss;
    ss << "Episode_" << episode.timestamp << 
          "_Action_" << static_cast<int>(episode.action) << 
          "_Reward_" << episode.reward;
    trace.semanticContent = ss.str();
    
    memoryTraces_.push_back(trace);
    pImpl->totalTraceCreations++;
}

void PersistentMemorySystem::updateTraceStrength(LongTermMemoryTrace& trace, float timeDelta) {
    // Update trace strength based on decay and access
    float strengthDecay = trace.decayRate * timeDelta;
    trace.strength *= std::max(0.0f, 1.0f - strengthDecay);
    
    // Update stability based on recent use
    float accessRecency = 1.0f / (1.0f + (brain_ ? brain_->getCurrentStep() - trace.lastAccessTime : 1.0f));
    trace.stabilityScore = (trace.stabilityScore * 0.9f) + (accessRecency * 0.1f);
}

void PersistentMemorySystem::performTracePruning() {
    if (memoryTraces_.size() <= maxMemoryTraces_) {
        return;
    }
    
    // Sort traces by strength (weaker traces first)
    std::sort(memoryTraces_.begin(), memoryTraces_.end(),
              [](const LongTermMemoryTrace& a, const LongTermMemoryTrace& b) {
                  return a.strength < b.strength;
              });
    
    // Remove weakest traces
    size_t removeCount = memoryTraces_.size() - maxMemoryTraces_;
    memoryTraces_.erase(memoryTraces_.begin(), memoryTraces_.begin() + removeCount);
    
    memoryEvictions_ += removeCount;
    
    NLM_LOG_INFO("Pruned " + std::to_string(removeCount) + " weak memory traces, " + 
                 "keeping " + std::to_string(memoryTraces_.size()) + " strongest traces");
}

void PersistentMemorySystem::scheduleConsolidationEvents() {
    // Get prioritized memories
    auto priorities = prioritizeMemories();
    
    if (priorities.empty()) {
        return;
    }
    
    // Consolidate top priority memories
    consolidateMemories(priorities);
}

void PersistentMemorySystem::updateTraceIntegration(LongTermMemoryTrace& sourceTrace, LongTermMemoryTrace& targetTrace) {
    // Integrate source trace into target trace
    float integrationWeight = 0.3f;  // Integration weight
    
    // Blend activation patterns
    if (!sourceTrace.activationPattern.empty() && !targetTrace.activationPattern.empty()) {
        size_t minSize = std::min(sourceTrace.activationPattern.size(), targetTrace.activationPattern.size());
        
        for (size_t i = 0; i < minSize; ++i) {
            float sourceVal = sourceTrace.activationPattern[i];
            float targetVal = targetTrace.activationPattern[i];
            float blendedVal = (1.0f - integrationWeight) * targetVal + integrationWeight * sourceVal;
            targetTrace.activationPattern[i] = blendedVal;
        }
    }
    
    // Update target trace strength
    targetTrace.strength = (1.0f - integrationWeight) * targetTrace.strength + integrationWeight * sourceTrace.strength;
    
    // Update target trace stability
    targetTrace.stabilityScore = (1.0f - integrationWeight) * targetTrace.stabilityScore + integrationWeight * sourceTrace.stabilityScore;
    
    // Add source episode to target's related episodes
    targetTrace.relatedEpisodes.push_back(sourceTrace.creationTime);
    
    // Reduce source trace strength (as it's been integrated)
    sourceTrace.strength *= 0.5f;
}

void PersistentMemorySystem::calculateIntegrationWeights(MemoryIntegrationWeights& weights) const {
    // Adjust integration weights based on current system state
    if (brain_) {
        float neuralActivity = static_cast<float>(brain_->getActiveNeuronCount()) / 
                               std::max<float>(1.0f, brain_->getTotalNeuronCount());
        
        // Adjust weights based on neural activity
        weights.workingEpisodicIntegration *= (1.0f + neuralActivity * 0.5f);
        weights.crossSystemConsolidation *= (1.0f + neuralActivity * 0.3f);
    }
    
    // Ensure weights are within reasonable range
    weights.workingEpisodicIntegration = std::clamp(weights.workingEpisodicIntegration, 0.1f, 1.0f);
    weights.episodicLongTermIntegration = std::clamp(weights.episodicLongTermIntegration, 0.1f, 1.0f);
    weights.crossSystemConsolidation = std::clamp(weights.crossSystemConsolidation, 0.1f, 1.0f);
}

void PersistentMemorySystem::applyAdaptiveDecay(float timeDelta) {
    // Apply adaptive decay that adjusts based on memory strength
    float systemHealth = 1.0f - (pImpl->totalIntegrationStrength / std::max<float>(1.0f, pImpl->successfulConsolidations));
    
    // Adjust decay rate based on system health
    decayParams_.adaptationFactor = 1.0f + systemHealth * 0.5f;
}

void PersistentMemorySystem::consolidateToSemanticMemory(const std::vector<LongTermMemoryTrace>& traces) {
    // This would consolidate traces to semantic memory system
    // For now, it's a placeholder
}

LongTermMemoryTrace* PersistentMemorySystem::findSimilarTrace(const std::vector<float>& pattern) const {
    if (pattern.empty()) {
        return nullptr;
    }
    
    float bestSimilarity = 0.0f;
    LongTermMemoryTrace* bestMatch = nullptr;
    
    for (auto& trace : memoryTraces_) {
        if (trace.activationPattern.empty()) {
            continue;
        }
        
        // Calculate cosine similarity between patterns
        float similarity = 0.0f;
        size_t minSize = std::min(pattern.size(), trace.activationPattern.size());
        
        for (size_t i = 0; i < minSize; ++i) {
            similarity += pattern[i] * trace.activationPattern[i];
        }
        
        float normA = 0.0f, normB = 0.0f;
        for (size_t i = 0; i < minSize; ++i) {
            normA += pattern[i] * pattern[i];
            normB += trace.activationPattern[i] * trace.activationPattern[i];
        }
        
        if (normA > 0.0f && normB > 0.0f) {
            similarity /= std::sqrt(normA * normB);
        }
        
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestMatch = const_cast<LongTermMemoryTrace*>(&trace);
        }
    }
    
    return bestMatch;
}

float PersistentMemorySystem::getAverageMemoryStrength() const {
    if (memoryTraces_.empty()) {
        return 0.0f;
    }
    
    float totalStrength = 0.0f;
    for (const auto& trace : memoryTraces_) {
        totalStrength += trace.strength;
    }
    
    return totalStrength / memoryTraces_.size();
}

float PersistentMemorySystem::getAverageMemoryStability() const {
    if (memoryTraces_.empty()) {
        return 0.0f;
    }
    
    float totalStability = 0.0f;
    for (const auto& trace : memoryTraces_) {
        totalStability += trace.stabilityScore;
    }
    
    return totalStability / memoryTraces_.size();
}

bool PersistentMemorySystem::saveMemoryTraces(const std::string& filepath) const {
    std::ofstream file(filepath, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Failed to open file for saving memory traces: " + filepath);
        return false;
    }
    
    try {
        // Write number of traces
        size_t traceCount = memoryTraces_.size();
        file.write(reinterpret_cast<const char*>(&traceCount), sizeof(traceCount));
        
        // Write each trace
        for (const auto& trace : memoryTraces_) {
            // Write trace data (simplified - would serialize fully in real implementation)
            file.write(reinterpret_cast<const char*>(&trace.creationTime), sizeof(trace.creationTime));
            file.write(reinterpret_cast<const char*>(&trace.lastAccessTime), sizeof(trace.lastAccessTime));
            file.write(reinterpret_cast<const char*>(&trace.strength), sizeof(trace.strength));
            
            // Write semantic content length and content
            size_t contentLength = trace.semanticContent.size();
            file.write(reinterpret_cast<const char*>(&contentLength), sizeof(contentLength));
            file.write(trace.semanticContent.c_str(), contentLength);
        }
        
        file.close();
        NLM_LOG_INFO("Saved " + std::to_string(traceCount) + " memory traces to " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception saving memory traces: " + std::string(e.what()));
        return false;
    }
}

bool PersistentMemorySystem::loadMemoryTraces(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Failed to open file for loading memory traces: " + filepath);
        return false;
    }
    
    try {
        // Clear existing traces
        memoryTraces_.clear();
        
        // Read number of traces
        size_t traceCount = 0;
        file.read(reinterpret_cast<char*>(&traceCount), sizeof(traceCount));
        
        memoryTraces_.reserve(traceCount);
        
        // Read each trace
        for (size_t i = 0; i < traceCount; ++i) {
            LongTermMemoryTrace trace;
            
            // Read trace data
            file.read(reinterpret_cast<char*>(&trace.creationTime), sizeof(trace.creationTime));
            file.read(reinterpret_cast<char*>(&trace.lastAccessTime), sizeof(trace.lastAccessTime));
            file.read(reinterpret_cast<char*>(&trace.strength), sizeof(trace.strength));
            
            // Read semantic content
            size_t contentLength = 0;
            file.read(reinterpret_cast<char*>(&contentLength), sizeof(contentLength));
            
            std::string content(contentLength, '\0');
            if (contentLength > 0) {
                file.read(&content[0], contentLength);
            }
            trace.semanticContent = content;
            
            memoryTraces_.push_back(trace);
        }
        
        file.close();
        NLM_LOG_INFO("Loaded " + std::to_string(memoryTraces_.size()) + " memory traces from " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading memory traces: " + std::string(e.what()));
        return false;
    }
}

void PersistentMemorySystem::reset() {
    memoryTraces_.clear();
    consolidationEvents_.clear();
    pImpl->accessFrequency.clear();
    pImpl->lastAccessTime.clear();
    pImpl->traceIntegrations.clear();
    
    NLM_LOG_INFO("PersistentMemorySystem reset");
}

} // namespace nlm
