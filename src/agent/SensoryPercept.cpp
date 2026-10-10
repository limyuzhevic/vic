#include "SensoryPercept.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
    , vision_(16 * 16, 0.0f)
    , touch_(8, 0.0f)
    , internal_(4, 0.0f)
    , proprioception_(6, 0.0f)
    , audio_()  // Empty vector - initialized directly
{
    // All vectors are now properly initialized with their final values
    // in the initializer list, which is more efficient than resizing in the constructor body
}

SensoryPercept::~SensoryPercept() = default;

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    all.reserve(
        vision_.size() + touch_.size() + internal_.size() + 
        proprioception_.size() + audio_.size()
    );
    
    // Vision (flattened)
    all.insert(all.end(), vision_.begin(), vision_.end());
    
    // Touch
    all.insert(all.end(), touch_.begin(), touch_.end());
    
    // Internal
    all.insert(all.end(), internal_.begin(), internal_.end());
    
    // Proprioception
    all.insert(all.end(), proprioception_.begin(), proprioception_.end());
    
    // Audio
    all.insert(all.end(), audio_.begin(), audio_.end());
    
    return all;
}

} // namespace nlm
