#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// Motor neuron distribution algorithms
enum class MotorDistributionAlgorithm {
    RoundRobin,
    Balanced,
    Random
};

// Sensory neuron grouping configuration
struct SensoryGroupingConfig {
    bool enableVision;
    bool enableTouch;
    bool enableInternal;
    bool enableProprioception;
    bool enableAudio;
    
    SensoryGroupingConfig() 
        : enableVision(true), enableTouch(true), 
          enableInternal(true), enableProprioception(true), enableAudio(false) {}
};

// Novelty detection weights for different modalities
struct NoveltyWeights {
    float vision;
    float touch;
    float internal;
    float proprioception;
    float audio;
    
    NoveltyWeights() 
        : vision(1.0f), touch(0.5f), internal(0.3f), 
          proprioception(0.4f), audio(0.2f) {}
};

// Exploration probability curve parameters
struct ExplorationCurve {
    enum CurveType {
        Linear,
        Exponential,
        Sigmoid,
        Logistic
    };
    
    CurveType type;
    float minExploration;
    float maxExploration;
    float inflectionPoint;
    float steepness;
    
    ExplorationCurve() 
        : type(CurveType::Linear), minExploration(0.1f), 
          maxExploration(0.5f), inflectionPoint(0.5f), steepness(1.0f) {}
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    // Constructor with configuration
    AgentBrain(std::shared_ptr<Brain> brain,
               const MotorDistributionAlgorithm& motorDistAlgorithm = MotorDistributionAlgorithm::RoundRobin,
               const SensoryGroupingConfig& sensoryConfig = SensoryGroupingConfig(),
               const NoveltyWeights& noveltyWeights = NoveltyWeights(),
               const ExplorationCurve& explorationCurve = ExplorationCurve());
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
    const Brain* getBrain() const { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // New configuration setters
    void setMotorDistributionAlgorithm(MotorDistributionAlgorithm algorithm) { motorDistAlgorithm_ = algorithm; }
    void setSensoryGroupingConfig(const SensoryGroupingConfig& config) { sensoryConfig_ = config; }
    void setNoveltyWeights(const NoveltyWeights& weights) { noveltyWeights_ = weights; }
    void setMotorCommandThreshold(float threshold) { motorCommandThreshold_ = threshold; }
    void setCuriosityThreshold(float threshold) { curiosityThreshold_ = threshold; }
    void setExplorationCurve(const ExplorationCurve& curve) { explorationCurve_ = curve; }
    void setExplorationBaseProbability(float prob) { explorationBaseProb_ = prob; }
    void setNoveltyDecayRate(float decay) { noveltyDecayRate_ = decay; }
    
    // Novelty calculation methods
    void computeNoveltyFromAllModalities(const SensoryPercept& percept);
    void computeVisionNovelty(const std::vector<float>& current, const std::vector<float>& previous);
    void computeTouchNovelty(const std::vector<float>& current, const std::vector<float>& previous);
    void computeInternalNovelty(const std::vector<float>& current, const std::vector<float>& previous);
    void computeProprioceptionNovelty(const std::vector<float>& current, const std::vector<float>& previous);
    void computeAudioNovelty(const std::vector<float>& current, const std::vector<float>& previous);
    
    // Query methods for neuron group states
    size_t getMotorNeuronCount(MotorCommand command) const;
    size_t getSensoryNeuronCount(const std::string& modality) const;
    float getNeuronGroupActivity(const std::vector<Neuron*>& group) const;
    float getAverageMotorActivity() const;
    float getAverageSensoryActivity() const;
    
    // Exploration tuning methods
    float getExplorationProbability(float curiosity) const;
    float calculateCuriosityFromNovelty() const;
    MotorCommand getRandomMotorCommand() const;
    bool shouldExplore(float curiosity) const;
    
    // State inspection methods
    std::string getDistributionStats() const;
    float getTotalSensoryActivity() const;
    float getTotalMotorActivity() const;
    bool isNeuronGroupEmpty(const std::vector<Neuron*>& group) const;
    
    // Utility methods
    void updateDistribution();
    void resetNoveltyHistory();
    float getExplorationProbabilityForLevel(float curiosityLevel) const;
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Configuration data
    MotorDistributionAlgorithm motorDistAlgorithm_;
    SensoryGroupingConfig sensoryConfig_;
    NoveltyWeights noveltyWeights_;
    ExplorationCurve explorationCurve_;
    
    // Thresholds and parameters
    float motorCommandThreshold_;
    float curiosityThreshold_;
    float explorationBaseProb_;
    float noveltyDecayRate_;
    
    // Novelty history for all modalities
    std::vector<float> previousVision_;
    std::vector<float> previousTouch_;
    std::vector<float> previousInternal_;
    std::vector<float> previousProprioception_;
    std::vector<float> previousAudio_;
    
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
    std::vector<Neuron*> sensoryAudio_;
    
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
    
    // Backward compatibility
    float sensoryNoveltyDecay_;
};

} // namespace nlm
