#pragma once

#include "../brain/Neuron.hpp"
#include <vector>
#include <cmath>

namespace nlm {

class CuriosityManager {
public:
    CuriosityManager();
    ~CuriosityManager();
    
    // Update curiosity level based on novelty and prediction error
    float updateCuriosityLevel(float novelty, float predictionError,
                              float decayFactor = 2.0f, float errorWeight = 0.5f);
    
    // Apply curiosity-based exploration
    bool shouldExplore(float curiosityLevel, float threshold = 0.3f);
    
    // Select action with curiosity (exploration)
    int selectExplorationAction(float curiosityLevel);
    
    // Get current curiosity level
    float getCuriosityLevel() const { return curiosityLevel_; }
    
    // Reset curiosity
    void reset() { curiosityLevel_ = 0.0f; }
    
    // Configuration
    void setCuriosityEnabled(bool enabled) { curiosityEnabled_ = enabled; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    void setExplorationChance(float chance) { explorationChance_ = chance; }
    float getExplorationChance() const { return explorationChance_; }

private:
    float curiosityLevel_;
    bool curiosityEnabled_;
    float explorationChance_;
};

} // namespace nlm