#pragma once

#include "OptimizedPlasticitySystem.hpp"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <mutex>
#include <chrono>
#include <functional>

namespace nlm {

class OptimizedPlasticitySystem {
public:
    struct PlasticityConfig {
        bool enableSTDP{true};
        bool enableHebbian{true};
        bool enableStructural{true};
        float stdpLearningRate{0.01f};
        float hebbianLearningRate{0.01f};
        float structuralRate{0.0001f};
        bool lazyEvaluation{true};
        bool sparseUpdates{true};
        bool weightNormalization{true};
        size_t updateBatchSize{1000};
        float inactivityThreshold{10.0f};
    };

    OptimizedPlasticitySystem();
    ~OptimizedPlasticitySystem();

    // Disable copying, enable moving
    OptimizedPlasticitySystem(const OptimizedPlasticitySystem&) = delete;
    OptimizedPlasticitySystem& operator=(const OptimizedPlasticitySystem&) = delete;
    OptimizedPlasticitySystem(OptimizedPlasticitySystem&&) noexcept;
    OptimizedPlasticitySystem& operator=(OptimizedPlasticitySystem&&) noexcept;

    // Configuration
    void configure(const PlasticityConfig& config);
    const PlasticityConfig& getConfig() const;

    // Integration with existing plasticity systems
    void setSTDPSystem(STDP* stdp);
    void setHebbianSystem(Hebbian* hebbian);
    void setStructuralPlasticity(StructuralPlasticity* structural);
    void setInactiveSynapseTracker(InactiveSynapseTracker* tracker);
    void setWeightNormalizer(WeightNormalizer* normalizer);

    // Optimized plasticity updates
    void updatePlasticity(const std::vector<Synapse*>& synapses,
                          SimulationStep step, Timestamp time);
    void batchUpdatePlasticity(const std::vector<Synapse*>& synapses,
                              const std::vector<std::vector<Timestamp>>& preSpikeHistories,
                              const std::vector<std::vector<Timestamp>>& postSpikeHistories,
                              SimulationStep step, Timestamp time);

    // Lazy evaluation
    void schedulePlasticityUpdate(Synapse* synapse,
                                  const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  SimulationStep step, Timestamp time);
    void flushPendingUpdates();
    void enableLazyUpdates(bool enable);

    // Statistics and monitoring
    size_t getTotalUpdates() const;
    size_t getSuccessfulUpdates() const;
    size_t getSkippedUpdates() const;
    float getAverageUpdateTime() const;
    std::string getStatistics() const;

    // Optimization
    void optimizeForSparseConnectivity();
    void enableWeightNormalization(bool enable);
    void setUpdateThreshold(float threshold);

    // Reset and cleanup
    void reset();

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
