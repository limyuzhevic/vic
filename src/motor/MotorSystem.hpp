#pragma once

#include "Action.hpp"
#include <memory>
#include <vector>
#include <unordered_map>

namespace nlm {

// Motor system: converts neural activity to actions
// Implements action selection with exploration-exploitation tradeoff

class MotorSystem {
public:
    MotorSystem();
    ~MotorSystem();
    
    // Select action based on neural activity
    // TODO PHASE 2: Implement real action selection
    std::unique_ptr<Action> selectAction(const class NeuralRegion& motorRegion);
    
    // Select action with input preprocessing
    std::unique_ptr<Action> selectActionWithPreprocessing(const class NeuralRegion& motorRegion);
    
    // Get available actions
    const std::vector<ActionType>& getAvailableActions() const;
    
    // Set action preferences (for exploration)
    void setActionPreference(ActionType action, float preference);
    
    // Get action history
    const std::vector<const Action*>& getActionHistory() const;
    void clearHistory();
    
    // Get action selection statistics
    struct SelectionStats {
        std::unordered_map<ActionType, int> selectionCounts;
        int totalSelections = 0;
        float explorationRate = 0.1f;
        float exploitationRate = 0.9f;
        float temperature = 1.0f; // For softmax selection
    };
    
    SelectionStats getSelectionStats() const;
    
    // Reset statistics
    void resetStats();
    
    // Performance monitoring
    float getActionSuccessRate(ActionType action) const;
    void recordActionOutcome(ActionType action, bool success);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
