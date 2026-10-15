#pragma once

#include "Brain.hpp"
#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../environment/Environment.hpp"
#include <memory>
#include <vector>

namespace nlm {

class BrainIO {
public:
    BrainIO() = default;
    ~BrainIO() = default;
    
    // Sensory input processing
    void processSensoryInput(std::shared_ptr<Brain> brain, const SensoryInput& input);
    void normalizeAndInjectSensoryData(std::shared_ptr<Brain> brain, const SensoryInput& input);
    void updateWorkingMemoryFromSensory(std::shared_ptr<Brain> brain, const SensoryInput& input);
    
    // Motor output generation
    std::unique_ptr<Action> generateMotorOutput(std::shared_ptr<Brain> brain);
    ActionType selectActionBasedOnMotorNeurons(std::shared_ptr<Brain> brain);
    void convertActionToMotorOutput(std::shared_ptr<Brain> brain, const Action& action);
    
    // Environment interaction
    void updateEnvironmentBasedOnAction(std::shared_ptr<Brain> brain, const Action& action);
    void handleWorldFeedback(std::shared_ptr<Brain> brain, const Environment& environment);
    void updateNeuromodulationFromFeedback(std::shared_ptr<Brain> brain, float reward, float surprise);
    
    // Input validation and preprocessing
    bool validateSensoryInput(const SensoryInput& input) const;
    SensoryInput preprocessSensoryInput(const SensoryInput& input) const;
    SensoryInput transformSensoryInput(const SensoryInput& input, 
                                      std::shared_ptr<Brain> brain) const;
    
    // Output formatting and logging
    void logInputProcessing(const SensoryInput& input) const;
    void logActionSelection(const Action& action) const;
    void logEnvironmentInteraction(const Action& action, const Environment& environment) const;
    
    // State management
    void updateNeuralStatesFromInput(std::shared_ptr<Brain> brain);
    void updateNeuralStatesFromOutput(std::shared_ptr<Brain> brain);
    void resetIOState(std::shared_ptr<Brain> brain);
    
    // Configuration and parameter management
    void configureInputMapping(std::shared_ptr<Config> config);
    void configureOutputMapping(std::shared_ptr<Config> config);
    void setInputScalingFactors(std::shared_ptr<Config> config);
    void setOutputActivationThresholds(std::shared_ptr<Config> config);
    
private:
    // Helper methods
    void distributeInputAcrossSensoryNeurons(std::shared_ptr<Brain> brain, const SensoryInput& input);
    void encodeInputPattern(const SensoryInput& input, std::vector<float>& pattern) const;
    float calculateInput salience(const SensoryInput& input) const;
    
    void calculateMotorNeuronsActivity(std::shared_ptr<Brain> brain);
    ActionType selectActionFromCompetition(std::shared_ptr<Brain> brain);
    void injectCurrentToMotorPopulations(std::shared_ptr<Brain> brain, const Action& action);
    
    void applyRewardPredictionError(std::shared_ptr<Brain> brain, float reward);
    void applyNoveltySurprise(std::shared_ptr<Brain> brain, float surprise);
    
    // Input mapping helpers
    void setupDefaultInputMapping(std::vector<float>& mapping) const;
    void setupEnvironmentSpecificInputMapping(const Environment& env, std::vector<float>& mapping);
    
    // Output mapping helpers
    void setupDefaultOutputMapping(Action& action) const;
    void setupEnvironmentSpecificOutputMapping(const Environment& env, Action& action);
    
    // Scaling factor helpers
    float getInputScalingFactor(int inputIndex) const;
    float getOutputActivationThreshold(ActionType type) const;
    
    // Internal state
    std::vector<float> inputScalingFactors_;
    std::vector<float> outputActivationThresholds_;
    std::vector<std::vector<float>> inputMapping_;
    std::vector<std::vector<float>> outputMapping_;
    
    // Processing state
    size_t lastInputStep_;
    float lastRewardLevel_;
    float lastSurpriseLevel_;
};

} // namespace nlm