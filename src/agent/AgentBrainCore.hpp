#pragma once

// Core AgentBrain interface - essential methods for brain-world interaction
// This file contains the minimal interface needed for core functionality
// Additional methods are defined in specialized headers as needed

#include "AgentBrainCore.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations for internal implementation
class SelfModel;
class SocialLearning;
class DevelopmentalStage;

// Phase 6 integrated systems - wired into the main loop
// These systems were previously library-only classes now wired into AgentBrain
class AgentBrainPhase6 {
public:
    AgentBrainPhase6();
    ~AgentBrainPhase6();
    
    // Text encoding for Phase 6 text-as-a-sense
    static std::vector<size_t> encodeText(const std::string& text);
    
    // Internal firing rates for text-association recall probe
    std::vector<float> getInternalFiringRates() const;
    
    // Self-model confidence calculation
    float getSelfModelConfidence(const std::vector<float>& vision) const;
    
    // Social learning and observation
    void observeOtherAgent(MotorCommand observed, const std::vector<float>& beforeVision,
                           const std::vector<float>& afterVision);
    size_t getSocialObservationCount() const;
    
    // Concept-related functionality (wired into decode)
    size_t getConceptCount() const;
    std::vector<float> conceptPattern() const;
    
    // Get counters for advanced diagnostics
    int getPlannerOverrideCount() const { return plannerOverrideCount_; }
    int getPlannerAgreementCount() const { return plannerAgreementCount_; }
    int getPlannerDecisionCount() const { return plannerDecisionCount_; }
    int getConceptBiasCount() const { return conceptBiasCount_; }
    int getSelfBiasCount() const { return selfBiasCount_; }
    int getSocialBiasCount() const { return socialBiasCount_; }

private:
    // Phase 6 counters and state
    int plannerOverrideCount_ = 0;
    int plannerAgreementCount_ = 0;
    int plannerDecisionCount_ = 0;
    int conceptBiasCount_ = 0;
    int selfBiasCount_ = 0;
    int socialBiasCount_ = 0;
    
    // Self-model and social learning instances
    SelfModel selfModel_;
    SocialLearning socialLearning_;
    bool selfModelReady_ = false;
    bool socialReady_ = false;
    
    // Text encoding cache for performance
    static std::vector<size_t> encodeText(const std::string& text, uint64_t hash);
};

// AgentBrain: Main interface - now delegates to core and specialized components
// This maintains backward compatibility while improving code organization
class AgentBrain : public AgentBrainCore {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Core methods from AgentBrainCore (inherited)
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

    // Additional configuration methods
    void enableRewardModulation(bool enable) override { AgentBrainCore::enableRewardModulation(enable); }
    void enableStructuralPlasticity(bool enable) override { AgentBrainCore::enableStructuralPlasticity(enable); }
    void enableDevelopment(bool enable) override { AgentBrainCore::enableDevelopment(enable); }
    void enableCuriosity(bool enable) override { AgentBrainCore::enableCuriosity(enable); }
    
    // Getters
    bool isRewardModulationEnabled() const override { return AgentBrainCore::isRewardModulationEnabled(); }
    bool isStructuralPlasticityEnabled() const override { return AgentBrainCore::isStructuralPlasticityEnabled(); }
    bool isDevelopmentEnabled() const override { return AgentBrainCore::isDevelopmentEnabled(); }
    bool isCuriosityEnabled() const override { return AgentBrainCore::isCuriosityEnabled(); }

    // Phase 6 integrated methods (delegated to phase6_)
    std::vector<size_t> encodeText(const std::string& text) override;
    std::vector<float> getInternalFiringRates() const override;
    size_t getConceptCount() const override;
    float getSelfModelConfidence() const override;
    size_t getSocialObservationCount() const override;
    void observeOtherAgent(MotorCommand observed, const std::vector<float>& beforeVision,
                           const std::vector<float>& afterVision) override;

    // Phase 5 diagnostic counters (for demo reports)
    int getReflexBonusCount() const { return reflexBonusCount_; }
    int getReflexSuppressCount() const { return reflexSuppressCount_; }
    int getWtaPulseCount() const { return wtaPulseCount_; }

    // Additional internal methods
    void updatePhase6Systems();
    void updatePhase5Systems();

private:
    // Internal implementation - delegated to core component
    class Impl;
    std::unique_ptr<Impl> pImpl_;
    
    // Phase 6 systems
    AgentBrainPhase6 phase6_;
    
    // Phase 5 counters
    int reflexBonusCount_ = 0;
    int reflexSuppressCount_ = 0;
    int wtaPulseCount_ = 0;
    
    // Phase 6 counters (synced with phase6_)
    int& plannerOverrideCount_ = phase6_.getPlannerOverrideCount();
    int& plannerAgreementCount_ = phase6_.getPlannerAgreementCount();
    int& plannerDecisionCount_ = phase6_.getPlannerDecisionCount();
    int& conceptBiasCount_ = phase6_.getConceptBiasCount();
    int& selfBiasCount_ = phase6_.getSelfBiasCount();
    int& socialBiasCount_ = phase6_.getSocialBiasCount();
};

} // namespace nlm
