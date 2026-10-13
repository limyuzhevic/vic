#pragma once

#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "BrainInitializer.hpp"
#include <memory>

namespace nlm {

class Brain;
class Neuron;

class BrainSensoryMotorProcessor {
public:
    explicit BrainSensoryMotorProcessor(std::shared_ptr<Config> config);
    ~BrainSensoryMotorProcessor() = default;
    
    // Sensory processing
    void processSensoryInput(Brain* brain, const SensoryInput& input);
    void storeSensoryToWorkingMemory(Brain* brain, const SensoryInput& input);
    
    // Motor processing
    std::unique_ptr<Action> generateAction(Brain* brain);
    void applyMotorCommand(Brain* brain, const Action& action, Timestamp time);
    
    // Neuron current injection
    void injectCurrentToNeuron(Brain* brain, NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(Brain* brain, NeuronType type, MembranePotential current);
    
    // I/O neuron management
    void updateSensoryNeurons(Brain* brain);
    void updateMotorNeurons(Brain* brain);
    
    // Motor neuron activity analysis
    size_t getActiveMotorNeurons(Brain* brain) const;
    float getMotorNeuronsActivation(Brain* brain) const;
    
    // Configuration
    void configureSensoryEncoding(Brain* brain, float scalingFactor);
    void configureMotorDecoding(Brain* brain, float threshold);
    
    // State management
    void resetIOLogic(Brain* brain);
    void clearSensoryBuffers(Brain* brain);
    void clearMotorBuffers(Brain* brain);
    
    // Advanced I/O features
    void enableReinforcementLearning(Brain* brain, bool enabled);
    void enableImitationLearning(Brain* brain, bool enabled);
    void enableSocialLearning(Brain* brain, bool enabled);

private:
    // I/O configuration parameters
    float sensoryEncodingScalingFactor;
    float motorDecodingThreshold;
    bool reinforcementLearningEnabled;
    bool imitationLearningEnabled;
    bool socialLearningEnabled;
    
    // Helper methods
    void encodeSensoryInput(Brain* brain, const SensoryInput& input);
    void distributeInputAcrossSensoryNeurons(Brain* brain, const SensoryInput& input);
    void normalizeSensoryValues(Brain* brain, std::vector<float>& values);
    void decodeMotorOutput(Brain* brain);
    void updateMotorNeuronsFromBrainActivity(Brain* brain);
    void selectMotorAction(Brain* brain);
    
    // Integration methods
    void integrateSensoryWithMemory(Brain* brain);
    void integrateMotorWithWorld(Brain* brain);
    void integrateLearningWithIOLogic(Brain* brain);
    
    // Advanced I/O features
    void enableReinforcementLearning(Brain* brain);
    void enableImitationLearning(Brain* brain);
    void enableSocialLearning(Brain* brain);
    
    // Statistics and monitoring
    void updateIOStatistics(Brain* brain);
    float calculateSensoryEntropy(Brain* brain) const;
    float calculateMotorOutputUncertainty(Brain* brain) const;
};