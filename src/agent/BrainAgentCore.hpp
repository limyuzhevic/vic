#pragma once

#include "../brain/Brain.hpp"
#include "AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class Brain;
class AgentBrain;
class SimpleWorld;
class SensoryPercept;
class Neuromodulator;
class Neuron;
class MotorCommand;
class DevelopmentStage;

class BrainAgentCore {
public:
    // ===========================================
    // AGENT CORE FUNCTIONALITY
    // ===========================================
    
    static void initializeAgentBrain(AgentBrain* agentBrain, std::shared_ptr<Brain> brain);
    static void destructAgentBrain(AgentBrain* agentBrain);
    static void initializeAgent(AgentBrain* agentBrain, const SimpleWorld& world);
    static void resetAgent(AgentBrain* agentBrain);
    
    // ===========================================
    // MOTOR DECODING AND ACTION SELECTION
    // ===========================================
    
    static MotorCommand decodeMotorCommand(AgentBrain* agentBrain);
    static MotorCommand decodeFromMotorNeurons(AgentBrain* agentBrain);
    static MotorCommand selectWithCuriosity(AgentBrain* agentBrain, MotorCommand defaultCmd);
    
    // ===========================================
    // SENSORY PROCESSING
    // ===========================================
    
    static void processSensoryInput(AgentBrain* agentBrain, const SensoryPercept& percept);
    static void computeNovelty(AgentBrain* agentBrain, const std::vector<float>& vision,
                              const std::vector<float>& previousVision, float& noveltyLevel);
    
    // ===========================================
    // NEUROMODULATION AND LEARNING
    // ===========================================
    
    static void applyRewardModulation(AgentBrain* agentBrain, float reward, float predictedReward);
    static void updateDevelopment(AgentBrain* agentBrain, double timestep);
    static void updateNeuromodulation(AgentBrain* agentBrain, const Neuromodulator& signal);
    
    // ===========================================
    // CONFIGURATION AND STATE ACCESSORS
    // ===========================================
    
    static Brain* getBrain(AgentBrain* agentBrain);
    static std::vector<Neuron*> getMotorNeurons(AgentBrain* agentBrain);
    static std::vector<Neuron*> getSensoryNeurons(AgentBrain* agentBrain);
    
    // ===========================================
    // STATISTICAL INFORMATION
    // ===========================================
    
    static float getNeuromodulationLevel(AgentBrain* agentBrain);
    static float getCuriosityLevel(AgentBrain* agentBrain);
    static float getNoveltyLevel(AgentBrain* agentBrain);
    static float getPredictionError(AgentBrain* agentBrain);
    static DevelopmentalStage getDevelopmentalStage(AgentBrain* agentBrain);
    
    // ===========================================
    // CONFIGURATION FLAGS
    // ===========================================
    
    static void enableRewardModulation(AgentBrain* agentBrain, bool enable);
    static void enableStructuralPlasticity(AgentBrain* agentBrain, bool enable);
    static void enableDevelopment(AgentBrain* agentBrain, bool enable);
    static void enableCuriosity(AgentBrain* agentBrain, bool enable);
    
    static bool isRewardModulationEnabled(AgentBrain* agentBrain);
    static bool isStructuralPlasticityEnabled(AgentBrain* agentBrain);
    static bool isDevelopmentEnabled(AgentBrain* agentBrain);
    static bool isCuriosityEnabled(AgentBrain* agentBrain);
    
    // ===========================================
    // UTILITY FUNCTIONS
    // ===========================================
    
    static void logAgentStatus(AgentBrain* agentBrain);
};

} // namespace nlm