#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>

namespace nlm {

// Command Pattern: MotorAction interface
class MotorAction {
public:
    virtual ~MotorAction() = default;
    virtual void execute() = 0;
    virtual MotorCommand getCommand() const = 0;
    virtual std::string getDescription() const = 0;
};

// Concrete Motor Actions
class MoveForwardAction : public MotorAction {
public:
    MoveForwardAction() = default;
    void execute() override;
    MotorCommand getCommand() const override { return MotorCommand::MoveForward; }
    std::string getDescription() const override { return "Move forward"; }
};

class MoveBackwardAction : public MotorAction {
public:
    MoveBackwardAction() = default;
    void execute() override;
    MotorCommand getCommand() const override { return MotorCommand::MoveBackward; }
    std::string getDescription() const override { return "Move backward"; }
};

class TurnLeftAction : public MotorAction {
public:
    TurnLeftAction() = default;
    void execute() override;
    MotorCommand getCommand() const override { return MotorCommand::TurnLeft; }
    std::string getDescription() const override { return "Turn left"; }
};

class TurnRightAction : public MotorAction {
public:
    TurnRightAction() = default;
    void execute() override;
    MotorCommand getCommand() const override { return MotorCommand::TurnRight; }
    std::string getDescription() const override { return "Turn right"; }
};

class InteractAction : public MotorAction {
public:
    InteractAction() = default;
    void execute() override;
    MotorCommand getCommand() const override { return MotorCommand::Interact; }
    std::string getDescription() const override { return "Interact with environment"; }
};

class WaitAction : public MotorAction {
public:
    WaitAction() = default;
    void execute() override;
    MotorCommand getCommand() const override { return MotorCommand::Wait; }
    std::string getDescription() const override { return "Wait/idle"; }
};

class LookLeftAction : public MotorAction {
public:
    LookLeftAction() = default;
    void execute() override;
    MotorCommand getCommand() const override { return MotorCommand::LookLeft; }
    std::string getDescription() const override { return "Look left"; }
};

class LookRightAction : public MotorAction {
public:
    LookRightAction() = default;
    void execute() override;
    MotorCommand getCommand() const override { return MotorCommand::LookRight; }
    std::string getDescription() const override { return "Look right"; }
};

// Observer Pattern: NeuromodulationObserver interface
class NeuromodulationObserver {
public:
    virtual ~NeuromodulationObserver() = default;
    virtual void onDopamineChange(float level) = 0;
    virtual void onReward(float reward, float predictionError) = 0;
    virtual void onNovelty(float novelty) = 0;
};

// Strategy Pattern: Neuromodulator interface
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    virtual void update(float dt) = 0;
    virtual float getSignal() const = 0;
    virtual std::string getName() const = 0;
};

// Concrete Neuromodulators
class DopamineNeuromodulator : public Neuromodulator {
public:
    DopamineNeuromodulator(float baseline = 0.1f) : baseline_(baseline), predictionError_(0.0f) {}
    void update(float dt) override;
    float getSignal() const override { return current_; }
    std::string getName() const override { return "Dopamine"; }
    void setPredictionError(float error) { predictionError_ = error; }
    
private:
    float baseline_;
    float predictionError_;
    float current_ = 0.0f;
};

class CuriosityNeuromodulator : public Neuromodulator {
public:
    CuriosityNeuromodulator(float noveltyBase = 0.0f) : noveltyBase_(noveltyBase) {}
    void update(float dt) override;
    float getSignal() const override { return current_; }
    std::string getName() const override { return "Curiosity"; }
    void setNovelty(float novelty) { novelty_ = novelty; }
    
private:
    float noveltyBase_;
    float novelty_ = 0.0f;
    float current_ = 0.0f;
};

// State Pattern: DevelopmentStage interface
class DevelopmentStage {
public:
    virtual ~DevelopmentStage() = default;
    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void update(float dt) = 0;
    virtual std::string getName() const = 0;
    virtual float getPlasticityModifier() const = 0;
};

// Concrete Development Stages
class InitialDevelopmentStage : public DevelopmentStage {
public:
    void enter() override;
    void exit() override;
    void update(float dt) override;
    std::string getName() const override { return "Initial"; }
    float getPlasticityModifier() const override { return 1.0f; }
};

class CriticalPeriodDevelopmentStage : public DevelopmentStage {
public:
    void enter() override;
    void exit() override;
    void update(float dt) override;
    std::string getName() const override { return "CriticalPeriod"; }
    float getPlasticityModifier() const override { return 0.8f; }
};

class MaturationDevelopmentStage : public DevelopmentStage {
public:
    void enter() override;
    void exit() override;
    void update(float dt) override;
    std::string getName() const override { return "Maturation"; }
    float getPlasticityModifier() const override { return 0.5f; }
};

class AdultDevelopmentStage : public DevelopmentStage {
public:
    void enter() override;
    void exit() override;
    void update(float dt) override;
    std::string getName() const override { return "Adult"; }
    float getPlasticityModifier() const override { return 0.2f; }
};

// Factory Pattern: CommandFactory for creating motor actions
class CommandFactory {
public:
    static std::unique_ptr<MotorAction> createCommand(MotorCommand cmd);
    static std::vector<MotorCommand> getAllAvailableCommands();
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with modern design patterns
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Execute current motor command
    void executeMotorCommand(MotorCommand cmd);
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Observer pattern methods
    void addObserver(std::shared_ptr<NeuromodulationObserver> observer);
    void removeObserver(std::shared_ptr<NeuromodulationObserver> observer);
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Get current developmental stage
    DevelopmentalStage getCurrentDevelopmentStage() const;
    std::string getDevelopmentStageName() const;
    
    // Get neuromodulation levels
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
    // Get motor command history
    std::vector<MotorCommand> getCommandHistory() const { return commandHistory_; }
    size_t getCommandCount(MotorCommand cmd) const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration for neuromodulators
    void setDopamineBaseline(float baseline);
    void setCuriosityNoveltyBase(float base);
    
    // Add custom neuromodulator
    void addNeuromodulator(std::unique_ptr<Neuromodulator> modulator);
    
    // Get list of active neuromodulators
    std::vector<std::string> getNeuromodulatorNames() const;
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Internal helper methods
    void notifyObservers();
    void updateNeuromodulators(float dt);
    void updateDevelopmentState(float dt);
    
    // Observer management
    std::vector<std::shared_ptr<NeuromodulationObserver>> observers_;
    
    std::shared_ptr<Brain> brain_;
    
    // Neuromodulators (using strategy pattern)
    std::vector<std::unique_ptr<Neuromodulator>> neuromodulators_;
    
    // Development state machine (using state pattern)
    std::unique_ptr<DevelopmentStage> currentDevelopmentStage_;
    double developmentalAge_ = 0.0;
    
    // Motor neuron groups (kept for backward compatibility)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups (kept for backward compatibility)
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Command history and statistics
    std::vector<MotorCommand> commandHistory_;
    std::unordered_map<MotorCommand, size_t> commandCounts_;
    
    // Event-driven state
    bool curiosityTriggered_ = false;
    float currentNovelty_ = 0.0f;
    float currentPredictionError_ = 0.0f;
};