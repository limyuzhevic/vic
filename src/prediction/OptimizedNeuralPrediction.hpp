#pragma once

#include "NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/SIMDVectorization.hpp"
#include "../performance/MemoryPool.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <deque>
#include <vector>
#include <array>
#include <chrono>

namespace nlm {

// Optimized NeuralPrediction with SIMD and caching
class OptimizedNeuralPrediction : public NeuralPrediction {
public:
    OptimizedNeuralPrediction();
    ~OptimizedNeuralPrediction() override;
    
    void initialize(Brain* brain) override;
    
    // Optimized prediction generation with SIMD
    std::vector<float> generatePrediction(SimulationStep currentStep) override;
    
    // Optimized prediction error computation with SIMD
    float updateWithObservation(const std::vector<float>& actualState,
                               SimulationStep currentStep) override;
    
    // Optimized action consequence prediction
    std::vector<float> predictActionConsequence(ActionType action,
                                              const std::vector<float>& currentState) override;
    
    // Optimized multi-step prediction
    std::vector<std::vector<float>> predictMultipleSteps(SimulationStep currentStep,
                                                         size_t numSteps) override;
    
    // Optimized sequence learning
    void recordSensoryState(const std::vector<float>& sensoryState,
                          SimulationStep currentStep) override;
    
    // Optimized temporal sequence learning
    void learnTemporalSequence(const std::vector<float>& currentState,
                               const std::vector<float>& nextState,
                               SimulationStep currentStep) override;
    
    // Optimized pattern matching with caching
    NeuronId findMatchingPatternNeuron(const std::vector<float>& pattern,
                                      float similarityThreshold) override;
    
    // Optimized similarity computation with SIMD
    float computeSimilarity(const std::vector<float>& a,
                           const std::vector<float>& b) const override;
    float computeSimilaritySIMD(const float* a, const float* b, size_t size) const;
    
    // Optimized prediction synapse modulation
    void modulatePredictionSynapses(float error, float reward) override;
    
    // Performance-optimized getters
    std::vector<NeuronId> getPredictionNeurons() const override;
    std::vector<NeuronId> getSequenceNeurons() const override;
    
    // Reset with optimized clearing
    void clearHistory() override;
    
    // Performance statistics
    struct PredictionPerformanceStats {
        double predictionAccuracy = 0.0;
        double learningSpeed = 0.0;
        size_t successfulPredictions = 0;
        size_t totalPredictions = 0;
        float avgPredictionTimeMs = 0.0f;
        size_t cacheHits = 0;
        size_t cacheMisses = 0;
        float memoryUsageMB = 0.0f;
        double processingEfficiency = 0.0;
    };
    
    PredictionPerformanceStats getPerformanceStats() const { return perfStats_; }
    
    // Optimization controls
    void enableSIMD(bool enable) { simdEnabled_ = enable; }
    void enableCaching(bool enable) { cachingEnabled_ = enable; }
    void setCacheSize(size_t size) { cacheSize_ = size; }
    
    bool isSIMDEnabled() const { return simdEnabled_; }
    bool isCachingEnabled() const { return cachingEnabled_; }
    
private:
    // Optimized data structures for caching
    struct PatternCache {
        std::vector<float> pattern;
        NeuronId neuronId;
        float similarity;
        SimulationStep timestamp;
        uint32_t hash;
        
        PatternCache(const std::vector<float>& p, NeuronId n, float s, SimulationStep t)
            : pattern(p), neuronId(n), similarity(s), timestamp(t), hash(computeHash(p)) {}
        
        static uint32_t computeHash(const std::vector<float>& pattern) {
            uint32_t hash = 0;
            for (float val : pattern) {
                hash = (hash * 31) + static_cast<uint32_t>(val * 1000.0f) & 0xFFFFFFFF;
            }
            return hash;
        }
    };
    
    // Cache for pattern matching
    std::unordered_map<uint32_t, PatternCache> patternCache_;
    std::deque<std::pair<std::vector<float>, SimulationStep>> optimizedStateHistory_;
    
    // Pre-allocated vector pools for reduced allocations
    struct VectorPool {
        std::vector<float> tempVector;
        std::vector<float> squaredVector;
        std::vector<float> similarityVector;
        
        void clear() {
            tempVector.clear();
            squaredVector.clear();
            similarityVector.clear();
        }
        
        void reserve(size_t size) {
            tempVector.reserve(size);
            squaredVector.reserve(size);
            similarityVector.reserve(size);
        }
    } pool_;
    
    // Optimized action consequence prediction
    struct ActionConsequenceOptimized {
        ActionType action;
        std::vector<float> beforeState;
        std::vector<float> afterState;
        float reward;
        float similarity;  // Cached similarity for faster lookup
        
        ActionConsequenceOptimized(ActionType a, const std::vector<float>& before,
                                  const std::vector<float>& after, float r)
            : action(a), beforeState(before), afterState(after), reward(r), similarity(0.0f) {}
    };
    
    // Batch processing for better cache locality
    void batchProcessStates(const std::vector<std::pair<std::vector<float>, SimulationStep>>& states);
    void batchLearnSequences(const std::vector<std::pair<std::vector<float>, SimulationStep>>& states);
    void batchComputeSimilarities(const std::vector<float>& target,
                                std::vector<std::pair<size_t, float>>& results) const;
    
    // Optimized error history with moving average
    struct ErrorHistoryOptimized {
        std::vector<float> errors;
        float movingAverage = 0.0f;
        size_t index = 0;
        
        void addError(float error) {
            if (errors.size() <= index) {
                errors.push_back(error);
            } else {
                float old = errors[index];
                errors[index] = error;
                movingAverage += (error - old) / errors.size();
            }
            index = (index + 1) % errors.size();
        }
        
        float getMovingAverage() const { return movingAverage; }
        void resize(size_t size) { errors.resize(size); }
    } errorHistory_;
    
    // Performance optimization flags
    bool simdEnabled_ = true;
    bool cachingEnabled_ = true;
    size_t cacheSize_ = 1000;
    
    // Performance statistics
    mutable PredictionPerformanceStats perfStats_;
    SimulationStep lastStep_;
    double totalPredictionTime_;
    
    // Integration with SIMD vectorizer
    VectorizedLIF simdVectorizer_;
    
    // Optimized memory pools
    std::unique_ptr<MemoryPool<PatternCache>> patternCachePool_;
    std::unique_ptr<MemoryPool<float>> statePool_;
    
    // Update performance statistics
    void updatePerformanceStats(float predictionTime) const;
    
    // Cache management
    void cachePattern(const std::vector<float>& pattern, NeuronId neuronId, float similarity);
    bool getCachedPattern(uint32_t hash, PatternCache& out) const;
    
    // SIMD-friendly operations
    void simdBatchSimilarity(const std::vector<float>& target,
                           const std::vector<std::vector<float>>& patterns,
                           std::vector<float>& similarities) const;
};

} // namespace nlm