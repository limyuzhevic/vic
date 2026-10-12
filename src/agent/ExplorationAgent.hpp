#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../core/Config/Config.hpp"
#include <vector>
#include <memory>
#include <algorithm>

namespace nlm {

// Forward declarations
class Brain;

// ExplorationAgent: Handles curiosity-based exploration
// Implements novelty detection and curiosity-driven behavior
class ExplorationAgent {
public:
    ExplorationAgent(std::shared_ptr<Config> config);
    ~ExplorationAgent();
    
    // Configure with brain reference
    void initialize(std::shared_ptr<Brain> brain);
    
    // Update curiosity and novelty levels
    void update(double timestep, float noveltyLevel);
    
    // Get curiosity level
    float getCuriosityLevel() const { return curiosityLevel_; }
    
    // Get novelty level
    float getNoveltyLevel() const { return noveltyLevel_; }
    
    // Check if should explore based on curiosity
    bool shouldExplore(float curiosityThreshold) const;
    
    // Generate exploration action
    MotorCommand generateExplorationAction();
    
    // Enable/disable curiosity
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Reset exploration state
    void reset();
    
private:
    // Configuration
    std::shared_ptr<Config> config_;
    
    // Brain reference
    std::shared_ptr<Brain> brain_;
    
    // Exploration state
    float curiosityLevel_;
    float noveltyLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Configuration flags
    bool curiosityEnabled_;
    
    // Generate random action from available motor commands
    MotorCommand generateRandomAction();
};

} // namespace nlm
