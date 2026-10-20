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
    // TODO PHASE 2: Implement real reward computation from observation
    // PLACEHOLDER: Returns 0
    
    // Enhanced reward computation that integrates with observations:
    
    float computedReward = 0.0f;
    
    // 1. Primary reward signal from observation
    computedReward += computePrimaryReward(observation);
    
    // 2. Secondary reward signals
    computedReward += computeSecondaryRewards(observation);
    
    // 3. Contextual reward modulation
    computedReward = modulateRewardByContext(computedReward, observation);
    
    // 4. Time-based reward decay
    computedReward = applyTemporalDecay(computedReward);
    
    // 5. Store in history
    pImpl->history.push_back(computedReward);
    
    // Update current value
    pImpl->currentValue = computedReward;
    
    return computedReward;
}

float Reward::computePrimaryReward(const Observation& observation) const {
    // Compute primary reward based on observation
    const auto& sensoryData = observation.getSensoryData();
    const auto& internalState = observation.getInternalState();
    
    float primaryReward = 0.0f;
    
    // Vision-based reward (from VisionProcessor)
    if (sensoryData.find("vision") != sensoryData.end()) {
        const auto& visionData = sensoryData.at("vision");
        primaryReward += computeVisionBasedReward(visionData);
    }
    
    // Internal state rewards
    if (sensoryData.find("internal") != sensoryData.end()) {
        const auto& internalData = sensoryData.at("internal");
        primaryReward += computeInternalStateReward(internalData);
    }
    
    // External environment rewards
    if (sensoryData.find("external") != sensoryData.end()) {
        const auto& externalData = sensoryData.at("external");
        primaryReward += computeExternalEnvironmentReward(externalData);
    }
    
    return primaryReward;
}

float Reward::computeVisionBasedReward(const std::map<std::string, float>& visionData) const {
    // Reward based on visual processing results
    float reward = 0.0f;
    
    if (visionData.find("edges") != visionData.end()) {
        float edgeStrength = visionData.at("edges");
        reward += edgeStrength * 0.1f;  // Edge detection reward
    }
    
    if (visionData.find("features") != visionData.end()) {
        float featureComplexity = visionData.at("features");
        reward += featureComplexity * 0.2f;  // Feature complexity reward
    }
    
    if (visionData.find("salience") != visionData.end()) {
        float visualSalience = visionData.at("salience");
        reward += visualSalience * 0.3f;  // Visual salience reward
    }
    
    return reward;
}

float Reward::computeInternalStateReward(const std::map<std::string, float>& internalData) const {
    // Reward based on internal neural state
    float reward = 0.0f;
    
    if (internalData.find("excitatoryActivity") != internalData.end()) {
        float excActivity = internalData.at("excitatoryActivity");
        // Reward moderate levels of activity (avoid saturation)
        if (excActivity > 0.1f && excActivity < 0.9f) {
            reward += 0.5f * (1.0f - std::abs(excActivity - 0.5f) * 2.0f);
        }
    }
    
    if (internalData.find("adaptation") != internalData.end()) {
        float adaptation = internalData.at("adaptation");
        // Reward proper adaptation
        reward += std::min(adaptation, 1.0f) * 0.3f;
    }
    
    return reward;
}

float Reward::computeExternalEnvironmentReward(const std::map<std::string, float>& externalData) const {
    // Reward based on external environment state
    float reward = 0.0f;
    
    if (externalData.find("goalReached") != externalData.end()) {
        bool goalReached = externalData.at("goalReached") > 0.5f;
        if (goalReached) {
            reward += 2.0f; // High reward for reaching goals
        }
    }
    
    if (externalData.find("hazardPresent") != externalData.end()) {
        float hazard = externalData.at("hazardPresent");
        reward -= hazard * 1.0f; // Penalty for hazards
    }
    
    return reward;
}

float Reward::computeSecondaryRewards(const Observation& observation) const {
    // Compute secondary reward signals
    float secondaryReward = 0.0f;
    
    // Exploration bonus
    secondaryReward += computeExplorationReward(observation);
    
    // Novelty detection reward
    secondaryReward += computeNoveltyReward(observation);
    
    // Curiosity reward
    secondaryReward += computeCuriosityReward(observation);
    
    return secondaryReward;
}

float Reward::computeExplorationReward(const Observation& observation) const {
    // Reward for exploring new areas
    static std::map<std::string, std::set<std::string>> visitedStates;
    static bool initialized = false;
    
    if (!initialized) {
        visitedStates.clear();
        initialized = true;
    }
    
    // Extract state identifier from observation
    std::string stateId = generateStateIdentifier(observation);
    
    float reward = 0.0f;
    if (visitedStates.find(stateId) == visitedStates.end()) {
        // First visit to this state
        visitedStates[stateId] = std::set<std::string>();
        reward = 0.5f; // Exploration bonus
    }
    
    return reward;
}

float Reward::computeNoveltyReward(const Observation& observation) const {
    // Reward for novel or unexpected observations
    const auto& sensoryData = observation.getSensoryData();
    
    float novelty = 0.0f;
    
    // Compute statistical novelty
    if (sensoryData.find("vision") != sensoryData.end()) {
        const auto& visionData = sensoryData.at("vision");
        if (visionData.find("features") != visionData.end()) {
            float featureComplexity = visionData.at("features");
            // Novelty is high for unusual feature distributions
            novelty = std::min(featureComplexity, 1.0f);
        }
    }
    
    return novelty * 0.3f;
}

float Reward::computeCuriosityReward(const Observation& observation) const {
    // Intrinsic curiosity reward based on prediction errors
    // This would be implemented in the prediction system
    return 0.0f; // Placeholder - will be enhanced with prediction system
}

std::string Reward::generateStateIdentifier(const Observation& observation) const {
    // Generate a simple state identifier from observation
    const auto& sensoryData = observation.getSensoryData();
    
    // Combine key features to create a state signature
    std::stringstream ss;
    
    if (sensoryData.find("vision") != sensoryData.end()) {
        const auto& visionData = sensoryData.at("vision");
        if (visionData.find("edges") != visionData.end()) {
            ss << "edges:" << visionData.at("edges") << ";";
        }
    }
    
    if (sensoryData.find("internal") != sensoryData.end()) {
        const auto& internalData = sensoryData.at("internal");
        if (internalData.find("excitatoryActivity") != internalData.end()) {
            ss << "excit:" << internalData.at("excitatoryActivity") << ";";
        }
    }
    
    return ss.str();
}

float Reward::modulateRewardByContext(float reward, const Observation& observation) const {
    // Modulate reward based on contextual factors
    const auto& sensoryData = observation.getSensoryData();
    
    // Time-based modulation
    float timeModulation = 1.0f;
    if (sensoryData.find("timeSinceLastReward") != sensoryData.end()) {
        float timeSinceLastReward = sensoryData.at("timeSinceLastReward");
        // Reward decay over time
        timeModulation = std::exp(-timeSinceLastReward * 0.1f);
    }
    
    // State-based modulation
    float stateModulation = 1.0f;
    if (sensoryData.find("currentStateValue") != sensoryData.end()) {
        float stateValue = sensoryData.at("currentStateValue");
        // Modulate based on current state value
        stateModulation = std::max(0.1f, stateValue);
    }
    
    return reward * timeModulation * stateModulation;
}

float Reward::applyTemporalDecay(float reward) const {
    // Apply temporal decay to reward
    float decayFactor = 0.9f; // 10% decay per time step
    return reward * decayFactor;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
