// MotorSystem.cpp - Implementation
#include "MotorSystem.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

MotorSystem::MotorSystem() : lastCommand_(nullptr) {}

MotorSystem::~MotorSystem() {}

void MotorSystem::setActionSelectionStrategy(ActionSelectionStrategy strategy) {
    strategy_ = strategy;
}

void MotorSystem::addPolicy(std::shared_ptr<ActionPolicy> policy) {
    policies_.push_back(policy);
}

void MotorSystem::update(const std::vector<float>& sensoryInput,
                         float rewardPrediction,
                         float noveltyLevel) {
    
    // Evaluate all policies
    std::vector<std::pair<float, std::shared_ptr<Action>>> policyResults;
    
    for (const auto& policy : policies_) {
        auto action = policy->selectAction(sensoryInput, rewardPrediction, noveltyLevel);
        if (action) {
            policyResults.emplace_back(policy->getValue(), action);
        }
    }
    
    if (policyResults.empty()) {
        return;
    }
    
    // Select best action based on strategy
    switch (strategy_) {
        case ActionSelectionStrategy::Greedy:
            lastCommand_ = std::max_element(policyResults.begin(), policyResults.end(),
                [](const auto& a, const auto& b) { return a.first < b.first; })->second;
            break;
            
        case ActionSelectionStrategy::EpsilonGreedy:
            // Simplified epsilon-greedy: mostly greedy, occasional random
            if (rand() % 100 < 10) { // 10% exploration
                lastCommand_ = policyResults[rand() % policyResults.size()].second;
            } else {
                lastCommand_ = std::max_element(policyResults.begin(), policyResults.end(),
                    [](const auto& a, const auto& b) { return a.first < b.first; })->second;
            }
            break;
            
        case ActionSelectionStrategy::Softmax:
            // Simplified softmax implementation
            float maxValue = std::max_element(policyResults.begin(), policyResults.end(),
                [](const auto& a, const auto& b) { return a.first < b.first; })->first;
            float sum = 0.0f;
            std::vector<float> weights;
            
            for (const auto& result : policyResults) {
                float weight = std::exp((result.first - maxValue) / 0.1f); // temperature = 0.1
                weights.push_back(weight);
                sum += weight;
            }
            
            float randVal = static_cast<float>(rand()) / RAND_MAX * sum;
            float cumulative = 0.0f;
            
            for (size_t i = 0; i < policyResults.size(); ++i) {
                cumulative += weights[i];
                if (randVal <= cumulative) {
                    lastCommand_ = policyResults[i].second;
                    break;
                }
            }
            break;
    }
}

MotorCommand MotorSystem::getLastCommand() const {
    return lastCommand_ ? *lastCommand_ : MotorCommand();
}

std::shared_ptr<MotorCommand> MotorSystem::getLastCommandPtr() const {
    return lastCommand_;
}

void MotorSystem::reset() {
    lastCommand_ = nullptr;
}

} // namespace nlm