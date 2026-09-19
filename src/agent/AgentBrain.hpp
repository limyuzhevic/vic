#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <unordered_set>
#include "../core/Error/NLMException.hpp"

namespace nlm {

// Forward declaration
class Neuron;

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
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Helper method to clean up expired weak references
    void cleanExpiredNeuronReferences();
    
    // Helper method to lock weak_ptr safely
    template<typename T>
    std::shared_ptr<T> safeLock(const std::weak_ptr<T>& weakPtr) const;
    
    std::shared_ptr<Brain> brain_;
    
    // Weak references to neurons to avoid circular dependencies and double deletion
    // Motor neuron groups
    std::vector<std::weak_ptr<Neuron>> motorForward_;
    std::vector<std::weak_ptr<Neuron>> motorBackward_;
    std::vector<std::weak_ptr<Neuron>> motorTurnLeft_;
    std::vector<std::weak_ptr<Neuron>> motorTurnRight_;
    std::vector<std::weak_ptr<Neuron>> motorInteract_;
    std::vector<std::weak_ptr<Neuron>> motorWait_;
    
    // Sensory neuron groups
    std::vector<std::weak_ptr<Neuron>> sensoryVision_;
    std::vector<std::weak_ptr<Neuron>> sensoryTouch_;
    std::vector<std::weak_ptr<Neuron>> sensoryInternal_;
    std::vector<std::weak_ptr<Neuron>> sensoryProprioception_;
    
    // Track valid neuron pointers to clean up expired weak references
    std::unordered_set<std::weak_ptr<Neuron>> validNeuronRefs_;
    
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
