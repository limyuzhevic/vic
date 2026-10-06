#pragma once

// Enhanced neuromodulation system for NLM brain simulation
// Provides biologically realistic neuromodulation with advanced dynamics,
// cross-talk, and integration mechanisms

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <string>
#include <functional>
#include <random>

namespace nlm {

// ==================== NEUROMODULATION CONSTANTS ====================

namespace NeuromodulationConstants {
    // Dopamine dynamics
    constexpr float DOPAMINE_PEAK = 2.0f;                    // Maximum dopamine level
    constexpr float DOPAMINE_BASELINE = 0.0f;                // Baseline dopamine
    constexpr float DOPAMINE_DECAY_RATE = 0.2f;              // Decay per timestep
    constexpr float DOPAMINE_TIME_CONSTANT = 100.0f;         // Time constant (ms)
    constexpr float DOPAMINE_MEPSYNAPSE_CONVERSION = 0.1f;   // e.g.p to synaptic effect
    constexpr float DOPAMINE_RECEPTOR_DENSITY = 1.0f;        // Receptor density
    
    // Neuromodulator interactions
    constexpr float NEUROMODULATOR_CROSS_TALK_FACTOR = 0.1f;  // Cross-talk strength
    constexpr float NOVELTY_THRESHOLD_LOW = 0.1f;             // Low novelty threshold
    constexpr float NOVELTY_THRESHOLD_HIGH = 0.5f;            // High novelty threshold
    constexpr float NOVELTY_ADAPTATION_RATE = 0.01f;         // Adaptation rate
    
    // Learning modulation factors
    constexpr float PLASTICITY_MODULATION_RANGE = 0.5f;      // Range of plasticity modulation
    constexpr float NOVELTY_LEARNING_BOOST = 1.5f;           // Novelty boosts learning
    constexpr float DORMANT_LEARNING_FACTOR = 0.01f;        // Learning when dormant
    
    // State-dependent modulation
    constexpr float EXCITATION_MODULATION_FACTOR = 1.2f;    // Excitatory modulation
    constexpr float INHIBITION_MODULATION_FACTOR = 0.8f;    // Inhibitory modulation
    constexpr float CONSOLIDATION_MODULATION = 0.3f;        // Memory consolidation effect
    
    // Meta-learning parameters
    constexpr float ADAPTATION_RATE = 0.05f;                // Neuromodulator adaptation
    constexpr float STABILITY = 0.95f;                      // Stability factor
    constexpr float NOISE_VARIANCE = 0.01f;                 // Signal noise
}

// ==================== NEUROMODULATION UPDATE STRUCTURE ====================

struct NeuromodulationUpdate {
    float dopamine;        // Dopamine level
    float curiosity;       // Curiosity level
    float novelty;         // Novelty level
    float predictionError; // Prediction error level
    float reward;          // Reward signal
    
    NeuromodulationUpdate() : dopamine(0.0f), curiosity(0.0f), novelty(0.0f), 
                             predictionError(0.0f), reward(0.0f) {}
    
    // Apply neuromodulator interactions and cross-talk
    void applyInteractions() {
        // Dopamine-curiosity interaction (dopamine boosts curiosity)
        float dopamineCuriosityEffect = NeuromodulationConstants::NEUROMODULATOR_CROSS_TALK_FACTOR * dopamine;
        curiosity = std::min(1.0f, curiosity + dopamineCuriosityEffect * 0.5f);
        
        // Novelty-dopamine interaction (novelty affects dopamine)
        float noveltyDopamineEffect = NeuromodulationConstants::NEUROMODULATOR_CROSS_TALK_FACTOR * novelty;
        dopamine = std::max(0.0f, dopamine + noveltyDopamineEffect * 0.3f);
        
        // Curiosity-novelty interaction (curiosity increases sensitivity to novelty)
        float curiosityNoveltyEffect = NeuromodulationConstants::NEUROMODULATOR_CROSS_TALK_FACTOR * curiosity;
        novelty = std::min(1.0f, novelty + curiosityNoveltyEffect * 0.2f);
        
        // Prediction error-dopamine interaction (canonical)
        dopamine = std::max(0.0f, dopamine + predictionError * 0.5f);
        
        // Normalize all values to [0, 1]
        dopamine = std::clamp(dopamine, 0.0f, 1.0f);
        curiosity = std::clamp(curiosity, 0.0f, 1.0f);
        novelty = std::clamp(novelty, 0.0f, 1.0f);
        reward = std::clamp(reward, 0.0f, 1.0f);
    }
    
    // Apply neuromodulator effects to brain state
    void applyEffects(Brain* brain) {
        if (!brain) return;
        
        // Dopamine effects on neural excitability and plasticity
        float dopamineExcitabilityMod = 1.0f + NeuromodulationConstants::EXCITATION_MODULATION_FACTOR * dopamine;
        float dopaminePlasticityMod = 1.0f + NeuromodulationConstants::PLASTICITY_MODULATION_RANGE * dopamine;
        
        // Apply to neurons
        auto* spikeSystem = brain->getSpikeSystem();
        if (spikeSystem) {
            spikeSystem->setExcitabilityFactor(dopamineExcitabilityMod);
            spikeSystem->setPlasticityModulator(dopaminePlasticityMod);
        }
        
        // Neuromodulator effects on memory
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            workingMemory->setModulation(dopamine * 0.5f);
        }
        
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            episodicMemory->setConsolidationModulation(NeuromodulationConstants::CONSOLIDATION_MODULATION * dopamine);
        }
        
        // Curiosity effects on attention and exploration
        float curiosityAttentionMod = 1.0f + (curiosity * 0.5f);
        float curiosityExplorationMod = 1.0f + (curiosity * 0.3f);
        
        auto* attention = brain->getAttention();
        if (attention) {
            attention->setAttentionModulator(curiosityAttentionMod);
            attention->setExplorationModulator(curiosityExplorationMod);
        }
        
        // Novelty effects on concept formation
        float noveltyConceptMod = 1.0f + (novelty * NeuromodulationConstants::NOVELTY_LEARNING_BOOST);
        
        auto* conceptFormation = brain->getConceptFormation();
        if (conceptFormation) {
            conceptFormation->setNoveltyModulator(noveltyConceptMod);
        }
        
        // Prediction error effects on planning
        float predictionErrorPlanningMod = 1.0f + (predictionError * 0.5f);
        
        auto* planner = brain->getPlanner();
        if (planner) {
            planner->setPredictionErrorModulator(predictionErrorPlanningMod);
        }
    }
};

// ==================== ENHANCED DOPAMINE SYSTEM ====================

class DopamineEnhanced {
public:
    DopamineEnhanced() {
        initializeParameters();
    }
    
    void initialize(Brain* brain) {
        brain_ = brain;
        reset();
    }
    
    void update(TimestepDuration dt) {
        // Real dopamine dynamics with multiple channels
        updateBurstDynamics(dt);
        updatePredictionErrorLearning(dt);
        updateReceptorDynamics(dt);
        updateBackgroundNoise();
        
        // Apply effects
        if (brain_) {
            applyToBrainState();
        }
    }
    
    void signalReward(float reward) {
        // Reward signaling with temporal difference learning
        float rewardPrediction = rewardExpected_;
        float rewardError = reward - rewardPrediction;
        
        // Update prediction
        rewardExpected_ = reward;
        
        // Dopamine burst for unexpected rewards
        if (rewardError > 0.0f) {
            // Phasic response to unexpected reward
            dopaminePhasic_ = std::min(NeuromodulationConstants::DOPAMINE_PEAK,
                                      dopaminePhasic_ + rewardError * 2.0f);
        } else {
            // Dopamine dip for omitted expected rewards
            dopaminePhasic_ = std::max(0.0f, dopaminePhasic_ + rewardError * 0.5f);
        }
    }
    
    void signalPredictionError(float predictionError) {
        // Canonical reward prediction error
        float scaledError = predictionError * NeuromodulationConstants::DOPAMINE_MEPSYNAPSE_CONVERSION;
        
        // Dopamine phasic response to prediction error
        dopaminePhasic_ = std::clamp(scaledError, -0.5f, NeuromodulationConstants::DOPAMINE_PEAK);
        
        // Update expectation
        rewardPrediction_ += scaledError;
    }
    
    float getLevel() const {
        // Combined tonic and phasic dopamine
        float tonic = dopamineTonic_ * NeuromodulationConstants::STABILITY;
        return tonic + dopaminePhasic_ * 0.3f;
    }
    
    float getPlasticityFactor() const {
        // More sophisticated plasticity factor
        float level = getLevel();
        
        // Nonlinear relationship: low dopamine facilitates, high dopamine potentiates
        if (level < 0.3f) {
            return 1.0f + level * 2.0f;      // Facilitation at low levels
        } else if (level < 0.7f) {
            return 1.5f;                     // Potentiation at moderate levels
        } else {
            return 1.5f + (level - 0.7f) * 1.0f; // Strong potentiation at high levels
        }
    }
    
    float getExcitabilityModulator() const {
        // Excitability modulation by dopamine
        float level = getLevel();
        return NeuromodulationConstants::EXCITATION_MODULATION_FACTOR * (1.0f + level * 0.5f);
    }
    
    void reset() {
        dopamineTonic_ = NeuromodulationConstants::DOPAMINE_BASELINE;
        dopaminePhasic_ = 0.0f;
        rewardExpected_ = 0.0f;
        rewardPrediction_ = 0.0f;
        receptorActivation_ = 0.0f;
        receptorInactivation_ = 0.0f;
    }
    
    void setAdaptationRate(float rate) {
        adaptationRate_ = std::clamp(rate, 0.001f, 1.0f);
    }
    
    void setBaseline(float baseline) {
        dopamineTonic_ = baseline;
    }
    
    void setPeak(float peak) {
        maxPhasicResponse_ = peak;
    }
    
protected:
    void initializeParameters() {
        dopamineTonic_ = NeuromodulationConstants::DOPAMINE_BASELINE;
        dopaminePhasic_ = 0.0f;
        rewardExpected_ = 0.0f;
        rewardPrediction_ = 0.0f;
        receptorActivation_ = 0.0f;
        receptorInactivation_ = 0.0f;
        adaptationRate_ = NeuromodulationConstants::ADAPTATION_RATE;
        maxPhasicResponse_ = NeuromodulationConstants::DOPAMINE_PEAK;
        brain_ = nullptr;
        
        // Initialize random number generator for noise
        std::random_device rd;
        rng_.seed(rd());
        noiseDist_ = std::normal_distribution<float>(0.0f, NeuromodulationConstants::NOISE_VARIANCE);
    }
    
    void updateBurstDynamics(TimestepDuration dt) {
        // Phasic dynamics (fast)
        dopaminePhasic_ *= (1.0f - NeuromodulationConstants::DOPAMINE_DECAY_RATE * static_cast<float>(dt));
        
        // Bounded by maximum response
        dopaminePhasic_ = std::clamp(dopaminePhasic_, -maxPhasicResponse_, maxPhasicResponse_);
        
        // Add some biological noise
        dopaminePhasic_ += noiseDist_(rng_);
    }
    
    void updatePredictionErrorLearning(TimestepDuration dt) {
        // Learning dynamics based on prediction error
        float learningRate = adaptationRate_;
        
        // Dopamine can facilitate learning
        float learningModulator = getPlasticityFactor();
        learningRate *= learningModulator;
        
        // Update predictions
        rewardPrediction_ += learningRate * (rewardExpected_ - rewardPrediction_);
    }
    
    void updateReceptorDynamics(TimestepDuration dt) {
        // Receptor activation dynamics
        float activationRate = 1.0f / NeuromodulationConstants::DOPAMINE_TIME_CONSTANT;
        
        // Activation by dopamine
        receptorActivation_ = std::max(0.0f, receptorActivation_ + (1.0f - receptorActivation_) * activationRate * getLevel() * dt);
        
        // Inactivation by dopamine
        float inactivationRate = 1.0f / (NeuromodulationConstants::DOPAMINE_TIME_CONSTANT * 2.0f);
        receptorInactivation_ = std::min(1.0f, receptorInactivation_ + (NeuromodulationConstants::STABILITY - receptorInactivation_) * inactivationRate * dt);
    }
    
    void updateBackgroundNoise() {
        // Background noise fluctuations
        noiseDist_ = std::normal_distribution<float>(0.0f, NeuromodulationConstants::NOISE_VARIANCE);
    }
    
    void applyToBrainState() {
        if (!brain_ || !brain_->getSTDP()) return;
        
        auto* stdp = brain_->getSTDP();
        
        // Apply dopamine effects to STDP parameters
        float dopamineLevel = getLevel();
        
        // Dopamine affects STDP balance
        float originalLTP = stdp->getLTPWeight();
        float originalLTD = stdp->getLTDWeight();
        
        // Dopamine modulates LTP/LTD balance
        float ltpModulation = 1.0f + (dopamineLevel * 0.5f);
        float ltdModulation = 1.0f - (dopamineLevel * 0.3f);
        
        stdp->setLTPWeight(originalLTP * ltpModulation);
        stdp->setLTDWeight(originalLTD * ltdModulation);
        
        // Apply effects to other plasticity systems
        if (auto* hebbian = brain_->getHebbian()) {
            float hebbianMod = 1.0f + (dopamineLevel * 0.3f);
            // Hebbian doesn't have setLearningRate, but affects plasticity
        }
        
        if (auto* structuralPlasticity = brain_->getStructuralPlasticity()) {
            // Dopamine affects structural plasticity
            float synaptogenesisMod = 1.0f + (dopamineLevel * 0.4f);
            float pruningMod = 1.0f - (dopamineLevel * 0.2f);
            
            structuralPlasticity->setSynaptogenesisRate(
                structuralPlasticity->getSynaptogenesisRate() * synaptogenesisMod);
            structuralPlasticity->setPruningRate(
                structuralPlasticity->getPruningRate() * pruningMod);
        }
    }
    
private:
    Brain* brain_;
    
    // Dopamine state variables
    float dopamineTonic_;           // Tonic dopamine level
    float dopaminePhasic_;         // Phasic dopamine response
    float rewardExpected_;          // Expected reward
    float rewardPrediction_;        // Predicted reward
    float receptorActivation_;      // Receptor activation level
    float receptorInactivation_;    // Receptor inactivation level
    
    // Learning and adaptation
    float adaptationRate_;          // Learning rate
    float maxPhasicResponse_;       // Maximum phasic response
    
    // Noise and random generation
    std::mt19937 rng_;
    std::normal_distribution<float> noiseDist_;
};

// ==================== ENHANCED CURIOUSITY SYSTEM ====================

class CuriosityEnhanced {
public:
    CuriosityEnhanced() {
        initializeParameters();
    }
    
    void initialize(Brain* brain) {
        brain_ = brain;
        reset();
    }
    
    void update(float novelty, float predictionError, TimestepDuration dt) {
        // Enhanced curiosity dynamics with multiple components
        updateIntrinsicCuriosity(dt);
        updateExtrinsicFactors(novelty, predictionError, dt);
        updateWorkingMemoryIntegration();
        updateAttentionIntegration(dt);
        
        // Apply effects
        if (brain_) {
            applyToBrainState();
        }
    }
    
    float getLevel() const {
        // Combine different curiosity components
        float totalCuriosity = curiosityIntrinsic_;
        
        // Add extrinsic components weighted by importance
        totalCuriosity += curiosityNovelty_ * noveltyWeight_ * 0.3f;
        totalCuriosity += curiosityPredictionError_ * predictionErrorWeight_ * 0.4f;
        totalCuriosity += curiosityMemory_ * memoryWeight_ * 0.3f;
        
        return std::clamp(totalCuriosity, 0.0f, 1.0f);
    }
    
    float getExplorationDrive() const {
        // Exploration drive modulated by various factors
        float baseExploration = getLevel() * 0.8f;
        
        // Add memory-based exploration
        float memoryExploration = curiosityMemory_ * 0.5f;
        
        // Add prediction error-based exploration
        float predictionExploration = curiosityPredictionError_ * 0.6f;
        
        return baseExploration + memoryExploration + predictionExploration;
    }
    
    void reset() {
        curiosityIntrinsic_ = 0.0f;
        curiosityNovelty_ = 0.0f;
        curiosityPredictionError_ = 0.0f;
        curiosityMemory_ = 0.0f;
        explorationTarget_ = 0.0f;
    }
    
    void setNoveltyWeight(float weight) {
        noveltyWeight_ = std::clamp(weight, 0.0f, 1.0f);
    }
    
    void setPredictionErrorWeight(float weight) {
        predictionErrorWeight_ = std::clamp(weight, 0.0f, 1.0f);
    }
    
    void setMemoryWeight(float weight) {
        memoryWeight_ = std::clamp(weight, 0.0f, 1.0f);
    }
    
    void setAdaptationRate(float rate) {
        adaptationRate_ = std::clamp(rate, 0.001f, 1.0f);
    }
    
protected:
    void initializeParameters() {
        curiosityIntrinsic_ = 0.0f;
        curiosityNovelty_ = 0.0f;
        curiosityPredictionError_ = 0.0f;
        curiosityMemory_ = 0.0f;
        explorationTarget_ = 0.0f;
        noveltyWeight_ = 0.5f;
        predictionErrorWeight_ = 0.5f;
        memoryWeight_ = 0.5f;
        adaptationRate_ = NeuromodulationConstants::ADAPTATION_RATE;
        brain_ = nullptr;
        
        // Initialize random number generator
        std::random_device rd;
        rng_.seed(rd());
    }
    
    void updateIntrinsicCuriosity(TimestepDuration dt) {
        // Intrinsic curiosity dynamics (motive force)
        float targetLevel = explorationTarget_;
        
        // Smooth approach to target
        curiosityIntrinsic_ += (targetLevel - curiosityIntrinsic_) * adaptationRate_ * dt;
        
        // Decay towards zero if no strong drives
        curiosityIntrinsic_ *= NeuromodulationConstants::STABILITY;
        
        // Add biological noise
        std::normal_distribution<float> noise(0.0f, NeuromodulationConstants::NOISE_VARIANCE);
        curiosityIntrinsic_ += noise(rng_);
    }
    
    void updateExtrinsicFactors(float novelty, float predictionError, TimestepDuration dt) {
        // Novelty-based curiosity
        float noveltyDrive = novelty * noveltyWeight_;
        curiosityNovelty_ += (noveltyDrive - curiosityNovelty_) * adaptationRate_ * dt;
        curiosityNovelty_ *= NeuromodulationConstants::STABILITY;
        
        // Prediction error-based curiosity
        float predictionDrive = std::abs(predictionError) * predictionErrorWeight_;
        curiosityPredictionError_ += (predictionDrive - curiosityPredictionError_) * adaptationRate_ * dt;
        curiosityPredictionError_ *= NeuromodulationConstants::STABILITY;
        
        // Memory-based curiosity (based on working memory)
        float memoryDrive = 0.0f;
        if (brain_ && brain_->getWorkingMemory()) {
            memoryDrive = brain_->getWorkingMemory()->getActiveTraces() * 0.01f * memoryWeight_;
        }
        curiosityMemory_ += (memoryDrive - curiosityMemory_) * adaptationRate_ * dt;
        curiosityMemory_ *= NeuromodulationConstants::STABILITY;
        
        // Combine factors
        explorationTarget_ = (curiosityIntrinsic_ + curiosityNovelty_ + curiosityPredictionError_ + curiosityMemory_) / 4.0f;
    }
    
    void updateWorkingMemoryIntegration() {
        // Curiosity integrates with working memory
        if (!brain_ || !brain_->getWorkingMemory()) return;
        
        auto* workingMemory = brain_->getWorkingMemory();
        size_t activeTraces = workingMemory->getActiveTraces();
        
        // Higher working memory activity increases curiosity
        float memoryInfluence = static_cast<float>(activeTraces) * 0.01f;
        curiosityMemory_ = std::min(1.0f, curiosityMemory_ + memoryInfluence * 0.1f);
    }
    
    void updateAttentionIntegration(TimestepDuration dt) {
        // Curiosity integrates with attention system
        if (!brain_ || !brain_->getAttention()) return;
        
        auto* attention = brain_->getAttention();
        float attentionLevel = attention->getAttentionLevel();
        
        // Attention modulates curiosity
        curiosityIntrinsic_ *= (1.0f + attentionLevel * 0.2f);
        curiosityIntrinsic_ = std::min(1.0f, curiosityIntrinsic_);
    }
    
    void applyToBrainState() {
        if (!brain_ || !brain_->getWorkingMemory()) return;
        
        auto* workingMemory = brain_->getWorkingMemory();
        
        // Curiosity modulates working memory
        float curiosityMod = getLevel() * 0.5f;
        workingMemory->setModulation(workingMemory->getModulation() + curiosityMod * 0.1f);
        
        // Curiosity affects episodic memory
        auto* episodicMemory = brain_->getEpisodicMemory();
        if (episodicMemory) {
            // More curiosity leads to more episode storage
            float episodeStorageMod = 1.0f + getLevel() * 0.3f;
            episodicMemory->setStorageModulation(episodeStorageMod * 0.01f);
        }
    }
    
private:
    Brain* brain_;
    
    // Curiosity state variables
    float curiosityIntrinsic_;        // Intrinsic curiosity motive
    float curiosityNovelty_;         // Novelty-based curiosity
    float curiosityPredictionError_;  // Prediction error-based curiosity
    float curiosityMemory_;          // Memory-based curiosity
    float explorationTarget_;        // Target exploration level
    
    // Weight factors
    float noveltyWeight_;             // Weight for novelty factor
    float predictionErrorWeight_;    // Weight for prediction error factor
    float memoryWeight_;              // Weight for memory factor
    
    // Learning parameters
    float adaptationRate_;            // Adaptation rate
    
    // Random number generator
    std::mt19937 rng_;
};

// ==================== ENHANCED NOVELTY DETECTION SYSTEM ====================

class NoveltyEnhanced {
public:
    NoveltyEnhanced() {
        initializeParameters();
    }
    
    void initialize(Brain* brain) {
        brain_ = brain;
        reset();
    }
    
    void update(TimestepDuration dt) {
        // Enhanced novelty detection with multi-dimensional analysis
        updateMultiDimensionalNovelty();
        updateNoveltyAdaptation(dt);
        updateTemporalDynamics(dt);
        updateMemoryIntegration();
        
        // Apply effects
        if (brain_) {
            applyToBrainState();
        }
    }
    
    float getLevel() const {
        // Combine novelty detection channels
        float totalNovelty = noveltySensory_;
        totalNovelty += noveltyTemporal_ * temporalWeight_ * 0.3f;
        totalNovelty += noveltyPattern_ * patternWeight_ * 0.4f;
        totalNovelty += noveltyMemory_ * memoryWeight_ * 0.3f;
        
        return std::clamp(totalNovelty, 0.0f, 1.0f);
    }
    
    void detectNovelty(const Observation& observation, const Observation& previousObservation) {
        // Multi-dimensional novelty detection
        noveltySensory_ = computeSensoryNovelty(observation, previousObservation);
        noveltyTemporal_ = computeTemporalNovelty();
        noveltyPattern_ = computePatternNovelty();
        noveltyMemory_ = computeMemoryNovelty();
    }
    
    void detectNovelty(const std::vector<float>& currentPattern,
                       const std::vector<float>& previousPattern) {
        // Pattern-based novelty detection
        if (currentPattern.empty() || previousPattern.empty()) {
            noveltySensory_ = 0.0f;
            return;
        }
        
        // Compute multi-dimensional differences
        noveltySensory_ = computeMultiDimensionalNovelty(currentPattern, previousPattern);
        noveltyTemporal_ = computeTemporalNovelty();
        noveltyPattern_ = computePatternNovelty();
        noveltyMemory_ = computeMemoryNovelty();
    }
    
    const std::vector<float>& getHistory() const {
        return noveltyHistory_;
    }
    
    void clearHistory() {
        noveltyHistory_.clear();
    }
    
    void reset() {
        noveltySensory_ = 0.0f;
        noveltyTemporal_ = 0.0f;
        noveltyPattern_ = 0.0f;
        noveltyMemory_ = 0.0f;
        patternThreshold_ = NeuromodulationConstants::NOVELTY_THRESHOLD_LOW;
        adaptationTarget_ = 0.5f;
    }
    
    void setNoveltyThreshold(float threshold) {
        patternThreshold_ = std::clamp(threshold, 0.01f, 1.0f);
    }
    
    void setTemporalWeight(float weight) {
        temporalWeight_ = std::clamp(weight, 0.0f, 1.0f);
    }
    
    void setPatternWeight(float weight) {
        patternWeight_ = std::clamp(weight, 0.0f, 1.0f);
    }
    
    void setMemoryWeight(float weight) {
        memoryWeight_ = std::clamp(weight, 0.0f, 1.0f);
    }
    
protected:
    void initializeParameters() {
        noveltySensory_ = 0.0f;
        noveltyTemporal_ = 0.0f;
        noveltyPattern_ = 0.0f;
        noveltyMemory_ = 0.0f;
        patternThreshold_ = NeuromodulationConstants::NOVELTY_THRESHOLD_LOW;
        adaptationTarget_ = 0.5f;
        temporalWeight_ = 0.3f;
        patternWeight_ = 0.4f;
        memoryWeight_ = 0.3f;
        brain_ = nullptr;
        
        // Initialize random number generator
        std::random_device rd;
        rng_.seed(rd());
    }
    
    float computeMultiDimensionalNovelty(const std::vector<float>& current,
                                         const std::vector<float>& previous) const {
        if (current.size() != previous.size()) {
            return 0.5f; // Moderate novelty for pattern mismatch
        }
        
        float totalDifference = 0.0f;
        size_t compareLen = std::min(current.size(), previous.size());
        
        for (size_t i = 0; i < compareLen; ++i) {
            // Euclidean distance in feature space
            float diff = current[i] - previous[i];
            totalDifference += diff * diff;
        }
        
        // Normalize by pattern length and scale
        float euclideanDistance = std::sqrt(totalDifference);
        float normalizedNovelty = euclideanDistance / std::sqrt(static_cast<float>(compareLen) * 10.0f);
        
        // Apply threshold-based detection
        return std::min(1.0f, normalizedNovelty / patternThreshold_);
    }
    
    float computeSensoryNovelty(const Observation& observation,
                                const Observation& previousObservation) const {
        // Simple sensory novelty detection
        std::vector<float> currentFeatures;
        std::vector<float> previousFeatures;
        
        if (observation.getData().size() > previousObservation.getData().size()) {
            currentFeatures = observation.getData();
            previousFeatures = previousObservation.getData();
        } else {
            currentFeatures = previousObservation.getData();
            previousFeatures = observation.getData();
        }
        
        return computeMultiDimensionalNovelty(currentFeatures, previousFeatures);
    }
    
    float computeTemporalNovelty() const {
        // Temporal novelty based on recent history
        if (noveltyHistory_.size() < 2) {
            return 0.0f;
        }
        
        float current = noveltyHistory_.back();
        float previous = noveltyHistory_[noveltyHistory_.size() - 2];
        
        // Novelty of change over time
        float changeRate = std::abs(current - previous);
        
        // Higher rate of change indicates temporal novelty
        return std::min(1.0f, changeRate * 2.0f);
    }
    
    float computePatternNovelty() const {
        // Pattern novelty based on recurrent patterns
        if (patternHistory_.size() < 3) {
            return 0.0f;
        }
        
        // Check if current pattern resembles recent patterns
        float recentSimilarity = 0.0f;
        for (size_t i = patternHistory_.size() - 3; i < patternHistory_.size(); ++i) {
            recentSimilarity += patternHistory_[i];
        }
        recentSimilarity /= 3.0f;
        
        // Lower similarity indicates higher novelty
        return 1.0f - recentSimilarity;
    }
    
    float computeMemoryNovelty() const {
        // Memory-based novelty (based on working memory)
        if (!brain_ || !brain_->getWorkingMemory()) {
            return 0.0f;
        }
        
        auto* workingMemory = brain_->getWorkingMemory();
        size_t activeTraces = workingMemory->getActiveTraces();
        
        // More active traces indicate more novel information
        return std::min(1.0f, static_cast<float>(activeTraces) * 0.02f);
    }
    
    void updateMultiDimensionalNovelty() {
        // Combine different novelty sources
        float combinedNovelty = (noveltySensory_ + noveltyTemporal_ + noveltyPattern_ + noveltyMemory_) / 4.0f;
        
        // Store in history
        noveltyHistory_.push_back(combinedNovelty);
        
        // Limit history size
        if (noveltyHistory_.size() > 1000) {
            noveltyHistory_.erase(noveltyHistory_.begin());
        }
        
        // Update pattern history
        if (noveltySensory_ > 0.1f) {
            patternHistory_.push_back(noveltySensory_);
            if (patternHistory_.size() > 100) {
                patternHistory_.erase(patternHistory_.begin());
            }
        }
    }
    
    void updateNoveltyAdaptation(TimestepDuration dt) {
        // Novelty adapts threshold and sensitivity over time
        float adaptation = NeuromodulationConstants::NOVELTY_ADAPTATION_RATE * dt;
        
        // Adapt threshold towards target
        patternThreshold_ += (adaptationTarget_ - patternThreshold_) * adaptation;
        
        // Prevent extreme values
        patternThreshold_ = std::clamp(patternThreshold_, 0.01f, 1.0f);
    }
    
    void updateTemporalDynamics(TimestepDuration dt) {
        // Temporal dynamics of novelty
        // Add some temporal noise and dynamics
        std::normal_distribution<float> noise(0.0f, NeuromodulationConstants::NOISE_VARIANCE);
        
        // Smoothing
        noveltySensory_ = noveltySensory_ * (1.0f - adaptationRate_) + noise(rng_) * 0.1f;
        noveltyTemporal_ = noveltyTemporal_ * (1.0f - adaptationRate_) + noise(rng_) * 0.1f;
        noveltyPattern_ = noveltyPattern_ * (1.0f - adaptationRate_) + noise(rng_) * 0.1f;
        noveltyMemory_ = noveltyMemory_ * (1.0f - adaptationRate_) + noise(rng_) * 0.1f;
    }
    
    void updateMemoryIntegration() {
        // Novelty integrates with memory systems
        if (!brain_ || !brain_->getEpisodicMemory()) return;
        
        auto* episodicMemory = brain_->getEpisodicMemory();
        size_t episodeCount = episodicMemory->getEpisodeCount();
        
        // More episodes increase novelty detection ability
        float memoryExperience = std::min(1.0f, static_cast<float>(episodeCount) * 0.001f);
        noveltyMemory_ += (memoryExperience - noveltyMemory_) * 0.01f;
    }
    
    void applyToBrainState() {
        if (!brain_ || !brain_->getConceptFormation()) return;
        
        auto* conceptFormation = brain_->getConceptFormation();
        
        // Novelty affects concept formation
        float conceptMod = getLevel() * 0.5f;
        conceptFormation->setNoveltyModulator(conceptMod);
        
        // Novelty affects planning
        auto* planner = brain_->getPlanner();
        if (planner) {
            float planningMod = 1.0f + getLevel() * 0.3f;
            planner->setExplorationModulator(planningMod);
        }
    }
    
private:
    Brain* brain_;
    
    // Novelty detection state variables
    float noveltySensory_;          // Sensory-based novelty
    float noveltyTemporal_;        // Temporal-based novelty
    float noveltyPattern_;         // Pattern-based novelty
    float noveltyMemory_;          // Memory-based novelty
    
    // History and thresholds
    std::vector<float> noveltyHistory_;   // Novelty history
    std::vector<float> patternHistory_;   // Pattern similarity history
    float patternThreshold_;               // Novelty threshold
    float adaptationTarget_;               // Target adaptation level
    
    // Weight factors
    float temporalWeight_;          // Weight for temporal novelty
    float patternWeight_;           // Weight for pattern novelty
    float memoryWeight_;            // Weight for memory novelty
    
    // Learning parameters
    float adaptationRate_;          // Adaptation rate
    
    // Random number generator
    std::mt19937 rng_;
};

// ==================== ENHANCED PREDICTION ERROR SYSTEM ====================

class PredictionErrorEnhanced {
public:
    PredictionErrorEnhanced() {
        initializeParameters();
    }
    
    void initialize(Brain* brain) {
        brain_ = brain;
        reset();
    }
    
    void update(const Observation& currentObservation,
                const Observation& expectedObservation) {
        // Enhanced prediction error computation
        updatePredictionError(currentObservation, expectedObservation);
        updateErrorHistory();
        updateErrorAdaptation();
        
        // Apply effects
        if (brain_) {
            applyToBrainState();
        }
    }
    
    float getLevel() const {
        return predictionErrorLevel_;
    }
    
    const std::vector<float>& getHistory() const {
        return predictionErrorHistory_;
    }
    
    void clearHistory() {
        predictionErrorHistory_.clear();
    }
    
    void reset() {
        predictionErrorLevel_ = 0.0f;
        predictionErrorMagnitude_ = 0.0f;
        predictionErrorSign_ = 1.0f; // 1.0 for positive, -1.0 for negative
        adaptationTarget_ = 0.0f;
        noiseLevel_ = 0.0f;
    }
    
    void setLearningRate(float rate) {
        learningRate_ = std::clamp(rate, 0.001f, 1.0f);
    }
    
    void setAdaptationRate(float rate) {
        adaptationRate_ = std::clamp(rate, 0.001f, 1.0f);
    }
    
    void setNoiseLevel(float noise) {
        noiseLevel_ = std::clamp(noise, 0.0f, 1.0f);
    }
    
protected:
    void initializeParameters() {
        predictionErrorLevel_ = 0.0f;
        predictionErrorMagnitude_ = 0.0f;
        predictionErrorSign_ = 1.0f;
        adaptationTarget_ = 0.0f;
        learningRate_ = 0.1f;
        adaptationRate_ = NeuromodulationConstants::ADAPTATION_RATE;
        noiseLevel_ = NeuromodulationConstants::NOISE_VARIANCE;
        brain_ = nullptr;
        
        // Initialize random number generator
        std::random_device rd;
        rng_.seed(rd());
    }
    
    void updatePredictionError(const Observation& currentObservation,
                               const Observation& expectedObservation) {
        // Compute prediction error using multiple methods
        float sensoryError = computeSensoryPredictionError(currentObservation, expectedObservation);
        float conceptualError = computeConceptualPredictionError(currentObservation, expectedObservation);
        
        // Combine errors with different weights
        float combinedError = sensoryError * 0.7f + conceptualError * 0.3f;
        
        // Apply noise
        std::normal_distribution<float> noise(0.0f, noiseLevel_);
        combinedError += noise(rng_);
        
        // Update prediction error state
        predictionErrorLevel_ += (combinedError - predictionErrorLevel_) * learningRate_;
        predictionErrorMagnitude_ = std::abs(predictionErrorLevel_);
        predictionErrorSign_ = predictionErrorLevel_ >= 0.0f ? 1.0f : -1.0f;
        
        // Store in history
        predictionErrorHistory_.push_back(predictionErrorLevel_);
        if (predictionErrorHistory_.size() > 1000) {
            predictionErrorHistory_.erase(predictionErrorHistory_.begin());
        }
    }
    
    void updateErrorHistory() {
        // Maintain prediction error history for learning
        // History is updated in updatePredictionError
    }
    
    void updateErrorAdaptation() {
        // Adapt prediction error computation over time
        float adaptation = adaptationRate_ * 0.01f; // Slower adaptation for prediction error
        
        // Adapt target based on recent errors
        float recentAverage = 0.0f;
        size_t historySize = std::min(size_t(100), predictionErrorHistory_.size());
        if (historySize > 0) {
            float total = 0.0f;
            for (size_t i = predictionErrorHistory_.size() - historySize; i < predictionErrorHistory_.size(); ++i) {
                total += std::abs(predictionErrorHistory_[i]);
            }
            recentAverage = total / static_cast<float>(historySize);
        }
        
        adaptationTarget_ += (recentAverage - adaptationTarget_) * adaptation;
    }
    
    float computeSensoryPredictionError(const Observation& current,
                                        const Observation& expected) const {
        // Compare sensory features
        std::vector<float> currentFeatures = current.getData();
        std::vector<float> expectedFeatures = expected.getData();
        
        if (currentFeatures.empty() || expectedFeatures.empty()) {
            return 0.0f;
        }
        
        // Use the larger feature vector as reference
        if (currentFeatures.size() > expectedFeatures.size()) {
            std::swap(currentFeatures, expectedFeatures);
        }
        
        // Compute weighted error
        float totalError = 0.0f;
        size_t compareLen = std::min(currentFeatures.size(), expectedFeatures.size());
        
        for (size_t i = 0; i < compareLen; ++i) {
            float diff = currentFeatures[i] - expectedFeatures[i];
            totalError += diff * diff;
        }
        
        // Normalize
        float normalizedError = std::sqrt(totalError) / std::sqrt(static_cast<float>(compareLen) * 10.0f);
        return std::min(1.0f, normalizedError * predictionErrorSign_);
    }
    
    float computeConceptualPredictionError(const Observation& current,
                                           const Observation& expected) const {
        // Conceptual prediction error based on higher-level features
        if (!brain_ || !brain_->getConceptFormation()) {
            return 0.0f;
        }
        
        auto* conceptFormation = brain_->getConceptFormation();
        
        // Get concept representations
        std::vector<float> currentConcepts = conceptFormation->getConceptRepresentation(current);
        std::vector<float> expectedConcepts = conceptFormation->getConceptRepresentation(expected);
        
        if (currentConcepts.empty() || expectedConcepts.empty()) {
            return 0.0f;
        }
        
        // Compute conceptual error
        float totalError = 0.0f;
        size_t compareLen = std::min(currentConcepts.size(), expectedConcepts.size());
        
        for (size_t i = 0; i < compareLen; ++i) {
            float diff = currentConcepts[i] - expectedConcepts[i];
            totalError += diff * diff;
        }
        
        return std::min(1.0f, std::sqrt(totalError) / std::sqrt(static_cast<float>(compareLen) * 2.0f));
    }
    
    void applyToBrainState() {
        if (!brain_ || !brain_->getPredictionSystem()) return;
        
        auto* predictionSystem = brain_->getPredictionSystem();
        
        // Prediction error affects prediction system
        float errorMod = predictionErrorMagnitude_ * 0.5f;
        predictionSystem->setErrorModulator(errorMod);
        
        // Prediction error affects planning
        auto* planner = brain_->getPlanner();
        if (planner) {
            float planningMod = 1.0f + predictionErrorMagnitude_ * 0.3f;
            planner->setPredictionErrorModulator(planningMod);
        }
    }
    
private:
    Brain* brain_;
    
    // Prediction error state variables
    float predictionErrorLevel_;     // Current prediction error level
    float predictionErrorMagnitude_; // Absolute magnitude of prediction error
    float predictionErrorSign_;      // Sign of prediction error (+1 or -1)
    
    // Learning and adaptation parameters
    float learningRate_;              // Learning rate
    float adaptationRate_;            // Adaptation rate
    float noiseLevel_;                // Noise level
    
    // History and targets
    std::vector<float> predictionErrorHistory_; // Prediction error history
    float adaptationTarget_;               // Target adaptation level
    
    // Random number generator
    std::mt19937 rng_;
};

// ==================== ENHANCED NEUROMODULATOR MANAGER ====================

class NeuromodulatorManagerEnhanced {
public:
    NeuromodulatorManagerEnhanced() {
        initializeEnhancedSystem();
    }
    
    ~NeuromodulatorManagerEnhanced() {
        // Clean up enhanced neuromodulators
        delete enhancedDopamine_;
        delete enhancedCuriosity_;
        delete enhancedNovelty_;
        delete enhancedPredictionError_;
    }
    
    void initializeEnhancedSystem() {
        // Initialize enhanced neuromodulators
        enhancedDopamine_ = new DopamineEnhanced();
        enhancedCuriosity_ = new CuriosityEnhanced();
        enhancedNovelty_ = new NoveltyEnhanced();
        enhancedPredictionError_ = new PredictionErrorEnhanced();
        
        // Set up cross-modulation
        setupCrossModulation();
    }
    
    void updateEnhanced(Brain* brain,
                        const Observation& currentObservation,
                        const Observation& expectedObservation) {
        if (!brain) return;
        
        // Update each neuromodulator with enhanced dynamics
        float novelty = enhancedNovelty_->getLevel();
        float predictionError = enhancedPredictionError_->getLevel();
        
        enhancedDopamine_->update(0.001f); // Default timestep
        enhancedCuriosity_->update(novelty, predictionError, 0.001f);
        enhancedNovelty_->update(0.001f);
        enhancedPredictionError_->update(currentObservation, expectedObservation);
        
        // Create combined update
        NeuromodulationUpdate update;
        update.dopamine = enhancedDopamine_->getLevel();
        update.curiosity = enhancedCuriosity_->getLevel();
        update.novelty = novelty;
        update.predictionError = predictionError;
        update.reward = 0.0f; // Would come from external reward system
        
        // Apply interactions and effects
        update.applyInteractions();
        update.applyEffects(brain);
    }
    
    NeuromodulationUpdate getCurrentUpdate() const {
        NeuromodulationUpdate update;
        update.dopamine = enhancedDopamine_->getLevel();
        update.curiosity = enhancedCuriosity_->getLevel();
        update.novelty = enhancedNovelty_->getLevel();
        update.predictionError = enhancedPredictionError_->getLevel();
        return update;
    }
    
    void reset() {
        enhancedDopamine_->reset();
        enhancedCuriosity_->reset();
        enhancedNovelty_->reset();
        enhancedPredictionError_->reset();
    }
    
    void setEnhancedParameters(const std::string& neuromodulatorType,
                               const std::string& parameter,
                               float value) {
        if (neuromodulatorType == "dopamine") {
            if (parameter == "baseline") {
                enhancedDopamine_->setBaseline(value);
            } else if (parameter == "peak") {
                enhancedDopamine_->setPeak(value);
            } else if (parameter == "adaptation_rate") {
                enhancedDopamine_->setAdaptationRate(value);
            }
        } else if (neuromodulatorType == "curiosity") {
            if (parameter == "novelty_weight") {
                enhancedCuriosity_->setNoveltyWeight(value);
            } else if (parameter == "prediction_error_weight") {
                enhancedCuriosity_->setPredictionErrorWeight(value);
            } else if (parameter == "memory_weight") {
                enhancedCuriosity_->setMemoryWeight(value);
            } else if (parameter == "adaptation_rate") {
                enhancedCuriosity_->setAdaptationRate(value);
            }
        } else if (neuromodulatorType == "novelty") {
            if (parameter == "threshold") {
                enhancedNovelty_->setNoveltyThreshold(value);
            } else if (parameter == "temporal_weight") {
                enhancedNovelty_->setTemporalWeight(value);
            } else if (parameter == "pattern_weight") {
                enhancedNovelty_->setPatternWeight(value);
            } else if (parameter == "memory_weight") {
                enhancedNovelty_->setMemoryWeight(value);
            }
        } else if (neuromodulatorType == "prediction_error") {
            if (parameter == "learning_rate") {
                enhancedPredictionError_->setLearningRate(value);
            } else if (parameter == "adaptation_rate") {
                enhancedPredictionError_->setAdaptationRate(value);
            } else if (parameter == "noise_level") {
                enhancedPredictionError_->setNoiseLevel(value);
            }
        }
    }
    
    std::string getEnhancedParametersReport() const {
        std::ostringstream oss;
        oss << "Enhanced Neuromodulation System Report:" << std::endl;
        
        oss << "Dopamine:" << std::endl;
        oss << "  Level: " << enhancedDopamine_->getLevel() << std::endl;
        oss << "  Plasticity Factor: " << enhancedDopamine_->getPlasticityFactor() << std::endl;
        oss << "  Excitability Modulator: " << enhancedDopamine_->getExcitabilityModulator() << std::endl;
        
        oss << "Curiosity:" << std::endl;
        oss << "  Level: " << enhancedCuriosity_->getLevel() << std::endl;
        oss << "  Exploration Drive: " << enhancedCuriosity_->getExplorationDrive() << std::endl;
        
        oss << "Novelty:" << std::endl;
        oss << "  Level: " << enhancedNovelty_->getLevel() << std::endl;
        oss << "  History Size: " << enhancedNovelty_->getHistory().size() << std::endl;
        
        oss << "Prediction Error:" << std::endl;
        oss << "  Level: " << enhancedPredictionError_->getLevel() << std::endl;
        oss << "  History Size: " << enhancedPredictionError_->getHistory().size() << std::endl;
        
        return oss.str();
    }
    
protected:
    void setupCrossModulation() {
        // Set up enhanced cross-modulation between neuromodulators
        // This creates complex interactions that mimic biological systems
        
        // Dopamine influences curiosity
        enhancedCuriosity_->setNoveltyWeight(0.5f + enhancedDopamine_->getLevel() * 0.3f);
        enhancedCuriosity_->setPredictionErrorWeight(0.5f + enhancedDopamine_->getLevel() * 0.2f);
        
        // Novelty influences dopamine
        enhancedNovelty_->setNoveltyThreshold(0.3f - enhancedNovelty_->getLevel() * 0.2f);
        
        // Prediction error influences novelty detection
        enhancedNovelty_->setPatternWeight(0.4f + enhancedPredictionError_->getLevel() * 0.2f);
    }
    
private:
    DopamineEnhanced* enhancedDopamine_;
    CuriosityEnhanced* enhancedCuriosity_;
    NoveltyEnhanced* enhancedNovelty_;
    PredictionErrorEnhanced* enhancedPredictionError_;
};

// ==================== ENHANCED NEUROMODULATOR INTERFACE ====================

// Enhanced neuromodulator interface that maintains backward compatibility
// while providing access to enhanced functionality

class NeuromodulatorEnhancedInterface {
public:
    NeuromodulatorEnhancedInterface() {
        manager_ = new NeuromodulatorManagerEnhanced();
    }
    
    ~NeuromodulatorEnhancedInterface() {
        delete manager_;
    }
    
    void initialize(Brain* brain) {
        manager_->getDopamine()->initialize(brain);
        manager_->getCuriosity()->initialize(brain);
        manager_->getNovelty()->initialize(brain);
        manager_->getPredictionError()->initialize(brain);
    }
    
    void update(Brain* brain,
                const Observation& currentObservation,
                const Observation& expectedObservation) {
        manager_->updateEnhanced(brain, currentObservation, expectedObservation);
    }
    
    // Backward compatibility methods
    void update(float novelty, float predictionError, TimestepDuration dt) {
        // Enhanced update with enhanced curiosity
        float currentPredictionError = manager_->getPredictionError()->getLevel();
        manager_->getCuriosity()->update(novelty, currentPredictionError, dt);
    }
    
    void detectNovelty(const Observation& observation,
                       const Observation& previousObservation) {
        manager_->getNovelty()->detectNovelty(observation, previousObservation);
    }
    
    void detectNovelty(const std::vector<float>& currentPattern,
                       const std::vector<float>& previousPattern) {
        manager_->getNovelty()->detectNovelty(currentPattern, previousPattern);
    }
    
    float getDopamineLevel() const {
        return manager_->getDopamine()->getLevel();
    }
    
    float getCuriosityLevel() const {
        return manager_->getCuriosity()->getLevel();
    }
    
    float getNoveltyLevel() const {
        return manager_->getNovelty()->getLevel();
    }
    
    float getPredictionErrorLevel() const {
        return manager_->getPredictionError()->getLevel();
    }
    
    const std::vector<float>& getNoveltyHistory() const {
        return manager_->getNovelty()->getHistory();
    }
    
    const std::vector<float>& getPredictionErrorHistory() const {
        return manager_->getPredictionError()->getHistory();
    }
    
    void reset() {
        manager_->reset();
    }
    
    void setEnhancedParameter(const std::string& neuromodulatorType,
                              const std::string& parameter,
                              float value) {
        manager_->setEnhancedParameter(neuromodulatorType, parameter, value);
    }
    
    std::string getReport() const {
        return manager_->getEnhancedParametersReport();
    }
    
    NeuromodulationUpdate getCurrentUpdate() const {
        return manager_->getCurrentUpdate();
    }
    
protected:
    NeuromodulatorManagerEnhanced* manager_;
};

// ==================== GLOBAL ENHANCED NEUROMODULATOR INSTANCE ====================

inline NeuromodulatorEnhancedInterface& getEnhancedNeuromodulatorInterface() {
    static NeuromodulatorEnhancedInterface interface;
    return interface;
}

// ==================== CONVENIENCE FUNCTIONS ====================

// Get enhanced neuromodulator interface
inline NeuromodulatorEnhancedInterface& getEnhancedNeuromodulators() {
    return getEnhancedNeuromodulatorInterface();
}

// Initialize enhanced neuromodulators
inline void initializeEnhancedNeuromodulators(Brain* brain) {
    getEnhancedNeuromodulators().initialize(brain);
}

// Update enhanced neuromodulators
inline void updateEnhancedNeuromodulators(Brain* brain,
                                           const Observation& currentObservation,
                                           const Observation& expectedObservation) {
    getEnhancedNeuromodulators().update(brain, currentObservation, expectedObservation);
}

// Enhanced compatibility functions
inline void updateEnhancedNeuromodulators(float novelty, float predictionError, TimestepDuration dt) {
    getEnhancedNeuromodulators().update(novelty, predictionError, dt);
}

inline void detectEnhancedNovelty(const Observation& observation,
                                 const Observation& previousObservation) {
    getEnhancedNeuromodulators().detectNovelty(observation, previousObservation);
}

inline void detectEnhancedNovelty(const std::vector<float>& currentPattern,
                                 const std::vector<float>& previousPattern) {
    getEnhancedNeuromodulators().detectNovelty(currentPattern, previousPattern);
}

// Get current enhanced neuromodulation update
inline NeuromodulationUpdate getCurrentEnhancedUpdate() {
    return getEnhancedNeuromodulators().getCurrentUpdate();
}

// Set enhanced neuromodulator parameters
inline void setEnhancedNeuromodulatorParameter(const std::string& neuromodulatorType,
                                                const std::string& parameter,
                                                float value) {
    getEnhancedNeuromodulators().setEnhancedParameter(neuromulatorType, parameter, value);
}

// Get enhanced neuromodulation report
inline std::string getEnhancedNeuromodulatorReport() {
    return getEnhancedNeuromodulators().getReport();
}

} // namespace nlm

// Include the original enhanced neuromodulator headers for backward compatibility
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Reward.hpp"