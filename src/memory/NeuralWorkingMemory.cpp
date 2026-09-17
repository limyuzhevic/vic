#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>
#include <queue>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Recurrent connections for maintenance (stable connections)
    std::vector<std::pair<NeuronId, std::vector<NeuronId>>> maintenancePaths;
    
    // Memory trace dynamics
    std::vector<float> traceStabilities;
    std::vector<float> synapticWeight;
    std::vector<SimulationStep> lastReinforcedStep;
    
    // Competition dynamics
    std::vector<float> activationHistory;
    std::vector<float> lateralInhibition;
    
    // Capacity and management
    std::vector<size_t> traceActivityCount;
    
    Impl() : brain(nullptr) {}
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
{
    // Pre-allocate vectors for efficiency
    memoryNeurons_.reserve(capacity_);
    memoryActivations_.reserve(capacity_);
    memoryTimestamps_.reserve(capacity_);
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralWorkingMemory initialized with capacity: " + std::to_string(capacity_));
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Clear existing memory for new pattern (working memory is active)
    clear();
    
    // Encode pattern into neural activity with competitive allocation
    std::vector<std::pair<float, NeuronId>> neuronCandidates;
    
    // Find available neurons with sufficient capacity
    for (size_t i = 0; i < pattern.size(); ++i) {
        float activation = pattern[i] * strength;
        if (activation > 0.01f) {  // Only store non-trivial activations
            // Find neuron in brain with similar excitation level
            NeuronId selectedNeuron = findAvailableNeuron(activation);
            if (selectedNeuron != INVALID_NEURON_ID) {
                neuronCandidates.emplace_back(activation, selectedNeuron);
            }
        }
    }
    
    // Sort by activation strength (stronger patterns get priority)
    std::sort(neuronCandidates.begin(), neuronCandidates.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Store top neurons up to capacity
    size_t storedCount = 0;
    for (const auto& candidate : neuronCandidates) {
        if (storedCount >= capacity_) break;
        
        memoryNeurons_.push_back(candidate.second);
        memoryActivations_.push_back(candidate.first);
        memoryTimestamps_.push_back(0);
        
        // Set neuron to active state
        brain_->injectCurrent(candidate.second, candidate.first * 5.0f);
        
        // Create maintenance pathways for trace persistence
        createMaintenancePath(candidate.second);
        
        storedCount++;
    }
    
    // Update activity counts
    pImpl->traceActivityCount.resize(memoryNeurons_.size(), 0);
    
    NLM_LOG_DEBUG("Stored working memory pattern with " + std::to_string(storedCount) + " traces");
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
        pImpl->traceActivityCount[idx]++;
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
        pImpl->traceActivityCount.push_back(1);
        
        // Activate neuron
        brain_->injectCurrent(neuron, activation * 5.0f);
        
        // Create maintenance pathway
        createMaintenancePath(neuron);
    }
    
    // Boost stability of this trace
    boostTraceStability(neuron, 1.2f);
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    result.reserve(memoryNeurons_.size());
    
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
    
    // Update maintenance - reinforce active memory traces
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > 0.05f) {
            // Inject maintenance current to sustain activity
            brain_->injectCurrent(neuron, activation * 2.0f);
            
            // Age the trace
            memoryTimestamps_[i]++;
            
            // Update synaptic weight based on usage
            pImpl->synapticWeight[i] = std::min(1.0f, pImpl->synapticWeight[i] * 1.01f);
            
            // Boost stability for recently reinforced traces
            if (memoryTimestamps_[i] <= 100) {
                boostTraceStability(neuron, 1.01f);
            }
            
            // Check if trace needs reinforcement
            if (memoryTimestamps_[i] > 500) {
                // Weak traces decay
                activation *= (1.0f - decayRate_);
                memoryActivations_[i] = activation;
                
                // Decay synaptic weight
                pImpl->synapticWeight[i] *= (1.0f - decayRate_ * 2.0f);
            }
            
            // Re-inject if trace is stable enough
            if (pImpl->synapticWeight[i] > 0.3f && activation > 0.01f) {
                brain_->injectCurrent(neuron, activation * 1.5f);
            }
        } else {
            // Very weak activations decay faster
            activation *= (1.0f - decayRate_ * 3.0f);
            memoryActivations_[i] = activation;
        }
    }
    
    // Decay weak traces below threshold
    decayWeakTraces();
    
    // Run competition to select dominant traces
    runCompetition();
}

void NeuralWorkingMemory::clear() {
    // Clear neural activity
    for (NeuronId neuron : memoryNeurons_) {
        brain_->injectCurrent(neuron, -10.0f); // Strong inhibition to clear
    }
    
    // Clear memory structures
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    activeTraces_.clear();
    pImpl->maintenancePaths.clear();
    pImpl->traceStabilities.clear();
    pImpl->synapticWeight.clear();
    pImpl->traceActivityCount.clear();
    
    NLM_LOG_DEBUG("Working memory cleared");
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        memoryActivations_[i] = std::min(1.0f, memoryActivations_[i] * factor);
        pImpl->synapticWeight[i] = std::min(1.0f, pImpl->synapticWeight[i] * factor);
        boostTraceStability(memoryNeurons_[i], factor);
    }
}

void NeuralWorkingMemory::runCompetition() {
    winners_.clear();
    
    if (memoryActivations_.empty()) return;
    
    // Compute competition scores including stability and history
    std::vector<std::pair<float, size_t>> scores;
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        float activation = memoryActivations_[i];
        float stability = pImpl->traceStabilities[i];
        float history = pImpl->traceActivityCount[i] > 0 ? std::log10f(pImpl->traceActivityCount[i] + 1) : 0.0f;
        
        // Combined score for competition
        float score = activation * 0.5f + stability * 0.3f + history * 0.2f;
        scores.emplace_back(score, i);
    }
    
    // Sort by score (descending)
    std::sort(scores.begin(), scores.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Select winners (top 30% of traces)
    size_t numWinners = std::max(size_t(1), static_cast<size_t>(scores.size() * 0.3f));
    numWinners = std::min(numWinners, scores.size());
    
    for (size_t i = 0; i < numWinners; ++i) {
        size_t idx = scores[i].second;
        winners_.push_back(memoryNeurons_[idx]);
        
        // Strongly excite winners
        brain_->injectCurrent(memoryNeurons_[idx], memoryActivations_[idx] * 3.0f);
        
        // Mark as active trace
        activeTraces_.push_back(idx);
    }
    
    // Apply lateral inhibition to non-winners
    for (size_t i = numWinners; i < scores.size(); ++i) {
        size_t idx = scores[i].second;
        float inhibition = 0.5f + 0.5f * (scores[i].first / scores[numWinners-1].first);
        brain_->injectCurrent(memoryNeurons_[idx], -inhibition * 5.0f);
    }
    
    // Decay losing traces more aggressively
    for (size_t i = numWinners; i < scores.size(); ++i) {
        size_t idx = scores[i].second;
        memoryActivations_[idx] *= (1.0f - decayRate_ * 2.0f);
        pImpl->synapticWeight[idx] *= (1.0f - decayRate_ * 3.0f);
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
    createMaintenancePath(to);  // Simplified - create path to maintain trace
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Apply maintenance currents through recurrent connections
    for (const auto& path : pImpl->maintenancePaths) {
        NeuronId target = path.first;
        const std::vector<NeuronId>& sources = path.second;
        
        float totalInput = 0.0f;
        for (NeuronId source : sources) {
            auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), source);
            if (it != memoryNeurons_.end()) {
                size_t idx = std::distance(memoryNeurons_.begin(), it);
                totalInput += memoryActivations_[idx];
            }
        }
        
        if (totalInput > 0.1f && brain_) {
            brain_->injectCurrent(target, totalInput * 1.5f);
        }
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        memoryActivations_[i] *= (1.0f - decayRate_);
        pImpl->synapticWeight[i] *= (1.0f - decayRate_);
        
        // Remove traces that are too weak
        if (memoryActivations_[i] < 0.001f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        // Remove maintenance path for this neuron
        auto pathIt = std::find_if(pImpl->maintenancePaths.begin(), pImpl->maintenancePaths.end(),
                                   [neuron = memoryNeurons_[*it]](const auto& path) { return path.first == neuron; });
        if (pathIt != pImpl->maintenancePaths.end()) {
            pImpl->maintenancePaths.erase(pathIt);
        }
        
        memoryNeurons_.erase(memoryNeurons_.begin() + *it);
        memoryActivations_.erase(memoryActivations_.begin() + *it);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + *it);
        pImpl->traceStabilities.erase(pImpl->traceStabilities.begin() + *it);
        pImpl->synapticWeight.erase(pImpl->synapticWeight.begin() + *it);
        pImpl->traceActivityCount.erase(pImpl->traceActivityCount.begin() + *it);
    }
}

// Helper methods
NeuronId NeuralWorkingMemory::findAvailableNeuron(float activation) {
    // Simple search for available neurons
    // In a real implementation, this would query brain's neuron pool
    // For now, return a placeholder
    static uint64_t neuronCounter = 10000;
    return NeuronId(neuronCounter++);
}

void NeuralWorkingMemory::createMaintenancePath(NeuronId neuron) {
    // Create a maintenance pathway for this neuron
    // This represents recurrent connections that maintain the memory trace
    pImpl->maintenancePaths.push_back({neuron, std::vector<NeuronId>()});
}

void NeuralWorkingMemory::boostTraceStability(NeuronId neuron, float factor) {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        pImpl->traceStabilities[idx] = std::min(1.0f, pImpl->traceStabilities[idx] * factor);
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
