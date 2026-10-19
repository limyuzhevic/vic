#pragma once

#include "MemoryInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

class ProceduralMemory : public Memory {
public:
    ProceduralMemory();
    ~ProceduralMemory();
    
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
    
    // Procedural memory specific operations
    void learnSkill(const std::string& name, const std::vector<float>& pattern) override;
    std::vector<float> executeSkill(const std::string& name) override;
    float getSkillProficiency(const std::string& name) const override;
    void updateSkillProficiency(const std::string& name, float delta) override;
    std::vector<std::string> getAllSkills() const override;
    
    // Skill composition
    void sequenceSkills(const std::vector<std::string>& skillSequence) override;
    void interruptSkill(const std::string& name) override;
    void pauseSkill(const std::string& name) override;
    void resumeSkill(const std::string& name) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm