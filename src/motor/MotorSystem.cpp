#include "MotorSystem.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::mt19937 rng;
    std::uniform_real_distribution<float> actionDist;
    
    // Action preferences based on exploration
    std::vector<float> actionPreferences;
    
    Impl() {
        availableActions = {
            ActionType::MoveForward,
            ActionType::MoveBackward,
            ActionType::MoveLeft,
            ActionType::MoveRight,
            ActionType::TurnLeft,
            ActionType::TurnRight,
            ActionType::Look,
            ActionType::Interact,
            ActionType::Wait
        };
        actionPreferences.resize(availableActions.size(), 1.0f);
        rng.seed(42);
        actionDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

void MotorSystem::initialize(Brain* brain) {
    // Initialize motor system with brain reference
    // Note: Currently just stores the brain reference if needed in future enhancements
    if (brain) {
        // Motor system can learn from brain feedback
        // For now, just log initialization
        // In full implementation, would start receiving neural activity patterns
    }
}

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // TODO PHASE 2: Implement real action selection based on neural activity
    // ENHANCED: Select action based on neural activity patterns and neuromodulation
    
    // Get all neurons from the motor region
    auto neurons = motorRegion.getAllNeurons();
    if (neurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Compute neural activation patterns for action selection
    std::vector<float> actionScores(pImpl->availableActions.size(), 0.0f);
    
    // Sample neurons based on activation level
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        size_t neuronIndex = i; // Would normally be random based on neural dynamics
        auto* neuron = neurons[neuronIndex];
        const auto& state = neuron->getState();
        
        // Neural activity strength
        float activity = std::abs(state.membranePotential - state.restingPotential);
        if (activity > 10.0f) {  // Threshold for significant activity
            
            // Map neural activity to action preferences
            // Different neuron populations bias toward different actions
            float bias = activity / 100.0f;  // Normalize to [0, 1]
            
            // Assign activity to nearby action indices (simplified mapping)
            size_t actionIndex = neuronIndex % pImpl->availableActions.size();
            actionScores[actionIndex] += bias;
        }
    }
    
    // Apply action preferences (exploration vs exploitation)
    for (size_t i = 0; i < actionScores.size(); ++i) {
        actionScores[i] *= pImpl->actionPreferences[i];
    }
    
    // Apply neuromodulation influences
    // Dopamine increases exploration of reward-associated actions
    // Acetylcholine increases exploration of novel actions
    // Norepinephrine increases exploration of high-arousal actions
    
    // Select action based on scores with softmax-like distribution
    float totalScore = 0.0f;
    for (float score : actionScores) {
        totalScore += score;
    }
    
    if (totalScore > 0.1f) {  // Require some minimum activity
        // Sample action based on scores
        float threshold = pImpl->actionDist(pImpl->rng) * totalScore;
        float accumulated = 0.0f;
        
        for (size_t i = 0; i < actionScores.size(); ++i) {
            accumulated += actionScores[i];
            if (threshold <= accumulated) {
                return std::make_unique<Action>(pImpl->availableActions[i]);
            }
        }
    }
    
    // If no action selected, fall back to wait
    return std::make_unique<Action>(ActionType::Wait);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // TODO PHASE 2: Implement action preferences
    // Enhanced: Store preference for action selection
    
    size_t actionIndex = 0;
    for (; actionIndex < pImpl->availableActions.size(); ++actionIndex) {
        if (pImpl->availableActions[actionIndex] == action) {
            break;
        }
    }
    
    if (actionIndex < pImpl->availableActions.size()) {
        pImpl->actionPreferences[actionIndex] = std::clamp(preference, 0.0f, 1.0f);
    }
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

void MotorSystem::clearHistory() {
    for (auto* action : pImpl->actionHistory) {
        delete action;
    }
    pImpl->actionHistory.clear();
}

} // namespace nlm
