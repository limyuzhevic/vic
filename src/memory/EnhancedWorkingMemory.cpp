#include "EnhancedWorkingMemory.hpp"
#include "WorkingMemoryPriorityQueue.hpp"
#include "WorkingMemoryPatternMatcher.hpp"
#include "WorkingMemoryConsolidation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>

namespace nlm {

struct EnhancedWorkingMemory::Impl {
    Brain* brain;
    WorkingMemoryPriorityQueue* priorityQueue;
    WorkingMemoryPatternMatcher* patternMatcher;
    WorkingMemoryConsolidation* consolidationSystem;
    
    Impl() : brain(nullptr), priorityQueue(nullptr), patternMatcher(nullptr), consolidationSystem(nullptr) {}
};

EnhancedWorkingMemory::EnhancedWorkingMemory()
    : brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
    , replayCount_(0)
    , priorityQueue_(nullptr)
    , patternMatcher_(nullptr)
    , consolidationSystem_(nullptr)
{
}

EnhancedWorkingMemory::~EnhancedWorkingMemory() {
    // Cleanup is handled by smart pointers in implementations
}

void EnhancedWorkingMemory::initialize(Brain* brain) {
    brain_ = brain;
    
    // Initialize enhanced components
    priorityQueue_ = new WorkingMemoryPriorityQueue();
    patternMatcher_ = new WorkingMemoryPatternMatcher();
    consolidationSystem_ = new WorkingMemoryConsolidation();
    
    NLM_LOG_INFO("EnhancedWorkingMemory initialized with advanced neural dynamics");
}

void EnhancedWorkingMemory::store(const std::vector<float>& pattern, float strength, bool isTransient) {
    if (pattern.empty() || !brain_ || !priorityQueue_ || !patternMatcher_) return;
    
    // Profile the operation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Create memory traces for each neuron in the pattern
    for (size_t i = 0; i < pattern.size() && i < capacity_; ++i) {
        // Create neuron ID from pattern index
        NeuronId neuron(static_cast<uint64_t>(i) + 10000);
        float activation = pattern[i] * strength;
        SimulationStep timestamp = i;
        
        // Create enhanced memory trace
        EnhancedMemoryTrace trace(neuron, activation, timestamp);
        trace.isTransient = isTransient;
        trace.isConsolidated = false;
        trace.stability = 1.0f;
        
        // Add to priority queue
        priorityQueue_->push(trace);
        
        // Update neural activations
        brain_->injectCurrent(neuron, activation * 5.0f);
        
        // Cache pattern for efficient retrieval
        if (patternMatcher_) {
            patternMatcher_->cachePattern(pattern);
        }
        
        // Update hash maps for O(1) lookup
        neuronActivations_[neuron.value] = activation;
        neuronTimestamps_[neuron.value] = timestamp;
        neuronImportances_[neuron.value] = 1.0f;
        neuronLevels_[neuron.value] = isTransient ? MemoryLevel::WORKING : MemoryLevel::EPISODIC;
        
        // Add to ordered set for efficient iteration
        orderedNeurons_.insert(neuron);
    }
    
    // Store pattern in sparse representation for large patterns
    if (pattern.size() > capacity_) {
        size_t blockSize = capacity_;
        for (size_t block = 0; block < pattern.size() / blockSize + 1; ++block) {
            std::vector<float> blockPattern;
            for (size_t i = 0; i < blockSize && block * blockSize + i < pattern.size(); ++i) {
                blockPattern.push_back(pattern[block * blockSize + i]);
            }
            if (!blockPattern.empty()) {
                sparseActivations_[block] = blockPattern;
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
}

void EnhancedWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    if (!brain_) return;
    
    // Create enhanced memory trace
    SimulationStep timestamp = 0;
    EnhancedMemoryTrace trace(neuron, activation, timestamp);
    trace.isTransient = true;
    trace.isConsolidated = false;
    trace.stability = 1.0f;
    
    // Add to priority queue
    if (priorityQueue_) {
        priorityQueue_->push(trace);
    }
    
    // Update neural activation
    brain_->injectCurrent(neuron, activation * 5.0f);
    
    // Update hash maps for O(1) lookup
    neuronActivations_[neuron.value] = activation;
    neuronTimestamps_[neuron.value] = timestamp;
    neuronImportances_[neuron.value] = 1.0f;
    neuronLevels_[neuron.value] = MemoryLevel::WORKING;
    
    // Add to ordered set
    orderedNeurons_.insert(neuron);
}

std::vector<float> EnhancedWorkingMemory::retrieve() const {
    std::vector<float> result;
    result.reserve(orderedNeurons_.size());
    
    // Retrieve activations from hash maps for O(1) lookup per neuron
    for (const NeuronId& neuron : orderedNeurons_) {
        auto it = neuronActivations_.find(neuron.value);
        if (it != neuronActivations_.end()) {
            result.push_back(it->second);
        }
    }
    
    return result;
}

bool EnhancedWorkingMemory::contains(NeuronId neuron) const {
    // O(1) lookup using hash map
    return neuronActivations_.find(neuron.value) != neuronActivations_.end();
}

float EnhancedWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    // O(1) lookup using hash map
    auto it = neuronActivations_.find(neuron.value);
    if (it != neuronActivations_.end()) {
        return it->second;
    }
    return 0.0f;
}

void EnhancedWorkingMemory::update(TimestepDuration dt) {
    if (!brain_ || !priorityQueue_ || !consolidationSystem_) return;
    
    // Profile the operation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    SimulationStep currentStep = 1;  // Simplified - in real implementation would track actual steps
    
    // Update priority queue (check for expired traces)
    priorityQueue_->cleanupExpiredTraces(currentStep, 1000);
    
    // Update all memory traces through priority queue
    auto allTraces = priorityQueue_->getAllSorted();
    
    for (auto& trace : allTraces) {
        // Update activation through brain interface
        if (brain_->getRegion(RegionId(trace.neuronId.value / 1000))) {
            brain_->injectCurrent(trace.neuronId, trace.activation * 2.0f);
        }
        
        // Age the trace
        trace.timestamp++;
        
        // Apply decay
        if (trace.timestamp > 100) {
            trace.activation *= (1.0f - decayRate_);
        }
        
        // Update in priority queue (re-insert with updated importance)
        priorityQueue_->push(trace);
        
        // Update hash map
        neuronActivations_[trace.neuronId.value] = trace.activation;
        neuronTimestamps_[trace.neuronId.value] = trace.timestamp;
    }
    
        // Perform memory consolidation
        consolidationSystem_->consolidate(priorityQueue_->getPriorityQueue(), neuronLevels_, neuronImportances_,
                                          capacity_, currentStep);
    
    // Run enhanced competition
    runCompetition();
    
    // Schedule replay if needed
    if (replayCount_ > 0 && replayCount_ % 100 == 0) {
        // In a full implementation, this would use WorkingMemoryReplay
        // For now, just increment for tracking
        replayCount_++;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
}

void EnhancedWorkingMemory::clear() {
    if (priorityQueue_) {
        while (!priorityQueue_->empty()) {
            priorityQueue_->pop();
        }
    }
    
    // Clear hash maps
    neuronActivations_.clear();
    neuronTimestamps_.clear();
    neuronImportances_.clear();
    neuronLevels_.clear();
    
    // Clear ordered set
    orderedNeurons_.clear();
    
    // Clear sparse representation
    sparseActivations_.clear();
}

void EnhancedWorkingMemory::strengthenMemory(float factor) {
    if (factor <= 0.0f) return;
    
    // Strengthen all memory traces through priority queue
    auto allTraces = priorityQueue_->getAllSorted();
    
    for (auto& trace : allTraces) {
        trace.activation = std::min(1.0f, trace.activation * factor);
        trace.importanceScore = std::min(1.0f, trace.importanceScore * factor);
        trace.stability = std::min(1.0f, trace.stability * factor);
        
        // Update hash maps
        neuronActivations_[trace.neuronId.value] = trace.activation;
        neuronImportances_[trace.neuronId.value] = trace.importanceScore;
        
        // Re-insert into priority queue
        priorityQueue_->push(trace);
    }
}

void EnhancedWorkingMemory::runCompetition() {
    winners_.clear();
    
    if (priorityQueue_->empty()) return;
    
    // Get all traces sorted by importance
    auto allTraces = priorityQueue_->getAllSorted();
    
    if (allTraces.empty()) return;
    
    // Enhanced competition: select top traces based on importance and stability
    float threshold = 0.3f;
    
    for (const auto& trace : allTraces) {
        if (trace.importanceScore >= threshold && trace.stability > 0.5f) {
            winners_.push_back(trace.neuronId);
            
            // Apply excitation to winners through brain interface
            if (brain_) {
                brain_->injectCurrent(trace.neuronId, trace.importanceScore * 2.0f);
            }
        }
    }
}

bool EnhancedWorkingMemory::isWinning(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

float EnhancedWorkingMemory::getMemoryActivity() const {
    if (orderedNeurons_.empty()) return 0.0f;
    
    float total = 0.0f;
    for (const NeuronId& neuron : orderedNeurons_) {
        auto it = neuronActivations_.find(neuron.value);
        if (it != neuronActivations_.end()) {
            total += it->second;
        }
    }
    
    return total / orderedNeurons_.size();
}

float EnhancedWorkingMemory::getNeuronImportance(NeuronId neuron) const {
    auto it = neuronImportances_.find(neuron.value);
    if (it != neuronImportances_.end()) {
        return it->second;
    }
    return 0.0f;
}

MemoryLevel EnhancedWorkingMemory::getMemoryLevel(NeuronId neuron) const {
    auto it = neuronLevels_.find(neuron.value);
    if (it != neuronLevels_.end()) {
        return it->second;
    }
    return MemoryLevel::WORKING;
}

void EnhancedWorkingMemory::promoteToStable(NeuronId neuron) {
    auto it = neuronLevels_.find(neuron.value);
    if (it != neuronLevels_.end()) {
        it->second = MemoryLevel::EPISODIC;
    }
}

void EnhancedWorkingMemory::demoteToTransient(NeuronId neuron) {
    auto it = neuronLevels_.find(neuron.value);
    if (it != neuronLevels_.end()) {
        it->second = MemoryLevel::WORKING;
    }
}

void EnhancedWorkingMemory::consolidateMemory() {
    if (consolidationSystem_ && !priorityQueue_->empty()) {
        SimulationStep currentStep = 1;  // Simplified
        consolidationSystem_->consolidate(priorityQueue_->getPriorityQueue(), neuronLevels_, neuronImportances_,
                                          capacity_, currentStep);
    }
}

EnhancedWorkingMemory::MemoryStats EnhancedWorkingMemory::getMemoryStats() const {
    MemoryStats stats;
    
    stats.totalTraces = orderedNeurons_.size();
    
    // Count transient vs stable traces
    stats.transientTraces = 0;
    stats.stableTraces = 0;
    
    for (const NeuronId& neuron : orderedNeurons_) {
        auto levelIt = neuronLevels_.find(neuron.value);
        if (levelIt != neuronLevels_.end()) {
            if (levelIt->second == MemoryLevel::EPISODIC || 
                levelIt->second == MemoryLevel::SEMANTIC ||
                levelIt->second == MemoryLevel::STRATEGIC) {
                stats.stableTraces++;
            } else {
                stats.transientTraces++;
            }
        }
    }
    
    // Calculate average importance
    if (!neuronImportances_.empty()) {
        float sum = 0.0f;
        for (const auto& pair : neuronImportances_) {
            sum += pair.second;
        }
        stats.averageImportance = sum / neuronImportances_.size();
    } else {
        stats.averageImportance = 0.0f;
    }
    
    // Calculate consolidation ratio
    if (stats.totalTraces > 0) {
        stats.consolidationRatio = static_cast<float>(stats.stableTraces) / stats.totalTraces;
    } else {
        stats.consolidationRatio = 0.0f;
    }
    
    return stats;
}

} // namespace nlm