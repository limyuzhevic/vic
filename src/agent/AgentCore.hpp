#pragma once

// agent/AgentCore.hpp
// Core agent utilities and shared functionality
// This header provides the foundation for the modular agent architecture

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace nlm {

// Forward declarations
class Brain;
class SimpleWorld;
class MotorCommand;

// Agent core state management
class AgentCore {
public:
    // Core agent operations
    static bool validateAgentConfiguration(const std::shared_ptr<const Config>& config);
    static void initializeAgentBody(AgentBody& agent, const SimpleWorld& world);
    static void distributeMotorNeurons(AgentBody& agent, const Brain& brain);
    static void distributeSensoryNeurons(AgentBody& agent, const Brain& brain);
    static void setupSensoryGroups(AgentBody& agent, const Brain& brain);
    static void setupMotorGroups(AgentBody& agent, const Brain& brain);
    
    // Sensory integration utilities
    static void processVisionInput(AgentBody& agent, const std::vector<float>& visionData);
    static void processTouchInput(AgentBody& agent, const std::vector<float>& touchData);
    static void processInternalInput(AgentBody& agent, const std::vector<float>& internalData);
    static void processProprioceptionInput(AgentBody& agent, const std::vector<float>& proprioceptionData);
    static void computeNovelty(AgentBody& agent, const std::vector<float>& currentVision);
    static void updateCuriosity(AgentBody& agent, float noveltyLevel, float predictionError);
    
    // Motor decoding utilities
    static MotorCommand decodeMotorCommand(AgentBody& agent);
    static MotorCommand decodeFromMotorNeurons(AgentBody& agent);
    static MotorCommand selectWithCuriosity(AgentBody& agent, MotorCommand defaultCmd);
    static float calculateMotorActivity(AgentBody& agent, const std::vector<Neuron*>& motorNeurons);
    
    // Reward and neuromodulation utilities
    static void applyRewardModulation(AgentBody& agent, float reward, float predictedReward);
    static void updateDevelopment(AgentBody& agent, double timestep);
    static void updatePlasticityRates(AgentBody& agent, double timestep);
    
    // Statistics utilities
    static std::string formatAgentStatistics(const AgentBody& agent, const Brain& brain);
    static float calculateActionSelectionConfidence(AgentBody& agent);
    static size_t getActiveMotorGroups(const AgentBody& agent);
    static float getAverageMotorActivity(AgentBody& agent);
    
    // Configuration utilities
    static bool validateAgentConfig(const std::shared_ptr<const Config>& config);
    static void applyDefaultAgentSettings(std::shared_ptr<Config> config);
    
    // Stimulus-response mapping
    static void recordExperience(AgentBody& agent, const std::vector<float>& stimulus, 
                                MotorCommand action, float outcome);
    static MotorCommand selectActionWithLearning(AgentBody& agent, const std::vector<float>& stimulus);
    static float calculateActionValue(AgentBody& agent, MotorCommand action, const std::vector<float>& stimulus);
};

// Agent body utilities
namespace AgentUtils {
    // Motor command utilities
    std::string motorCommandToString(MotorCommand cmd);
    MotorCommand stringToMotorCommand(const std::string& str);
    bool isActionTypeValid(MotorCommand cmd);
    
    // Sensory percept utilities
    float getVisionDataQuality(const SensoryPercept& percept);
    float getTouchDataQuality(const SensoryPercept& percept);
    float getInternalDataQuality(const SensoryPercept& percept);
    
    // Experience recording utilities
    std::string generateExperienceKey(const std::vector<float>& stimulus, MotorCommand action);
    float calculateExperienceOutcome(const std::vector<float>& experience, MotorCommand action);
    
    // Statistics utilities
    float calculateExplorationRate(const AgentBody& agent);
    float calculateExploitationRate(const AgentBody& agent);
    float calculateLearningRate(const AgentBody& agent);
}

} // namespace nlm
