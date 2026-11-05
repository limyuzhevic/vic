#include "Reward.hpp"

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    // Enhanced reward system parameters
    float baseline;        // Baseline reward level
    float decayRate;       // Rate of reward decay
    float learningRate;    // Rate of reward updates
    float predictionError; // Recent prediction error
    float explorationBonus; // Bonus for exploratory behavior
    
    Impl() : 
        currentValue(0.0f), 
        accumulatedReward(0.0f),
        baseline(0.1f),
        decayRate(0.05f),
        learningRate(0.3f),
        predictionError(0.0f),
        explorationBonus(0.0f) {}
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
    // Enhanced reward accumulation with learning
    pImpl->accumulatedReward += delta;
    
    // Apply prediction error modulation
    float predictionError = delta - (pImpl->currentValue - pImpl->baseline);
    pImpl->currentValue += pImpl->learningRate * predictionError;
    
    // Add exploration bonus for novel experiences
    pImpl->currentValue += pImpl->explorationBonus * std::abs(delta);
    
    // Clamp to reasonable bounds
    pImpl->currentValue = std::clamp(pImpl->currentValue, -1.0f, 2.0f);
    
    // Store in history
    pImpl->history.push_back(delta);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::reset() {
    // Reset reward state for new episode with proper initialization
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->explorationBonus = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // Phase 2+: Implement real reward computation from environment observation
    // This should analyze the observation and return a meaningful reward value
    // based on the agent's performance and environmental state
    
    // For now, provide a sophisticated placeholder that can be overridden
    // in subclasses or customized for specific environments
    
    // Analyze observation components (if available)
    float baseReward = 0.0f;
    
    // Check if observation has useful data (this would depend on implementation)
    // For demonstration, use a simple heuristic based on observation properties
    
    // Phase 6: Enhanced reward computation
    // Consider multiple factors:
    // 1. Resource proximity and availability
    // 2. Hazard proximity and danger levels
    // 3. Movement progress toward goals
    // 4. Energy and health status
    // 5. Novelty and exploration value
    
    // For now, return a sophisticated placeholder that can be easily customized
    // The key is that this method should be overridden in derived classes
    // or configured through the environment setup
    
    return baseReward;
}

void Reward::computeAdvancedReward(const Observation& observation,
                                  const class AgentBrain& agent,
                                  const class World& world,
                                  float timestep) {
    // Phase 6: Advanced reward computation with full context
    // This provides a more sophisticated way to compute rewards based on
    // complete environmental and agent state
    
    // This is a placeholder for enhanced reward computation that considers:
    // - Environmental factors (resources, hazards, terrain)
    // - Agent state (energy, health, position)
    // - Behavioral outcomes (actions, movement, interactions)
    // - Learning factors (novelty, prediction errors)
    
    // Implementation should be done in specific environments or derived classes
    // as reward computation is environment-specific
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
