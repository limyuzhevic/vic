// ACh.cpp - Acetylcholine implementation file
#include "Acetylcholine.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

struct Acetylcholine::Impl {
    float level;                    // Normalized level (0-1)
    float concentration;            // Concentration in nM (0-1000 nM)
    float baselineConcentration;    // Baseline concentration (20 nM)
    float homeostaticSetPoint;      // Target concentration (50 nM)
    float homeostaticStrength;      // Homeostatic regulation strength (0.1)
    
    // Cholinergic dynamics
    float corticalActivity;         // Cortical acetylcholine activity (0-1)
    float attentionalDemand;        // Attentional demand (0-1)
    float cholinergicTone;           // Tonic ACh level (0-1)
    
    // Memory consolidation
    float memoryConsolidation;      // Memory consolidation strength (0-1)
    float workingMemoryTrace;       // Working memory trace (0-1)
    
    // VTA ACh projections
    float vtaAChInfluence;           // VTA ACh influence (0-1)
    float prefrontalAChActivity;   // Prefrontal cortex ACh activity (0-1)
    
    // Cross-modulation
    float daModulation;             // DA modulation effect (-1 to +1)
    float neModulation;             // NE modulation effect (-1 to +1)
    float fiveHtModulation;         // 5-HT modulation effect (-1 to +1)
    
    // Time constants
    float timeConstant;             // Time constant for dynamics (ms)
    
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    
    Impl() : level(0.0f), concentration(20.0f), baselineConcentration(20.0f),
             homeostaticSetPoint(50.0f), homeostaticStrength(0.1f),
             corticalActivity(0.0f), attentionalDemand(0.0f), cholinergicTone(0.0f),
             memoryConsolidation(0.0f), workingMemoryTrace(0.0f),
             vtaAChInfluence(0.0f), prefrontalAChActivity(0.0f),
             daModulation(1.0f), neModulation(1.0f), fiveHtModulation(1.0f),
             timeConstant(80.0f),
             rng(std::chrono::steady_clock::now().time_since_epoch().count()),
             uniformDist(0.0f, 1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getConcentration() const {
    return pImpl->concentration;
}

float Acetylcholine::getBaselineConcentration() const {
    return pImpl->baselineConcentration;
}

float Acetylcholine::getHomeostaticSetPoint() const {
    return pImpl->homeostaticSetPoint;
}

bool Acetylcholine::isHomeostaticallyRegulated() const {
    return pImpl->homeostaticStrength > 0.0f;
}

void Acetylcholine::resetToBaseline() {
    pImpl->concentration = pImpl->baselineConcentration;
    pImpl->level = 0.01f;
}

float Acetylcholine::getPlasticityFactor() const {
    float baselinePlasticity = 1.0f;
    float attentionalEnhancement = pImpl->attentionalDemand * 0.5f;
    float memoryConsolidation = pImpl->memoryConsolidation * 0.8f;
    
    return std::min(3.0f, baselinePlasticity + attentionalEnhancement + memoryConsolidation);
}

void Acetylcholine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Homeostatic regulation
    if (pImpl->homeostaticStrength > 0.0f) {
        float deviation = pImpl->concentration - pImpl->homeostaticSetPoint;
        float homeostaticCorrection = -pImpl->homeostaticStrength * deviation * dtFloat;
        pImpl->concentration += homeostaticCorrection;
        pImpl->level += homeostaticCorrection * 0.1f;
    }
    
    // Time constant dynamics
    float decayRate = 1.0f / pImpl->timeConstant;
    float decay = (pImpl->level - 0.01f) * decayRate * dtFloat;
    pImpl->level -= decay;
    pImpl->concentration -= decay * 50.0f;
    
    // Integration with cortical activity
    float activityModulation = pImpl->corticalActivity * pImpl->attentionalDemand;
    pImpl->level += activityModulation * 0.05f * dtFloat;
    
    // Clamp levels
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    pImpl->concentration = std::max(0.0f, pImpl->concentration);
}

void Acetylcholine::updateAChDynamics(float corticalActivity, float attentionalDemand) {
    pImpl->corticalActivity = std::clamp(corticalActivity, 0.0f, 1.0f);
    pImpl->attentionalDemand = std::clamp(attentionalDemand, 0.0f, 1.0f);
    
    float toneAdaptation = (pImpl->attentionalDemand - pImpl->cholinergicTone) * 0.1f;
    pImpl->cholinergicTone += toneAdaptation;
    
    pImpl->level = std::min(pImpl->level + pImpl->cholinergicTone * 0.1f, 1.0f);
    pImpl->concentration += pImpl->level * 50.0f;
}

void Acetylcholine::modulateSensoryGain(float baselineGain, float cholinergicInfluence) {
    float enhancedGain = baselineGain * (1.0f + cholinergicInfluence * 0.5f);
}

void Acetylcholine::triggerMemoryConsolidation(float noveltySignal, float consolidationStrength) {
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + 
                                         noveltySignal * consolidationStrength * 0.1f);
    pImpl->level += pImpl->memoryConsolidation * 0.3f;
}

void Acetylcholine::updateWorkingMemoryTrace(float memoryTrace, float cholinergicModulation) {
    pImpl->workingMemoryTrace = memoryTrace * (1.0f + cholinergicModulation * 0.5f);
    if (pImpl->workingMemoryTrace > 0.5f) {
        pImpl->level += 0.02f;
    }
}

void Acetylcholine::updateVTAChProjections(float dopamineLevel, float rewardPredictionError) {
    pImpl->vtaAChInfluence = dopamineLevel * 0.3f + std::abs(rewardPredictionError) * 0.2f;
}

void Acetylcholine::modulatePrefrontalCortex(float pfActivity, float attentionalFocus) {
    pImpl->prefrontalAChActivity = pfActivity * attentionalFocus;
    pImpl->level += pImpl->prefrontalAChActivity * 0.1f;
}

void Acetylcholine::modulateWithDopamine(float daLevel) {
    pImpl->daModulation = std::clamp(daLevel, 0.0f, 1.0f);
    if (daLevel > 0.5f) {
        pImpl->level *= (1.0f + (daLevel - 0.5f) * 0.2f);
    }
}

void Acetylcholine::modulateWithNorepinephrine(float neLevel) {
    pImpl->neModulation = std::clamp(neLevel, 0.0f, 1.0f);
    pImpl->level += neLevel * 0.1f;
}

void Acetylcholine::modulateWithSerotonin(float fiveHtLevel) {
    pImpl->fiveHtModulation = std::clamp(fiveHtLevel, 0.0f, 1.0f);
    if (fiveHtLevel > 0.7f) {
        pImpl->level *= (1.0f - (fiveHtLevel - 0.7f) * 0.3f);
    }
}

// NE.cpp - Norepinephrine implementation file
#include "Norepinephrine.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

struct Norepinephrine::Impl {
    float level;                    // Normalized level (0-1)
    float concentration;            // Concentration in nM (0-1000 nM)
    float baselineConcentration;    // Baseline concentration (30 nM)
    float homeostaticSetPoint;      // Target concentration (80 nM)
    float homeostaticStrength;      // Homeostatic regulation strength (0.1)
    
    // Locus coeruleus dynamics
    float lcFiringRate;             // Locus coeruleus firing rate (Hz, 0-4 Hz)
    float arousalLevel;            // Arousal level (0-1)
    float signalToNoiseRatio;      // Signal-to-noise ratio enhancement (0-1)
    
    // Stress response
    float stressResponse;           // Stress response level (0-1)
    float stressRecoveryRate;       // Stress recovery rate (1/ms)
    float stressSetPoint;           // Stress target level (0-1)
    
    // NE-dependent plasticity
    float plasticityModulation;     // Plasticity modulation (0-1)
    float receptorSensitivity;      // Receptor sensitivity (0-1)
    
    // Cross-modulation
    float daModulation;             // DA modulation effect (-1 to +1)
    float achModulation;            // ACh modulation effect (-1 to +1)
    float fiveHtModulation;         // 5-HT modulation effect (-1 to +1)
    
    // Time constants
    float timeConstant;             // Time constant for dynamics (ms)
    
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    
    Impl() : level(0.0f), concentration(30.0f), baselineConcentration(30.0f),
             homeostaticSetPoint(80.0f), homeostaticStrength(0.1f),
             lcFiringRate(0.8f), arousalLevel(0.5f), signalToNoiseRatio(1.0f),
             stressResponse(0.0f), stressRecoveryRate(0.001f), stressSetPoint(0.5f),
             plasticityModulation(1.0f), receptorSensitivity(1.0f),
             daModulation(1.0f), achModulation(1.0f), fiveHtModulation(1.0f),
             timeConstant(60.0f),
             rng(std::chrono::steady_clock::now().time_since_epoch().count()),
             uniformDist(0.0f, 1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getConcentration() const {
    return pImpl->concentration;
}

float Norepinephrine::getBaselineConcentration() const {
    return pImpl->baselineConcentration;
}

float Norepinephrine::getHomeostaticSetPoint() const {
    return pImpl->homeostaticSetPoint;
}

bool Norepinephrine::isHomeostaticallyRegulated() const {
    return pImpl->homeostaticStrength > 0.0f;
}

void Norepinephrine::resetToBaseline() {
    pImpl->concentration = pImpl->baselineConcentration;
    pImpl->level = 0.01f;
}

float Norepinephrine::getPlasticityFactor() const {
    float snrEnhancement = pImpl->signalToNoiseRatio * 0.5f;
    float stressModulation = (1.0f - pImpl->stressResponse) * 0.3f;
    
    return std::min(2.0f, 1.0f + snrEnhancement + stressModulation);
}

void Norepinephrine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Homeostatic regulation
    if (pImpl->homeostaticStrength > 0.0f) {
        float deviation = pImpl->concentration - pImpl->homeostaticSetPoint;
        float homeostaticCorrection = -pImpl->homeostaticStrength * deviation * dtFloat;
        pImpl->concentration += homeostaticCorrection;
        pImpl->level += homeostaticCorrection * 0.1f;
    }
    
    // Stress recovery
    if (pImpl->stressResponse > pImpl->stressSetPoint) {
        float recovery = (pImpl->stressResponse - pImpl->stressSetPoint) * 
                        pImpl->stressRecoveryRate * dtFloat;
        pImpl->stressResponse -= recovery;
        pImpl->level -= recovery * 0.2f;
    }
    
    // Time constant dynamics
    float decayRate = 1.0f / pImpl->timeConstant;
    float decay = (pImpl->level - 0.01f) * decayRate * dtFloat;
    pImpl->level -= decay;
    pImpl->concentration -= decay * 50.0f;
    
    // Clamp levels
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    pImpl->concentration = std::max(0.0f, pImpl->concentration);
}

void Norepinephrine::updateLocusCoeruleusFiring(float arousal, float novelty, float stressLevel) {
    pImpl->arousalLevel = std::clamp(arousal, 0.0f, 1.0f);
    
    float baselineFiring = 0.8f;
    float noveltyDrive = novelty * 2.5f;
    float stressInhibition = stressLevel * 0.5f;
    
    pImpl->lcFiringRate = baselineFiring + noveltyDrive - stressInhibition;
    pImpl->lcFiringRate = std::clamp(pImpl->lcFiringRate, 0.0f, 4.0f);
    
    pImpl->level = std::min(pImpl->level + pImpl->lcFiringRate * 0.1f, 1.0f);
    pImpl->concentration += pImpl->lcFiringRate * 50.0f;
}

void Norepinephrine::modulateSignalToNoiseRatio(float baselineSNR, float neInfluence) {
    pImpl->signalToNoiseRatio = baselineSNR * (1.0f + neInfluence * 0.8f);
    pImpl->signalToNoiseRatio = std::min(5.0f, pImpl->signalToNoiseRatio);
}

void Norepinephrine::updateArousalState(float environmentalStimuli, float internalState) {
    float stimulusResponse = environmentalStimuli * 0.7f;
    float internalDrive = internalState * 0.3f;
    
    pImpl->arousalLevel = (stimulusResponse + internalDrive) * 0.5f;
    
    pImpl->lcFiringRate = 0.8f + pImpl->arousalLevel * 3.2f;
    pImpl->level = std::min(pImpl->level + pImpl->arousalLevel * 0.2f, 1.0f);
}

void Norepinephrine::triggerStressResponse(float stressorIntensity, float duration) {
    pImpl->stressResponse = std::min(1.0f, pImpl->stressResponse + 
                                    stressorIntensity * duration * 0.1f);
    pImpl->level = std::min(pImpl->level + pImpl->stressResponse * 0.3f, 1.0f);
}

void Norepinephrine::updateStressRecovery(float recoveryRate) {
    pImpl->stressRecoveryRate = recoveryRate;
    
    if (pImpl->stressResponse > 0.0f) {
        float recovery = pImpl->stressRecoveryRate * recoveryRate;
        pImpl->stressResponse -= recovery;
        pImpl->stressResponse = std::max(0.0f, pImpl->stressResponse);
    }
}

void Norepinephrine::setStressSetPoint(float setpoint) {
    pImpl->stressSetPoint = std::clamp(setpoint, 0.0f, 1.0f);
}

void Norepinephrine::modulateSynapticPlasticity(float baselinePlasticity, float neInfluence) {
    pImpl->plasticityModulation = baselinePlasticity * (1.0f + neInfluence * 0.5f);
    pImpl->plasticityModulation = std::min(2.0f, pImpl->plasticityModulation);
}

void Norepinephrine::updateReceptorSensitivity(float sensitivity, float adaptationRate) {
    pImpl->receptorSensitivity = std::clamp(sensitivity, 0.0f, 1.0f);
    pImpl->stressRecoveryRate *= (1.0f + adaptationRate * 0.1f);
}

void Norepinephrine::modulateWithDopamine(float daLevel) {
    pImpl->daModulation = std::clamp(daLevel, 0.0f, 1.0f);
    if (daLevel > 0.5f && pImpl->arousalLevel > 0.5f) {
        pImpl->level *= 1.2f;
    }
}

void Norepinephrine::modulateWithAcetylcholine(float achLevel) {
    pImpl->achModulation = std::clamp(achLevel, 0.0f, 1.0f);
    pImpl->level += achLevel * 0.1f;
}

void Norepinephrine::modulateWithSerotonin(float fiveHtLevel) {
    pImpl->fiveHtModulation = std::clamp(fiveHtLevel, 0.0f, 1.0f);
    if (fiveHtLevel > 0.7f) {
        pImpl->level *= (1.0f - (fiveHtLevel - 0.7f) * 0.4f);
    }
}

// 5HT.cpp - Serotonin implementation file
#include "Serotonin.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

struct Serotonin::Impl {
    float level;                    // Normalized level (0-1)
    float concentration;            // Concentration in nM (0-1000 nM)
    float baselineConcentration;    // Baseline concentration (40 nM)
    float homeostaticSetPoint;      // Target concentration (60 nM)
    float homeostaticStrength;      // Homeostatic regulation strength (0.1)
    
    // Raphe nucleus dynamics
    float rapheFiringRate;          // Raphe nucleus firing rate (Hz, 0-8 Hz)
    float moodState;                // Mood state (0-1)
    float behavioralInhibition;    // Behavioral inhibition level (0-1)
    
    // Decision making
    float explorationDrive;         // Exploration vs exploitation (0-1)
    float exploitationValue;        // Exploitation value (0-1)
    float decisionUncertainty;      // Decision uncertainty (0-1)
    
    // 5-HT receptor subtypes
    float fiveHT1AReceptorActivation; // 5-HT1A receptor activation (0-1)
    float fiveHT2AReceptorActivation; // 5-HT2A receptor activation (0-1)
    
    // Punishment and avoidance
    float punishmentSensitivity;    // Sensitivity to punishment (0-1)
    float avoidanceLearning;        // Avoidance learning strength (0-1)
    
    // Cross-modulation
    float daModulation;             // DA modulation effect (-1 to +1)
    float achModulation;            // ACh modulation effect (-1 to +1)
    float neModulation;             // NE modulation effect (-1 to +1)
    
    // Time constants
    float timeConstant;             // Time constant for dynamics (ms)
    
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    
    Impl() : level(0.0f), concentration(40.0f), baselineConcentration(40.0f),
             homeostaticSetPoint(60.0f), homeostaticStrength(0.1f),
             rapheFiringRate(1.5f), moodState(0.5f), behavioralInhibition(0.5f),
             explorationDrive(0.5f), exploitationValue(0.5f), decisionUncertainty(0.5f),
             fiveHT1AReceptorActivation(0.0f), fiveHT2AReceptorActivation(0.0f),
             punishmentSensitivity(0.5f), avoidanceLearning(0.5f),
             daModulation(1.0f), achModulation(1.0f), neModulation(1.0f),
             timeConstant(90.0f),
             rng(std::chrono::steady_clock::now().time_since_epoch().count()),
             uniformDist(0.0f, 1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getConcentration() const {
    return pImpl->concentration;
}

float Serotonin::getBaselineConcentration() const {
    return pImpl->baselineConcentration;
}

float Serotonin::getHomeostaticSetPoint() const {
    return pImpl->homeostaticSetPoint;
}

bool Serotonin::isHomeostaticallyRegulated() const {
    return pImpl->homeostaticStrength > 0.0f;
}

void Serotonin::resetToBaseline() {
    pImpl->concentration = pImpl->baselineConcentration;
    pImpl->level = 0.01f;
}

float Serotonin::getPlasticityFactor() const {
    float moodEffect = (1.0f - pImpl->moodState) * 0.3f;
    float inhibitionEffect = pImpl->behavioralInhibition * 0.4f;
    
    return std::max(0.1f, 1.0f + moodEffect - inhibitionEffect);
}

void Serotonin::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Homeostatic regulation
    if (pImpl->homeostaticStrength > 0.0f) {
        float deviation = pImpl->concentration - pImpl->homeostaticSetPoint;
        float homeostaticCorrection = -pImpl->homeostaticStrength * deviation * dtFloat;
        pImpl->concentration += homeostaticCorrection;
        pImpl->level += homeostaticCorrection * 0.1f;
    }
    
    // Time constant dynamics
    float decayRate = 1.0f / pImpl->timeConstant;
    float decay = (pImpl->level - 0.01f) * decayRate * dtFloat;
    pImpl->level -= decay;
    pImpl->concentration -= decay * 50.0f;
    
    // Clamp levels
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    pImpl->concentration = std::max(0.0f, pImpl->concentration);
}

void Serotonin::updateRapheDynamics(float moodState, float behavioralDemand) {
    pImpl->moodState = std::clamp(moodState, 0.0f, 1.0f);
    
    float baselineFiring = 1.5f;
    float moodDrive = (1.0f - pImpl->moodState) * 3.0f;
    float behavioralDemand = behavioralDemand * 2.0f;
    
    pImpl->rapheFiringRate = baselineFiring + moodDrive - behavioralDemand;
    pImpl->rapheFiringRate = std::clamp(pImpl->rapheFiringRate, 0.0f, 8.0f);
    
    pImpl->level = std::min(pImpl->level + pImpl->rapheFiringRate * 0.1f, 1.0f);
    pImpl->concentration += pImpl->rapheFiringRate * 40.0f;
}

void Serotonin::modulateMoodRegulation(float baselineMood, float 5htInfluence) {
    float moodModulation = (pImpl->fiveHT1AReceptorActivation * 0.6f - 
                           pImpl->fiveHT2AReceptorActivation * 0.4f) * 5htInfluence;
    
    pImpl->moodState = baselineMood + moodModulation;
    pImpl->moodState = std::clamp(pImpl->moodState, 0.0f, 1.0f);
}

void Serotonin::processPunishmentSignal(float punishmentIntensity, float aversiveLearning) {
    pImpl->punishmentSensitivity = std::min(1.0f, 
                                           pImpl->punishmentSensitivity + 
                                           punishmentIntensity * aversiveLearning * 0.1f);
    
    pImpl->behavioralInhibition = std::min(1.0f, pImpl->behavioralInhibition + 
                                          pImpl->punishmentSensitivity * 0.3f);
    
    pImpl->level += pImpl->punishmentSensitivity * 0.2f;
}

void Serotonin::updateAvoidanceLearning(float threatLevel, float avoidanceStrength) {
    pImpl->avoidanceLearning = std::min(1.0f, 
                                       pImpl->avoidanceLearning + 
                                       threatLevel * avoidanceStrength * 0.05f);
}

void Serotonin::updateStrategySwitching(float explorationDrive, float exploitationValue) {
    pImpl->explorationDrive = std::clamp(explorationDrive, 0.0f, 1.0f);
    pImpl->exploitationValue = std::clamp(exploitationValue, 0.0f, 1.0f);
    
    if (pImpl->level > 0.7f) {
        pImpl->explorationDrive *= (1.0f - (pImpl->level - 0.7f) * 1.5f);
        pImpl->exploitationValue *= (1.0f + (pImpl->level - 0.7f) * 0.5f);
    }
    
    pImpl->decisionUncertainty = 1.0f - std::abs(pImpl->explorationDrive - pImpl->exploitationValue);
}

void Serotonin::modulateDecisionMaking(float choiceUncertainty, float 5htModulation) {
    float uncertaintyModulation = choiceUncertainty * 5htModulation * 0.5f;
    
    pImpl->decisionUncertainty = std::max(0.1f, pImpl->decisionUncertainty - uncertaintyModulation);
}

float Serotonin::get5HT1AReceptorActivation() const {
    return pImpl->fiveHT1AReceptorActivation;
}

float Serotonin::get5HT2AReceptorActivation() const {
    return pImpl->fiveHT2AReceptorActivation;
}

void Serotonin::set5HT1AReceptorActivation(float activation) {
    pImpl->fiveHT1AReceptorActivation = std::clamp(activation, 0.0f, 1.0f);
}

void Serotonin::set5HT2AReceptorActivation(float activation) {
    pImpl->fiveHT2AReceptorActivation = std::clamp(activation, 0.0f, 1.0f);
}

void Serotonin::modulateWithDopamine(float daLevel) {
    pImpl->daModulation = std::clamp(daLevel, 0.0f, 1.0f);
    if (daLevel > 0.7f && pImpl->fiveHT1AReceptorActivation > 0.5f) {
        pImpl->level *= 0.9f;
    }
}

void Serotonin::modulateWithAcetylcholine(float achLevel) {
    pImpl->achModulation = std::clamp(achLevel, 0.0f, 1.0f);
    pImpl->level += achLevel * 0.05f;
}

void Serotonin::modulateWithNorepinephrine(float neLevel) {
    pImpl->neModulation = std::clamp(neLevel, 0.0f, 1.0f);
    if (neLevel > 0.7f && pImpl->moodState < 0.5f) {
        pImpl->level *= (1.0f - (neLevel - 0.7f) * 0.3f);
    }
}

} // namespace nlm