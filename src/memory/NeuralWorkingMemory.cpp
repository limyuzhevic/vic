#pragma once

#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Connection management for maintenance loops
    struct MaintenanceConnection {
        NeuronId fromNeuron;
        NeuronId toNeuron;
        float strength;
        float lastUpdate;
        
        MaintenanceConnection(NeuronId from, NeuronId to, float str)
            : fromNeuron(from), toNeuron(to), strength(str), lastUpdate(0.0f) {}
    };
    
    std::vector<MaintenanceConnection> maintenanceConnections;
    
    // Neural persistence mechanisms
    struct PersistentNeuron {
        NeuronId neuronId;
        float baselineActivation;
        float targetActivation;
        float timeInMemory;
        float adaptationLevel;
        float synapticWeight;
        
        PersistentNeuron(NeuronId id, float baseline)
            : neuronId(id), baselineActivation(baseline), targetActivation(baseline),
              timeInMemory(0.0f), adaptationLevel(0.5f), synapticWeight(0.5f) {}
    };
    
    std::vector<PersistentNeuron> persistentNeurons;
    
    // Short-term synaptic plasticity
    struct SynapticPlasticity {
        float eligibilityTrace;    // For STDP-like eligibility
        float neuromodulationFactor; // Modulated by neurotransmitters
        float adaptationPower;    // Rate of weight change
        
        SynapticPlasticity() : eligibilityTrace(0.0f), neuromodulationFactor(1.0f), adaptationPower(0.001f) {}
    };
    
    // Competition mechanisms
    struct CompetitionWeights {
        float lateralInhibition;
        float recurrentExcitation;
        float externalDrive;
        float baselineGain;
        
        CompetitionWeights()
            : lateralInhibition(0.8f), recurrentExcitation(0.3f), 
              externalDrive(0.5f), baselineGain(1.0f) {}
    };
    
    CompetitionWeights competitionWeights;
    
    // Implementation methods
    void createPersistentLoop(PersistentNeuron& neuron, std::mt19937& rng);
    void maintainPersistentActivity(PersistentNeuron& neuron, float dt);
    void updateSynapticWeights(PersistentNeuron& neuron, float dt, float neuromodulation);
    float computeLateralInhibition(const std::vector<PersistentNeuron>& neurons, size_t idx) const;
    
    Impl() : brain(nullptr) {}
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
{
    std::random_device rd;
    rng_.seed(rd());
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Pre-allocate memory for common operations
    memoryNeurons_.reserve(capacity_);
    memoryActivations_.reserve(capacity_);
    memoryTimestamps_.reserve(capacity_);
    
    NLM_LOG_INFO("NeuralWorkingMemory initialized with capacity: " + std::to_string(capacity_));
}

void NeuralWorkingMemory::Impl::createPersistentLoop(PersistentNeuron& neuron, std::mt19937& rng) {
    // Create a recurrent connection for maintenance
    // Multiple weak connections can create a self-sustaining loop
    
    float loopStrength = std::uniform_real_distribution<float>(0.1f, 0.4f)(rng);
    
    // Find a neuron with similar activation profile to create a loop
    if (!persistentNeurons.empty()) {
        size_t partnerIdx = rng() % persistentNeurons.size();
        auto& partner = persistentNeurons[partnerIdx];
        
        if (partner.neuronId != neuron.neuronId) {
            // Create bidirectional connection
            maintenanceConnections.emplace_back(neuron.neuronId, partner.neuronId, loopStrength);
            maintenanceConnections.emplace_back(partner.neuronId, neuron.neuronId, loopStrength);
        }
    }
}

void NeuralWorkingMemory::Impl::maintainPersistentActivity(PersistentNeuron& neuron, float dt) {
    if (!brain_) return;
    
    // Update neuron state using biologically realistic dynamics
    float currentTime = static_cast<float>(brain_->getCurrentTime());
    
    // Use LIF-like dynamics for maintenance
    // Input from recurrent connections
    float recurrentInput = 0.0f;
    for (const auto& conn : maintenanceConnections) {
        if (conn.toNeuron == neuron.neuronId) {
            // Find source neuron activation
            for (auto& other : persistentNeurons) {
                if (other.neuronId == conn.fromNeuron) {
                    float otherActivation = other.targetActivation;
                    recurrentInput += otherActivation * conn.strength;
                    break;
                }
            }
        }
    }
    
    // Apply current leak and recurrent drive
    float leak = neuron.targetActivation * decayRate_;
    float netInput = recurrentInput - leak;
    
    // Update neuron membrane potential using LIF dynamics
    float tau = 20.0f; // Time constant
    float dtAlpha = dt / tau;
    
    // If we have the actual neuron, update it
    // For now, we'll simulate through the brain's injection mechanism
    // NeuronId is a simple wrapper around uint64_t
    uint64_t neuronId = neuron.neuronId;
    
    // Update target activation (simulating neural dynamics)
    float desiredChange = (recurrentInput - neuron.targetActivation) * dtAlpha;
    neuron.targetActivation += desiredChange;
    
    // Apply neuromodulation and adaptation
    neuron.adaptationLevel = std::clamp(neuron.adaptationLevel, 0.1f, 2.0f);
    neuron.targetActivation *= neuron.adaptationLevel;
    
    // Keep activation within reasonable bounds
    neuron.targetActivation = std::clamp(neuron.targetActivation, 0.0f, 2.0f);
    
    // Inject current if significant
    if (std::abs(netInput) > 0.01f) {
        brain_->injectCurrent(neuron.neuronId, netInput * 10.0f);
    }
}

void NeuralWorkingMemory::Impl::updateSynapticWeights(PersistentNeuron& neuron, float dt, float neuromodulation) {
    // STDP-like synaptic plasticity for maintenance connections
    
    for (auto& conn : maintenanceConnections) {
        if (conn.toNeuron == neuron.neuronId) {
            // Compute eligibility trace based on co-activation
            float eligibility = std::exp(-dt * 0.1f); // Decay
            conn.strength *= (1.0f + eligibility * neuromodulation * 0.01f);
            
            // Keep strength within bounds
            conn.strength = std::clamp(conn.strength, 0.05f, 1.0f);
            
            // Update neuron's synaptic weight
            neuron.synapticWeight = std::min(1.0f, neuron.synapticWeight + 0.001f * neuromodulation);
        }
    }
}

float NeuralWorkingMemory::Impl::computeLateralInhibition(const std::vector<PersistentNeuron>& neurons, size_t idx) const {
    float totalInhibition = 0.0f;
    float selfActivation = neurons[idx].targetActivation;
    
    for (size_t i = 0; i < neurons.size(); ++i) {
        if (i == idx) continue;
        
        float otherActivation = neurons[i].targetActivation;
        
        // Competitive lateral inhibition (shorter distances stronger)
        float distance = std::abs(static_cast<float>(i) - static_cast<float>(idx)));
        float inhibitionStrength = competitionWeights.lateralInhibition / (1.0f + distance);
        
        // Normalize by other's activation
        float relativeActivation = otherActivation / (selfActivation + 0.001f);
        
        if (relativeActivation > 1.2f) { // Other is significantly stronger
            totalInhibition += inhibitionStrength * (relativeActivation - 1.2f);
        }
    }
    
    return std::min(1.0f, totalInhibition);
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    clear(); // Start fresh for simplicity
    
    // Encode pattern as persistent neural activity
    for (size_t i = 0; i < pattern.size() && i < capacity_; ++i) {
        float activation = pattern[i] * strength;
        NeuronId neuronId(static_cast<uint64_t>(i) + 1000); // Unique IDs
        
        // Create persistent neuron entry
        PersistentNeuron persistentNeuron(neuronId, activation);
        pImpl->persistentNeurons.push_back(persistentNeuron);
        
        // Store basic memory entry
        memoryNeurons_.push_back(neuronId);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
        
        // Inject initial current to establish the pattern
        brain_->injectCurrent(neuronId, activation * 5.0f);
    }
    
    // Create maintenance loops between neurons in the pattern
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    for (auto& neuron : pImpl->persistentNeurons) {
        pImpl->createPersistentLoop(neuron, rng);
    }
    
    NLM_LOG_INFO("Stored pattern in working memory with " + std::to_string(memoryNeurons_.size()) + " neurons");
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Check if neuron is already in memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
        
        // Update persistent neuron state
        for (auto& persistent : pImpl->persistentNeurons) {
            if (persistent.neuronId == neuron) {
                persistent.targetActivation = activation;
                persistent.baselineActivation = activation;
                break;
            }
        }
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
        
        // Create new persistent neuron
        PersistentNeuron persistentNeuron(neuron, activation);
        pImpl->persistentNeurons.push_back(persistentNeuron);
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
    
    float floatDt = static_cast<float>(dt);
    
    // Update persistent neurons using realistic dynamics
    for (auto& persistent : pImpl->persistentNeurons) {
        // Maintain activity through recurrent connections
        pImpl->maintainPersistentActivity(persistent, floatDt);
        
        // Update synaptic weights based on neuromodulation
        float neuromodulation = 1.0f; // In real system, would get from neuromodulators
        pImpl->updateSynapticWeights(persistent, floatDt, neuromodulation);
        
        // Update target activation based on memory age
        persistent.timeInMemory += floatDt;
        
        // Age-dependent adaptation
        persistent.adaptationLevel = 1.0f + std::exp(-persistent.timeInMemory / 1000.0f);
        
        // Update stored activation
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), persistent.neuronId);
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            memoryActivations_[idx] = persistent.targetActivation;
            memoryTimestamps_[idx]++;
        }
    }
    
    // Apply lateral inhibition and competition
    runCompetition();
    
    // Decay weak traces
    decayWeakTraces();
}

void NeuralWorkingMemory::runCompetition() {
    winners_.clear();
    
    if (memoryActivations_.empty()) return;
    
    // Multi-level competition with lateral inhibition
    std::vector<float> effectiveActivation(memoryActivations_.size(), 0.0f);
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        // Apply lateral inhibition from other neurons
        float lateralInhibition = pImpl->computeLateralInhibition(pImpl->persistentNeurons, i);
        effectiveActivation[i] = memoryActivations_[i] * (1.0f - lateralInhibition);
        
        // Add external drive
        effectiveActivation[i] += 0.1f * pImpl->competitionWeights.externalDrive;
    }
    
    // Normalize for competition
    float maxActivation = *std::max_element(effectiveActivation.begin(), effectiveActivation.end());
    if (maxActivation > 0.0f) {
        for (float& act : effectiveActivation) {
            act /= maxActivation;
        }
    }
    
    // Apply winner-take-all competition
    winners_.clear();
    for (size_t i = 0; i < effectiveActivation.size(); ++i) {
        if (effectiveActivation[i] > 0.6f) { // Winner threshold
            winners_.push_back(memoryNeurons_[i]);
            
            // Strong excitation for winners
            if (brain_) {
                brain_->injectCurrent(memoryNeurons_[i], 2.0f * memoryActivations_[i]);
            }
            
            // Apply inhibition to others
            for (size_t j = 0; j < memoryNeurons_.size(); ++j) {
                if (j != i) {
                    float inhibition = pImpl->competitionWeights.lateralInhibition * 
                                      std::max(0.0f, effectiveActivation[i] - effectiveActivation[j]);
                    if (brain_) {
                        brain_->injectCurrent(memoryNeurons_[j], -inhibition * 5.0f);
                    }
                }
            }
        }
    }
    
    // If no clear winner, use threshold-based selection
    if (winners_.empty()) {
        float threshold = 0.4f;
        for (size_t i = 0; i < effectiveActivation.size(); ++i) {
            if (effectiveActivation[i] > threshold) {
                winners_.push_back(memoryNeurons_[i]);
            }
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
    // Find or add maintenance connection
    for (auto& conn : pImpl->maintenanceConnections) {
        if (conn.fromNeuron == from && conn.toNeuron == to) {
            conn.strength = strength;
            return;
        }
    }
    
    pImpl->maintenanceConnections.emplace_back(from, to, strength);
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Apply recurrent maintenance currents
    for (const auto& conn : pImpl->maintenanceConnections) {
        float sourceActivation = 0.0f;
        
        // Find source neuron activation
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.fromNeuron);
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            sourceActivation = memoryActivations_[idx];
        }
        
        if (sourceActivation > 0.1f && brain_) {
            // Send maintenance signal
            brain_->injectCurrent(conn.toNeuron, sourceActivation * 2.0f * conn.strength);
        }
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        // Apply natural decay
        memoryActivations_[i] *= (1.0f - decayRate_);
        
        // Decay persistent neuron target activation
        if (i < pImpl->persistentNeurons.size()) {
            pImpl->persistentNeurons[i].targetActivation *= (1.0f - decayRate_ * 2.0f);
        }
        
        // Remove very weak traces
        if (memoryActivations_[i] < 0.005f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t idx = *it;
        
        // Also remove corresponding persistent neuron
        auto persIt = std::find_if(pImpl->persistentNeurons.begin(), pImpl->persistentNeurons.end(),
            [idx](const PersistentNeuron& p) { return std::find(memoryNeurons_.begin() + idx, memoryNeurons_.end(), p.neuronId) != memoryNeurons_.end(); });
        
        if (persIt != pImpl->persistentNeurons.end()) {
            pImpl->persistentNeurons.erase(persIt);
        }
        
        // Remove from other arrays
        memoryNeurons_.erase(memoryNeurons_.begin() + idx);
        memoryActivations_.erase(memoryActivations_.begin() + idx);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
    }
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    for (auto& activation : memoryActivations_) {
        activation = std::min(1.0f, activation * factor);
    }
    
    // Also strengthen persistent neuron target activations
    for (auto& persistent : pImpl->persistentNeurons) {
        persistent.targetActivation = std::min(1.0f, persistent.targetActivation * factor);
    }
}

// AttentionalSelection Implementation
struct AttentionalSelection::Impl {
    // Neural competition state
    std::vector<float> competitionStrength;
    std::vector<float> inhibitionLevel;
    std::vector<float> attentionalGain;
    
    // Salience computation
    std::vector<float> stimulusSalience;
    std::vector<float> goalRelevance;
    
    // Dynamic routing parameters
    std::vector<RegionId> attentionalFocus;
    float globalInhibition;
    
    Impl() : globalInhibition(0.5f) {}
};

AttentionalSelection::AttentionalSelection()
    : pImpl(new Impl)
    , brain_(nullptr)
    , inhibitionStrength_(0.5f)
    , excitationStrength_(1.5f)
    , competitionThreshold_(0.3f)
{
    attentionalGain_.reserve(100);
    stimulusSalience.reserve(100);
    goalRelevance.reserve(100);
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl = std::make_unique<Impl>();
    brain_ = brain;
    
    // Initialize attentional gain fields
    attentionalGain_.resize(brain_->getTotalNeuronCount(), 1.0f);
    stimulusSalience.resize(brain_->getTotalNeuronCount(), 0.0f);
    goalRelevance.resize(brain_->getTotalNeuronCount(), 0.0f);
    
    NLM_LOG_INFO("AttentionalSelection initialized");
}

std::vector<NeuronId> AttentionalSelection::processCompetition(const std::vector<NeuronId>& competitors,
                                                              float globalInhibition) {
    winners_.clear();
    
    if (competitors.empty()) return winners_;
    
    // Pre-allocate vectors
    std::vector<float> activities(competitors.size(), 0.0f);
    std::vector<float> effectiveActivities(competitors.size(), 0.0f);
    float totalActivity = 0.0f;
    
    // Compute combined activity with salience and goal relevance
    for (size_t i = 0; i < competitors.size(); ++i) {
        NeuronId neuron = competitors[i];
        
        // Get salience from bottom-up processing
        auto salIt = bottomUpSalience_.find(neuron.value);
        float salience = (salIt != bottomUpSalience_.end()) ? salIt->second : 0.0f;
        
        // Get goal relevance from top-down processing
        auto goalIt = topDownBias_.find(neuron.value);
        float goal = (goalIt != topDownBias_.end()) ? goalIt->second : 0.0f;
        
        // Get current neural activity
        float currentActivity = 0.0f;
        if (brain_) {
            // Try to get neuron activation
            for (const auto& region : brain_->getRegions()) {
                for (const auto& pop : region->getPopulations()) {
                    for (auto* n : pop->getNeurons()) {
                        if (n->getId() == neuron) {
                            currentActivity = n->getState().membranePotential;
                            break;
                        }
                    }
                    if (currentActivity > 0.0f) break;
                }
                if (currentActivity > 0.0f) break;
            }
        }
        
        // Combine signals
        float combined = currentActivity + salience * 0.5f + goal * 0.3f;
        activities[i] = combined;
        
        // Apply attentional gain modulation
        effectiveActivities[i] = combined * attentionalGain_[neuron.value];
        totalActivity += effectiveActivities[i];
    }
    
    if (totalActivity < 0.001f) {
        // No strong signal - distribute attention equally
        float equalWeight = 1.0f / competitors.size();
        for (size_t i = 0; i < competitors.size(); ++i) {
            attentionalGain_[competitors[i].value] = equalWeight;
        }
        return competitors;
    }
    
    // Dynamic competition with normalization
    std::vector<float> normalizedActivities(competitors.size(), 0.0f);
    for (size_t i = 0; i < competitors.size(); ++i) {
        normalizedActivities[i] = effectiveActivities[i] / (totalActivity + 0.001f);
    }
    
    // Competitive lateral inhibition
    for (size_t i = 0; i < competitors.size(); ++i) {
        for (size_t j = 0; j < competitors.size(); ++j) {
            if (i == j) continue;
            
            // Inhibition based on relative strength
            float ratio = normalizedActivities[i] / (normalizedActivities[j] + 0.001f);
            
            if (ratio > 1.5f) {
                // i is significantly stronger - inhibit j
                float inhibition = globalInhibition * inhibitionStrength_ * 
                                  std::min(1.0f, (ratio - 1.5f) / 2.0f);
                
                if (brain_) {
                    brain_->injectCurrent(competitors[j], -inhibition * 10.0f);
                }
                
                pImpl->inhibitionLevel.push_back(inhibition);
            }
        }
    }
    
    // Winner selection based on threshold and normalization
    float threshold = competitionThreshold_ * totalActivity / competitors.size();
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        if (effectiveActivities[i] >= threshold) {
            winners_.push_back(competitors[i]);
            
            // Strong excitation for winners
            if (brain_) {
                brain_->injectCurrent(competitors[i], excitationStrength_ * 3.0f);
            }
            
            // Update attentional gain for winners
            attentionalGain_[competitors[i].value] = 1.5f;
        }
    }
    
    // If no clear winner, select top N neurons
    if (winners_.empty() && !normalizedActivities.empty()) {
        // Sort by effective activity
        std::vector<size_t> indices(competitors.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::sort(indices.begin(), indices.end(),
            [&effectiveActivities](size_t a, size_t b) {
                return effectiveActivities[a] > effectiveActivities[b];
            });
        
        // Select top 30% as winners
        size_t numWinners = std::max(1UL, static_cast<size_t>(competitors.size() * 0.3f));
        for (size_t i = 0; i < numWinners; ++i) {
            winners_.push_back(competitors[indices[i]]);
            attentionalGain_[competitors[indices[i]].value] = 1.2f;
        }
    }
    
    // Update attentional focus
    attentionalFocus.clear();
    for (const auto& region : brain_->getRegionIds()) {
        bool regionHasWinner = false;
        for (const auto& winner : winners_) {
            if (brain_->getRegion(winner.getId() / 1000) && 
                brain_->getRegion(winner.getId() / 1000)->getId() == region) {
                regionHasWinner = true;
                break;
            }
        }
        if (regionHasWinner) {
            attentionalFocus.push_back(region);
        }
    }
    
    return winners_;
}

void AttentionalSelection::focusOnRegion(RegionId region) {
    if (std::find(attendedRegions_.begin(), attendedRegions_.end(), region) == attendedRegions_.end()) {
        attendedRegions_.push_back(region);
        
        // Increase gain for neurons in this region
        for (const auto& regionPtr : brain_->getRegions()) {
            if (regionPtr->getId() == region) {
                for (const auto& pop : regionPtr->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        attentionalGain_[neuron->getId().value] *= 1.5f;
                    }
                }
                break;
            }
        }
    }
}

void AttentionalSelection::releaseAttention() {
    attendedRegions_.clear();
    
    // Decay attentional gain back to baseline
    for (float& gain : attentionalGain_) {
        gain = std::max(0.5f, gain * 0.8f);
    }
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
    float floatDt = static_cast<float>(dt);
    
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
    
    // Decay attentional gain
    for (float& gain : attentionalGain_) {
        gain = std::max(1.0f, gain * 0.99f);
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
    
    // Increase attentional gain
    attentionalGain_[neuron.value] *= (1.0f + biasStrength);
}

void AttentionalSelection::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    // Store salience in map keyed by NeuronId
    auto it = bottomUpSalience_.find(neuron.value);
    if (it != bottomUpSalience_.end()) {
        it->second += salienceStrength;
    } else {
        bottomUpSalience_[neuron.value] = salienceStrength;
    }
    
    // Increase attentional gain
    attentionalGain_[neuron.value] *= (1.0f + salienceStrength * 0.5f);
}

void AttentionalSelection::reset() {
    winners_.clear();
    attendedRegions_.clear();
    attentionalGain_.assign(attentionalGain_.size(), 1.0f);
    neuronSalience_.clear();
    topDownBias_.clear();
    bottomUpSalience_.clear();
    pImpl->inhibitionLevel.clear();
}

} // namespace nlm
