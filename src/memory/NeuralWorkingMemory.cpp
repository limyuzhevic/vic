#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>
#include <limits>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Maintenance connections (recurrent)
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    
    // Memory trace ages
    std::vector<SimulationStep> traceAges;
    
    // Co-activation history for Hebbian-like strengthening
    std::unordered_map<uint64_t, float> coActivationHistory;
    
    // Spontaneous reactivation counter
    std::vector<SimulationStep> lastReactivationStep;
    
    Impl() : brain(nullptr) {}
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
    , minActivationForMaintenance_(0.05f)
    , spontaneousReactivationThreshold_(0.1f)
    , neuromodulationStabilizationFactor_(0.5f)
{
    // Initialize PImpl members
    pImpl->brain = nullptr;
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize all dynamic structures
    pImpl->maintenanceSynapses.clear();
    pImpl->traceAges.clear();
    pImpl->coActivationHistory.clear();
    pImpl->lastReactivationStep.clear();
    
    // Resize container structures to initial capacity
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    activityHistory_.clear();
    traceImportance_.clear();
    
    // Reserve capacity for better performance
    memoryNeurons_.reserve(capacity_);
    memoryActivations_.reserve(capacity_);
    memoryTimestamps_.reserve(capacity_);
    activityHistory_.reserve(capacity_);
    traceImportance_.reserve(capacity_);
    
    NLM_LOG_INFO("NeuralWorkingMemory initialized with enhanced maintenance mechanisms");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Find neurons to encode this pattern
    size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
        float activation = pattern[i] * strength;
        
        // Set neuron activation
        if (auto* n = brain_->getRegion(neuron.getId() / 1000)->getAllNeurons()) {
            for (auto* nn : *n) {
                if (nn->getId() == neuron) {
                    nn->injectCurrent(activation * 5.0f);
                    break;
                }
            }
        }
        
        // Update stored activation
        if (i < memoryActivations_.size()) {
            memoryActivations_[i] = activation;
            if (activityHistory_.size() > i) {
                activityHistory_[i] = std::min(1.0f, activityHistory_[i] + 0.5f);
            }
        } else {
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
            memoryNeurons_.push_back(neuron);
            activityHistory_.push_back(0.0f);
            traceImportance_.push_back(activation); // Initial importance based on activation
        }
    }
    
    // Create robust bidirectional recurrent connections for maintenance
    for (size_t i = 1; i < memoryNeurons_.size(); ++i) {
        createRecurrentConnection(memoryNeurons_[i-1], memoryNeurons_[i], strength * 0.7f);
        // Also create backward connection for stronger maintenance
        createRecurrentConnection(memoryNeurons_[i], memoryNeurons_[i-1], strength * 0.7f);
        
        // Update co-activation history
        uint64_t hash = std::hash<uint64_t>()(memoryNeurons_[i-1].value);
        hash ^= std::hash<uint64_t>()(memoryNeurons_[i].value) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        pImpl->coActivationHistory[hash] += 0.5f;
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
        if (activityHistory_.size() > idx) {
            activityHistory_[idx] = std::min(1.0f, activityHistory_[idx] + 0.5f);
        }
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
        activityHistory_.push_back(0.0f);
        traceImportance_.push_back(activation);
    }
    
    // Inject current to maintain activation with enhanced strength
    if (brain_) {
        brain_->injectCurrent(neuron, activation * 5.0f);
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    result.reserve(memoryActivations_.size());
    
    for (float activation : memoryActivations_) {
        result.push_back(activation);
    }
    
    return result;
}

bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    return std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron) != memoryNeurons_.end();
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        return memoryActivations_[idx];
    }
    return 0.0f;
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Update maintenance - reinforce active memory neurons with enhanced recurrent connections
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > 0.1f) {
            // Inject maintenance current with enhanced strength
            brain_->injectCurrent(neuron, activation * 2.5f);
            
            // Update activity history for importance computation
            if (activityHistory_.size() > i) {
                activityHistory_[i] = std::min(1.0f, activityHistory_[i] + 0.1f);
            }
            
            // Age the trace
            memoryTimestamps_[i]++;
            
            // Check if trace is too old
            if (memoryTimestamps_[i] > 1000) {
                activation *= (1.0f - decayRate_);
            }
            
            memoryActivations_[i] = activation;
        }
    }
    
    // Update activity history (decay over time)
    for (size_t i = 0; i < activityHistory_.size(); ++i) {
        activityHistory_[i] *= 0.9f;
    }
    
    // Apply enhanced maintenance mechanisms
    strengthenCoActivationConnections();  // Hebbian-like strengthening
    spontaneousReactivation();           // Reactivate dormant traces
    applyNeuromodulatoryStabilization();   // Neuromodulation-based stabilization
    prioritizeImportantTraces();           // Compute importance scores
    evictLeastImportantTraces();           // Maintain capacity
    
    // Run enhanced competition to select winners
    runCompetition();
}

void NeuralWorkingMemory::clear() {
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    activeTraces_.clear();
    pImpl->maintenanceSynapses.clear();
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    for (auto& activation : memoryActivations_) {
        activation = std::min(1.0f, activation * factor);
    }
}

void NeuralWorkingMemory::runCompetition() {
    winners_.clear();
    
    if (memoryActivations_.empty()) return;
    
    // Find neurons with above-threshold activation
    float threshold = 0.3f;
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryActivations_[i] >= threshold) {
            winners_.push_back(memoryNeurons_[i]);
        }
    }
    
    // Inhibitory competition - suppress non-winners
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        bool isWinner = std::find(winners_.begin(), winners_.end(), memoryNeurons_[i]) != winners_.end();
        
        if (!isWinner && brain_) {
            // Apply strong inhibition
            brain_->injectCurrent(memoryNeurons_[i], -memoryActivations_[i] * 3.0f);
        }
    }
}

bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

float NeuralWorkingMemory::getMemoryActivity() const {
    if (memoryActivations_.empty()) return 0.0f;
    
    float total = 0.0f;
    for (float act : memoryActivations_) {
        total += act;
    }
    return total / memoryActivations_.size();
}

float NeuralWorkingMemory::getCurrentNeuromodulationLevel() const {
    if (!brain_) return 0.0f;
    
    // Get dopamine level from brain's dopamine system if available
    Dopamine* dopamine = brain_->getDopamine();
    if (dopamine) {
        return dopamine->getLevel();
    }
    
    // Fallback: use a default level
    return 0.5f;
}

void NeuralWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    // Check if connection already exists
    for (const auto& conn : pImpl->maintenanceSynapses) {
        if (conn.first == from && conn.second == to) return;
    }
    
    pImpl->maintenanceSynapses.emplace_back(from, to);
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Apply maintenance currents through recurrent connections
    for (const auto& conn : pImpl->maintenanceSynapses) {
        float fromActivation = 0.0f;
        
        // Find from neuron activation
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.first);
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            fromActivation = memoryActivations_[idx];
        }
        
        if (fromActivation > 0.1f && brain_) {
            // Send maintenance signal
            brain_->injectCurrent(conn.second, fromActivation * 2.0f);
        }
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        memoryActivations_[i] *= (1.0f - decayRate_);
        
        if (memoryActivations_[i] < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        memoryNeurons_.erase(memoryNeurons_.begin() + *it);
        memoryActivations_.erase(memoryActivations_.begin() + *it);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + *it);
    }
}

// AttentionalSelection Implementation
struct AttentionalSelection::Impl {
    // Neuron competition state
    std::vector<float> competitionStrength;
    std::vector<float> inhibitionLevel;
    
    Impl() {}
};

AttentionalSelection::AttentionalSelection()
    : pImpl(new Impl)
    , brain_(nullptr)
    , inhibitionStrength_(0.5f)
    , excitationStrength_(1.5f)
    , competitionThreshold_(0.3f)
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
    
    // Compute activity levels for competitors
    std::vector<float> activities(competitors.size(), 0.0f);
    float totalActivity = 0.0f;
    
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
    
    if (totalActivity < 0.001f) {
        // No strong competitors - all equal
        return competitors;
    }
    
    // Competition: neurons inhibit each other based on relative activity
    for (size_t i = 0; i < competitors.size(); ++i) {
        for (size_t j = 0; j < competitors.size(); ++j) {
            if (i == j) continue;
            
            float relativeActivity = activities[i] / (activities[j] + 0.001f);
            
            if (relativeActivity > 1.5f) {
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
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        if (activities[i] >= threshold) {
            winners_.push_back(competitors[i]);
            
            // Apply excitation to winners
            if (brain_) {
                brain_->injectCurrent(competitors[i], excitationStrength_ * 2.0f);
            }
        }
    }
    
    return winners_;
}

void AttentionalSelection::focusOnRegion(RegionId region) {
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
    auto it = std::find(winners_.begin(), winners_.end(), neuron);
    if (it != winners_.end()) {
        return 0.0f;  // Winners don't receive inhibition
    }
    return inhibitionStrength_;
}

float AttentionalSelection::getExcitationFor(NeuronId neuron) const {
    auto it = std::find(winners_.begin(), winners_.end(), neuron);
    if (it != winners_.end()) {
        return excitationStrength_;
    }
    return 0.0f;
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Decay salience and bias over time
    for (auto& s : bottomUpSalience_) {
        s *= 0.95f;
    }
    
    for (auto& b : topDownBias_) {
        b *= 0.98f;
    }
    
    // Decay inhibition
    for (auto& i : pImpl->inhibitionLevel) {
        i *= 0.9f;
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

// Helper functions for enhanced maintenance mechanisms

void NeuralWorkingMemory::strengthenCoActivationConnections() {
    if (!brain_ || pImpl->coActivationHistory.empty()) return;
    
    // Strengthen connections between neurons that have been co-activated
    for (const auto& historyPair : pImpl->coActivationHistory) {
        uint64_t hash = historyPair.first;
        float strength = historyPair.second;
        
        // Decode hash to get neuron pair (simplified: use hash modulo for demonstration)
        uint64_t neuron1Hash = hash % (std::numeric_limits<uint64_t>::max() / 2);
        uint64_t neuron2Hash = neuron1Hash + 1;
        
        // Find neurons with these hashes (simplified implementation)
        // In a full implementation, you'd maintain a proper mapping
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            NeuronId neuron1 = memoryNeurons_[i];
            float act1 = memoryActivations_[i];
            
            for (size_t j = i + 1; j < memoryNeurons_.size(); ++j) {
                NeuronId neuron2 = memoryNeurons_[j];
                float act2 = memoryActivations_[j];
                
                // Create strong recurrent connections
                createRecurrentConnection(neuron1, neuron2, strength * 0.8f);
                createRecurrentConnection(neuron2, neuron1, strength * 0.8f);
                
                // Boost activation through recurrent connections
                if (act1 > 0.1f && act2 > 0.1f) {
                    float boost = strength * 0.3f;
                    brain_->injectCurrent(neuron1, boost);
                    brain_->injectCurrent(neuron2, boost);
                }
            }
        }
    }
}

void NeuralWorkingMemory::spontaneousReactivation() {
    if (!brain_) return;
    
    // Reactivate weak but potentially important traces
    float neuromodLevel = getCurrentNeuromodulationLevel();
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        float& activation = memoryActivations_[i];
        
        // Check if trace is below spontaneous reactivation threshold
        if (activation < spontaneousReactivationThreshold_ && 
            (pImpl->traceAges.size() > i && memoryTimestamps_[i] > 1000)) {
            
            // Calculate reactivation strength based on neuromodulation level
            float reactivateStrength = spontaneousReactivationThreshold_ * 
                                     (1.0f + neuromodLevel * 0.5f);
            
            // Boost activation
            activation = std::min(1.0f, activation + reactivateStrength);
            
            // Update timestamp
            memoryTimestamps_[i] = 0;
            
            // Record last reactivation
            if (pImpl->lastReactivationStep.size() <= i) {
                pImpl->lastReactivationStep.resize(i + 1);
            }
            pImpl->lastReactivationStep[i] = pImpl->brain->getCurrentStep();
            
            // Inject current to maintain reactivation
            brain_->injectCurrent(memoryNeurons_[i], activation * 3.0f);
        }
    }
}

void NeuralWorkingMemory::applyNeuromodulatoryStabilization() {
    if (!brain_) return;
    
    float neuromodLevel = getCurrentNeuromodulationLevel();
    
    // Apply stabilization to all memory traces
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        float& activation = memoryActivations_[i];
        
        if (activation > 0.1f) {
            // Neuromodulation stabilizes memory traces
            float stabilizationBoost = neuromodLevel * neuromodulationStabilizationFactor_;
            activation = std::min(1.0f, activation * (1.0f + stabilizationBoost));
            
            // Inject additional current for stabilization
            brain_->injectCurrent(memoryNeurons_[i], stabilizationBoost * 2.0f);
        }
    }
}

void NeuralWorkingMemory::prioritizeImportantTraces() {
    traceImportance_.resize(memoryNeurons_.size());
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        // Calculate importance based on multiple factors:
        // 1. Current activation level
        // 2. Age of trace
        // 3. Recent co-activation history
        float activation = memoryActivations_[i];
        float age = (pImpl->traceAges.size() > i) ? pImpl->traceAges[i] : 0.0f;
        
        // Base importance from activation and age
        float importance = activation * 0.5f + std::min(1.0f, age / 1000.0f) * 0.3f;
        
        // Add co-activation bonus (higher for recently co-activated neurons)
        // Simplified: use activation history as proxy
        if (activityHistory_.size() > i) {
            importance += activityHistory_[i] * 0.2f;
        }
        
        traceImportance_[i] = importance;
    }
}

void NeuralWorkingMemory::evictLeastImportantTraces() {
    if (memoryNeurons_.size() <= capacity_) return;
    
    // Create indices sorted by importance (ascending = least important first)
    std::vector<size_t> indices(memoryNeurons_.size());
    std::iota(indices.begin(), indices.end(), 0);
    
    std::sort(indices.begin(), indices.end(), [this](size_t a, size_t b) {
        return getTraceImportance(a) < getTraceImportance(b);
    });
    
    // Evict from least to most important until we meet capacity
    size_t toEvict = memoryNeurons_.size() - capacity_;
    for (size_t i = 0; i < toEvict; ++i) {
        size_t idx = indices[i];
        
        // Remove from all containers
        memoryNeurons_.erase(memoryNeurons_.begin() + idx);
        memoryActivations_.erase(memoryActivations_.begin() + idx);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
        activityHistory_.erase(activityHistory_.begin() + idx);
        traceImportance_.erase(traceImportance_.begin() + idx);
        
        // Remove from maintenance synapses
        pImpl->maintenanceSynapses.erase(
            std::remove_if(pImpl->maintenanceSynapses.begin(), pImpl->maintenanceSynapses.end(),
                [idx](const auto& conn) {
                    return (conn.first.value == memoryNeurons_[idx].value || 
                           conn.second.value == memoryNeurons_[idx].value);
                }),
            pImpl->maintenanceSynapses.end()
        );
    }
}

float NeuralWorkingMemory::computeActivityDependentDecayRate() const {
    if (memoryActivations_.empty()) return decayRate_;
    
    // Compute average activation level
    float totalActivation = 0.0f;
    for (float act : memoryActivations_) {
        totalActivation += act;
    }
    float avgActivation = totalActivation / memoryActivations_.size();
    
    // Activity-dependent decay: higher activation = slower decay
    // Formula: base_decay * (1.0 - 0.8 * activation)
    float activityDependentDecay = decayRate_ * (1.0f - 0.8f * avgActivation);
    
    // Ensure decay rate doesn't go below 0.001
    return std::max(0.001f, activityDependentDecay);
}

float NeuralWorkingMemory::getTraceImportance(size_t traceIndex) const {
    if (traceIndex >= traceImportance_.size()) return 0.0f;
    return traceImportance_[traceIndex];
}

// Implementation of other private methods

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    float currentDecayRate = computeActivityDependentDecayRate();
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        memoryActivations_[i] *= (1.0f - currentDecayRate);
        
        if (memoryActivations_[i] < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Update activity history before removal
    for (size_t idx : toRemove) {
        if (activityHistory_.size() > idx) {
            activityHistory_[idx] = 0.0f;
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        memoryNeurons_.erase(memoryNeurons_.begin() + *it);
        memoryActivations_.erase(memoryActivations_.begin() + *it);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + *it);
        activityHistory_.erase(activityHistory_.begin() + *it);
        traceImportance_.erase(traceImportance_.begin() + *it);
    }
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Apply maintenance currents through recurrent connections
    for (const auto& conn : pImpl->maintenanceSynapses) {
        float fromActivation = 0.0f;
        
        // Find from neuron activation
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.first);
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            fromActivation = memoryActivations_[idx];
        }
        
        if (fromActivation > minActivationForMaintenance_ && brain_) {
            // Send maintenance signal with enhanced strength
            brain_->injectCurrent(conn.second, fromActivation * 2.5f);
        }
    }
}

} // namespace nlm