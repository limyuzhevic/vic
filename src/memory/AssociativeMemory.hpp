#pragma once

#include "MemoryInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <queue>

namespace nlm {

class AssociativeMemory : public Memory {
public:
    AssociativeMemory();
    ~AssociativeMemory();
    
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
    
    // Associative memory specific operations
    void createAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float strength = 1.0f) override;
    std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cue,
                                                   size_t steps = 2) const override;
    float getAssociationStrength(const std::vector<float>& patternA,
                                const std::vector<float>& patternB) const override;
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float delta) override;
    void clearAssociations() override;
    
    // Pattern completion
    std::vector<float> completePattern(const std::vector<float>& partialPattern) const override;
    bool isPatternAssociated(const std::vector<float>& pattern) const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm