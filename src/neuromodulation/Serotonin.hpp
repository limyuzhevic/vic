#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Apply attention effects (serotonin modulates impulsivity and decision time)
    void applyAttentionEffect(Brain* brain) override;
    
    // Apply memory effects (serotonin influences social memory and context)
    void applyMemoryEffect(Brain* brain) override;
    
    // Apply arousal effects (serotonin regulates mood and behavioral inhibition)
    void applyArousalEffect(Brain* brain) override;
    
    // Modulate social behavior
    void modulateSocialBehavior(Brain* brain, float socialSalience);
    
    // Enhance associative memory for social patterns
    void enhanceSocialMemory(const std::vector<float>& pattern, float socialValue);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
