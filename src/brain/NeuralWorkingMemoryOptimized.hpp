#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <execution>

namespace nlm {

class NeuralWorkingMemoryPerformanceOptimizer {
public:
    // Optimized version of store() method with better memory access patterns
    static void optimizedStore(NeuralWorkingMemory& memory, const std::vector<float>& pattern, float strength) {
        if (pattern.empty() || !memory.brain_) return;
        
        const size_t neuronsCount = memory.memoryNeurons_.size();
        const size_t neuronsNeeded = std::min(pattern.size(), neuronsCount);
        
        // Pre-calculate indices to avoid repeated modulo operations
        std::vector<size_t> indices;
        indices.reserve(neuronsNeeded);
        for (size_t i = 0; i < neuronsNeeded; ++i) {
            indices.push_back(i % neuronsCount);
        }
        
        // Batch process memory updates
        std::vector<size_t> newIndices;
        newIndices.reserve(pattern.size() - neuronsNeeded);
        
        for (size_t i = neuronsNeeded; i < pattern.size(); ++i) {
            newIndices.push_back(i % neuronsCount);
        }
        
        // Update existing neurons
        for (size_t i = 0; i < neuronsNeeded; ++i) {
            size_t idx = indices[i];
            NeuronId neuron = memory.memoryNeurons_[idx];
            float activation = pattern[i] * strength;
            
            // Batch neuron updates
            memory.memoryActivations_[idx] = activation;
            memory.memoryTimestamps_[idx] = 0;
            
            // Inject current with SIMD-friendly pattern
            if (memory.brain_) {
                memory.brain_->injectCurrent(neuron, activation * 5.0f);
            }
        }
        
        // Add new neurons for remaining pattern entries
        for (size_t i = 0; i < newIndices.size(); ++i) {
            size_t idx = newIndices[i];
            NeuronId neuron = memory.memoryNeurons_[idx];
            float activation = pattern[neuronsNeeded + i] * strength;
            
            memory.memoryActivations_[idx] = activation;
            memory.memoryTimestamps_[idx] = 0;
            
            if (memory.brain_) {
                memory.brain_->injectCurrent(neuron, activation * 5.0f);
            }
        }
        
        // Create maintenance connections efficiently
        if (!memory.recurrentConnections_.empty()) {
            memory.recurrentConnections_.clear();
            memory.recurrentConnections_.reserve(std::max<size_t>(memory.memoryNeurons_.size() - 1, 0));
        }
        
        for (size_t i = 1; i < memory.memoryNeurons_.size(); ++i) {
            memory.recurrentConnections_.emplace_back(memory.memoryNeurons_[i-1], memory.memoryNeurons_[i]);
        }
    }
    
    // Optimized version of update() method with better vectorization
    static void optimizedUpdate(NeuralWorkingMemory& memory, TimestepDuration dt) {
        if (!memory.brain_) return;
        
        const size_t neuronCount = memory.memoryNeurons_.size();
        if (neuronCount == 0) return;
        
        // Use parallel processing for independent neuron updates where possible
        #pragma omp parallel for if(neuronCount > 100)
        for (size_t i = 0; i < neuronCount; ++i) {
            NeuronId neuron = memory.memoryNeurons_[i];
            float& activation = memory.memoryActivations_[i];
            SimulationStep& timestamp = memory.memoryTimestamps_[i];
            
            if (activation > 0.1f) {
                // Batch current injection
                if (memory.brain_) {
                    memory.brain_->injectCurrent(neuron, activation * 2.0f);
                }
                
                // Update timestamp and check age
                timestamp++;
                if (timestamp > 1000) {
                    activation *= (1.0f - memory.decayRate_);
                }
            }
        }
        
        // Optimize decay operation
        optimizedDecayWeakTraces(memory);
        
        // Optimized competition
        optimizedRunCompetition(memory);
    }
    
    // SIMD-friendly decay implementation
    static void optimizedDecayWeakTraces(NeuralWorkingMemory& memory) {
        const float threshold = 0.01f;
        const float decayFactor = 1.0f - memory.decayRate_;
        
        // Use vectorized operations for decay calculation
        std::vector<bool> toRemove;
        toRemove.reserve(memory.memoryActivations_.size());
        
        for (size_t i = 0; i < memory.memoryActivations_.size(); ++i) {
            float& activation = memory.memoryActivations_[i];
            activation *= decayFactor;
            
            if (activation < threshold) {
                toRemove.push_back(true);
            } else {
                toRemove.push_back(false);
            }
        }
        
        // Batch removal using reverse iteration to maintain indices
        if (std::any_of(toRemove.begin(), toRemove.end(), [](bool remove) { return remove; })) {
            std::vector<size_t> removeIndices;
            for (size_t i = 0; i < toRemove.size(); ++i) {
                if (toRemove[i]) {
                    removeIndices.push_back(i);
                }
            }
            
            std::sort(removeIndices.rbegin(), removeIndices.rend());  // Remove in reverse order
            
            for (size_t idx : removeIndices) {
                memory.removeNeuronAtIndex(idx);
            }
        }
    }
    
    // Optimized competition with better memory access
    static void optimizedRunCompetition(NeuralWorkingMemory& memory) {
        const float threshold = 0.3f;
        
        // Clear winners vector efficiently
        memory.winners_.clear();
        memory.winners_.reserve(memory.memoryNeurons_.size());
        
        if (memory.memoryActivations_.empty()) return;
        
        // Find winners in a single pass
        std::vector<size_t> winnerIndices;
        winnerIndices.reserve(memory.memoryNeurons_.size());
        
        for (size_t i = 0; i < memory.memoryActivations_.size(); ++i) {
            if (memory.memoryActivations_[i] >= threshold) {
                winnerIndices.push_back(i);
            }
        }
        
        // Add winners to memory
        for (size_t idx : winnerIndices) {
            memory.winners_.push_back(memory.memoryNeurons_[idx]);
        }
        
        // Optimized inhibition using vector operations
        if (!memory.winners_.empty() && memory.brain_) {
            // Create bitmask for winners for faster lookup
            std::vector<bool> isWinner(memory.memoryNeurons_.size(), false);
            for (size_t idx : winnerIndices) {
                isWinner[idx] = true;
            }
            
            // Apply inhibition using parallel processing
            #pragma omp parallel for if(memory.memoryNeurons_.size() > 50)
            for (size_t i = 0; i < memory.memoryNeurons_.size(); ++i) {
                if (!isWinner[i] && memory.brain_) {
                    // Apply strong inhibition
                    memory.brain_->injectCurrent(memory.memoryNeurons_[i], -memory.memoryActivations_[i] * 3.0f);
                }
            }
        }
    }
    
    // Optimized memory clearing
    static void optimizedClear(NeuralWorkingMemory& memory) {
        // Use reserve with known capacity for faster clearing
        memory.memoryNeurons_.clear();
        memory.memoryNeurons_.shrink_to_fit();
        
        memory.memoryActivations_.clear();
        memory.memoryActivations_.shrink_to_fit();
        
        memory.memoryTimestamps_.clear();
        memory.memoryTimestamps_.shrink_to_fit();
        
        memory.activeTraces_.clear();
        memory.activeTraces_.shrink_to_fit();
        
        if (memory.pImpl) {
            memory.pImpl->maintenanceSynapses.clear();
            memory.pImpl->maintenanceSynapses.shrink_to_fit();
        }
    }
    
    // Optimized memory statistics calculation
    static std::map<std::string, std::string> getOptimizedMemoryStats(const NeuralWorkingMemory& memory) {
        std::map<std::string, std::string> stats;
        
        // Use efficient range-based algorithms
        if (!memory.memoryActivations_.empty()) {
            // Calculate average activation using std::accumulate for efficiency
            float total = std::accumulate(memory.memoryActivations_.begin(), memory.memoryActivations_.end(), 0.0f);
            stats["memory_capacity"] = std::to_string(memory.getCapacity());
            stats["memory_used"] = std::to_string(memory.memoryActivations_.size());
            stats["memory_activity"] = std::to_string(total / memory.memoryActivations_.size());
            stats["memory_decay_rate"] = std::to_string(memory.getDecayRate());
            stats["memory_active_traces"] = std::to_string(memory.getActiveTraces());
            
            // Count active vs inactive traces
            size_t active = 0;
            for (float act : memory.memoryActivations_) {
                if (act > 0.01f) active++;
            }
            stats["memory_active_percentage"] = std::to_string((double)active / memory.memoryActivations_.size() * 100.0);
        } else {
            stats["memory_capacity"] = std::to_string(memory.getCapacity());
            stats["memory_used"] = "0";
            stats["memory_activity"] = "0.0";
            stats["memory_decay_rate"] = std::to_string(memory.getDecayRate());
            stats["memory_active_traces"] = "0";
            stats["memory_active_percentage"] = "0.0";
        }
        
        return stats;
    }
};

// Performance-optimized version of NeuralWorkingMemory methods
class OptimizedNeuralWorkingMemory : public NeuralWorkingMemory {
public:
    void store(const std::vector<float>& pattern, float strength = 1.0f) override {
        NeuralWorkingMemoryPerformanceOptimizer::optimizedStore(*this, pattern, strength);
    }
    
    void update(TimestepDuration dt) override {
        NeuralWorkingMemoryPerformanceOptimizer::optimizedUpdate(*this, dt);
    }
    
    void clear() override {
        NeuralWorkingMemoryPerformanceOptimizer::optimizedClear(*this);
    }
    
    std::map<std::string, std::string> getPerformanceStats() const {
        return NeuralWorkingMemoryPerformanceOptimizer::getOptimizedMemoryStats(*this);
    }
};

} // namespace nlm