#include "Action.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

Action::Action() : type_(ActionType::Wait), isExecuting_(false), completionTime_(0.0), preference_(0.5f) {}

Action::Action(ActionType type) : type_(type), isExecuting_(false), completionTime_(0.0), preference_(0.5f) {}

Action::Action(ActionType type, const std::vector<float>& params) 
    : type_(type), parameters_(params), isExecuting_(false), completionTime_(0.0), preference_(0.5f) {}

Action::~Action() = default;

ActionType Action::getType() const {
    return type_;
}

void Action::setType(ActionType type) {
    type_ = type;
}

const std::vector<float>& Action::getParameters() const {
    return parameters_;
}

void Action::setParameters(const std::vector<float>& params) {
    parameters_ = params;
}

const char* Action::actionTypeToString(ActionType type) {
    switch (type) {
        case ActionType::MoveForward: return "MoveForward";
        case ActionType::MoveBackward: return "MoveBackward";
        case ActionType::MoveLeft: return "MoveLeft";
        case ActionType::MoveRight: return "MoveRight";
        case ActionType::TurnLeft: return "TurnLeft";
        case ActionType::TurnRight: return "TurnRight";
        case ActionType::Look: return "Look";
        case ActionType::LookUp: return "LookUp";
        case ActionType::LookDown: return "LookDown";
        case ActionType::Interact: return "Interact";
        case ActionType::Eat: return "Eat";
        case ActionType::Drink: return "Drink";
        case ActionType::Rest: return "Rest";
        case ActionType::Wait: return "Wait";
        case ActionType::Custom: return "Custom";
        default: return "Unknown";
    }
}

std::string Action::getName() const {
    return std::string(actionTypeToString(type_));
}

std::unique_ptr<Action> Action::clone() const {
    return std::make_unique<Action>(type_, parameters_);
}

void Action::addConsequence(const ActionConsequence& consequence) {
    consequences_.push_back(consequence);
}

const std::vector<ActionConsequence>& Action::getConsequences() const {
    return consequences_;
}

float Action::getExpectedReward() const {
    if (consequences_.empty()) return 0.0f;
    
    float totalReward = 0.0f;
    float totalProbability = 0.0f;
    
    for (const auto& cons : consequences_) {
        totalReward += cons.expectedReward * cons.probability;
        totalProbability += cons.probability;
    }
    
    return totalProbability > 0.0f ? totalReward / totalProbability : 0.0f;
}

double Action::getExecutionTime() const {
    if (consequences_.empty()) return 1.0;
    
    double totalTime = 0.0;
    float totalProbability = 0.0f;
    
    for (const auto& cons : consequences_) {
        totalTime += cons.executionTime * cons.probability;
        totalProbability += cons.probability;
    }
    
    return totalProbability > 0.0f ? totalTime / totalProbability : 1.0;
}

float Action::getResourceCost() const {
    if (consequences_.empty()) return 0.0f;
    
    float totalCost = 0.0f;
    float totalProbability = 0.0f;
    
    for (const auto& cons : consequences_) {
        totalCost += cons.resourceCost * cons.probability;
        totalProbability += cons.probability;
    }
    
    return totalProbability > 0.0f ? totalCost / totalProbability : 0.0f;
}

float Action::getUncertainty() const {
    if (consequences_.empty()) return 0.0f;
    
    float maxUncertainty = 0.0f;
    for (const auto& cons : consequences_) {
        maxUncertainty = std::max(maxUncertainty, cons.uncertainty);
    }
    
    return maxUncertainty;
}

bool Action::isValid(const std::vector<float>& currentState) const {
    // Base validation - can be overridden in derived classes
    return true;
}

const std::string& Action::getMetadata(const std::string& key) const {
    static const std::string empty = "";
    auto it = metadata_.find(key);
    if (it != metadata_.end()) {
        return it->second;
    }
    return empty;
}

void Action::setMetadata(const std::string& key, const std::string& value) {
    metadata_[key] = value;
}

std::unique_ptr<Action> Action::cloneWithParameters(const std::vector<float>& newParams) const {
    auto cloned = std::make_unique<Action>(type_, newParams);
    
    // Copy consequences
    cloned->consequences_ = consequences_;
    
    // Copy execution state
    cloned->isExecuting_ = isExecuting_;
    cloned->completionTime_ = completionTime_;
    cloned->preference_ = preference_;
    
    // Copy metadata
    cloned->metadata_ = metadata_;
    
    return cloned;
}

} // namespace nlm