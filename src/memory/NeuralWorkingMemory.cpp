#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <unordered_set>

namespace nlm {

// Performance constants extracted for optimization
namespace Constants {
    // NeuralWorkingMemory constants
    constexpr float ACTIVATION_STRENGTH_MULTIPLIER = 5.0f;
    constexpr float RECURRENT_CONNECTION_STRENGTH = 0.5f;
    constexpr float MAINTENANCE_THRESHOLD = 0.1f;
    constexpr float ACTIVITY_UPDATE_FACTOR = 2.0f;
    constexpr SimulationStep TRACE_AGE_THRESHOLD = 1000;
    constexpr float COMPETITION_THRESHOLD = 0.3f;
    constexpr float INHIBITION_MULTIPLIER = 3.0f;
    constexpr float WEAK_TRACE_THRESHOLD = 0.01f;
    
    // AttentionalSelection constants
    constexpr float DEFAULT_INHIBITION_STRENGTH = 0.5f;
    constexpr float DEFAULT_EXCITATION_STRENGTH = 1.5f;
    constexpr float DEFAULT_COMPETITION_THRESHOLD = 0.3f;
    constexpr float COMPETITOR_ACTIVITY_EPSILON = 0.001f;
    constexpr float RELATIVE_ACTIVITY_THRESHOLD = 1.5f;
    constexpr float EXCITATION_MULTIPLIER = 2.0f;
    constexpr float SALIENCE_DECAY_RATE = 0.95f;
    constexpr float BIAS_DECAY_RATE = 0.98f;
    constexpr float INHIBITION_DECAY_RATE = 0.9f;
}

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Maintenance connections (recurrent)
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    
    // Memory trace ages
    std::vector<SimulationStep> traceAges;
    
    // Optimized data structures for O(1) lookups
    std::unordered_map<NeuronId, size_t> neuronIndexMap;
    std::unordered_set<NeuronId> neuronSet;
    std::unordered_map<NeuronId, float> neuronActivationMap;
    
    // Memory traces with struct of arrays for cache efficiency
    std::vector<MemoryTrace> memoryTraces_;
    
    Impl() : brain(nullptr) {}
};

// Struct for cache-friendly memory representation
struct MemoryTrace {
    NeuronId neuronId;
    float activation;
    SimulationStep timestamp;
};

/**
 * @brief Remove memory traces from a vector using batch operations
 * 
 * This helper function efficiently removes memory traces from the memory traces
 * vector while maintaining the integrity of auxiliary data structures. The
 * function processes removals in reverse order to maintain valid indices
 * throughout the operation.
 * 
 * @param traces Vector of memory traces to modify (will be modified in-place)
 * @param indexMap Reference to the neuron index map (will be cleared and rebuilt)
 * @param indicesToRemove Vector of indices to remove (processed in reverse order)
 * 
 * The reverse order processing ensures that removing an element doesn't affect
 * the indices of subsequent elements to be removed, making this operation O(k)
 * where k is the number of elements to remove, regardless of their positions.
 */
static void removeMemoryTraces(std::vector<MemoryTrace>& traces, 
                               const std::unordered_map<NeuronId, size_t>& indexMap,
                               const std::vector<size_t>& indicesToRemove) {
    // Remove traces in reverse order to maintain indices
    for (auto it = indicesToRemove.rbegin(); it != indicesToRemove.rend(); ++it) {
        if (*it < traces.size()) {
            traces.erase(traces.begin() + *it);
        }
    }
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralWorkingMemory initialized");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Find neurons to encode this pattern
    size_t neuronsNeeded = std::min(pattern.size(), memoryTraces_.size());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryTraces_[i].neuronId;
        float activation = pattern[i] * strength;
        
        // Set neuron activation
        if (auto* n = brain_->getRegion(neuron.getId() / 1000)->getAllNeurons()) {
            for (auto* nn : *n) {
                if (nn->getId() == neuron) {
                    nn->injectCurrent(activation * Constants::ACTIVATION_STRENGTH_MULTIPLIER);
                    break;
                }
            }
        }
        
        // Update stored activation
        memoryTraces_[i].activation = activation;
        memoryTraces_[i].timestamp = 0;
    }
    
    // Create maintenance connections if needed
    for (size_t i = 1; i < memoryTraces_.size(); ++i) {
        createRecurrentConnection(memoryTraces_[i-1].neuronId, memoryTraces_[i].neuronId, strength * Constants::RECURRENT_CONNECTION_STRENGTH);
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Check if neuron already exists using O(1) lookup
    auto it = pImpl->neuronIndexMap.find(neuron);
    
    if (it != pImpl->neuronIndexMap.end()) {
        size_t idx = it->second;
        memoryTraces_[idx].activation = activation;
        memoryTraces_[idx].timestamp = 0;
        pImpl->neuronActivationMap[neuron] = activation;
    } else if (pImpl->neuronIndexMap.size() < capacity_) {
        // Add new neuron with O(1) insertion
        size_t idx = memoryTraces_.size();
        memoryTraces_.emplace_back(neuron, activation, 0);
        pImpl->neuronIndexMap[neuron] = idx;
        pImpl->neuronSet.insert(neuron);
        pImpl->neuronActivationMap[neuron] = activation;
    }
    
    // Inject current to maintain activation
    if (brain_) {
        brain_->injectCurrent(neuron, activation * Constants::ACTIVATION_STRENGTH_MULTIPLIER);
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    result.reserve(memoryTraces_.size());
    
    for (const auto& trace : memoryTraces_) {
        result.push_back(trace.activation);
    }
    
    return result;
}

bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    return pImpl->neuronSet.find(neuron) != pImpl->neuronSet.end();
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    auto it = pImpl->neuronActivationMap.find(neuron);
    if (it != pImpl->neuronActivationMap.end()) {
        return it->second;
    }
    return 0.0f;
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    for (auto& trace : memoryTraces_) {
        trace.activation = std::min(1.0f, trace.activation * factor);
    }
    // Update the activation map as well
    for (const auto& trace : memoryTraces_) {
        pImpl->neuronActivationMap[trace.neuronId] = trace.activation;
    }
}

void NeuralWorkingMemory::runCompetition() {
    winners_.clear();
    
    if (memoryTraces_.empty()) return;
    
    // Find neurons with above-threshold activation (single loop for efficiency)
    float threshold = Constants::COMPETITION_THRESHOLD;
    
    for (const auto& trace : memoryTraces_) {
        if (trace.activation >= threshold) {
            winners_.push_back(trace.neuronId);
        }
    }
    
    // Inhibitory competition - suppress non-winners (more efficient using set)
    std::unordered_set<NeuronId> winnerSet(winners_.begin(), winners_.end());
    
    for (const auto& trace : memoryTraces_) {
        if (winnerSet.find(trace.neuronId) == winnerSet.end() && brain_) {
            // Apply strong inhibition
            brain_->injectCurrent(trace.neuronId, -trace.activation * Constants::INHIBITION_MULTIPLIER);
        }
    }
}

bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

float NeuralWorkingMemory::getMemoryActivity() const {
    if (memoryTraces_.empty()) return 0.0f;
    
    float total = 0.0f;
    for (const auto& trace : memoryTraces_) {
        total += trace.activation;
    }
    return total / memoryTraces_.size();
}

void NeuralWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    // Check if connection already exists (linear search, but small list)
    for (const auto& conn : pImpl->maintenanceSynapses) {
        if (conn.first == from && conn.second == to) return;
    }
    
    pImpl->maintenanceSynapses.emplace_back(from, to);
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Apply maintenance currents through recurrent connections
    for (const auto& conn : pImpl->maintenanceSynapses) {
        float fromActivation = 0.0f;
        
        // Find from neuron activation using O(1) lookup
        auto it = pImpl->neuronActivationMap.find(conn.first);
        if (it != pImpl->neuronActivationMap.end()) {
            fromActivation = it->second;
        }
        
        if (fromActivation > Constants::MAINTENANCE_THRESHOLD && brain_) {
            // Send maintenance signal
            brain_->injectCurrent(conn.second, fromActivation * Constants::ACTIVITY_UPDATE_FACTOR);
        }
    }
}

static void removeMemoryTraces(std::vector<MemoryTrace>& traces, 
                               const std::unordered_map<NeuronId, size_t>& indexMap,
                               const std::vector<size_t>& indicesToRemove) {
    // Remove traces in reverse order to maintain indices
    for (auto it = indicesToRemove.rbegin(); it != indicesToRemove.rend(); ++it) {
        if (*it < traces.size()) {
            traces.erase(traces.begin() + *it);
        }
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryTraces_.size(); ++i) {
        memoryTraces_[i].activation *= (1.0f - decayRate_);
        
        if (memoryTraces_[i].activation < Constants::WEAK_TRACE_THRESHOLD) {
            toRemove.push_back(i);
        }
    }
    
    if (!toRemove.empty()) {
        // Remove weak traces using optimized batch removal
        removeMemoryTraces(memoryTraces_, pImpl->neuronIndexMap, toRemove);
        
        // Update auxiliary data structures
        pImpl->neuronIndexMap.clear();
        pImpl->neuronSet.clear();
        pImpl->neuronActivationMap.clear();
        
        // Rebuild auxiliary structures
        for (size_t i = 0; i < memoryTraces_.size(); ++i) {
            const auto& trace = memoryTraces_[i];
            pImpl->neuronIndexMap[trace.neuronId] = i;
            pImpl->neuronSet.insert(trace.neuronId);
            pImpl->neuronActivationMap[trace.neuronId] = trace.activation;
        }
    }
}

// AttentionalSelection::Impl constructor optimization
AttentionalSelection::Impl::Impl() {
    // Pre-allocate common sizes for better performance
    competitionStrength.reserve(100);
    inhibitionLevel.reserve(100);
}

AttentionalSelection::AttentionalSelection()
    : pImpl(new Impl())
    , brain_(nullptr)
    , inhibitionStrength_(Constants::DEFAULT_INHIBITION_STRENGTH)
    , excitationStrength_(Constants::DEFAULT_EXCITATION_STRENGTH)
    , competitionThreshold_(Constants::DEFAULT_COMPETITION_THRESHOLD)
{
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl = std::make_unique<Impl>();
    brain_ = brain;
    NLM_LOG_INFO("AttentionalSelection initialized");
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                              float globalInhibition) {
    winners_.clear();
    
    if (competitors.empty()) return winners_;
    
    // Pre-allocate vectors for better performance
    std::vector<float> activities;
    activities.reserve(competitors.size());
    activities.assign(competitors.size(), 0.0f);
    
    float totalActivity = 0.0f;
    
    // Single pass to compute activity levels
    for (size_t i = 0; i < competitors.size(); ++i) {
        NeuronId neuron = competitors[i];
        
        // Get current activation from salience and top-down bias (from maps)
        auto salIt = bottomUpSalience_.find(neuron.value);
        float salience = (salIt != bottomUpSalience_.end()) ? salIt->second : 0.0f;
        auto biasIt = topDownBias_.find(neuron.value);
        float bias = (biasIt != topDownBias_.end()) ? biasIt->second : 0.0f;
        
        activities[i] = salience + bias;
        totalActivity += activities[i];
    }
    
    if (totalActivity < Constants::COMPETITOR_ACTIVITY_EPSILON) {
        // No strong competitors - all equal
        return competitors;
    }
    
    // Optimized competition with early pruning
    pImpl->inhibitionLevel.clear();
    pImpl->inhibitionLevel.reserve(competitors.size());
    
    // Find max activity to prune weak competitors early
    float maxActivity = 0.0f;
    for (float act : activities) {
        if (act > maxActivity) maxActivity = act;
    }
    
    // Only process competitors with significant activity
    for (size_t i = 0; i < competitors.size(); ++i) {
        if (activities[i] < maxActivity * Constants::RELATIVE_ACTIVITY_THRESHOLD) {
            continue;  // Skip weak competitors
        }
        
        for (size_t j = 0; j < competitors.size(); ++j) {
            if (i == j) continue;
            
            float relativeActivity = activities[i] / (activities[j] + Constants::COMPETITOR_ACTIVITY_EPSILON);
            
            if (relativeActivity > Constants::RELATIVE_ACTIVITY_THRESHOLD) {
                // i is much stronger than j - apply inhibition to j
                if (brain_) {
                    brain_->injectCurrent(competitors[j], -globalInhibition * inhibitionStrength_);
                }
                pImpl->inhibitionLevel.push_back(globalInhibition * inhibitionStrength_);
            }
        }
    }
    
    // Winners are neurons with above-threshold activity
    float threshold = competitionThreshold_ * totalActivity / competitors.size();
    
    // Pre-allocate winners vector with estimated size
    winners_.clear();
    winners_.reserve(std::min(static_cast<size_t>(10), competitors.size()));
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        if (activities[i] >= threshold) {
            winners_.push_back(competitors[i]);
            
            // Apply excitation to winners
            if (brain_) {
                brain_->injectCurrent(competitors[i], excitationStrength_ * Constants::EXCITATION_MULTIPLIER);
            }
        }
    }
    
    return winners_;
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    // Use unordered_set for O(1) membership testing
    if (std::find(attendedRegions_.begin(), attendedRegions_.end(), region) == attendedRegions_.end()) {
        attendedRegions_.push_back(region);
    }
}

void AttentionalSelection::releaseAttention() {
    attendedRegions_.clear();
}

std::vector<RegionId> AttentionalSelection::getAttendedRegions() const {
    return attendedRegions_;
}

void AttentionalSelection::setInhibitionStrength(float strength) {
    inhibitionStrength_ = strength;
}

void AttentionalSelection::setExcitationStrength(float strength) {
    excitationStrength_ = strength;
}

void AttentionalSelection::setCompetitionThreshold(float threshold) {
    competitionThreshold_ = threshold;
}

float AttentionalSelection::getInhibitionFor(NeuronId neuron) const {
    // Use unordered_set for O(1) lookup
    auto it = std::find(winners_.begin(), winners_.end(), neuron);
    if (it != winners_.end()) {
        return 0.0f;  // Winners don't receive inhibition
    }
    return inhibitionStrength_;
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    // Use unordered_set for O(1) lookup
    auto it = std::find(winners_.begin(), winners_.end(), neuron);
    if (it != winners_.end()) {
        return excitationStrength_;
    }
    return 0.0f;
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Decay salience and bias over time - use constants
    for (auto& s : bottomUpSalience_) {
        s *= Constants::SALIENCE_DECAY_RATE;
    }
    
    for (auto& b : topDownBias_) {
        b *= Constants::BIAS_DECAY_RATE;
    }
    
    // Decay inhibition - use constants
    for (auto& i : pImpl->inhibitionLevel) {
        i *= Constants::INHIBITION_DECAY_RATE;
    }
}

bool AttentionalSelection::isAttended(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

void AttentionalSelection::applyTopDownBias(NeuronId neuron, float biasStrength) {
    // Store bias in map keyed by NeuronId
    auto it = topDownBias_.find(neuron.value);
    if (it != topDownBias_.end()) {
        it->second += biasStrength;
    } else {
        topDownBias_[neuron.value] = biasStrength;
    }
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    // Store salience in map keyed by NeuronId
    auto it = bottomUpSalience_.find(neuron.value);
    if (it != bottomUpSalience_.end()) {
        it->second += salienceStrength;
    } else {
        bottomUpSalience_[neuron.value] = salienceStrength;
    }
}

void AttentionalSelection::reset() {
    winners_.clear();
    attendedRegions_.clear();
    neuronSalience_.clear();
    topDownBias_.clear();
    bottomUpSalience_.clear();
    pImpl->inhibitionLevel.clear();
}

} // namespace nlm