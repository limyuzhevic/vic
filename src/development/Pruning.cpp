#include "Pruning.hpp"

namespace nlm {

struct Pruning::Impl {
    // Pruning parameters
    float pruningRate;                    // Base pruning probability
    float activityThreshold;              // Minimum activity for synapse survival
    float weightThreshold;                // Minimum absolute weight for survival
    float correlatedActivityThreshold;    // Minimum correlated activity for survival
    
    // Hebbian competition parameters
    float hebbianCompetitionFactor;        // Strength of Hebbian competition
    float homeostaticTarget;              // Target synaptic density
    
    // Statistics
    size_t synapsesPruned;
    size_t totalConnectionsRemoved;
    size_t competitiveEliminations;
    size_t homeostaticRemovals;
    
    // Activity tracking for correlated activity computation
    std::vector<float> neuronRecentActivities;
    std::unordered_map<NeuronId, float> neuronMaxActivityHistory;
    
    Impl(std::shared_ptr<Config> config) 
        : pruningRate(config ? config->getOr<float>("pruning_rate", 0.00001f) : 0.00001f)
        , activityThreshold(config ? config->getOr<float>("pruning_activity_threshold", 0.001f) : 0.001f)
        , weightThreshold(config ? config->getOr<float>("pruning_weight_threshold", 0.01f) : 0.01f)
        , correlatedActivityThreshold(config ? config->getOr<float>("pruning_correlation_threshold", 0.1f) : 0.1f)
        , hebbianCompetitionFactor(config ? config->getOr<float>("pruning_hebbian_competition", 0.8f) : 0.8f)
        , homeostaticTarget(config ? config->getOr<float>("pruning_homeostatic_target", 0.1f) : 0.1f)
        , synapsesPruned(0)
        , totalConnectionsRemoved(0)
        , competitiveEliminations(0)
        , homeostaticRemovals(0)
    {
        // Initialize activity history (max neurons we expect)
        neuronRecentActivities.reserve(10000);
    }
};

Pruning::Pruning(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Pruning::~Pruning() = default;

void Pruning::update(Brain* brain, RandomGenerator& rng, const DevelopmentSystem& devSys) {
    if (!brain) return;
    
    // Update recent activities for correlated activity computation
    std::vector<NeuronId> neuronIds;
    std::vector<float> neuronActivities;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (Neuron* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                float activity = std::abs(state.membranePotential - state.restingPotential);
                neuronIds.push_back(neuron->getId());
                neuronActivities.push_back(activity);
                
                // Update max activity history
                if (pImpl->neuronMaxActivityHistory.find(neuron->getId()) == pImpl->neuronMaxActivityHistory.end() ||
                    activity > pImpl->neuronMaxActivityHistory[neuron->getId()]) {
                    pImpl->neuronMaxActivityHistory[neuron->getId()] = activity;
                }
            }
        }
    }
    
    pImpl->neuronRecentActivities = neuronActivities;
    
    // Identify weak synapses
    identifyWeakSynapses(brain, pImpl->activityThreshold, pImpl->weightThreshold);
    
    // Apply pruning based on activity-dependent survival rules
    size_t synapsesToPrune = brain->getTotalSynapseCount() * pImpl->pruningRate;
    if (synapsesToPrune > 0) {
        size_t prunedCount = 0;
        
        // Collect all synapses with their activity and correlated activity
        std::vector<std::tuple<Synapse*, float, float, float>> synapseCandidates;
        
        for (const auto& region : brain->getRegions()) {
            for (Synapse* synapse : region->getSynapses()) {
                if (synapse->getWeight() == 0.0f) continue;  // Already pruned
                
                // Get pre and post neurons
                Neuron* preNeuron = nullptr;
                Neuron* postNeuron = nullptr;
                
                for (const auto& r : brain->getRegions()) {
                    // Find pre neuron
                    auto* syn = r->getSynapse(synapse->getId());
                    if (syn) {
                        // Get synapses to find the post neuron
                        auto synapsesTo = r->getSynapsesTo(neuronIds.size() > 0 ? neuronIds[0] : 0);
                        for (auto* s : synapsesTo) {
                            if (s->getDestinationNeuron().index() == synapse->getDestinationNeuron().index()) {
                                postNeuron = r->getNeuron(s->getDestinationNeuron());
                                break;
                            }
                        }
                    }
                    
                    if (!preNeuron) {
                        // Find pre neuron directly
                        for (const auto& pop : r->getPopulations()) {
                            for (Neuron* n : pop->getNeurons()) {
                                if (n->getId() == synapse->getSourceNeuron()) {
                                    preNeuron = n;
                                    break;
                                }
                            }
                            if (preNeuron) break;
                        }
                    }
                    
                    if (preNeuron) break;
                }
                
                // Calculate correlated activity for Hebbian competition
                float correlatedActivity = 0.0f;
                if (preNeuron && postNeuron) {
                    float preActivity = preNeuron->getState().firingRate;
                    float postActivity = postNeuron->getState().firingRate;
                    correlatedActivity = preActivity * postActivity;
                }
                
                // Store synapse candidate for pruning decision
                synapseCandidates.push_back(std::make_tuple(
                    synapse,
                    neuronActivities.size() > 0 ? neuronActivities[0] : 0.0f,
                    std::abs(synapse->getWeight()),
                    correlatedActivity
                ));
            }
        }
        
        // Sort synapses by importance (higher = less likely to be pruned)
        std::sort(synapseCandidates.begin(), synapseCandidates.end(),
            [this](const auto& a, const auto& b) {
                float aActivity = std::get<1>(a);
                float aWeight = std::get<2>(a);
                float aCorrelated = std::get<3>(a);
                
                float bActivity = std::get<1>(b);
                float bWeight = std::get<2>(b);
                float bCorrelated = std::get<3>(b);
                
                // Compute importance score
                float aImportance = aActivity * pImpl->hebbianCompetitionFactor + aWeight + aCorrelated;
                float bImportance = bActivity * pImpl->hebbianCompetitionFactor + bWeight + bCorrelated;
                
                return aImportance > bImportance;
            }
        );
        
        // Apply probabilistic pruning, preserving important synapses
        size_t totalSynapses = brain->getTotalSynapseCount();
        if (totalSynapses > 0) {
            size_t keepCount = static_cast<size_t>(totalSynapses * (1.0f - pImpl->pruningRate));
            keepCount = std::max(keepCount, static_cast<size_t>(totalSynapses * 0.5f));  // Keep at least 50%
            
            size_t synapsesToRemove = totalSynapses - keepCount;
            
            for (size_t i = 0; i < synapsesToRemove && i < synapseCandidates.size(); ++i) {
                auto [synapse, activity, weight, correlatedActivity] = synapseCandidates[i];
                
                // Double-check survival rules
                if (shouldSurvive(synapse, nullptr, nullptr, activity, correlatedActivity)) {
                    continue;  // Skip pruning if it should survive
                }
                
                // Prune the synapse
                if (synapse->getWeight() != 0.0f) {
                    synapse->setWeight(0.0f);
                    ++pImpl->synapsesPruned;
                    ++pImpl->totalConnectionsRemoved;
                    ++prunedCount;
                }
            }
            
            // Log pruning results
            if (prunedCount > 0) {
                NLM_LOG_INFO(std::string("Pruning: Removed ") + std::to_string(prunedCount) +
                             std::string(" synapses, remaining=") + std::to_string(totalSynapses - prunedCount));
            }
        }
    }
}

void Pruning::identifyWeakSynapses(Brain* brain, float activityThreshold, float weightThreshold) {
    // This function is called internally by update() to mark synapses for potential pruning
    // The actual pruning happens in update() based on survival rules
}

bool Pruning::shouldSurvive(Synapse* synapse, const Neuron* pre, const Neuron* post,
                           float activityLevel, float correlatedActivity) const {
    // Activity-dependent survival rules (Hebbian competition)
    bool survives = true;
    
    // Rule 1: Prune if weight is below threshold
    if (std::abs(synapse->getWeight()) < weightThreshold_) {
        survives = false;
        ++pImpl->homeostaticRemovals;
    }
    
    // Rule 2: Prune if activity is too low (Hebbian competition)
    else if (activityLevel < activityThreshold_) {
        // Randomly eliminate weak synapses based on activity
        if (correlatedActivity < correlatedActivityThreshold_ && 
            pImpl->neuronMaxActivityHistory.size() > 0) {
            survives = false;
            ++pImpl->competitiveEliminations;
        }
    }
    
    // Rule 3: Preserve synapses with high correlated activity (Hebbian competition preservation)
    else if (correlatedActivity >= correlatedActivityThreshold_) {
        survives = true;  // Always preserve strongly correlated synapses
    }
    
    // Rule 4: Age-dependent survival (implied through development system)
    // Older synapses are more likely to be preserved
    
    return survives;
}

} // namespace nlm
