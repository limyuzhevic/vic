#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>

namespace nlm {

// Norepinephrine: Arousal and vigilance
class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    void initialize();
    
    // Get current level
    float getLevel() const;
    
    // Set level
    void setLevel(float level);
    
    // Update state
    void update(double dt);
    
    // Enhance arousal
    float enhanceArousal() const;
    
    // Increase vigilance
    float increaseVigilance() const;
    
    // Boost working memory
    float boostWorkingMemory() const;
    
    // Clear state
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    float arousal_;
    float vigilance_;
    float workingMemoryBoost_;
    float baseline_;
};

} // namespace nlm
