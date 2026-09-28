#pragma once

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "../performance/MemoryPool.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>
#include <array>
#include <atomic>

namespace nlm {

// Optimized AgentBrain with batch processing and cache-friendly structures
class OptimizedAgentBrain : public AgentBrain {
public:
    OptimizedAgentBrain(std::shared_ptr<Brain> brain);
    ~OptimizedAgentBrain() override;
    
    void initialize(const SimpleWorld& world) override;
    
    // Optimized sensory processing with SIMD-friendly batch operations
    void processSensoryInput(const SensoryPercept& percept) override;
    void batchProcessVision(const std::vector<float>& vision);
    void batchProcessTouch(const std::vector<float>& touch);
    void batchProcessInternal(const std::vector<float>& internal);
    void batchProcessProprioception(const std::vector<float>& proprio);
    
    // Optimized motor command decoding
    MotorCommand decodeMotorCommand() override;
    MotorCommand decodeFromMotorNeurons() override;
    
    // Optimized neuromodulation
    void applyRewardModulation(float reward, float predictedReward) override;
    
    // Optimized development
    void updateDevelopment(double timestep) override;
    
    // Optimized reset
    void reset() override;
    
    // Getters
    float getNeuromodulationLevel() const override { return dopamineLevel_; }
    float getCuriosityLevel() const override { return curiosityLevel_; }
    float getNoveltyLevel() const override { return noveltyLevel_; }
    float getPredictionError() const override { return predictionError_; }
    
    // Performance statistics
    struct BrainPerformanceStats {
        double totalActivity = 0.0;
        double totalEnergy = 0.0;
        size_t activeNeurons = 0;
        size_t firingNeurons = 0;
        float avgFiringRate = 0.0f;
        float memoryUsageMB = 0.0f;
        double processingTimeMs = 0.0;
    };
    
    BrainPerformanceStats getPerformanceStats() const { return perfStats_; }
    
private:
    // Cache-friendly data structures for O(1) neuron group access
    struct NeuronGroup {
        Neuron* neuron;           // Pointer to neuron
        float activity;           // Cached activity value
        bool active;              // Fast active check
        uint32_t hash;            // Hash for O(1) lookup
        
        NeuronGroup(Neuron* n) : neuron(n), activity(0.0f), active(false), hash(0) {
            if (n) hash = static_cast<uint32_t>(n->getId().getId() & 0xFFFFFFFF);
        }
    };
    
    // Optimized lookup tables with perfect hashing
    std::unordered_map<uint32_t, NeuronGroup*> motorLookup_;
    std::unordered_map<uint32_t, NeuronGroup*> sensoryLookup_;
    
    // Pre-allocated vector pools for reduced allocations
    struct BatchPool {
        std::vector<float> visionBatch;
        std::vector<float> touchBatch;
        std::vector<float> internalBatch;
        std::vector<float> proprioBatch;
        std::vector<float> motorActivities;
        
        void clear() {
            visionBatch.clear();
            touchBatch.clear();
            internalBatch.clear();
            proprioBatch.clear();
            motorActivities.clear();
        }
        
        void reserve(size_t size) {
            visionBatch.reserve(size);
            touchBatch.reserve(size);
            internalBatch.reserve(size);
            proprioBatch.reserve(size);
            motorActivities.reserve(6); // Motor has 6 commands
        }
    } batchPool_;
    
    // Optimized neuromodulator system with exponential smoothing
    struct NeuromodulatorOptimized {
        float level = 0.0f;
        float target = 0.0f;
        float previousLevel = 0.0f;
        float decayRate = 0.01f;
        float releaseRate = 0.05f;
        float smoothingFactor = 0.95f;  // For smooth transitions
        SimulationStep lastUpdate = 0;
        
        // Optimized update with exponential smoothing
        void update(TimestepDuration dt, SimulationStep step) {
            if (step == lastUpdate) return;
            
            lastUpdate = step;
            
            // Exponential smoothing for smooth transitions
            if (target != level) {
                float diff = target - level;
                level += diff * (1.0f - std::pow(smoothingFactor, dt));
            }
            
            // Gentle decay towards target
            float decay = decayRate * static_cast<float>(dt);
            level = (1.0f - decay) * level + decay * target;
            
            // Clamp to reasonable range
            level = std::clamp(level, 0.0f, 1.0f);
        }
        
        void setLevel(float newLevel, float strength = 1.0f) {
            target = std::clamp(newLevel * strength, 0.0f, 1.0f);
        }
        
        float getSmoothedLevel(TimestepDuration dt) const {
            // Predict next value based on trend
            return level + (level - previousLevel) * (1.0f - decayRate * dt);
        }
    };
    
    // Optimized neuromodulators
    NeuromodulatorOptimized dopamine_;
    NeuromodulatorOptimized curiosity_;
    NeuromodulatorOptimized novelty_;
    
    // Fast path for common operations
    struct FastPath {
        bool initialized = false;
        size_t visionSize = 0;
        size_t touchSize = 0;
        size_t internalSize = 0;
        size_t proprioSize = 0;
        
        const float* visionPtr = nullptr;
        const float* touchPtr = nullptr;
        const float* internalPtr = nullptr;
        const float* proprioPtr = nullptr;
        
        void updatePointers(const std::vector<float>& vision,
                           const std::vector<float>& touch,
                           const std::vector<float>& internal,
                           const std::vector<float>& proprio) {
            visionSize = vision.size();
            touchSize = touch.size();
            internalSize = internal.size();
            proprioSize = proprio.size();
            
            visionPtr = vision.data();
            touchPtr = touch.data();
            internalPtr = internal.data();
            proprioPtr = proprio.data();
            
            initialized = true;
        }
        
        void clear() {
            visionSize = touchSize = internalSize = proprioSize = 0;
            visionPtr = touchPtr = internalPtr = proprioPtr = nullptr;
            initialized = false;
        }
    } fastPath_;
    
    // Optimized motor command selection with SIMD-like operations
    struct MotorCommandOptimized {
        MotorCommand command;
        float activity;
        float curiosityWeight;
        float noveltyWeight;
        
        MotorCommandOptimized(MotorCommand cmd, float act, float cw = 1.0f, float nw = 1.0f)
            : command(cmd), activity(act), curiosityWeight(cw), noveltyWeight(nw) {}
        
        bool operator<(const MotorCommandOptimized& other) const {
            return activity < other.activity;
        }
    };
    
    // Batch processing for better cache locality
    void batchComputeActivities();
    void batchUpdateNeuromodulators(TimestepDuration dt);
    
    // Optimized eligibility trace system
    struct EligibilityTraceOptimized {
        float trace;              // Current eligibility
        float decay;              // Decay rate
        SimulationStep lastUpdate; // Last update step
        
        EligibilityTraceOptimized() : trace(0.0f), decay(0.1f), lastUpdate(0) {}
        
        void update(float reward, TimestepDuration dt, SimulationStep step) {
            if (step == lastUpdate) return;
            
            // Reward-modulated eligibility
            trace = std::max(0.0f, trace + reward * 0.01f - decay * static_cast<float>(dt));
            lastUpdate = step;
        }
        
        void decay() {
            trace *= (1.0f - decay);
        }
    };
    
    // Pre-allocated pool for eligibility traces
    std::vector<EligibilityTraceOptimized> eligibilityTraces_;
    
    // Performance statistics
    mutable BrainPerformanceStats perfStats_;
    SimulationStep lastStep_;
    double stepTimeStart_;
    
    // Integration with performance framework
    void updatePerformanceStats() const;
};