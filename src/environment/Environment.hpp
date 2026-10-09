#pragma once

#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "RealEnvironment.hpp"
#include <memory>
#include <string>
#include "EnvironmentStateManager.hpp"

namespace nlm {

// Environment state - now using EnvironmentStateManager
class EnvironmentState {
public:
    EnvironmentState() = default;
    ~EnvironmentState() = default;
    
    // Initialize with environmental conditions
    void initialize(const EnvironmentalConditions& conditions) {
        if (stateManager_) {
            stateManager_->initialize(conditions);
        }
    }
    
    // Get state as dictionary-like structure
    bool is_terminal() const {
        return stateManager_ && stateManager_->getCurrentConditions().dayNightCycle < 0.1f;
    }
    
    void set_terminal(bool terminal) {
        if (stateManager_) {
            EnvironmentalConditions conditions = stateManager_->getCurrentConditions();
            // Modify time of day to simulate end of episode
            conditions.dayNightCycle = terminal ? 0.0f : 1.0f;
            stateManager_->initialize(conditions);
        }
    }
    
    // Get environmental conditions
    const EnvironmentalConditions& getEnvironmentalConditions() const {
        return stateManager_->getCurrentConditions();
    }
    
    // Get feedback
    const EnvironmentalFeedback& getFeedback() const {
        return stateManager_->getFeedback();
    }
    
private:
    std::unique_ptr<EnvironmentStateManager> stateManager_;
};

// Real environment implementation for Phase 4-6
// Replaces placeholder environments with comprehensive simulation
class RealEnvironmentWrapper : public Environment {
public:
    RealEnvironmentWrapper(float width = 20.0f, float height = 20.0f);
    ~RealEnvironmentWrapper() override;
    
    // Reset environment to initial state
    std::unique_ptr<SensoryInput> reset() override;
    
    // Take action and get observation
    std::unique_ptr<SensoryInput> step(const Action& action) override;
    
    // Get current state
    const EnvironmentState& getState() const override;
    
    // Get reward for last action
    float getLastReward() const override;
    
    // Check if episode is done
    bool isDone() const override;
    
    // Get environment name
    const char* getName() const override;
    
    // Get action space
    size_t getActionSpaceSize() const override;
    
    // Get observation space
    size_t getObservationSpaceSize() const override;
    
    // Get underlying real environment for advanced access
    RealEnvironment* getRealEnvironment() const {
        return realEnv_.get();
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Legacy GridWorldEnvironment - kept for backward compatibility
// but now uses RealEnvironment internally
class GridWorldEnvironment : public Environment {
public:
    GridWorldEnvironment(int width = 10, int height = 10);
    ~GridWorldEnvironment() override;
    
    std::unique_ptr<SensoryInput> reset() override;
    std::unique_ptr<SensoryInput> step(const Action& action) override;
    const EnvironmentState& getState() const override;
    float getLastReward() const override;
    bool isDone() const override;
    const char* getName() const override;
    size_t getActionSpaceSize() const override;
    size_t getObservationSpaceSize() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Alias for backward compatibility
using EnvironmentBase = Environment;

} // namespace nlm
