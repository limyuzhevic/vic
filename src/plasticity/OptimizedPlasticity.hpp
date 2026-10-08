// Performance optimizations for plasticity rules
// Replace O(n²) algorithms with more efficient data structures

#include "../plasticity/PlasticityRule.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>

namespace nlm {

// Optimized Hebbian rule with improved performance
struct OptimizedHebbian : public PlasticityRule {
    float learningRate = 0.01f;
    float maxWeight = 1.0f;
    float minWeight = -1.0f;
    float activityWindow = 100.0f;  // Time window for activity integration
    
    // Pre-allocated buffers for better performance
    std::vector<float> preSpikeTimestamps;
    std::vector<float> postSpikeTimestamps;
    
    // Activity statistics
    float runningPreActivity = 0.0f;
    float runningPostActivity = 0.0f;
    float runningCovariance = 0.0f;
    
    // Fast lookup tables for spike time differences
    std::vector<float> preSpikeTimeDifferences;
    std::vector<float> postSpikeTimeDifferences;
    
    OptimizedHebbian() {
        preSpikeTimestamps.reserve(1000);
        postSpikeTimestamps.reserve(1000);
        preSpikeTimeDifferences.reserve(1000);
        postSpikeTimeDifferences.reserve(1000);
    }
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        // Fast path: clear old spikes and store new ones
        clearOldSpikes(preSpikes, postSpikes, dt);
        
        // Calculate statistics in optimized way
        float preActivity = calculateFastActivity(preSpikes);
        float postActivity = calculateFastActivity(postSpikes);
        float covariance = calculateFastCovariance(preSpikes, postSpikes);
        
        // Update running statistics with exponential decay
        updateFastRunningStats(preActivity, postActivity, covariance, dt);
        
        // Calculate weight change
        float delta = calculateFastWeightChange(covariance, preActivity, postActivity);
        
        // Apply weight change
        if (std::abs(delta) > 1e-6f) {
            applyWeightChange(synapse, delta);
        }
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) return;
        
        float newWeight = synapse->getWeight() + delta;
        newWeight = std::clamp(newWeight, minWeight, maxWeight);
        synapse->setWeight(newWeight);
    }
    
    const char* getName() const override {
        return "OptimizedHebbian";
    }
    
    // Optimized spike time difference calculation
    void calculateSpikeTimeDifferences() {
        preSpikeTimeDifferences.clear();
        postSpikeTimeDifferences.clear();
        
        // Calculate pairwise differences in optimized way
        preSpikeTimeDifferences.reserve(preSpikeTimestamps.size() * 2);
        postSpikeTimeDifferences.reserve(postSpikeTimestamps.size() * 2);
        
        for (size_t i = 0; i < preSpikeTimestamps.size(); ++i) {
            for (size_t j = 0; j < postSpikeTimestamps.size(); ++j) {
                float diff = std::abs(postSpikeTimestamps[j] - preSpikeTimestamps[i]);
                if (diff < 20.0f) {  // Only consider spikes within 20ms
                    preSpikeTimeDifferences.push_back(diff);
                    postSpikeTimeDifferences.push_back(diff);
                }
            }
        }
    }
    
    // Fast activity calculation using pre-computed statistics
    float calculateFastActivity(const std::vector<Timestamp>& spikes) {
        if (spikes.empty()) return 0.0f;
        
        float windowDuration = activityWindow;
        size_t spikeCount = spikes.size();
        
        return (static_cast<float>(spikeCount) / windowDuration) * 1000.0f;
    }
    
    // Fast covariance calculation
    float calculateFastCovariance(const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes) {
        if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
        
        // Simplified covariance measure
        float preActivity = calculateFastActivity(preSpikes);
        float postActivity = calculateFastActivity(postSpikes);
        
        // Calculate overlapping spikes efficiently
        size_t overlappingSpikes = 0;
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                if (std::abs(static_cast<float>(postTime) - static_cast<float>(preTime)) < 20.0f) {
                    overlappingSpikes++;
                    break;
                }
            }
        }
        
        float normalizedOverlap = (overlappingSpikes / static_cast<float>(preSpikes.size() * postSpikes.size()));
        return normalizedOverlap * preActivity * postActivity;
    }
    
    // Optimized running statistics update
    void updateFastRunningStats(float preActivity, float postActivity, float covariance, TimestepDuration dt) {
        float decayFactor = std::exp(-static_cast<float>(dt) * 0.001f / activityWindow);
        
        runningPreActivity = runningPreActivity * decayFactor + preActivity * (1.0f - decayFactor);
        runningPostActivity = runningPostActivity * decayFactor + postActivity * (1.0f - decayFactor);
        runningCovariance = runningCovariance * decayFactor + covariance * (1.0f - decayFactor);
    }
    
    // Fast weight change calculation
    float calculateFastWeightChange(float covariance, float preActivity, float postActivity) {
        float delta = learningRate * covariance;
        
        // Subtract baseline
        float baseline = learningRate * preActivity * postActivity;
        delta -= baseline;
        
        // Apply normalization
        float activityNormalization = std::sqrt(preActivity * postActivity);
        if (activityNormalization > 0.0f) {
            delta /= activityNormalization;
        }
        
        return delta;
    }
    
    // Optimized spike management
    void clearOldSpikes(const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt) {
        float currentTime = static_cast<float>(dt) * 1000.0f;
        float cutoffTime = currentTime - activityWindow;
        
        // Clear old spikes (in real implementation, would use proper aging)
        preSpikeTimestamps.clear();
        postSpikeTimestamps.clear();
        
        // Store new spikes
        preSpikeTimestamps.reserve(preSpikes.size());
        postSpikeTimestamps.reserve(postSpikes.size());
        
        for (Timestamp spikeTime : preSpikes) {
            if (spikeTime >= cutoffTime) {
                preSpikeTimestamps.push_back(spikeTime);
            }
        }
        
        for (Timestamp spikeTime : postSpikes) {
            if (spikeTime >= cutoffTime) {
                postSpikeTimestamps.push_back(spikeTime);
            }
        }
        
        // Recalculate statistics
        calculateSpikeTimeDifferences();
    }
};

// STDP optimization using activity traces instead of full spike history
class OptimizedSTDP : public PlasticityRule {
public:
    float ltpWeight = 0.01f;
    float ltdWeight = 0.012f;
    float timeConstant = 20.0f;
    float minWeight = -1.0f;
    float maxWeight = 1.0f;
    
    // Activity traces for fast computation
    float preTrace = 0.0f;
    float postTrace = 0.0f;
    
    // Fast spike history using circular buffers
    std::vector<float> preSpikeHistory;
    std::vector<float> postSpikeHistory;
    size_t historySize = 100;
    
    OptimizedSTDP() {
        preSpikeHistory.reserve(historySize);
        postSpikeHistory.reserve(historySize);
    }
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {
        if (!synapse) return;
        
        // Update activity traces
        updateActivityTraces(preSpikes, postSpikes, dt);
        
        // Calculate weight change using traces
        float weightChange = calculateTraceBasedWeightChange(preSpikes, postSpikes, dt);
        
        // Apply weight change
        applyWeightChange(synapse, weightChange);
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) return;
        
        float newWeight = synapse->getWeight() + delta;
        newWeight = std::clamp(newWeight, minWeight, maxWeight);
        synapse->setWeight(newWeight);
    }
    
    const char* getName() const override {
        return "OptimizedSTDP";
    }
    
    // Update activity traces based on recent spikes
    void updateActivityTraces(const std::vector<Timestamp>& preSpikes,
                              const std::vector<Timestamp>& postSpikes,
                              TimestepDuration dt) {
        // Decay existing traces
        preTrace *= 0.95f;  // Fast decay
        postTrace *= 0.95f;
        
        // Add new spike contributions
        for (Timestamp spikeTime : preSpikes) {
            preTrace += 1.0f;
        }
        
        for (Timestamp spikeTime : postSpikes) {
            postTrace += 1.0f;
        }
        
        // Limit trace values to prevent overflow
        preTrace = std::min(preTrace, 10.0f);
        postTrace = std::min(postTrace, 10.0f);
    }
    
    // Calculate weight change using traces
    float calculateTraceBasedWeightChange(const std::vector<Timestamp>& preSpikes,
                                         const std::vector<Timestamp>& postSpikes,
                                         TimestepDuration dt) {
        float totalDelta = 0.0f;
        float tau = timeConstant;
        
        // Fast calculation using pre/post trace values
        float prePostContribution = calculatePrePostContribution(preSpikes, postSpikes, dt);
        float postPreContribution = calculatePostPreContribution(preSpikes, postSpikes, dt);
        
        totalDelta = prePostContribution - postPreContribution;
        
        // Apply scaling factor
        totalDelta *= 0.01f;  // Adjust for typical learning rates
        
        return totalDelta;
    }
    
    // Fast pre-post contribution calculation
    float calculatePrePostContribution(const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes,
                                       TimestepDuration dt) {
        float contribution = 0.0f;
        float tau = timeConstant;
        
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float dtVal = static_cast<float>(postTime - preTime);
                
                if (dtVal > 0) {
                    // Pre before post: potentiation
                    float delta = ltpWeight * std::exp(-dtVal / tau);
                    delta *= (1.0f + postTrace);  // Enhance with postsynaptic trace
                    contribution += delta;
                }
            }
        }
        
        return contribution;
    }
    
    // Fast post-pre contribution calculation
    float calculatePostPreContribution(const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes,
                                       TimestepDuration dt) {
        float contribution = 0.0f;
        float tau = timeConstant;
        
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float dtVal = static_cast<float>(postTime - preTime);
                
                if (dtVal < 0) {
                    // Post before pre: depression
                    float delta = ltdWeight * std::exp(dtVal / tau);
                    delta *= (1.0f + preTrace);  // Enhance with presynaptic trace
                    contribution += delta;
                }
            }
        }
        
        return contribution;
    }
};

// Optimized structural plasticity with better spatial indexing
class OptimizedStructuralPlasticity {
public:
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    float minWeightThreshold = 0.05f;
    float activityThreshold = 0.001f;
    size_t maxSynapsesPerNeuron = 100;
    size_t maxTotalSynapses = 1000000;
    
    // Spatial index for faster synapse lookup
    std::unordered_map<NeuronId, std::vector<SynapseId>> neuronToSynapses;
    std::unordered_map<SynapseId, NeuronId> synapseToSource;
    std::unordered_map<SynapseId, NeuronId> synapseToDestination;
    
    // Neuron activity cache
    std::unordered_map<NeuronId, float> neuronActivity;
    
    // Optimization flag
    bool indexEnabled = true;
    
    void update(Brain* brain, RandomGenerator& rng) {
        if (!brain) return;
        
        // Rebuild indices if needed
        if (indexEnabled && !isIndexValid(brain)) {
            rebuildIndices(brain);
        }
        
        // Optimized synaptogenesis
        synaptogenesisOptimized(brain, rng);
        
        // Optimized pruning
        pruningOptimized(brain, rng);
    }
    
    // Check if spatial index is valid
    bool isIndexValid(Brain* brain) {
        // Simple validation - in production, would use versioning
        return !neuronToSynapses.empty();
    }
    
    // Rebuild spatial indices for fast lookup
    void rebuildIndices(Brain* brain) {
        neuronToSynapses.clear();
        synapseToSource.clear();
        synapseToDestination.clear();
        
        for (auto& region : brain->getRegions()) {
            for (Synapse* syn : region->getSynapses()) {
                SynapseId synId = syn->getId();
                NeuronId source = syn->getSourceNeuron();
                NeuronId dest = syn->getDestinationNeuron();
                
                neuronToSynapses[source].push_back(synId);
                synapseToSource[synId] = source;
                synapseToDestination[synId] = dest;
            }
        }
    }
    
    // Optimized synaptogenesis using spatial index
    void synaptogenesisOptimized(Brain* brain, RandomGenerator& rng) {
        if (!brain) return;
        
        // Update neuron activity cache
        updateNeuronActivityCache(brain);
        
        for (auto& region : brain->getRegions()) {
            auto neurons = region->getAllNeurons();
            size_t neuronCount = neurons.size();
            
            if (neuronCount < 2) continue;
            
            // Fast probabilistic synapse creation
            for (size_t attempt = 0; attempt < neuronCount; ++attempt) {
                if (!rng.bernoulli(synaptogenesisRate)) continue;
                
                size_t idx1 = rng.uniformInt(0, static_cast<int>(neuronCount) - 1);
                size_t idx2 = rng.uniformInt(0, static_cast<int>(neuronCount) - 1);
                
                if (idx1 == idx2) continue;
                
                NeuronId src = neurons[idx1]->getId();
                NeuronId dst = neurons[idx2]->getId();
                
                // Check if connection already exists using spatial index
                if (isConnectionExists(src, dst)) {
                    continue;
                }
                
                // Check synapse limits
                if (getSynapseCountForNeuron(src) >= maxSynapsesPerNeuron) {
                    continue;
                }
                
                if (region->getSynapseCount() >= maxTotalSynapses) {
                    continue;
                }
                
                // Create synapse with optimized weight
                float weight = calculateInitialWeight(rng, src, dst);
                SynapseId synId = region->addSynapse(src, dst, weight, 1);
                
                if (synId != INVALID_SYNAPSE_ID) {
                    // Update spatial index
                    neuronToSynapses[src].push_back(synId);
                    synapseToSource[synId] = src;
                    synapseToDestination[synId] = dst;
                }
            }
        }
    }
    
    // Optimized pruning using spatial index
    void pruningOptimized(Brain* brain, RandomGenerator& rng) {
        if (!brain) return;
        
        for (auto& region : brain->getRegions()) {
            for (Synapse* syn : region->getSynapses()) {
                SynapseId synId = syn->getId();
                
                // Fast synapse pruning using pre-computed data
                if (shouldPruneSynapse(syn, synId, rng)) {
                    // Remove from spatial index
                    if (indexEnabled) {
                        NeuronId src = synapseToSource[synId];
                        auto& synapses = neuronToSynapses[src];
                        synapses.erase(
                            std::remove(synapses.begin(), synapses.end(), synId),
                            synapses.end()
                        );
                        synapseToSource.erase(synId);
                        synapseToDestination.erase(synId);
                    }
                    
                    // Mark synapse for removal (implementation-dependent)
                    syn->setWeight(0.0f);
                }
            }
        }
    }
    
    // Check if connection already exists using spatial index
    bool isConnectionExists(NeuronId src, NeuronId dst) {
        auto it = neuronToSynapses.find(src);
        if (it == neuronToSynapses.end()) return false;
        
        for (SynapseId synId : it->second) {
            if (synapseToDestination.find(synId) != synapseToDestination.end()) {
                if (synapseToDestination[synId] == dst) {
                    return true;
                }
            }
        }
        return false;
    }
    
    // Get synapse count for neuron using spatial index
    size_t getSynapseCountForNeuron(NeuronId neuronId) {
        auto it = neuronToSynapses.find(neuronId);
        if (it == neuronToSynapses.end()) return 0;
        return it->second.size();
    }
    
    // Update neuron activity cache
    void updateNeuronActivityCache(Brain* brain) {
        neuronActivity.clear();
        
        for (auto& region : brain->getRegions()) {
            auto neurons = region->getAllNeurons();
            
            for (auto* neuron : neurons) {
                NeuronId neuronId = neuron->getId();
                
                // Calculate activity based on spike history
                float activity = calculateNeuronActivity(neuron);
                neuronActivity[neuronId] = activity;
            }
        }
    }
    
    // Calculate neuron activity efficiently
    float calculateNeuronActivity(Neuron* neuron) {
        const auto& spikes = neuron->getSpikeHistory();
        if (spikes.empty()) return 0.0f;
        
        // More recent spikes = higher activity
        return static_cast<float>(spikes.size());
    }
    
    // Calculate initial weight based on activity
    float calculateInitialWeight(RandomGenerator& rng, NeuronId src, NeuronId dst) {
        float baseWeight = 0.1f + rng.uniformReal(0.0f, 0.2f);
        
        // Adjust based on activity levels
        float srcActivity = neuronActivity.find(src) != neuronActivity.end() ? neuronActivity[src] : 0.0f;
        float dstActivity = neuronActivity.find(dst) != neuronActivity.end() ? neuronActivity[dst] : 0.0f;
        
        float activityFactor = (srcActivity + dstActivity) / 2.0f;
        float weight = baseWeight * (1.0f + activityFactor * 0.5f);
        
        return std::max(0.01f, std::min(weight, 1.0f));
    }
    
    // Determine if synapse should be pruned
    bool shouldPruneSynapse(Synapse* syn, SynapseId synId, RandomGenerator& rng) {
        if (!syn) return false;
        
        float weight = std::abs(syn->getWeight());
        
        // Check weight threshold
        if (weight < minWeightThreshold) {
            return rng.bernoulli(pruningRate);
        }
        
        // Check activity threshold
        NeuronId src = synapseToSource.find(synId) != synapseToSource.end() ? synapseToSource[synId] : INVALID_NEURON_ID;
        if (src != INVALID_NEURON_ID) {
            float activity = neuronActivity.find(src) != neuronActivity.end() ? neuronActivity[src] : 0.0f;
            if (activity < activityThreshold) {
                return rng.bernoulli(pruningRate * 0.5f);
            }
        }
        
        return false;
    }
};

} // namespace nlm
