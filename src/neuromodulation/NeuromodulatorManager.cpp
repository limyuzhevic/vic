#include "NeuromodulatorManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuromodulationManager::Impl {
    Brain* brain;
    
    // Neuromodulator instances
    std::unique_ptr<ACh> ach;
    std::unique_ptr<NE> ne;
    std::unique_ptr<Serotonin> serotonin;
    std::unique_ptr<Dopamine> dopamine;
    
    // System flags
    bool achEnabled;
    bool neEnabled;
    bool serotoninEnabled;
    bool dopamineEnabled;
    
    Impl()
        : brain(nullptr)
        , ach(nullptr)
        , ne(nullptr)
        , serotonin(nullptr)
        , dopamine(nullptr)
        , achEnabled(true)
        , neEnabled(true)
        , serotoninEnabled(true)
        , dopamineEnabled(true) {}
};

NeuromodulationManager::NeuromodulationManager() : pImpl(new Impl) {}

NeuromodulationManager::~NeuromodulationManager() = default;

void NeuromodulationManager::initialize(Brain* brainRef) {
    pImpl->brain = brainRef;
    
    // Initialize neuromodulators
    ach = std::make_unique<ACh>();
    ne = std::make_unique<NE>();
    serotonin = std::make_unique<Serotonin>();
    dopamine = std::make_unique<Dopamine>();
    
    NLM_LOG_INFO("NeuromodulationManager initialized with ACh, NE, 5-HT, and DA systems");
}

void NeuromodulationManager::updateAll(TimestepDuration dt) {
    // Update each neuromodulator if enabled
    if (achEnabled && ach) {
        ach->update(dt);
    }
    if (neEnabled && ne) {
        ne->update(dt);
    }
    if (serotoninEnabled && serotonin) {
        serotonin->update(dt);
    }
    if (dopamineEnabled && dopamine) {
        dopamine->update(dt);
    }
}

void NeuromodulationManager::processReward(float reward) {
    if (dopamineEnabled && dopamine) {
        dopamine->signalReward(reward);
    }
    if (serotoninEnabled && serotonin) {
        serotonin->signalReward(reward);
    }
    if (achEnabled && ach) {
        // ACh responds to reward with memory encoding enhancement
        ach->signalMemoryEncoding(reward * 0.3f);
    }
}

void NeuromodulationManager::processPredictionError(float error) {
    if (dopamineEnabled && dopamine) {
        dopamine->signalRewardPredictionError(error);
    }
    if (neEnabled && ne) {
        ne->signalPredictionError(error);
    }
    if (serotoninEnabled && serotonin) {
        // 5-HT modulates decision making based on prediction errors
        serotonin->signalSocialFeedback(error * 0.5f);
    }
}

void NeuromodulationManager::processNovelty(float novelty) {
    if (neEnabled && ne) {
        ne->signalNovelty(novelty);
    }
    if (achEnabled && ach) {
        ach->signalAttention(novelty * 0.8f, novelty);
    }
    if (dopamineEnabled && dopamine) {
        // Dopamine responds to novelty for exploration drive
        dopamine->signalReward(novelty * 0.3f);
    }
}

void NeuromodulationManager::processAttention(float salience, float focus) {
    if (achEnabled && ach) {
        ach->signalAttention(salience, focus);
    }
    if (neEnabled && ne) {
        ne->signalArousal(salience * 0.5f);
    }
}

void NeuromodulationManager::processSocialFeedback(float feedback) {
    if (serotoninEnabled && serotonin) {
        serotonin->signalSocialFeedback(feedback);
    }
    if (dopamineEnabled && dopamine) {
        dopamine->signalReward(feedback * 0.5f);
    }
}

void NeuromodulationManager::processImpulsivity(float level) {
    if (serotoninEnabled && serotonin) {
        serotonin->signalImpulsivity(level);
    }
    if (neEnabled && ne) {
        // NE responds to impulsivity with stress signal
        ne->signalStress(level * 0.3f);
    }
}

void NeuromodulationManager::processStress(float stressLevel) {
    if (neEnabled && ne) {
        ne->signalStress(stressLevel);
    }
    if (serotoninEnabled && serotonin) {
        // 5-HT responds to stress with sleep modulation
        serotonin->signalSleepModulation(stressLevel * 0.3f);
    }
}

float NeuromodulationManager::getAChLevel() const {
    return ach ? ach->getLevel() : 0.0f;
}

float NeuromodulationManager::getNELevel() const {
    return ne ? ne->getLevel() : 0.0f;
}

float NeuromodulationManager::get5HTLevel() const {
    return serotonin ? serotonin->getLevel() : 0.0f;
}

float NeuromodulationManager::getDALevel() const {
    return dopamine ? dopamine->getLevel() : 0.0f;
}

float NeuromodulationManager::getIntegratedPlasticityFactor() const {
    float totalFactor = 1.0f;
    
    if (achEnabled && ach) {
        totalFactor *= ach->getPlasticityFactor();
    }
    if (neEnabled && ne) {
        totalFactor *= ne->getPlasticityFactor();
    }
    if (serotoninEnabled && serotonin) {
        totalFactor *= serotonin->getPlasticityFactor();
    }
    if (dopamineEnabled && dopamine) {
        totalFactor *= dopamine->getPlasticityFactor();
    }
    
    return totalFactor;
}

float NeuromodulationManager::getAttentionModulationFactor() const {
    return ach ? ach->getAttentionGain() : 1.0f;
}

float NeuromodulationManager::getArousalModulationFactor() const {
    return ne ? ne->getArousalLevel() : 1.0f;
}

float NeuromodulationManager::getMoodModulationFactor() const {
    return serotonin ? serotonin->getMoodRegulationFactor() : 1.0f;
}

void NeuromodulationManager::reset() {
    if (ach) ach->reset();
    if (ne) ne->reset();
    if (serotonin) serotonin->reset();
    if (dopamine) dopamine->reset();
}

void NeuromodulationManager::configureAChParameters(float baseline, float decayRate, float releaseRate) {
    if (ach) {
        ach->setBaseline(baseline);
        ach->setDecayRate(decayRate);
        ach->setReleaseRate(releaseRate);
    }
}

void NeuromodulationManager::configureNEParameters(float baseline, float decayRate, float releaseRate) {
    if (ne) {
        ne->setBaseline(baseline);
        ne->setDecayRate(decayRate);
        ne->setReleaseRate(releaseRate);
    }
}

void NeuromodulationManager::configure5HTParameters(float baseline, float decayRate, float releaseRate) {
    if (serotonin) {
        serotonin->setBaseline(baseline);
        serotonin->setDecayRate(decayRate);
        serotonin->setReleaseRate(releaseRate);
    }
}

void NeuromodulationManager::configureDAParameters(float baseline, float decayRate, float releaseRate) {
    if (dopamine) {
        dopamine->setBaseline(baseline);
        dopamine->setDecayRate(decayRate);
        dopamine->setReleaseRate(releaseRate);
    }
}

void NeuromodulationManager::enableACh(bool enable) {
    achEnabled = enable;
}

void NeuromodulationManager::enableNE(bool enable) {
    neEnabled = enable;
}

void NeuromodulationManager::enable5HT(bool enable) {
    serotoninEnabled = enable;
}

void NeuromodulationManager::enableDA(bool enable) {
    dopamineEnabled = enable;
}

ACh* NeuromodulationManager::getACh() const {
    return ach.get();
}

NE* NeuromodulationManager::getNE() const {
    return ne.get();
}

Serotonin* NeuromodulationManager::get5HT() const {
    return serotonin.get();
}

Dopamine* NeuromodulationManager::getDA() const {
    return dopamine.get();
}

const char* NeuromodulationManager::getAChName() const {
    return ach ? ach->getName() : "ACh";
}

const char* NeuromodulationManager::getNEName() const {
    return ne ? ne->getName() : "NE";
}

const char* NeuromodulationManager::get5HTName() const {
    return serotonin ? serotonin->getName() : "5-HT";
}

const char* NeuromodulationManager::getDAName() const {
    return dopamine ? dopamine->getName() : "DA";
}

} // namespace nlm