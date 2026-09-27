#include "SensoryPercept.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    // Initialize sensory buffers with default sizes and zero values
    vision_.resize(16 * 16, 0.0f);      // 16x16 vision grid
    touch_.resize(8, 0.0f);            // 8 touch/proximity sensors
    internal_.resize(4, 0.0f);         // 4 internal homeostatic signals
    proprioception_.resize(6, 0.0f);  // 6 body state signals
    audio_.resize(0, 0.0f);            // No audio signals by default
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> allSignals;
    
    // Reserve space for efficiency
    size_t totalSize = vision_.size() + touch_.size() + internal_.size() + 
                      proprioception_.size() + audio_.size();
    allSignals.reserve(totalSize);
    
    // Vision (flattened)
    allSignals.insert(allSignals.end(), vision_.begin(), vision_.end());
    
    // Touch
    allSignals.insert(allSignals.end(), touch_.begin(), touch_.end());
    
    // Internal
    allSignals.insert(allSignals.end(), internal_.begin(), internal_.end());
    
    // Proprioception
    allSignals.insert(allSignals.end(), proprioception_.begin(), proprioception_.end());
    
    // Audio (if enabled)
    allSignals.insert(allSignals.end(), audio_.begin(), audio_.end());
    
    return allSignals;
}

/**
 * @brief Equality comparison operator implementation
 * 
 * Compares all sensory data for equality. Note that timestamps are not
 * included in the comparison since they may differ for percepts with
 * identical content generated at different times.
 * 
 * @param other The other SensoryPercept to compare against
 * @return True if all sensory data vectors are identical
 */
bool SensoryPercept::operator==(const SensoryPercept& other) const {
    return vision_ == other.vision_ &&
           touch_ == other.touch_ &&
           internal_ == other.internal_ &&
           proprioception_ == other.proprioception_ &&
           audio_ == other.audio_;
}

} // namespace nlm
