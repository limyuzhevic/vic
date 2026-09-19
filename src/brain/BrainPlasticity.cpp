#include "BrainPlasticity.hpp"

namespace nlm {

BrainPlasticity::BrainPlasticity(std::shared_ptr<Config> cfg)
    : config(cfg)
    , stdp(nullptr)
    , hebbian(nullptr)
    , structuralPlasticity(nullptr)
    , developmentalStage(DevelopmentalStage::Initial)
    , plasticityModulationFactor(1.0f)
{
    // Initialize plasticity systems
    stdp = std::make_unique<STDP>();
    hebbian = std::make_unique<Hebbian>();
    structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    // Configure from config
    configureSTDPFromConfig();
    configureStructuralPlasticityFromConfig();
}

BrainPlasticity::~BrainPlasticity() = default;

void BrainPlasticity::initialize() {
    // Already initialized in constructor
}

void BrainPlasticity::configureSTDP(float ltpWeight, float ltdWeight, float tau) {
    if (stdp) {
        stdp->configure(ltpWeight, ltdWeight, tau);
    }
}

void BrainPlasticity::setLTPWeight(float weight) {
    if (stdp) {
        stdp->setLTPWeight(weight);
    }
}

void BrainPlasticity::setLTDWeight(float weight) {
    if (stdp) {
        stdp->setLTDWeight(weight);
    }
}

float BrainPlasticity::getLTPWeight() const {
    if (stdp) {
        return stdp->getLTPWeight();
    }
    return 0.0f;
}

float BrainPlasticity::getLTDWeight() const {
    if (stdp) {
        return stdp->getLTDWeight();
    }
    return 0.0f;
}

void BrainPlasticity::configureStructuralPlasticity(float synaptogenesisRate, float pruningRate) {
    if (structuralPlasticity) {
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
    }
}

void BrainPlasticity::setSynaptogenesisRate(float rate) {
    if (structuralPlasticity) {
        structuralPlasticity->setSynaptogenesisRate(rate);
    }
}

void BrainPlasticity::setPruningRate(float rate) {
    if (structuralPlasticity) {
        structuralPlasticity->setPruningRate(rate);
    }
}

float BrainPlasticity::getSynaptogenesisRate() const {
    if (structuralPlasticity) {
        return structuralPlasticity->getSynaptogenesisRate();
    }
    return 0.0f;
}

float BrainPlasticity::getPruningRate() const {
    if (structuralPlasticity) {
        return structuralPlasticity->getPruningRate();
    }
    return 0.0f;
}

void BrainPlasticity::applySTDP(Synapse* syn, const std::vector<Timestamp>& preSpikes, const std::vector<Timestamp>& postSpikes, TimestepDuration timestep) {
    if (stdp && syn) {
        // Calculate neuromodulation factor for plasticity
        float modulationFactor = calculatePlasticityModulation();
        
        // Apply STDP with neuromodulation
        stdp->update(syn, preSpikes, postSpikes, timestep);
        
        // Apply neuromodulation to weight change
        float weight = syn->getWeight();
        weight += (weight > 0 ? 1.0f : -1.0f) * (modulationFactor - 1.0f) * 0.001f;
        syn->setWeight(weight);
    }
}

void BrainPlasticity::applyHebbian(Synapse* syn, const std::vector<Timestamp>& preSpikes, const std::vector<Timestamp>& postSpikes, TimestepDuration timestep) {
    if (hebbian && syn) {
        hebbian->update(syn, preSpikes, postSpikes, timestep);
    }
}

void BrainPlasticity::applyStructuralPlasticity(Brain* brain, RandomGenerator& rng) {
    if (structuralPlasticity && brain) {
        structuralPlasticity->update(brain, rng);
    }
}

void BrainPlasticity::applyNeuromodulationToPlasticity(float modulationFactor) {
    setPlasticityModulationFactor(modulationFactor);
    
    // Scale STDP learning rates based on neuromodulation
    if (stdp) {
        float ltpWeight = getLTPWeight();
        float ltdWeight = getLTDWeight();
        setLTPWeight(ltpWeight * modulationFactor);
        setLTDWeight(ltdWeight * modulationFactor);
    }
}

void BrainPlasticity::updateDevelopmentEffects() {
    // Apply development stage effects on plasticity rates
    float plasticityMod = 1.0f;
    
    switch (developmentalStage) {
        case DevelopmentalStage::Initial:
            plasticityMod = 1.0f;  // High plasticity
            break;
        case DevelopmentalStage::CriticalPeriod:
            plasticityMod = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            plasticityMod = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            plasticityMod = 0.2f;  // Stable
            break;
    }
    
    // Update structural plasticity rates based on development stage
    if (structuralPlasticity) {
        float currentSynaptogenesis = getSynaptogenesisRate();
        float currentPruning = getPruningRate();
        
        setSynaptogenesisRate(currentSynaptogenesis * plasticityMod);
        setPruningRate(currentPruning * (2.0f - plasticityMod));
    }
}

void BrainPlasticity::reset() {
    if (stdp) stdp->reset();
    if (hebbian) hebbian->reset();
    if (structuralPlasticity) structuralPlasticity->reset();
    
    setDevelopmentalStage(DevelopmentalStage::Initial);
    setPlasticityModulationFactor(1.0f);
}

void BrainPlasticity::logStatus() const {
    // TODO: Add logging implementation
}

void BrainPlasticity::configureSTDPFromConfig() {
    if (!config) return;
    
    float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config->getOr<float>("stdp_tau", 20.0f);
    
    configureSTDP(ltpWeight, ltdWeight, tau);
}

void BrainPlasticity::configureStructuralPlasticityFromConfig() {
    if (!config) return;
    
    float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
    
    configureStructuralPlasticity(synaptogenesisRate, pruningRate);
}

float BrainPlasticity::calculatePlasticityModulation() const {
    // Base modulation factor
    float mod = plasticityModulationFactor;
    
    // Clamp to reasonable range
    if (mod < 0.1f) mod = 0.1f;
    if (mod > 10.0f) mod = 10.0f;
    
    return mod;
}

} // namespace nlm