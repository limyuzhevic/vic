#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Behavioral modulation states
    float behavioralFlexibility;
    float cooperationLevel;
    float riskAversion;
    
    // Decision and impulse control
    float impulseControl;
    float rewardBias;
    float explorationDrive;
    
    // Social and developmental modulation
    float socialCohesion;
    float developmentalTiming;
    float maturationStage;
    
    // State tracking
    float lastRewardValue;
    float lastRiskLevel;
    std::vector<float> socialHistory;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.12f)
        , releaseRate(1.0f)
        , behavioralFlexibility(1.0f)
        , cooperationLevel(1.0f)
        , riskAversion(0.5f)
        , impulseControl(1.0f)
        , rewardBias(0.5f)
        , explorationDrive(1.0f)
        , socialCohesion(1.0f)
        , developmentalTiming(0.5f)
        , maturationStage(0.0f)
        , lastRewardValue(0.0f)
        , lastRiskLevel(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin (5-HT) system initialized for mood, social behavior, and developmental regulation");
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
    
    // Update derived behavioral states
    pImpl->behavioralFlexibility = 0.5f + pImpl->level * 0.8f; // More flexible with high 5-HT
    pImpl->cooperationLevel = 0.5f + pImpl->level * 0.7f; // More cooperative with high 5-HT
    pImpl->riskAversion = 0.3f + pImpl->level * 0.6f; // More risk-averse with high 5-HT
    pImpl->impulseControl = 0.5f + pImpl->level * 0.7f; // Better impulse control with high 5-HT
    pImpl->rewardBias = 0.6f + pImpl->level * 0.3f; // Shift towards exploration/reward seeking
    pImpl->explorationDrive = 0.5f + pImpl->level * 0.5f; // More exploratory with high 5-HT
    pImpl->socialCohesion = 0.5f + pImpl->level * 0.8f; // More social with high 5-HT
}

float Serotonin::getPlasticityFactor() const {
    return 1.0f + pImpl->level * 0.4f; // Strong plasticity modulation for behavioral learning
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    float target = pImpl->baseline;
    pImpl->level += (target - pImpl->level) * 0.06f * static_cast<float>(dt);
    
    // Update developmental timing
    pImpl->maturationStage = std::min(1.0f, pImpl->maturationStage + 0.001f * static_cast<float>(dt));
}

void Serotonin::modulateRiskRewardEvaluation(float rewardValue, float riskLevel) {
    pImpl->lastRewardValue = rewardValue;
    pImpl->lastRiskLevel = riskLevel;
    
    // Modulate decision making based on reward and risk
    float evaluationModulation = 1.0f - pImpl->level * 0.4f; // Higher 5-HT = more conservative evaluation
    
    // Adjust behavior based on 5-HT level
    if (pImpl->level > 0.7f) {
        // High 5-HT: prefer safer, more certain rewards
        if (riskLevel > 0.7f) {
            // Avoid high risk even for high rewards
            // (In implementation: would bias away from risky choices)
        }
    } else if (pImpl->level < 0.3f) {
        // Low 5-HT: more willing to take risks for rewards
        // (In implementation: would bias towards risky but potentially high-reward choices)
    }
    
    // Update reward bias
    pImpl->rewardBias = 0.5f + pImpl->level * 0.4f;
}

void Serotonin::influenceImpulseControl(float decisionBias) {
    pImpl->impulseControl = decisionBias * pImpl->level;
    
    // Apply impulse control modulation
    if (pImpl->brain && pImpl->brain->getWorkingMemory()) {
        auto* workingMemory = pImpl->brain->getWorkingMemory();
        
        // Better impulse control strengthens memory traces of self-control
        workingMemory->strengthenMemory(pImpl->impulseControl * 0.5f);
        
        // Reduce interference from competing responses
        const auto& memoryNeurons = workingMemory->getMemoryNeurons();
        for (const auto& neuronId : memoryNeurons) {
            float activation = workingMemory->getNeuronActivation(neuronId);
            // Apply dampening to prevent runaway excitation (self-control)
            float regulatedActivation = activation * (1.0f - pImpl->level * 0.3f);
            workingMemory->storeToNeuron(neuronId, regulatedActivation);
        }
    }
}

void Serotonin::affectSocialLearning(float socialCohesion) {
    pImpl->socialCohesion = socialCohesion * pImpl->level;
    
    // Enhance social learning and cooperation
    pImpl->cooperationLevel = socialCohesion * pImpl->level;
    pImpl->behavioralFlexibility = socialCohesion * pImpl->level;
    
    // Update social history
    pImpl->socialHistory.push_back(pImpl->socialCohesion);
    if (pImpl->socialHistory.size() > 100) {
        pImpl->socialHistory.erase(pImpl->socialHistory.begin());
    }
    
    // If brain available, modulate attentional selection for social stimuli
    if (pImpl->brain && pImpl->brain->getAttention()) {
        auto* attention = pImpl->brain->getAttention();
        
        // Enhance social stimulus salience
        if (pImpl->socialCohesion > 0.5f) {
            // Would apply social attention bias in full implementation
        }
    }
}

void Serotonin::regulateDevelopmentalTiming(float developmentalStage) {
    pImpl->developmentalTiming = developmentalStage * pImpl->level;
    
    // Modulate development based on 5-HT levels
    if (pImpl->brain) {
        auto* devSystem = pImpl->brain->getDevelopmentSystem();
        if (devSystem) {
            // 5-HT regulates critical periods and developmental transitions
            if (pImpl->level > 0.6f && pImpl->developmentalTiming > 0.7f) {
                // High 5-HT in later development promotes social behavior
                // In full implementation: would adjust developmental rate
            }
        }
    }
}

void Serotonin::enhanceExploration(float explorationDrive) {
    pImpl->explorationDrive = explorationDrive * pImpl->level;
    
    // Apply exploration bias
    if (pImpl->brain && pImpl->brain->getCuriosity()) {
        auto* curiosity = pImpl->brain->getCuriosity();
        
        // Boost curiosity and exploration
        float explorationBoost = pImpl->explorationDrive * 0.5f;
        curiosity->setExplorationDrive(std::min(1.0f, curiosity->getCuriosityLevel() + explorationBoost));
    }
    
    // Also influence attentional selection to explore novel options
    if (pImpl->brain && pImpl->brain->getAttention()) {
        auto* attention = pImpl->brain->getAttention();
        
        // Increase likelihood of switching attention to novel stimuli
        attention->setExcitationStrength(1.2f * pImpl->explorationDrive);
        attention->setInhibitionStrength(0.2f / pImpl->explorationDrive);
    }
}

float Serotonin::getBehavioralFlexibility() const {
    return pImpl->behavioralFlexibility;
}

float Serotonin::getCooperationLevel() const {
    return pImpl->cooperationLevel;
}

float Serotonin::getRiskAversion() const {
    return pImpl->riskAversion;
}

} // namespace nlm
