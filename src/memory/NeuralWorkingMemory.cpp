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
    , persistentActivityThreshold_(0.8f)
    , maintenanceStrength_(2.5f)
    , replayEnabled_(true)
    , consolidationEnabled_(true)
{
    // Initialize active trace tracking
    pImpl->traceAges.clear();
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize new fields with biologically plausible defaults
    pImpl->traceAges.clear();
    pImpl->traceAges.resize(capacity_, 0);
    
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
        
        // Update stored activation and timestamp
        if (i < memoryActivations_.size()) {
            memoryActivations_[i] = activation;
            if (i < pImpl->traceAges.size()) {
                pImpl->traceAges[i] = 0;  // Reset age on storage
            }
        } else {
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
            memoryNeurons_.push_back(neuron);
            pImpl->traceAges.push_back(0);
        }
    }
    
    // Create stronger maintenance connections based on pattern similarity
    for (size_t i = 1; i < memoryNeurons_.size(); ++i) {
        // Create biologically plausible recurrent connections
        float connStrength = strength * 0.5f;
        
        // Add synaptic efficacy based on activation levels
        if (i < memoryActivations_.size() && (i-1) < memoryActivations_.size()) {
            float avgActivation = (memoryActivations_[i] + memoryActivations_[i-1]) * 0.5f;
            if (avgActivation > 0.5f) {
                // Stronger connections for well-activated patterns
                connStrength *= (1.0f + avgActivation);
            }
        }
        
        createRecurrentConnection(memoryNeurons_[i-1], memoryNeurons_[i], connStrength);
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
        if (idx < pImpl->traceAges.size()) {
            pImpl->traceAges[idx] = 0;  // Reset age on storage
        }
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
        pImpl->traceAges.push_back(0);
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
    
    // Update maintenance - reinforce active memory neurons with adaptive strength
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > 0.1f) {
            // Adaptive maintenance based on trace age and activation level
            float maintenanceCurrent = activation * maintenanceStrength_;
            
            // Enhance maintenance for older traces to prevent decay
            if (pImpl->traceAges[i] > 100) {
                maintenanceCurrent *= 1.5f;  // Boost maintenance for older traces
            }
            
            // Apply maintenance current
            brain_->injectCurrent(neuron, maintenanceCurrent);
            
            // Age the trace with more sophisticated tracking
            pImpl->traceAges[i]++;
            memoryTimestamps_[i]++;
            
            // Check if trace needs consolidation (based on activation strength and age)
            if (consolidationEnabled_ && activation > persistentActivityThreshold_ && pImpl->traceAges[i] > 500) {
                // Check if episodic memory is available for consolidation
                if (brain_->getEpisodicMemory()) {
                    // Mark this trace for consolidation
                    pImpl->traceAges[i] |= 0x80000000;  // Use MSB as consolidation flag
                }
            }
            
            // Enhanced decay for very weak traces
            if (activation < 0.01f && pImpl->traceAges[i] > 50) {
                activation *= (1.0f - decayRate_ * 2.0f);  // Faster decay for weak traces
            } else {
                // Normal decay for moderate traces
                activation *= (1.0f - decayRate_);
            }
            
            memoryActivations_[i] = activation;
        }
    }
    
    // Update recurrent connections for better maintenance
    updateRecurrentConnections();
    
    // Decay weak traces with improved criteria
    decayWeakTraces();
    
    // Run competition to select winners with adaptive threshold
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
}

// New methods for enhanced working memory

void NeuralWorkingMemory::rehearseMemories() {
    if (!replayEnabled_ || !brain_) return;
    
    // Rehearsal: reactivate recent memories to strengthen them
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        // Only rehearse recent memories (within last 1000 steps)
        if (pImpl->traceAges[i] < 1000) {
            float activation = memoryActivations_[i];
            if (activation > 0.01f) {
                // Strong rehearsal boost
                float rehearsalCurrent = activation * 3.0f;
                brain_->injectCurrent(memoryNeurons_[i], rehearsalCurrent);
                
                // Additional boost for reinforcement learning
                if (pImpl->traceAges[i] < 100) {
                    rehearsalCurrent *= 2.0f;  // Stronger boost for very recent traces
                }
                
                // Apply rehearsal
                brain_->injectCurrent(memoryNeurons_[i], rehearsalCurrent);
                
                // Boost activation
                memoryActivations_[i] = std::min(1.0f, activation * 1.2f);
            }
        }
    }
}

void NeuralWorkingMemory::consolidateToEpisodic() {
    if (!consolidationEnabled_ || !brain_ || !brain_->getEpisodicMemory()) return;
    
    // Find traces that need consolidation
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (needsConsolidation(i)) {
            float activation = memoryActivations_[i];
            if (activation > 0.1f) {
                // Convert to episodic memory item
                EpisodicMemoryItem episode;
                episode.timestamp = pImpl->traceAges[i];
                episode.reward = 0.0f;  // Would be updated by reward signals
                
                // Add neuron to episode
                episode.activeNeurons.push_back(memoryNeurons_[i]);
                episode.neuronActivations.push_back(activation);
                
                // Store in episodic memory
                brain_->getEpisodicMemory()->storeEpisode(episode);
                
                // Remove from working memory after consolidation
                memoryNeurons_.erase(memoryNeurons_.begin() + i);
                memoryActivations_.erase(memoryActivations_.begin() + i);
                memoryTimestamps_.erase(memoryTimestamps_.begin() + i);
                pImpl->traceAges.erase(pImpl->traceAges.begin() + i);
                
                // Adjust indices for next iteration
                i--;
            }
        }
    }
}

bool NeuralWorkingMemory::needsConsolidation(size_t traceIndex) const {
    if (traceIndex >= pImpl->traceAges.size() || traceIndex >= memoryNeurons_.size()) {
        return false;
    }
    
    size_t age = pImpl->traceAges[traceIndex];
    float activation = memoryActivations_[traceIndex];
    
    // Consolidate if trace is old enough AND strongly activated
    return (age > 500 && activation > 0.2f) || (age > 1000 && activation > 0.1f);
}

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