#include "SensoryPercept.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : vision_(),
      visionWidth_(0),
      visionHeight_(0),
      touch_(),
      internal_(),
      proprioception_(),
      audio_(),
      timestamp_(0.0) {}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> result;
    result.reserve(vision_.size() + touch_.size() + internal_.size() +
                   proprioception_.size() + audio_.size());
    
    result.insert(result.end(), vision_.begin(), vision_.end());
    result.insert(result.end(), touch_.begin(), touch_.end());
    result.insert(result.end(), internal_.begin(), internal_.end());
    result.insert(result.end(), proprioception_.begin(), proprioception_.end());
    result.insert(result.end(), audio_.begin(), audio_.end());
    
    return result;
}

} // namespace nlm