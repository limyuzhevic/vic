#include "Neuromodulator.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <numeric>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float tonicLevel;
    float baselineLevel;
    float phasicLevel;
    float peak;
    float decayRate;
    float releaseRate;
    float recoveryRate;
    float stressAdaptationRate;
    float signalDetectionGain;
    float vigilanceBoost;
    float arousalModulation;
    
    // Phasic response dynamics
    float phasicStartTime;
    float phasicDuration;
    float phasicPeak;
    float phasicDecayRate;
    bool isPhasicActive;
    
    // Stress and arousal dynamics
    float currentStressLevel;
    float chronicStressLevel;
    float stressResponseTime;
    float recoveryTime;
    float adaptationThreshold;
    float homeostasis;
    
    // Signal detection enhancement
    float signalToNoiseRatio;
    float detectionThreshold;
    float noiseBaseline;
    
    // Prioritization state
    float priorityWeight;
    float salienceWeight;
    float rewardWeight;
    float threatWeight;
    
    // Integration state
    float attentionArousalBalance;
    float salienceRewardIntegration;
    float noveltyThreatAssociation;
    float workingMemoryPriority;
    
    // History tracking
    std::vector<float> history;
    std::vector<float> arousalHistory;
    std::vector<float> vigilanceHistory;
    std::vector<float> stressHistory;
    std::vector<float> signalToNoiseHistory;
    std::vector<float> priorityHistory;
    
    // Random generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> normalDist;
    
    // Statistics
    size_t totalPhasicEvents;
    size_t successfulSignalEnhancements;
    size_t vigilanceBoosts;
    size_t stressResponseEvents;
    size_t recoveryEvents;
    
    Impl()
        : brain(nullptr)
        , level(0.05f)              // Baseline NE level
        , tonicLevel(0.05f)
        , baselineLevel(0.05f)
        , phasicLevel(0.0f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(2.0f)
        , recoveryRate(0.1f)
        , stressAdaptationRate(0.05f)
        , signalDetectionGain(1.0f)
        , vigilanceBoost(1.0f)
        , arousalModulation(1.0f)
        , phasicStartTime(0.0f)
        , phasicDuration(30.0f)    // Shorter than ACh for faster dynamics
        , phasicPeak(0.0f)
        , phasicDecayRate(0.3f)
        , isPhasicActive(false)
        , currentStressLevel(0.0f)
        , chronicStressLevel(0.0f)
        , stressResponseTime(0.0f)
        , recoveryTime(0.0f)
        , adaptationThreshold(0.7f)
        , homeostasis(1.0f)
        , signalToNoiseRatio(1.0f)
        , detectionThreshold(0.5f)
        , noiseBaseline(0.1f)
        , priorityWeight(0.5f)
        , salienceWeight(0.4f)
        , rewardWeight(0.3f)
        , threatWeight(0.6f)
        , attentionArousalBalance(0.5f)
        , salienceRewardIntegration(0.5f)
        , noveltyThreatAssociation(0.5f)
        , workingMemoryPriority(0.5f)
        , totalPhasicEvents(0)
        , successfulSignalEnhancements(0)
        , vigilanceBoosts(0)
        , stressResponseEvents(0)
        , recoveryEvents(0)
    {
        // Initialize random generator with time-based seed
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        rng.seed(static_cast<unsigned int>(seed));
        uniformDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
        normalDist = std::normal_distribution<float>(0.0f, 1.0f);
    }
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine neuromodulation system initialized");
    NLM_LOG_INFO("Baseline NE level: 0.05, Peak: 1.0, Decay rate: 0.05");
    NLM_LOG_INFO("NE system ready for arousal, vigilance, and stress response modulation");
}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update history
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances plasticity for important/learned information
    // Base plasticity factor of 1.0, enhanced by NE level for significant events
    float basePlasticity = 1.0f;
    float nePlasticity = 0.8f * pImpl->level; // Stronger NE influence on plasticity
    return basePlasticity + nePlasticity;
}

void Norepinephrine::update(TimestepDuration dt) {
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
    
    // Update stress and adaptation dynamics
    float stressAdaptation = pImpl->stressAdaptationRate * dtFloat;
    pImpl->chronicStressLevel = std::max(0.0f, pImpl->chronicStressLevel - stressAdaptation);
    
    // Recovery from stress
    recoverFromStress(pImpl->recoveryRate, dtFloat);
    
    // Adaptation to chronic stress
    adaptToStress(pImpl->stressHistory, dtFloat);
    
    // Update homeostasis
    pImpl->homeostasis = 1.0f / (1.0f + pImpl->chronicStressLevel);
    
    // Update history for analysis
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Norepinephrine::enhanceSignalDetection(const std::vector<float>& neuralActivity,
                                           const std::vector<float>& noiseLevels,
                                           std::vector<float>& signalToNoiseRatios) {
    // Enhance signal detection in neural circuits
    // NE increases signal-to-noise ratio by modulating neural excitability
    
    signalToNoiseRatios.resize(neuralActivity.size());
    
    // Compute enhanced signal detection based on NE level
    float signalBoost = 1.0f + pImpl->signalDetectionGain * pImpl->level;
    float noiseReduction = 1.0f - 0.3f * pImpl->level; // Reduce noise contribution
    
    for (size_t i = 0; i < neuralActivity.size(); ++i) {
        // Base signal and noise
        float signal = neuralActivity[i];
        float noise = noiseLevels[i];
        
        // Enhanced signal detection
        float enhancedSignal = signal * signalBoost;
        float reducedNoise = noise * noiseReduction;
        
        // Compute signal-to-noise ratio
        float snr = enhancedSignal / (reducedNoise + 0.01f); // Add small epsilon to avoid division by zero
        
        // Apply NE-specific detection threshold
        float detectionThreshold = pImpl->detectionThreshold * (1.0f - pImpl->level * 0.5f);
        float detectedSignal = (snr > detectionThreshold) ? snr : 0.0f;
        
        // Ensure reasonable bounds
        detectedSignal = std::clamp(detectedSignal, 0.0f, 10.0f);
        
        signalToNoiseRatios[i] = detectedSignal;
        
        // Update statistics
        if (snr > 2.0f) {
            pImpl->successfulSignalEnhancements++;
        }
    }
    
    // Update signal-to-noise history
    float avgSNR = 0.0f;
    for (float snr : signalToNoiseRatios) {
        avgSNR += snr;
    }
    avgSNR /= signalToNoiseRatios.size();
    pImpl->signalToNoiseHistory.push_back(avgSNR);
    if (pImpl->signalToNoiseHistory.size() > 1000) {
        pImpl->signalToNoiseHistory.erase(pImpl->signalToNoiseHistory.begin(),
                                        pImpl->signalToNoiseHistory.begin() + 100);
    }
}

void Norepinephrine::generatePhasicResponse(const std::vector<float>& stimulusFeatures,
                                           const Novelty& novelty,
                                           const PredictionError& predictionError,
                                           const std::vector<float>& threatLevel) {
    // Generate phasic response to novel, surprising, or threatening stimuli
    // Locus coeruleus-like phasic responses
    
    // Compute stimulus salience
    float stimulusSalience = 0.0f;
    for (float feature : stimulusFeatures) {
        stimulusSalience += std::abs(feature);
    }
    stimulusSalience /= stimulusFeatures.size();
    
    // Check for triggering events
    float noveltyLevel = novelty.getLevel();
    float predictionErrorLevel = predictionError.getLevel();
    float avgThreatLevel = 0.0f;
    for (float threat : threatLevel) {
        avgThreatLevel += threat;
    }
    avgThreatLevel /= threatLevel.size();
    
    // Generate phasic response if stimulus is novel, surprising, or threatening
    bool shouldTrigger = stimulusSalience > 0.5f || 
                         noveltyLevel > 0.3f || 
                         predictionErrorLevel > 0.2f || 
                         avgThreatLevel > 0.4f;
    
    if (shouldTrigger) {
        pImpl->isPhasicActive = true;
        pImpl->phasicStartTime = 0.0f;
        
        // Compute phasic amplitude based on stimulus properties
        float noveltyContribution = noveltyLevel * 0.3f;
        float surpriseContribution = predictionErrorLevel * 0.4f;
        float threatContribution = avgThreatLevel * 0.3f;
        
        float phasicAmplitude = std::min(1.0f, 
                                        stimulusSalience * 1.5f + 
                                        noveltyContribution + 
                                        surpriseContribution + 
                                        threatContribution);
        
        pImpl->phasicPeak = phasicAmplitude;
        
        // Update stress level
        pImpl->currentStressLevel += avgThreatLevel * 0.5f;
        pImpl->stressHistory.push_back(pImpl->currentStressLevel);
        if (pImpl->stressHistory.size() > 1000) {
            pImpl->stressHistory.erase(pImpl->stressHistory.begin(),
                                      pImpl->stressHistory.begin() + 100);
        }
        
        // Increment phasic event counter
        pImpl->totalPhasicEvents++;
        
        if (avgThreatLevel > 0.5f) {
            pImpl->stressResponseEvents++;
        }
        
        NLM_LOG_INFO("Norepinephrine: Phasic response triggered (salience=" + 
                    std::to_string(stimulusSalience) + ", novelty=" + 
                    std::to_string(noveltyLevel) + ", surprise=" + 
                    std::to_string(predictionErrorLevel) + ", threat=" + 
                    std::to_string(avgThreatLevel) + ", amplitude=" + 
                    std::to_string(phasicAmplitude) + ")");
    }
}

void Norepinephrine::enhanceArousal(float environmentalDemand, float stressorLevel, TimestepDuration dt) {
    // Enhance arousal based on environmental demands
    // Increases overall alertness and responsiveness
    
    float dtFloat = static_cast<float>(dt);
    
    // Compute arousal modulation based on environmental factors
    float demandComponent = environmentalDemand * 0.6f;
    float stressComponent = stressorLevel * 0.4f;
    float arousalDrive = demandComponent + stressComponent;
    
    // Apply NE-dependent arousal modulation
    float targetArousal = 1.0f + arousalDrive * pImpl->arousalModulation;
    targetArousal = std::clamp(targetArousal, 0.0f, 2.0f);
    
    // Smooth transition to target arousal level
    float arousalChange = (targetArousal - pImpl->tonicLevel) * pImpl->recoveryRate * dtFloat;
    pImpl->tonicLevel += arousalChange;
    pImpl->tonicLevel = std::clamp(pImpl->tonicLevel, 0.0f, 1.0f);
    
    // Update arousal history
    pImpl->arousalHistory.push_back(pImpl->tonicLevel);
    if (pImpl->arousalHistory.size() > 1000) {
        pImpl->arousalHistory.erase(pImpl->arousalHistory.begin(),
                                   pImpl->arousalHistory.begin() + 100);
    }
    
    NLM_LOG_DEBUG("NE arousal modulation: demand=" + std::to_string(environmentalDemand) + 
                  ", stress=" + std::to_string(stressorLevel) + 
                  ", target_arousal=" + std::to_string(targetArousal));
}

void Norepinephrine::enhanceVigilance(const std::vector<float>& sensoryInput,
                                     const std::vector<float>& threatSignals,
                                     std::vector<float>& vigilanceIndices) {
    // Boost vigilance during uncertain or threatening situations
    // Increases overall alertness and threat detection sensitivity
    
    vigilanceIndices.resize(sensoryInput.size());
    
    // Compute vigilance boost based on NE level and threat presence
    float baseVigilance = pImpl->vigilanceBoost * pImpl->level;
    
    // Average threat level across all signals
    float avgThreat = 0.0f;
    for (float threat : threatSignals) {
        avgThreat += threat;
    }
    avgThreat /= threatSignals.size();
    
    float threatVigilanceBoost = 1.5f + avgThreat * 2.0f; // More boost with more threat
    float combinedVigilance = baseVigilance * threatVigilanceBoost;
    combinedVigilance = std::clamp(combinedVigilance, 0.1f, 5.0f);
    
    for (size_t i = 0; i < sensoryInput.size(); ++i) {
        // Base vigilance from sensory input strength
        float baseVigilanceLevel = sensoryInput[i];
        
        // Enhance with NE-dependent vigilance
        float enhancedVigilance = baseVigilanceLevel * combinedVigilance;
        
        // Apply sigmoidal activation function for realistic vigilance dynamics
        enhancedVigilance = 1.0f / (1.0f + std::exp(-8.0f * (enhancedVigilance - 0.5f)));
        
        // Ensure reasonable bounds
        enhancedVigilance = std::clamp(enhancedVigilance, 0.1f, 2.0f);
        
        vigilanceIndices[i] = enhancedVigilance;
        
        // Record successful vigilance boosts
        if (enhancedVigilance > 1.2f) {
            pImpl->vigilanceBoosts++;
        }
    }
    
    // Update vigilance history
    float avgVigilance = 0.0f;
    for (float vigilance : vigilanceIndices) {
        avgVigilance += vigilance;
    }
    avgVigilance /= vigilanceIndices.size();
    pImpl->vigilanceHistory.push_back(avgVigilance);
    if (pImpl->vigilanceHistory.size() > 1000) {
        pImpl->vigilanceHistory.erase(pImpl->vigilanceHistory.begin(),
                                    pImpl->vigilanceHistory.begin() + 100);
    }
}

void Norepinephrine::prioritizeInformation(const std::vector<float>& informationSalience,
                                          const std::vector<float>& behavioralRelevance,
                                          std::vector<float>& priorityWeights) {
    // Prioritize behaviorally relevant information
    // Makes important information more salient and accessible
    
    priorityWeights.resize(informationSalience.size());
    
    // Compute integrated priority based on salience, relevance, and NE modulation
    for (size_t i = 0; i < informationSalience.size(); ++i) {
        // Weighted combination of salience and behavioral relevance
        float salienceContribution = informationSalience[i] * pImpl->salienceWeight;
        float relevanceContribution = behavioralRelevance[i] * (1.0f - pImpl->salienceWeight);
        
        // NE amplifies priority signals
        float basePriority = salienceContribution + relevanceContribution;
        float enhancedPriority = basePriority * (1.0f + pImpl->priorityWeight * pImpl->level);
        
        // Apply neurobiological prioritization function (sigmoidal)
        float prioritizedSignal = 1.0f / (1.0f + std::exp(-10.0f * (enhancedPriority - 0.5f)));
        
        // Ensure reasonable bounds
        prioritizedSignal = std::clamp(prioritizedSignal, 0.0f, 1.0f);
        
        priorityWeights[i] = prioritizedSignal;
    }
    
    // Update priority history
    float avgPriority = 0.0f;
    for (float priority : priorityWeights) {
        avgPriority += priority;
    }
    avgPriority /= priorityWeights.size();
    pImpl->priorityHistory.push_back(avgPriority);
    if (pImpl->priorityHistory.size() > 1000) {
        pImpl->priorityHistory.erase(pImpl->priorityHistory.begin(),
                                    pImpl->priorityHistory.begin() + 100);
    }
}

void Norepinephrine::enhanceWorkingMemory(const std::vector<float>& memoryItems,
                                       std::vector<float>& enhancedEncoding,
                                       std::vector<float>& enhancedRetrieval) {
    // Boost working memory encoding and retrieval
    // NE influences working memory capacity and fidelity
    
    enhancedEncoding.resize(memoryItems.size());
    enhancedRetrieval.resize(memoryItems.size());
    
    // NE-dependent working memory enhancement factor
    float encodingBoost = 1.0f + pImpl->level * 0.8f; // Strong encoding enhancement
    float retrievalBoost = 1.0f + pImpl->level * 0.6f; // Moderate retrieval enhancement
    
    for (size_t i = 0; i < memoryItems.size(); ++i) {
        // Base memory performance
        float baseEncoding = memoryItems[i];
        float baseRetrieval = memoryItems[i];
        
        // Enhance encoding
        float enhancedEncodingSignal = baseEncoding * encodingBoost;
        enhancedEncodingSignal = std::clamp(enhancedEncodingSignal, 0.0f, 1.0f);
        enhancedEncoding[i] = enhancedEncodingSignal;
        
        // Enhance retrieval (with decay over time)
        float timeFactor = std::exp(-0.01f * i); // Decay over item position
        float enhancedRetrievalSignal = baseRetrieval * retrievalBoost * timeFactor;
        enhancedRetrievalSignal = std::clamp(enhancedRetrievalSignal, 0.0f, 1.0f);
        enhancedRetrieval[i] = enhancedRetrievalSignal;
        
        // Store working memory priority
        pImpl->workingMemoryPriority = (pImpl->workingMemoryPriority * 0.9f) + 
                                      enhancedEncodingSignal * 0.1f;
    }
}

void Norepinephrine::applyTonicLevel(float baselineLevel) {
    // Apply tonic background level
    pImpl->tonicLevel = baselineLevel;
    
    // Smooth transition to new tonic level
    float targetLevel = pImpl->phasicLevel + pImpl->tonicLevel;
    if (std::abs(pImpl->level - targetLevel) > 0.01f) {
        float transitionAmount = (targetLevel - pImpl->level) * 0.1f;
        pImpl->level += transitionAmount;
    }
}

void Norepinephrine::applyDecay(float decayRate, TimestepDuration dt) {
    // Decay between phasic events
    float dtFloat = static_cast<float>(dt);
    float decayAmount = decayRate * dtFloat;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->tonicLevel, 
                           pImpl->level - decayAmount);
}

void Norepinephrine::recoverFromStress(float recoveryRate, TimestepDuration dt) {
    // Recovery mechanisms after stress responses
    // Return NE levels to baseline after stress exposure
    
    float dtFloat = static_cast<float>(dt);
    
    // Recovery from stress is enhanced when homeostasis is high
    float recoveryBoost = pImpl->homeostasis * recoveryRate;
    float stressRecovery = std::min(pImpl->currentStressLevel, recoveryBoost * dtFloat);
    
    pImpl->currentStressLevel -= stressRecovery;
    pImpl->recoveryTime += stressRecovery;
    
    // Record recovery event
    if (stressRecovery > 0.01f) {
        pImpl->recoveryEvents++;
    }
}

void Norepinephrine::adaptToStress(const std::vector<float>& stressHistory, TimestepDuration dt) {
    // Adaptation to chronic stress levels
    // Adjusts NE response dynamics based on stress exposure history
    
    float dtFloat = static_cast<float>(dt);
    
    if (stressHistory.size() >= 10) {
        // Compute average recent stress
        float recentStress = 0.0f;
        size_t count = std::min(size_t(10), stressHistory.size());
        for (size_t i = 0; i < count; ++i) {
            recentStress += stressHistory[stressHistory.size() - 1 - i];
        }
        recentStress /= count;
        
        pImpl->chronicStressLevel = recentStress;
        
        // Adaptation based on chronic stress levels
        if (recentStress > pImpl->adaptationThreshold) {
            // Chronic stress adaptation - reduce peak response amplitude
            pImpl->peak = std::max(0.5f, 1.0f - (recentStress - pImpl->adaptationThreshold) * 0.5f);
            
            // Modify recovery dynamics
            pImpl->recoveryRate = 0.05f + recentStress * 0.1f;
            
            NLM_LOG_INFO("NE chronic stress adaptation: stress=" + std::to_string(recentStress) + 
                        ", peak adjusted to " + std::to_string(pImpl->peak));
        }
    }
}

void Norepinephrine::integrateWithAcetylcholine(const Acetylcholine& acetylcholine,
                                               float& attentionArousalBalance) {
    // Work with acetylcholine for attention-arousal balance
    // NE and ACh interact to balance alertness and focused attention
    
    float achLevel = acetylcholine.getLevel();
    float neLevel = getLevel();
    
    // Compute attention-arousal balance
    // High NE = high arousal, low ACh = diffuse attention
    // Low NE = low arousal, high ACh = focused attention
    float neInfluence = neLevel * 0.6f;
    float achInfluence = achLevel * 0.4f;
    
    attentionArousalBalance = neInfluence + achInfluence;
    attentionArousalBalance = std::clamp(attentionArousalBalance, 0.0f, 1.0f);
    
    // Store for analysis
    pImpl->attentionArousalBalance = attentionArousalBalance;
    
    NLM_LOG_DEBUG("NE-ACh integration: balance=" + std::to_string(attentionArousalBalance));
}

void Norepinephrine::integrateWithDopamine(const Dopamine& dopamine,
                                          float& salienceRewardIntegration) {
    // Work with dopamine for salience-reward integration
    // NE provides arousal context for reward processing
    
    float daLevel = dopamine.getLevel();
    float neLevel = getLevel();
    
    // Compute salience-reward integration
    // NE amplifies reward salience, dopamine provides reward prediction
    float neSalienceAmplification = neLevel * 0.7f; // NE strongly amplifies salience
    float daRewardInfluence = daLevel * 0.3f;
    
    salienceRewardIntegration = neSalienceAmplification + daRewardInfluence;
    salienceRewardIntegration = std::clamp(salienceRewardIntegration, 0.0f, 1.0f);
    
    // Store for analysis
    pImpl->salienceRewardIntegration = salienceRewardIntegration;
    
    NLM_LOG_DEBUG("NE-DA integration: salience_reward=" + std::to_string(salienceRewardIntegration));
}

void Norepinephrine::integrateWithNovelty(const Novelty& novelty,
                                         std::vector<float>& noveltyThreatAssociation) {
    // Work with novelty for threat detection
    // NE links novelty detection with threat assessment
    
    float noveltyLevel = novelty.getLevel();
    float neLevel = getLevel();
    
    // Compute novelty-threat association
    // High NE increases the weight of novelty in threat detection
    float noveltyThreatWeight = neLevel * 0.8f;
    float baseAssociation = noveltyLevel * 0.2f; // Novelty alone has some threat value
    
    noveltyThreatAssociation.resize(noveltyThreatAssociation.size()); // Ensure correct size
    
    for (size_t i = 0; i < noveltyThreatAssociation.size(); ++i) {
        float association = baseAssociation + noveltyThreatWeight;
        association = std::clamp(association, 0.0f, 1.0f);
        noveltyThreatAssociation[i] = association;
    }
    
    // Store for analysis
    pImpl->noveltyThreatAssociation = (noveltyThreatAssociation.empty() ? 0.0f : noveltyThreatAssociation[0]);
    
    NLM_LOG_DEBUG("NE-Novelty integration: association=" + std::to_string(pImpl->noveltyThreatAssociation));
}

void Norepinephrine::integrateWithWorkingMemory(class NeuralWorkingMemory* workingMemory,
                                               const std::vector<float>& memoryRequests) {
    // Influence working memory encoding and retrieval
    // NE modulates working memory priority and access
    
    if (workingMemory && !memoryRequests.empty()) {
        // Compute working memory priority based on NE level
        float nePriority = getLevel() * pImpl->workingMemoryPriority;
        
        // Apply NE modulation to working memory
        // This would interface with working memory implementation
        // For now, we just store the priority for analysis
        pImpl->workingMemoryPriority = nePriority;
        
        // Simulate working memory influence based on priority
        for (size_t i = 0; i < memoryRequests.size(); ++i) {
            // Items with higher priority get better working memory access
            float priorityBoost = nePriority * memoryRequests[i];
            priorityBoost = std::clamp(priorityBoost, 0.0f, 1.0f);
            
            // This would update working memory in a real implementation
            // workingMemory->enhancePriority(i, priorityBoost);
        }
        
        NLM_LOG_DEBUG("NE working memory integration: priority=" + std::to_string(nePriority) + 
                     ", requests=" + std::to_string(memoryRequests.size()));
    }
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baselineLevel;
    pImpl->tonicLevel = pImpl->baselineLevel;
    pImpl->phasicLevel = 0.0f;
    pImpl->isPhasicActive = false;
    pImpl->phasicStartTime = 0.0f;
    pImpl->currentStressLevel = 0.0f;
    pImpl->chronicStressLevel = 0.0f;
    pImpl->homeostasis = 1.0f;
    pImpl->signalToNoiseRatio = 1.0f;
    pImpl->attentionArousalBalance = 0.5f;
    pImpl->salienceRewardIntegration = 0.5f;
    pImpl->noveltyThreatAssociation = 0.5f;
    pImpl->workingMemoryPriority = 0.5f;
    
    // Clear history
    pImpl->history.clear();
    pImpl->arousalHistory.clear();
    pImpl->vigilanceHistory.clear();
    pImpl->stressHistory.clear();
    pImpl->signalToNoiseHistory.clear();
    pImpl->priorityHistory.clear();
    
    // Reset statistics
    pImpl->totalPhasicEvents = 0;
    pImpl->successfulSignalEnhancements = 0;
    pImpl->vigilanceBoosts = 0;
    pImpl->stressResponseEvents = 0;
    pImpl->recoveryEvents = 0;
    
    NLM_LOG_INFO("Norepinephrine neuromodulation system reset");
}

} // namespace nlm
