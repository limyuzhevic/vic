#include "SensoryPercept.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    try {
        // Validate and resize arrays with bounds checking
        vision_.resize(16 * 16, 0.0f);
        
        // Validate touch array size (common small sensor count)
        if (sizeof(float) * 8 > 1024 * 1024) { // 8KB limit
            touch_.resize(0);
            NLM_LOG_WARNING("Touch array would exceed memory limit, using empty array");
        } else {
            touch_.resize(8, 0.0f);
        }
        
        // Validate internal signals array size (small homeostatic signals)
        if (sizeof(float) * 4 > 1024) { // 4KB limit
            internal_.resize(0);
            NLM_LOG_WARNING("Internal array would exceed memory limit, using empty array");
        } else {
            internal_.resize(4, 0.0f);
        }
        
        // Validate proprioception array size
        if (sizeof(float) * 6 > 1024) { // 6KB limit
            proprioception_.resize(0);
            NLM_LOG_WARNING("Proprioception array would exceed memory limit, using empty array");
        } else {
            proprioception_.resize(6, 0.0f);
        }
        
        // Audio array defaults to empty (optional)
        audio_.resize(0, 0.0f);
        
        // Validate initialization
        if (vision_.empty() || visionWidth_ * visionHeight_ != vision_.size()) {
            NLM_LOG_ERROR("Failed to initialize vision array properly");
            // Attempt recovery
            visionWidth_ = 0;
            visionHeight_ = 0;
            vision_.clear();
        }
        
        NLM_LOG_DEBUG("SensoryPercept created with vision=" + 
                     std::to_string(visionWidth_ * visionHeight_) +
                     ", touch=" + std::to_string(touch_.size()) +
                     ", internal=" + std::to_string(internal_.size()) +
                     ", proprioception=" + std::to_string(proprioception_.size()));
                     
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in SensoryPercept constructor: " + std::string(e.what()));
        // Try to recover to minimal valid state
        vision_.clear();
        visionWidth_ = 0;
        visionHeight_ = 0;
        touch_.clear();
        internal_.clear();
        proprioception_.clear();
        audio_.clear();
        timestamp_ = 0.0;
    }
}

std::vector<float> SensoryPercept::getAllSignals() const {
    try {
        std::vector<float> all;
        
        // Reserve approximate capacity to avoid reallocations
        size_t estimatedSize = vision_.size() + touch_.size() + 
                              internal_.size() + proprioception_.size() +
                              audio_.size();
        if (estimatedSize > 1000000) { // Safety limit (1M floats)
            NLM_LOG_WARNING("Large sensory data size (" + std::to_string(estimatedSize) + "), potential memory issue");
            all.reserve(estimatedSize);
        }
        
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in getAllSignals: " + std::string(e.what()));
        return std::vector<float>(); // Return empty on error
    }
}

} // namespace nlm
