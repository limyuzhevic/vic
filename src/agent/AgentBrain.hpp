#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

// Include expert brain headers if available
#ifdef HAVE_EXPERTBRAIN_HPP
#include "ExpertBrain.hpp"
#include "ExpertBrainController.hpp"
#endif

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
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
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Expert brain integration (Phase 2.1+ Expert Features)
#ifdef HAVE_EXPERTBRAIN_HPP
    
    // Enable expert mode with advanced features
    bool enableExpertMode(bool enable);
    bool isExpertModeEnabled() const;
    
    // Access expert controller
    ExpertBrainController& getExpertController();
    const ExpertBrainController& getExpertController() const;
    
    // Execute expert operations
    bool executeExpertOperation(const std::string& operationName,
                               const std::function<bool()>& operation,
                               const std::string& category = "default");
    
    // Checkpoint management
    bool createExpertCheckpoint(const std::string& name,
                               const std::unordered_map<std::string, std::string>& metadata = {});
    bool restoreExpertCheckpoint(const std::string& name);
    
    // Brain state management
    std::string getExpertBrainStatus() const;
    
    // Expert optimization
    bool applyExpertOptimization(const std::string& optimizationName,
                                const std::unordered_map<std::string, float>& params);
    
    // Data export/import
    bool exportExpertData(const std::string& filepath, ExportFormat format = ExportFormat::JSON);
    bool importExpertData(const std::string& filepath, ImportFormat format = ImportFormat::JSON);
#endif
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
