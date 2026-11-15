#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Implemented: Enhances memory consolidation and focuses neural resources
class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Signal attention or salience events
    void signalAttention(float salience);
    
    // Memory consolidation effects
    float getConsolidationRate() const;
    void setConsolidationRate(float rate);
    
    // History tracking
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
