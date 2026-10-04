#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <deque>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <functional>

namespace nlm {

// MemoryReplaySession: Represents a single memory replay session
struct MemoryReplaySession {
    SimulationStep startTime;
    SimulationStep endTime;
    SimulationStep episodeId;
    float replayStrength;
    float temporalCompression;
    float synapticChange;
    std::vector<NeuronId> reactivatedNeurons;
    std::vector<float> activationPattern;
    float predictionError;
    float noveltyValue;
    float rewardAssociation;
    std::string context;
    
    MemoryReplaySession()
        : startTime(0)
        , endTime(0)
        , episodeId(0)
        , replayStrength(0.0f)
        , temporalCompression(1.0f)
        , synapticChange(0.0f)
        , predictionError(0.0f)
        , noveltyValue(0.0f)
        , rewardAssociation(0.0f) {}
};

// SpontaneousReplayTrigger: Triggers for spontaneous memory replay
struct SpontaneousReplayTrigger {
    enum class TriggerType {
        TimeBased,
        RestBased,
        NoveltyBased,
        RewardBased,
        AttentionBased,
        Homeostatic
    };
    
    TriggerType type;
    float threshold;
    float probability;
    float cooldown;
    float intensity;
    
    SpontaneousReplayTrigger()
        : type(TriggerType::TimeBased)
        , threshold(0.0f)
        , probability(0.0f)
        , cooldown(0.0f)
        , intensity(1.0f) {}
};

// ReplayCompressionParameters: Parameters for temporal compression of replay
struct ReplayCompressionParameters {
    float compressionRatio;
    float decayFactor;
    float replayDuration;
    float strengthScaling;
    float noveltyAmplification;
    float rewardAmplification;
    
    ReplayCompressionParameters()
        : compressionRatio(0.5f)
        , decayFactor(0.9f)
        , replayDuration(100.0f)
        , strengthScaling(1.0f)
        , noveltyAmplification(1.5f)
        , rewardAmplification(1.2f) {}
};

// ReconsolidationUpdate: Information for memory reconsolidation
struct ReconsolidationUpdate {
    SimulationStep episodeId;
    std::vector<float> updatedActivationPattern;
    float updatedStrength;
    float updateMagnitude;
    float reconsolidationSuccess;
    
    ReconsolidationUpdate()
        : episodeId(0)
        , updatedStrength(0.0f)
        , updateMagnitude(0.0f)
        , reconsolidationSuccess(0.0f) {}
};

// TargetedReplayRequest: Request for targeted memory strengthening
struct TargetedReplayRequest {
    SimulationStep episodeId;
    float priorityScore;
    float targetStrength;
    float expectedBenefit;
    float replayDuration;
    std::vector<NeuronId> targetNeurons;
    std::string context;
    float noveltyThreshold;
    float rewardThreshold;
    
    TargetedReplayRequest()
        : episodeId(0)
        , priorityScore(0.0f)
        , targetStrength(0.0f)
        , expectedBenefit(0.0f)
        , replayDuration(100.0f)
        , noveltyThreshold(0.5f)
        , rewardThreshold(0.5f) {}
};

class ReplaySystem {
public:
    ReplaySystem();
    ~ReplaySystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update replay system
    void update(TimestepDuration dt);
    
    // Trigger spontaneous replay
    void triggerSpontaneousReplay(const std::vector<SpontaneousReplayTrigger>& triggers);
    
    // Trigger reconsolidation for an episode
    void triggerReconsolidation(SimulationStep episodeId, const std::vector<float>& newInput);
    
    // Request targeted replay for memory strengthening
    void requestTargetedReplay(const TargetedReplayRequest& request);
    
    // Execute replay session
    void executeReplaySession(const MemoryReplaySession& session);
    
    // Schedule replay events
    void scheduleReplayEvents();
    
    // Get recent replay sessions
    const std::vector<MemoryReplaySession>& getRecentReplaySessions() const { return replaySessions_; }
    
    // Get reconsolidation updates
    const std::vector<ReconsolidationUpdate>& getReconsolidationUpdates() const { return reconsolidationUpdates_; }
    
    // Get spontaneous replay triggers
    const std::vector<SpontaneousReplayTrigger>& getSpontaneousTriggers() const { return spontaneousTriggers_; }
    
    // Get targeted replay requests
    const std::vector<TargetedReplayRequest>& getTargetedRequests() const { return targetedRequests_; }
    
    // Set replay parameters
    void setCompressionParameters(const ReplayCompressionParameters& params) { compressionParams_ = params; }
    const ReplayCompressionParameters& getCompressionParameters() const { return compressionParams_; }
    
    // Set replay statistics
    void setReplayStatistics(const std::unordered_map<std::string, float>& stats) { replayStatistics_ = stats; }
    const std::unordered_map<std::string, float>& getReplayStatistics() const { return replayStatistics_; }
    
    // Enable/disable replay
    void enableReplay(bool enable) { replayEnabled_ = enable; }
    bool isReplayEnabled() const { return replayEnabled_; }
    
    // Reset system
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::vector<MemoryReplaySession> replaySessions_;
    std::vector<ReconsolidationUpdate> reconsolidationUpdates_;
    std::vector<SpontaneousReplayTrigger> spontaneousTriggers_;
    std::vector<TargetedReplayRequest> targetedRequests_;
    
    // Replay control
    bool replayEnabled_;
    float replayCooldown_;
    float spontaneousReplayProbability_;
    float reconsolidationThreshold_;
    
    // Replay statistics
    std::unordered_map<std::string, float> replayStatistics_;
    
    // Timing
    float replayTimer_;
    float reconsolidationTimer_;
    float compressionTimer_;
    
    // Compression parameters
    ReplayCompressionParameters compressionParams_;
    
    // Internal methods
    void initializeSpontaneousTriggers();
    void initializeCompressionParameters();
    void processSpontaneousTriggers();
    void processReconsolidationUpdates();
    void processTargetedRequests();
    void calculateReplayStrength(const MemoryReplaySession& session);
    void applyTemporalCompression(MemoryReplaySession& session);
    void updateSynapticConnectivity(const MemoryReplaySession& session);
    void recordReplayEvent(const MemoryReplaySession& session);
    void recordReconsolidationEvent(const ReconsolidationUpdate& update);
    MemoryReplaySession createReplaySession(SimulationStep episodeId, float strength);
    float calculateReplayBenefit(const MemoryReplaySession& session) const;
    bool shouldReconsolidate(SimulationStep episodeId) const;
};

} // namespace nlm
