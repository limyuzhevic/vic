#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

// AbstractCognitiveStrategy: Base interface for all cognitive processing
// Provides common functionality for cognitive architectures
// Allows different implementations (NeuralPlanner, ConceptFormation, AttentionalSelection)
// to be used interchangeably through strategy pattern

class AbstractCognitiveStrategy {
public:
    virtual ~AbstractCognitiveStrategy() = default;

    // Initialize with brain reference for system coordination
    virtual void initialize(Brain* brain) = 0;

    // Common cognitive planning interface
    // Can be implemented differently for each strategy type
    virtual void planAction(const std::vector<float>& currentState, float targetReward = 0.5f) = 0;

    // Common cognitive input processing interface
    // Used by different strategies for input interpretation
    virtual void processInput(const std::vector<float>& input) = 0;

    // Common cognitive update interface
    // Called every simulation timestep for strategy maintenance
    virtual void update(TimestepDuration dt) = 0;

    // Get strategy name/type for debugging and logging
    virtual const char* getName() const = 0;

    // Get current strategy status
    virtual std::string getStatus() const = 0;

    // Enable/disable strategy functionality
    virtual void enable(bool enable) = 0;
    virtual bool isEnabled() const = 0;

    // Reset strategy state
    virtual void reset() = 0;

protected:
    AbstractCognitiveStrategy() = default;
};

} // namespace nlm
