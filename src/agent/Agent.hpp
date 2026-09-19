#pragma once

namespace nlm {

class AgentBody;

// Forward declarations for motor commands
class Action;

// Agent system that connects brain to body
// Handles sensory perception and motor output
class Agent {
public:
    Agent(std::shared_ptr<Brain> brain);
    ~Agent();
    
    // Initialize agent with world
    bool initialize(const SimpleWorld& world);
    
    // Main agent loop - one step of interaction
    void step(double timestep);
    
    // Get sensory percept
    SensoryPercept getSensoryPercept() const;
    
    // Get motor action
    std::shared_ptr<Action> getAction() const;
    
    // Apply reward to the agent
    void applyReward(float reward);
    
    // Check if agent is alive
    bool isAlive() const;
    
    // Get current agent state
    const AgentBody& getAgentBody() const;
    
    // Get agent's brain pointer
    std::shared_ptr<Brain> getBrain() const;
    
    // Reset agent for new episode
    void reset();
    
    // Enable/disable specific features
    void enableRewardModulation(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    void enableCuriosity(bool enable);
    
    bool isRewardModulationEnabled() const;
    bool isStructuralPlasticityEnabled() const;
    bool isDevelopmentEnabled() const;
    bool isCuriosityEnabled() const;
    
    // Get neuromodulation levels
    float getDopamineLevel() const;
    float getNoveltyLevel() const;
    float getCuriosityLevel() const;
    float getPredictionError() const;
    
    // Get developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get agent metrics
    double getTotalReward() const;
    size_t getTotalSteps() const;
    size_t getTotalActions() const;
    
private:
    // Core agent components
    std::shared_ptr<Brain> brain_;
    AgentBrain agentBrain_;
    AgentBody agentBody_;
    
    // Sensory and motor systems
    SensoryPercept currentPercept_;
    std::shared_ptr<Action> currentAction_;
    
    // State tracking
    bool isAlive_;
    double totalReward_;
    size_t totalSteps_;
    size_t totalActions_;
    double lastStepTime_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Update sensory input from world
    void updateSensoryInput(const SimpleWorld& world);
    
    // Generate motor output based on brain activity
    void generateMotorOutput();
    
    // Apply physical dynamics
    void applyPhysics(double timestep);
    
    // Update internal state
    void updateState(double timestep);
    
    // Check if agent should die
    void checkAgentHealth(double timestep);
};

} // namespace nlm