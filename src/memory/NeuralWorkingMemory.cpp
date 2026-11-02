// Enhanced NeuralWorkingMemory with Advanced Features

#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/utils/BrainUtils.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_set>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Enhanced maintenance connections with plasticity
    std::vector<std::tuple<NeuronId, NeuronId, float, float>> maintenanceSynapses;
    
    // Memory trace metadata
    struct Trace {
        NeuronId neuronId;
        float activation;
        SimulationStep age;
        float decayFactor;
        float stabilityScore;
        SimulationStep lastUpdated;
        bool isReinforced;
        float noveltySignal;
    };
    
    std::vector<Trace> traces;
    
    // Competition system
    std::vector<size_t> winnerIndices;
    float competitionThreshold;
    
    // Hebbian learning for maintenance connections
    std::vector<std::pair<NeuronId, NeuronId>> hebbianConnections;
    
    // Temporal context
    SimulationStep currentStep;
    float temporalDecay;
    
    // Integration with attentional system
    std::vector<NeuronId> attendedNeurons;
    
    Impl() 
        : brain(nullptr)
        , competitionThreshold(0.5f)
        , currentStep(0)
        , temporalDecay(0.005f) {}
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
    , pImpl(new Impl())
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
    , competitionThreshold_(0.5f)
    , currentStep_(0)
    , temporalDecay_(0.005f)
{
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize with enhanced parameters
    pImpl->traces.clear();
    pImpl->winnerIndices.clear();
    pImpl->hebbianConnections.clear();
    pImpl->attendedNeurons.clear();
    
    NLM_LOG_INFO("Enhanced NeuralWorkingMemory initialized with capacity: " + std::to_string(capacity_));
    
    // Initialize recurrent connections based on brain structure
    initializeRecurrentConnections();
}

void NeuralWorkingMemory::initializeRecurrentConnections() {
    if (!brain_) return;
    
    // Create a ring of recurrent connections for maintenance
    for (auto& region : brain_->getRegions()) {
        auto neurons = region->getAllNeurons();
        if (neurons.size() < 2) continue;
        
        // Connect neurons in a chain
        for (size_t i = 0; i < neurons.size() - 1; ++i) {
            createRecurrentConnection(neurons[i]->getId(), neurons[i+1]->getId(), 
                                   0.3f, 0.1f); // strength, delay
        }
        
        // Close the loop for a ring attractor
        if (!neurons.empty()) {
            createRecurrentConnection(neurons.back()->getId(), neurons.front()->getId(),
                                   0.3f, 0.1f);
        }
    }
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) {
        BrainLogger::getInstance().logWarning("Cannot store empty pattern or when brain is not initialized");
        return;
    }
    
    // Enhanced validation
    float clampedStrength = std::clamp(strength, 0.0f, 10.0f);
    std::vector<float> clampedPattern;
    clampedPattern.reserve(pattern.size());
    for (float v : pattern) {
        clampedPattern.push_back(std::clamp(v, 0.0f, 1.0f));
    }
    
    SimulationStep currentTime = pImpl->currentStep;
    
    // Update or create memory traces
    for (size_t i = 0; i < std::min(clampedPattern.size(), pImpl->traces.size()); ++i) {
        updateTrace(i, clampedPattern[i], clampedStrength, currentTime);
    }
    
    // Add new traces if needed
    for (size_t i = clampedPattern.size(); i < capacity_; ++i) {
        NeuronId randomNeuron = generateRandomNeuron();
        if (randomNeuron.index() > 0) {
            pImpl->traces.push_back({
                randomNeuron,
                0.0f,
                currentTime,
                1.0f,  // decay factor
                0.0f,  // stability score
                currentTime,
                false,
                0.0f   // novelty signal
            });
        }
    }
    
    // Prune weak traces
    pruneWeakTraces();
    
    // Run competition
    runCompetition();
    
    // Apply attention modulation
    applyAttentionModulation();
    
    // Create hebbian connections for pattern
    createPatternHebbianConnections(clampedPattern, clampedStrength);
}

void NeuralWorkingMemory::updateTrace(size_t index, float activation, float strength, SimulationStep time) {
    if (index >= pImpl->traces.size()) return;
    
    auto& trace = pImpl->traces[index];
    
    // Enhanced trace update with temporal integration
    float oldActivation = trace.activation;
    
    // Apply strength factor
    activation *= strength;
    
    // Temporal integration for stability
    float temporalFactor = std::exp(-static_cast<float>(time - trace.age) * pImpl->temporalDecay);
    activation = oldActivation * temporalFactor + activation * (1.0f - temporalFactor);
    
    // Update trace
    trace.activation = activation;
    trace.age = time;
    trace.lastUpdated = time;
    trace.isReinforced = (strength > 1.0f);
    
    // Update stability score based on persistence
    trace.stabilityScore = std::min(1.0f, trace.stabilityScore + 0.01f * strength);
    
    // Determine if trace is active
    bool wasActive = std::find(pImpl->activeTraces_.begin(), pImpl->activeTraces_.end(), index) != pImpl->activeTraces_.end();
    bool isActive = activation > 0.1f;
    
    if (isActive && !wasActive) {
        pImpl->activeTraces_.push_back(index);
    } else if (!isActive && wasActive) {
        pImpl->activeTraces_.erase(std::remove(pImpl->activeTraces_.begin(), pImpl->activeTraces_.end(), index),
                                  pImpl->activeTraces_.end());
    }
}

NeuronId NeuralWorkingMemory::generateRandomNeuron() const {
    if (!brain_) return NeuronId(0);
    
    // Get all available neurons
    std::vector<NeuronId> allNeurons;
    for (auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                allNeurons.push_back(neuron->getId());
            }
        }
    }
    
    if (allNeurons.empty()) return NeuronId(0);
    
    // Random selection
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, allNeurons.size() - 1);
    
    return allNeurons[dist(gen)];
}

void NeuralWorkingMemory::pruneWeakTraces() {
    // Remove traces with very low activation
    pImpl->activeTraces_.erase(std::remove_if(pImpl->activeTraces_.begin(), pImpl->activeTraces_.end(),
                                              [this](size_t idx) {
                                                  if (idx >= pImpl->traces.size()) return true;
                                                  return pImpl->traces[idx].activation <= 0.01f;
                                              }),
                              pImpl->activeTraces_.end());
}

void NeuralWorkingMemory::runCompetition() {
    if (pImpl->traces.empty()) return;
    
    // Enhanced competition algorithm
    std::vector<std::pair<size_t, float>> scores; // index, score
    
    for (size_t i = 0; i < pImpl->traces.size(); ++i) {
        auto& trace = pImpl->traces[i];
        
        // Base score from activation
        float score = trace.activation;
        
        // Apply stability bonus
        score *= (1.0f + trace.stabilityScore * 0.5f);
        
        // Apply novelty penalty to reduce interference
        score *= (1.0f - trace.noveltySignal * 0.2f);
        
        // Apply age penalty (older traces decay faster)
        float agePenalty = std::min(1.0f, static_cast<float>(pImpl->currentStep - trace.age) * 0.01f);
        score *= (1.0f - agePenalty);
        
        // Apply attention bonus if neuron is attended
        bool isAttended = std::find(pImpl->attendedNeurons.begin(), pImpl->attendedNeurons.end(), trace.neuronId) != pImpl->attendedNeurons.end();
        if (isAttended) {
            score *= 1.5f;
        }
        
        scores.push_back({i, score});
    }
    
    // Sort by score (highest first)
    std::sort(scores.begin(), scores.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Select winners based on threshold
    pImpl->winnerIndices.clear();
    for (const auto& pair : scores) {
        if (pair.second >= competitionThreshold_) {
            pImpl->winnerIndices.push_back(pair.first);
        } else {
            break; // Since sorted, we can stop early
        }
    }
}

void NeuralWorkingMemory::applyAttentionModulation() {
    // Integrate with attentional selection system
    if (!brain_) return;
    
    auto* attention = brain_->getAttention();
    if (!attention) return;
    
    // Update attended neurons based on attention
    pImpl->attendedNeurons.clear();
    const auto& winners = attention->getWinners();
    
    for (NeuronId neuronId : winners) {
        // Check if this neuron is in our traces
        auto it = std::find_if(pImpl->traces.begin(), pImpl->traces.end(),
                              [neuronId](const Trace& trace) { return trace.neuronId == neuronId; });
        if (it != pImpl->traces.end()) {
            size_t index = std::distance(pImpl->traces.begin(), it);
            if (std::find(pImpl->attendedNeurons.begin(), pImpl->attendedNeurons.end(), neuronId) == pImpl->attendedNeurons.end()) {
                pImpl->attendedNeurons.push_back(neuronId);
            }
        }
    }
}

void NeuralWorkingMemory::createPatternHebbianConnections(const std::vector<float>& pattern, float strength) {
    // Create additional hebbian connections based on pattern
    if (pattern.size() < 2) return;
    
    for (size_t i = 0; i < pattern.size() - 1; ++i) {
        if (pattern[i] > 0.5f && pattern[i+1] > 0.5f) {
            // Create connection between consecutive pattern neurons
            NeuronId neuron1 = memoryNeurons_[i % memoryNeurons_.size()];
            NeuronId neuron2 = memoryNeurons_[(i + 1) % memoryNeurons_.size()];
            
            createRecurrentConnection(neuron1, neuron2, strength * 0.1f, 1);
        }
    }
}

// Additional helper methods
void NeuralWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength, float delay) {
    // Create a recurrent connection with the given parameters
    auto it = std::find_if(pImpl->maintenanceSynapses.begin(), pImpl->maintenanceSynapses.end(),
                          [from, to](const auto& conn) {
                              return std::get<0>(conn) == from && std::get<1>(conn) == to;
                          });
    
    if (it == pImpl->maintenanceSynapses.end()) {
        pImpl->maintenanceSynapses.push_back({from, to, strength, delay});
        
        // Create actual neural connection
        if (brain_) {
            auto* fromRegion = brain_->getRegion(RegionId(from.index() / 1000));
            auto* toRegion = brain_->getRegion(RegionId(to.index() / 1000));
            
            if (fromRegion && toRegion) {
                Neuron* fromNeuron = nullptr, *toNeuron = nullptr;
                for (auto* n : fromRegion->getAllNeurons()) if (n->getId() == from) fromNeuron = n;
                for (auto* n : toRegion->getAllNeurons()) if (n->getId() == to) toNeuron = n;
                
                if (fromNeuron && toNeuron) {
                    // Create synaptic connection (simplified)
                    fromNeuron->addOutgoingSynapse(SynapseHandle()); // Placeholder
                    toNeuron->addIncomingSynapse(SynapseHandle());  // Placeholder
                }
            }
        }
    }
}

// Additional implementation for other methods can continue...

} // namespace nlm
