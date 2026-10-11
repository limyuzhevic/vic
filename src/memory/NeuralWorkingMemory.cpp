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

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength,
                               const AttentionalSelection& attention) {
    // Store pattern with attention guidance - enhanced version
    if (pattern.empty() || !brain_) return;
    
    // Use attention to guide which patterns to store
    std::vector<size_t> patternIndices;
    for (size_t i = 0; i < pattern.size(); ++i) {
        // Get attention strength for this neuron if we have neuron IDs
        float attentionWeight = 1.0f;
        
        // In a real implementation, we would map pattern indices to neuron IDs
        // and use the attention system to get actual attention weights
        
        // Store based on attention-weighted importance
        float weightedActivation = pattern[i] * strength * attentionWeight;
        
        if (weightedActivation > 0.01f) {  // Minimum threshold
            patternIndices.push_back(i);
        }
    }
    
    // Store selected patterns with attention guidance
    if (!patternIndices.empty()) {
        // Create enhanced storage with attention-modulated strengths
        for (size_t idx : patternIndices) {
            float activation = pattern[idx] * strength * 1.5f;  // Attention boost
            
            // Store to working memory with enhanced attention integration
            if (brain_) {
                brain_->injectCurrent(memoryNeurons_[idx % memoryNeurons_.size()], 
                    activation * 5.0f);
            }
        }
    }
}

void NeuralWorkingMemory::storeWithPrediction(const std::vector<float>& pattern,
                                              float strength,
                                              const PredictionSystem& prediction,
                                              Brain* brain) {
    // Store pattern with prediction guidance - enhanced version
    if (pattern.empty() || !brain) return;
    
    // Get prediction error
    float predictionError = prediction.getPredictionError();
    
    // Use prediction error to select patterns to store
    std::vector<size_t> patternIndices;
    for (size_t i = 0; i < pattern.size(); ++i) {
        // Store patterns with high prediction error more strongly
        float predictionWeight = 1.0f + predictionError * 2.0f;
        
        // Store based on prediction-weighted importance
        float weightedActivation = pattern[i] * strength * predictionWeight;
        
        // Check if this pattern is predicted well (low error) or poorly (high error)
        if (weightedActivation > 0.01f) {
            patternIndices.push_back(i);
        }
    }
    
    // Store selected patterns
    if (!patternIndices.empty()) {
        // Create temporary pattern with only selected indices
        std::vector<float> selectedPattern;
        for (size_t idx : patternIndices) {
            selectedPattern.push_back(pattern[idx]);
        }
        
        if (!selectedPattern.empty()) {
            store(selectedPattern, strength);
            
            // Update working memory with prediction-based consolidation
            for (size_t idx : patternIndices) {
                if (idx < selectedPattern.size() && brain) {
                    float predictionActivation = selectedPattern[idx] * strength * 1.2f;
                    // Store to working memory with prediction error influence
                    storeToNeuron(memoryNeurons_[idx % memoryNeurons_.size()], predictionActivation);
                }
            }
        }
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
    
    // Check if brain is in sleep state
    bool isSleeping = brain_->getSleepManager() && 
                      brain_->getSleepManager()->isSleeping();
    
    // Update maintenance - reinforce active memory neurons
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > 0.1f) {
            if (isSleeping) {
                // Sleep-based memory consolidation - enhanced retention
                // In deep sleep, working memory traces are strengthened
                float sleepConsolidation = 1.0f + 0.3f * (isSleeping ? 1.0f : 0.0f);
                activation *= sleepConsolidation;
                
                // Less decay during sleep
                brain_->injectCurrent(neuron, activation * 1.5f);
                memoryTimestamps_[i]++;
                
                // Check if trace is too old (slower decay during sleep)
                if (memoryTimestamps_[i] > 2000) {
                    activation *= (1.0f - decayRate_ * 0.5f);
                }
                
                memoryActivations_[i] = activation;
            } else {
                // Normal wake state processing
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
    }
    
    // Decay weak traces
    decayWeakTraces();
    
    // Run competition to select winners
    runCompetition();
    
    // Apply sleep-based consolidation
    if (isSleeping) {
        applySleepConsolidation();
    }
}

void NeuralWorkingMemory::update(TimestepDuration dt, const AttentionalSelection& attention) {
    // Enhanced working memory update with attentional modulation
    if (!brain_) return;
    
    // Update maintenance - reinforce active memory neurons with attentional boost
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > 0.1f) {
            // Apply attentional modulation to memory maintenance
            float attentionMod = attention.getAttentionStrength(neuron);
            activation *= (1.0f + attentionMod * 0.5f);
            
            // Inject maintenance current with attentional boost
            brain_->injectCurrent(neuron, activation * 2.0f);
            
            // Age the trace with attentional weighting
            memoryTimestamps_[i] += (attentionMod > 0.5f ? 2 : 1);
            
            // Check if trace is too old
            if (memoryTimestamps_[i] > 1000) {
                activation *= (1.0f - decayRate_);
            }
            
            memoryActivations_[i] = activation;
        }
    }
    
    // Decay weak traces
    decayWeakTraces();
    
    // Run competition with attentional guidance
    runCompetition();
    
    // Apply attention-based memory consolidation
    auto winners = attention.getWinners();
    for (const auto& winner : winners) {
        // Consolidate attended memories
        if (contains(winner)) {
            float boostFactor = 1.5f + attention.getAttentionStrength(winner) * 0.5f;
            float idx = findNeuronIndex(winner);
            if (idx >= 0) {
                memoryActivations_[idx] = std::min(1.0f, memoryActivations_[idx] * boostFactor);
            }
        }
    }
}

void NeuralWorkingMemory::consolidateMemory(const AttentionalSelection& attention, 
                                            const PredictionSystem& prediction) {
    // Consolidate working memory based on attention and prediction
    if (memoryNeurons_.empty()) return;
    
    // Use attention to identify important memories
    auto winners = attention.getWinners();
    
    // Use prediction error to identify predictive memories
    float predictionError = prediction.getPredictionError();
    
    // Apply consolidation: reinforce attended/predictively useful memories
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float& activation = memoryActivations_[i];
        
        bool isAttended = std::find(winners.begin(), winners.end(), neuron) != winners.end();
        
        if (isAttended) {
            // Strengthen attended memories
            activation = std::min(1.0f, activation * 1.3f);
        }
        
        if (predictionError > 0.1f) {
            // For high prediction error, store this pattern as predictive
            activation = std::min(1.0f, activation * 1.2f);
        }
        
        // Decay non-attended memories faster
        if (!isAttended) {
            activation *= (1.0f - decayRate_ * 1.5f);
        }
    }
}

void NeuralWorkingMemory::matchPattern(const std::vector<float>& pattern,
                                       float threshold,
                                       std::vector<NeuronId>& matches) const {
    // Pattern matching in working memory
    if (pattern.empty() || memoryActivations_.empty()) return;
    
    matches.clear();
    
    // Simple pattern matching: compare patterns with current memory
    for (size_t i = 0; i < pattern.size() && i < memoryActivations_.size(); ++i) {
        float similarity = std::abs(pattern[i] - memoryActivations_[i]);
        
        if (similarity < threshold && !memoryNeurons_.empty()) {
            matches.push_back(memoryNeurons_[i]);
        }
    }
}

void NeuralWorkingMemory::applyTopDownModulation(const AttentionalSelection& attention,
                                                  const PredictionSystem& prediction) {
    // Apply top-down modulation to working memory
    if (!brain_) return;
    
    // Get attention winners and prediction guidance
    auto winners = attention.getWinners();
    float predictionError = prediction.getPredictionError();
    
    // Apply attention bias to working memory patterns
    for (const auto& winner : winners) {
        float attentionBias = attention.getAttentionStrength(winner);
        
        if (contains(winner)) {
            float idx = findNeuronIndex(winner);
            if (idx >= 0) {
                // Apply top-down bias to attended memories
                memoryActivations_[idx] = std::min(1.0f, 
                    memoryActivations_[idx] + attentionBias * 0.2f);
            }
        }
    }
    
    // Apply prediction error to working memory updates
    if (predictionError > 0.2f) {
        // High prediction error triggers memory reorganization
        float reorganizationStrength = predictionError * 0.3f;
        
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            // Reorganize memories based on prediction error
            memoryActivations_[i] *= (1.0f + reorganizationStrength);
            memoryActivations_[i] = std::min(1.0f, memoryActivations_[i]);
        }
    }
}

void NeuralWorkingMemory::update(TimestepDuration dt,
                               const AttentionalSelection& attention,
                               const PredictionSystem& prediction) {
    // Comprehensive working memory update with attention and prediction integration
    if (!brain_) return;
    
    // Apply attention-modulated maintenance
    update(dt, attention);
    
    // Apply top-down modulation
    applyTopDownModulation(attention, prediction);
    
    // Consolidate memory based on attention and prediction
    consolidateMemory(attention, prediction);
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

size_t NeuralWorkingMemory::findNeuronIndex(NeuronId neuron) const {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        return std::distance(memoryNeurons_.begin(), it);
    }
    return -1;
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

} // namespace nlm