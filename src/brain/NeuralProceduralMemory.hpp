#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <unordered_map>

namespace nlm {

// ActionPattern: Neural pattern representing an action
struct ActionPattern {
    // The neural activity pattern for this action
    std::vector<float> pattern;
    
    // When this action was learned
    SimulationStep learnedAt;
    
    // How successful this action has been (0-1)
    float successRate;
    
    // How many times this action has been tried
    size_t trialCount;
    
    // Associated reward prediction
    float rewardPrediction;
    
    ActionPattern()
        : learnedAt(0), successRate(0.0f), trialCount(0), rewardPrediction(0.0f) {}
};

// ProceduralMemory: Skills and habits learned from experience
// Encodes sequences of actions and their expected outcomes
//
// Key mechanisms:
// - Actions are learned through trial and error
// - Success reinforces action patterns
// - Failure weakens or modifies action patterns
// - Actions can be retrieved and executed

class NeuralProceduralMemory {
public:
    NeuralProceduralMemory();
    ~NeuralProceduralMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Learn an action from experience
    // Returns true if action was learned or updated
    bool learnAction(const std::string& actionName,
                    const std::vector<float>& pattern,
                    float reward,
                    SimulationStep currentTime);

    // Get action by name
    ActionPattern* getAction(const std::string& actionName);

    // Get all actions
    const std::vector<ActionPattern>& getAllActions() const { return actions_; }

    // Get action by index
    ActionPattern* getAction(size_t index);

    // Get number of actions
    size_t getActionCount() const { return actions_.size(); }

    // Get best action for a given state
    std::string getBestAction(const std::vector<float>& state) const;

    // Execute action - updates success based on actual outcome
    void executeAction(const std::string& actionName, float outcome);

    // Get action success rate
    float getActionSuccessRate(const std::string& actionName) const;

    // Clear all actions
    void clear();

private:
    // Find or create action for pattern
    ActionPattern* findOrCreateAction(const std::string& actionName,
                                      const std::vector<float>& pattern,
                                      float reward);

    // Compute similarity between patterns
    float computePatternSimilarity(const std::vector<float>& patternA,
                                   const ActionPattern& actionB) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<ActionPattern> actions_;
    
    // Quick lookup by name
    std::unordered_map<std::string, size_t> actionNameToIndex_;
};

} // namespace nlm
