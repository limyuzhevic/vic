#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Attention modulation
    void enhanceAttention(float strength);
    void modulateWorkingMemory(float factor);
    void facilitateMemoryConsolidation();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm