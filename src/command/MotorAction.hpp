#pragma once

#include <memory>
#include <string>
#include <vector>

namespace nlm {

class Brain;

// Abstract base class for motor actions
class MotorAction {
public:
    virtual ~MotorAction() = default;
    virtual void execute() = 0;
    virtual std::string getName() const = 0;
    virtual float getPriority() const = 0;
    virtual void setContext(Brain* brain) { contextBrain_ = brain; }
    
protected:
    Brain* contextBrain_ = nullptr;
};

// Concrete motor actions
class MoveForwardAction : public MotorAction {
public:
    void execute() override {
        if (contextBrain_) {
            contextBrain_->injectCurrentToNeurons(NeuronType::Motor, 50.0f);
        }
    }
    
    std::string getName() const override { return "MoveForward"; }
    float getPriority() const override { return 1.0f; }
};

class MoveBackwardAction : public MotorAction {
public:
    void execute() override {
        if (contextBrain_) {
            contextBrain_->injectCurrentToNeurons(NeuronType::Motor, -30.0f);
        }
    }
    
    std::string getName() const override { return "MoveBackward"; }
    float getPriority() const override { return 0.9f; }
};

class TurnLeftAction : public MotorAction {
public:
    void execute() override {
        if (contextBrain_) {
            // Turn left by stimulating left-turn neurons
            contextBrain_->injectCurrentToNeurons(NeuronType::Motor, 20.0f);
        }
    }
    
    std::string getName() const override { return "TurnLeft"; }
    float getPriority() const override { return 0.8f; }
};

class TurnRightAction : public MotorAction {
public:
    void execute() override {
        if (contextBrain_) {
            // Turn right by stimulating right-turn neurons
            contextBrain_->injectCurrentToNeurons(NeuronType::Motor, 20.0f);
        }
    }
    
    std::string getName() const override { return "TurnRight"; }
    float getPriority() const override { return 0.8f; }
};

class InteractAction : public MotorAction {
public:
    void execute() override {
        if (contextBrain_) {
            contextBrain_->injectCurrentToNeurons(NeuronType::Motor, 40.0f);
        }
    }
    
    std::string getName() const override { return "Interact"; }
    float getPriority() const override { return 0.7f; }
};

class WaitAction : public MotorAction {
public:
    void execute() override {
        if (contextBrain_) {
            // Do nothing - allows system to rest and process internally
        }
    }
    
    std::string getName() const override { return "Wait"; }
    float getPriority() const override { return 0.5f; }
};

class ActionFactory {
public:
    static std::unique_ptr<MotorAction> createAction(MotorCommand command) {
        switch (command) {
            case MotorCommand::MoveForward:
                return std::make_unique<MoveForwardAction>();
            case MotorCommand::MoveBackward:
                return std::make_unique<MoveBackwardAction>();
            case MotorCommand::TurnLeft:
                return std::make_unique<TurnLeftAction>();
            case MotorCommand::TurnRight:
                return std::make_unique<TurnRightAction>();
            case MotorCommand::Interact:
                return std::make_unique<InteractAction>();
            case MotorCommand::LookLeft:
            case MotorCommand::LookRight:
            case MotorCommand::Wait:
            default:
                return std::make_unique<WaitAction>();
        }
    }
};

} // namespace nlm
