#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <algorithm>
#include <mutex>

namespace nlm {

// Neuron Spike History Manager - Specialized for neuron spike history
class NeuronSpikeHistoryManager {
public:
    explicit NeuronSpikeHistoryManager(size_t maxHistory = 100)
        : maxHistorySize(maxHistory) {}
    
    virtual ~NeuronSpikeHistoryManager() = default;
    
    // Record a pre-synaptic spike (from sending neuron)
    virtual void recordPreSpike(Timestamp timestamp) {
        std::lock_guard<std::mutex> lock(mutex);
        preSpikeHistory.push_back(timestamp);
        if (preSpikeHistory.size() > maxHistorySize) {
            preSpikeHistory.erase(preSpikeHistory.begin());
        }
    }
    
    // Record a post-synaptic spike (from receiving neuron)
    virtual void recordPostSpike(Timestamp timestamp) {
        std::lock_guard<std::mutex> lock(mutex);
        postSpikeHistory.push_back(timestamp);
        if (postSpikeHistory.size() > maxHistorySize) {
            postSpikeHistory.erase(postSpikeHistory.begin());
        }
    }
    
    // Clear all spike history
    virtual void clearHistory() {
        std::lock_guard<std::mutex> lock(mutex);
        preSpikeHistory.clear();
        postSpikeHistory.clear();
    }
    
    // Get const reference to pre-synaptic spike history
    virtual const std::vector<Timestamp>& getPreSpikeHistory() const {
        std::lock_guard<std::mutex> lock(mutex);
        return preSpikeHistory;
    }
    
    // Get const reference to post-synaptic spike history
    virtual const std::vector<Timestamp>& getPostSpikeHistory() const {
        std::lock_guard<std::mutex> lock(mutex);
        return postSpikeHistory;
    }
    
    // Get current pre-synaptic history size
    virtual size_t getPreSpikeHistorySize() const {
        std::lock_guard<std::mutex> lock(mutex);
        return preSpikeHistory.size();
    }
    
    // Get current post-synaptic history size
    virtual size_t getPostSpikeHistorySize() const {
        std::lock_guard<std::mutex> lock(mutex);
        return postSpikeHistory.size();
    }
    
protected:
    std::vector<Timestamp> preSpikeHistory;
    std::vector<Timestamp> postSpikeHistory;
    size_t maxHistorySize;
    mutable std::mutex mutex;
};

// Synapse efficacy manager - Handles use-dependent synaptic modulation
class SynapseEfficacyManager {
public:
    SynapseEfficacyManager() : efficacy(1.0f), depression(1.0f), facilitation(0.0f) {}
    
    virtual ~SynapseEfficacyManager() = default;
    
    // Get current efficacy
    virtual float getEfficacy() const {
        std::lock_guard<std::mutex> lock(mutex);
        return efficacy;
    }
    
    // Set efficacy with bounds checking
    virtual void setEfficacy(float eff) {
        std::lock_guard<std::mutex> lock(mutex);
        efficacy = std::clamp(eff, 0.0f, 2.0f);
    }
    
    // Update efficacy based on use
    virtual void updateEfficacy(float preSpikeTime, float postSpikeTime, Timestamp currentTime) {
        std::lock_guard<std::mutex> lock(mutex);
        
        if (preSpikeTime >= 0.0f) {
            float timeSincePre = currentTime - preSpikeTime;
            facilitation = std::exp(-timeSincePre / 100.0f); // STP_FACILITATION_TAU
        }
        
        if (postSpikeTime >= 0.0f || preSpikeTime >= 0.0f) {
            float timeSinceActivity = std::max(
                postSpikeTime >= 0.0f ? currentTime - postSpikeTime : 0.0f,
                preSpikeTime >= 0.0f ? currentTime - preSpikeTime : 0.0f
            );
            // Recovery from depression toward 1.0
            depression += (1.0f - depression) * (1.0f - std::exp(-timeSinceActivity / 200.0f)); // STP_DEPRESSION_TAU
        }
        
        // Clamp efficacy to prevent instability
        efficacy = std::clamp(efficacy, 0.0f, 2.0f);
    }
    
    // Reset to initial state
    virtual void reset() {
        std::lock_guard<std::mutex> lock(mutex);
        efficacy = 1.0f;
        depression = 1.0f;
        facilitation = 0.0f;
    }
    
    // Get depression factor
    virtual float getDepression() const {
        std::lock_guard<std::mutex> lock(mutex);
        return depression;
    }
    
    // Get facilitation factor
    virtual float getFacilitation() const {
        std::lock_guard<std::mutex> lock(mutex);
        return facilitation;
    }
    
    // Set last spike times
    virtual void setLastSpikeTimes(float preSpikeTime, float postSpikeTime) {
        std::lock_guard<std::mutex> lock(mutex);
        lastPreSpikeTime = preSpikeTime;
        lastPostSpikeTime = postSpikeTime;
    }
    
    virtual float getLastPreSpikeTime() const {
        std::lock_guard<std::mutex> lock(mutex);
        return lastPreSpikeTime;
    }
    
    virtual float getLastPostSpikeTime() const {
        std::lock_guard<std::mutex> lock(mutex);
        return lastPostSpikeTime;
    }
    
protected:
    float efficacy;
    float depression;
    float facilitation;
    float lastPreSpikeTime;
    float lastPostSpikeTime;
    mutable std::mutex mutex;
};

// Synapse eligibility trace manager - For reward-modulated learning
class SynapseEligibilityTraceManager {
public:
    SynapseEligibilityTraceManager() : eligibilityTrace(0.0f) {}
    
    virtual ~SynapseEligibilityTraceManager() = default;
    
    // Get eligibility trace
    virtual float getEligibilityTrace() const {
        std::lock_guard<std::mutex> lock(mutex);
        return eligibilityTrace;
    }
    
    // Set eligibility trace
    virtual void setEligibilityTrace(float trace) {
        std::lock_guard<std::mutex> lock(mutex);
        eligibilityTrace = trace;
    }
    
    // Decay eligibility trace
    virtual void decayEligibilityTrace(float decayRate) {
        std::lock_guard<std::mutex> lock(mutex);
        eligibilityTrace *= (1.0f - decayRate);
        if (std::abs(eligibilityTrace) < 0.001f) {
            eligibilityTrace = 0.0f;
        }
    }
    
    // Reset trace
    virtual void reset() {
        std::lock_guard<std::mutex> lock(mutex);
        eligibilityTrace = 0.0f;
    }
    
protected:
    float eligibilityTrace;
    mutable std::mutex mutex;
};

} // namespace nlm
