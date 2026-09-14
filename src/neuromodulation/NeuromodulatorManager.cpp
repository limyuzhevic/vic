#include "NeuromodulatorManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

struct NeuromodulatorManager::Impl {
    // Neuromodulator references
    Dopamine* dopamine;
    Acetylcholine* acetylcholine;
    Norepinephrine* norepinephrine;
    Serotonin* serotonin;
    Reward* reward;
    Curiosity* curiosity;
    Novelty* novelty;
    PredictionError* predictionError;
    
    // Interaction matrix
    NeuromodulatorInteraction interactionMatrix;
    
    // Integration parameters
    float integrationStrength;           // Overall neuromodulation integration strength
    float crossTalkCoefficient;           // Coefficient for cross-talk between neuromodulators
    float temporalCoherence;             // Temporal coherence between neuromodulators
    float spatialCoherence;              // Spatial coherence across brain regions
    
    // State tracking
    float lastDopamineLevel;             // Previous dopamine level for change detection
    float lastAChLevel;                   // Previous acetylcholine level
    float lastNELevel;                    // Previous norepinephrine level
    float last5HTLevel;                   // Previous serotonin level
    
    // History tracking for interaction analysis
    std::vector<float> dopamineHistory;
    std::vector<float> achHistory;
    std::vector<float> neHistory;
    std::vector<float> 5htHistory;
    
    // Random generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> normalDist;
    
    // Integration state
    bool neuromodulatorsInitialized;      // Whether all neuromodulators are initialized
    float globalModulationFactor;         // Global modulation factor affecting all neuromodulators
    float neuromodulationComplexity;       // Current complexity of neuromodulation environment
    
    Impl() : dopamine(nullptr), acetylcholine(nullptr), norepinephrine(nullptr), serotonin(nullptr)
           , reward(nullptr), curiosity(nullptr), novelty(nullptr), predictionError(nullptr)
           , integrationStrength(1.0f), crossTalkCoefficient(0.8f), temporalCoherence(0.9f), spatialCoherence(0.8f)
           , lastDopamineLevel(0.0f), lastAChLevel(0.0f), lastNELevel(0.0f), last5HTLevel(0.0f)
           , neuromodulatorsInitialized(false), globalModulationFactor(1.0f), neuromodulationComplexity(1.0f) {
        rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
        uniformDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
        normalDist = std::normal_distribution<float>(0.0f, 0.1f);
        dopamineHistory.reserve(50);
        achHistory.reserve(50);
        neHistory.reserve(50);
        5htHistory.reserve(50);
    }
};

NeuromodulatorManager::NeuromodulatorManager() : pImpl(new Impl) {}

NeuromodulatorManager::~NeuromodulatorManager() = default;

void NeuromodulatorManager::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize NeuromodulatorManager: brain is null");
        return;
    }
    
    // Get references to all neuromodulators
    pImpl->dopamine = brain->getDopamine();
    pImpl->acetylcholine = brain->getAcetylcholine();
    pImpl->norepinephrine = brain->getNorepinephrine();
    pImpl->serotonin = brain->getSerotonin();
    pImpl->reward = brain->getReward();
    pImpl->curiosity = brain->getCuriosity();
    pImpl->novelty = brain->getNovelty();
    pImpl->predictionError = brain->getPredictionErrorSignal();
    
    // Verify all neuromodulators are available
    pImpl->neuromodulatorsInitialized = (pImpl->dopamine && pImpl->acetylcholine && 
                                         pImpl->norepinephrine && pImpl->serotonin && 
                                         pImpl->reward && pImpl->curiosity && 
                                         pImpl->novelty && pImpl->predictionError);
    
    if (pImpl->neuromodulatorsInitialized) {
        NLM_LOG_INFO("NeuromodulatorManager initialized with all neuromodulators");
        NLM_LOG_INFO("Interaction matrix initialized");
    } else {
        NLM_LOG_WARN("NeuromodulatorManager initialized with missing neuromodulators");
    }
}

void NeuromodulatorManager::update(TimestepDuration dt) {
    if (!pImpl->neuromodulatorsInitialized) {
        return;
    }
    
    float dtFloat = static_cast<float>(dt);
    
    // Track neuromodulator levels for change detection
    float currentDA = pImpl->dopamine->getLevel();
    float currentACh = pImpl->acetylcholine->getLevel();
    float currentNE = pImpl->norepinephrine->getLevel();
    float current5HT = pImpl->serotonin->getLevel();
    
    // Update history
    pImpl->dopamineHistory.push_back(currentDA);
    pImpl->achHistory.push_back(currentACh);
    pImpl->neHistory.push_back(currentNE);
    pImpl->5htHistory.push_back(current5HT);
    
    // Maintain history sizes
    const size_t maxHistorySize = 50;
    if (pImpl->dopamineHistory.size() > maxHistorySize) {
        pImpl->dopamineHistory.erase(pImpl->dopamineHistory.begin());
    }
    if (pImpl->achHistory.size() > maxHistorySize) {
        pImpl->achHistory.erase(pImpl->achHistory.begin());
    }
    if (pImpl->neHistory.size() > maxHistorySize) {
        pImpl->neHistory.erase(pImpl->neHistory.begin());
    }
    if (pImpl->5htHistory.size() > maxHistorySize) {
        pImpl->5htHistory.erase(pImpl->5htHistory.begin());
    }
    
    // Update global modulation factor
    pImpl->globalModulationFactor = (currentDA + currentACh + currentNE + current5HT) / 4.0f;
    
    // Update neuromodulation complexity based on variability
    float daVar = std::abs(currentDA - pImpl->lastDopamineLevel);
    float achVar = std::abs(currentACh - pImpl->lastAChLevel);
    float neVar = std::abs(currentNE - pImpl->lastNELevel);
    float 5htVar = std::abs(current5HT - pImpl->last5HTLevel);
    
    pImpl->neuromodulationComplexity = (daVar + achVar + neVar + 5htVar) / 4.0f;
    
    // Update last levels
    pImpl->lastDopamineLevel = currentDA;
    pImpl->lastAChLevel = currentACh;
    pImpl->lastNELevel = currentNE;
    pImpl->last5HTLevel = current5HT;
    
    // Apply cross-talk interactions
    propagateRewardPrediction(pImpl->dopamine);
    enhanceAttentionThroughACh(pImpl->acetylcholine, pImpl->norepinephrine);
    regulateMoodSerotonin(pImpl->acetylcholine, pImpl->serotonin);
    maintainArousalBalance(pImpl->norepinephrine, pImpl->dopamine);
    coordinateSensoryProcessing(pImpl->acetylcholine, pImpl->norepinephrine);
    
    // Process special interactions
    if (pImpl->predictionError) {
        processRewardPredictionError(pImpl->dopamine, pImpl->reward, pImpl->predictionError);
    }
    
    integrateAttentionAndReward(pImpl->acetylcholine, pImpl->dopamine, pImpl->reward);
    balanceArousalAndAttention(pImpl->norepinephrine, pImpl->acetylcholine);
    stabilizeMoodAndImpulsivity(pImpl->acetylcholine, pImpl->serotonin);
    
    // Update interaction matrix based on current neuromodulator states
    computeNeuromodulatorInteractions();
    
    // Synchronize neuromodulator states
    synchronizeNeuromodulatorStates();
    
    // Apply integrated neuromodulation to brain
    applyNeuromodulationToBrain();
}

Dopamine* NeuromodulatorManager::getDopamine() {
    return pImpl->dopamine;
}

Acetylcholine* NeuromodulatorManager::getAcetylcholine() {
    return pImpl->acetylcholine;
}

Norepinephrine* NeuromodulatorManager::getNorepinephrine() {
    return pImpl->norepinephrine;
}

Serotonin* NeuromodulatorManager::getSerotonin() {
    return pImpl->serotonin;
}

Reward* NeuromodulatorManager::getReward() {
    return pImpl->reward;
}

Curiosity* NeuromodulatorManager::getCuriosity() {
    return pImpl->curiosity;
}

Novelty* NeuromodulatorManager::getNovelty() {
    return pImpl->novelty;
}

PredictionError* NeuromodulatorManager::getPredictionErrorSignal() {
    return pImpl->predictionError;
}

void NeuromodulatorManager::propagateRewardPrediction(Dopamine* dopamine) {
    if (!dopamine || !pImpl->reward) return;
    
    // Get current reward prediction error
    float rewardRPE = pImpl->reward->getPredictionError();
    
    // Apply reward prediction to dopamine signaling
    if (rewardRPE != 0.0f) {
        float propagatedEffect = rewardRPE * pImpl->interactionMatrix.ach_effect_on_da;
        dopamine->signalRewardPredictionError(propagatedEffect * pImpl->crossTalkCoefficient);
    }
}

void NeuromodulatorManager::enhanceAttentionThroughACh(Acetylcholine* ach, Norepinephrine* ne) {
    if (!ach || !ne) return;
    
    // Acetylcholine enhances norepinephrine's attention effects
    float achLevel = ach->getLevel();
    float neLevel = ne->getLevel();
    
    // Calculate combined attention enhancement
    float attentionEnhancement = achLevel * neLevel * pImpl->interactionMatrix.ach_effect_on_ne;
    
    // Apply enhancement through norepinephrine
    ne->increaseArousal(attentionEnhancement * pImpl->crossTalkCoefficient * 0.5f);
}

void NeuromodulatorManager::regulateMoodSerotonin(Acetylcholine* ach, Serotonin* serotonin) {
    if (!ach || !serotonin) return;
    
    // Acetylcholine modulates serotonin's mood regulation
    float achLevel = ach->getLevel();
    float serotoninLevel = serotonin->getLevel();
    
    // Calculate mood regulation effect
    float moodRegulation = achLevel * (serotoninLevel - 0.5f) * pImpl->interactionMatrix.ach_effect_on_5ht;
    
    // Apply mood regulation
    if (moodRegulation > 0.0f) {
        serotonin->increaseMood(moodRegulation * pImpl->crossTalkCoefficient * 0.3f);
    } else {
        serotonin->decreaseMood(-moodRegulation * pImpl->crossTalkCoefficient * 0.3f);
    }
}

void NeuromodulatorManager::maintainArousalBalance(Norepinephrine* ne, Dopamine* dopamine) {
    if (!ne || !dopamine) return;
    
    // Norepinephrine and dopamine balance each other's arousal effects
    float neLevel = ne->getLevel();
    float daLevel = dopamine->getLevel();
    
    // Calculate arousal balance effect
    float arousalBalance = (neLevel - daLevel) * pImpl->interactionMatrix.ne_effect_on_da;
    
    // Apply balance adjustments
    if (arousalBalance > 0.2f) {
        // NE too high, reduce by increasing dopamine
        dopamine->setLevel(std::min(dopamine->getLevel() + arousalBalance * 0.2f, 1.0f));
    } else if (arousalBalance < -0.2f) {
        // DA too high, reduce by decreasing NE
        ne->setLevel(std::max(ne->getLevel() + arousalBalance * 0.2f, 0.0f));
    }
}

void NeuromodulatorManager::coordinateSensoryProcessing(Acetylcholine* ach, Norepinephrine* ne) {
    if (!ach || !ne) return;
    
    // Coordinated sensory processing through ACh and NE
    float achLevel = ach->getLevel();
    float neLevel = ne->getLevel();
    
    // Calculate sensory coordination strength
    float sensoryCoordination = achLevel * neLevel * pImpl->crossTalkCoefficient;
    
    // Apply sensory coordination effects
    if (sensoryCoordination > 0.5f) {
        // Enhanced sensory processing
        ne->increaseArousal(0.3f * sensoryCoordination);
        ach->signalAttention(0.4f * sensoryCoordination);
    }
}

void NeuromodulatorManager::applyNeuromodulationToBrain() {
    if (!pImpl->neuromodulatorsInitialized) return;
    
    // Calculate overall neuromodulation level
    float overallLevel = getOverallNeuromodulationLevel();
    
    // Apply global modulation factor to all neuromodulators
    float globalAdjustment = overallLevel * pImpl->globalModulationFactor;
    
    // Update neuromodulator levels with global modulation
    if (pImpl->dopamine) {
        float currentDA = pImpl->dopamine->getLevel();
        float adjustedDA = currentDA + globalAdjustment * 0.3f;
        pImpl->dopamine->setLevel(std::min(adjustedDA, 1.0f));
    }
    
    if (pImpl->acetylcholine) {
        float currentACh = pImpl->acetylcholine->getLevel();
        float adjustedACh = currentACh + globalAdjustment * 0.2f;
        pImpl->acetylcholine->setLevel(std::min(adjustedACh, 1.0f));
    }
    
    if (pImpl->norepinephrine) {
        float currentNE = pImpl->norepinephrine->getLevel();
        float adjustedNE = currentNE + globalAdjustment * 0.3f;
        pImpl->norepinephrine->setLevel(std::min(adjustedNE, 1.0f));
    }
    
    if (pImpl->serotonin) {
        float current5HT = pImpl->serotonin->getLevel();
        float adjusted5HT = current5HT + globalAdjustment * 0.2f;
        pImpl->serotonin->setLevel(std::min(adjusted5HT, 1.0f));
    }
}

void NeuromodulatorManager::synchronizeNeuromodulatorStates() {
    if (!pImpl->neuromodulatorsInitialized) return;
    
    // Synchronize neuromodulator states based on coherence
    float coherence = pImpl->temporalCoherence * pImpl->spatialCoherence;
    
    // Calculate target states for synchronization
    float targetDA = getTargetNeuromodulatorLevel("DA", coherence);
    float targetACh = getTargetNeuromodulatorLevel("ACh", coherence);
    float targetNE = getTargetNeuromodulatorLevel("NE", coherence);
    float target5HT = getTargetNeuromodulatorLevel("5-HT", coherence);
    
    // Apply synchronization with smoothing
    if (pImpl->dopamine) {
        float currentDA = pImpl->dopamine->getLevel();
        float syncForce = (targetDA - currentDA) * coherence * 0.1f;
        pImpl->dopamine->setLevel(std::max(0.0f, std::min(1.0f, currentDA + syncForce)));
    }
    
    if (pImpl->acetylcholine) {
        float currentACh = pImpl->acetylcholine->getLevel();
        float syncForce = (targetACh - currentACh) * coherence * 0.1f;
        pImpl->acetylcholine->setLevel(std::max(0.0f, std::min(1.0f, currentACh + syncForce)));
    }
    
    if (pImpl->norepinephrine) {
        float currentNE = pImpl->norepinephrine->getLevel();
        float syncForce = (targetNE - currentNE) * coherence * 0.1f;
        pImpl->norepinephrine->setLevel(std::max(0.0f, std::min(1.0f, currentNE + syncForce)));
    }
    
    if (pImpl->serotonin) {
        float current5HT = pImpl->serotonin->getLevel();
        float syncForce = (target5HT - current5HT) * coherence * 0.1f;
        pImpl->serotonin->setLevel(std::max(0.0f, std::min(1.0f, current5HT + syncForce)));
    }
}

void NeuromodulatorManager::computeNeuromodulatorInteractions() {
    if (!pImpl->neuromodulatorsInitialized) return;
    
    // Update interaction matrix based on current neuromodulator states
    // This simulates the dynamic nature of neuromodulator interactions
    
    float currentDA = pImpl->dopamine->getLevel();
    float currentACh = pImpl->acetylcholine->getLevel();
    float currentNE = pImpl->norepinephrine->getLevel();
    float current5HT = pImpl->serotonin->getLevel();
    
    // Update interaction matrix elements based on neuromodulator states
    // These are simplified models of the complex interactions
    
    // ACh effect on DA (enhanced by ACh level)
    pImpl->interactionMatrix.ach_effect_on_da = 1.0f + currentACh * 0.5f;
    
    // ACh effect on NE (enhanced by ACh level)
    pImpl->interactionMatrix.ach_effect_on_ne = 1.0f + currentACh * 0.3f;
    
    // ACh effect on 5-HT (inhibitory at high levels)
    pImpl->interactionMatrix.ach_effect_on_5ht = std::max(0.5f, 1.0f - currentACh * 0.5f);
    
    // NE effect on DA (enhanced by NE level)
    pImpl->interactionMatrix.ne_effect_on_da = 1.0f + currentNE * 0.4f;
    
    // NE effect on ACh (inhibitory at very high levels)
    pImpl->interactionMatrix.ne_effect_on_ach = std::max(0.7f, 1.0f - currentNE * 0.3f);
    
    // NE effect on 5-HT (excitatory at moderate levels)
    pImpl->interactionMatrix.ne_effect_on_5ht = 1.0f + currentNE * 0.2f;
    
    // DA effect on ACh (inhibitory at high levels)
    pImpl->interactionMatrix.da_effect_on_ach = std::max(0.7f, 1.0f - currentDA * 0.4f);
    
    // DA effect on NE (excitatory at low levels)
    pImpl->interactionMatrix.da_effect_on_ne = 1.0f + (1.0f - currentDA) * 0.3f;
    
    // DA effect on 5-HT (inhibitory at high levels)
    pImpl->interactionMatrix.da_effect_on_5ht = std::max(0.6f, 1.0f - currentDA * 0.5f);
    
    // 5-HT effect on DA (inhibitory at high levels)
    pImpl->interactionMatrix.5ht_effect_on_da = std::max(0.7f, 1.0f - current5HT * 0.4f);
    
    // 5-HT effect on ACh (excitatory at low levels)
    pImpl->interactionMatrix.5ht_effect_on_ach = 1.0f + (1.0f - current5HT) * 0.3f;
    
    // 5-HT effect on NE (inhibitory at high levels)
    pImpl->interactionMatrix.5ht_effect_on_ne = std::max(0.7f, 1.0f - current5HT * 0.5f);
    
    // Add biological variability to interaction matrix
    float variability = normalDist(pImpl->rng) * 0.05f;
    pImpl->interactionMatrix.ach_effect_on_da = std::max(0.5f, pImpl->interactionMatrix.ach_effect_on_da + variability);
}

void NeuromodulatorManager::processRewardPredictionError(Dopamine* dopamine, Reward* reward, PredictionError* predError) {
    if (!dopamine || !reward || !predError) return;
    
    // Process reward prediction error through the integrated system
    float rewardRPE = reward->getPredictionError();
    float predictionRPE = predError->getError();
    
    // Combine reward and prediction error signals
    float combinedRPE = (rewardRPE + predictionRPE) * 0.5f;
    
    // Apply combined RPE to dopamine signaling
    if (combinedRPE != 0.0f) {
        dopamine->signalRewardPredictionError(combinedRPE * pImpl->crossTalkCoefficient * 1.2f);
    }
}

void NeuromodulatorManager::integrateAttentionAndReward(Acetylcholine* ach, Dopamine* dopamine, Reward* reward) {
    if (!ach || !dopamine || !reward) return;
    
    // Integrate attention and reward processing
    float achLevel = ach->getLevel();
    float daLevel = dopamine->getLevel();
    float rewardValue = reward->getValue();
    
    // Calculate integrated attention-reward signal
    float integratedSignal = (achLevel * daLevel * rewardValue) * pImpl->crossTalkCoefficient;
    
    // Apply integrated signal to neuromodulators
    if (integratedSignal > 0.3f) {
        ach->signalAttention(integratedSignal * 0.5f);
        dopamine->setLevel(std::min(dopamine->getLevel() + integratedSignal * 0.3f, 1.0f));
    }
}

void NeuromodulatorManager::balanceArousalAndAttention(Norepinephrine* ne, Acetylcholine* ach) {
    if (!ne || !ach) return;
    
    // Balance arousal and attention
    float neLevel = ne->getLevel();
    float achLevel = ach->getLevel();
    
    // Calculate arousal-attention balance
    float balanceIndex = std::abs(neLevel - achLevel);
    
    // Apply balancing adjustments
    if (balanceIndex > 0.5f) {
        // Too much imbalance, apply correction
        float correction = (neLevel - achLevel) * 0.2f;
        
        if (neLevel > achLevel) {
            ne->decreaseArousal(correction);
            ach->signalAttention(correction * 0.5f);
        } else {
            ach->setLevel(std::max(achLevel + correction, 0.0f));
            ne->increaseArousal(correction * 0.5f);
        }
    }
}

void NeuromodulatorManager::stabilizeMoodAndImpulsivity(Acetylcholine* ach, Serotonin* serotonin) {
    if (!ach || !serotonin) return;
    
    // Stabilize mood and impulsivity through ACh and 5-HT interaction
    float achLevel = ach->getLevel();
    float serotoninLevel = serotonin->getLevel();
    
    // Calculate mood-impulsivity stabilization factor
    float moodStability = achLevel * (1.0f - serotoninLevel) * 0.5f;
    
    // Apply stabilization effects
    if (moodStability > 0.3f) {
        // Good conditions for mood stabilization
        serotonin->stabilizeMood(0.0f);  // Stabilize at neutral mood
        ach->signalAttention(moodStability * 0.3f);
    }
}

void NeuromodulatorManager::resetNeuromodulators() {
    if (!pImpl->neuromodulatorsInitialized) return;
    
    // Reset all neuromodulators to baseline
    if (pImpl->dopamine) {
        pImpl->dopamine->setLevel(0.1f);
    }
    
    if (pImpl->acetylcholine) {
        pImpl->acetylcholine->setLevel(0.05f);
    }
    
    if (pImpl->norepinephrine) {
        pImpl->norepinephrine->setLevel(0.05f);
    }
    
    if (pImpl->serotonin) {
        pImpl->serotonin->setLevel(0.1f);
    }
    
    // Reset histories
    pImpl->dopamineHistory.clear();
    pImpl->achHistory.clear();
    pImpl->neHistory.clear();
    pImpl->5htHistory.clear();
    
    pImpl->lastDopamineLevel = 0.1f;
    pImpl->lastAChLevel = 0.05f;
    pImpl->lastNELevel = 0.05f;
    pImpl->last5HTLevel = 0.1f;
    
    NLM_LOG_INFO("NeuromodulatorManager: Reset all neuromodulators to baseline");
}

void NeuromodulatorManager::adaptNeuromodulatorParameters(float environmentComplexity) {
    if (!pImpl->neuromodulatorsInitialized) return;
    
    // Adapt neuromodulator parameters based on environment complexity
    pImpl->neuromodulationComplexity = environmentComplexity;
    
    // Adjust interaction matrix based on complexity
    if (environmentComplexity > 0.7f) {
        // High complexity: increase cross-talk
        pImpl->crossTalkCoefficient = std::min(pImpl->crossTalkCoefficient + 0.1f, 1.0f);
        pImpl->integrationStrength = std::min(pImpl->integrationStrength + 0.2f, 2.0f);
        
        NLM_LOG_INFO("NeuromodulatorManager: High complexity environment - enhanced integration");
    } else if (environmentComplexity < 0.3f) {
        // Low complexity: reduce cross-talk
        pImpl->crossTalkCoefficient = std::max(pImpl->crossTalkCoefficient - 0.1f, 0.5f);
        pImpl->integrationStrength = std::max(pImpl->integrationStrength - 0.2f, 0.5f);
        
        NLM_LOG_INFO("NeuromodulatorManager: Low complexity environment - reduced integration");
    }
}

void NeuromodulatorManager::updateNeuromodulatorSynergies() {
    if (!pImpl->neuromodulatorsInitialized) return;
    
    // Update neuromodulator synergies based on current states
    float currentDA = pImpl->dopamine->getLevel();
    float currentACh = pImpl->acetylcholine->getLevel();
    float currentNE = pImpl->norepinephrine->getLevel();
    float current5HT = pImpl->serotonin->getLevel();
    
    // Calculate synergy strength
    float daAChSynergy = currentDA * currentACh;
    float daNESynergy = currentDA * currentNE;
    float da5HTSynergy = currentDA * current5HT;
    float achNESynergy = currentACh * currentNE;
    float ach5HTSynergy = currentACh * current5HT;
    float ne5HTSynergy = currentNE * current5HT;
    
    // Log synergies
    if (daAChSynergy > 0.3f) {
        NLM_LOG_INFO("NeuromodulatorManager: DA-ACh synergy detected (strength: " + 
                     std::to_string(daAChSynergy) + ")");
    }
    
    if (daNESynergy > 0.3f) {
        NLM_LOG_INFO("NeuromodulatorManager: DA-NE synergy detected (strength: " + 
                     std::to_string(daNESynergy) + ")");
    }
    
    if (ne5HTSynergy > 0.3f) {
        NLM_LOG_INFO("NeuromodulatorManager: NE-5-HT synergy detected (strength: " + 
                     std::to_string(ne5HTSynergy) + ")");
    }
}

float NeuromodulatorManager::getOverallNeuromodulationLevel() const {
    if (!pImpl->neuromodulatorsInitialized) return 0.0f;
    
    float currentDA = pImpl->dopamine->getLevel();
    float currentACh = pImpl->acetylcholine->getLevel();
    float currentNE = pImpl->norepinephrine->getLevel();
    float current5HT = pImpl->serotonin->getLevel();
    
    // Calculate weighted overall neuromodulation level
    return (currentDA * 0.3f + currentACh * 0.2f + currentNE * 0.3f + current5HT * 0.2f);
}

float NeuromodulatorManager::getNeuromodulatorCoherence() const {
    if (!pImpl->neuromodulatorsInitialized || pImpl->dopamineHistory.size() < 10) return 0.0f;
    
    // Calculate coherence between neuromodulators based on history
    if (pImpl->dopamineHistory.size() < 10) return 0.0f;
    
    // Calculate correlation between DA and ACh
    float daMean = 0.0f, achMean = 0.0f;
    for (size_t i = 0; i < pImpl->dopamineHistory.size(); ++i) {
        daMean += pImpl->dopamineHistory[i];
        achMean += pImpl->achHistory[i];
    }
    daMean /= static_cast<float>(pImpl->dopamineHistory.size());
    achMean /= static_cast<float>(pImpl->dopamineHistory.size());
    
    float daVar = 0.0f, achVar = 0.0f, cov = 0.0f;
    for (size_t i = 0; i < pImpl->dopamineHistory.size(); ++i) {
        float daDev = pImpl->dopamineHistory[i] - daMean;
        float achDev = pImpl->achHistory[i] - achMean;
        daVar += daDev * daDev;
        achVar += achDev * achDev;
        cov += daDev * achDev;
    }
    
    daVar /= static_cast<float>(pImpl->dopamineHistory.size());
    achVar /= static_cast<float>(pImpl->dopamineHistory.size());
    cov /= static_cast<float>(pImpl->dopamineHistory.size());
    
    float correlation = 0.0f;
    if (daVar > 0.0f && achVar > 0.0f) {
        correlation = cov / std::sqrt(daVar * achVar);
    }
    
    // Return coherence (normalized correlation)
    return std::max(0.0f, correlation);
}

float NeuromodulatorManager::getNeuromodulatorBalance() const {
    if (!pImpl->neuromodulatorsInitialized) return 0.0f;
    
    float currentDA = pImpl->dopamine->getLevel();
    float currentACh = pImpl->acetylcholine->getLevel();
    float currentNE = pImpl->norepinephrine->getLevel();
    float current5HT = pImpl->serotonin->getLevel();
    
    // Calculate balance (how evenly distributed neuromodulators are)
    float sum = currentDA + currentACh + currentNE + current5HT;
    if (sum <= 0.0f) return 0.0f;
    
    // Balance is higher when neuromodulators are more evenly distributed
    float ideal = 0.25f;  // Ideal level for each neuromodulator
    float balanceScore = 0.0f;
    
    balanceScore += (currentDA - ideal) * (currentDA - ideal);
    balanceScore += (currentACh - ideal) * (currentACh - ideal);
    balanceScore += (currentNE - ideal) * (currentNE - ideal);
    balanceScore += (current5HT - ideal) * (current5HT - ideal);
    
    // Convert to balance score (higher is better)
    return std::max(0.0f, 1.0f - balanceScore * 4.0f);
}

float NeuromodulatorManager::getTargetNeuromodulatorLevel(const std::string& modulator, float coherence) const {
    if (!pImpl->neuromodulatorsInitialized) return 0.0f;
    
    // Calculate target level for a neuromodulator based on coherence
    // Higher coherence = more stable target levels
    float baseLevel = 0.1f;  // Baseline level
    
    if (modulator == "DA") {
        return baseLevel + coherence * 0.4f;
    } else if (modulator == "ACh") {
        return baseLevel + coherence * 0.3f;
    } else if (modulator == "NE") {
        return baseLevel + coherence * 0.5f;
    } else if (modulator == "5-HT") {
        return baseLevel + coherence * 0.2f;
    }
    
    return baseLevel;
}

} // namespace nlm