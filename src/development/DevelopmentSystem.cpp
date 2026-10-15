#include "DevelopmentSystem.hpp"
#include "Synaptogenesis.hpp"
#include "Pruning.hpp"
#include "Maturation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct DevelopmentSystem::Impl {
    // Developmental stages and timing
    DevelopmentalStage currentStage;
    double developmentalAge;
    double timeSinceLastStageChange;
    
    // Plasticity regulation
    float plasticityRate;
    float structuralPlasticityEnabled;
    float synapticStrengthTarget;
    
    // Development parameters
    double criticalPeriodStart;
    double criticalPeriodEnd;
    double maturationRate;
    double pruningRate;
    
    // State tracking
    size_t synaptogenesisEvents;
    size_t pruningEvents;
    size_t maturationEvents;
    double cumulativeDevelopmentalTime;
    
    Impl() : currentStage(DevelopmentalStage::Initial),
             developmentalAge(0.0),
             timeSinceLastStageChange(0.0),
             plasticityRate(1.0f),
             structuralPlasticityEnabled(1.0f),
             synapticStrengthTarget(1.0f),
             criticalPeriodStart(60.0),  // 60 seconds (1 minute)
             criticalPeriodEnd(300.0), // 300 seconds (5 minutes)
             maturationRate(0.1f),
             pruningRate(0.01f),
             synaptogenesisEvents(0),
             pruningEvents(0),
             maturationEvents(0),
             cumulativeDevelopmentalTime(0.0) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("DevelopmentSystem initialized");
}

DevelopmentSystem::~DevelopmentSystem() = default;

void DevelopmentSystem::initialize(Brain* brain) {
    pImpl->brain_ = brain;
    NLM_LOG_INFO("DevelopmentSystem initialized with brain reference");
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, double timestep) {
    if (!brain) return;
    
    pImpl->developmentalAge += timestep;
    pImpl->cumulativeDevelopmentalTime += timestep;
    pImpl->timeSinceLastStageChange += timestep;
    
    // Update developmental stage based on age
    updateDevelopmentalStage();
    
    // Apply developmental changes based on current stage
    applyDevelopmentEffects(brain, rng, timestep);
    
    // Update plasticity based on development
    updatePlasticityForDevelopment(brain, rng, timestep);
    
    // Record development statistics
    pImpl->synaptogenesisEvents = 0;  // Would be tracked by individual systems
    pImpl->pruningEvents = 0;
    pImpl->maturationEvents = 0;
    
    NLM_LOG_INFO("Development update: age=" + std::to_string(pImpl->developmentalAge) +
                 ", stage=" + std::to_string(static_cast<int>(pImpl->currentStage)));
}

void DevelopmentSystem::updateDevelopmentalStage() {
    // Determine current developmental stage based on age
    DevelopmentalStage oldStage = pImpl->currentStage;
    
    if (pImpl->developmentalAge < pImpl->criticalPeriodStart) {
        pImpl->currentStage = DevelopmentalStage::Initial;
    } else if (pImpl->developmentalAge < pImpl->criticalPeriodEnd) {
        pImpl->currentStage = DevelopmentalStage::CriticalPeriod;
    } else if (pImpl->developmentalAge < 900.0) {  // 15 minutes
        pImpl->currentStage = DevelopmentalStage::Maturation;
    } else {
        pImpl->currentStage = DevelopmentalStage::Adult;
    }
    
    // Log stage changes
    if (pImpl->currentStage != oldStage) {
        NLM_LOG_INFO("Developmental stage changed: " +
                     std::to_string(static_cast<int>(oldStage)) + " -> " +
                     std::to_string(static_cast<int>(pImpl->currentStage)));
    }
}

void DevelopmentSystem::applyDevelopmentEffects(Brain* brain, RandomGenerator& rng, double timestep) {
    // Apply development effects based on current stage
    switch (pImpl->currentStage) {
        case DevelopmentalStage::Initial:
            applyInitialDevelopmentEffects(brain, rng, timestep);
            break;
        case DevelopmentalStage::CriticalPeriod:
            applyCriticalPeriodEffects(brain, rng, timestep);
            break;
        case DevelopmentalStage::Maturation:
            applyMaturationEffects(brain, rng, timestep);
            break;
        case DevelopmentalStage::Adult:
            applyAdultEffects(brain, rng, timestep);
            break;
        case DevelopmentalStage::Aging:
            applyAgingEffects(brain, rng, timestep);
            break;
    }
}

void DevelopmentSystem::applyInitialDevelopmentEffects(Brain* brain, RandomGenerator& rng, double timestep) {
    // Initial phase: high plasticity, rapid learning
    // Increase synaptogenesis
    auto* synaptogenesis = brain->getStructuralPlasticity();
    if (synaptogenesis) {
        synaptogenesis->setSynaptogenesisRate(0.0001f * pImpl->plasticityRate);
    }
    
    // Enhance novelty detection and exploration
    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        curiosity->setNoveltyWeight(1.5f);
    }
    
    // High reward sensitivity
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        dopamine->setBaseline(0.1f);
        dopamine->setReleaseRate(2.0f);
    }
}

void DevelopmentSystem::applyCriticalPeriodEffects(Brain* brain, RandomGenerator& rng, double timestep) {
    // Critical period: language and pattern learning specialization
    // Enhance synaptic pruning for efficiency
    auto* pruning = brain->getStructuralPlasticity();
    if (pruning) {
        pruning->setPruningRate(0.00005f * pImpl->plasticityRate);
    }
    
    // Refine sensory mapping
    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        curiosity->setPredictionErrorWeight(1.2f);
    }
    
    // Balanced reward learning
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        dopamine->setBaseline(0.2f);
        dopamine->setReleaseRate(1.5f);
    }
    
    // Apply developmental constraint learning
    if (pImpl->timeSinceLastStageChange > 10.0) {  // Every 10 seconds
        // Learn stable preferences and patterns
        NLM_LOG_INFO("Critical period: forming stable associations");
    }
}

void DevelopmentSystem::applyMaturationEffects(Brain* brain, RandomGenerator& rng, double timestep) {
    // Maturation: specialization and refinement
    // Stabilize synaptic connections
    auto* synaptogenesis = brain->getStructuralPlasticity();
    if (synaptogenesis) {
        synaptogenesis->setSynaptogenesisRate(0.00002f * pImpl->plasticityRate);
    }
    
    // Fine-tune motor skills
    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        curiosity->setNoveltyWeight(0.8f);
    }
    
    // Mature reward processing
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        dopamine->setBaseline(0.3f);
        dopamine->setReleaseRate(1.2f);
    }
    
    // Learn skill consolidation
    pImpl->maturationEvents++;
    NLM_LOG_INFO("Development: skill maturation and consolidation");
}

void DevelopmentSystem::applyAdultEffects(Brain* brain, RandomGenerator& rng, double timestep) {
    // Adult phase: maintenance and optimization
    // Minimal structural changes
    auto* synaptogenesis = brain->getStructuralPlasticity();
    if (synaptogenesis) {
        synaptogenesis->setSynaptogenesisRate(0.000005f * pImpl->plasticityRate);
    }
    
    // Stable curiosity levels
    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        curiosity->setNoveltyWeight(0.5f);
        curiosity->setPredictionErrorWeight(0.8f);
    }
    
    // Mature, stable reward processing
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        dopamine->setBaseline(0.5f);
        dopamine->setReleaseRate(1.0f);
    }
    
    // Life experience integration
    pImpl->maturationEvents++;
    NLM_LOG_INFO("Development: adult learning and experience integration");
}

void DevelopmentSystem::applyAgingEffects(Brain* brain, RandomGenerator& rng, double timestep) {
    // Aging phase: gradual decline and adaptation
    // Reduced plasticity
    auto* synaptogenesis = brain->getStructuralPlasticity();
    if (synaptogenesis) {
        synaptogenesis->setSynaptogenesisRate(0.000001f * pImpl->plasticityRate);
    }
    
    // Decreased curiosity and exploration
    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        curiosity->setNoveltyWeight(0.2f);
        curiosity->setPredictionErrorWeight(0.5f);
    }
    
    // Flattened reward responsiveness
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        dopamine->setBaseline(0.3f);
        dopamine->setReleaseRate(0.8f);
    }
    
    // Adaptive learning strategies
    pImpl->maturationEvents++;
    NLM_LOG_INFO("Development: adaptive learning for aging brain");
}

void DevelopmentSystem::updatePlasticityForDevelopment(Brain* brain, RandomGenerator& rng, double timestep) {
    // Adjust plasticity based on developmental stage
    float developmentalPlasticityFactor = 1.0f;
    
    switch (pImpl->currentStage) {
        case DevelopmentalStage::Initial:
            developmentalPlasticityFactor = 2.0f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            developmentalPlasticityFactor = 1.5f;
            break;
        case DevelopmentalStage::Maturation:
            developmentalPlasticityFactor = 1.2f;
            break;
        case DevelopmentalStage::Adult:
            developmentalPlasticityFactor = 1.0f;
            break;
        case DevelopmentalStage::Aging:
            developmentalPlasticityFactor = 0.8f;
            break;
    }
    
    // Update STDP parameters based on development
    auto* stdp = brain->getSTDP();
    if (stdp) {
        float baseLearningRate = 0.001f;
        float developmentalModulation = std::min(2.0f, developmentalPlasticityFactor);
        stdp->configure(baseLearningRate * developmentalModulation,
                       baseLearningRate * 1.2f * developmentalModulation,
                       20.0f * developmentalModulation);  // Tau scales with development
    }
    
    // Update Hebbian learning based on development
    auto* hebbian = brain->getHebbian();
    if (hebbian) {
        hebbian->setLearningRate(0.01f * developmentalPlasticityFactor);
    }
}

void DevelopmentSystem::setSynaptogenesisRate(float rate) {
    pImpl->plasticityRate = rate;
}

float DevelopmentSystem::getSynaptogenesisRate() const {
    return pImpl->plasticityRate * 0.0001f;  // Scale factor
}

void DevelopmentSystem::setPruningRate(float rate) {
    pImpl->pruningRate = rate;
}

float DevelopmentSystem::getPruningRate() const {
    return pImpl->pruningRate * 0.00001f;  // Scale factor
}

void DevelopmentSystem::setMaturationRate(float rate) {
    pImpl->maturationRate = rate;
}

float DevelopmentSystem::getMaturationRate() const {
    return pImpl->maturationRate;
}

DevelopmentalStage DevelopmentSystem::getDevelopmentalStage() const {
    return pImpl->currentStage;
}

void DevelopmentSystem::setDevelopmentalStage(DevelopmentalStage stage) {
    if (pImpl->currentStage != stage) {
        NLM_LOG_INFO("Development system stage transition: " +
                     std::to_string(static_cast<int>(pImpl->currentStage)) + " -> " +
                     std::to_string(static_cast<int>(stage)));
        pImpl->currentStage = stage;
        pImpl->timeSinceLastStageChange = 0.0;
    }
}

void DevelopmentSystem::forceDevelopmentStage(DevelopmentalStage stage, double timeOffset) {
    // Force a specific developmental stage (used for testing)
    if (pImpl->currentStage != stage) {
        NLM_LOG_INFO("Forcing developmental stage to: " + std::to_string(static_cast<int>(stage)));
        pImpl->currentStage = stage;
        pImpl->developmentalAge += timeOffset;
    }
}

float DevelopmentSystem::getDevelopmentalProgress() const {
    // Progress from 0.0 (initial) to 1.0 (fully developed)
    float adultAge = 900.0;  // 15 minutes in seconds
    return std::min(1.0f, static_cast<float>(pImpl->developmentalAge) / adultAge);
}

void DevelopmentSystem::recordDevelopmentEvent(DevelopmentalEvent event) {
    // Record developmental events for analysis
    pImpl->cumulativeDevelopmentalTime += event.timeDuration;
    
    switch (event.type) {
        case DevelopmentalEventType::Synaptogenesis:
            pImpl->synaptogenesisEvents++;
            break;
        case DevelopmentalEventType::Pruning:
            pImpl->pruningEvents++;
            break;
        case DevelopmentalEventType::Maturation:
            pImpl->maturationEvents++;
            break;
        default:
            break;
    }
}

std::vector<DevelopmentalEvent> DevelopmentSystem::getDevelopmentEvents() const {
    // Return summary of development events
    std::vector<DevelopmentalEvent> events;
    
    if (pImpl->synaptogenesisEvents > 0) {
        events.push_back({DevelopmentalEventType::Synaptogenesis,
                         static_cast<double>(pImpl->synaptogenesisEvents),
                         0.0});
    }
    
    if (pImpl->pruningEvents > 0) {
        events.push_back({DevelopmentalEventType::Pruning,
                         static_cast<double>(pImpl->pruningEvents),
                         0.0});
    }
    
    if (pImpl->maturationEvents > 0) {
        events.push_back({DevelopmentalEventType::Maturation,
                         static_cast<double>(pImpl->maturationEvents),
                         0.0});
    }
    
    return events;
}

void DevelopmentSystem::reset() {
    pImpl->developmentalAge = 0.0;
    pImpl->timeSinceLastStageChange = 0.0;
    pImpl->cumulativeDevelopmentalTime = 0.0;
    pImpl->synaptogenesisEvents = 0;
    pImpl->pruningEvents = 0;
    pImpl->maturationEvents = 0;
    
    setDevelopmentalStage(DevelopmentalStage::Initial);
    
    NLM_LOG_INFO("DevelopmentSystem reset - returning to initial state");
}

} // namespace nlm
