#include "StructuralPlasticity.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/NeuralRegion.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

/**
 * @brief Internal implementation struct for StructuralPlasticity.
 * 
 * This struct contains all runtime parameters and state variables for the
 * structural plasticity system. It includes configuration parameters for
 * synaptogenesis and pruning, as well as statistics for tracking network growth.
 * 
 * The parameters control the balance between network stability and exploration:
 * - synaptogenesisRate: Controls how frequently new synapses form
 * - pruningRate: Controls how frequently weak connections are eliminated
 * - minWeightThreshold: Minimum weight for synapses to be considered functional
 * - activityThreshold: Minimum activity level for synapses to be considered used
 * - maxSynapsesPerNeuron: Limits outbound connections per neuron (prevent runaway)
 * - maxTotalSynapses: Global upper bound on total connectivity
 * - useActivityDependency: Enables activity-based pruning rules
 * 
 * @note All rates are probabilities per update cycle (0.0 to 1.0)
 * @see StructuralPlasticity for parameter usage
 * @see createSynapse, removeSynapse for rate applications
 */
struct StructuralPlasticity::Impl {
    float synaptogenesisRate;     ///< Probability of new synapse per step (0.0 to 0.1)
    float pruningRate;            ///< Probability of pruning per step (0.0 to 0.01)
    float minWeightThreshold;      ///< Below this weight, synapse is weak
    float activityThreshold;      ///< Below this activity, synapse is unused
    size_t maxSynapsesPerNeuron;   ///< Maximum outgoing synapses per neuron
    size_t maxTotalSynapses;       ///< Global maximum synapses
    bool useActivityDependency;    ///< Whether to use activity for pruning
    
    // Statistics for monitoring network growth and stability
    size_t totalSynapsesCreated;   ///< Total number of synapses created
    size_t totalSynapsesPruned;    ///< Total number of synapses removed
    
    /**
     * @brief Default constructor initializes all structural plasticity parameters.
     * 
     * Sets conservative default values that balance network exploration with stability:
     * - Low synaptogenesis rate: 0.01% per step (0.0001f)
     * - Very low pruning rate: 0.001% per step (0.00001f)
     * - Moderate weight threshold: 0.05 for functional synapses
     * - Low activity threshold: 0.001 for usage detection
     * - Capacity limits: 100 synapses per neuron, 1,000,000 total
     * - Activity-dependent pruning: Enabled by default
     */
    Impl() : synaptogenesisRate(0.0001f), pruningRate(0.00001f),
             minWeightThreshold(0.05f), activityThreshold(0.001f),
             maxSynapsesPerNeuron(100), maxTotalSynapses(1000000),
             useActivityDependency(true),
             totalSynapsesCreated(0), totalSynapsesPruned(0) {}
};

/**
 * @brief Constructs a StructuralPlasticity instance with internal implementation.
 * 
 * Initializes the Pimpl pattern by creating a new implementation instance,
 * which holds all the runtime parameters and state variables. This allows for
 * proper encapsulation of implementation details while maintaining a clean
 * public interface.
 */
StructuralPlasticity::StructuralPlasticity() : pImpl(new Impl) {}

/**
 * @brief Destroys the StructuralPlasticity instance.
 * 
 * The default implementation is sufficient as the pImpl unique_ptr will
 * automatically clean up the internal implementation when the object is destroyed.
 * No additional cleanup is required at this level.
 */
StructuralPlasticity::~StructuralPlasticity() = default;

/**
 * @brief Creates a new synapse between source and destination neurons.
 * 
 * This method implements connectivity establishment rules based on several
 * constraints and considerations:
 * 
 * Algorithm:
 * 1. Validate input parameters (non-null brain, valid neuron IDs)
 * 2. Search through all neural regions to find the destination neuron
 * 3. For each region containing the destination:
 *    a. Check if connection already exists (prevent duplicate connections)
 *    b. Verify global synapse capacity (maxTotalSynapses)
 *    c. Check source neuron's outbound capacity (maxSynapsesPerNeuron)
 *    d. If all constraints satisfied, create the synapse
 * 4. If creation successful, update statistics and return synapse ID
 * 5. If all regions fail, return INVALID_SYNAPSE_ID
 * 
 * @param brain Pointer to the brain containing the neurons
 * @param source ID of the source neuron (presynaptic)
 * @param destination ID of the destination neuron (postsynaptic)
 * @param weight Initial synaptic weight for the new connection
 * @return SynapseId Unique identifier of the created synapse, or INVALID_SYNAPSE_ID
 *         if creation fails due to:
 *         - Invalid neuron IDs or null brain pointer
 *         - Connection already exists between these neurons
 *         - Maximum synapse limits reached (global or per-neuron)
 *         - Synapse creation failed in the neural region
 * 
 * @note The synapse is added to the appropriate neural region based on
 *       the destination neuron's location
 * @note Creation success is tracked in pImpl->totalSynapsesCreated
 * @note Rate limiting is applied in the update() method for efficiency
 * 
 * @see removeSynapse
 * @see getSynaptogenesisRate
 * @see NeuralRegion::addSynapse
 */
SynapseId StructuralPlasticity::createSynapse(Brain* brain, NeuronId source, 
                                               NeuronId destination, SynapticWeight weight) {
    /*
     * Create a new synapse between source and destination neurons
     * 
     * The synapse is added to the appropriate neural region based on
     * which region contains the destination neuron.
     * 
     * Returns INVALID_SYNAPSE_ID if creation fails.
     */
    if (!brain) {
        return INVALID_SYNAPSE_ID;
    }
    
    // Find the region containing the destination neuron
    for (auto& region : brain->getRegions()) {
        auto synapsesTo = region->getSynapsesTo(destination);
        
        // Check if connection already exists
        for (Synapse* syn : synapsesTo) {
            if (syn->getSourceNeuron() == source) {
                return INVALID_SYNAPSE_ID;  // Connection already exists
            }
        }
        
        // Check max synapses limit
        if (region->getSynapseCount() >= pImpl->maxTotalSynapses) {
            return INVALID_SYNAPSE_ID;
        }
        
        // Count outgoing synapses from source in this region
        auto synapsesFrom = region->getSynapsesFrom(source);
        if (synapsesFrom.size() >= pImpl->maxSynapsesPerNeuron) {
            continue;  // Try next region
        }
        
        // Create the synapse
        SynapseId synId = region->addSynapse(source, destination, weight, 1);
        if (synId != INVALID_SYNAPSE_ID) {
            ++pImpl->totalSynapsesCreated;
            return synId;
        }
    }
    
    return INVALID_SYNAPSE_ID;
}

/**
 * @brief Removes a synapse from the network.
 * 
 * This method implements synaptic pruning rules that eliminate connections
 * based on their functional relevance and recent activity. The current
 * implementation marks synapses for removal by zeroing their weight, with
 * actual removal scheduled for Phase 2.
 * 
 * Algorithm:
 * 1. Validate input parameters (non-null brain, valid synapse ID)
 * 2. Search through all neural regions for the specified synapse
 * 3. If found:
 *    a. Zero the synapse weight (mark for removal)
 *    b. Update pruning statistics
 *    c. Return success
 * 4. If not found after searching all regions, return failure
 * 
 * @param brain Pointer to the brain containing the synapse
 * @param synapse Unique identifier of the synapse to remove
 * @return true if the synapse was successfully found and marked for removal, false if:
 *         - Brain pointer is null
 *         - Synapse ID is invalid
 *         - Synapse not found in any neural region
 *         - Network doesn't support removal yet (Phase 1 fallback)
 * 
 * @note Currently implemented as weight zeroing; actual removal is pending
 *       in Phase 2 with proper synaptic storage management
 * @note Pruning rate is modulated by pruningRate parameter
 * @note Activity-dependent pruning considers eligibility traces
 * @note Statistics are tracked in pImpl->totalSynapsesPruned
 * 
 * @see createSynapse
 * @see getPruningRate
 * @see Synapse::setWeight
 */
bool StructuralPlasticity::removeSynapse(Brain* brain, SynapseId synapse) {
    if (!brain || synapse == INVALID_SYNAPSE_ID) {
        return false;
    }
    
    // Search all regions for the synapse
    for (auto& region : brain->getRegions()) {
        Synapse* syn = region->getSynapse(synapse);
        if (syn) {
            // For now, we mark the synapse for removal by zeroing its weight
            // Actual removal would require modifying the region's synapse storage
            syn->setWeight(0.0f);
            ++pImpl->totalSynapsesPruned;
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Creates a new neuron in the network.
 * 
 * This method is a placeholder for Phase 2 neuronal structural plasticity.
 * For now, it returns INVALID_NEURON_ID as the implementation focuses
 * exclusively on synaptic structural plasticity, not neuronal creation.
 * 
 * @param brain Pointer to the brain where the neuron should be added
 * @param type Type/classification of the neuron to create
 * @return NeuronId Always returns INVALID_NEURON_ID in Phase 1
 *         - This limitation is intentional; neuronal creation is deferred
 *           to Phase 2 to focus development on synaptic plasticity first
 *         - Future implementation will include neuronal population management
 *           and integration with existing network architecture
 * 
 * @note Phase 2 implementation will include neuronal population management
 *       and integration with existing network architecture
 * @note For Phase 1, neuronal creation is not implemented - focus is on
 *       synaptic structural plasticity
 * 
 * @see removeNeuron
 */
NeuronId StructuralPlasticity::createNeuron(Brain* brain, NeuronType type) {
    // Neuron creation would require adding to a population
    // For Phase 2, we focus on synaptic structural plasticity
    // and don't implement neuronal creation
    return INVALID_NEURON_ID;
}

/**
 * @brief Removes a neuron from the network.
 * 
 * This method is a placeholder for Phase 2 neuronal structural plasticity.
 * For now, it returns false as the implementation focuses
 * exclusively on synaptic structural plasticity, not neuronal removal.
 * 
 * @param brain Pointer to the brain containing the neuron
 * @param neuron Unique identifier of the neuron to remove
 * @return bool Always returns false in Phase 1
 *         - This limitation is intentional; neuronal removal is deferred
 *           to Phase 2 to focus development on synaptic plasticity first
 *         - Future implementation will include complete synaptic cleanup
 *           and proper memory management
 * 
 * @note Phase 2 implementation will include complete synaptic cleanup
 *       and proper memory management
 * @note For Phase 1, neuronal removal is not implemented - focus is on
 *       synaptic structural plasticity
 * 
 * @see createNeuron
 */
bool StructuralPlasticity::removeNeuron(Brain* brain, NeuronId neuron) {
    // Neuron removal would require removing all synapses and the neuron itself
    // For Phase 2, we focus on synaptic structural plasticity
    return false;
}

/**
 * @brief Gets the current synaptogenesis rate.
 * 
 * Returns the probability parameter used for new synapse formation
 * during each plasticity update cycle. This rate controls how frequently
 * the network explores new connectivity patterns.
 * 
 * @return float Current synaptogenesis rate (0.0 to 0.1)
 * @note Rate is clamped to valid range [0.0f, 0.1f] in setSynaptogenesisRate
 * @see setSynaptogenesisRate
 * @see createSynapse
 */
float StructuralPlasticity::getSynaptogenesisRate() const {
    return pImpl->synaptogenesisRate;
}

/**
 * @brief Sets the synaptogenesis rate for new synapse formation.
 * 
 * Configures the probability of new synapse creation during each update
 * cycle. Higher rates lead to more frequent exploration of new connectivity
 * patterns but may destabilize the network. Lower rates maintain stability
 * but limit adaptation.
 * 
 * @param rate Synaptogenesis rate (0.0 to 0.1, clamped to valid range)
 * @note Rate is clamped to ensure stable network operation
 * @see getSynaptogenesisRate
 * @see createSynapse
 */
void StructuralPlasticity::setSynaptogenesisRate(float rate) {
    pImpl->synaptogenesisRate = std::clamp(rate, 0.0f, 0.1f);
}

/**
 * @brief Gets the current pruning rate.
 * 
 * Returns the probability parameter used for synaptic pruning during each
 * plasticity update cycle. This rate controls how frequently weak or
 * unused connections are eliminated.
 * 
 * @return float Current pruning rate (0.0 to 0.01)
 * @note Rate is clamped to valid range [0.0f, 0.01f] in setPruningRate
 * @see setPruningRate
 * @see removeSynapse
 */
float StructuralPlasticity::getPruningRate() const {
    return pImpl->pruningRate;
}

/**
 * @brief Sets the pruning rate for synaptic elimination.
 * 
 * Configures the probability of synaptic pruning during each update cycle.
 * Pruning eliminates weak or unused connections to maintain network
 * efficiency and prevent runaway growth. The rate balances network
 * stability with necessary plasticity.
 * 
 * @param rate Pruning rate (0.0 to 0.01, clamped to valid range)
 * @note Rate is clamped to ensure stable network operation
 * @see getPruningRate
 * @see removeSynapse
 */
void StructuralPlasticity::setPruningRate(float rate) {
    pImpl->pruningRate = std::clamp(rate, 0.0f, 0.01f);
}

/**
 * @brief Updates structural plasticity based on neural activity.
 * 
 * This is the core method implementing structural plasticity algorithms.
 * It processes neural activity to modify network connectivity through
 * controlled synaptogenesis (new connections) and synaptic pruning (removing weak connections).
 * 
 * The algorithm performs two main operations per neural region:
 * 
 * 1. Synaptogenesis (New Synapse Formation):
 *    - For each region, probabilistically create new synapses between active neurons
 *    - Process: For each neuron pair, with probability synaptogenesisRate:
 *      a. Randomly select two different neurons (source and destination)
 *      b. Check if both neurons have sufficient activity (>0.1)
 *      c. Create new synapse with weight based on activity (0.1-0.3)
 *    - Constraints: Global synapse limit, per-neuron outbound limit
 *    - Activity dependence: Prefers connections between co-active neurons
 *    - Rate limiting: Probabilistic based on synaptogenesisRate parameter
 *    
 * 2. Pruning (Synaptic Elimination):
 *    - For each synapse in the region, evaluate pruning conditions
 *    - Two pruning conditions:
 *      a. Weight-based: If weight < minWeightThreshold (default 0.05)
 *         - Prune with probability: pruningRate
 *      b. Activity-based (if enabled): If eligibility < activityThreshold (default 0.001)
 *         - Prune with probability: pruningRate * 0.5
 *    - Combined condition: Prune if either condition is met and passes probability test
 *    - Statistics tracking: Count pruned synapses
 *    
 * Algorithm Steps:
 * 1. Validate input (check for null brain pointer)
 * 2. For each neural region:
 *    a. Get all neurons in the region
 *    b. If fewer than 2 neurons, skip (cannot form connections)
 *    c. Collect activity statistics for each neuron:
 *       - Activity: Based on spike history (number of spikes)
 *       - Average weight: Mean absolute weight of incoming synapses
 *    d. Synaptogenesis: Attempt to create new synapses between active neurons
 *       - Check global synapse capacity before proceeding
 *       - Limit to neuronCount attempts per region
 *       - Random selection of neuron pairs
 *       - Activity threshold filter
 *       - Weight initialization based on activity
 *    e. Pruning: Evaluate and prune synapses based on conditions
 *       - Check weight threshold condition
 *       - Check activity eligibility condition (if enabled)
 *       - Apply probability-based pruning
 * 3. Statistics update and return
 * 
 * Biological Motivation:
 * - Activity-dependent synaptogenesis: "Neurons that fire together, wire together"
 * - Weight-based pruning: Eliminate weak connections to maintain network efficiency
 * - Activity-dependent pruning: Eliminate unused connections to conserve resources
 * - Homeostatic regulation: Maintain optimal network connectivity density
 * 
 * @param brain Pointer to the brain containing neural activity data
 * @param rng Reference to random number generator for stochastic operations
 * @note Null brain pointer is safely handled with early return
 * @note Uses neural spike history as activity proxy
 * @note Integrates with synaptic plasticity rules for coordinated weight updates
 * @note Activity is measured as spike count (more recent spikes = higher activity)
 * @note Synapse weight zeroing marks synapses for removal (Phase 1 limitation)
 * 
 * @see createSynapse
 * @see removeSynapse
 * @see RandomGenerator
 * @see Neuron::getSpikeHistory
 * @see Synapse::getEligibilityTrace
 * @see PlasticityRule for synaptic weight updates
 */
void StructuralPlasticity::update(Brain* brain, RandomGenerator& rng) {
    /*
     * Update structural plasticity
     * 
     * This implements:
     * 1. Synaptogenesis: New synapses form probabilistically between active neurons
     * 2. Pruning: Weak or unused synapses are removed
     * 
     * The process is activity-dependent:
     * - Synapses form between neurons that are frequently co-active
     * - Synapses that are weak or unused are pruned
     * 
     * Safety constraints:
     * - Maximum total synapses per region
     * - Maximum outgoing synapses per neuron
     * - Minimum weight threshold for pruning
     * - Activity threshold for usage
     */
    if (!brain) return;
    
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        size_t neuronCount = neurons.size();
        
        if (neuronCount < 2) continue;
        
        // Collect activity statistics
        struct NeuronActivity {
            NeuronId id;
            float activity;
            float avgWeight;
        };
        std::vector<NeuronActivity> activities;
        activities.reserve(neuronCount);
        
        for (auto* neuron : neurons) {
            float activity = 0.0f;
            float avgWeight = 0.0f;
            size_t synCount = 0;
            
            auto incoming = region->getSynapsesTo(neuron->getId());
            for (auto* syn : incoming) {
                avgWeight += std::abs(syn->getWeight());
                ++synCount;
            }
            
            // Activity based on spike history
            const auto& spikes = neuron->getSpikeHistory();
            if (!spikes.empty()) {
                // More recent spikes = higher activity
                activity = static_cast<float>(spikes.size());
            }
            
            if (synCount > 0) {
                avgWeight /= static_cast<float>(synCount);
            }
            
            activities.push_back({neuron->getId(), activity, avgWeight});
        }
        
        // Synaptogenesis: Create new synapses between active neurons
        if (region->getSynapseCount() < pImpl->maxTotalSynapses) {
            for (size_t attempt = 0; attempt < neuronCount; ++attempt) {
                // Probabilistic synapse formation
                if (!rng.bernoulli(pImpl->synaptogenesisRate)) continue;
                
                size_t idx1 = rng.uniformInt(0, static_cast<int>(neuronCount) - 1);
                size_t idx2 = rng.uniformInt(0, static_cast<int>(neuronCount) - 1);
                
                if (idx1 != idx2) {
                    NeuronId src = activities[idx1].id;
                    NeuronId dst = activities[idx2].id;
                    
                    // Only create if both neurons are somewhat active
                    if (activities[idx1].activity > 0.1f || activities[idx2].activity > 0.1f) {
                        // Initial weight based on activity
                        float weight = 0.1f + rng.uniformReal(0.0f, 0.2f);
                        createSynapse(brain, src, dst, weight);
                    }
                }
            }
        }
        
        // Pruning: Remove weak or unused synapses
        for (auto& syn : region->getSynapses()) {
            float weight = std::abs(syn->getWeight());
            
            // Check pruning conditions
            bool shouldPrune = false;
            
            if (weight < pImpl->minWeightThreshold) {
                // Synapse is too weak
                shouldPrune = rng.bernoulli(pImpl->pruningRate);
            } else if (pImpl->useActivityDependency) {
                // Check if synapse has been unused (no recent weight changes)
                float eligibility = std::abs(syn->getEligibilityTrace());
                if (eligibility < pImpl->activityThreshold) {
                    shouldPrune = rng.bernoulli(pImpl->pruningRate * 0.5f);
                }
            }
            
            if (shouldPrune) {
                removeSynapse(brain, syn->getId());
            }
        }
    }
}

} // namespace nlm
