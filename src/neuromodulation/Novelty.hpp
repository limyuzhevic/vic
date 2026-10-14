#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

/**
 * @class Novelty
 * @brief Neuromodulator for detecting and tracking novel stimuli
 * 
 * Novelty detection computes the difference between current and previous observations
 * to identify new or unexpected patterns in the environment. This signal drives
 * curiosity and learning processes.
 * 
 * @details Maintains a history of detected novelty levels and provides methods
 * for extracting features from sensory inputs. Uses a sliding window approach
 * for pattern comparison.
 */
class Novelty {
public:
    Novelty();
    ~Novelty();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get novelty level
    float getLevel() const;
    void setLevel(float level);
    
    // Detect novelty from observation
    void detectNovelty(const class Observation& observation, 
                       const class Observation& previousObservation);
    
    // Detect novelty from sensory input pattern
    void detectNovelty(const std::vector<float>& currentPattern,
                       const std::vector<float>& previousPattern);
    
    // Decay novelty over time
    void update(TimestepDuration dt);
    
    // Get novelty history
    const std::vector<float>& getHistory() const;
    void clearHistory();

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
