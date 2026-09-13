#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Novelty detection signal
//
// Computes novelty from comparison with previous observations
//
// Implementation:
// - Computes difference between current and previous observation patterns
// - Normalizes by pattern size and threshold
// - Decays over time to prevent unbounded growth
//
// Usage:
// - Call detectNovelty() with consecutive observations or patterns
// - Call update() with timestep to apply decay
// - Get current novelty level with getLevel()
//
// Configuration:
// - decayRate: Rate of decay per timestep (default: 0.1)
// - noveltyThreshold: Threshold for considering change as novel (default: 0.3)
class Novelty {
public:
    /**
     * @brief Construct a new Novelty object
     */
    Novelty();
    
    /**
     * @brief Destroy the Novelty object
     */
    ~Novelty();
    
    /**
     * @brief Initialize the novelty detector with a brain reference
     * 
     * @param brain Pointer to the brain object (must not be null)
     * @note This should be called after the brain is initialized
     */
    void initialize(class Brain* brain);
    
    /**
     * @brief Get the current novelty level
     * 
     * @return float Current novelty level (0.0 to 1.0)
     */
    float getLevel() const;
    
    /**
     * @brief Set novelty level directly (for testing or special use cases)
     * 
     * @param level Novelty level to set (0.0 to 1.0)
     */
    void setLevel(float level);
    
    /**
     * @brief Detect novelty from observation structures
     * 
     * @param observation Current observation
     * @param previousObservation Previous observation
     * @note This is a placeholder implementation that returns a constant value
     */
    void detectNovelty(const class Observation& observation, 
                       const class Observation& previousObservation);
    
    /**
     * @brief Detect novelty from raw feature patterns
     * 
     * @param currentPattern Current feature pattern (normalized 0.0 to 1.0)
     * @param previousPattern Previous feature pattern (normalized 0.0 to 1.0)
     */
    void detectNovelty(const std::vector<float>& currentPattern,
                       const std::vector<float>& previousPattern);
    
    /**
     * @brief Decay novelty over time
     * 
     * @param dt Timestep duration
     */
    void update(TimestepDuration dt);
    
    /**
     * @brief Get novelty history
     * 
     * @return const std::vector<float>& History of novelty levels
     */
    const std::vector<float>& getHistory() const;
    
    /**
     * @brief Clear novelty history
     */
    void clearHistory();
    
private:
    /**
     * @brief Pimpl pattern implementation details
     */
    struct Impl;
    
    /**
     * @brief Pointer to implementation
     */
    Impl* pImpl;
};

} // namespace nlm
