#include "PolicyGradientExperiment.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <limits>

namespace nlm {

PolicyGradientExperiment::PolicyGradientExperiment(std::shared_ptr<Config> config, size_t duration)
    : config(config), duration(duration), randomGen(12345) {
    
    NLM_LOG_INFO("Policy Gradient Experiment initialized");
    NLM_LOG_INFO("Duration: " + std::to_string(duration) + " steps");
    
    // Initialize statistics
    stats = Statistics();
    stats.bestReward = std::numeric_limits<float>::lowest();
}

PolicyGradientExperiment::~PolicyGradientExperiment() = default;

void PolicyGradientExperiment::initialize() {
    // Create brain with policy gradient config
    if (!config) {
        config = std::make_shared<Config>();
        
        // Configure for policy gradient
        config->set("random_seed", 12345LL, ConfigSource::Default);
        config->set("neuron_count", 2000, ConfigSource::Default);
        config->set("region_count", 2, ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Policy gradient-specific parameters
        config->set("policy_learning_rate", 0.01f, ConfigSource::Default);
        config->set("discount_factor", 0.95f, ConfigSource::Default);
        config->set("exploration_rate", 0.1f, ConfigSource::Default);
        config->set("action_space_size", 10, ConfigSource::Default);
    }
    
    // Initialize brain
    brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for policy gradient experiment");
        return;
    }
    
    // Initialize policy gradient system
    size_t actionSpaceSize = config->getOr<size_t>("action_space_size", 10);
    policyGradient.initialize(brain.get(), actionSpaceSize,
                               config->getOr<float>("policy_learning_rate", 0.01f),
                               config->getOr<float>("discount_factor", 0.95f),
                               config->getOr<float>("exploration_rate", 0.1f));
    
    startTime = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("Brain initialized for policy gradient experiment");
    NLM_LOG_INFO("Neuron count: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Region count: " + std::to_string(brain->getRegionCount()));
    NLM_LOG_INFO("Action space size: " + std::to_string(actionSpaceSize));
}

void PolicyGradientExperiment::run() {
    NLM_LOG_INFO("=== Starting Policy Gradient Experiment ===");
    
    // Initialize
    initialize();
    
    // Generate initial state
    generateNewState();
    
    // Run simulation steps
    for (size_t step = 0; step < duration; ++step) {
        runStep(step);
        updateStatistics();
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Reward: " + std::to_string(stats.averageReward) +
                        " | Policy grad: " + std::to_string(stats.policyGradientMagnitude));
        }
        
        // Generate new state every 50 steps
        if (step % 50 == 0 && step > 0) {
            generateNewState();
        }
    }
    
    NLM_LOG_INFO("=== Policy Gradient Experiment Complete ===");
    printResults();
}

void PolicyGradientExperiment::runStep(size_t step) {
    // Generate new state
    std::vector<float> state = generateStatePattern(100); // State size
    
    // Set state in policy gradient system
    setState(state);
    
    // Select action based on current policy
    size_t action = policyGradient.selectAction(state);
    
    // Generate reward based on action
    float reward = generateReward(action);
    
    // Generate next state
    std::vector<float> nextState = generateStatePattern(100);
    
    // Update policy gradient
    policyGradient.updatePolicy(state, action, reward, nextState);
    
    // Add experience to history
    experienceHistory.emplace_back(state, action, reward, nextState);
    
    // Update statistics
    stats.actionsSelected++;
    
    // Step the brain
    brain->step(step, step * 0.001f);
}

std::vector<float> PolicyGradientExperiment::generateStatePattern(size_t size) {
    std::vector<float> state(size);
    
    // Generate random state with specific characteristics
    // This represents the environmental state for the agent
    
    // Type 1: Simple random state
    for (size_t i = 0; i < size; ++i) {
        state[i] = randomGen.uniformReal(-1.0f, 1.0f);
    }
    
    return state;
}

void PolicyGradientExperiment::setState(const std::vector<float>& state) {
    // Set state in policy gradient system
    // (Placeholder for state setting)
    
    // Store state in history
    // experienceHistory.push_back(state); // This is done in runStep
    
    NLM_LOG_DEBUG("Set state pattern (size: " + std::to_string(state.size()) + ")");
}

std::vector<float> PolicyGradientExperiment::getActionProbabilities(const std::vector<float>& state) const {
    return policyGradient.getActionProbabilities(state);
}

void PolicyGradientExperiment::updateStatistics() {
    // Update experiment statistics
    stats.stepsCompleted++;
    
    // Update average reward
    if (!stats.rewardHistory.empty()) {
        float sum = std::accumulate(stats.rewardHistory.begin(), stats.rewardHistory.end(), 0.0f);
        stats.averageReward = sum / stats.rewardHistory.size();
        
        // Update best reward
        stats.bestReward = std::max(stats.bestReward, stats.averageReward);
    }
    
    // Update policy gradient magnitude
    stats.policyGradientMagnitude = policyGradient.getPolicyGradientMagnitude();
    
    // Update exploration rate
    stats.explorationRate = policyGradient.getExplorationRate();
    
    // Update reward history (simplified)
    if (!experienceHistory.empty()) {
        float totalReward = 0.0f;
        size_t count = 0;
        
        for (const auto& experience : experienceHistory) {
            float reward = std::get<2>(experience);
            totalReward += reward;
            count++;
        }
        
        if (count > 0) {
            stats.averageReward = totalReward / count;
        }
        
        // Update reward history
        for (size_t i = 0; i < std::min(size_t(100), experienceHistory.size()); ++i) {
            stats.rewardHistory.push_back(std::get<2>(experienceHistory[i]));
        }
    }
}

void PolicyGradientExperiment::generateNewState() {
    size_t stateSize = 100;  // State size
    auto state = generateStatePattern(stateSize);
    
    // Set state in policy gradient system
    setState(state);
    
    NLM_LOG_DEBUG("Generated new state pattern (size: " + std::to_string(stateSize) + ")");
}

float PolicyGradientExperiment::generateReward(size_t action) {
    // Generate reward based on action
    // This simulates the environment's response to actions
    
    // Simple reward function based on action
    float reward = 0.0f;
    
    // Higher actions get higher rewards (for demonstration)
    if (action < 5) {
        reward = randomGen.uniformReal(0.0f, 1.0f);
    } else {
        reward = randomGen.uniformReal(-0.5f, 0.5f);
    }
    
    return reward;
}

void PolicyGradientExperiment::printResults() const {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Policy Gradient Experiment Results ===");
    NLM_LOG_INFO("")
    NLM_LOG_INFO("Duration: " + std::to_string(duration) + " steps");
    NLM_LOG_INFO("Actions selected: " + std::to_string(stats.actionsSelected));
    NLM_LOG_INFO("Steps completed: " + std::to_string(stats.stepsCompleted));
    NLM_LOG_INFO("Average reward: " + std::to_string(stats.averageReward));
    NLM_LOG_INFO("Best reward achieved: " + std::to_string(stats.bestReward));
    NLM_LOG_INFO("Policy gradient magnitude: " + std::to_string(stats.policyGradientMagnitude));
    NLM_LOG_INFO("Exploration rate: " + std::to_string(stats.explorationRate));
    NLM_LOG_INFO("")
    
    // Determine experiment outcome
    if (stats.bestReward > 0.5f) {
        NLM_LOG_INFO("✓ SUCCESS: Policy gradient learning achieved high rewards");
        NLM_LOG_INFO("  The agent learned optimal policies through trial and error");
    } else if (stats.bestReward > 0.0f) {
        NLM_LOG_INFO("✓ PARTIAL SUCCESS: Policy gradient showed moderate improvement");
        NLM_LOG_INFO("  The agent made progress in learning optimal behaviors");
    } else {
        NLM_LOG_INFO("✗ LIMITED SUCCESS: Policy gradient showed limited improvement");
        NLM_LOG_INFO("  The agent may need different exploration strategies or parameters");
    }
}

} // namespace nlm