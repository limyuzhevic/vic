#include "SensoryPercept.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    vision_.resize(16 * 16, 0.0f);
    touch_.resize(8, 0.0f);
    internal_.resize(4, 0.0f);
    proprioception_.resize(6, 0.0f);
    audio_.resize(0, 0.0f);
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    
    try {
        // Vision (flattened) - safely copy all elements
        all.insert(all.end(), vision_.begin(), vision_.end());
        
        // Touch - safely copy all elements  
        all.insert(all.end(), touch_.begin(), touch_.end());
        
        // Internal - safely copy all elements
        all.insert(all.end(), internal_.begin(), internal_.end());
        
        // Proprioception - safely copy all elements
        all.insert(all.end(), proprioception_.begin(), proprioception_.end());
        
        // Audio - safely copy all elements
        all.insert(all.end(), audio_.begin(), audio_.end());
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error copying sensory signals: " + std::string(e.what()));
        all.clear();
    }
    
    return all;
}

} // namespace nlm
