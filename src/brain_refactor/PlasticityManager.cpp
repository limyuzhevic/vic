#include "PlasticityManager.hpp"
#include "Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct PlasticityManager::Impl {
    Brain* brain;
    
    // Plasticity rule implementations
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Plasticity statistics
    size_t stdpEvents;
    size_t hebbianEvents;
    size_t structuralPlasticityEvents;
    
    // Modulation factors
    float plasticityFactor;
    float developmentModulation;
    
    // Configuration
    float stdpLTPWeight;
    float stdpLTDWeight;
    float stdpTimeConstant;
    float hebbianLearningRate;
    float synaptogenesisRate;
    float pruningRate;
    
    bool initialized;
};

PlasticityManager::PlasticityManager()
    : pImpl(std::make_unique<Impl>()) {
    pImpl->initialized = false;
    pImpl->stdpEvents = 0;
    pImpl->hebbianEvents = 0;
    pImpl->structuralPlasticityEvents = 0;
    pImpl->plasticityFactor = 1.0f;
    pImpl->developmentModulation = 1.0f;
    pImpl->stdpLTPWeight = 0.01f;
    pImpl->stdpLTDWeight = 0.012f;
    pImpl->stdpTimeConstant = 20.0f;
    pImpl->hebbianLearningRate = 0.01f;
    pImpl->synaptogenesisRate = 0.0001f;
    pImpl->pruningRate = 0.00001f;
}

PlasticityManager::~PlasticityManager() = default;

bool PlasticityManager::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Create plasticity rule implementations
    pImpl->stdp = std::make_unique<STDP>();
    pImpl->hebbian = std::make_unique<Hebbian>();
    pImpl->structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    // Configure from brain config
    if (pImpl->brain && pImpl->brain->getConfig()) {
        auto config = pImpl->brain->getConfig();
        
        // STDP configuration
        pImpl->stdpLTPWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        pImpl->stdpLTDWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        pImpl->stdpTimeConstant = config->getOr<float>("stdp_tau", 20.0f);
        pImpl->stdp->configure(pImpl->stdpLTPWeight, pImpl->stdpLTDWeight, pImpl->stdpTimeConstant);
        
        // Hebbian configuration
        pImpl->hebbianLearningRate = config->getOr<float>("hebbian_learning_rate", 0.01f);
        
        // Structural plasticity configuration
        pImpl->synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        pImpl->pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        pImpl->structuralPlasticity->setSynaptogenesisRate(pImpl->synaptogenesisRate);
        pImpl->structuralPlasticity->setPruningRate(pImpl->pruningRate);
    }
    
    pImpl->initialized = true;
    
    return true;
}

void PlasticityManager::update(float dt) {
    if (!pImpl->initialized || !pImpl->brain) {
        return;
    }
    
    // Apply plasticity rules to synapses
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto* synapse : region->getSynapses()) {
            // Apply STDP if enabled
            if (synapse->getPlasticityFlags().stdp) {
                const auto& preSpikes = synapse->getPreSpikeHistory();
                const auto& postSpikes = synapse->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->stdp->update(synapse, preSpikes, postSpikes, dt);
                    pImpl->stdpEvents++;
                    
                    // Apply neuromodulation factor
                    float weight = synapse->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (pImpl->plasticityFactor - 1.0f) * 0.001f;
                    synapse->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning if enabled
            if (synapse->getPlasticityFlags().hebbian) {
                const auto& preSpikes = synapse->getPreSpikeHistory();
                const auto& postSpikes = synapse->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->hebbian->update(synapse, preSpikes, postSpikes, dt);
                    pImpl->hebbianEvents++;
                }
            }
        }
    }
}

void PlasticityManager::reset() {
    pImpl->stdpEvents = 0;
    pImpl->hebbianEvents = 0;
    pImpl->structuralPlasticityEvents = 0;
}

void PlasticityManager::logStatus() const {
    NLM_LOG_INFO("=== PlasticityManager Status ===");
    NLM_LOG_INFO("Initialized: " + std::string(pImpl->initialized ? "Yes" : "No"));
    NLM_LOG_INFO("STDP events: " + std::to_string(pImpl->stdpEvents));
    NLM_LOG_INFO("Hebbian events: " + std::to_string(pImpl->hebbianEvents));
    NLM_LOG_INFO("Structural plasticity events: " + std::to_string(pImpl->structuralPlasticityEvents));
    NLM_LOG_INFO("Plasticity factor: " + std::to_string(pImpl->plasticityFactor));
    NLM_LOG_INFO("Development modulation: " + std::to_string(pImpl->developmentModulation));
}

STDP* PlasticityManager::getSTDP() const {
    return pImpl->stdp.get();
}

Hebbian* PlasticityManager::getHebbian() const {
    return pImpl->hebbian.get();
}

StructuralPlasticity* PlasticityManager::getStructuralPlasticity() const {
    return pImpl->structuralPlasticity.get();
}

void PlasticityManager::configureSTDP(float ltpWeight, float ltdWeight, float tau) {
    pImpl->stdpLTPWeight = ltpWeight;
    pImpl->stdpLTDWeight = ltdWeight;
    pImpl->stdpTimeConstant = tau;
    if (pImpl->stdp) {
        pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
    }
}

float PlasticityManager::getSTDPTimeConstant() const {
    return pImpl->stdpTimeConstant;
}

void PlasticityManager::configureHebbian(float learningRate) {
    pImpl->hebbianLearningRate = learningRate;
}

float PlasticityManager::getHebbianLearningRate() const {
    return pImpl->hebbianLearningRate;
}

void PlasticityManager::updateStructuralPlasticity(Brain* brain, RandomGenerator& rng) {
    if (pImpl->structuralPlasticity && brain) {
        pImpl->structuralPlasticity->update(brain, rng);
        pImpl->structuralPlasticityEvents++;
    }
}

void PlasticityManager::setSynaptogenesisRate(float rate) {
    pImpl->synaptogenesisRate = rate;
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->setSynaptogenesisRate(rate);
    }
}

void PlasticityManager::setPruningRate(float rate) {
    pImpl->pruningRate = rate;
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->setPruningRate(rate);
    }
}

float PlasticityManager::getSynaptogenesisRate() const {
    return pImpl->synaptogenesisRate;
}

float PlasticityManager::getPruningRate() const {
    return pImpl->pruningRate;
}

void PlasticityManager::applyNeuromodulationFactor(float factor) {
    pImpl->plasticityFactor = factor;
}

void PlasticityManager::setPlasticityFactor(float factor) {
    pImpl->plasticityFactor = factor;
}

float PlasticityManager::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void PlasticityManager::applyDevelopmentModulation(DevelopmentalStage stage) {
    float modulation = 1.0f;
    
    switch (stage) {
        case DevelopmentalStage::Initial:
            modulation = 1.0f;  // High plasticity
            break;
        case DevelopmentalStage::CriticalPeriod:
            modulation = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            modulation = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            modulation = 0.2f;  // Stable
            break;
    }
    
    pImpl->developmentModulation = modulation;
}

float PlasticityManager::getDevelopmentModulation(DevelopmentalStage stage) const {
    float modulation = 1.0f;
    
    switch (stage) {
        case DevelopmentalStage::Initial:
            modulation = 1.0f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            modulation = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            modulation = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            modulation = 0.2f;
            break;
    }
    
    return modulation;
}

void PlasticityManager::enablePlasticity(Synapse* synapse, bool hebbian, bool stdp, bool rewardModulated) {
    PlasticityFlags flags;
    flags.hebbian = hebbian;
    flags.stdp = stdp;
    flags.rewardModulated = rewardModulated;
    
    synapse->enablePlasticity(hebbian, stdp, rewardModulated);
}

PlasticityFlags PlasticityManager::getSynapsePlasticity(Synapse* synapse) const {
    return synapse->getPlasticityFlags();
}

size_t PlasticityManager::getSTDPPlasticityEvents() const {
    return pImpl->stdpEvents;
}

size_t PlasticityManager::getHebbianPlasticityEvents() const {
    return pImpl->hebbianEvents;
}

size_t PlasticityManager::getStructuralPlasticityEvents() const {
    return pImpl->structuralPlasticityEvents;
}

} // namespace nlm
