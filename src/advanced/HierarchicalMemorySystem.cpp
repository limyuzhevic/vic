#include "HierarchicalMemorySystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <queue>

namespace nlm {

struct HierarchicalMemorySystem::Impl {
    // Memory traces organized by level
    std::vector<std::vector<MemoryTrace>> memoryTraces;
    
    // Current levels
    enum MemoryLevel {
        SHORT_TERM = 0,
        INTERMEDIATE = 1,
        LONG_TERM = 2
    };
    
    // Consolidation queues
    std::deque<size_t> consolidationQueue;
    
    // Transfer learning mappings
    std::unordered_map<std::vector<float>, std::vector<float>> taskMappings;
    
    // Meta-learning parameters
    std::vector<float> metaLearningRates;
    std::vector<float> consolidationWeights;
    
    // Forgetting parameters
    std::vector<float> forgettingRates;
    
    // Integration with brain
    Brain* brain;
    
    // Statistics
    std::vector<size_t> memoryStats;
    
    Impl() : brain(nullptr) {
        memoryTraces.resize(3);
        metaLearningRates = {0.3f, 0.2f, 0.1f};  // Different learning rates per level
        consolidationWeights = {0.5f, 0.3f, 0.2f};
        forgettingRates = {0.1f, 0.05f, 0.01f};  // Faster forgetting at lower levels
        memoryStats.assign(3, 0);
        
        std::random_device rd;
        rng.seed(rd());
    }
    
    std::mt19937 rng;
    
    float computeSimilarity(const std::vector<float>& a, const std::vector<float>& b) const {
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
};

HierarchicalMemorySystem::HierarchicalMemorySystem() : pImpl(std::make_unique<Impl>()) {}
HierarchicalMemorySystem::~HierarchicalMemorySystem() = default;

void HierarchicalMemorySystem::store(const std::vector<float>& pattern, MemoryLevel level) {
    MemoryTrace trace;
    trace.pattern = pattern;
    trace.level = level;
    trace.strength = 1.0f;
    trace.timestamp = 0.0f;  // Would use actual timestamp
    trace.lastAccess = trace.timestamp;
    trace.accessCount = 1;
    trace.transferPotential = 1.0f;  // Initially high transfer potential
    
    pImpl->memoryTraces[level].push_back(trace);
    pImpl->memoryStats[level]++;
    
    NLM_LOG_INFO("Stored memory trace at level " + std::to_string(static_cast<int>(level)) + 
                 " with strength " + std::to_string(trace.strength));
}

std::vector<float> HierarchicalMemorySystem::retrieve(const std::vector<float>& queryPattern, 
                                                       MemoryLevel minLevel) const {
    // Retrieve from all levels >= minLevel, prioritizing higher levels
    for (int level = static_cast<int>(MemoryLevel::LongTerm); level >= static_cast<int>(minLevel); --level) {
        const auto& traces = pImpl->memoryTraces[level];
        for (const auto& trace : traces) {
            float similarity = computeSimilarity(queryPattern, trace.pattern);
            if (similarity > 0.7f) {  // Threshold for retrieval
                return trace.pattern;
            }
        }
    }
    return std::vector<float>();  // No match found
}

void HierarchicalMemorySystem::promoteToNextLevel(size_t traceIndex) {
    // Find trace and promote
    for (int level = static_cast<int>(MemoryLevel::Intermediate); level >= static_cast<int>(MemoryLevel::ShortTerm); --level) {
        if (traceIndex < pImpl->memoryTraces[level].size()) {
            MemoryTrace& trace = pImpl->memoryTraces[level][traceIndex];
            
            if (level < static_cast<int>(MemoryLevel::LongTerm)) {
                // Move to next level
                MemoryTrace promotedTrace = trace;
                promotedTrace.level = static_cast<MemoryLevel>(level + 1);
                promotedTrace.strength *= 0.8f;  // Strength decay during transfer
                
                pImpl->memoryTraces[level + 1].push_back(promotedTrace);
                pImpl->memoryStats[level + 1]++;
                
                // Remove from current level
                pImpl->memoryTraces[level].erase(pImpl->memoryTraces[level].begin() + traceIndex);
                pImpl->memoryStats[level]--;
                
                NLM_LOG_INFO("Promoted memory trace to level " + 
                             std::to_string(static_cast<int>(promotedTrace.level)));
                return;
            }
        }
    }
}

void HierarchicalMemorySystem::consolidateToLongTerm(MemoryLevel sourceLevel, 
                                                   ConsolidationPolicy policy) {
    float consolidationThreshold = 0.6f;
    
    switch (policy) {
        case ConsolidationPolicy::TimeBased:
            // Time-based consolidation
            for (auto& trace : pImpl->memoryTraces[sourceLevel]) {
                float age = trace.timestamp;  // Would use actual age
                if (age > 100.0f) {  // Threshold age
                    promoteToNextLevel(pImpl->memoryStats[sourceLevel]);  // Simplified
                }
            }
            break;
            
        case ConsolidationPolicy::StrengthBased:
            // Strength-based consolidation
            for (auto& trace : pImpl->memoryTraces[sourceLevel]) {
                if (trace.strength > consolidationThreshold) {
                    promoteToNextLevel(pImpl->memoryStats[sourceLevel]);  // Simplified
                }
            }
            break;
            
        case ConsolidationPolicy::RewardBased:
            // Reward-based consolidation (simplified)
            for (auto& trace : pImpl->memoryTraces[sourceLevel]) {
                if (trace.strength > 0.8f) {  // High reward association
                    promoteToNextLevel(pImpl->memoryStats[sourceLevel]);  // Simplified
                }
            }
            break;
            
        case ConsolidationPolicy::MetaLearning:
            // Meta-learning about consolidation
            adaptConsolidationPolicy();
            for (auto& trace : pImpl->memoryTraces[sourceLevel]) {
                if (trace.strength > consolidationThreshold * pImpl->metaLearningRates[sourceLevel]) {
                    promoteToNextLevel(pImpl->memoryStats[sourceLevel]);  // Simplified
                }
            }
            break;
    }
}

void HierarchicalMemorySystem::forget(ForgettingPolicy policy, float threshold) {
    switch (policy) {
        case ForgettingPolicy::ExponentialDecay:
            for (int level = 0; level < 3; ++level) {
                for (auto it = pImpl->memoryTraces[level].begin(); 
                     it != pImpl->memoryTraces[level].end(); ) {
                    it->strength *= std::exp(-pImpl->forgettingRates[level]);
                    if (it->strength < threshold) {
                        it = pImpl->memoryTraces[level].erase(it);
                        pImpl->memoryStats[level]--;
                    } else {
                        ++it;
                    }
                }
            }
            break;
            
        case ForgettingPolicy::LinearDecay:
            for (int level = 0; level < 3; ++level) {
                for (auto it = pImpl->memoryTraces[level].begin(); 
                     it != pImpl->memoryTraces[level].end(); ) {
                    it->strength -= pImpl->forgettingRates[level];
                    if (it->strength < threshold) {
                        it = pImpl->memoryTraces[level].erase(it);
                        pImpl->memoryStats[level]--;
                    } else {
                        ++it;
                    }
                }
            }
            break;
            
        case ForgettingPolicy::PriorityBased:
            // Keep strongest memories
            for (int level = 0; level < 3; ++level) {
                std::sort(pImpl->memoryTraces[level].begin(), pImpl->memoryTraces[level].end(),
                         [](const MemoryTrace& a, const MemoryTrace& b) {
                             return a.strength > b.strength;
                         });
                size_t keepCount = static_cast<size_t>(pImpl->memoryTraces[level].size() * 0.8f);
                pImpl->memoryTraces[level].resize(keepCount);
                pImpl->memoryStats[level] = keepCount;
            }
            break;
            
        case ForgettingPolicy::MetaplasticDecay:
            // Meta-plasticity affects decay rate
            updateForgettingRates();
            for (int level = 0; level < 3; ++level) {
                for (auto it = pImpl->memoryTraces[level].begin(); 
                     it != pImpl->memoryTraces[level].end(); ) {
                    it->strength *= std::exp(-pImpl->forgettingRates[level]);
                    if (it->strength < threshold) {
                        it = pImpl->memoryTraces[level].erase(it);
                        pImpl->memoryStats[level]--;
                    } else {
                        ++it;
                    }
                }
            }
            break;
    }
}

void HierarchicalMemorySystem::updateForgettingRates() {
    // Meta-plasticity adapts forgetting rates based on learning success
    float totalStrength = 0.0f;
    for (int level = 0; level < 3; ++level) {
        for (const auto& trace : pImpl->memoryTraces[level]) {
            totalStrength += trace.strength;
        }
    }
    
    if (totalStrength > 0.0f) {
        // Increase forgetting rate if total memory strength is too high
        float targetStrength = 100.0f;  // Target total strength
        float currentStrength = totalStrength;
        float adjustment = (currentStrength - targetStrength) * 0.01f;
        
        for (int level = 0; level < 3; ++level) {
            pImpl->forgettingRates[level] = std::max(0.001f, pImpl->forgettingRates[level] + adjustment);
        }
    }
}

void HierarchicalMemorySystem::transferToTask(const std::vector<float>& pattern, 
                                           const std::vector<float>& taskContext) {
    // Find similar memories and create task-specific versions
    for (int level = 0; level < 3; ++level) {
        for (const auto& trace : pImpl->memoryTraces[level]) {
            float similarity = computeSimilarity(pattern, trace.pattern);
            if (similarity > 0.8f) {
                // Transfer this memory to new task context
                MemoryTrace transferredTrace = trace;
                transferredTrace.taskContext = taskContext;
                transferredTrace.transferPotential = similarity;
                
                pImpl->taskMappings[taskContext].push_back(transferredTrace.pattern);
                NLM_LOG_INFO("Transferred memory to task context with potential: " + 
                             std::to_string(similarity));
            }
        }
    }
}

std::vector<float> HierarchicalMemorySystem::retrieveForTask(const std::vector<float>& taskContext) const {
    auto it = pImpl->taskMappings.find(taskContext);
    if (it != pImpl->taskMappings.end() && !it->second.empty()) {
        return it->second[0];  // Return first matching memory
    }
    return std::vector<float>();
}

void HierarchicalMemorySystem::updateMetaLearningParameters(const std::vector<float>& predictionErrors) {
    // Update meta-learning rates based on prediction errors
    for (size_t i = 0; i < predictionErrors.size() && i < pImpl->metaLearningRates.size(); ++i) {
        pImpl->metaLearningRates[i] *= (1.0f - std::abs(predictionErrors[i]));
        pImpl->metaLearningRates[i] = std::clamp(pImpl->metaLearningRates[i], 0.05f, 1.0f);
    }
}

float HierarchicalMemorySystem::getMetaLearningRate(MemoryLevel level) const {
    if (static_cast<size_t>(level) < pImpl->metaLearningRates.size()) {
        return pImpl->metaLearningRates[level];
    }
    return 0.1f;
}

void HierarchicalMemorySystem::adaptConsolidationPolicy() {
    // Meta-learning about optimal consolidation strategy
    float avgStrength = 0.0f;
    size_t count = 0;
    for (int level = 0; level < 3; ++level) {
        for (const auto& trace : pImpl->memoryTraces[level]) {
            avgStrength += trace.strength;
            ++count;
        }
    }
    
    if (count > 0) {
        avgStrength /= count;
        
        // Adjust consolidation weights based on average strength
        float targetStrength = 50.0f;
        float adjustment = (avgStrength - targetStrength) * 0.01f;
        
        for (int level = 0; level < 3; ++level) {
            pImpl->consolidationWeights[level] = std::clamp(pImpl->consolidationWeights[level] + adjustment, 0.1f, 1.0f);
        }
    }
}

size_t HierarchicalMemorySystem::getMemoryTraceCount(MemoryLevel level) const {
    if (static_cast<size_t>(level) < pImpl->memoryStats.size()) {
        return pImpl->memoryStats[level];
    }
    return 0;
}

float HierarchicalMemorySystem::getTotalMemoryStrength(MemoryLevel level) const {
    if (static_cast<size_t>(level) >= pImpl->memoryTraces.size()) {
        return 0.0f;
    }
    
    float total = 0.0f;
    for (const auto& trace : pImpl->memoryTraces[level]) {
        total += trace.strength;
    }
    return total;
}

float HierarchicalMemorySystem::getRetentionRate(MemoryLevel level) const {
    // Estimate retention rate based on memory age and strength
    if (static_cast<size_t>(level) >= pImpl->memoryTraces.size()) {
        return 0.0f;
    }
    
    float totalRetention = 0.0f;
    for (const auto& trace : pImpl->memoryTraces[level]) {
        totalRetention += trace.strength * (1.0f - pImpl->forgettingRates[level]);
    }
    
    float totalMemory = getTotalMemoryStrength(level);
    return totalMemory > 0.0f ? totalRetention / totalMemory : 0.0f;
}

void HierarchicalMemorySystem::setBrain(Brain* brain) {
    pImpl->brain = brain;
}

void HierarchicalMemorySystem::step(float timestep) {
    // Update memory traces
    updateForgettingRates();
    
    // Periodically consolidate memories
    static float timeSinceConsolidation = 0.0f;
    timeSinceConsolidation += timestep;
    
    if (timeSinceConsolidation > 1000.0f) {  // Every 1000 time units
        consolidateToLongTerm(MemoryLevel::ShortTerm, ConsolidationPolicy::MetaLearning);
        timeSinceConsolidation = 0.0f;
    }
}

} // namespace nlm
