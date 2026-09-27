#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "Dopamine.hpp"
#include "Curiosity.hpp"
#include "Novelty.hpp"
#include "PredictionError.hpp"
#include "Norepinephrine.hpp"
#include "AcetylCholine.hpp"
#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct NeuromodulatorManager::Impl {
    // Physiological parameters
    float baselineNE;
    float baselineACh;
    float baseline5HT;
    float neuromodulatoryCoupling;
    float saturationLevel;
    
    // Integration weights
    float noveltyToNE;
    float predictionErrorToACh;
    float rewardTo5HT;
    float workingMemoryToNE;
    
    Impl() : baselineNE(0.05f)
        , baselineACh(0.05f)
        , baseline5HT(0.05f)
        , neuromodulatoryCoupling(0.8f)
        , saturationLevel(0.95f)
        , noveltyToNE(0.6f)
        , predictionErrorToACh(0.4f)
        , rewardTo5HT(0.3f)
        , workingMemoryToNE(0.5f) {}
};

NeuromodulatorManager::NeuromodulatorManager() : pImpl(new Impl)
    , brain_(nullptr)
    , attentionEnhancement_(1.0f)
    , workingMemoryEnhancement_(1.0f)
    , learningEnhancement_(1.0f)
    , decisionStability_(1.0f)
    , explorationDrive_(0.5f)
    , totalModulation_(0.0f)
    , attention_(nullptr)
    , planner_(nullptr)
{
}

NeuromodulatorManager::~NeuromodulatorManager() = default;

void NeuromodulatorManager::initialize(Brain* brain) {
    brain_ = brain;
    
    // Initialize all neuromodulators
    dopamine_ = std::make_unique<Dopamine>();
    curiosity_ = std::make_unique<Curiosity>();
    novelty_ = std::make_unique<Novelty>();
    predictionError_ = std::make_unique<PredictionError>();
    norepinephrine_ = std::make_unique<Norepinephrine>();
    norepinephrine_->initialize(brain);
    acetylcholine_ = std::make_unique<AcetylCholine>();
    acetylcholine_->initialize(brain);
    serotonin_ = std::make_unique<Serotonin>();
    serotonin_->initialize(brain);
    
    NLM_LOG_INFO("NeuromodulatorManager initialized with enhanced systems");
}

void NeuromodulatorManager::updateAll(float novelty, float predictionError, float reward,
                                      float workingMemoryLoad, TimestepDuration dt) {
    // Update individual neuromodulators
    novelty_->update(novelty, dt);
    curiosity_->update(novelty, predictionError, dt);
    dopamine_->update(dt);
    predictionError_->update(predictionError, dt);
    
    // Enhanced neuromodulators
    norepinephrine_->update(novelty, predictionError, dt);
    acetylcholine_->update(workingMemoryLoad, novelty, dt);
    serotonin_->update(predictionError, reward, dt);
    
    // Compute integrated modulation factors
    float neLevel = norepinephrine_->getLevel();
    float achLevel = acetylcholine_->getLevel();
    float serotoninLevel = serotonin_->getLevel();
    
    // Attention enhancement (NE + ACh)
    attentionEnhancement_ = (1.0f + neLevel * 1.5f) * (1.0f + achLevel * 0.7f);
    
    // Working memory enhancement (ACh)
    workingMemoryEnhancement_ = (1.0f + achLevel * 1.5f);
    
    // Learning enhancement (5-HT)
    learningEnhancement_ = (1.0f + serotoninLevel * 1.2f);
    
    // Decision stability (5-HT)
    decisionStability_ = (1.0f + serotoninLevel * 0.8f);
    
    // Exploration drive (NE + curiosity)
    float curiosityLevel = curiosity_->getLevel();
    explorationDrive_ = neLevel * 0.6f + curiosityLevel * 0.4f;
    
    // Total modulation load
    totalModulation_ = (neLevel + achLevel + serotoninLevel + curiosityLevel + dopamine_->getLevel()) / 5.0f;
    
    // Apply neuromodulatory effects to cognitive systems
    applyModulatoryEffects(dt);
}

void NeuromodulatorManager::connectToAttention(AttentionalSelection* attention) {
    attention_ = attention;
    
    // Register attention enhancement callbacks
    if (attention_ && norepinephrine_) {
        attention_->setAttentionEnhancementFactor(norepinephrine_->getAttentionEnhancement());
        attention_->setWorkingMemoryBenefitFactor(acetylcholine_->getWorkingMemoryBenefit());
        attention_->setAttentionClarityFactor(acetylcholine_->getAttentionClarity());
    }
    
    NLM_LOG_INFO("NeuromodulatorManager connected to Attention system");
}

void NeuromodulatorManager::connectToNeuralPlanner(NeuralPlanner* planner) {
    planner_ = planner;
    
    // Register planning enhancement callbacks
    if (planner_ && serotonin_) {
        planner_->setLearningEnhancementFactor(serotonin_->getLearningEnhancement());
        planner_->setDecisionStabilityFactor(serotonin_->getDecisionStability());
        planner_->setExplorationDriveFactor(serotonin_->getExplorationDrive());
    }
    
    NLM_LOG_INFO("NeuromodulatorManager connected to NeuralPlanner");
}

void NeuromodulatorManager::reset() {
    dopamine_->reset();
    curiosity_->reset();
    novelty_->reset();
    predictionError_->reset();
    norepinephrine_->reset();
    acetylcholine_->reset();
    serotonin_->reset();
    
    attentionEnhancement_ = 1.0f;
    workingMemoryEnhancement_ = 1.0f;
    learningEnhancement_ = 1.0f;
    decisionStability_ = 1.0f;
    explorationDrive_ = 0.5f;
    totalModulation_ = 0.0f;
    
    NLM_LOG_INFO("NeuromodulatorManager reset complete");
}

void NeuromodulatorManager::applyModulatoryEffects(TimestepDuration dt) {
    if (!brain_) return;
    
    float dtFloat = static_cast<float>(dt);
    
    // Apply NE effects on attention
    float neLevel = norepinephrine_->getLevel();
    if (neLevel > 0.1f) {
        // Modulate attentional focus
        for (auto& regionFocus : norepinephrine_->getFocusRegions()) {
            if (brain_) {
                brain_->injectCurrentToNeurons(NeuronType::Internal, 
                                               regionFocus.second * neLevel * 0.1f * dtFloat);
            }
        }
    }
    
    // Apply ACh effects on working memory
    float achLevel = acetylcholine_->getLevel();
    if (achLevel > 0.1f) {
        // Enhance working memory trace stability
        auto* workingMemory = brain_->getWorkingMemory();
        if (workingMemory) {
            // Modulate memory consolidation
            float memoryBenefit = acetylcholine_->getWorkingMemoryEnhancement();
            // (Working memory would use this factor in its consolidation logic)
        }
    }
    
    // Apply 5-HT effects on planning
    float serotoninLevel = serotonin_->getLevel();
    if (serotoninLevel > 0.1f) {
        // Reduce impulsivity in planning
        float impulsivityReduction = serotonin_->getImpulsivityReduction();
        // (Neural planner would use this factor in action selection)
    }
    
    // Cross-neuromodulator interactions
    float totalMod = totalModulation_;
    if (totalMod > pImpl->saturationLevel) {
        // Apply saturation effects
        float saturationFactor = 1.0f - (totalMod - pImpl->saturationLevel) / (1.0f - pImpl->saturationLevel);
        attentionEnhancement_ *= saturationFactor;
        workingMemoryEnhancement_ *= saturationFactor;
    }
}

} // namespace nlm