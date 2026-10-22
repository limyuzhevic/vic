#pragma once

#include <string>

namespace nlm {

/**
 * Action result from world interaction
 * 
 * Represents the outcome of an agent's action in the environment,
 * including the reward received and whether the action was successful.
 */
struct ActionResult {
    float reward;           ///< Reward value (positive for good, negative for bad)
    bool success;          ///< Whether action succeeded
    std::string message;   ///< Human-readable message explaining result
    
    /**
     * Default constructor
     * Creates an invalid action result with zero reward and failed status
     */
    ActionResult() : reward(0.0f), success(false), message("") {}
    
    /**
     * Constructor with reward and success status
     * 
     * @param r Reward value
     * @param s Success status
     * @param msg Optional human-readable message
     */
    ActionResult(float r, bool s, const std::string& msg = "") 
        : reward(r), success(s), message(msg) {}
    
    /**
     * Validate action result
     * 
     * @return True if action is valid (successful or zero reward), false otherwise
     */
    bool isValid() const {
        return success || reward == 0.0f;
    }
};

} // namespace nlm

