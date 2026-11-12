#pragma once

// Simplified AgentBrain header - delegates to specialized components
// This maintains backward compatibility while enabling better code organization

#include "AgentBrainCore.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <string>

namespace nlm {

// Simplified AgentBrain facade that delegates to core implementation
// This file maintains the same public API as the original while improving organization
class AgentBrain : public AgentBrainCore {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Core methods - inherited from AgentBrainCore
    void initialize(const SimpleWorld& world) override;
    size_t getSensoryInputSize() const override;
    size_t getMotorOutputSize() const override;
    void processSensoryInput(const SensoryPercept& percept) override;
    MotorCommand decodeMotorCommand() override;
    void applyRewardModulation(float reward, float predictedReward) override;
    void updateDevelopment(double timestep) override;
    DevelopmentalStage getDevelopmentalStage() const override;
    float getNeuromodulationLevel() const override;
    float getCuriosityLevel() const override;
    float getNoveltyLevel() const override;
    float getPredictionError() const override;
    std::string getThoughtTrace() const override;
    float getLearnedPreference() const override;
    void reset() override;
    Brain* getBrain() override;

    // Configuration methods (inline from base)
    void enableRewardModulation(bool enable) override { AgentBrainCore::enableRewardModulation(enable); }
    void enableStructuralPlasticity(bool enable) override { AgentBrainCore::enableStructuralPlasticity(enable); }
    void enableDevelopment(bool enable) override { AgentBrainCore::enableDevelopment(enable); }
    void enableCuriosity(bool enable) override { AgentBrainCore::enableCuriosity(enable); }

    // Getters (inline from base)
    bool isRewardModulationEnabled() const override { return AgentBrainCore::isRewardModulationEnabled(); }
    bool isStructuralPlasticityEnabled() const override { return AgentBrainCore::isStructuralPlasticityEnabled(); }
    bool isDevelopmentEnabled() const override { return AgentBrainCore::isDevelopmentEnabled(); }
    bool isCuriosityEnabled() const override { return AgentBrainCore::isCuriosityEnabled(); }

    // Phase 6 methods (inline from base)
    static std::vector<size_t> encodeText(const std::string& text) { return AgentBrainCore::encodeText(text); }
    std::vector<float> getInternalFiringRates() const override;
    size_t getConceptCount() const override;
    float getSelfModelConfidence() const override;
    size_t getSocialObservationCount() const override;
    void observeOtherAgent(MotorCommand observed, const std::vector<float>& beforeVision,
                           const std::vector<float>& afterVision) override;

    // Additional useful methods
    std::vector<MotorCommand::MoveForward> getMotorGroupActivities() const;

private:
    // Internal implementation details - all moved to .cpp file
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace nlm
