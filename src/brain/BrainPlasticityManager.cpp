#include "BrainPlasticityManager.hpp"
#include "BrainNeuralCore.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct BrainPlasticityManager::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    
    // Plasticity system components
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Plasticity parameters
    float ltpWeight;
    float ltdWeight;
    float timeConstant;
    float hebbianLearningRate;
    float synaptogenesisRate;
    float pruningRate;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , ltpWeight(0.01f)
        , ltdWeight(0.012f)
        , timeConstant(20.0f)
        , hebbianLearningRate(0.01f)
        , synaptogenesisRate(0.0001f)
        , pruningRate(0.00001f)
    {
        // Initialize random generator
        uint64_t seed = 42;
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize plasticity systems
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // Configure from config
        configureSTDP(ltpWeight, ltdWeight, timeConstant);
        hebbian->setLearningRate(hebbianLearningRate);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
    }
};

BrainPlasticityManager::BrainPlasticityManager(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

BrainPlasticityManager::~BrainPlasticityManager() = default;

bool BrainPlasticityManager::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain Plasticity Manager...");
    
    // Configure STDP parameters
    float ltpWeight = pImpl->config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = pImpl->config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = pImpl->config->getOr<float>("stdp_tau", 20.0f);
    configureSTDP(ltpWeight, ltdWeight, tau);
    
    // Configure Hebbian learning rate
    hebbianLearningRate = pImpl->config->getOr<float>("hebbian_learning_rate", 0.01f);
    pImpl->hebbian->setLearningRate(hebbianLearningRate);
    
    // Configure structural plasticity rates
    float synaptogenesisRate = pImpl->config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = pImpl->config->getOr<float>("pruning_rate", 0.00001f);
    setSynaptogenesisRate(synaptogenesisRate);
    setPruningRate(pruningRate);
    
    NLM_LOG_INFO("NLM Brain Plasticity Manager initialization complete");
    return true;
}

void BrainPlasticityManager::update(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                    SimulationStep currentStep, Timestamp currentTime,
                                    TimestepDuration timestep, float plasticityMod) {
    // Apply STDP and Hebbian plasticity to all synapses
    for (auto& region : regions) {
        for (auto& syn : region->getSynapses()) {
            // Calculate neuromodulation factor for plasticity
            float effectivePlasticityMod = plasticityMod;
            
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Update weight change based on plasticity modulation
                    pImpl->stdp->update(syn, preSpikes, postSpikes, timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (effectivePlasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void BrainPlasticityManager::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    pImpl->stdp->setLTPWeight(0.01f * modulation);
    pImpl->stdp->setLTDWeight(0.012f * modulation);
    
    NLM_LOG_INFO("Applied neuromodulation (level: " + std::to_string(modulation) + ") to plasticity");
}

void BrainPlasticityManager::applyDevelopment(const DevelopmentalStage& stage) {
    // Development affects structural plasticity rates
    auto* sp = pImpl->structuralPlasticity.get();
    if (sp) {
        float plasticityMod = 1.0f;
        
        switch (stage) {
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
        
        float currentSynaptogenesis = pImpl->synaptogenesisRate;
        float currentPruning = pImpl->pruningRate;
        
        sp->setSynaptogenesisRate(currentSynaptogenesis * plasticityMod);
        sp->setPruningRate(currentPruning * (2.0f - plasticityMod));
        
        NLM_LOG_INFO("Applied development stage " + std::to_string(static_cast<int>(stage)) +
                     " to plasticity (synaptogenesis: " + std::to_string(plasticityMod * 0.0001) +
                     ", pruning: " + std::to_string((2.0f - plasticityMod) * 0.00001) + ")");
    }
}

void BrainPlasticityManager::updateStructuralPlasticity(BrainNeuralCore* neuralCore,
                                                       std::unique_ptr<RandomGenerator>& rng) {
    // Apply structural plasticity (synaptogenesis, pruning)
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update(neuralCore, *rng);
    }
}

STDP* BrainPlasticityManager::getSTDP() {
    return pImpl->stdp.get();
}

const STDP* BrainPlasticityManager::getSTDP() const {
    return pImpl->stdp.get();
}

Hebbian* BrainPlasticityManager::getHebbian() {
    return pImpl->hebbian.get();
}

const Hebbian* BrainPlasticityManager::getHebbian() const {
    return pImpl->hebbian.get();
}

StructuralPlasticity* BrainPlasticityManager::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
}

const StructuralPlasticity* BrainPlasticityManager::getStructuralPlasticity() const {
    return pImpl->structuralPlasticity.get();
}

void BrainPlasticityManager::configureSTDP(float ltpWeight, float ltdWeight, float tau) {
    pImpl->ltpWeight = ltpWeight;
    pImpl->ltdWeight = ltdWeight;
    pImpl->timeConstant = tau;
    pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
}

void BrainPlasticityManager::setHebbianLearningRate(float rate) {
    pImpl->hebbianLearningRate = rate;
    pImpl->hebbian->setLearningRate(rate);
}

void BrainPlasticityManager::setSynaptogenesisRate(float rate) {
    pImpl->synaptogenesisRate = rate;
    pImpl->structuralPlasticity->setSynaptogenesisRate(rate);
}

void BrainPlasticityManager::setPruningRate(float rate) {
    pImpl->pruningRate = rate;
    pImpl->structuralPlasticity->setPruningRate(rate);
}

float BrainPlasticityManager::getLTPWeight() const {
    return pImpl->ltpWeight;
}

float BrainPlasticityManager::getLTDWeight() const {
    return pImpl->ltdWeight;
}

float BrainPlasticityManager::getTimeConstant() const {
    return pImpl->timeConstant;
}

float BrainPlasticityManager::getHebbianLearningRate() const {
    return pImpl->hebbianLearningRate;
}

float BrainPlasticityManager::getSynaptogenesisRate() const {
    return pImpl->synaptogenesisRate;
}

float BrainPlasticityManager::getPruningRate() const {
    return pImpl->pruningRate;
}

void BrainPlasticityManager::reset() {
    NLM_LOG_INFO("Resetting NLM Brain Plasticity Manager...");
    
    if (pImpl->stdp) pImpl->stdp->reset();
    if (pImpl->hebbian) pImpl->hebbian->reset();
    if (pImpl->structuralPlasticity) pImpl->structuralPlasticity->reset();
    
    NLM_LOG_INFO("NLM Brain Plasticity Manager reset complete");
}

std::shared_ptr<const Config> BrainPlasticityManager::getConfig() const {
    return pImpl->config;
}

RandomGenerator* BrainPlasticityManager::getRandomGenerator() {
    return pImpl->rng.get();
}

void BrainPlasticityManager::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Plasticity Manager Status ===");
    NLM_LOG_INFO("STDP LTP weight: " + std::to_string(pImpl->ltpWeight));
    NLM_LOG_INFO("STDP LTD weight: " + std::to_string(pImpl->ltdWeight));
    NLM_LOG_INFO("STDP time constant: " + std::to_string(pImpl->timeConstant));
    NLM_LOG_INFO("Hebbian learning rate: " + std::to_string(pImpl->hebbianLearningRate));
    NLM_LOG_INFO("Synaptogenesis rate: " + std::to_string(pImpl->synaptogenesisRate));
    NLM_LOG_INFO("Pruning rate: " + std::to_string(pImpl->pruningRate));
}

} // namespace nlm