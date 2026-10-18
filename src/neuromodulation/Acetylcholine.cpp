#include "Acetylcholine.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float tonicLevel;
    float baselineLevel;
    float phasicLevel;
    float peak;
    float decayRate;
    float releaseRate;
    float recoveryRate;
    float taskDemandSensitivity;
    float noveltySensitivity;
    float workingMemoryBoost;
    float plasticityBoost;
    
    // Phasic response dynamics
    float phasicStartTime;
    float phasicDuration;
    float phasicPeak;
    float phasicDecayRate;
    bool isPhasicActive;
    
    // Integration state
    float rewardAttentionBalance;
    float explorationExploitation;
    float predictionUncertainty;
    
    // History tracking
    std::vector<float> history;
    std::vector<float> attentionalGains;
    std::vector<float> workingMemoryRetention;
    std::vector<float> plasticityFactors;
    
    // Random generator for novelty-based plasticity
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> normalDist;
    
    // Statistics
    size_t totalPhasicEvents;
    size_t successfulAttentionModulations;
    size_t memoryConsolidationEvents;
    
    Impl()
        : brain(nullptr)
        , level(0.05f)              // Baseline ACh level
        , tonicLevel(0.05f)
        , baselineLevel(0.05f)
        , phasicLevel(0.0f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(2.0f)
        , recoveryRate(0.1f)
        , taskDemandSensitivity(0.8f)
        , noveltySensitivity(0.7f)
        , workingMemoryBoost(0.5f)
        , plasticityBoost(0.6f)
        , phasicStartTime(0.0f)
        , phasicDuration(50.0f)    // 50 time units
        , phasicPeak(0.0f)
        , phasicDecayRate(0.2f)
        , isPhasicActive(false)
        , rewardAttentionBalance(0.5f)
        , explorationExploitation(0.5f)
        , predictionUncertainty(0.1f)
        , totalPhasicEvents(0)
        , successfulAttentionModulations(0)
        , memoryConsolidationEvents(0)
    {
        // Initialize random generator with time-based seed
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        rng.seed(static_cast<unsigned int>(seed));
        uniformDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
        normalDist = std::normal_distribution<float>(0.0f, 1.0f);
    }
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine neuromodulation system initialized");
    NLM_LOG_INFO("Baseline ACh level: 0.05, Peak: 1.0, Decay rate: 0.05");
}

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update history
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity during novel learning
    // Base plasticity factor of 1.0, enhanced by ACh level
    float basePlasticity = 1.0f;
    float achPlasticity = pImpl->plasticityBoost * pImpl->level;
    return basePlasticity + achPlasticity;
}

void Acetylcholine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Update phasic responses if active
    if (pImpl->isPhasicActive) {
        float timeSincePhasicStart = pImpl->phasicStartTime;
        if (timeSincePhasicStart < pImpl->phasicDuration) {
            // Phasic response is active - compute current level
            float timeProgress = timeSincePhasicStart / pImpl->phasicDuration;
            float phasicAmplitude = pImpl->phasicPeak * (1.0f - timeProgress);
            
            // Decay during phasic phase
            pImpl->phasicLevel = phasicAmplitude * std::exp(-pImpl->phasicDecayRate * timeSincePhasicStart);
            
            // Current level is phasic plus tonic
            setLevel(pImpl->phasicLevel + pImpl->tonicLevel);
        } else {
            // Phasic response ended - return to tonic level
            pImpl->isPhasicActive = false;
            pImpl->phasicLevel = 0.0f;
            applyTonicLevel(pImpl->baselineLevel);
        }
    } else {
        // No phasic activity - apply decay towards baseline
        applyDecay(pImpl->decayRate, dtFloat);
    }
    
    // Decay to baseline when not activated
    float targetLevel = pImpl->isPhasicActive ? 
        (pImpl->phasicLevel + pImpl->tonicLevel) : 
        pImpl->tonicLevel;
    
    // Smooth recovery to target
    float recoveryAmount = (targetLevel - pImpl->level) * pImpl->recoveryRate * dtFloat;
    pImpl->level += recoveryAmount;
    
    // Ensure level doesn't exceed bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Update history for analysis
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Acetylcholine::computeAttentionalGain(const std::vector<float>& taskDemands,
                                          std::vector<float>& gainFactors) {
    // Enhance signaling in attended neural populations
    // Based on task demands and current ACh level
    
    gainFactors.resize(taskDemands.size());
    float achBoost = 1.0f + pImpl->taskDemandSensitivity * pImpl->level;
    
    for (size_t i = 0; i < taskDemands.size(); ++i) {
        // Base gain from task demand
        float baseGain = taskDemands[i];
        
        // Enhance with ACh-dependent attention modulation
        float enhancedGain = baseGain * achBoost;
        
        // Ensure reasonable bounds
        enhancedGain = std::clamp(enhancedGain, 0.1f, 5.0f);
        
        gainFactors[i] = enhancedGain;
        
        // Record for statistics
        if (enhancedGain > 1.5f) {
            pImpl->successfulAttentionModulations++;
        }
    }
    
    // Store attentional gains for later analysis
    pImpl->attentionalGains.insert(pImpl->attentionalGains.end(),
                                  gainFactors.begin(), gainFactors.end());
    if (pImpl->attentionalGains.size() > 1000) {
        pImpl->attentionalGains.erase(pImpl->attentionalGains.begin(),
                                     pImpl->attentionalGains.begin() + 100);
    }
}

void Acetylcholine::enhanceWorkingMemory(const std::vector<float>& memoryItems,
                                        std::vector<float>& retention) {
    // Promote transfer from working memory to long-term storage
    // Enhanced by high ACh levels
    
    retention.resize(memoryItems.size());
    float achMemoryEnhancement = 1.0f + pImpl->workingMemoryBoost * pImpl->level;
    
    for (size_t i = 0; i < memoryItems.size(); ++i) {
        // Base retention from memory item strength
        float baseRetention = memoryItems[i];
        
        // Enhance with ACh-dependent memory consolidation
        float enhancedRetention = baseRetention * achMemoryEnhancement;
        
        // Ensure reasonable bounds
        enhancedRetention = std::clamp(enhancedRetention, 0.0f, 1.0f);
        
        retention[i] = enhancedRetention;
    }
    
    // Record successful consolidation events
    float averageRetention = 0.0f;
    for (float retention : retention) {
        averageRetention += retention;
    }
    averageRetention /= retention.size();
    
    if (averageRetention > 0.7f) {
        pImpl->memoryConsolidationEvents++;
    }
    
    // Store working memory retention for analysis
    pImpl->workingMemoryRetention.insert(pImpl->workingMemoryRetention.end(),
                                         retention.begin(), retention.end());
    if (pImpl->workingMemoryRetention.size() > 1000) {
        pImpl->workingMemoryRetention.erase(pImpl->workingMemoryRetention.begin(),
                                           pImpl->workingMemoryRetention.begin() + 100);
    }
}

void Acetylcholine::boostPlasticity(const std::vector<float>& noveltySignals,
                                   const std::vector<float>& errorSignals,
                                   float& plasticityModulator) {
    // Boost plasticity during novelty learning
    // ACh potentiates learning from unexpected events
    
    float averageNovelty = 0.0f;
    for (float novelty : noveltySignals) {
        averageNovelty += novelty;
    }
    averageNovelty /= noveltySignals.size();
    
    float averageError = 0.0f;
    for (float error : errorSignals) {
        averageError += error;
    }
    averageError /= errorSignals.size();
    
    // Novelty-enhanced plasticity factor
    float noveltyPlasticity = 1.0f + pImpl->noveltySensitivity * averageNovelty * pImpl->level;
    
    // Error-based plasticity modulation
    float errorPlasticity = 1.0f + averageError * pImpl->level;
    
    // Combined plasticity factor
    plasticityModulator = noveltyPlasticity * errorPlasticity;
    
    // Ensure reasonable bounds
    plasticityModulator = std::clamp(plasticityModulator, 0.5f, 5.0f);
    
    // Store plasticity factor for analysis
    pImpl->plasticityFactors.push_back(plasticityModulator);
    if (pImpl->plasticityFactors.size() > 1000) {
        pImpl->plasticityFactors.erase(pImpl->plasticityFactors.begin(),
                                      pImpl->plasticityFactors.begin() + 100);
    }
}

void Acetylcholine::modulateBurstFiring(const std::vector<float>& neuronActivity,
                                        std::vector<float>& burstModulation) {
    // Modulate burst firing in attention-related neurons
    // Higher ACh increases burst firing probability
    
    burstModulation.resize(neuronActivity.size());
    float achBurstMod = 1.0f + pImpl->level * 2.0f;  // 2x boost at max ACh
    
    for (size_t i = 0; i < neuronActivity.size(); ++i) {
        // Base burst firing from neural activity
        float baseBurst = neuronActivity[i];
        
        // Enhance with ACh-dependent burst modulation
        float enhancedBurst = baseBurst * achBurstMod;
        
        // Apply sigmoidal activation function
        enhancedBurst = 1.0f / (1.0f + std::exp(-10.0f * (enhancedBurst - 0.5f)));
        
        // Ensure reasonable bounds
        enhancedBurst = std::clamp(enhancedBurst, 0.1f, 2.0f);
        
        burstModulation[i] = enhancedBurst;
    }
}

void Acetylcholine::generatePhasicResponse(const std::vector<float>& stimulusFeatures,
                                         const Novelty& novelty) {
    // Generate phasic response to novel stimuli
    // Phasic ACh bursts are triggered by novel or salient events
    
    // Compute stimulus salience
    float stimulusSalience = 0.0f;
    for (float feature : stimulusFeatures) {
        stimulusSalience += std::abs(feature);
    }
    stimulusSalience /= stimulusFeatures.size();
    
    // Check for novelty trigger
    float noveltyLevel = novelty.getLevel();
    
    // Generate phasic response if stimulus is novel or salient
    if (stimulusSalience > 0.5f || noveltyLevel > 0.3f) {
        pImpl->isPhasicActive = true;
        pImpl->phasicStartTime = 0.0f;
        
        // Compute phasic amplitude based on stimulus properties
        float phasicAmplitude = std::min(1.0f, stimulusSalience * 2.0f + noveltyLevel);
        pImpl->phasicPeak = phasicAmplitude;
        
        // Increment phasic event counter
        pImpl->totalPhasicEvents++;
        
        NLM_LOG_INFO("Acetylcholine: Phasic response triggered (salience=" + 
                    std::to_string(stimulusSalience) + ", novelty=" + 
                    std::to_string(noveltyLevel) + ", amplitude=" + 
                    std::to_string(phasicAmplitude) + ")");
    }
}

void Acetylcholine::applyTonicLevel(float baselineLevel) {
    // Apply tonic background level
    pImpl->tonicLevel = baselineLevel;
    
    // Smooth transition to new tonic level
    float targetLevel = pImpl->phasicLevel + pImpl->tonicLevel;
    if (std::abs(pImpl->level - targetLevel) > 0.01f) {
        float transitionAmount = (targetLevel - pImpl->level) * 0.1f;
        pImpl->level += transitionAmount;
    }
}

void Acetylcholine::applyDecay(float decayRate, TimestepDuration dt) {
    // Decay between phasic events
    float dtFloat = static_cast<float>(dt);
    float decayAmount = decayRate * dtFloat;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->tonicLevel, 
                           pImpl->level - decayAmount);
}

void Acetylcholine::integrateWithDopamine(const Dopamine& dopamine,
                                         float& attentionRewardBalance) {
    // Work with dopamine for reward-attention balance
    // ACh and dopamine interact to prioritize reward-relevant information
    
    float dopamineLevel = dopamine.getLevel();
    
    // Compute reward-attention balance
    // High dopamine + high ACh = strong reward-attention coupling
    float achInfluence = pImpl->level * 0.6f;
    float dopamineInfluence = dopamineLevel * 0.4f;
    
    attentionRewardBalance = achInfluence + dopamineInfluence;
    attentionRewardBalance = std::clamp(attentionRewardBalance, 0.0f, 1.0f);
    
    // Store for analysis
    pImpl->rewardAttentionBalance = attentionRewardBalance;
}

void Acetylcholine::integrateWithNovelty(const Novelty& novelty,
                                        std::vector<float>& explorationExploitation) {
    // Work with novelty for exploration-exploitation
    // ACh promotes exploration when novelty is high
    
    float noveltyLevel = novelty.getLevel();
    
    // ACh promotes exploration at moderate levels
    // Very high ACh may lead to over-exploration
    float exploration = 0.5f + pImpl->level * 0.4f - noveltyLevel * 0.2f;
    exploration = std::clamp(exploration, 0.0f, 1.0f);
    
    // Exploitation increases with certainty/novelty decrease
    float exploitation = 1.0f - exploration;
    
    explorationExploitation.resize(2);
    explorationExploitation[0] = exploration;
    explorationExploitation[1] = exploitation;
    
    // Store for analysis
    pImpl->explorationExploitation = exploration;
}

void Acetylcholine::integrateWithPredictionSystem(const PredictionSystem& prediction,
                                                 float& predictionUncertainty) {
    // Influence prediction system uncertainty
    // ACh increases uncertainty to promote exploratory learning
    
    float predictionError = prediction.getPredictionError();
    
    // ACh modulates uncertainty based on prediction error
    float achUncertaintyMod = 1.0f + pImpl->level * predictionError * 2.0f;
    
    predictionUncertainty = predictionError * achUncertaintyMod;
    predictionUncertainty = std::clamp(predictionUncertainty, 0.0f, 1.0f);
    
    // Store for analysis
    pImpl->predictionUncertainty = predictionUncertainty;
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baselineLevel;
    pImpl->tonicLevel = pImpl->baselineLevel;
    pImpl->phasicLevel = 0.0f;
    pImpl->isPhasicActive = false;
    pImpl->phasicStartTime = 0.0f;
    
    // Clear history
    pImpl->history.clear();
    pImpl->attentionalGains.clear();
    pImpl->workingMemoryRetention.clear();
    pImpl->plasticityFactors.clear();
    
    // Reset statistics
    pImpl->totalPhasicEvents = 0;
    pImpl->successfulAttentionModulations = 0;
    pImpl->memoryConsolidationEvents = 0;
    
    NLM_LOG_INFO("Acetylcholine neuromodulation system reset");
}

} // namespace nlm