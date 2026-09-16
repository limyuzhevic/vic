#pragma once

#include "Action.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * MotorSystem: Converts neural activity to actions
 * 
 * This class implements the final layer of the NLM brain architecture,
 * bridging neural computation to behavioral output. It integrates multiple
 * approaches to action selection:
 * 
 * 1. Neural-driven: Direct mapping from motor neuron activity
 * 2. Preference-based: Weighted action selection based on learned preferences
 * 3. Exploration-based: Curiosity-driven random exploration
 * 
 * The system maintains action history and preferences to enable learning
 * and adaptive behavior over time.
 */

class MotorSystem {
public:
    MotorSystem();
    ~MotorSystem();
    
    /**
     * Select action based on neural activity
     * 
     * @param motorRegion Neural region containing motor neurons
     * @return Unique action selected based on neural patterns and preferences
     */
    std::unique_ptr<Action> selectAction(const NeuralRegion& motorRegion);
    
    /**
     * Get available action types for this agent
     * 
     * @return Vector of available action types
     */
    const std::vector<ActionType>& getAvailableActions() const;
    
    /**
     * Set preference for an action type
     * 
     * Actions with higher preference are more likely to be selected.
     * Preference values affect the weight of each action in selection.
     * 
     * @param action Action type to set preference for
     * @param preference Preference value (higher = more likely to be selected)
     */
    void setActionPreference(ActionType action, float preference);
    
    /**
     * Get history of recently selected actions
     * 
     * @return Vector of pointers to action objects (caller owns these)
     */
    const std::vector<const Action*>& getActionHistory() const;
    
    /**
     * Clear action history
     * 
     * Frees memory held by historical actions and resets the history.
     */
    void clearHistory();
    
private:
    /**
     * Internal implementation structure (Pimpl pattern)
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
