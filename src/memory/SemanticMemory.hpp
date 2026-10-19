#pragma once

#include "MemoryInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

class SemanticMemory : public Memory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
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
    
    // Semantic memory specific operations
    void learnConcept(const std::string& name, const std::vector<float>& pattern) override;
    std::string retrieveConcept(const std::vector<float>& pattern) const override;
    float getConceptStrength(const std::string& name) const override;
    void updateConceptStrength(const std::string& name, float delta) override;
    std::vector<std::string> getAllConcepts() const override;
    
    // Concept formation
    void formConceptsFromData(const std::vector<std::vector<float>>& patterns) override;
    void mergeConcepts(const std::string& conceptA, const std::string& conceptB) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm