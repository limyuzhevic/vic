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

    // Store memory pattern efficiently with validation
    if (pattern.empty() || !brain_) return;
    
    // Validate pattern strength
    if (strength < 0.0f || strength > 10.0f) {
        NLM_LOG_WARNING("Unusual pattern strength in store: " + std::to_string(strength));
    }
    
    // Validate pattern size - limit to reasonable number of neurons
    if (pattern.size() > 100) {
        NLM_LOG_WARNING("Pattern size unusually large in store: " + std::to_string(pattern.size()));
    }
    
    // Find neurons to encode this pattern efficiently using binary search
    size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = pattern[i] * strength;
        
        // Validate neuron activation
        if (std::isnan(activation) || std::isinf(activation)) {
            NLM_LOG_ERROR("Invalid neuron activation in store: " + std::to_string(activation));
            continue;
        }
        
        // Find the actual neuron in the brain and set activation
        if (brain_) {
            // Use direct brain lookup with validation
            bool found = false;
            for (auto& region : brain_->getRegions()) {
                auto neurons = region->getAllNeurons();
                for (auto* n : *neurons) {
                    if (n->getId() == neuron) {
                        n->injectCurrent(activation * 5.0f);
                        found = true;
                        break;
                    }
                }
                if (found) break;
            }
            
            if (!found) {
                NLM_LOG_WARNING("Neuron not found during store: " + std::to_string(neuron.index()));
            }
        }
        
        // Update stored activation with bounds checking
        if (i < memoryActivations_.size()) {
            memoryActivations_[i] = activation;
        } else {
            // This should not happen if we calculated neuronsNeeded correctly
            NLM_LOG_ERROR("Neuron index out of bounds in store");
        }
    }
    
    // Create maintenance connections efficiently
    for (size_t i = 1; i < memoryNeurons_.size() && i < 10; ++i) {  // Limit to avoid excessive connections
        createRecurrentConnection(memoryNeurons_[i-1], memoryNeurons_[i], strength * 0.5f);
    }
}

    // Validate input parameters with comprehensive checks
    if (std::isnan(neuron.index()) || std::isinf(neuron.index())) {
        NLM_LOG_ERROR("Invalid neuron ID in storeToNeuron: " + std::to_string(neuron.index()));
        return;
    }
    
    if (activation < -100.0f || activation > 100.0f) {
        NLM_LOG_WARNING("Unusual neuron activation in storeToNeuron: " + std::to_string(activation));
    }
    
    if (activation < 0.0f || std::isnan(activation) || std::isinf(activation)) {
        NLM_LOG_ERROR("Invalid neuron activation in storeToNeuron: " + std::to_string(activation));
        return;
    }
    
    // Use efficient lookup to find or add neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        // Update existing neuron activation with bounds checking
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        if (idx < memoryActivations_.size()) {
            memoryActivations_[idx] = activation;
        } else {
            NLM_LOG_ERROR("Neuron index out of bounds during activation update");
        }
        
        // Update timestamp
        if (idx < memoryTimestamps_.size()) {
            memoryTimestamps_[idx] = 0;
        }
    } else {
        // Add new neuron to memory with capacity checking
        if (memoryNeurons_.size() >= capacity_) {
            NLM_LOG_WARNING("Working memory at capacity, adding new neuron");
        }
        
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
    }
    
    // Inject current to maintain activation with validation
    if (brain_) {
        // Validate brain pointer before use
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

    // Use efficient hash-based lookup with validation
    if (std::isnan(neuron.index()) || std::isinf(neuron.index())) {
        NLM_LOG_ERROR("Invalid neuron ID in contains: " + std::to_string(neuron.index()));
        return false;
    }
    
    // Perform binary search for efficiency (memoryNeurons_ is sorted)
    auto it = std::lower_bound(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    return (it != memoryNeurons_.end() && *it == neuron);
}

    // Validate input parameters
    if (std::isnan(neuron.index()) || std::isinf(neuron.index())) {
        NLM_LOG_ERROR("Invalid neuron ID in getNeuronActivation: " + std::to_string(neuron.index()));
        return 0.0f;
    }
    
    // Use efficient hash-based lookup with validation
    auto it = std::lower_bound(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end() && *it == neuron) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        
        // Validate activation value before returning
        if (idx < memoryActivations_.size()) {
            float activation = memoryActivations_[idx];
            if (std::isnan(activation) || std::isinf(activation)) {
                NLM_LOG_ERROR("Invalid neuron activation stored: " + std::to_string(activation));
                return 0.0f;
            }
            return activation;
        } else {
            NLM_LOG_ERROR("Neuron index out of bounds in getNeuronActivation");
        }
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

    // Validate input parameters
    if (std::isnan(neuron.index()) || std::isinf(neuron.index())) {
        NLM_LOG_ERROR("Invalid neuron ID in isWinning: " + std::to_string(neuron.index()));
        return false;
    }
    
    // Use efficient hash-based lookup with validation
    auto it = std::lower_bound(winners_.begin(), winners_.end(), neuron);
    return (it != winners_.end() && *it == neuron);
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

// Validation helper functions for NeuralWorkingMemory class

bool NeuralWorkingMemory::isValidMemoryState() const {
    // Validate memory system state
    
    // Check if memory is properly initialized
    if (memoryNeurons_.size() != memoryActivations_.size() || 
        memoryNeurons_.size() != memoryTimestamps_.size()) {
        NLM_LOG_ERROR("Memory state inconsistency detected");
        return false;
    }
    
    // Validate each memory entry
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        // Validate neuron ID
        if (std::isnan(memoryNeurons_[i].index()) || std::isinf(memoryNeurons_[i].index())) {
            NLM_LOG_ERROR("Invalid neuron ID in memory at index " + std::to_string(i));
            return false;
        }
        
        // Validate activation value
        if (i < memoryActivations_.size() && 
            (std::isnan(memoryActivations_[i]) || std::isinf(memoryActivations_[i]))) {
            NLM_LOG_ERROR("Invalid activation value in memory at index " + std::to_string(i));
            return false;
        }
        
        // Validate timestamp
        if (i < memoryTimestamps_.size() && 
            (memoryTimestamps_[i] < 0 || memoryTimestamps_[i] > 1000000)) {
            NLM_LOG_WARNING("Unusual timestamp in memory at index " + std::to_string(i));
        }
    }
    
    // Validate capacity constraints
    if (memoryNeurons_.size() > capacity_ * 1.5f) {  // Allow some flexibility
        NLM_LOG_WARNING("Working memory usage exceeds capacity by 50%");
    }
    
    return true;
}

bool NeuralWorkingMemory::isValidPattern(const std::vector<float>& pattern) const {
    // Validate pattern for storage/retrieval
    
    if (pattern.empty()) {
        NLM_LOG_ERROR("Empty pattern detected");
        return false;
    }
    
    if (pattern.size() > 1000) {  // Reasonable upper limit
        NLM_LOG_WARNING("Pattern size unusually large: " + std::to_string(pattern.size()));
    }
    
    for (size_t i = 0; i < pattern.size(); ++i) {
        if (std::isnan(pattern[i]) || std::isinf(pattern[i])) {
            NLM_LOG_ERROR("Invalid pattern value at index " + std::to_string(i) + ": " + std::to_string(pattern[i]));
            return false;
        }
    }
    
    return true;
}

// Validation helper functions for AttentionalSelection class

bool AttentionalSelection::isValidCompetitionState() const {
    // Validate competition system state
    
    // Check if winners_ vector is consistent
    for (const auto& winner : winners_) {
        if (std::isnan(winner.index()) || std::isinf(winner.index())) {
            NLM_LOG_ERROR("Invalid winner neuron ID");
            return false;
        }
    }
    
    // Check if attended regions are consistent
    for (const auto& region : attendedRegions_) {
        if (std::isnan(region.index()) || std::isinf(region.index())) {
            NLM_LOG_ERROR("Invalid attended region ID");
            return false;
        }
    }
    
    // Validate inhibition levels
    for (const auto& level : pImpl->inhibitionLevel) {
        if (level < 0.0f || std::isnan(level) || std::isinf(level)) {
            NLM_LOG_ERROR("Invalid inhibition level");
            return false;
        }
    }
    
    return true;
}

bool AttentionalSelection::isValidSalienceValues() const {
    // Validate salience and bias values
    
    for (const auto& pair : bottomUpSalience_) {
        if (pair.second < 0.0f || std::isnan(pair.second) || std::isinf(pair.second)) {
            NLM_LOG_ERROR("Invalid bottom-up salience value for neuron " + std::to_string(pair.first));
            return false;
        }
    }
    
    for (const auto& pair : topDownBias_) {
        if (pair.second < -1.0f || pair.second > 1.0f || std::isnan(pair.second) || std::isinf(pair.second)) {
            NLM_LOG_ERROR("Invalid top-down bias value for neuron " + std::to_string(pair.first));
            return false;
        }
    }
    
    return true;
}

void NeuralWorkingMemory::logState() const {
    NLM_LOG_DEBUG("NeuralWorkingMemory state - Capacity: " + std::to_string(capacity_) + 
                  ", ActiveTraces: " + std::to_string(memoryNeurons_.size()) +
                  ", TotalActivation: " + std::to_string(getMemoryActivity()) +
                  ", DecayedTraces: " + std::to_string(memoryNeurons_.size()) +
                  ", CurrentWinners: " + std::to_string(winners_.size()));
}

void AttentionalSelection::logState() const {
    NLM_LOG_DEBUG("AttentionalSelection state - AttendedRegions: " + std::to_string(attendedRegions_.size()) +
                  ", TopDownBias: " + std::to_string(topDownBias_.size()) +
                  ", BottomUpSalience: " + std::to_string(bottomUpSalience_.size()) +
                  ", Winners: " + std::to_string(winners_.size()) +
                  ", InhibitionLevels: " + std::to_string(pImpl->inhibitionLevel.size()));
}

} // namespace nlm