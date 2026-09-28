#pragma once

#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "../performance/MemoryPool.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <deque>
#include <atomic>

namespace nlm {

// Optimized NeuralWorkingMemory with O(1) lookups and reduced allocations
class OptimizedNeuralWorkingMemory : public NeuralWorkingMemory {
public:
    OptimizedNeuralWorkingMemory();
    ~OptimizedNeuralWorkingMemory() override;
    
    // Initialize with brain reference
    void initialize(Brain* brain) override;
    
    // Store information in working memory
    void store(const std::vector<float>& pattern, float strength = 1.0f) override;
    
    // Store specific neuron activation  
    void storeToNeuron(NeuronId neuron, float activation) override;
    
    // Retrieve current working memory content as activity levels
    std::vector<float> retrieve() const override;
    
    // Check if specific neuron is part of working memory
    bool contains(NeuronId neuron) const override;
    
    // Get activation level of a specific neuron in working memory
    float getNeuronActivation(NeuronId neuron) const override;
    
    // Update working memory (maintenance and decay)
    void update(TimestepDuration dt) override;
    
    // Clear working memory
    void clear() override;
    
    // Get number of active memory traces
    size_t getActiveTraces() const override { return activeTraces_.size(); }
    
    // Get capacity
    size_t getCapacity() const override { return capacity_; }
    void setCapacity(size_t cap) override { capacity_ = cap; }
    
    // Decay rate for memory traces
    float getDecayRate() const override { return decayRate_; }
    void setDecayRate(float rate) override { decayRate_ = rate; }
    
    // Get neurons currently in working memory
    const std::vector<NeuronId>& getMemoryNeurons() const override { return memoryNeurons_; }
    
    // Strengthen working memory representation (for rehearsal)
    void strengthenMemory(float factor) override;
    
    // Competition between memory traces
    void runCompetition() override;
    
    // Is this neuron part of the winning population?
    bool isWinning(NeuronId neuron) const override;
    
    // Get overall memory activity level
    float getMemoryActivity() const override;
    
    // Neuromodulator getters
    float getDopamineLevel() const override;
    float getCuriosityLevel() const override;
    float getNoveltyLevel() const override;
    float getDevelopmentalStage() const override;
    float getPredictionError() const override;
    
    // Set target memory activity level for prediction error
    void setTargetMemoryActivity(float target) override { targetMemoryActivity_ = target; }
    
    // Integration with prediction system
    void integratePrediction(const std::vector<float>& prediction) override;
    
    // Integration with attention system
    void integrateAttention(const std::vector<NeuronId>& attendedNeurons) override;
    
private:
    // Optimized data structures for O(1) lookups
    struct NeuronActivation {
        NeuronId neuron;
        float activation;
        SimulationStep timestamp;
        uint32_t hash;
        
        NeuronActivation(NeuronId n, float a, SimulationStep t) 
            : neuron(n), activation(a), timestamp(t), hash(hashNeuronId(n)) {}
        
        static uint32_t hashNeuronId(NeuronId id) {
            return static_cast<uint32_t>(id.getId() & 0xFFFFFFFF);
        }
    };
    
    // Hash table for O(1) neuron lookup
    std::unordered_map<uint32_t, size_t> neuronIndex_;
    std::vector<NeuronActivation> neuronActivations_;
    
    // Pre-allocated vector pools for reduced allocations
    struct VectorPool {
        std::vector<NeuronId> neuronPool;
        std::vector<float> activationPool;
        std::vector<SimulationStep> timestampPool;
        size_t size_ = 0;
        
        void clear() { 
            neuronPool.clear(); 
            activationPool.clear(); 
            timestampPool.clear(); 
            size_ = 0; 
        }
        
        void reserve(size_t capacity) {
            neuronPool.reserve(capacity);
            activationPool.reserve(capacity);
            timestampPool.reserve(capacity);
        }
    };
    
    VectorPool tempPool_;
    VectorPool sortedPool_;
    
    // Enhanced neuromodulator system
    struct NeuromodulatorOptimized {
        float level = 0.0f;
        float target = 0.0f;
        float decayRate = 0.01f;
        float releaseRate = 0.05f;
        float smoothingFactor = 0.9f;  // For exponential smoothing
        SimulationStep lastUpdate = 0;
        
        void update(TimestepDuration dt, SimulationStep step) {
            // Exponential smoothing for smooth transitions
            if (target != level) {
                float diff = target - level;
                level += diff * (1.0f - std::pow(smoothingFactor, dt));
            }
            
            // Decay towards target
            float decay = decayRate * static_cast<float>(dt);
            level = std::max(target - decay, std::min(target + decay, level));
            
            lastUpdate = step;
        }
        
        void setLevel(float newLevel, float strength = 1.0f) {
            target = std::clamp(newLevel * strength, 0.0f, 1.0f);
        }
    };
    
    // Optimized neuromodulators with smoothing
    NeuromodulatorOptimized dopamine_;
    NeuromodulatorOptimized curiosity_;
    NeuromodulatorOptimized novelty_;
    
    // Fast path for common operations
    struct FastPath {
        bool initialized = false;
        size_t neuronCount = 0;
        float* activationPtr = nullptr;
        NeuronId* neuronPtr = nullptr;
        
        void updatePointers(const std::vector<NeuronId>& neurons, 
                           const std::vector<float>& activations) {
            if (!neurons.empty()) {
                neuronCount = neurons.size();
                neuronPtr = const_cast<NeuronId*>(neurons.data());
                activationPtr = const_cast<float*>(activations.data());
                initialized = true;
            }
        }
    } fastPath_;
    
    // Batch processing for better cache locality
    void batchStore(const std::vector<float>& pattern, float strength);
    void batchUpdate(TimestepDuration dt);
    void batchCompetition();
    
    // Optimized similarity computation
    float computeSimilarityOptimized(const float* a, const float* b, size_t size) const;
    
    // Memory-efficient reuse of temporary vectors
    static std::vector<float>& getTempVector(size_t size);
    static void returnTempVector(std::vector<float>&& vec);
    
    // Pre-allocated memory pools from Performance framework
    std::unique_ptr<MemoryPool<NeuronActivation>> neuronPool_;
    std::unique_ptr<MemoryPool<float>> activationPool_;
};