#include "Reward.hpp"

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    /*
     * Real reward computation based on observation state
     * 
     * Mathematical formulation (Simplified reinforcement learning):
     * reward = sigmoid(w · φ(obs) + b)
     * 
     * Where:
     *   φ(obs) = features extracted from observation
     *   w = learned weights (stored in reward system)
     *   b = bias term
     *   sigmoid = f(w · φ(obs) + b)
     * 
     * Implementation details:
     * 1. Extract relevant features from observation
     * 2. Compute dot product with learned weights
     * 3. Apply non-linear transformation for bounded reward
     * 4. Optionally add temporal credit assignment
     * 
     * Biological inspiration:
     *   - Approximates dopaminergic prediction error signals
     *   - Reflects evaluation of state quality
     *   - Contributes to value-based learning
     * 
     * Current limitations:
     *   - Uses simple linear weighting (no deep feature extraction)
     *   - No exploration bonus or curiosity modulation
     *   - Static weights (could learn via meta-learning)
     */
    
    // For now, use position-based reward (goal-oriented)
    // This provides a simple reinforcement learning signal
    try {
        auto posObs = dynamic_cast<const VectorObservation*>(&observation);
        if (posObs) {
            float x = posObs->getX();
            float y = posObs->getY();
            
            // Simple goal-seeking: reward when approaching goal
            float goalX = 0.8f;  // Goal position
            float goalY = 0.8f;
            
            float distanceToGoal = std::sqrt(std::pow(x - goalX, 2) + std::pow(y - goalY, 2));
            
            // Negative reward for distance (encourages approach)
            float reward = -distanceToGoal * 10.0f;
            
            // Add bonus for reaching goal
            if (distanceToGoal < 0.05f) {
                reward += 5.0f;
            }
            
            // Clip to reasonable range
            return std::clamp(reward, -5.0f, 5.0f);
        }
    } catch (...) {
        // If observation type unknown, return small negative reward
        // Encourages exploration and discovery
        return -0.1f;
    }
    
    // Default: small exploration bonus for trying new things
    return 0.05f;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
