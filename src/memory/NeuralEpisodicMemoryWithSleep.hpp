#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/SleepState.hpp"
#include "NeuralEpisodicMemory.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <deque>

namespace nlm {

// Sleep-enhanced episodic memory with state-specific replay patterns
class NeuralEpisodicMemoryWithSleep {
public:
    NeuralEpisodicMemoryWithSleep();
    ~NeuralEpisodicMemoryWithSleep();
    
    // Initialize with sleep state manager reference
    void initialize(Brain* brain);
    
    // Store a new experience with sleep state awareness
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Retrieve episodes similar to a query pattern
    std::vector<const EpisodicMemoryItem*> retrieveSimilar(const std::vector<float>& sensoryPattern,
                                                           size_t maxResults = 5) const;
    
    // Retrieve episodes from a specific time window
    std::vector<const EpisodicMemoryItem*> retrieveTemporal(SimulationStep startTime,
                                                             SimulationStep endTime,
                                                             size_t maxResults = 10) const;
    
    // Retrieve episodes by location
    std::vector<const EpisodicMemoryItem*> retrieveByLocation(float x, float y,
                                                              float radius,
                                                              size_t maxResults = 5) const;
    
    // Retrieve episodes following a specific action
    std::vector<const EpisodicMemoryItem*> retrieveAfterAction(ActionType action,
                                                               size_t maxResults = 5) const;
    
    // Replay an episode - reactivates the neural patterns associated with it
    void replayEpisode(const EpisodicMemoryItem* episode);
    
    // Update episode relevance based on recent use
    void updateRelevance(SimulationStep episodeId, float relevanceDelta);
    
    // Get episode by index
    const EpisodicMemoryItem* getEpisode(size_t index) const;
    
    // Get total number of episodes
    size_t getEpisodeCount() const { return episodes_.size(); }
    
    // Get recent episodes
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const;
    
    // Get average reward from episodes
    float getAverageReward() const;
    
    // Clear old episodes
    void consolidate(float relevanceThreshold);
    
    // Clear all episodes
    void clear();
    
    // Enable/disable replay
    void enableReplay(bool enable) { replayEnabled_ = enable; }
    bool isReplayEnabled() const { return replayEnabled_; }
    
    // Get episodes for replay (selection based on relevance and recency)
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
    
    // Replay multiple episodes (for sleep-like consolidation)
    void replaySequence(const std::vector<size_t>& episodeIds);
    
    // Sleep-specific replay and consolidation methods
    void executeSleepReplay(SleepState sleepState);
    void consolidateDuringSleep(SleepState sleepState, float duration);
    void formSchemasDuringREMSleep();
    void integrateMemoriesDuringSleep(SleepState sleepState);
    
    // State-specific replay patterns
    void executeQuietSleepReplayPattern();
    void executeDeepSleepReplayPattern();
    void executeREMSleepReplayPattern();
    
    // Replay sequencing and timing based on sleep state
    void scheduleReplaySequence(SleepState sleepState);
    void sequenceReplayTiming(SleepState sleepState, float& replayInterval, float& replayStrength);
    
    // Consolidation integration with sleep states
    void integrateConsolidationWithSleepStates(SleepState sleepState);
    void systemsConsolidationDuringDeepSleep();
    void synapticConsolidationDuringQuietSleep();
    void schemaFormationAndGeneralization();
    
    // State management
    SleepState getCurrentSleepState() const { return currentSleepState_; }
    void setCurrentSleepState(SleepState state) { currentSleepState_ = state; }
    
    // Memory replay coordination
    void scheduleMemoryReplay();
    void executeMemoryReplay();
    
    // Get memory statistics
    float getMemoryReplayCount() const { return memoryReplayCount_; }
    float getConsolidationProgress() const { return consolidationProgress_; }
    float getSchemaFormationProgress() const { return schemaFormationProgress_; }
    
    // Checkpointing support
    std::string serialize() const;
    bool deserialize(const std::string& data);

private:
    // Compute similarity between query and stored episode
    float computeSimilarity(const std::vector<float>& query,
                          const EpisodicMemoryItem& episode) const;
    
    // Check if episode matches criteria
    bool matchesCriteria(const EpisodicMemoryItem& episode,
                        const std::vector<float>* sensoryQuery,
                        SimulationStep* startTime,
                        SimulationStep* endTime,
                        ActionType* actionQuery) const;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::deque<EpisodicMemoryItem> episodes_;
    size_t maxEpisodes_;
    bool replayEnabled_;
    
    // Sleep state management
    SleepState currentSleepState_;
    
    // Memory replay coordination
    float memoryReplayCount_;
    float replayStartTime_;
    float replayDuration_;
    
    // Consolidation statistics
    float consolidationProgress_;
    float schemaFormationProgress_;
    float integrationProgress_;
    
    // Sleep-specific timing
    float quietSleepReplayTimer_;
    float deepSleepReplayTimer_;
    float remSleepReplayTimer_;
    
    // State-specific replay parameters
    std::map<SleepState, float> stateReplayIntervals_;
    std::map<SleepState, float> stateReplayStrengths_;
    std::map<SleepState, float> stateConsolidationIntensities_;
    std::map<SleepState, float> stateSchemaFormationStrengths_;
};

} // namespace nlm
