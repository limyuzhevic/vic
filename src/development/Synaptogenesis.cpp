#include "Synaptogenesis.hpp"

namespace nlm {

struct Synaptogenesis::Impl {
    // Synaptogenesis parameters
    float formationRate;                    // Base rate of synapse formation
    float targetDensity;                    // Target synaptic density (synapses per neuron)
    size_t maxSynapsesPerNeuron;            // Maximum outgoing synapses per neuron
    
    // Homeostatic regulation
    float homeostaticRate;                   // Rate of homeostatic adjustment
    float competitionFactor;                 // Strength of Hebbian competition
    float ageDependentFactor;                // Age-dependent modulation factor
    
    // Statistics
    size_t synapsesFormed;
    size_t totalPotentialConnections;
    size_t homeostaticAdjustments;
    size_t competitiveFormations;
    
    // Activity history for Hebbian formation
    std::unordered_map<NeuronId, float> neuronActivityHistory;
    std::unordered_map<NeuronId, size_t> neuronConnectionCount;
    
    Impl(std::shared_ptr<Config> config) 
        : formationRate(config ? config->getOr<float>("synaptogenesis_rate", 0.001f) : 0.001f)
        , targetDensity(config ? config->getOr<float>("synaptogenesis_target_density", 0.1f) : 0.1f)
        , maxSynapsesPerNeuron(config ? config->getOr<size_t>("max_synapses_per_neuron", 100) : 100)
        , homeostaticRate(config ? config->getOr<float>("synaptogenesis_homeostatic_rate", 0.01f) : 0.01f)
        , competitionFactor(config ? config->getOr<float>("synaptogenesis_competition", 0.8f) : 0.8f)
        , ageDependentFactor(config ? config->getOr<float>("synaptogenesis_age_factor", 1.0f) : 1.0f)
        , synapsesFormed(0)
        , totalPotentialConnections(0)
        , homeostaticAdjustments(0)
        , competitiveFormations(0)
    {}
};

Synaptogenesis::Synaptogenesis(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Synaptogenesis::~Synaptogenesis() = default;

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng, const DevelopmentSystem& devSys) {
    if (!brain) return;
    
    // Update activity history for Hebbian formation
    std::vector<NeuronId> neuronIds;
    std::vector<float> neuronActivities;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (Neuron* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                float activity = std::abs(state.membranePotential - state.restingPotential);
                neuronIds.push_back(neuron->getId());
                neuronActivities.push_back(activity);
                
                // Update activity history
                if (pImpl->neuronActivityHistory.find(neuron->getId()) == pImpl->neuronActivityHistory.end() ||
                    activity > pImpl->neuronActivityHistory[neuron->getId()]) {
                    pImpl->neuronActivityHistory[neuron->getId()] = activity;
                }
            }
        }
    }
    
    // Calculate current synaptic density
    size_t totalSynapses = brain->getTotalSynapseCount();
    size_t totalNeurons = brain->getTotalNeuronCount();
    currentDensity_ = totalNeurons > 0 ? static_cast<float>(totalSynapses) / totalNeurons : 0.0f;
    
    // Apply homeostatic regulation to maintain target density
    if (currentDensity_ < targetDensity_) {
        // Need more synapses - promote formation
        float densityDeficit = targetDensity_ - currentDensity_;
        float homeostaticFactor = densityDeficit * pImpl->homeostaticRate;
        
        // Adjust formation rate based on homeostatic needs
        float adjustedFormationRate = formationRate_ * (1.0f + homeostaticFactor);
        
        for (size_t i = 0; i < neuronIds.size(); ++i) {
            // Activity-dependent synapse formation (Hebbian)
            if (neuronActivities[i] > 0.1f) {
                size_t connectionCount = pImpl->neuronConnectionCount.count(neuronIds[i]) ? 
                    pImpl->neuronConnectionCount[neuronIds[i]] : 0;
                
                // Check if neuron has reached max connections
                if (connectionCount < maxSynapsesPerNeuron_) {
                    // Probability of synapse formation based on activity and competition
                    float activityFactor = std::min(1.0f, neuronActivities[i] * 10.0f);
                    float competitionFactor = pImpl->competitionFactor;
                    
                    // Age-dependent modulation
                    float ageFactor = pImpl->ageDependentFactor * devSys.getPlasticityModifier();
                    
                    float formationProbability = adjustedFormationRate * 
                        activityFactor * competitionFactor * ageFactor;
                    
                    if (rng.bernoulli(formationProbability)) {
                        // Find potential partners with complementary activity
                        size_t partnerIdx = rng.uniformInt(0, static_cast<int>(neuronIds.size()) - 1);
                        
                        if (i != partnerIdx) {
                            float partnerActivity = neuronActivities[partnerIdx];
                            float activitySimilarity = 1.0f - std::abs(neuronActivities[i] - partnerActivity);
                            
                            // Form synapse if activities are similar (Hebbian principle)
                            if (activitySimilarity > 0.5f) {
                                float baseWeight = 0.1f + rng.uniformReal(0.0f, 0.2f) * activitySimilarity;
                                if (formSynapse(brain, neuronIds[i], neuronIds[partnerIdx], baseWeight, devSys)) {
                                    ++pImpl->synapsesFormed;
                                    ++pImpl->competitiveFormations;
                                    
                                    // Update connection counts
                                    ++pImpl->neuronConnectionCount[neuronIds[i]];
                                    ++pImpl->neuronConnectionCount[neuronIds[partnerIdx]];
                                }
                            }
                        }
                    }
                }
            }
        }
    } else if (currentDensity_ > targetDensity_ * 1.5f) {
        // Density is too high - apply homeostatic reduction
        float excess = currentDensity_ - targetDensity_ * 1.5f;
        float reductionRate = excess * pImpl->homeostaticRate * 0.1f;
        
        for (const auto& region : brain->getRegions()) {
            for (Synapse* synapse : region->getSynapses()) {
                if (synapse->getWeight() != 0.0f) {
                    // Homeostatic weakening
                    if (rng.bernoulli(reductionRate)) {
                        synapse->setWeight(synapse->getWeight() * 0.9f);
                        ++pImpl->homeostaticAdjustments;
                    }
                }
            }
        }
    }
    
    // Log synaptogenesis statistics periodically
    if (synapsesFormed_ % 1000 == 0 && synapsesFormed_ > 0) {
        NLM_LOG_INFO(std::string("Synaptogenesis: Formed ") + std::to_string(pImpl->synapsesFormed) +
                     std::string(", Current density=") + std::to_string(currentDensity_) +
                     std::string(", Target density=") + std::to_string(targetDensity_));
    }
}

bool Synaptogenesis::formSynapse(Brain* brain, NeuronId source, NeuronId destination,
                                 float baseWeight, const DevelopmentSystem& devSys) {
    if (!brain) return false;
    
    // Check if source and destination are in the same region
    for (auto& region : brain->getRegions()) {
        // Check if both neurons are in this region
        bool sourceInRegion = false;
        bool destinationInRegion = false;
        
        for (const auto& pop : region->getPopulations()) {
            for (Neuron* neuron : pop->getNeurons()) {
                if (neuron->getId() == source) sourceInRegion = true;
                if (neuron->getId() == destination) destinationInRegion = true;
            }
        }
        
        if (sourceInRegion && destinationInRegion) {
            // Check if synapse already exists
            auto synapsesTo = region->getSynapsesTo(destination);
            bool exists = false;
            for (Synapse* syn : synapsesTo) {
                if (syn->getSourceNeuron() == source) {
                    exists = true;
                    break;
                }
            }
            
            if (!exists) {
                // Check max synapses per neuron
                auto synapsesFromSource = region->getSynapsesFrom(source);
                if (synapsesFromSource.size() < maxSynapsesPerNeuron_) {
                    // Form new synapse
                    SynapseId synId = region->addSynapse(source, destination, baseWeight, 1);
                    if (synId != INVALID_SYNAPSE_ID) {
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}

} // namespace nlm
