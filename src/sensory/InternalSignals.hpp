#pragma once

#include "SensoryInput.hpp"

namespace nlm {

class InternalSignalsProcessor {
public:
    InternalSignalsProcessor();
    ~InternalSignalsProcessor();
    
    // Process internal signals
    void process(const InternalSignals& signals);
    
    // Get processed representation
    const std::vector<float>& getProcessedSignals() const;
    
    // Get homeostasis signals
    const std::vector<float>& getHomeostaticSignals() const;
    
    // Get neuromodulatory signals
    const std::vector<float>& getNeuromodulatorySignals() const;
    
    // Get developmental signals
    const std::vector<float>& getDevelopmentalSignals() const;
    
    // Get individual internal state signals
    float getAllostaticLoad() const;
    float getMetabolicState() const;
    float getArousalLevel() const;
    float getStressLevel() const;
    float getAttentionLevel() const;
    float getRewardPrediction() const;
    float getPredictionError() const;
    float getExplorationDrive() const;
    float getCuriosityLevel() const;
    float getSynapticEfficacy() const;
    float getInhibitionBalance() const;
    float getNetworkExcitability() const;
    
private:
    // Helper functions for internal signal processing
    void processInternalSignals(const std::vector<float>& rawSignals, class Brain* brain);
    void updateHomeostaticSignals();
    void updateNeuromodulatorySignals(class Brain* brain);
    void updateDevelopmentalSignals(class Brain* brain);
    void updatePlasticityState(class Brain* brain);
    void generateInternalSignals();
    float applyNeuralFiltering(float signal, size_t index, size_t totalSignals);
    float integrateWithState(float signal, const std::vector<float>& currentState);
    float applyNeuromodulation(float signal, class Brain* brain);
    float applyHomeostaticRegulation(float signal);
    void updateCurrentTime(float time);
    
    struct Impl {
        std::vector<float> processedSignals;
        std::vector<float> homeostaticSignals;
        std::vector<float> neuromodulatorySignals;
        std::vector<float> developmentalSignals;
        
        // Internal state variables
        float allostaticLoad;
        float metabolicState;
        float arousalLevel;
        float stressLevel;
        float attentionLevel;
        float rewardPrediction;
        float predictionError;
        float explorationDrive;
        float curiosityLevel;
        
        // Plasticity-related state
        float synapticEfficacy;
        float inhibitionBalance;
        float networkExcitability;
        
        // Time for dynamic signals
        float currentTime;
        
        Impl() : 
            allostaticLoad(0.5f), metabolicState(1.0f), arousalLevel(0.5f),
            stressLevel(0.3f), attentionLevel(0.7f), rewardPrediction(0.5f),
            predictionError(0.0f), explorationDrive(0.5f), curiosityLevel(0.5f),
            synapticEfficacy(1.0f), inhibitionBalance(1.0f), networkExcitability(1.0f),
            currentTime(0.0f) {}
    };
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
