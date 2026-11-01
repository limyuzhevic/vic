#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <string>

namespace nlm {

float Reward::computeReward(const Observation& observation) const {
    float reward = 0.0f;
    
    // Get different observation types if available
    const auto& vision = observation.getVision();
    const auto& touch = observation.getTouch();
    const auto& internal = observation.getInternal();
    
    // TODO: Implement proper observation-based reward computation
    // For now, return a simple baseline to test the system
    
    // Basic resource finding reward (based on world state)
    // In SimpleWorld, resources are +10, hazards are -15
    // We'll simulate this by checking vision for bright objects
    if (!vision.empty()) {
        // Bright objects in vision indicate resources
        float brightObjects = 0.0f;
        for (float v : vision) {
            if (v > 0.7f) {  // Bright object detected
                brightObjects++;
            }
        }
        reward += brightObjects * 5.0f;  // Small reward for detecting resources
    }
    
    // Touch-based reward signals
    if (!touch.empty()) {
        float maxTouch = *std::max_element(touch.begin(), touch.end());
        if (maxTouch > 0.3f) {  // Object within touch range
            if (maxTouch > 0.6f) {  // Bright object (likely resource)
                reward += 10.0f;  // Resource interaction
            } else {  // Dark object (likely hazard)
                reward -= 5.0f;   // Hazard interaction
            }
        }
    }
    
    // Internal state rewards
    if (!internal.empty() && internal.size() >= 2) {
        float energy = internal[0];  // Energy level
        float health = internal[1];   // Health level
        
        // Positive reward for high energy (avoid energy decay)
        if (energy > 0.7f) {
            reward += 0.5f;
        }
        
        // Negative reward for low health
        if (health < 0.5f) {
            reward -= 2.0f;
        }
    }
    
    // Novelty-based reward (curiosity bonus)
    float novelty = getNoveltyFromObservation(observation);
    reward += novelty * 0.1f;  // Small curiosity bonus
    
    // Apply homeostatic scaling to prevent runaway rewards
    // Keep rewards in reasonable range
    reward = std::clamp(reward, -20.0f, 20.0f);
    
    // Record reward in history
    pImpl->history.push_back(reward);
    
    // Keep history manageable
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    return reward;
}

float Reward::getNoveltyFromObservation(const Observation& observation) const {
    const auto& vision = observation.getVision();
    const auto& touch = observation.getTouch();
    const auto& internal = observation.getInternal();
    
    float novelty = 0.0f;
    
    // Vision novelty: change from previous state would be ideal
    // For simplicity, use entropy of current vision
    if (!vision.empty()) {
        std::unordered_map<float, int> intensityCount;
        for (float v : vision) {
            // Bin intensities into 10 bins
            int bin = static_cast<int>(v * 10.0f) % 10;
            intensityCount[bin]++;
        }
        
        float entropy = 0.0f;
        for (const auto& pair : intensityCount) {
            float p = static_cast<float>(pair.second) / vision.size();
            if (p > 0.0f) {
                entropy -= p * std::log(p);
            }
        }
        
        // Normalize to [0, 1]
        novelty += entropy / std::log(10.0f);
    }
    
    // Touch novelty
    if (!touch.empty()) {
        float touchEntropy = 0.0f;
        std::unordered_map<float, int> touchCount;
        for (float t : touch) {
            int bin = static_cast<int>(t * 10.0f) % 10;
            touchCount[bin]++;
        }
        
        for (const auto& pair : touchCount) {
            float p = static_cast<float>(pair.second) / touch.size();
            if (p > 0.0f) {
                touchEntropy -= p * std::log(p);
            }
        }
        
        novelty += touchEntropy / std::log(10.0f) * 0.5f;
    }
    
    return std::min(novelty, 1.0f);
}

} // namespace nlm
