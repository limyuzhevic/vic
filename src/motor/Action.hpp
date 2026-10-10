#pragma once

#include <vector>
#include <string>

namespace nlm {

enum class ActionType {
    MoveForward = 0,
    MoveBackward = 1,
    MoveLeft = 2,
    MoveRight = 3,
    Look = 4,
    Interact = 5,
    Wait = 6,
    LookLeft = 7,
    LookRight = 8,
    LookUp = 9,
    LookDown = 10,
    Eat = 11,
    Drink = 12,
    Rest = 13,
    Custom = 14
};

class Action {
public:
    Action();
    Action(ActionType type);
    Action(ActionType type, const std::vector<float>& params);
    ~Action();
    
    ActionType getType() const { return type_; }
    void setType(ActionType type) { type_ = type; }
    
    const std::vector<float>& getParameters() const { return parameters_; }
    void setParameters(const std::vector<float>& params) { parameters_ = params; }
    
    float getReward() const { return reward_; }
    void setReward(float r) { reward_ = r; }
    
    bool getSuccess() const { return success_; }
    void setSuccess(bool s) { success_ = s; }
    
    const std::string& getMessage() const { return message_; }
    void setMessage(const std::string& m) { message_ = m; }
    
    static const char* actionTypeToString(ActionType type);
    
    std::string getName() const;
    
    std::unique_ptr<Action> clone() const;
    
private:
    ActionType type_;
    std::vector<float> parameters_;
    float reward_;
    bool success_;
    std::string message_;
};

} // namespace nlm