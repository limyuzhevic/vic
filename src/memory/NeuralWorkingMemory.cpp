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
    , decayModel_(1)  // 0: exponential, 1: power-law
    , maxTraceAge_(2000)
    , competitionThreshold_(0.35f)
    , winnerPriorityBoost_(1.5f)
    , memoryEfficiency_(1.0f)
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
        
        // Enhanced efficiency calculation
        if (memoryNeurons_.size() > 0) {
            memoryEfficiency_ = std::min(1.0f, static_cast<float>(memoryNeurons_.size()) / capacity_);
        }
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

float NeuralWorkingMemory::getMemoryEfficiency() const {
    return memoryEfficiency_;
}

void NeuralWorkingMemory::setWinnerPriorityBoost(float boost) {
    winnerPriorityBoost_ = std::max(1.0f, std::min(3.0f, boost));
}

// Enhanced decay based on trace age and activation
void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        float activation = memoryActivations_[i];
        size_t age = memoryTimestamps_[i];
        
        if (activation < 0.01f) {
            toRemove.push_back(i);
            continue;
        }
        
        // Enhanced decay algorithm based on trace age
        float decayFactor;
        if (decayModel_ == 0) {
            // Exponential decay with age modulation
            decayFactor = decayRate_ * (1.0f + static_cast<float>(age) / maxTraceAge_ * 2.0f);
        } else {
            // Power-law decay (more biologically plausible for working memory)
            decayFactor = decayRate_ * std::pow(1.0f + static_cast<float>(age) / 100.0f, -0.5f);
        }
        
        // Apply decay
        activation *= (1.0f - decayFactor);
        
        // Adaptive thresholding based on memory efficiency
        float adaptiveThreshold = 0.01f * (1.0f - memoryEfficiency_ * 0.5f);
        
        if (activation < adaptiveThreshold) {
            toRemove.push_back(i);
        } else {
            memoryActivations_[i] = activation;
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t idx = *it;
        
        // Update memory efficiency
        if (memoryNeurons_.size() > 0) {
            memoryEfficiency_ = std::min(1.0f, static_cast<float>(memoryNeurons_.size() - 1) / capacity_);
        }
        
        // Remove from all arrays
        memoryNeurons_.erase(memoryNeurons_.begin() + idx);
        memoryActivations_.erase(memoryActivations_.begin() + idx);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
    }
}

// Enhanced competition with priority-based selection
void NeuralWorkingMemory::runCompetition() {
    winners_.clear();
    
    if (memoryActivations_.empty()) return;
    
    // Enhanced competition algorithm with priority weighting
    std::vector<std::pair<size_t, float>> scoredNeurons;
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        float activation = memoryActivations_[i];
        
        // Apply priority boost based on recency (younger traces get priority)
        float recencyBonus = 1.0f;
        if (memoryTimestamps_[i] < 500) {
            recencyBonus = 1.2f;
        } else if (memoryTimestamps_[i] < 1000) {
            recencyBonus = 1.1f;
        }
        
        // Apply efficiency-based modulation
        float efficiencyMod = 1.0f + (1.0f - memoryEfficiency_) * 0.2f;
        
        float effectiveActivation = activation * recencyBonus * efficiencyMod;
        
        // Apply competition threshold
        if (effectiveActivation >= competitionThreshold_ * winnerPriorityBoost_) {
            scoredNeurons.emplace_back(i, effectiveActivation);
        }
    }
    
    // Sort by effective activation (descending)
    std::sort(scoredNeurons.begin(), scoredNeurons.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Apply lateral inhibition among winners
    for (size_t i = 0; i < scoredNeurons.size(); ++i) {
        size_t neuronIdx = scoredNeurons[i].first;
        float activation = scoredNeurons[i].second;
        
        // Apply winner excitation
        winners_.push_back(memoryNeurons_[neuronIdx]);
        
        // Apply lateral inhibition to other traces
        for (size_t j = i + 1; j < scoredNeurons.size() && j < i + 3; ++j) {
            if (j < scoredNeurons.size()) {
                // Inhibit based on relative strength
                float inhibition = std::min(1.0f, (scoredNeurons[i].second - scoredNeurons[j].second) * 2.0f);
                if (brain_) {
                    brain_->injectCurrent(memoryNeurons_[scoredNeurons[j].first], 
                                       -inhibition * activation * 3.0f);
                }
            }
        }
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
        
        if (fromActivation > 0.1f && brain_) {
            // Send maintenance signal
            brain_->injectCurrent(conn.second, fromActivation * 2.0f);
        }
    }
}

void NeuralWorkingMemory::createOptimizedRecurrentConnections() {
    // Enhanced maintenance connections with strength-based placement
    if (memoryNeurons_.size() >= 2) {
        // Create stronger connections between more active neurons
        std::vector<std::pair<size_t, float>> neuronStrengths;
        
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            neuronStrengths.emplace_back(i, memoryActivations_[i]);
        }
        
        // Sort by strength descending
        std::sort(neuronStrengths.begin(), neuronStrengths.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        // Create connections between top N neurons
        size_t numConnections = std::min(size_t(3), neuronStrengths.size());
        for (size_t i = 0; i < numConnections - 1; ++i) {
            size_t fromIdx = neuronStrengths[i].first;
            size_t toIdx = neuronStrengths[i + 1].first;
            
            if (fromIdx != toIdx) {
                float strength = std::min(1.0f, neuronStrengths[i].second * neuronStrengths[i + 1].second);
                createRecurrentConnection(memoryNeurons_[fromIdx], memoryNeurons_[toIdx], strength * 0.5f);
            }
        }
    }
}

void NeuralWorkingMemory::optimizeTraceDistribution() {
    // Optimize memory usage by redistributing traces
    if (memoryNeurons_.empty() || memoryEfficiency_ < 0.3f) {
        // Clear and rebuild when efficiency is too low
        clear();
        return;
    }
    
    // Ensure capacity constraint is met
    if (memoryNeurons_.size() > capacity_) {
        std::vector<std::pair<size_t, float>> neuronPriorities;
        
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            // Calculate priority based on activation, recency, and efficiency
            float recency = 1.0f / (1.0f + static_cast<float>(memoryTimestamps_[i]) / maxTraceAge_);
            float priority = memoryActivations_[i] * recency * memoryEfficiency_;
            neuronPriorities.emplace_back(i, priority);
        }
        
        // Sort by priority descending
        std::sort(neuronPriorities.begin(), neuronPriorities.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        // Remove lowest priority traces
        size_t toRemove = memoryNeurons_.size() - capacity_;
        std::vector<size_t> removeIndices;
        for (size_t i = neuronPriorities.size() - toRemove; i < neuronPriorities.size(); ++i) {
            removeIndices.push_back(neuronPriorities[i].first);
        }
        
        // Remove in reverse order to maintain indices
        for (auto it = removeIndices.rbegin(); it != removeIndices.rend(); ++it) {
            size_t idx = *it;
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