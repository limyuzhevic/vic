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
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
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

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
    , maxTraceAge_(1000)  // Prevent infinite aging
    , winnerSelectionThreshold_(0.3f)
{
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize working memory with null brain");
        return;
    }
    
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize maintenance connections with pruning
    pImpl->maintenanceSynapses.clear();
    pImpl->traceAges.clear();
    
    NLM_LOG_INFO("NeuralWorkingMemory initialized with brain");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) {
        return;
    }
    
    // Resize working memory if needed
    size_t neuronsNeeded = pattern.size();
    memoryNeurons_.reserve(neuronsNeeded);
    memoryActivations_.reserve(neuronsNeeded);
    memoryTimestamps_.reserve(neuronsNeeded);
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        // Check if we have enough neurons in the brain
        if (i >= memoryNeurons_.size() && 
            pImpl->brain->getTotalNeuronCount() < neuronsNeeded) {
            break;  // Not enough neurons to encode this pattern
        }
        
        NeuronId neuron = memoryNeurons_[i];
        float activation = pattern[i] * strength;
        
        // Set neuron activation with bounds checking
        activation = std::clamp(activation, -1.0f, 1.0f);
        
        // Inject current to maintain activation with gain factor
        if (brain_) {
            brain_->injectCurrent(neuron, activation * 5.0f);
        }
        
        // Update stored activation
        if (i < memoryActivations_.size()) {
            memoryActivations_[i] = activation;
            if (i < memoryTimestamps_.size()) {
                memoryTimestamps_[i] = 0;  // Reset timestamp
            }
        } else {
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
            memoryNeurons_.push_back(neuron);
            
            // Initialize trace age
            if (i < pImpl->traceAges.size()) {
                pImpl->traceAges[i] = 0;
            }
        }
    }
    
    // Create maintenance connections if needed with error checking
    if (!memoryNeurons_.empty()) {
        for (size_t i = 1; i < std::min(size_t(10), memoryNeurons_.size()); ++i) {
            NeuronId from = memoryNeurons_[i - 1];
            NeuronId to = memoryNeurons_[i];
            if (from != to) {  // Prevent self-connections
                createRecurrentConnection(from, to, strength * 0.5f);
            }
        }
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot store to neuron: brain is null");
        return;
    }
    
    // Clamp activation for stability
    activation = std::clamp(activation, -1.0f, 1.0f);
    
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;  // Reset timestamp
        
        // Update trace age
        if (idx < pImpl->traceAges.size()) {
            pImpl->traceAges[idx] = 0;
        }
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
        pImpl->traceAges.push_back(0);  // Initialize trace age
        
        // Inject current to establish memory trace
        brain_->injectCurrent(neuron, activation * 5.0f);
    } else {
        // Replace oldest trace (FIFO) when at capacity
        if (!memoryNeurons_.empty()) {
            size_t oldestIdx = 0;
            uint32_t oldestTime = std::numeric_limits<uint32_t>::max();
            for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
                if (memoryTimestamps_[i] < oldestTime) {
                    oldestTime = memoryTimestamps_[i];
                    oldestIdx = i;
                }
            }
            
            // Remove old memory
            memoryNeurons_[oldestIdx] = neuron;
            memoryActivations_[oldestIdx] = activation;
            memoryTimestamps_[oldestIdx] = 0;
            
            // Inject current for new memory
            brain_->injectCurrent(neuron, activation * 5.0f);
            NLM_LOG_DEBUG("Working memory replaced old trace with new neuron");
        }
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    result.reserve(memoryActivations_.size());
    
    // Smooth retrieval to reduce noise
    float smoothingFactor = 0.5f;
    float weightedSum = 0.0f;
    float totalWeight = 0.0f;
    
    for (float activation : memoryActivations_) {
        weightedSum += activation;
        totalWeight += 1.0f;
    }
    
    if (totalWeight > 0.0f) {
        float averageActivation = weightedSum / totalWeight;
        for (float activation : memoryActivations_) {
            // Apply smoothing and bounds
            float smoothedActivation = averageActivation * smoothingFactor + activation * (1.0f - smoothingFactor);
            result.push_back(std::clamp(smoothedActivation, -1.0f, 1.0f));
        }
    } else {
        // Return normalized activations if no average
        for (float activation : memoryActivations_) {
            float normalized = activation / std::max(1.0f, std::abs(activation));
            result.push_back(normalized);
        }
    }
    
    return result;
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot update working memory: brain is null");
        return;
    }
    
    // Update maintenance - reinforce active memory neurons
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (std::abs(activation) > 0.1f) {
            // Inject maintenance current proportional to activation
            brain_->injectCurrent(neuron, activation * 2.0f);
            
            // Age the trace
            memoryTimestamps_[i]++;
            
            // Update trace age
            if (i < pImpl->traceAges.size()) {
                pImpl->traceAges[i]++;
            }
            
            // Check if trace is too old
            if (memoryTimestamps_[i] > maxTraceAge_) {
                activation *= (1.0f - decayRate_);  // Apply decay
            } else if (pImpl->traceAges[i] > 100) {  // Age-based decay
                activation *= (1.0f - decayRate_ * 0.5f);
            }
            
            memoryActivations_[i] = activation;
        }
    }
    
    // Update recurrent connections
    updateRecurrentConnections();
    
    // Decay weak traces
    decayWeakTraces();
    
    // Run competition to select winners
    runCompetition();
}

void NeuralWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    // Prevent self-connections
    if (from == to) {
        return;
    }
    
    // Check if connection already exists
    for (const auto& conn : pImpl->maintenanceSynapses) {
        if (conn.first == from && conn.second == to) {
            return;
        }
    }
    
    // Limit total connections for stability
    if (pImpl->maintenanceSynapses.size() > static_cast<size_t>(capacity_ * 2)) {
        // Remove oldest connections (FIFO)
        pImpl->maintenanceSynapses.erase(pImpl->maintenanceSynapses.begin());
    }
    
    pImpl->maintenanceSynapses.emplace_back(from, to);
    NLM_LOG_DEBUG("Created recurrent connection between neurons");
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
        
        if (std::abs(fromActivation) > 0.1f && brain_) {
            // Send maintenance signal with exponential decay
            float signalStrength = fromActivation * 2.0f * std::exp(-memoryTimestamps_[idx] / 100.0f);
            brain_->injectCurrent(conn.second, signalStrength);
        }
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        // Multiplicative decay for stability
        memoryActivations_[i] *= (1.0f - decayRate_);
        
        // Remove traces that are too weak
        if (std::abs(memoryActivations_[i]) < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t idx = *it;
        
        // Remove all associated connections
        pImpl->maintenanceSynapses.erase(
            std::remove_if(pImpl->maintenanceSynapses.begin(), pImpl->maintenanceSynapses.end(),
                [this, idx](const auto& conn) {
                    return conn.first == memoryNeurons_[idx] || conn.second == memoryNeurons_[idx];
                }),
            pImpl->maintenanceSynapses.end()
        );
        
        // Remove memory trace
        memoryNeurons_.erase(memoryNeurons_.begin() + idx);
        memoryActivations_.erase(memoryActivations_.begin() + idx);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
        if (idx < pImpl->traceAges.size()) {
            pImpl->traceAges.erase(pImpl->traceAges.begin() + idx);
        }
        
        // Update remaining connections after removal
        for (auto& conn : pImpl->maintenanceSynapses) {
            if (conn.first == memoryNeurons_[idx] || conn.second == memoryNeurons_[idx]) {
                conn = std::make_pair(NeuronId(), NeuronId());  // Mark for cleanup
            }
        }
    }
    
    // Clean up invalid connections
    pImpl->maintenanceSynapses.erase(
        std::remove_if(pImpl->maintenanceSynapses.begin(), pImpl->maintenanceSynapses.end(),
            [](const auto& conn) { return conn.first == NeuronId() && conn.second == NeuronId(); }),
        pImpl->maintenanceSynapses.end()
    );
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

} // namespace nlm