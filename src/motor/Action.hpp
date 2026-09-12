namespace nlm {

// Action: Represents a motor command that the brain can execute
// This is a simple value object with type and parameters
class Action {
public:
    Action() : type_(ActionType::Wait) {}
    Action(ActionType type) : type_(type) {}
    Action(ActionType type, const std::vector<float>& params) 
        : type_(type), parameters_(params) {}
    
    ~Action() = default;
    
    ActionType getType() const { return type_; }
    void setType(ActionType type) { type_ = type; }
    
    const std::vector<float>& getParameters() const { return parameters_; }
    void setParameters(const std::vector<float>& params) { parameters_ = params; }
    
    std::string getName() const {
        return std::string(actionTypeToString(type_));
    }
    
    std::unique_ptr<Action> clone() const {
        return std::make_unique<Action>(type_, parameters_);
    }
    
    static const char* actionTypeToString(ActionType type) {
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
    
private:
    ActionType type_;
    std::vector<float> parameters_;
};

} // namespace nlm
