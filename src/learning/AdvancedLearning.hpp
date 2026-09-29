#ifndef ADVANCED_LEARNING_HPP
#define ADVANCED_LEARNING_HPP

#include <vector>

namespace nlm {

class Brain;

class AdvancedLearning {
public:
    AdvancedLearning(size_t featureDim);
    ~AdvancedLearning();
    
    void initialize(Brain* brain);
    void updateFastLearning(const std::vector<std::pair<float, float>>& spikes);
    void updateSlowLearning(const std::vector<std::vector<float>>& features);
    void updateMetaplasticity(bool learningOccurred, float learningRate);
    void updateReinforcementLearning(int action, float reward, float valueEstimate);
    void integrateLearningSignals(const std::vector<float>& signals);
    void applyLearning(Brain* brain, float dt);
    
    // Configuration methods
    void setFastLearningParameters(const FastSynapticPlasticity::FSPParameters& params);
    void setMetaplasticityParameters(const Metaplasticity::MetaplasticityState& params);
    void setRLEnabled(bool enabled);
    void setMetaplasticityEnabled(bool enabled);
    void enableFastLearning(bool enabled);
    void enableSlowLearning(bool enabled);
    
    // Status and control
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration flags
    bool fastLearningEnabled_;
    bool slowLearningEnabled_;
    bool metaplasticityEnabled_;
    bool rlEnabled_;
    
    // Learning rates
    float fastLearningRate_;
    float slowLearningRate_;
};

// Learning component classes (for internal use)
class FastSynapticPlasticity;
class Metaplasticity;
class ExpectationMaximizationLearner;
class BayesianLearner;
class ReinforcementLearningIntegrator;

} // namespace nlm

#endif // ADVANCED_LEARNING_HPP
