#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Maintenance connections (recurrent)
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    
    // Memory trace ages
    std::vector<SimulationStep> traceAges;
    
    Impl() : brain(nullptr) {}
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
    , memoryNeurons_()
    , memoryActivations_()
    , memoryTimestamps_()
    , activeTraces_()
    , recurrentConnections_()
    , winners_()
{
    NLM_LOG_INFO("NeuralWorkingMemory created with capacity " + std::to_string(capacity_));
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
    size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
        float activation = pattern[i] * strength;
        
        // Set neuron activation
        if (auto* region = brain_->getRegion(RegionId(neuron.index() / 1000))) {
            for (auto* nn : region->getAllNeurons()) {
                if (nn->getId() == neuron) {
                    nn->injectCurrent(activation * 5.0f);
                    break;
                }
            }
        }
        
        // Update stored activation
        if (i < memoryActivations_.size()) {
            memoryActivations_[i] = activation;
        } else {
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
            memoryNeurons_.push_back(neuron);
        }
    }
    
    // Create maintenance connections if needed
    for (size_t i = 1; i < memoryNeurons_.size(); ++i) {
        createRecurrentConnection(memoryNeurons_[i-1], memoryNeurons_[i], strength * 0.5f);
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
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    return it != memoryNeurons_.end();
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it == memoryNeurons_.end()) {
        return 0.0f;
    }
    
    size_t idx = std::distance(memoryNeurons_.begin(), it);
    return memoryActivations_[idx];
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    if (!brain_ || memoryNeurons_.empty()) {
        return;
    }
    
    // Apply decay to all memory traces
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        memoryActivations_[i] *= (1.0f - decayRate_ * dt);
        memoryTimestamps_[i] += dt;
    }
    
    // Update active traces based on current activation levels
    activeTraces_.clear();
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        if (memoryActivations_[i] > 0.1f) {
            activeTraces_.push_back(i);
        }
    }
    
    // Inject current to maintain active traces
    for (size_t idx : activeTraces_) {
        NeuronId neuron = memoryNeurons_[idx];
        float activation = memoryActivations_[idx];
        brain_->injectCurrent(neuron, activation * 2.0f);  // Reduced from 5.0f
    }
    
    // Run competition for limited capacity
    if (memoryNeurons_.size() > capacity_) {
        runCompetition();
    }
    
    // Clean up weak traces
    decayWeakTraces();
}

void NeuralWorkingMemory::clear() {
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    activeTraces_.clear();
    recurrentConnections_.clear();
    winners_.clear();
    
    // Clear all stored currents
    for (NeuronId neuron : memoryNeurons_) {
        brain_->injectCurrent(neuron, 0.0f);
    }
}

float NeuralWorkingMemory::getMemoryActivity() const {
    if (memoryActivations_.empty()) {
        return 0.0f;
    }
    
    float totalActivity = 0.0f;
    for (float activation : memoryActivations_) {
        totalActivity += activation;
    }
    
    return totalActivity / static_cast<float>(memoryActivations_.size());
}

void NeuralWorkingMemory::runCompetition() {
    if (memoryNeurons_.size() <= capacity_) {
        return;
    }
    
    // Select winners based on highest activation levels
    std::vector<size_t> winnerIndices;
    std::vector<float> winnerActivations;
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        winnerIndices.push_back(i);
        winnerActivations.push_back(memoryActivations_[i]);
    }
    
    // Sort by activation (highest first)
    std::sort(winnerIndices.begin(), winnerIndices.end(),
              [&winnerActivations](size_t a, size_t b) {
                  return winnerActivations[a] > winnerActivations[b];
              });
    
    // Keep only top capacity_ neurons
    size_t numToKeep = std::min(capacity_, memoryNeurons_.size());
    
    // Update memory with winners
    std::vector<NeuronId> newNeurons;
    std::vector<float> newActivations;
    std::vector<SimulationStep> newTimestamps;
    
    for (size_t i = 0; i < numToKeep; ++i) {
        size_t idx = winnerIndices[i];
        newNeurons.push_back(memoryNeurons_[idx]);
        newActivations.push_back(memoryActivations_[idx]);
        newTimestamps.push_back(memoryTimestamps_[idx]);
    }
    
    memoryNeurons_ = newNeurons;
    memoryActivations_ = newActivations;
    memoryTimestamps_ = newTimestamps;
    
    // Update active traces
    activeTraces_.clear();
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        if (memoryActivations_[i] > 0.1f) {
            activeTraces_.push_back(i);
        }
    }
    
    // Clear recurrent connections
    recurrentConnections_.clear();
    
    // Create new maintenance connections
    for (size_t i = 1; i < memoryNeurons_.size(); ++i) {
        createRecurrentConnection(memoryNeurons_[i-1], memoryNeurons_[i], 0.5f);
    }
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    for (float& activation : memoryActivations_) {
        activation = std::min(1.0f, activation * factor);
    }
}

bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    auto it = std::find(winners_.begin(), winners_.end(), neuron);
    return it != winners_.end();
}

void NeuralWorkingMemory::decayWeakTraces() {
    // Remove traces with very low activation
    auto it = std::remove_if(memoryNeurons_.begin(), memoryNeurons_.end(),
                             [this](const NeuronId& neuron) {
                                 // Find the index of this neuron
                                 auto actIt = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
                                 if (actIt == memoryNeurons_.end()) return false;
                                 size_t idx = std::distance(memoryNeurons_.begin(), actIt);
                                 return memoryActivations_[idx] < 0.05f;
                             });
    
    // Remove the neuron from all parallel vectors
    if (it != memoryNeurons_.end()) {
        size_t removedIdx = std::distance(memoryNeurons_.begin(), it);
        memoryNeurons_.erase(it, memoryNeurons_.end());
        
        // Remove corresponding activations and timestamps
        if (removedIdx < memoryActivations_.size()) {
            memoryActivations_.erase(memoryActivations_.begin() + removedIdx,
                                    memoryActivations_.end());
            memoryTimestamps_.erase(memoryTimestamps_.begin() + removedIdx,
                                    memoryTimestamps_.end());
        }
        
        // Update active traces
        activeTraces_.erase(std::remove_if(activeTraces_.begin(), activeTraces_.end(),
                                           [removedIdx](size_t idx) { return idx == removedIdx; || idx > removedIdx; }),
                            activeTraces_.end());
        
        // Rebuild indices in activeTraces_
        for (size_t& idx : activeTraces_) {
            if (idx > removedIdx) idx--;
        }
    }
}
    
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
    
    // Update maintenance - reinforce active memory neurons
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > 0.1f) {
            // Inject maintenance current
            brain_->injectCurrent(neuron, activation * 2.0f);
            
            // Age the trace
            memoryTimestamps_[i]++;
            
            // Check if trace is too old
            if (memoryTimestamps_[i] > 1000) {
                activation *= (1.0f - decayRate_);
            }
            
            memoryActivations_[i] = activation;
        }
    }
    
    // Decay weak traces
    decayWeakTraces();
    
    // Run competition to select winners
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
    // Phase 6: erasing shifts memory indices, so the trace list
    // cannot survive removal untouched — rebuild it from the
    // remaining activations instead of leaving stale indices.
    activeTraces_.clear();
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        if (memoryActivations_[i] > 0.1f) activeTraces_.push_back(i);
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
        s.second *= 0.95f;
    }
    
    for (auto& b : topDownBias_) {
        b.second *= 0.98f;
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

} // namespace nlm