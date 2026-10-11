#pragma once

#include "PluginBase.hpp"
#include "../brain/NeuralRegion.hpp"

namespace nlm {

enum class CognitiveAlgorithm {
    CLASSICAL,
    BAYESIAN,
    ACT_R,
    NEURAL_NETWORK,
    CUSTOM
};

class CognitiveState {
public:
    double energy;
    double attention;
    double workingMemoryLoad;
    double motivation;
    double curiosity;
    
    CognitiveState() : energy(1.0), attention(1.0), workingMemoryLoad(0.0),
                      motivation(1.0), curiosity(1.0) {}
};

class Experience {
public:
    std::string id;
    double timestamp;
    std::vector<float> sensoryInput;
    std::vector<float> motorOutput;
    double reward;
    double predictionError;
    
    Experience() : timestamp(0.0), reward(0.0), predictionError(0.0) {}
};

class CognitionPlugin : public NLMPlugin {
public:
    // Cognitive extension
    virtual void updateCognition(double deltaTime) = 0;
    virtual void makeDecision(const CognitiveState& state) = 0;
    virtual void learnFromExperience(const Experience& experience) = 0;
    
    // Cognitive algorithms
    virtual CognitiveAlgorithm getAlgorithm() const = 0;
    virtual double getLearningRate() const = 0;
    virtual int getPlanningDepth() const = 0;
};

} // namespace nlm
