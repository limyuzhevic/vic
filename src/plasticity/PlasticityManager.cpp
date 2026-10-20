#include "PlasticityManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"

namespace nlm {

struct PlasticityManager::Impl {
    std::shared_ptr<Config> config;
    Brain* brain;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Development system for plasticity modulation
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // Neuromodulation for plasticity
    std::unique_ptr<Neuromodulator> neuromodulator;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg), brain(nullptr) {
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        developmentSystem = std::make_unique<DevelopmentSystem>();
        neuromodulator = std::make_unique<Neuromodulator>();
    }
    
    ~Impl() = default;
};

PlasticityManager::PlasticityManager(std::shared_ptr<Config> config) : 
    pImpl(new Impl(config)),
    hebbianLearningRate(1.0f),
    structuralPlasticityRate(0.001f) {}

PlasticityManager::~PlasticityManager() {
    delete pImpl;
}

bool PlasticityManager::initialize(Brain* brain) {
    NLM_LOG_INFO("Initializing PlasticityManager...");
    
    pImpl->brain = brain;
    
    // Configure STDP parameters
    float ltpWeight = pImpl->config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = pImpl->config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = pImpl->config->getOr<float>("stdp_tau", 20.0f);
    pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
    
    // Configure structural plasticity
    float synaptogenesisRate = pImpl->config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = pImpl->config->getOr<float>("pruning_rate", 0.00001f);
    pImpl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    pImpl->structuralPlasticity->setPruningRate(pruningRate);
    
    NLM_LOG_INFO("PlasticityManager initialized successfully");
    return true;
}

void PlasticityManager::update(Timestamp time, TimestepDuration timestep) {
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->neuromodulator) {
        plasticityMod = pImpl->neuromodulator->getLevel();
    }
    
    // Apply plasticity rules to all synapses
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->stdp->update(syn, preSpikes, postSpikes, timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
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
            syn->step(time);
        }
    }
}

void PlasticityManager::reset() {
    NLM_LOG_INFO("Resetting PlasticityManager...");
    NLM_LOG_INFO("PlasticityManager reset complete");
}

float PlasticityManager::getPlasticityRate() const {
    return 1.0f;  // Placeholder
}

size_t PlasticityManager::getActiveConnections() const {
    size_t total = 0;
    for (const auto& region : pImpl->brain->getRegions()) {
        total += region->getSynapseCount();
    }
    return total;
}

void PlasticityManager::setSTDPParameters(float ltp, float ltd, float tau) {
    if (pImpl->stdp) {
        pImpl->stdp->configure(ltp, ltd, tau);
    }
}

} // namespace nlm