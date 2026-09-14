#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>

namespace nlm {

// SensorimotorInterface provides bidirectional communication between Brain and AgentBrain
// 
// This interface allows:
// 1. Brain to send feedback about its state to AgentBrain
// 2. AgentBrain to receive and process brain feedback for adaptive behavior
// 3. Coordinated action selection and reward processing
//
// Key features:
// - Real-time brain state monitoring (firing rates, neural activity patterns)
// - Motor command feedback (actual vs expected outcomes)
// - Neuromodulation signal sharing (dopamine, curiosity, novelty)
// - Development stage coordination
// - Action outcome validation and learning

class SensorimotorInterface {
public:
    SensorimotorInterface();
    ~SensorimotorInterface();
    
    // Initialize interface with Brain and AgentBrain references
    void initialize(Brain* brain, AgentBrain* agentBrain);
    
    // Forward: Brain sends state updates to AgentBrain
    void sendBrainState(const std::vector<float>& neuralActivity,
                       const std::vector<NeuronId>& firingNeurons,
                       float averageFiringRate,
                       float eInertiaRatio);
    
    // Forward: Brain sends motor command feedback
    void sendMotorFeedback(ActionType expectedAction,
                          ActionType actualAction,
                          float reward,
                          const std::vector<float>& actionOutcome);
    
    // Forward: Brain sends neuromodulation updates
    void sendNeuromodulationUpdate(float dopamineLevel,
                                   float noveltyLevel,
                                   float curiosityLevel,
                                   float predictionError);
    
    // Forward: Brain sends development stage
    void sendDevelopmentStage(DevelopmentalStage stage,
                            float developmentalAge,
                            float plasticityModifier);
    
    // Backward: AgentBrain requests sensory input processing
    void processSensoryInput(const SensoryPercept& percept);
    
    // Backward: AgentBrain requests motor command decoding
    ActionType decodeMotorCommand();
    
    // Backward: AgentBrain requests neuromodulation based on outcome
    void applyRewardModulation(float reward, float predictedReward);
    
    // Backward: AgentBrain requests development updates
    void updateDevelopment(double timestep);
    
    // Get current brain state for monitoring
    struct CurrentBrainState {
        std::vector<float> neuralActivityPattern;
        std::vector<NeuronId> activeNeurons;
        float averageFiringRate;
        float eInertiaRatio;
        float dopamineLevel;
        float noveltyLevel;
        float curiosityLevel;
        float predictionError;
        DevelopmentalStage developmentalStage;
        float developmentalAge;
        float plasticityModifier;
    };
    
    CurrentBrainState getCurrentBrainState() const;
    
    // Check if interface is initialized
    bool isInitialized() const { return brain_ != nullptr && agentBrain_ != nullptr; }
    
    // Reset interface state
    void reset();
    
    // Enable/disable specific feedback loops
    void enableMotorFeedback(bool enable) { motorFeedbackEnabled_ = enable; }
    void enableNeuromodulationSharing(bool enable) { neuromodulationSharingEnabled_ = enable; }
    void enableDevelopmentCoordination(bool enable) { developmentCoordinationEnabled_ = enable; }
    
    bool isMotorFeedbackEnabled() const { return motorFeedbackEnabled_; }
    bool isNeuromodulationSharingEnabled() const { return neuromodulationSharingEnabled_; }
    bool isDevelopmentCoordinationEnabled() const { return developmentCoordinationEnabled_; }
    
    // Configuration
    void setSensorySampleRate(float rate) { sensorySampleRate_ = rate; }
    void setMotorFeedbackLatency(float latency) { motorFeedbackLatency_ = latency; }
    void setNeuromodulationGain(float gain) { neuromodulationGain_ = gain; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    AgentBrain* agentBrain_;
    
    // State tracking for feedback loops
    CurrentBrainState currentState_;
    
    // Configuration
    bool motorFeedbackEnabled_;
    bool neuromodulationSharingEnabled_;
    bool developmentCoordinationEnabled_;
    float sensorySampleRate_;
    float motorFeedbackLatency_;
    float neuromodulationGain_;
    
    // Previous state for change detection
    CurrentBrainState previousState_;
    
    // Action history for learning
    std::vector<std::pair<ActionType, std::vector<float>>> recentActions_;
    std::vector<std::pair<float, ActionType>> actionOutcomes_;
};

} // namespace nlm