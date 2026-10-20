#pragma once

#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../environment/Environment.hpp"
#include "ISubsystem.hpp"
#include <memory>

namespace nlm {

class EnvironmentInterface : public ISubsystem {
public:
    EnvironmentInterface();
    ~EnvironmentInterface() override;
    
    // ISubsystem interface
    bool initialize(Brain* brain) override;
    void update(float dt) override;
    void reset() override;
    void logStatus() const override;
    
    // Sensory input handling
    SensoryInput receiveSensoryInput() const;
    void processSensoryInput(const SensoryInput& input);
    std::vector<float> getSensoryData() const;
    void setSensoryModality(SensoryModality modality);
    
    // Motor output production
    std::unique_ptr<Action> produceAction() const;
    void processAction(const Action& action);
    void setMotorOutput(float speed, float direction);
    
    // Environment interaction
    void updateEnvironmentState() const;
    void updatePerception(const SensoryInput& input);
    void updateActionSelection() const;
    
    // Environment state
    bool isInRestingState() const;
    void setRestingState(bool resting);
    bool isInSleepCycle() const;
    void setSleepCycle(bool sleeping);
    
    // Perception and attention
    void updatePerceptualFeatures() const;
    void updateAttentionalFocus() const;
    bool isStimulusAttended(float stimulus salience) const;
    
    // Environment statistics
    size_t getSensoryModalityCount() const;
    size_t getActiveActionsCount() const;
    float getPerceptualAccuracy() const;
    
    // Environment configuration
    void setEnvironmentParameters(const std::string& parameters);
    void setSensoryNoise(float noiseLevel);
    void setMotorNoise(float noiseLevel);
    void setPerceptionAccuracy(float accuracy);
    
    // State management
    void resetEnvironmentInterface();
    void synchronizeWithEnvironment();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
