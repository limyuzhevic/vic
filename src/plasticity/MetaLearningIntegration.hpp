#pragma once

#include "MetaLearning.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <random>
#include <numeric>
#include <limits>
#include <functional>

namespace nlm {

// Learning-to-learn algorithms
class LearningToLearn {
public:
    LearningToLearn();
    ~LearningToLearn();
    
    // Optimize learning rate using meta-learning
    float optimizeLearningRate(const std::vector<float>& lossHistory, const std::vector<float>& gradientHistory);
    
    // Learn optimization algorithm using meta-learning
    void learnOptimizationAlgorithm(const std::vector<float>& optimizationHistory);
    
    // Apply learned optimization
    float applyOptimizedUpdate(float currentParam, float gradient, const std::vector<float>& optimizationHistory);
    
    // Get learned optimization parameters
    float getLearningRate() const { return currentLearningRate_; }
    float getMomentum() const { return momentum_; }
    float getAdamBeta1() const { return adamBeta1_; }
    float getAdamBeta2() const { return adamBeta2_; }
    
    // Set optimization algorithm parameters
    void setLearningRate(float rate) { currentLearningRate_ = rate; }
    void setMomentum(float momentum) { momentum_ = momentum; }
    void setAdamBeta1(float beta1) { adamBeta1_ = beta1; }
    void setAdamBeta2(float beta2) { adamBeta2_ = beta2; }
    
    // Reset optimization state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Current optimization parameters
    float currentLearningRate_;
    float momentum_;
    float adamBeta1_;
    float adamBeta2_;
    std::vector<float> adamFirstMoment_;
    std::vector<float> adamSecondMoment_;
    size_t adamStep_;
};

// Auto-curiosity system for intrinsic motivation
class AutoCuriosity {
public:
    AutoCuriosity();
    ~AutoCuriosity();
    
    // Update curiosity based on prediction error and novelty
    float update(float predictionError, float novelty, TimestepDuration dt);
    
    // Get current curiosity level
    float getLevel() const { return curiosityLevel_; }
    
    // Get exploration drive
    float getExplorationDrive() const { return explorationDrive_; }
    
    // Set curiosity parameters
    void setCuriosityGain(float gain) { curiosityGain_ = gain; }
    void setExplorationRate(float rate) { explorationRate_ = rate; }
    void setIntrinsicRewardWeight(float weight) { intrinsicRewardWeight_ = weight; }
    void setCuriosityDecay(float decay) { curiosityDecay_ = decay; }
    
    // Get curiosity statistics
    float getAveragePredictionError() const;
    float getAverageNovelty() const;
    float getCuriosityVariance() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Current curiosity state
    float curiosityLevel_;
    float explorationDrive_;
    
    // Curiosity parameters
    float curiosityGain_;
    float explorationRate_;
    float intrinsicRewardWeight_;
    float curiosityDecay_;
    
    // Curiosity history
    std::vector<float> predictionErrorHistory_;
    std::vector<float> noveltyHistory_;
    std::vector<float> curiosityHistory_;
};

// Metaplasticity - adaptive learning rate mechanisms
class MetaPlasticity : public PlasticityRule {
public:
    MetaPlasticity();
    ~MetaPlasticity() override;
    
    // Update with meta-plastic updates
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "MetaPlasticity"; }
    
    // Set metaplasticity parameters
    void setAdaptiveLearningRate(bool adaptive) { adaptiveLearningRate_ = adaptive; }
    void setMetaPlasticityRate(float rate) { metaPlasticityRate_ = rate; }
    void setPlasticityHistorySize(size_t size) { plasticityHistorySize_ = size; }
    void setMetaPlasticityThreshold(float threshold) { metaPlasticityThreshold_ = threshold; }
    
    // Get metaplasticity statistics
    float getAveragePlasticityModulation() const;
    float getMetaPlasticityEffectiveness() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Metaplasticity parameters
    bool adaptiveLearningRate_;
    float metaPlasticityRate_;
    size_t plasticityHistorySize_;
    float metaPlasticityThreshold_;
    
    // Plasticity history and statistics
    std::map<NeuronId, std::vector<float>> learningRateHistory_;
    std::map<NeuronId, std::vector<float>> weightChangeHistory_;
    std::map<NeuronId, float> neuronPlasticityState_;
    std::map<NeuronId, float> metaPlasticityEffectiveness_;
};

// Integration utilities for meta-learning
class MetaLearningIntegration {
public:
    MetaLearningIntegration();
    ~MetaLearningIntegration();
    
    // Initialize meta-learning integration with brain
    void initialize(Brain* brain);
    
    // Update meta-learning state based on brain activity
    void update(const std::vector<Synapse*>& synapses, float taskLoss, float validationLoss);
    
    // Register a new task for meta-learning
    void registerTask(const std::string& taskId, const std::vector<float>& taskFeatures);
    
    // Get meta-learning system
    MetaLearning* getMetaLearning() { return metaLearning_.get(); }
    TaskInference* getTaskInference() { return taskInference_.get(); }
    LearningToLearn* getLearningToLearn() { return learningToLearn_.get(); }
    AutoCuriosity* getAutoCuriosity() { return autoCuriosity_.get(); }
    MetaPlasticity* getMetaPlasticity() { return metaPlasticity_.get(); }
    
    // Set integration parameters
    void setMetaLearningEnabled(bool enabled) { metaLearningEnabled_ = enabled; }
    void setTaskInferenceEnabled(bool enabled) { taskInferenceEnabled_ = enabled; }
    void setLearningToLearnEnabled(bool enabled) { learningToLearnEnabled_ = enabled; }
    void setAutoCuriosityEnabled(bool enabled) { autoCuriosityEnabled_ = enabled; }
    void setMetaPlasticityEnabled(bool enabled) { metaPlasticityEnabled_ = enabled; }
    
private:
    // Integration components
    std::unique_ptr<MetaLearning> metaLearning_;
    std::unique_ptr<TaskInference> taskInference_;
    std::unique_ptr<LearningToLearn> learningToLearn_;
    std::unique_ptr<AutoCuriosity> autoCuriosity_;
    std::unique_ptr<MetaPlasticity> metaPlasticity_;
    
    // Integration flags
    bool metaLearningEnabled_;
    bool taskInferenceEnabled_;
    bool learningToLearnEnabled_;
    bool autoCuriosityEnabled_;
    bool metaPlasticityEnabled_;
    
    Brain* brain_;
};

} // namespace nlm