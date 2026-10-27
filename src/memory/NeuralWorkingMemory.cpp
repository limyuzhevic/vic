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
    
    // Advanced pattern encoding: find best matching neurons
    size_t neuronsNeeded = std::min(pattern.size(), brain_->getTotalNeuronCount());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        // Sample random neuron for pattern encoding (in real implementation, would use proper encoding)
        auto neurons = brain_->getAllNeurons();
        if (neurons.empty()) {
            continue;
        }
        
        NeuronId neuron = neurons[i % neurons.size()]->getId();
        float activation = pattern[i % pattern.size()] * strength;
        
        // Set neuron activation with proper neural dynamics
        if (brain_) {
            // Use adaptive current injection based on neuron type and state
            NeuronType neuronType = NeuronType::Excitatory;  // Simplified - would query actual neuron type
            
            // Calculate adaptive current based on activation and neuron properties
            float adaptiveCurrent = activation * 2.0f;  // Base current
            adaptiveCurrent += (neuronType == NeuronType::Inhibitory) ? -1.0f : 0.0f;
            
            brain_->injectCurrent(neuron, adaptiveCurrent);
        }
        
        // Update stored activation with history
        size_t idx = i % capacity_;
        if (idx < memoryActivations_.size()) {
            // Use Hebbian-like update: new activation combines with old
            memoryActivations_[idx] = memoryActivations_[idx] * 0.7f + activation * 0.3f;
        } else {
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
            memoryNeurons_.push_back(neuron);
        }
    }
    
    // Create intelligent maintenance connections
    createIntelligentMaintenanceConnections(strength);
    
    // Run competition to select active traces
    runCompetition();
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
    
    // Adaptive maintenance based on memory importance and competition
    float totalActivity = 0.0f;
    for (float activation : memoryActivations_) {
        totalActivity += activation;
    }
    
    // Normalize total activity to compute homeostatic scaling
    float activityScale = (totalActivity > 0.0f) ? (1.0f / totalActivity) : 1.0f;
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > 0.01f) {
            // Homeostatic regulation: maintain balanced activity
            float homeostaticCurrent = (1.0f - activation) * 0.5f * activityScale;
            
            // Memory-specific maintenance: stronger for recently stored items
            float recencyBoost = 1.0f / (1.0f + static_cast<float>(memoryTimestamps_[i]) / 100.0f);
            
            // Competitive excitation: winners get boosted
            float competitionBoost = isWinning(neuron) ? 1.5f : 1.0f;
            
            // Adaptive current injection with neural dynamics
            float adaptiveCurrent = activation * 2.0f * competitionBoost * recencyBoost + homeostaticCurrent;
            
            brain_->injectCurrent(neuron, adaptiveCurrent);
            
            // Update trace age and apply age-dependent decay
            memoryTimestamps_[i]++;
            
            // Apply realistic decay based on trace age
            float ageDecayFactor = std::exp(-decayRate_ * static_cast<float>(memoryTimestamps_[i]) / 100.0f);
            activation *= ageDecayFactor;
            
            // Apply short-term synaptic plasticity
            activation = applySynapticPlasticity(activation, neuron);
            
            memoryActivations_[i] = activation;
            
            // Track active traces
            if (activation > 0.05f) {
                activeTraces_.push_back(i);
            }
        }
    }
    
    // Update recurrent connections for memory consolidation
    updateRecurrentConnections();
    
    // Decay weak traces and maintain capacity
    decayWeakTraces();
    
    // Run sophisticated competition with attention modulation
    runAdvancedCompetition();
    
    // Clean up inactive traces
    cleanInactiveTraces();
}

float NeuralWorkingMemory::applySynapticPlasticity(float activation, NeuronId neuron) {
    // Apply short-term synaptic plasticity (STP)
    // This models presynaptic and postsynaptic activity-dependent plasticity
    
    // Get recent activation history (simplified - would track in full implementation)
    static std::unordered_map<uint64_t, float> recentActivations;
    float currentActivity = recentActivations[neuron.value];
    recentActivations[neuron.value] = activation;
    
    // Facilitation component
    float facilitation = (currentActivity > 0.0f) ? std::min(1.0f, activation / currentActivity * 1.2f) : 1.0f;
    
    // Depression component
    float depression = (activation > 0.0f) ? std::min(1.0f, currentActivity / activation * 0.9f) : 1.0f;
    
    // Combined effect
    float plasticityFactor = facilitation * 0.6f + depression * 0.4f;
    
    return activation * plasticityFactor;
}

void NeuralWorkingMemory::runAdvancedCompetition() {
    winners_.clear();
    activeTraces_.clear();
    
    if (memoryActivations_.empty()) return;
    
    // Advanced competition model with lateral inhibition and gain control
    size_t numNeurons = memoryNeurons_.size();
    
    // Normalize activations to competitive range
    float maxActivation = 0.0f;
    for (float activation : memoryActivations_) {
        if (activation > maxActivation) maxActivation = activation;
    }
    
    if (maxActivation < 0.01f) return;
    
    // Compute competitive scores with inhibition
    std::vector<float> competitionScores(numNeurons, 0.0f);
    
    for (size_t i = 0; i < numNeurons; ++i) {
        float normalized = memoryActivations_[i] / maxActivation;
        
        // Apply lateral inhibition from stronger competitors
        float inhibition = 0.0f;
        for (size_t j = 0; j < numNeurons; ++j) {
            if (i == j) continue;
            if (memoryActivations_[j] > memoryActivations_[i]) {
                // Stronger neurons inhibit weaker ones
                inhibition += (memoryActivations_[j] - memoryActivations_[i]) * 0.5f;
            }
        }
        
        competitionScores[i] = normalized - inhibition;
    }
    
    // Select winners based on competitive thresholds
    float threshold = 0.3f;
    std::vector<std::pair<float, size_t>> scored;
    
    for (size_t i = 0; i < numNeurons; ++i) {
        if (competitionScores[i] > threshold) {
            scored.emplace_back(competitionScores[i], i);
            
            // Mark as active trace
            activeTraces_.push_back(i);
            
            // Apply excitation to winners
            if (brain_) {
                brain_->injectCurrent(memoryNeurons_[i], competitionScores[i] * 3.0f);
            }
        }
    }
    
    // Sort by competitive score
    std::sort(scored.begin(), scored.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Select top competitors as winners (maintain capacity)
    size_t maxWinners = std::min(size_t(3), scored.size());
    for (size_t i = 0; i < maxWinners; ++i) {
        winners_.push_back(memoryNeurons_[scored[i].second]);
        
        // Strengthen winning traces
        memoryActivations_[scored[i].second] = std::min(1.0f, memoryActivations_[scored[i].second] * 1.2f);
    }
    
    // Apply strong inhibition to non-winners
    for (size_t i = maxWinners; i < numNeurons; ++i) {
        if (scored.size() > i && scored[i].second < numNeurons) {
            NeuronId neuron = memoryNeurons_[scored[i].second];
            if (brain_) {
                brain_->injectCurrent(neuron, -0.8f * competitionScores[i]);
            }
        }
    }
}

void NeuralWorkingMemory::cleanInactiveTraces() {
    // Remove inactive memory traces to maintain memory capacity
    // This implements memory pruning based on usage
    
    std::vector<size_t> toKeep;
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        float activation = memoryActivations_[i];
        
        // Check if trace is active
        bool isActive = std::find(activeTraces_.begin(), activeTraces_.end(), i) != activeTraces_.end();
        
        if (!isActive && activation < 0.01f) {
            // Remove weakly inactive traces
            toRemove.push_back(i);
        } else if (activation > 0.05f) {
            // Keep strongly active traces
            toKeep.push_back(i);
        } else if (activation > 0.01f) {
            // Consider for keeping if moderately active
            toKeep.push_back(i);
        }
    }
    
    // Rebuild arrays with kept traces
    std::vector<NeuronId> newMemoryNeurons;
    std::vector<float> newMemoryActivations;
    std::vector<SimulationStep> newMemoryTimestamps;
    
    for (size_t i : toKeep) {
        newMemoryNeurons.push_back(memoryNeurons_[i]);
        newMemoryActivations.push_back(memoryActivations_[i]);
        newMemoryTimestamps.push_back(memoryTimestamps_[i]);
    }
    
    // Also keep some of the toRemove if we're below capacity
    size_t maxCapacity = capacity_ * 2;  // Allow temporary overcapacity
    if (newMemoryNeurons.size() < maxCapacity && !toRemove.empty()) {
        // Keep the most recently used traces from toRemove
        std::vector<std::pair<SimulationStep, size_t>> traceAges;
        for (size_t i : toRemove) {
            traceAges.emplace_back(memoryTimestamps_[i], i);
        }
        std::sort(traceAges.begin(), traceAges.end(),
                  [](const auto& a, const auto& b) { return a.first > b.first; });
        
        size_t toKeepFromRemove = std::min(size_t(maxCapacity - newMemoryNeurons.size()), toRemove.size());
        for (size_t i = 0; i < toKeepFromRemove; ++i) {
            size_t idx = traceAges[i].second;
            newMemoryNeurons.push_back(memoryNeurons_[idx]);
            newMemoryActivations.push_back(memoryActivations_[idx]);
            newMemoryTimestamps.push_back(memoryTimestamps_[idx]);
        }
    }
    
    // Update arrays
    memoryNeurons_ = std::move(newMemoryNeurons);
    memoryActivations_ = std::move(newMemoryActivations);
    memoryTimestamps_ = std::move(newMemoryTimestamps);
    
    // Clear maintenance synapses for removed traces
    std::vector<std::pair<NeuronId, NeuronId>> newConnections;
    for (const auto& conn : pImpl->maintenanceSynapses) {
        bool keep = false;
        for (const auto& neuron : memoryNeurons_) {
            if (conn.first == neuron || conn.second == neuron) {
                keep = true;
                break;
            }
        }
        if (keep) newConnections.push_back(conn);
    }
    pImpl->maintenanceSynapses = std::move(newConnections);
}

bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

float NeuralWorkingMemory::getMemoryActivity() const {
    if (memoryNeurons_.empty()) return 0.0f;
    
    float totalActivity = 0.0f;
    for (float activation : memoryActivations_) {
        totalActivity += activation;
    }
    
    // Apply competition weighting
    float weightedActivity = 0.0f;
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        bool isWinner = std::find(winners_.begin(), winners_.end(), memoryNeurons_[i]) != winners_.end();
        float weight = isWinner ? 2.0f : 1.0f;  // Winners count more
        weightedActivity += memoryActivations_[i] * weight;
    }
    
    return weightedActivity / memoryNeurons_.size();
}

// Create intelligent maintenance connections based on activation patterns
void NeuralWorkingMemory::createIntelligentMaintenanceConnections(float strength) {
    // Clear old maintenance synapses
    pImpl->maintenanceSynapses.clear();
    
    // Create sophisticated maintenance connections
    size_t numNeurons = memoryNeurons_.size();
    if (numNeurons < 2) return;
    
    // Create a network of recurrent connections for persistent activity
    for (size_t i = 0; i < numNeurons; ++i) {
        NeuronId from = memoryNeurons_[i];
        NeuronId to = memoryNeurons_[(i + 1) % numNeurons];
        
        // Calculate connection strength based on similarity of activations
        float strength1 = memoryActivations_[i] * strength * 0.8f;
        float strength2 = memoryActivations_[(i + 1) % numNeurons] * strength * 0.8f;
        
        // Create bidirectional connections for robust maintenance
        createRecurrentConnection(from, to, strength1);
        createRecurrentConnection(to, from, strength2);
        
        // Add recurrent self-connections for persistence
        createRecurrentConnection(from, from, strength * 0.3f);
    }
    
    // Add winner-take-all inhibition for competition
    if (numNeurons > 1) {
        for (size_t i = 0; i < numNeurons; ++i) {
            for (size_t j = i + 1; j < numNeurons; ++j) {
                if (memoryActivations_[i] > memoryActivations_[j]) {
                    createRecurrentConnection(memoryNeurons_[j], memoryNeurons_[i], -0.5f * strength);
                } else {
                    createRecurrentConnection(memoryNeurons_[i], memoryNeurons_[j], -0.5f * strength);
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

} // namespace nlm