#include "NeuromodulationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuromodulationManager::Impl {
    class Brain* brain;
    
    // Individual neuromodulators
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Serotonin> serotonin;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // Cross-modulation parameters
    float dopamineAChStrength;      // DA effect on ACh release
    float dopamineNEStringth;       // DA effect on NE release
    float dopamine5HTStrength;      // DA effect on 5-HT release
    float AChNEStringth;            // ACh effect on NE release
    float ACh5HTStrength;            // ACh effect on 5-HT release
    float NE5HTStrength;             // NE effect on 5-HT release
    
    // Response curve parameters
    float dopamineResponseCurvature;
    float AChResponseCurvature;
    float NEResponseCurvature;
    float 5HTResponseCurvature;
    
    // Integration weights for combined effects
    float attentionWeight;
    float arousalWeight;
    float rewardWeight;
    float memoryWeight;
    
    Impl() 
        : brain(nullptr)
        , dopamineAChStrength(0.3f)
        , dopamineNEStringth(0.2f)
        , dopamine5HTStrength(0.1f)
        , AChNEStringth(0.2f)
        , ACh5HTStrength(0.1f)
        , NE5HTStrength(0.1f)
        
        , dopamineResponseCurvature(2.0f)
        , AChResponseCurvature(1.5f)
        , NEResponseCurvature(1.8f)
        , 5HTResponseCurvature(1.2f)
        
        , attentionWeight(0.3f)
        , arousalWeight(0.2f)
        , rewardWeight(0.3f)
        , memoryWeight(0.2f) {}
};

NeuromodulationManager::NeuromodulationManager() : pImpl(new Impl) {}

NeuromodulationManager::~NeuromodulationManager() = default;

void NeuromodulationManager::initialize(class Brain* brain) {
    this->pImpl->brain = brain;
    
    // Initialize neuromodulators with brain reference
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->acetylcholine = std::make_unique<Acetylcholine>();
    pImpl->norepinephrine = std::make_unique<Norepinephrine>();
    pImpl->serotonin = std::make_unique<Serotonin>();
    
    // Initialize curiosity and prediction systems
    pImpl->curiosity = std::make_unique<Curiosity>();
    pImpl->predictionError = std::make_unique<PredictionError>();
    pImpl->novelty = std::make_unique<Novelty>();
    
    // Set up curiosity with brain reference
    pImpl->curiosity->initialize(brain);
    
    NLM_LOG_INFO("NeuromodulationManager initialized with cross-modulation system");
}

void NeuromodulationManager::update(TimestepDuration dt) {
    if (!pImpl->brain) return;
    
    // Update individual neuromodulators
    if (pImpl->dopamine) pImpl->dopamine->update(dt);
    if (pImpl->acetylcholine) pImpl->acetylcholine->update(dt);
    if (pImpl->norepinephrine) pImpl->norepinephrine->update(dt);
    if (pImpl->serotonin) pImpl->serotonin->update(dt);
    
    // Cross-modulation effects
    updateCrossModulation(dt);
    
    // Update curiosity based on novelty and prediction error
    if (pImpl->curiosity && pImpl->novelty && pImpl->predictionError) {
        float noveltyLevel = pImpl->novelty->getLevel();
        float predictionErrorLevel = pImpl->predictionError->getMagnitude();
        pImpl->curiosity->update(noveltyLevel, predictionErrorLevel, dt);
    }
}

void NeuromodulationManager::applyPlasticityEffects(const class Brain& brain) {
    if (!pImpl->brain) return;
    
    // Get combined neuromodulatory effects
    float attentionEffect = getAttentionModulation();
    float arousalEffect = getArousalModulation();
    float rewardEffect = getRewardModulation();
    float memoryEffect = getMemoryModulation();
    
    // Apply attention effects to working memory
    if (pImpl->brain->getWorkingMemory() && attentionEffect > 1.0f) {
        pImpl->brain->getWorkingMemory()->setAttentionalGain(attentionEffect);
    }
    
    // Apply arousal effects to neural excitability
    float arousalBoost = (arousalEffect - 1.0f) * 0.5f;
    if (arousalBoost > 0.0f) {
        // Inject arousal-based current into neurons
        for (auto& region : const_cast<Brain&>(brain).getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(arousalBoost * 10.0f);
                }
            }
        }
    }
    
    // Apply reward effects to plasticity
    if (pImpl->dopamine && rewardEffect > 1.0f) {
        float plasticityMod = (rewardEffect - 1.0f) * 0.5f;
        // Modify synaptic weights based on reward signal
        for (auto& region : const_cast<Brain&>(brain).getRegions()) {
            for (auto& syn : region->getSynapses()) {
                float currentWeight = syn->getWeight();
                float weightChange = currentWeight * plasticityMod * 0.001f;
                syn->setWeight(currentWeight + weightChange);
            }
        }
    }
    
    // Apply memory effects to consolidation
    if (pImpl->brain->getEpisodicMemory() && memoryEffect > 1.0f) {
        float consolidationStrength = (memoryEffect - 1.0f) * 0.3f;
        pImpl->brain->getEpisodicMemory()->setConsolidationStrength(consolidationStrength);
    }
}

Dopamine* NeuromodulationManager::getDopamine() {
    return pImpl->dopamine.get();
}

Acetylcholine* NeuromodulationManager::getAcetylcholine() {
    return pImpl->acetylcholine.get();
}

Norepinephrine* NeuromodulationManager::getNorepinephrine() {
    return pImpl->norepinephrine.get();
}

Serotonin* NeuromodulationManager::getSerotonin() {
    return pImpl->serotonin.get();
}

Curiosity* NeuromodulationManager::getCuriosity() {
    return pImpl->curiosity.get();
}

PredictionError* NeuromodulationManager::getPredictionError() {
    return pImpl->predictionError.get();
}

Novelty* NeuromodulationManager::getNovelty() {
    return pImpl->novelty.get();
}

float NeuromodulationManager::getCombinedNeuromodulatoryState() const {
    if (!pImpl) return 1.0f;
    
    float totalWeight = attentionWeight + arousalWeight + rewardWeight + memoryWeight;
    if (totalWeight <= 0.0f) return 1.0f;
    
    // Calculate weighted average of neuromodulatory states
    float attentionState = (pImpl->acetylcholine ? pImpl->acetylcholine->getLevel() : 0.0f) + 
                           (pImpl->norepinephrine ? pImpl->norepinephrine->getLevel() : 0.0f);
    attentionState = std::min(1.0f, attentionState);
    
    float arousalState = (pImpl->norepinephrine ? pImpl->norepinephrine->getLevel() : 0.0f);
    
    float rewardState = (pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f);
    
    float memoryState = (pImpl->serotonin ? pImpl->serotonin->getLevel() : 0.0f) +
                        (pImpl->acetylcholine ? pImpl->acetylcholine->getLevel() : 0.0f);
    memoryState = std::min(1.0f, memoryState);
    
    float combined = (attentionState * attentionWeight + 
                     arousalState * arousalWeight +
                     rewardState * rewardWeight +
                     memoryState * memoryWeight) / totalWeight;
    
    return 0.5f + 0.5f * combined;
}

float NeuromodulationManager::getAttentionModulation() const {
    float AChLevel = pImpl->acetylcholine ? pImpl->acetylcholine->getLevel() : 0.0f;
    float NELevel = pImpl->norepinephrine ? pImpl->norepinephrine->getLevel() : 0.0f;
    return 1.0f + (AChLevel + NELevel) * attentionWeight;
}

float NeuromodulationManager::getArousalModulation() const {
    float NELevel = pImpl->norepinephrine ? pImpl->norepinephrine->getLevel() : 0.0f;
    return 1.0f + NELevel * arousalWeight;
}

float NeuromodulationManager::getRewardModulation() const {
    float DALevel = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
    return 1.0f + DALevel * rewardWeight;
}

float NeuromodulationManager::getMemoryModulation() const {
    float AChLevel = pImpl->acetylcholine ? pImpl->acetylcholine->getLevel() : 0.0f;
    float 5HTLevel = pImpl->serotonin ? pImpl->serotonin->getLevel() : 0.0f;
    return 1.0f + (AChLevel + 5HTLevel) * memoryWeight;
}

void NeuromodulationManager::reset() {
    if (pImpl->dopamine) pImpl->dopamine->reset();
    if (pImpl->acetylcholine) pImpl->acetylcholine->reset();
    if (pImpl->norepinephrine) pImpl->norepinephrine->reset();
    if (pImpl->serotonin) pImpl->serotonin->reset();
    if (pImpl->curiosity) pImpl->curiosity->reset();
}

void NeuromodulationManager::setCrossModulationStrength(float dopamineACh, float dopamineNE, 
                                                         float dopamine5HT, float AChNE, 
                                                         float ACh5HT, float NE5HT) {
    pImpl->dopamineAChStrength = std::clamp(dopamineACh, 0.0f, 1.0f);
    pImpl->dopamineNEStringth = std::clamp(dopamineNE, 0.0f, 1.0f);
    pImpl->dopamine5HTStrength = std::clamp(dopamine5HT, 0.0f, 1.0f);
    pImpl->AChNEStringth = std::clamp(AChNE, 0.0f, 1.0f);
    pImpl->ACh5HTStrength = std::clamp(ACh5HT, 0.0f, 1.0f);
    pImpl->NE5HTStrength = std::clamp(NE5HT, 0.0f, 1.0f);
}

void NeuromodulationManager::setResponseCurves(float dopamineCurvature, float AChCurvature,
                                                float NECurvature, float 5HTCurvature) {
    pImpl->dopamineResponseCurvature = std::max(1.0f, dopamineCurvature);
    pImpl->AChResponseCurvature = std::max(1.0f, AChCurvature);
    pImpl->NEResponseCurvature = std::max(1.0f, NECurvature);
    pImpl->5HTResponseCurvature = std::max(1.0f, 5HTCurvature);
}

void NeuromodulationManager::updateCrossModulation(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Dopamine effects on other neuromodulators
    if (pImpl->dopamine) {
        float DALevel = pImpl->dopamine->getLevel();
        
        // DA facilitates ACh release in prefrontal cortex
        if (pImpl->acetylcholine) {
            float AChBoost = DALevel * pImpl->dopamineAChStrength;
            // Apply boost with response curve
            float AChNewLevel = std::min(pImpl->acetylcholine->getLevel() + 
                                        AChBoost * pImpl->dopamineResponseCurvature,
                                        1.0f);
            pImpl->acetylcholine->setLevel(AChNewLevel);
        }
        
        // DA modulates NE release in locus coeruleus
        if (pImpl->norepinephrine) {
            float NEBoost = DALevel * pImpl->dopamineNEStringth;
            float NENewLevel = std::min(pImpl->norepinephrine->getLevel() + 
                                       NEBoost * pImpl->dopamineResponseCurvature,
                                       1.0f);
            pImpl->norepinephrine->setLevel(NENewLevel);
        }
        
        // DA affects serotonin release in raphe nuclei
        if (pImpl->serotonin) {
            float 5HTBoost = DALevel * pImpl->dopamine5HTStrength;
            float 5HTNewLevel = std::min(pImpl->serotonin->getLevel() + 
                                        5HTBoost * pImpl->dopamineResponseCurvature,
                                        1.0f);
            pImpl->serotonin->setLevel(5HTNewLevel);
        }
    }
    
    // Acetylcholine effects on other neuromodulators
    if (pImpl->acetylcholine) {
        float AChLevel = pImpl->acetylcholine->getLevel();
        
        // ACh facilitates NE release in thalamus and cortex
        if (pImpl->norepinephrine) {
            float NEBoost = AChLevel * pImpl->AChNEStringth;
            float NENewLevel = std::min(pImpl->norepinephrine->getLevel() + 
                                       NEBoost * pImpl->AChResponseCurvature,
                                       1.0f);
            pImpl->norepinephrine->setLevel(NENewLevel);
        }
        
        // ACh affects serotonin release in limbic system
        if (pImpl->serotonin) {
            float 5HTBoost = AChLevel * pImpl->ACh5HTStrength;
            float 5HTNewLevel = std::min(pImpl->serotonin->getLevel() + 
                                        5HTBoost * pImpl->AChResponseCurvature,
                                        1.0f);
            pImpl->serotonin->setLevel(5HTNewLevel);
        }
    }
    
    // Norepinephrine effects on serotonin
    if (pImpl->norepinephrine && pImpl->serotonin) {
        float NELevel = pImpl->norepinephrine->getLevel();
        float 5HTBoost = NELevel * pImpl->NE5HTStrength;
        float 5HTNewLevel = std::min(pImpl->serotonin->getLevel() + 
                                    5HTBoost * pImpl->NEResponseCurvature,
                                    1.0f);
        pImpl->serotonin->setLevel(5HTNewLevel);
    }
}

} // namespace nlm
