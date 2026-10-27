#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Enhanced: Use brain's actual neural populations for pattern storage
    std::vector<RegionId> workingMemoryRegions;
    
    // Pattern competition scores
    std::vector<float> patternScores;
    
    // Short-term plasticity traces
    std::vector<float> eligibilityTraces;
    
    // Replay buffers for consolidation
    std::vector<std::vector<NeuronId>> replayPatterns;
    
    Impl() : brain(nullptr) {
        // Initialize competition scores
        patternScores.reserve(100);
        eligibilityTraces.reserve(100);
    }
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.005f)  // Lower decay for better persistence
{
    // Ensure winners_ is initialized
    winners_.reserve(capacity_);
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Create dedicated working memory regions in brain
    if (brain_) {
        pImpl->workingMemoryRegions.clear();
        for (int i = 0; i < 2; ++i) {  // Create 2 dedicated WM regions
            RegionId regionId = brain_->addRegion("WorkingMemory_" + std::to_string(i));
            pImpl->workingMemoryRegions.push_back(regionId);
        }
    }
    
    NLM_LOG_INFO("NeuralWorkingMemory initialized with " + 
                 std::to_string(pImpl->workingMemoryRegions.size()) + " dedicated regions");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_ || pImpl->workingMemoryRegions.empty()) return;
    
    // Clear old memory if capacity exceeded
    while (memoryNeurons_.size() > capacity_ * 0.8f) {
        decayWeakTraces();
    }
    
    // Encode pattern into working memory neurons
    size_t neuronsPerPattern = std::min(pattern.size(), 50);  // Limit neurons per pattern
    size_t regionIndex = 0;
    
    for (size_t i = 0; i < neuronsPerPattern; ++i) {
        // Get dedicated working memory region
        RegionId regionId = pImpl->workingMemoryRegions[regionIndex % pImpl->workingMemoryRegions.size()];
        auto* region = brain_->getRegion(regionId);
        if (!region) continue;
        
        // Create or find neuron in this region for this memory item
        NeuronId neuronId(regionId.index() * 1000 + memoryNeurons_.size() + 1);
        
        // Check if neuron exists in region
        bool neuronFound = false;
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron->getId() == neuronId) {
                    neuronFound = true;
                    // Update neuron activation based on pattern
                    float patternValue = pattern[i % pattern.size()];
                    float activation = patternValue * strength;
                    
                    // Store in working memory
                    memoryNeurons_[memoryNeurons_.size()] = neuronId;
                    memoryActivations_[memoryNeurons_.size()] = activation;
                    memoryTimestamps_[memoryNeurons_.size()] = 0;
                    
                    // Set neuron membrane potential to encode pattern
                    neuron->setMembranePotential(activation * 0.1f + neuron->getRestingPotential());
                    break;
                }
            }
            if (neuronFound) break;
        }
        
        // If neuron doesn't exist, create it (simplified - real implementation would need neuron factory)
        if (!neuronFound) {
            // For now, just store the pattern value
            memoryNeurons_.push_back(neuronId);
            memoryActivations_.push_back(pattern[i % pattern.size()] * strength);
            memoryTimestamps_.push_back(0);
        }
        
        regionIndex++;
    }
    
    // Create maintenance connections between pattern elements
    for (size_t i = 1; i < memoryNeurons_.size(); ++i) {
        createRecurrentConnection(memoryNeurons_[i-1], memoryNeurons_[i], 
                                 memoryActivations_[i-1] * memoryActivations_[i] * 0.1f);
    }
    
    // Update eligibility traces for reinforcement learning
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        if (i < eligibilityTraces.size()) {
            eligibilityTraces[i] = 1.0f;  // Strong trace for new patterns
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
        
        // Update eligibility trace for reinforcement
        if (idx < eligibilityTraces.size()) {
            eligibilityTraces[idx] = 1.0f;
        }
        
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
        eligibilityTraces.push_back(1.0f);
    }
    
    // Stronger injection to maintain activation
    if (brain_) {
        brain_->injectCurrent(neuron, activation * 10.0f);
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    
    // Retrieve based on current neuron states (real-time readout)
    result.reserve(memoryNeurons_.size() * 2);  // More detailed retrieval
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        // Get current activation from dedicated working memory regions
        RegionId regionId = pImpl->workingMemoryRegions[i % pImpl->workingMemoryRegions.size()];
        auto* region = brain_->getRegion(regionId);
        if (!region) continue;
        
        // Find the neuron in the region
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron->getId() == memoryNeurons_[i]) {
                    // Calculate activation from membrane potential
                    float activation = std::abs(neuron->getState().membranePotential - 
                                             neuron->getState().restingPotential) / 10.0f;
                    result.push_back(activation);
                    
                    // Also add eligibility trace if available
                    if (i < pImpl->brain_->getWorkingMemory()->getNeuronActivation(neuron->getId())) {
                        float eligibility = pImpl->brain_->getWorkingMemory()->getNeuronActivation(neuron->getId()) * 0.1f;
                        result.push_back(eligibility);
                    }
                    break;
                }
            }
        }
    }
    
    // If no dedicated neurons, return stored activations
    if (result.empty() && !memoryActivations_.empty()) {
        result = memoryActivations_;
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
        
        // Get actual current activation from brain if available
        if (brain_) {
            auto* region = brain_->getRegion(neuron.getId() / 1000);
            if (region) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* n : pop->getNeurons()) {
                        if (n->getId() == neuron) {
                            float currentAct = std::abs(n->getState().membranePotential - 
                                                      n->getState().restingPotential) / 10.0f;
                            return currentAct > 0.0f ? currentAct : memoryActivations_[idx];
                        }
                    }
                }
            }
        }
        
        return memoryActivations_[idx];
    }
    return 0.0f;
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Update maintenance through recurrent connections
    updateRecurrentConnections();
    
    // Update all memory neurons based on eligibility traces
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        
        // Get current activation
        float currentActivation = getNeuronActivation(neuron);
        
        if (currentActivation > 0.1f) {
            // Apply eligibility trace to strengthen/reinforce memory
            if (i < eligibilityTraces.size()) {
                float reinforcement = eligibilityTraces[i];
                currentActivation += reinforcement * 0.1f;
                
                // Decay eligibility trace
                eligibilityTraces[i] *= 0.9f;
            }
            
            // Maintain activation through recurrent connections
            brain_->injectCurrent(neuron, currentActivation * 5.0f);
            
            // Age the trace
            memoryTimestamps_[i]++;
            
            // Age-based decay (slower for strong memories)
            if (memoryTimestamps_[i] > 500) {
                float ageDecay = 1.0f / (1.0f + memoryTimestamps_[i] * 0.001f);
                currentActivation *= ageDecay;
            }
            
            memoryActivations_[i] = currentActivation;
        }
    }
    
    // Decay weak traces
    decayWeakTraces();
    
    // Run competition to select winners
    runCompetition();
    
    // Link with episodic memory for consolidation
    if (brain_->getEpisodicMemory()) {
        brain_->getEpisodicMemory()->enableReplay(true);
    }
}

void NeuralWorkingMemory::clear() {
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    activeTraces_.clear();
    pImpl->maintenanceSynapses.clear();
    eligibilityTraces.clear();
    winners_.clear();
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    for (auto& activation : memoryActivations_) {
        activation = std::min(1.0f, activation * factor);
    }
    
    // Also strengthen eligibility traces
    for (auto& trace : eligibilityTraces) {
        trace = std::min(1.0f, trace * factor);
    }
}

void NeuralWorkingMemory::runCompetition() {
    winners_.clear();
    
    if (memoryNeurons_.empty()) return;
    
    // Enhanced competition algorithm
    std::vector<float> competitionScores;
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        float activation = getNeuronActivation(memoryNeurons_[i]);
        float eligibility = (i < eligibilityTraces.size()) ? eligibilityTraces[i] : 0.0f;
        
        // Combined score for competition
        float score = activation * 0.7f + eligibility * 0.3f;
        competitionScores.push_back(score);
    }
    
    // Find top competitors (winner-take-all)
    std::vector<size_t> winnerIndices;
    for (size_t i = 0; i < competitionScores.size(); ++i) {
        bool isWinner = true;
        
        // Check if this neuron is significantly better than others
        for (size_t j = 0; j < competitionScores.size(); ++j) {
            if (i != j && competitionScores[j] > competitionScores[i] * 1.2f) {
                isWinner = false;
                break;
            }
        }
        
        if (isWinner && competitionScores[i] > 0.1f) {
            winnerIndices.push_back(i);
            
            // Apply strong excitation to winners
            if (brain_) {
                brain_->injectCurrent(memoryNeurons_[i], competitionScores[i] * 10.0f);
            }
        }
    }
    
    // Set winners
    winners_.clear();
    for (size_t idx : winnerIndices) {
        if (idx < memoryNeurons_.size()) {
            winners_.push_back(memoryNeurons_[idx]);
        }
    }
    
    // Apply inhibition to non-winners
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (std::find(winnerIndices.begin(), winnerIndices.end(), i) == winnerIndices.end()) {
            // Apply lateral inhibition
            if (brain_) {
                brain_->injectCurrent(memoryNeurons_[i], -memoryActivations_[i] * 2.0f);
            }
        }
    }
}

bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

float NeuralWorkingMemory::getMemoryActivity() const {
    if (memoryNeurons_.empty()) return 0.0f;
    
    float total = 0.0f;
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        total += getNeuronActivation(memoryNeurons_[i]);
    }
    
    return total / memoryNeurons_.size();
}

void NeuralWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    // Check if connection already exists
    for (const auto& conn : pImpl->maintenanceSynapses) {
        if (conn.first == from && conn.second == to) return;
    }
    
    // Create recurrent synapse for maintenance
    pImpl->maintenanceSynapses.emplace_back(from, to);
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Apply maintenance currents through enhanced recurrent connections
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
            brain_->injectCurrent(conn.second, fromActivation * 3.0f);
        }
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        float currentActivation = getNeuronActivation(memoryNeurons_[i]);
        
        // Apply eligibility-based decay
        if (i < eligibilityTraces.size()) {
            currentActivation *= (1.0f - decayRate_ * eligibilityTraces[i]);
        } else {
            currentActivation *= (1.0f - decayRate_);
        }
        
        memoryActivations_[i] = currentActivation;
        
        if (currentActivation < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t idx = *it;
        
        // Remove corresponding eligibility trace
        if (idx < eligibilityTraces.size()) {
            eligibilityTraces.erase(eligibilityTraces.begin() + idx);
        }
        
        memoryNeurons_.erase(memoryNeurons_.begin() + idx);
        memoryActivations_.erase(memoryActivations_.begin() + idx);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
    }
}

} // namespace nlm
