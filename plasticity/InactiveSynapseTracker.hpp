#pragma once

#include "OptimizedPlasticitySystem.hpp"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <mutex>
#include <chrono>

namespace nlm {

class InactiveSynapseTracker {
public:
    struct SynapseActivity {
        SynapseId id;
        NeuronId source;
        NeuronId destination;
        float lastActivityTime;
        float totalActivityCount;
        bool isActive;
        bool hasPreSpike;
        bool hasPostSpike;
        
        SynapseActivity() : id(), source(), destination(), lastActivityTime(0.0f), 
                           totalActivityCount(0), isActive(false), hasPreSpike(false), 
                           hasPostSpike(false) {}
    };

    InactiveSynapseTracker();
    ~InactiveSynapseTracker();

    // Disable copying, enable moving
    InactiveSynapseTracker(const InactiveSynapseTracker&) = delete;
    InactiveSynapseTracker& operator=(const InactiveSynapseTracker&) = delete;
    InactiveSynapseTracker(InactiveSynapseTracker&&) noexcept;
    InactiveSynapseTracker& operator=(InactiveSynapseTracker&&) noexcept;

    // Activity tracking
    void recordPreSpike(SynapseId synapseId, NeuronId source, NeuronId destination, 
                        Timestamp timestamp);
    void recordPostSpike(SynapseId synapseId, NeuronId source, NeuronId destination,
                         Timestamp timestamp);
    void updateActivity(SynapseId synapseId, Timestamp timestamp);
    void markSynapseActive(SynapseId synapseId, bool active);

    // Query optimizations
    bool isSynapseActive(SynapseId synapseId) const;
    bool needsPlasticityUpdate(SynapseId synapseId) const;
    std::vector<SynapseId> getActiveSynapses() const;
    std::vector<SynapseId> getInactiveSynapses() const;
    std::vector<SynapseId> getPlasticityCandidates() const;

    // Optimization
    void cleanupInactiveSynapses(float timeThreshold);
    void optimizeForPlasticity(float activityThreshold);
    size_t getTotalSynapseCount() const;
    size_t getActiveSynapseCount() const;

    // Statistics
    std::string getStatistics() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
