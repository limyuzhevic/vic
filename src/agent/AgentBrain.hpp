#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

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
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    // Advanced configuration methods
    void setCuriosityThreshold(float threshold) { curiosityThreshold_ = threshold; }
    void setExplorationMaxChance(float chance) { explorationMaxChance_ = chance; }
    void setRewardDecayRate(float rate) { rewardDecayRate_ = rate; }
    void setMotorActivationThreshold(float threshold) { motorActivationThreshold_ = threshold; }
    void setVisionInputScale(float scale) { visionInputScale_ = scale; }
    void setTouchInputScale(float scale) { touchInputScale_ = scale; }
    void setInternalInputScale(float scale) { internalInputScale_ = scale; }
    void setInternalInputCenter(float center) { internalInputCenter_ = center; }
    void setProprioceptionInputScale(float scale) { proprioceptionInputScale_ = scale; }
    void setProprioceptionInputCenter(float center) { proprioceptionInputCenter_ = center; }
    void setCuriosityNoveltyFactor(float factor) { curiosityNoveltyFactor_ = factor; }
    void setCuriosityPredictionFactor(float factor) { curiosityPredictionFactor_ = factor; }
    void setEligibilityThreshold(float threshold) { eligibilityThreshold_ = threshold; }
    void setEligibilityDecayRate(float rate) { eligibilityDecayRate_ = rate; }
    void setPlasticityFactor(float factor) { rewardPlasticityFactor_ = factor; }
    void setBasePlasticity(float base) { basePlasticity_ = base; }
    void setMinPlasticity(float min) { minPlasticity_ = min; }
    void setMaxPlasticity(float max) { maxPlasticity_ = max; }
    void setBaseLTPWeight(float weight) { baseLTPWeight_ = weight; }
    void setBaseLTDWeight(float weight) { baseLTDWeight_ = weight; }
    void setSynaptogenesisRateInitial(float rate) { synaptogenesisRateInitial_ = rate; }
    void setPruningRateInitial(float rate) { pruningRateInitial_ = rate; }
    void setInitialDevelopmentDuration(double duration) { initialDevelopmentDuration_ = duration; }
    void setCriticalPeriodDuration(double duration) { criticalPeriodDuration_ = duration; }
    void setMaturationDuration(double duration) { maturationDuration_ = duration; }
    void setHighPlasticityInitial(float value) { highPlasticityInitial_ = value; }
    void setHighPlasticityCriticalPeriod(float value) { highPlasticityCriticalPeriod_ = value; }
    void setModeratePlasticityMaturation(float value) { moderatePlasticityMaturation_ = value; }
    void setLowPlasticityAdult(float value) { lowPlasticityAdult_ = value; }
    
    // Get configuration values
    float getCuriosityThreshold() const { return curiosityThreshold_; }
    float getExplorationMaxChance() const { return explorationMaxChance_; }
    float getRewardDecayRate() const { return rewardDecayRate_; }
    float getMotorActivationThreshold() const { return motorActivationThreshold_; }
    float getVisionInputScale() const { return visionInputScale_; }
    float getTouchInputScale() const { return touchInputScale_; }
    float getInternalInputScale() const { return internalInputScale_; }
    float getInternalInputCenter() const { return internalInputCenter_; }
    float getProprioceptionInputScale() const { return proprioceptionInputScale_; }
    float getProprioceptionInputCenter() const { return proprioceptionInputCenter_; }
    float getCuriosityNoveltyFactor() const { return curiosityNoveltyFactor_; }
    float getCuriosityPredictionFactor() const { return curiosityPredictionFactor_; }
    float getEligibilityThreshold() const { return eligibilityThreshold_; }
    float getEligibilityDecayRate() const { return eligibilityDecayRate_; }
    float getPlasticityFactor() const { return rewardPlasticityFactor_; }
    float getBasePlasticity() const { return basePlasticity_; }
    float getMinPlasticity() const { return minPlasticity_; }
    float getMaxPlasticity() const { return maxPlasticity_; }
    float getBaseLTPWeight() const { return baseLTPWeight_; }
    float getBaseLTDWeight() const { return baseLTDWeight_; }
    float getSynaptogenesisRateInitial() const { return synaptogenesisRateInitial_; }
    float getPruningRateInitial() const { return pruningRateInitial_; }
    double getInitialDevelopmentDuration() const { return initialDevelopmentDuration_; }
    double getCriticalPeriodDuration() const { return criticalPeriodDuration_; }
    double getMaturationDuration() const { return maturationDuration_; }
    float getHighPlasticityInitial() const { return highPlasticityInitial_; }
    float getHighPlasticityCriticalPeriod() const { return highPlasticityCriticalPeriod_; }
    float getModeratePlasticityMaturation() const { return moderatePlasticityMaturation_; }
    float getLowPlasticityAdult() const { return lowPlasticityAdult_; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
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
    
    // Advanced configuration parameters
    float curiosityThreshold_;
    float explorationMaxChance_;
    float rewardDecayRate_;
    float motorActivationThreshold_;
    float visionInputScale_;
    float touchInputScale_;
    float internalInputScale_;
    float internalInputCenter_;
    float proprioceptionInputScale_;
    float proprioceptionInputCenter_;
    float curiosityNoveltyFactor_;
    float curiosityPredictionFactor_;
    float eligibilityThreshold_;
    float eligibilityDecayRate_;
    float rewardPlasticityFactor_;
    float basePlasticity_;
    float minPlasticity_;
    float maxPlasticity_;
    float baseLTPWeight_;
    float baseLTDWeight_;
    float synaptogenesisRateInitial_;
    float pruningRateInitial_;
    double initialDevelopmentDuration_;
    double criticalPeriodDuration_;
    double maturationDuration_;
    float highPlasticityInitial_;
    float highPlasticityCriticalPeriod_;
    float moderatePlasticityMaturation_;
    float lowPlasticityAdult_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
