#include "Action.hpp"

namespace nlm {

Action::Action() : type_(ActionType::Wait), neuralActivation_(0.0f), 
                   memoryBased_(false), predictionBased_(false), plannerBased_(false) {}

Action::Action(ActionType type) : type_(type), neuralActivation_(0.0f),
                                 memoryBased_(false), predictionBased_(false), plannerBased_(false) {}

Action::Action(ActionType type, const std::vector<float>& params) 
    : type_(type), parameters_(params), neuralActivation_(0.0f),
      memoryBased_(false), predictionBased_(false), plannerBased_(false) {}

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

// Motor system integration implementation
void Action::setNeuralActivation(float activation) {
    neuralActivation_ = activation;
}

void Action::setMemoryBased(bool isMemoryBased) {
    memoryBased_ = isMemoryBased;
}

void Action::setPredictionBased(bool isPredictionBased) {
    predictionBased_ = isPredictionBased;
}

void Action::setPlannerBased(bool isPlannerBased) {
    plannerBased_ = isPlannerBased;
}

float Action::getNeuralActivation() const {
    return neuralActivation_;
}

bool Action::isMemoryBased() const {
    return memoryBased_;
}

bool Action::isPredictionBased() const {
    return predictionBased_;
}

bool Action::isPlannerBased() const {
    return plannerBased_;
}

} // namespace nlm
