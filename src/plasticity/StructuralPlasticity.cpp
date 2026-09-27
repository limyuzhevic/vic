// Modern C++ Implementation of StructuralPlasticity.cpp
// This file implements structural plasticity rules for creating new synapses
// and modifying neural connectivity, using modern C++ features and design patterns

#include "StructuralPlasticity.hpp"
#include <algorithm>
#include <numeric>
#include <execution>
#include <cmath>
#include <random>

namespace nlm {

// Structural Plasticity Implementation Details
StructuralPlasticity::StructuralPlasticity() : pImpl(std::make_unique<Impl>()) {
    // Initialize default structural plasticity parameters
    pImpl->connectionProbability = 0.1f;    // Probability of forming new connections
    pImpl->maxConnections = 100;           // Maximum number of connections per neuron
    pImpl->growthRate = 0.01f;             // Rate of structural change
    pImpl->stabilityThreshold = 0.5f;      // Activity threshold for stability
    pImpl->pruningThreshold = 0.1f;        // Activity threshold for pruning
    pImpl->maxBranchingFactor = 4;         // Maximum branching factor for new connections
}

StructuralPlasticity::~StructuralPlasticity() = default;

// Create new synaptic connections based on structural plasticity rules
void StructuralPlasticity::createConnections(
    Neuron& sourceNeuron,
    const std::vector<Neuron*>& targetNeurons,
    const std::vector<Timestamp>& preSpikeHistory,
    Timestamp currentTime) {
    
    if (targetNeurons.empty() || preSpikeHistory.empty()) {
        return;
    }
    
    // Modern C++: Use random device and thread-safe generators
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    // Calculate connection eligibility based on pre-synaptic activity
    float activityScore = calculateActivityScore(preSpikeHistory);
    
    // Determine number of potential connections based on activity
    size_t maxNewConnections = static_cast<size_t>(
        pImpl->maxConnections * activityScore * pImpl->growthRate);
    
    // Create new connections with bounded probability
    std::vector<Neuron*> eligibleTargets;
    eligibleTargets.reserve(targetNeurons.size());
    
    for (Neuron* target : targetNeurons) {
        if (isEligibleForConnection(*target, currentTime)) {
            eligibleTargets.push_back(target);
        }
    }
    
    // Limit to maximum number of connections to maintain stability
    size_t numConnections = std::min(maxNewConnections, 
                                   static_cast<size_t>(pImpl->maxBranchingFactor));
    
    // Randomly select targets for new connections
    std::shuffle(eligibleTargets.begin(), eligibleTargets.end(), rng);
    
    for (size_t i = 0; i < numConnections && i < eligibleTargets.size(); ++i) {
        // Check probability of connection formation
        if (dist(rng) < pImpl->connectionProbability * activityScore) {
            // Create synapse using the synapse factory
            auto synapseResult = createSynapseForConnection(
                sourceNeuron, *eligibleTargets[i], currentTime);
            
            if (synapseResult) {
                // Add synapse to both neurons
                sourceNeuron.addOutgoingSynapse(synapseResult->getId());
                eligibleTargets[i]->addIncomingSynapse(synapseResult->getId());
                
                // Record structural change for learning
                recordStructuralChange(
                    sourceNeuron.getId(),
                    eligibleTargets[i]->getId(),
                    synapseResult->getId(),
                    StructuralChangeType::CONNECTION_CREATED,
                    currentTime
                );
            }
        }
    }
}

// Remove existing connections based on inactivity or stability criteria
void StructuralPlasticity::removeConnections(
    Neuron& neuron,
    const std::vector<Synapse*>& existingSynapses,
    const std::vector<Timestamp>& spikeHistory,
    Timestamp currentTime) {
    
    if (spikeHistory.empty()) {
        return;
    }
    
    // Calculate stability score based on spike history
    float stabilityScore = calculateStabilityScore(spikeHistory);
    
    // Modern C++: Use parallel algorithms for efficient processing
    auto preIt = std::execution::par_unseq;
    
    std::vector<Synapse*> synapsesToRemove;
    
    for (Synapse* synapse : existingSynapses) {
        if (shouldRemoveConnection(*synapse, stabilityScore, currentTime)) {
            synapsesToRemove.push_back(synapse);
        }
    }
    
    // Remove identified synapses
    for (Synapse* synapse : synapsesToRemove) {
        // Remove from neurons (in a real implementation, would need proper neuron access)
        recordStructuralChange(
            synapse->getSourceNeuron(),
            synapse->getDestinationNeuron(),
            synapse->getId(),
            StructuralChangeType::CONNECTION_REMOVED,
            currentTime
        );
    }
}

// Calculate activity score based on pre-synaptic spike history
float StructuralPlasticity::calculateActivityScore(
    const std::vector<Timestamp>& spikeHistory) const {
    
    if (spikeHistory.empty()) {
        return 0.0f;
    }
    
    // Modern C++: Use efficient algorithms for activity calculation
    float totalActivity = std::accumulate(spikeHistory.begin(), spikeHistory.end(), 0.0f);
    
    // Normalize activity by number of spikes and time window
    float meanActivity = totalActivity / spikeHistory.size();
    
    // Apply stability threshold
    return std::min(1.0f, meanActivity / pImpl->stabilityThreshold);
}

// Calculate stability score based on spike history variability
float StructuralPlasticity::calculateStabilityScore(
    const std::vector<Timestamp>& spikeHistory) const {
    
    if (spikeHistory.size() < 2) {
        return 0.0f;
    }
    
    // Calculate coefficient of variation for spike timing
    float mean = std::accumulate(spikeHistory.begin(), spikeHistory.end(), 0.0f) / spikeHistory.size();
    
    float variance = 0.0f;
    for (Timestamp t : spikeHistory) {
        float diff = t - mean;
        variance += diff * diff;
    }
    variance /= spikeHistory.size();
    
    float stdDev = std::sqrt(variance);
    
    // Stability is inversely related to coefficient of variation
    float coefficientOfVariation = stdDev > 0.0f ? stdDev / mean : 0.0f;
    
    return std::max(0.0f, 1.0f - coefficientOfVariation);
}

// Check if a neuron is eligible for new connections
bool StructuralPlasticity::isEligibleForConnection(
    const Neuron& neuron,
    Timestamp currentTime) const {
    
    // Check if neuron has reached maximum connections
    size_t currentConnections = neuron.getOutgoingSynapses().size();
    if (currentConnections >= pImpl->maxConnections) {
        return false;
    }
    
    // Check stability threshold
    float stabilityScore = calculateStabilityScore(neuron.getSpikeHistory());
    if (stabilityScore < pImpl->stabilityThreshold) {
        return false;
    }
    
    // Check if connection window is valid (not in refractory period)
    if (currentTime < pImpl->lastConnectionTime + pImpl->connectionCooldown) {
        return false;
    }
    
    return true;
}

// Determine if a synapse should be removed
bool StructuralPlasticity::shouldRemoveConnection(
    const Synapse& synapse,
    float stabilityScore,
    Timestamp currentTime) const {
    
    // Modern C++: Use modern algorithms for decision making
    
    // Check if synapse has been stable for sufficient time
    if (currentTime < pImpl->lastChangeTime + pImpl->minSynapseLifetime) {
        return false;
    }
    
    // Remove if stability is below threshold
    if (stabilityScore < pImpl->pruningThreshold) {
        return true;
    }
    
    // Remove if synapse weight is too small (synapse depression)
    float weight = synapse.getWeight();
    if (std::abs(weight) < pImpl->minWeightThreshold) {
        return true;
    }
    
    // Remove if pre-synaptic neuron is unstable
    // (In real implementation, would need access to neuron)
    
    return false;
}

// Create a new synapse for connection using factory pattern
std::expected<std::unique_ptr<Synapse>, PlasticityError> 
StructuralPlasticity::createSynapseForConnection(
    Neuron& source,
    Neuron& destination,
    Timestamp currentTime) {
    
    // Generate unique synapse ID
    static std::atomic<uint64_t> synapseCounter{0};
    SynapseId synapseId{synapseCounter++};
    
    // Create synapse with initial parameters
    auto synapse = std::make_unique<Synapse>(synapseId, source.getId(), destination.getId());
    
    // Initialize synapse based on neuron types and connection rules
    initializeNewSynapse(*synapse, source, destination, currentTime);
    
    return synapse;
}

// Initialize new synapse based on neuron characteristics
void StructuralPlasticity::initializeNewSynapse(
    Synapse& synapse,
    const Neuron& source,
    const Neuron& destination,
    Timestamp currentTime) {
    
    // Set initial weight based on neuron types
    NeuronType sourceType = source.getType();
    NeuronType destType = destination.getType();
    
    // Hebbian initialization based on neuron types
    if (sourceType == NeuronType::Excitatory && destType == NeuronType::Excitatory) {
        synapse.setWeight(0.5f);
    } else if (sourceType == NeuronType::Inhibitory && destType == NeuronType::Excitatory) {
        synapse.setWeight(-0.3f);
    } else if (sourceType == NeuronType::Modulatory) {
        synapse.setWeight(0.1f);
    } else {
        synapse.setWeight(0.05f * (1.0f - static_cast<float>(rand()) / RAND_MAX));
    }
    
    // Set initial delay based on distance heuristic
    float distance = calculateDistance(source.getId(), destination.getId());
    synapse.setDelay(static_cast<Delay>(distance * pImpl->delayScaleFactor));
    
    // Set initial type based on connection type
    if (sourceType == NeuronType::Inhibitory) {
        synapse.setType(SynapseType::Inhibitory);
    } else if (sourceType == NeuronType::Modulatory) {
        synapse.setType(SynapseType::Modulatory);
    } else {
        synapse.setType(SynapseType::Excitatory);
    }
    
    // Set plasticity flags
    synapse.enablePlasticity(true, true, false);
    
    // Initialize eligibility trace
    synapse.setEligibilityTrace(0.0f);
    
    // Record creation time for cooldown calculations
    pImpl->lastConnectionTime = currentTime;
}

// Calculate distance between neurons for delay estimation
float StructuralPlasticity::calculateDistance(
    NeuronId sourceId,
    NeuronId destId) const {
    
    // Simplified distance calculation using ID hash
    // In real implementation, would use spatial coordinates
    uint64_t sourceHash = std::hash<uint64_t>{}(sourceId.index());
    uint64_t destHash = std::hash<uint64_t>{}(destId.index());
    
    float distance = static_cast<float>(std::abs(static_cast<int64_t>(sourceHash - destHash)));
    return distance / static_cast<float>(UINT64_MAX);
}

// Record structural change for learning and analysis
void StructuralPlasticity::recordStructuralChange(
    NeuronId sourceId,
    NeuronId destId,
    SynapseId synapseId,
    StructuralChangeType changeType,
    Timestamp timestamp) {
    
    // Create structural change record
    StructuralChange change;
    change.sourceNeuron = sourceId;
    change.destinationNeuron = destId;
    change.synapseId = synapseId;
    change.changeType = changeType;
    change.timestamp = timestamp;
    
    // Add to history
    pImpl->structuralChanges.push_back(change);
    
    // Update last change time
    pImpl->lastChangeTime = timestamp;
    
    // Trim history if it grows too large (memory management)
    if (pImpl->structuralChanges.size() > pImpl->maxChangeHistory) {
        pImpl->structuralChanges.erase(
            pImpl->structuralChanges.begin());
    }
}

// Get structural change history for analysis
const std::vector<StructuralPlasticity::StructuralChange>& 
StructuralPlasticity::getStructuralChanges() const {
    return pImpl->structuralChanges;
}

// Set structural plasticity parameters with validation
void StructuralPlasticity::setParameter(const std::string& key, float value) {
    if (key == "connectionProbability") {
        pImpl->connectionProbability = std::clamp(value, 0.0f, 1.0f);
    } else if (key == "maxConnections") {
        pImpl->maxConnections = static_cast<size_t>(std::max(0.0f, value));
    } else if (key == "growthRate") {
        pImpl->growthRate = std::clamp(value, 0.0f, 1.0f);
    } else if (key == "stabilityThreshold") {
        pImpl->stabilityThreshold = std::clamp(value, 0.0f, 1.0f);
    } else if (key == "pruningThreshold") {
        pImpl->pruningThreshold = std::clamp(value, 0.0f, 1.0f);
    } else if (key == "maxBranchingFactor") {
        pImpl->maxBranchingFactor = static_cast<size_t>(std::max(1.0f, value));
    }
}

float StructuralPlasticity::getParameter(const std::string& key) const {
    if (key == "connectionProbability") return pImpl->connectionProbability;
    if (key == "maxConnections") return static_cast<float>(pImpl->maxConnections);
    if (key == "growthRate") return pImpl->growthRate;
    if (key == "stabilityThreshold") return pImpl->stabilityThreshold;
    if (key == "pruningThreshold") return pImpl->pruningThreshold;
    if (key == "maxBranchingFactor") return static_cast<float>(pImpl->maxBranchingFactor);
    return 0.0f;
}

// Reset structural plasticity state
void StructuralPlasticity::reset() {
    pImpl->structuralChanges.clear();
    pImpl->lastConnectionTime = 0.0;
    pImpl->lastChangeTime = 0.0;
    
    // Reset parameters to defaults
    pImpl->connectionProbability = 0.1f;
    pImpl->maxConnections = 100;
    pImpl->growthRate = 0.01f;
    pImpl->stabilityThreshold = 0.5f;
    pImpl->pruningThreshold = 0.1f;
    pImpl->maxBranchingFactor = 4;
}

// Get name of the structural plasticity rule
const char* StructuralPlasticity::getName() const {
    return "StructuralPlasticity";
}

} // namespace nlm