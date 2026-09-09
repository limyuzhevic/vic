#ifndef NLM_SENSORYPERCEPT_HPP
#define NLM_SENSORYPERCEPT_HPP

#include <vector>
#include <string>

namespace nlm {

struct SensoryPercept {
    // Vision data (16x16 grid)
    std::vector<float> vision;
    
    // Touch/somatosensory data
    std::vector<float> touch;
    
    // Internal signals (e.g., hunger, arousal)
    std::vector<float> internal;
    
    // Proprioception (body position)
    std::vector<float> proprioception;
    
    // Constructor
    SensoryPercept() = default;
    
    // Helper methods
    size_t getVisionSize() const { return vision.size(); }
    size_t getTouchSize() const { return touch.size(); }
    size_t getInternalSize() const { return internal.size(); }
    size_t getProprioceptionSize() const { return proprioception.size(); }
    
    // Total sensory input size
    size_t getTotalSize() const {
        return vision.size() + touch.size() + internal.size() + proprioception.size();
    }
};

} // namespace nlm

#endif // NLM_SENSORYPERCEPT_HPP