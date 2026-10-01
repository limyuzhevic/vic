#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Resource management
    std::vector<float> resourceAllocation;  // Per-neuron resource allocation
    float totalResourceBudget;
    float minThreshold;  // Minimum activation to maintain trace
    
    // Synaptic dynamics for maintenance
    std::unordered_map<uint64_t, float> synapticStrengths;  // Connection strength cache
    std::unordered_map<uint64_t, float> eligibilityTraces;  // For spike-based plasticity
    
    // Temporal dynamics
    std::unordered_map<uint64_t, SimulationStep> lastUpdateStep;
    std::vector<size_t> recentlyUpdatedNeurons;  // For resource competition
    
    // Competition parameters
    float competitionStrength;
    float inhibitionRadius;
    
    Impl() : 
        brain(nullptr),
        totalResourceBudget(100.0f),
        minThreshold(0.1f),
        competitionStrength(0.8f),
        inhibitionRadius(3.0f)  // Effective competition range
    {
        resourceAllocation.reserve(500);
        synapticStrengths.reserve(500);
        eligibilityTraces.reserve(500);
    }
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
{
    resourceAllocation.reserve(capacity_);
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize resource allocation (equal distribution)
    resourceAllocation.clear();
    resourceAllocation.resize(capacity_, 1.0f / capacity_ * pImpl->totalResourceBudget);
    
    NLM_LOG_INFO("NeuralWorkingMemory initialized with " + std::to_string(capacity_) + " capacity and " + 
                 std::to_string(pImpl->totalResourceBudget) + " total resource budget");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Normalize strength based on resource availability
    float availableResources = 0.0f;
    for (float alloc : resourceAllocation) {
        availableResources += alloc;
    }
    
    float resourceMultiplier = std::min(1.0f, availableResources / pImpl->totalResourceBudget);
    strength *= resourceMultiplier;
    
    // Calculate neurons needed and find most suitable neurons
    size_t neuronsNeeded = std::min(pattern.size(), capacity_);
    
    // Find neurons with highest available resources for encoding
    std::vector<std::pair<size_t, float>> neuronResources;
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        neuronResources.push_back({i, resourceAllocation[i]});
    }
    
    // Sort by available resources (descending)
    std::sort(neuronResources.begin(), neuronResources.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Encode pattern using neurons with most available resources
    size_t neuronsUsed = 0;
    for (size_t i = 0; i < neuronsNeeded && neuronsUsed < neuronResources.size(); ++i) {
        size_t neuronIdx = neuronResources[i].first;
        NeuronId neuron = memoryNeurons_[neuronIdx];
        
        float activation = pattern[i] * strength * resourceAllocation[neuronIdx];
        
        // Update neuron activation
        if (auto* n = brain_->getRegion(neuron.getId() / 1000)->getAllNeurons()) {
            for (auto* nn : *n) {
                if (nn->getId() == neuron) {
                    // Apply Hebbian learning rule for encoding
                    float currentPotential = nn->getMembranePotential();
                    nn->setMembranePotential(currentPotential + activation * 0.5f);
                    break;
                }
            }
        }
        
        // Update memory activation and timestamps
        if (neuronsUsed < memoryActivations_.size()) {
            memoryActivations_[neuronsUsed] = activation;
        } else {
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
        }
        
        // Update resource allocation (reduce for reuse)
        resourceAllocation[neuronIdx] *= 0.8f;
        
        // Create maintenance connection if not exists
        for (size_t j = 0; j < memoryNeurons_.size(); ++j) {
            if (j != neuronIdx) {
                createRecurrentConnection(memoryNeurons_[j], neuron, 0.5f * resourceAllocation[j]);
            }
        }
        
        ++neuronsUsed;
    }
    
    // Update active traces
    activeTraces_.clear();
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (resourceAllocation[i] > minThreshold) {
            activeTraces_.push_back(i);
        }
    }
    
    NLM_LOG_DEBUG("Stored pattern in working memory: " + std::to_string(neuronsUsed) + " neurons used");
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    size_t idx = 0;
    if (it != memoryNeurons_.end()) {
        idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
        
        // Update resource allocation
        if (idx < resourceAllocation.size()) {
            resourceAllocation[idx] = activation;
        }
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
        
        // Allocate resource to new neuron
        float availableResources = pImpl->totalResourceBudget;
        float perNeuronResource = availableResources / memoryNeurons_.size();
        resourceAllocation.push_back(perNeuronResource);
        
        idx = memoryNeurons_.size() - 1;
        
        // Create maintenance connections with existing neurons
        for (size_t j = 0; j < memoryNeurons_.size() - 1; ++j) {
            createRecurrentConnection(memoryNeurons_[j], neuron, 0.3f);
            createRecurrentConnection(neuron, memoryNeurons_[j], 0.3f);
        }
    } else {
        NLM_LOG_WARNING("Working memory full, cannot store new neuron: " + std::to_string(neuron.getId()));
        return;
    }
    
    // Inject current to maintain activation
    if (brain_) {
        brain_->injectCurrent(neuron, activation * 5.0f);
    }
    
    // Update eligibility trace for spike-based plasticity
    pImpl->eligibilityTraces[neuron.value] = 1.0f;
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
    
    float timeStep = static_cast<float>(dt);
    
    // Update maintenance - apply spike-based plasticity
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > minThreshold) {
            // Inject maintenance current proportional to activation
            brain_->injectCurrent(neuron, activation * 2.0f);
            
            // Update eligibility trace for plasticity
            if (pImpl->eligibilityTraces.find(neuron.value) != pImpl->eligibilityTraces.end()) {
                pImpl->eligibilityTraces[neuron.value] *= 0.95f;
            }
            
            // Update resource allocation based on activation
            resourceAllocation[i] = activation;
            
            // Age the trace and apply decay
            memoryTimestamps_[i]++;
            
            // Check if trace is too old and should decay
            if (memoryTimestamps_[i] > 1000) {
                float decayFactor = std::pow(0.95f, timeStep);
                memoryActivations_[i] *= decayFactor;
                
                // Reduce resource allocation for decayed trace
                resourceAllocation[i] *= decayFactor;
            }
            
            // Apply spike-based Hebbian learning
            updateEligibilityTraces(neuron, activation);
        } else {
            // Trace below threshold - reduce resource allocation
            resourceAllocation[i] *= 0.9f;
        }
    }
    
    // Decay weak traces based on resources
    decayWeakTraces();
    
    // Run adaptive competition
    runCompetition();
    
    // Update recurrent connections
    updateRecurrentConnections();
    
    // Update eligibility traces with decay
    for (auto& trace : pImpl->eligibilityTraces) {
        trace.second *= 0.99f;
    }
}

void NeuralWorkingMemory::clear() {
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    activeTraces_.clear();
    pImpl->maintenanceSynapses.clear();
    pImpl->eligibilityTraces.clear();
    resourceAllocation.clear();
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        memoryActivations_[i] = std::min(1.0f, memoryActivations_[i] * factor);
        resourceAllocation[i] = std::min(pImpl->totalResourceBudget, resourceAllocation[i] * factor);
        
        // Update neuron activation
        NeuronId neuron = memoryNeurons_[i];
        if (brain_) {
            brain_->injectCurrent(neuron, memoryActivations_[i] * 10.0f);
        }
    }
    
    NLM_LOG_INFO("Strengthened working memory by factor " + std::to_string(factor));
}

void NeuralWorkingMemory::runCompetition() {
    winners_.clear();
    
    if (memoryActivations_.empty()) return;
    
    // Adaptive competition threshold based on average activation
    float averageActivation = getMemoryActivity();
    float threshold = 0.3f + 0.2f * averageActivation;  // Dynamic threshold
    
    // Compute competition scores with lateral inhibition
    std::vector<float> scores(memoryNeurons_.size(), 0.0f);
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        // Base score from activation
        scores[i] = memoryActivations_[i];
        
        // Subtract inhibition from neighboring neurons (within radius)
        for (size_t j = 0; j < memoryNeurons_.size(); ++j) {
            if (i != j) {
                // Simple distance metric
                float distance = std::abs(static_cast<float>(i - j));
                if (distance < pImpl->inhibitionRadius) {
                    float inhibition = 1.0f - (distance / pImpl->inhibitionRadius);
                    scores[i] -= inhibition * pImpl->competitionStrength * memoryActivations_[j];
                }
            }
        }
        
        // Ensure score is non-negative
        scores[i] = std::max(0.0f, scores[i]);
    }
    
    // Select winners based on adaptive threshold
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (scores[i] >= threshold && memoryActivations_[i] > minThreshold) {
            winners_.push_back(memoryNeurons_[i]);
            
            // Apply excitation to winners
            NeuronId neuron = memoryNeurons_[i];
            brain_->injectCurrent(neuron, scores[i] * 3.0f);
            
            // Boost resource allocation for winners
            if (i < resourceAllocation.size()) {
                resourceAllocation[i] = std::min(pImpl->totalResourceBudget, resourceAllocation[i] * 1.5f);
            }
        }
    }
    
    // If no neurons meet threshold, reduce threshold and retry
    if (winners_.empty() && !memoryActivations_.empty()) {
        threshold *= 0.8f;
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            if (scores[i] >= threshold && memoryActivations_[i] > minThreshold / 2.0f) {
                winners_.push_back(memoryNeurons_[i]);
            }
        }
    }
    
    // Apply inhibition to non-winners
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        bool isWinner = std::find(winners_.begin(), winners_.end(), memoryNeurons_[i]) != winners_.end();
        
        if (!isWinner && brain_) {
            // Apply competition-based inhibition
            brain_->injectCurrent(memoryNeurons_[i], -memoryActivations_[i] * pImpl->competitionStrength);
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
    
    // Check if neurons exist in memory
    bool fromExists = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), from) != memoryNeurons_.end();
    bool toExists = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), to) != memoryNeurons_.end();
    
    if (fromExists && toExists) {
        pImpl->maintenanceSynapses.emplace_back(from, to);
        pImpl->synapticStrengths[std::hash<uint64_t>()(from) + std::hash<uint64_t>()(to)] = strength;
        NLM_LOG_DEBUG("Created recurrent connection from neuron " + std::to_string(from.getId()) + 
                     " to neuron " + std::to_string(to.getId()) + " with strength " + std::to_string(strength));
    }
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Apply maintenance currents through recurrent connections with spike-based plasticity
    for (const auto& conn : pImpl->maintenanceSynapses) {
        // Get from neuron activation
        float fromActivation = 0.0f;
        auto neuronIt = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.first);
        if (neuronIt != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), neuronIt);
            fromActivation = memoryActivations_[idx];
        }
        
        // Get connection strength
        uint64_t connKey = std::hash<uint64_t>()(conn.first) + std::hash<uint64_t>()(conn.second);
        float strength = 1.0f;
        if (pImpl->synapticStrengths.find(connKey) != pImpl->synapticStrengths.end()) {
            strength = pImpl->synapticStrengths[connKey];
        }
        
        if (fromActivation > 0.1f && brain_) {
            // Apply recurrent signal with Hebbian-like strengthening
            float signalStrength = fromActivation * strength;
            brain_->injectCurrent(conn.second, signalStrength);
            
            // Update synaptic strength based on co-activation
            uint64_t toKey = std::hash<uint64_t>()(conn.second);
            if (pImpl->eligibilityTraces.find(toKey) != pImpl->eligibilityTraces.end()) {
                pImpl->synapticStrengths[connKey] = std::min(1.0f, strength + 0.01f * fromActivation);
            }
        }
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        // Decay activation based on resource availability
        memoryActivations_[i] *= (1.0f - decayRate_);
        
        // Decay resource allocation
        resourceAllocation[i] *= (1.0f - decayRate_);
        
        // Check if trace is too weak (below threshold and low resources)
        if (memoryActivations_[i] < 0.01f && resourceAllocation[i] < 0.05f * pImpl->totalResourceBudget) {
            toRemove.push_back(i);
            NLM_LOG_DEBUG("Removing weak memory trace at neuron " + std::to_string(memoryNeurons_[i].getId()));
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t idx = *it;
        
        // Clean up synaptic connections involving this neuron
        std::vector<std::pair<NeuronId, NeuronId>> newConnections;
        for (const auto& conn : pImpl->maintenanceSynapses) {
            if (conn.first.getId() != memoryNeurons_[idx].getId() && 
                conn.second.getId() != memoryNeurons_[idx].getId()) {
                newConnections.push_back(conn);
            }
        }
        pImpl->maintenanceSynapses = newConnections;
        
        // Remove from synaptic strength cache
        uint64_t neuronKey = memoryNeurons_[idx].value;
        for (auto it2 = pImpl->synapticStrengths.begin(); it2 != pImpl->synapticStrengths.end(); ) {
            if (it2->first == neuronKey) {
                it2 = pImpl->synapticStrengths.erase(it2);
            } else {
                ++it2;
            }
        }
        
        // Remove from eligibility traces
        pImpl->eligibilityTraces.erase(neuronKey);
        
        // Remove from tracking vectors
        memoryNeurons_.erase(memoryNeurons_.begin() + idx);
        memoryActivations_.erase(memoryActivations_.begin() + idx);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
        resourceAllocation.erase(resourceAllocation.begin() + idx);
        
        // Update active traces
        for (auto& trace : activeTraces_) {
            if (trace > idx) {
                trace--;
            } else if (trace == idx) {
                toRemove.push_back(trace);
            }
        }
    }
    
    // Rebuild active traces
    activeTraces_.clear();
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (resourceAllocation[i] > minThreshold) {
            activeTraces_.push_back(i);
        }
    }
}

void NeuralWorkingMemory::updateEligibilityTraces(NeuronId neuron, float activation) {
    uint64_t neuronKey = neuron.value;
    
    if (pImpl->eligibilityTraces.find(neuronKey) == pImpl->eligibilityTraces.end()) {
        pImpl->eligibilityTraces[neuronKey] = 0.0f;
    }
    
    // Update eligibility trace based on activation (Hebbian-like)
    pImpl->eligibilityTraces[neuronKey] += activation * 0.1f;
    
    // Constrain eligibility trace
    pImpl->eligibilityTraces[neuronKey] = std::min(1.0f, pImpl->eligibilityTraces[neuronKey]);
}

} // namespace nlm
