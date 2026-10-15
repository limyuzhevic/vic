#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Maintenance connections (recurrent)
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    
    // Memory trace ages
    std::vector<SimulationStep> traceAges;
    
    // Efficient neuron lookup for O(1) access to neuron pointers
    std::unordered_map<uint64_t, Neuron*> neuronLookup;
    
    Impl() : brain(nullptr) {}
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , attention_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
{
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Build efficient neuron lookup table
    pImpl->neuronLookup.clear();
    if (brain_) {
        const auto& regions = brain_->getAllRegions();
        for (const auto& region : regions) {
            if (region && region->getAllNeurons()) {
                for (const auto* neuron : *(region->getAllNeurons())) {
                    if (neuron) {
                        pImpl->neuronLookup[neuron->getId().value] = const_cast<Neuron*>(neuron);
                    }
                }
            }
        }
    }
    
    NLM_LOG_INFO("NeuralWorkingMemory initialized");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Find neurons to encode this pattern
    size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
        float activation = pattern[i] * strength;
        
        // Use efficient neuron lookup - O(1) instead of O(n) iteration
        auto neuronIt = pImpl->neuronLookup.find(neuron.value);
        if (neuronIt != pImpl->neuronLookup.end()) {
            neuronIt->second->injectCurrent(activation * 5.0f);
        } else {
            // Fallback to old method if lookup fails
            if (auto* region = brain_->getRegion(neuron.getId() / 1000)) {
                if (auto* neurons = region->getAllNeurons()) {
                    for (auto* nn : *neurons) {
                        if (nn->getId() == neuron) {
                            nn->injectCurrent(activation * 5.0f);
                            break;
                        }
                    }
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
    // Find or add this neuron to memory using efficient lookup
    auto it = pImpl->neuronLookup.find(neuron.value);
    
    if (it != pImpl->neuronLookup.end()) {
        // Use lookup to find index - more efficient than linear search
        auto neuronIt = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
        if (neuronIt != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), neuronIt);
            memoryActivations_[idx] = activation;
            memoryTimestamps_[idx] = 0;
            // Add to active traces if not already present
            if (std::find(activeTraces_.begin(), activeTraces_.end(), idx) == activeTraces_.end()) {
                activeTraces_.push_back(idx);
            }
        } else if (memoryNeurons_.size() < capacity_) {
            memoryNeurons_.push_back(neuron);
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
            // Add to active traces
            activeTraces_.push_back(memoryNeurons_.size() - 1);
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
    // Use efficient O(1) lookup instead of O(n) linear search
    return pImpl->neuronLookup.find(neuron.value) != pImpl->neuronLookup.end();
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    // Use efficient lookup instead of linear search
    auto neuronIt = pImpl->neuronLookup.find(neuron.value);
    if (neuronIt != pImpl->neuronLookup.end()) {
        auto memIt = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
        if (memIt != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), memIt);
            return memoryActivations_[idx];
        }
    }
    return 0.0f;
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Phase 1: Hebbian consolidation - strengthen connections between co-active neurons
    consolidateHebbianConnections();
    
    // Phase 2: Update maintenance - reinforce active memory neurons with adaptive decay
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > 0.1f) {
            // Adaptive maintenance current - depends on memory age
            float maintenanceStrength = 2.0f;
            if (memoryTimestamps_[i] > 500) {
                maintenanceStrength *= 1.5f; // Boost maintenance for older memories
            }
            brain_->injectCurrent(neuron, activation * maintenanceStrength);
            
            // Apply synaptic normalization to prevent runaway excitation
            normalizeNeuronActivity(neuron, activation);
            
            // Adaptive age tracking with potential for memory reactivation
            if (memoryTimestamps_[i] < 100) {
                // Young traces benefit from Hebbian reinforcement
                reinforceHebbianTrace(neuron, activation);
            }
            
            memoryTimestamps_[i]++;
            
            // Adaptive decay - depends on activation level and age
            if (memoryTimestamps_[i] > getAdaptiveDecayThreshold(activation)) {
                // Age-dependent decay - older, stronger memories decay slower
                float ageFactor = std::min(1.0f, memoryTimestamps_[i] / 2000.0f);
                activation *= (1.0f - decayRate_ * (1.0f + ageFactor * 2.0f));
                
                // If activation is too low, reset age for potential strengthening
                if (activation < 0.01f) {
                    memoryTimestamps_[i] = 0;
                }
            }
            
            memoryActivations_[i] = activation;
        }
    }
    
    // Update recurrent connections for maintenance and memory consolidation
    updateRecurrentConnections();
    
    // Decay weak traces with adaptive threshold
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
    pImpl->neuronLookup.clear();
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    for (auto& activation : memoryActivations_) {
        activation = std::min(1.0f, activation * factor);
    }
}

void NeuralWorkingMemory::runCompetition() {
    // Reset winners
    winners_.clear();
    
    if (memoryNeurons_.empty()) return;
    
    // Biological competition with lateral inhibition and attention modulation
    std::vector<float> competitionScores(memoryNeurons_.size(), 0.0f);
    
    // Phase 1: Compute initial competition scores based on activation levels
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        competitionScores[i] = memoryActivations_[i];
        
        // Apply activity-dependent modulation - stronger neurons get amplified
        if (memoryActivations_[i] > 0.5f) {
            competitionScores[i] *= (1.0f + memoryActivations_[i]);
        }
    }
    
    // Phase 2: Implement lateral inhibition between competing traces
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (competitionScores[i] < 0.05f) continue; // Skip weak traces
        
        // Find strongest competitor within local neighborhood
        size_t bestCompetitor = i;
        float bestScore = competitionScores[i];
        
        for (size_t j = 0; j < memoryNeurons_.size(); ++j) {
            if (i == j || competitionScores[j] < 0.05f) continue;
            
            // Calculate competitive advantage (more biologically plausible than simple comparison)
            float relativeScore = competitionScores[i] / (competitionScores[j] + 0.001f);
            if (relativeScore > 1.2f) { // i is significantly stronger than j
                bestCompetitor = i;
                bestScore = competitionScores[i];
                
                // Apply inhibition to weaker competitor
                if (brain_) {
                    // Stronger inhibition for closer competitors (simulates surround suppression)
                    float inhibition = 0.5f * (1.0f - std::min(1.0f, std::abs(static_cast<int>(i - j)) / 10.0f));
                    brain_->injectCurrent(memoryNeurons_[j], -memoryActivations_[j] * inhibition * 3.0f);
                }
                break;
            }
        }
        
        // If this is the best competitor, select it as winner
        if (i == bestCompetitor) {
            winners_.push_back(memoryNeurons_[i]);
            
            // Apply winner excitation (positive feedback)
            if (brain_) {
                brain_->injectCurrent(memoryNeurons_[i], memoryActivations_[i] * 2.0f);
            }
        }
    }
    
    // Phase 3: Re-select winners based on final competitive state
    if (winners_.size() > 1) {
        std::vector<NeuronId> newWinners;
        float maxScore = 0.0f;
        
        for (const auto& winner : winners_) {
            auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), winner);
            if (it != memoryNeurons_.end()) {
                size_t idx = std::distance(memoryNeurons_.begin(), it);
                if (memoryActivations_[idx] > maxScore) {
                    maxScore = memoryActivations_[idx];
                    newWinners.clear();
                    newWinners.push_back(winner);
                } else if (memoryActivations_[idx] > maxScore * 0.8f) {
                    // Allow near-winners to compete for top spots
                    newWinners.push_back(winner);
                }
            }
        }
        
        winners_ = newWinners;
    }
    
    // Phase 4: Ensure at least one winner if memory is active
    if (winners_.empty()) {
        size_t bestIdx = 0;
        float bestAct = memoryActivations_[0];
        
        for (size_t i = 1; i < memoryNeurons_.size(); ++i) {
            if (memoryActivations_[i] > bestAct) {
                bestAct = memoryActivations_[i];
                bestIdx = i;
            }
        }
        
        if (bestAct > 0.01f) {
            winners_.push_back(memoryNeurons_[bestIdx]);
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

void NeuralWorkingMemory::consolidateHebbianConnections() {
    // Strengthen connections between neurons that fire together (Hebbian learning)
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryActivations_[i] < 0.3f) continue; // Only consolidate strong traces
        
        for (size_t j = i + 1; j < memoryNeurons_.size(); ++j) {
            if (memoryActivations_[j] < 0.3f) continue;
            
            // Calculate co-activation strength (simplified Hebbian rule: neurons that fire together wire together)
            float coActivation = memoryActivations_[i] * memoryActivations_[j];
            
            // Create or strengthen recurrent connection with weight proportional to co-activation
            createRecurrentConnection(memoryNeurons_[i], memoryNeurons_[j], coActivation * 0.1f);
        }
    }
}

void NeuralWorkingMemory::normalizeNeuronActivity(NeuronId neuron, float activation) {
    // Prevent runaway excitation through homeostatic regulation
    float normalizedAct = std::min(1.0f, activation * 1.2f);
    
    // Apply normalization current to maintain stability
    if (brain_) {
        brain_->injectCurrent(neuron, -normalizedAct * 0.3f);
    }
}

void NeuralWorkingMemory::reinforceHebbianTrace(NeuronId neuron, float activation) {
    // Enhance young traces through Hebbian reinforcement
    if (brain_) {
        // Apply additional excitation to strengthen trace
        brain_->injectCurrent(neuron, activation * 0.8f);
        
        // Increase trace age counter for potential decay
        auto memIt = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
        if (memIt != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), memIt);
            memoryTimestamps_[idx] = std::min(memoryTimestamps_[idx] + 10, 1000);
        }
    }
}

size_t NeuralWorkingMemory::getAdaptiveDecayThreshold(float activation) const {
    // Calculate decay threshold based on activation level
    // Stronger memories decay slower
    float threshold = 500.0f * (1.0f - activation);
    return static_cast<size_t>(threshold) + 500;
}

float NeuralWorkingMemory::getActivityThreshold() const {
    // Dynamic threshold based on memory density
    float baseThreshold = 0.01f;
    if (!memoryNeurons_.empty()) {
        // Higher density -> lower threshold for pruning
        float densityFactor = std::min(1.0f, memoryNeurons_.size() / 50.0f);
        baseThreshold *= (1.0f - densityFactor * 0.5f);
    }
    return baseThreshold;
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Apply maintenance currents through recurrent connections
    for (const auto& conn : pImpl->maintenanceSynapses) {
        float fromActivation = 0.0f;
        
        // Find from neuron activation using efficient lookup
        auto neuronIt = pImpl->neuronLookup.find(conn.first.value);
        if (neuronIt != pImpl->neuronLookup.end()) {
            auto memIt = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.first);
            if (memIt != memoryNeurons_.end()) {
                size_t idx = std::distance(memoryNeurons_.begin(), memIt);
                fromActivation = memoryActivations_[idx];
            }
        }
        
        if (fromActivation > 0.1f && brain_) {
            // Send maintenance signal
            brain_->injectCurrent(conn.second, fromActivation * 2.0f);
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
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , winnerTakeAllMode_(false)
    , attentionalScope_(1.0f)
    , attentionBlinkEnabled_(false)
    , attentionBlinkDuration_(10.0f)
    , attentionalFatigue_(0.0f)
    , attentionalRecoveryRate_(0.05f)
    , attentionalFatigueThreshold_(0.8f)
    , selectionTimeWindow_(100.0f)
    , temporalIntegrationDecay_(0.01f)
{
    // Initialize neural dynamics history vectors
    neuronHistory_.resize(100, 0.0f);  // Max 100 competing neurons
    inhibitionHistory_.resize(100, 0.0f);
    excitationHistory_.resize(100, 0.0f);
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl = std::make_unique<Impl>();
    brain_ = brain;
    
    // Note: Fields are already initialized in constructor
    NLM_LOG_INFO("AttentionalSelection initialized with enhanced biological mechanisms");
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                               float globalInhibition) {
    winners_.clear();
    
    if (competitors.empty()) return winners_;
    
    // Initialize enhanced competition state
    std::vector<float> competitionStrength(competitors.size(), 0.0f);
    std::vector<float> inhibitionLevel(competitors.size(), 0.0f);
    std::vector<float> noise(competitors.size(), 0.0f);
    
    // Phase 1: Initial selection - compute base competitive strengths
    float totalActivity = 0.0f;
    for (size_t i = 0; i < competitors.size(); ++i) {
        NeuronId neuron = competitors[i];
        
        // Get bottom-up salience with neuromodulation integration
        auto salIt = bottomUpSalience_.find(neuron.value);
        float baseSalience = (salIt != bottomUpSalience_.end()) ? salIt->second : 0.0f;
        
        // Get top-down bias with neuromodulation modulation
        auto biasIt = topDownBias_.find(neuron.value);
        float baseBias = (biasIt != topDownBias_.end()) ? biasIt->second : 0.0f;
        
        // Integrate working memory activation as competitive input
        float wmActivation = 0.0f;
        auto wmIt = std::find(workingMemoryNeurons_.begin(), workingMemoryNeurons_.end(), neuron);
        if (wmIt != workingMemoryNeurons_.end()) {
            size_t wmIdx = std::distance(workingMemoryNeurons_.begin(), wmIt);
            if (wmIdx < workingMemoryActivations_.size()) {
                wmActivation = workingMemoryActivations_[wmIdx];
            }
        }
        
        // Compute initial competitive strength with biological realism
        float noiseMagnitude = 0.02f * (1.0f + attentionalFatigue_ / attentionalFatigueThreshold_);
        float neuronNoise = (rand() / (RAND_MAX + 1.0f)) * 2.0f * noiseMagnitude - noiseMagnitude;
        
        // Neuromodulatory effects on selection strength
        float neuromodulationFactor = 1.0f + dopamineLevel_ * 0.3f + noveltyLevel_ * 0.2f;
        
        float strength = (baseSalience * 0.4f) + 
                        (baseBias * 0.3f) + 
                        (wmActivation * 0.3f);
        
        // Apply neuromodulation
        strength *= neuromodulationFactor;
        
        // Add biological noise for realistic variability
        strength += neuronNoise;
        
        // Apply attentional scope (local competition within focus area)
        if (attentionalScope_ < 1.0f) {
            // Local competition: only neurons within scope are considered
            // For simplicity, we assume all competitors are within the attentional scope
            // In a real implementation, this would be based on spatial relationships
        }
        
        competitionStrength[i] = std::max(0.0f, strength);
        totalActivity += competitionStrength[i];
        noise[i] = std::abs(neuronNoise);
    }
    
    // Phase 2: Inhibition - implement local lateral inhibition
    for (size_t i = 0; i < competitors.size(); ++i) {
        if (competitionStrength[i] < 0.01f) continue;
        
        // Local inhibition from nearby competitors within attentional scope
        for (size_t j = 0; j < competitors.size(); ++j) {
            if (i == j) continue;
            
            // Calculate distance for local inhibition (simplified - could use spatial coordinates)
            float distance = static_cast<float>(std::abs(static_cast<int>(i) - static_cast<int>(j)));
            distance = std::min(distance, attentionalScope_ * 10.0f) / (attentionalScope_ * 10.0f);
            
            // Inhibition strength depends on distance and relative activity
            float relativeStrength = competitionStrength[i] / (competitionStrength[j] + 0.001f);
            float inhibition = globalInhibition * inhibitionStrength_;
            
            // Decay with distance (local competition)
            inhibition *= (1.0f - distance);
            
            if (relativeStrength > 1.2f) {
                // Apply inhibition to weaker neuron
                inhibitionLevel[j] += inhibition;
                
                // Simulate neural inhibition through brain interface
                if (brain_) {
                    float inhibitionCurrent = -inhibition * 2.0f;
                    // Apply additional modulation based on attentional fatigue
                    if (attentionalFatigue_ > 0.5f) {
                        inhibitionCurrent *= (1.0f + attentionalFatigue_);
                    }
                    brain_->injectCurrent(competitors[j], inhibitionCurrent);
                }
            }
        }
    }
    
    // Phase 3: Final selection with multi-phase competition
    // Adjust thresholds based on context and neuromodulation
    float dynamicThreshold = competitionThreshold_ * (1.0f + noveltyLevel_ * 0.5f);
    
    // Apply attentional blink effects if enabled
    if (attentionBlinkEnabled_) {
        // During attention blink, reduce selection sensitivity
        dynamicThreshold *= 1.5f;
    }
    
    // Apply fatigue effects
    if (attitionalFatigue_ > attentionalFatigueThreshold_) {
        // High fatigue reduces selection precision
        dynamicThreshold *= (1.0f + (attentionalFatigue_ - attentionalFatigueThreshold_) * 2.0f);
    }
    
    // Ensure minimum threshold to prevent runaway selection
    dynamicThreshold = std::max(0.01f, dynamicThreshold);
    
    // Final selection phase
    std::vector<float> finalScores(competitors.size(), 0.0f);
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        // Apply inhibition effects
        float effectiveStrength = competitionStrength[i] - inhibitionLevel[i];
        
        // Add temporal integration from history
        if (!neuronHistory_.empty() && i < neuronHistory_.size()) {
            effectiveStrength += neuronHistory_[i] * 0.3f;
        }
        
        // Apply excitation to winners (positive feedback)
        if (brain_) {
            float excitation = excitationStrength_ * 0.5f;
            // Neuromodulation affects excitation strength
            excitation *= (1.0f + dopamineLevel_ * 0.2f);
            brain_->injectCurrent(competitors[i], excitation);
        }
        
        // Store in history for temporal integration
        if (i < neuronHistory_.size()) {
            neuronHistory_[i] = effectiveStrength * 0.7f + neuronHistory_[i] * 0.3f;
        }
        
        finalScores[i] = std::max(0.0f, effectiveStrength);
    }
    
    // Select winners based on dynamic threshold
    for (size_t i = 0; i < competitors.size(); ++i) {
        if (finalScores[i] >= dynamicThreshold) {
            winners_.push_back(competitors[i]);
            
            // Apply additional excitation to selected winners
            if (brain_) {
                float winnerExcitation = excitationStrength_ * 2.0f;
                // Amplify based on novelty and dopamine
                winnerExcitation *= (1.0f + noveltyLevel_ * 0.5f + dopamineLevel_ * 0.3f);
                brain_->injectCurrent(competitors[i], winnerExcitation);
            }
        }
    }
    
    // Phase 4: Competition mode adjustment
    if (winnerTakeAllMode_) {
        // Winner-take-all: select only the strongest competitor
        if (!winners_.empty()) {
            size_t bestIdx = 0;
            float bestScore = 0.0f;
            
            for (size_t i = 0; i < competitors.size(); ++i) {
                if (finalScores[i] > bestScore) {
                    bestScore = finalScores[i];
                    bestIdx = i;
                }
            }
            
            // Keep only the winner
            winners_.clear();
            winners_.push_back(competitors[bestIdx]);
        }
    } else {
        // Distributed attention: select multiple winners
        // Ensure at least one winner if there's any activity
        if (winners_.empty() && totalActivity > 0.01f) {
            size_t bestIdx = 0;
            float bestScore = finalScores[0];
            
            for (size_t i = 1; i < competitors.size(); ++i) {
                if (finalScores[i] > bestScore) {
                    bestScore = finalScores[i];
                    bestIdx = i;
                }
            }
            
            if (bestScore > 0.01f) {
                winners_.push_back(competitors[bestIdx]);
            }
        }
    }
    
    // Update inhibition history
    if (inhibitionLevel.size() == competitors.size()) {
        pImpl->inhibitionLevel = inhibitionLevel;
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
    
    // Update neuromodulation levels with decay
    dopamineLevel_ = std::max(0.0f, dopamineLevel_ - 0.01f * dt);
    noveltyLevel_ = std::max(0.0f, noveltyLevel_ - 0.02f * dt);
    curiosityLevel_ = std::max(0.0f, curiosityLevel_ - 0.01f * dt);
    
    // Update attentional resources (recovery from fatigue)
    attentionalFatigue_ = std::max(0.0f, attentionalFatigue_ - attentionalRecoveryRate_ * dt);
    
    // Temporal integration decay for selection history
    for (auto& h : neuronHistory_) {
        h *= (1.0f - temporalIntegrationDecay_ * dt);
    }
    
    // Decay excitation and inhibition history
    for (auto& e : excitationHistory_) {
        e *= (1.0f - temporalIntegrationDecay_ * dt);
    }
    
    for (auto& i : inhibitionHistory_) {
        i *= (1.0f - temporalIntegrationDecay_ * dt);
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
    
    // Reset neuromodulation levels
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    
    // Reset working memory integration
    workingMemoryNeurons_.clear();
    workingMemoryActivations_.clear();
    
    // Reset selection strategies
    winnerTakeAllMode_ = false;
    attentionalScope_ = 1.0f;
    
    // Reset biological realism features
    attentionBlinkEnabled_ = false;
    attentionalFatigue_ = 0.0f;
    attentionalRecoveryRate_ = 0.05f;
    attentionalFatigueThreshold_ = 0.8f;
    selectionTimeWindow_ = 100.0f;
    temporalIntegrationDecay_ = 0.01f;
    
    // Clear neural dynamics history
    neuronHistory_.clear();
    inhibitionHistory_.clear();
    excitationHistory_.clear();
}

// Neuromodulation integration methods
void AttentionalSelection::integrateDopamine(float level) {
    dopamineLevel_ = std::clamp(level, 0.0f, 1.0f);
    // Dopamine affects selection strength and decision confidence
    if (brain_) {
        // Apply dopamine effects on neural excitability
        for (const auto& winner : winners_) {
            float dopamineEffect = dopamineLevel_ * 0.5f;
            brain_->injectCurrent(winner, dopamineEffect * 2.0f);
        }
    }
}

void AttentionalSelection::integrateNovelty(float level) {
    noveltyLevel_ = std::clamp(level, 0.0f, 1.0f);
    // Novelty affects salience and selection bias
    for (auto& [neuronId, salience] : bottomUpSalience_) {
        salience *= (1.0f + noveltyLevel_ * 0.5f);
    }
}

void AttentionalSelection::integrateCuriosity(float level) {
    curiosityLevel_ = std::clamp(level, 0.0f, 1.0f);
    // Curiosity affects exploration and selection breadth
    attentionalScope_ = std::min(1.0f, attentionalScope_ * (1.0f + curiosityLevel_ * 0.3f));
}

void AttentionalSelection::updateBrainState(float attention, float arousal) {
    // Update brain state based on attention and arousal
    // This could modulate competition dynamics
    attentionalScope_ = std::min(1.0f, attentionalScope_ * (0.5f + attention * 0.5f));
    
    // High arousal increases competition threshold
    if (arousal > 0.7f) {
        competitionThreshold_ = std::min(1.0f, competitionThreshold_ * (1.0f + arousal * 0.3f));
    }
    
    // Update attentional fatigue based on sustained attention
    attentionalFatigue_ = std::min(1.0f, attentionalFatigue_ + (1.0f - attention) * 0.01f);
}

// Working memory integration
void AttentionalSelection::integrateWorkingMemory(const std::vector<NeuronId>& memoryNeurons,
                                                 const std::vector<float>& memoryActivations) {
    workingMemoryNeurons_ = memoryNeurons;
    workingMemoryActivations_ = memoryActivations;
    
    // Use working memory activation as additional input to attention
    for (size_t i = 0; i < memoryNeurons.size() && i < memoryActivations.size(); ++i) {
        NeuronId neuron = memoryNeurons[i];
        float activation = memoryActivations[i];
        
        // Apply working memory activation to top-down bias
        applyTopDownBias(neuron, activation * 0.5f);
        
        // Enhance salience for active working memory traces
        applyBottomUpSalience(neuron, activation * 0.3f);
    }
}

// Advanced selection strategies
void AttentionalSelection::setSelectionMode(bool winnerTakeAll) {
    winnerTakeAllMode_ = winnerTakeAll;
    
    // Reset attention when mode changes
    if (winnerTakeAll) {
        NLM_LOG_INFO("AttentionalSelection: Enabled Winner-Take-All mode");
    } else {
        NLM_LOG_INFO("AttentionalSelection: Enabled Distributed Attention mode");
    }
}

void AttentionalSelection::addTopDownBias(NeuronId neuron, float biasStrength, float timeWindow) {
    // Store time-limited top-down bias
    // In a real implementation, this would have expiration
    applyTopDownBias(neuron, biasStrength);
    
    // Schedule bias decay
    // For simplicity, we use the existing decay mechanism
}

void AttentionalSelection::addBottomUpSalience(NeuronId neuron, float salienceStrength, float timeWindow) {
    // Store time-limited bottom-up salience
    applyBottomUpSalience(neuron, salienceStrength);
    
    // Schedule salience decay
}

// Biological realism features
void AttentionalSelection::setAttentionalScope(float scope) {
    attentionalScope_ = std::clamp(scope, 0.1f, 2.0f);
}

void AttentionalSelection::enableAttentionBlink(bool enable, float duration) {
    attentionBlinkEnabled_ = enable;
    attentionBlinkDuration_ = duration;
    
    if (enable) {
        NLM_LOG_INFO("AttentionalSelection: Attention blink enabled for " + std::to_string(duration) + " timesteps");
    }
}

void AttentionalSelection::setFatigueParameters(float recoveryRate, float fatigueThreshold) {
    attentionalRecoveryRate_ = std::clamp(recoveryRate, 0.0f, 1.0f);
    attentionalFatigueThreshold_ = std::clamp(fatigueThreshold, 0.1f, 1.0f);
}

} // namespace nlm

} // namespace nlm