#pragma once

#include "MemoryInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

class WorkingMemory : public Memory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
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
    
    // Working memory specific operations
    void addTrace(const std::vector<float>& trace, float initialStrength = 1.0f) override;
    void removeTrace(size_t index) override;
    void updateTrace(size_t index, const std::vector<float>& newPattern) override;
    std::vector<size_t> getActiveTraces() const override;
    void setDecayRate(float rate) override;
    void setCapacity(size_t capacity) override;
    
    // Competition and selection
    void runCompetition() override;
    std::vector<size_t> getWinners() const override;
    void strengthenWinners(float factor) override;
    void inhibitLosers(float factor) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm