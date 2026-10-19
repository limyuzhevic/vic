#pragma once

#include "MemoryInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

class EpisodicMemory : public Memory {
public:
    EpisodicMemory();
    ~EpisodicMemory();
    
    // Core memory interface
    void store(const std::vector<float>& pattern, float relevance = 1.0f) override;
    std::vector<float> retrieve(size_t index) const override;
    bool contains(const std::vector<float>& pattern) const override;
    
    void decay(float decayRate) override;
    void consolidate(float relevanceThreshold) override;
    void clear() override;
    
    size_t getCapacity() const override;
    size_t getCurrentSize() const override;
    float getLoad() const override;
    
    void updateFromBrain(Brain* brain) override;
    void applyToBrain(Brain* brain) override;
    
    // Episodic memory specific operations
    void storeEpisode(const std::vector<float>& sensoryPattern, 
                     const std::vector<float>& action,
                     float reward,
                     const std::vector<float>& resultingPattern) override;
    std::vector<const MemoryItem*> retrieveSimilar(const std::vector<float>& pattern, 
                                                  size_t maxResults = 5) const override;
    std::vector<const MemoryItem*> retrieveTemporal(SimulationStep startTime,
                                                   SimulationStep endTime,
                                                   size_t maxResults = 10) const override;
    void replay(size_t index) override;
    void enableReplay(bool enable) override;
    bool isReplayEnabled() const override;
    
    // Temporal processing
    void advanceTime(SimulationStep steps) override;
    void consolidateOld(float ageThreshold) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm