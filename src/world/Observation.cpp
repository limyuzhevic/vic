#include "Observation.hpp"
#include "../agent/SensoryPercept.hpp"

namespace nlm {

Observation::Observation() {}

const std::vector<float>& Observation::getData() const {
    return data_;
}

size_t Observation::getDimensions() const {
    return data_.size();
}

void Observation::setFromPercept(const SensoryPercept& percept) {
    // Combine all sensory modalities into single vector for brain input
    // This matches getSensoryInputSize() in AgentBrain.cpp
    data_.clear();
    
    // Vision (flattened) - 16x16 = 256 values
    data_.insert(data_.end(), percept.getVision().begin(), percept.getVision().end());
    
    // Touch - 8 proximity sensor values
    data_.insert(data_.end(), percept.getTouch().begin(), percept.getTouch().end());
    
    // Internal - 4 homeostatic signal values
    data_.insert(data_.end(), percept.getInternal().begin(), percept.getInternal().end());
    
    // Proprioception - 6 body position/velocity values
    data_.insert(data_.end(), percept.getProprioception().begin(), percept.getProprioception().end());
    
    // Note: Audio signals excluded (not implemented)
}

} // namespace nlm
