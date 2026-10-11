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
{
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
    if (memoryNeurons_.empty()) return;
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        // Use modulo with safety check
        size_t neuronIndex = i % memoryNeurons_.size();
        NeuronId neuron = memoryNeurons_[neuronIndex];
        float activation = pattern[i] * strength;
        
        // Set neuron activation with comprehensive safety checks
        RegionId regionId = neuron.getId() / 1000;
        if (regionId.value >= 1000) return;  // Invalid region ID
        
        auto* region = brain_->getRegion(regionId);
        if (!region) return;  // Invalid region
        
        auto* neurons = region->getAllNeurons();
        if (!neurons) return;  // Null neurons container
        
        for (auto* nn : *neurons) {
            if (nn && nn->getId() == neuron) {
                nn->injectCurrent(activation * 5.0f);
                break;
            }
        }
        
        // Update stored activation with bounds checking
        if (neuronIndex < memoryActivations_.size()) {
            memoryActivations_[neuronIndex] = activation;
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

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Validate inputs
    if (!brain_) return;
    if (activation < 0.0f) return;  // Invalid activation
    
    // Find or add this neuron to memory with bounds checking
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        // Bounds checking for parallel vectors
        if (idx >= memoryActivations_.size() || idx >= memoryTimestamps_.size()) return;
        
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
    } else if (memoryNeurons_.size() < capacity_) {
        // Safe to add new neuron
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
    } else {
        // Memory is full - don't add new neurons
        return;
    }
    
    // Inject current to maintain activation
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
    // Find neuron in memory with bounds checking
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        
        // Bounds checking for parallel vectors
        if (idx < memoryActivations_.size()) {
            return memoryActivations_[idx];
        }
    }
    return 0.0f;
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Ensure all parallel vectors have consistent sizes
    size_t minSize = std::min({memoryNeurons_.size(), memoryActivations_.size(), memoryTimestamps_.size()});
    if (minSize == 0) return;
    
    // Update maintenance - reinforce active memory neurons
    for (size_t i = 0; i < minSize; ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > 0.1f) {
            // Inject maintenance current with null check
            if (brain_) {
                brain_->injectCurrent(neuron, activation * 2.0f);
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
    
    // Ensure parallel vectors have consistent sizes
    size_t minSize = std::min({memoryNeurons_.size(), memoryActivations_.size()});
    if (minSize == 0) return;
    
    // Find neurons with above-threshold activation
    float threshold = 0.3f;
    
    for (size_t i = 0; i < minSize; ++i) {
        if (memoryActivations_[i] >= threshold) {
            winners_.push_back(memoryNeurons_[i]);
        }
    }
    
    // Inhibitory competition - suppress non-winners
    for (size_t i = 0; i < minSize; ++i) {
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
    // Ensure brain_ is valid and parallel vectors have consistent sizes
    if (!brain_) return;
    size_t minSize = std::min({memoryNeurons_.size(), memoryActivations_.size()});
    if (minSize == 0) return;
    
    // Apply maintenance currents through recurrent connections
    for (const auto& conn : pImpl->maintenanceSynapses) {
        float fromActivation = 0.0f;
        
        // Find from neuron activation with bounds checking
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.first);
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            
            // Bounds checking for parallel vectors
            if (idx < memoryActivations_.size()) {
                fromActivation = memoryActivations_[idx];
            }
        }
        
        if (fromActivation > 0.1f) {
            // Send maintenance signal with null check
            if (brain_) {
                brain_->injectCurrent(conn.second, fromActivation * 2.0f);
            }
        }
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    // Ensure all parallel vectors have consistent sizes
    size_t minSize = std::min({memoryNeurons_.size(), memoryActivations_.size(), memoryTimestamps_.size()});
    if (minSize == 0) return;
    
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < minSize; ++i) {
        memoryActivations_[i] *= (1.0f - decayRate_);
        
        if (memoryActivations_[i] < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t idx = *it;
        
        // Bounds checking before erasing
        if (idx < memoryNeurons_.size() && idx < memoryActivations_.size() && idx < memoryTimestamps_.size()) {
            memoryNeurons_.erase(memoryNeurons_.begin() + idx);
            memoryActivations_.erase(memoryActivations_.begin() + idx);
            memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
        }
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
    
    // OPTIMIZED COMPETITION: O(n log n) instead of O(n²)
    // Sort competitors by activity level to enable efficient winner determination
    std::vector<size_t> indices(competitors.size());
    std::iota(indices.begin(), indices.end(), 0);
    
    // Sort indices by activity level (descending)
    std::sort(indices.begin(), indices.end(),
        [&activities](size_t a, size_t b) { return activities[a] > activities[b]; });
    
    // Apply global inhibition to all competitors first
    if (brain_) {
        for (size_t i : indices) {
            brain_->injectCurrent(competitors[i], -globalInhibition * inhibitionStrength_);
        }
    }
    
    // Apply excitation to top performers (using sorted indices for efficiency)
    float threshold = competitionThreshold_ * totalActivity / competitors.size();
    pImpl->inhibitionLevel.reserve(indices.size());
    
    for (size_t i : indices) {
        if (activities[i] >= threshold) {
            winners_.push_back(competitors[i]);
            
            // Apply excitation to winners (remove inhibition)
            if (brain_) {
                brain_->injectCurrent(competitors[i], globalInhibition * excitationStrength_);
            }
            
            // Track inhibition level for performance metrics
            pImpl->inhibitionLevel.push_back(globalInhibition * inhibitionStrength_);
        }
    }
    
    // Set competition strength based on actual competition (simplified)
    pImpl->competitionStrength.clear();
    pImpl->competitionStrength.reserve(competitors.size());
    
    // Calculate competition strength as measure of how competitive the selection was
    float maxActivity = totalActivity > 0.0f ? *std::max_element(activities.begin(), activities.end()) : 0.0f;
    float competitionScore = 1.0f;
    
    if (maxActivity > 0.0f) {
        // Competition score based on activity distribution
        float activityRatio = maxActivity / totalActivity;
        competitionScore = std::min(1.0f, activityRatio / 0.3f);  // Cap at 1.0
    }
    
    for (size_t i : indices) {
        // Store competition strength for each neuron based on its relative activity
        pImpl->competitionStrength.push_back(competitionScore * activities[i] / (totalActivity + 0.001f));
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
    
    // Decay inhibition with bounds checking
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
    pImpl->competitionStrength.clear();
}

} // namespace nlm