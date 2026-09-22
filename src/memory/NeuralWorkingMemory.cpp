// NeuralWorkingMemory Implementation
// Phase 6: Working Memory System with Neural Dynamics
// Implements persistent activity, recurrent connections, and competitive selection

#include "NeuralWorkingMemory.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    // Memory trace with neural activity patterns
    struct MemoryTrace {
        std::vector<NeuronId> neurons;
        std::vector<float> activations;
        std::vector<SimulationStep> timestamps;
        
        // Recurrent connections for maintenance
        std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections;
        
        // Trace strength and decay
        float strength;
        float decayRate;
        SimulationStep age;
        
        MemoryTrace() : strength(1.0f), decayRate(0.01f), age(0), strength(1.0f) {}
    };
    
    // All active memory traces
    std::vector<MemoryTrace> traces;
    
    // Competition and selection
    static constexpr float COMPETITION_THRESHOLD = 0.3f;  // Minimum strength to compete
    static constexpr float WINNER_BOOST_FACTOR = 2.0f;     // How much winners get strengthened
    static constexpr float MEMORY_DECAY_RATE = 0.95f;     // Decay per timestep
    static constexpr float MAX_TRACE_AGE = 10000;          // Max age before decay
    
    // Recurrent connection strength
    static constexpr float RECURRENT_STRENGTH = 0.3f;     // Baseline recurrent connection
    static constexpr float REINFORCEMENT_FACTOR = 1.5f;   // How much reactivation strengthens
    
    // Capacity and statistics
    size_t capacity_;
    size_t activeTraceCount;
    
    // Temporary competition state
    std::vector<float> traceScores;
    std::vector<size_t> winnerIndices;
    
    Impl() : capacity_(10), activeTraceCount(0) {
    }
};

NeuralWorkingMemory::NeuralWorkingMemory() : pImpl(std::make_unique<Impl>()) {
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    brain_ = brain;
    
    // Initialize with empty state
    pImpl->traces.clear();
    pImpl->activeTraceCount = 0;
    pImpl->traceScores.clear();
    pImpl->winnerIndices.clear();
    
    // Set default parameters
    pImpl->capacity_ = 10;
    pImpl->decayRate_ = 0.01f;
    
    NLM_LOG_INFO("NeuralWorkingMemory initialized");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (!brain_ || pattern.empty()) {
        return;
    }
    
    // Check if pattern fits within capacity
    if (pImpl->traces.size() >= pImpl->capacity_) {
        // Remove weakest trace to make room
        size_t weakestIndex = findWeakestTrace();
        if (weakestIndex < pImpl->traces.size()) {
            pImpl->traces.erase(pImpl->traces.begin() + weakestIndex);
        }
    }
    
    // Create new trace from pattern
    MemoryTrace newTrace;
    
    // Convert pattern to neural activity
    // For each element in pattern, create corresponding neuron activity
    newTrace.neurons.reserve(pattern.size());
    newTrace.activations.reserve(pattern.size());
    newTrace.timestamps.reserve(pattern.size());
    
    for (size_t i = 0; i < pattern.size(); ++i) {
        // Find neuron in brain that can represent this pattern element
        NeuronId neuron = findOrCreateNeuronForPattern(i, pattern[i]);
        
        if (neuron.index() != 0) {  // Valid neuron
            newTrace.neurons.push_back(neuron);
            newTrace.activations.push_back(pattern[i] * strength);
            newTrace.timestamps.push_back(brain_->getCurrentStep() ? *brain_->getCurrentStep() : 0);
        }
    }
    
    if (!newTrace.neurons.empty()) {
        newTrace.strength = strength;
        newTrace.decayRate = pImpl->decayRate_;
        newTrace.age = 0;
        
        // Create recurrent connections for maintenance
        createRecurrentConnections(newTrace);
        
        pImpl->traces.push_back(std::move(newTrace));
        pImpl->activeTraceCount++;
        
        // Create neurally-coupled storage in the brain
        storeToNeuralSystem(newTrace);
        
        NLM_LOG_INFO("Stored working memory trace with " + std::to_string(newTrace.neurons.size()) +
                    " neurons, strength: " + std::to_string(strength));
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find existing trace for this neuron
    for (auto& trace : pImpl->traces) {
        auto it = std::find(trace.neurons.begin(), trace.neurons.end(), neuron);
        if (it != trace.neurons.end()) {
            // Update existing trace
            size_t index = std::distance(trace.neurons.begin(), it);
            if (index < trace.activations.size()) {
                trace.activations[index] = activation;
                trace.strength = std::max(trace.strength, activation);
                return;
            }
        }
    }
    
    // Create new trace for this single neuron
    MemoryTrace newTrace;
    newTrace.neurons.push_back(neuron);
    newTrace.activations.push_back(activation);
    newTrace.timestamps.push_back(brain_->getCurrentStep() ? *brain_->getCurrentStep() : 0);
    newTrace.strength = activation;
    newTrace.decayRate = pImpl->decayRate_;
    newTrace.age = 0;
    
    pImpl->traces.push_back(std::move(newTrace));
    pImpl->activeTraceCount++;
    
    // Create recurrent connection for maintenance
    createRecurrentConnections(pImpl->traces.back());
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    // Retrieve current content of all active traces
    std::vector<float> result;
    
    for (const auto& trace : pImpl->traces) {
        if (trace.age < MAX_TRACE_AGE) {
            // Calculate current activation level based on trace strength and decay
            float currentActivation = trace.strength * std::pow(MEMORY_DECAY_RATE, trace.age);
            
            // Combine with any recurrent reinforcement
            for (const auto& neuron : trace.neurons) {
                // Get current neural activity
                float neuralActivity = 0.0f;
                if (brain_) {
                    // Find neuron and get its activity level
                    auto* n = findNeuronById(neuron);
                    if (n) {
                        neuralActivity = n->isFiring() ? 1.0f : n->getFiringRate() * 0.1f;
                    }
                }
                
                // Combine with stored activation
                result.push_back(currentActivation * (0.7f + 0.3f * neuralActivity));
            }
        }
    }
    
    return result;
}

bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    for (const auto& trace : pImpl->traces) {
        if (std::find(trace.neurons.begin(), trace.neurons.end(), neuron) != trace.neurons.end()) {
            return true;
        }
    }
    return false;
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    for (const auto& trace : pImpl->traces) {
        auto it = std::find(trace.neurons.begin(), trace.neurons.end(), neuron);
        if (it != trace.neurons.end()) {
            size_t index = std::distance(trace.neurons.begin(), it);
            if (index < trace.activations.size()) {
                return trace.activations[index];
            }
        }
    }
    return 0.0f;
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    // Update all traces
    for (auto& trace : pImpl->traces) {
        trace.age++;
        
        // Decay strength based on age
        trace.strength *= std::pow(MEMORY_DECAY_RATE, dt * 10.0f);
        
        // Update recurrent connections based on current neural activity
        updateRecurrentConnections(trace);
    }
    
    // Run competition to select winners
    runCompetition();
    
    // Update neural system to reflect current working memory state
    updateNeuralSystemFromTraces();
}

void NeuralWorkingMemory::clear() {
    pImpl->traces.clear();
    pImpl->activeTraceCount = 0;
    pImpl->traceScores.clear();
    pImpl->winnerIndices.clear();
    
    // Clear recurrent connections from brain
    clearNeuralSystemRecurrentConnections();
    
    NLM_LOG_INFO("NeuralWorkingMemory cleared");
}

NeuronId NeuralWorkingMemory::findOrCreateNeuronForPattern(size_t index, float value) {
    // Try to find existing neuron for this pattern element
    // In a real implementation, this would map pattern indices to actual neurons
    
    // For now, create simple mapping based on index
    // This is a simplified implementation - real system would have
    // proper neuron indexing and mapping
    
    if (!brain_) {
        return NeuronId(0);
    }
    
    // Simple mapping: index + offset
    NeuronId candidate(index + 1000);  // Use IDs in range 1000-1999 for working memory
    
    // Check if this neuron exists in brain
    auto* neuron = findNeuronById(candidate);
    if (neuron) {
        return candidate;
    }
    
    // In full implementation, would create new neurons or find suitable existing ones
    // For now, return invalid ID
    return NeuronId(0);
}

Neuron* NeuralWorkingMemory::findNeuronById(NeuronId neuronId) {
    if (!brain_) {
        return nullptr;
    }
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron->getId() == neuronId) {
                    return neuron;
                }
            }
        }
    }
    
    return nullptr;
}

void NeuralWorkingMemory::createRecurrentConnections(MemoryTrace& trace) {
    // Create recurrent connections within the trace to maintain activity
    if (trace.neurons.size() < 2) {
        return;  // Need at least 2 neurons for connections
    }
    
    // Connect neurons within the trace
    for (size_t i = 0; i < trace.neurons.size(); ++i) {
        for (size_t j = 0; j < trace.neurons.size(); ++j) {
            if (i != j) {
                // Create recurrent connection from neuron i to neuron j
                trace.recurrentConnections.emplace_back(trace.neurons[i], trace.neurons[j]);
                
                // In full implementation, would actually create synapses
            }
        }
    }
}

void NeuralWorkingMemory::updateRecurrentConnections(MemoryTrace& trace) {
    // Update recurrent connections based on current neural activity
    
    for (const auto& conn : trace.recurrentConnections) {
        auto* sourceNeuron = findNeuronById(conn.first);
        auto* targetNeuron = findNeuronById(conn.second);
        
        if (sourceNeuron && targetNeuron) {
            // Update connection strength based on source activity
            if (sourceNeuron->isFiring()) {
                // Strengthen connection if source is firing
                targetNeuron->receiveExcitatoryInput(RECURRENT_STRENGTH);
            }
            
            // Apply baseline connection
            targetNeuron->receiveExcitatoryInput(RECURRENT_STRENGTH * 0.1f);
        }
    }
}

void NeuralWorkingMemory::runCompetition() {
    // Run competition between all active traces
    if (pImpl->traces.empty()) {
        return;
    }
    
    pImpl->traceScores.clear();
    pImpl->traceScores.resize(pImpl->traces.size(), 0.0f);
    
    // Calculate scores for each trace
    for (size_t i = 0; i < pImpl->traces.size(); ++i) {
        auto& trace = pImpl->traces[i];
        
        if (trace.age >= MAX_TRACE_AGE) {
            // Old traces get low score
            pImpl->traceScores[i] = 0.0f;
            continue;
        }
        
        // Base score on trace strength
        float baseScore = trace.strength;
        
        // Add age penalty
        float agePenalty = 1.0f - std::min(1.0f, static_cast<float>(trace.age) / MAX_TRACE_AGE);
        float ageScore = baseScore * agePenalty;
        
        // Add neural coherence bonus (how well neural activity matches trace)
        float coherenceScore = calculateTraceCoherence(trace);
        
        // Combine scores
        pImpl->traceScores[i] = ageScore * 0.7f + coherenceScore * 0.3f;
    }
    
    // Select winners using competitive process
    selectWinners();
}

float NeuralWorkingMemory::calculateTraceCoherence(const MemoryTrace& trace) const {
    if (trace.neurons.empty() || !brain_) {
        return 0.0f;
    }
    
    float totalCoherence = 0.0f;
    float validNeurons = 0.0f;
    
    for (const auto& neuronId : trace.neurons) {
        auto* neuron = findNeuronById(neuronId);
        if (neuron) {
            // Calculate coherence between stored activation and current neural activity
            float storedActivation = 0.0f;
            // Find corresponding stored activation
            // In full implementation, would track which activation belongs to which neuron
            
            float currentActivity = neuron->isFiring() ? 1.0f : neuron->getFiringRate() * 0.1f;
            float coherence = 1.0f - std::abs(currentActivity - storedActivation);
            
            totalCoherence += coherence;
            validNeurons += 1.0f;
        }
    }
    
    return validNeurons > 0.0f ? totalCoherence / validNeurons : 0.0f;
}

void NeuralWorkingMemory::selectWinners() {
    // Select winning traces using competitive process
    pImpl->winnerIndices.clear();
    
    if (pImpl->traceScores.empty()) {
        return;
    }
    
    // Find all traces above threshold
    std::vector<size_t> candidates;
    for (size_t i = 0; i < pImpl->traceScores.size(); ++i) {
        if (pImpl->traceScores[i] >= COMPETITION_THRESHOLD) {
            candidates.push_back(i);
        }
    }
    
    if (candidates.empty()) {
        return;  // No traces meet threshold
    }
    
    // Sort by score (highest first)
    std::sort(candidates.begin(), candidates.end(),
              [this](size_t a, size_t b) {
                  return pImpl->traceScores[a] > pImpl->traceScores[b];
              });
    
    // Select a subset as winners (typically 1-2 winners)
    size_t numWinners = std::min(size_t(2), candidates.size());
    for (size_t i = 0; i < numWinners; ++i) {
        pImpl->winnerIndices.push_back(candidates[i]);
    }
    
    // Boost winning traces
    for (size_t winnerIdx : pImpl->winnerIndices) {
        if (winnerIdx < pImpl->traces.size()) {
            pImpl->traces[winnerIdx].strength *= WINNER_BOOST_FACTOR;
        }
    }
}

void NeuralWorkingMemory::updateNeuralSystemFromTraces() {
    // Update brain neural activity to reflect current working memory state
    
    if (!brain_) {
        return;
    }
    
    // Clear all working memory activity
    clearWorkingMemoryNeuralActivity();
    
    // Activate neurons that are part of winning traces
    for (size_t winnerIdx : pImpl->winnerIndices) {
        if (winnerIdx < pImpl->traces.size()) {
            const auto& trace = pImpl->traces[winnerIdx];
            
            for (const auto& neuronId : trace.neurons) {
                // Activate this neuron
                auto* neuron = findNeuronById(neuronId);
                if (neuron) {
                    neuron->injectCurrent(1.0f);  // Strong current injection for persistence
                    neuron->setFiringState(FiringState::Active);
                }
            }
        }
    }
}

void NeuralWorkingMemory::storeToNeuralSystem(const MemoryTrace& trace) {
    // Store working memory content in the brain's neural system
    // Creates persistent neural activity patterns
    
    if (!brain_) {
        return;
    }
    
    for (const auto& neuronId : trace.neurons) {
        auto* neuron = findNeuronById(neuronId);
        if (neuron) {
            // Store initial activation level
            neuron->injectCurrent(trace.activations[0]);
            
            // Create recurrent connections for maintenance
            createRecurrentMaintenanceConnections(neuron);
        }
    }
}

void NeuralWorkingMemory::createRecurrentMaintenanceConnections(Neuron* neuron) {
    // Create recurrent connections that help maintain working memory activity
    if (!neuron || !brain_) {
        return;
    }
    
    // Find neurons in same population for recurrent connections
    // In full implementation, would create actual synaptic connections
    // For now, create placeholder for maintenance
}

void NeuralWorkingMemory::clearWorkingMemoryNeuralActivity() {
    // Clear working memory related neural activity
    if (!brain_) {
        return;
    }
    
    // Deactivate all working memory neurons
    // In full implementation, would identify and deactivate
}

void NeuralWorkingMemory::clearNeuralSystemRecurrentConnections() {
    // Clear all working memory recurrent connections
    // In full implementation, would remove connections
}

size_t NeuralWorkingMemory::findWeakestTrace() {
    if (pImpl->traces.empty()) {
        return 0;
    }
    
    size_t weakestIndex = 0;
    float weakestStrength = pImpl->traces[0].strength;
    
    for (size_t i = 1; i < pImpl->traces.size(); ++i) {
        if (pImpl->traces[i].strength < weakestStrength) {
            weakestStrength = pImpl->traces[i].strength;
            weakestIndex = i;
        }
    }
    
    return weakestIndex;
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    // Strengthen all memory traces by given factor
    for (auto& trace : pImpl->traces) {
        trace.strength *= factor;
    }
}

bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    for (size_t winnerIdx : pImpl->winnerIndices) {
        if (winnerIdx < pImpl->traces.size()) {
            const auto& trace = pImpl->traces[winnerIdx];
            if (std::find(trace.neurons.begin(), trace.neurons.end(), neuron) != trace.neurons.end()) {
                return true;
            }
        }
    }
    return false;
}

float NeuralWorkingMemory::getMemoryActivity() const {
    float totalActivity = 0.0f;
    size_t validTraces = 0;
    
    for (const auto& trace : pImpl->traces) {
        if (trace.age < MAX_TRACE_AGE) {
            totalActivity += trace.strength;
            validTraces++;
        }
    }
    
    return validTraces > 0 ? totalActivity / validTraces : 0.0f;
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Override for compatibility - calls per-trace update
    // Each trace updates its own connections
}

void NeuralWorkingMemory::decayWeakTraces() {
    // Remove traces that have decayed below threshold
    auto it = std::remove_if(pImpl->traces.begin(), pImpl->traces.end(),
                            [this](const MemoryTrace& trace) {
                                return trace.age >= MAX_TRACE_AGE || trace.strength < 0.01f;
                            });
    
    pImpl->traces.erase(it, pImpl->traces.end());
    pImpl->activeTraceCount = pImpl->traces.size();
}

std::string NeuralWorkingMemory::getStats() const {
    std::ostringstream oss;
    oss << "=== NeuralWorkingMemory Statistics ===\n";
    oss << "Capacity: " << pImpl->capacity_ << "\n";
    oss << "Active traces: " << pImpl->activeTraceCount << "\n";
    oss << "Total traces stored: " << pImpl->traces.size() << "\n";
    oss << "Decay rate: " << pImpl->decayRate_ << "\n";
    oss << "Average trace strength: " << calculateAverageTraceStrength() << "\n";
    oss << "Winner traces: " << pImpl->winnerIndices.size() << "\n";
    oss << "Memory activity level: " << getMemoryActivity() << "\n";
    
    return oss.str();
}

float NeuralWorkingMemory::calculateAverageTraceStrength() const {
    if (pImpl->traces.empty()) {
        return 0.0f;
    }
    
    float totalStrength = 0.0f;
    for (const auto& trace : pImpl->traces) {
        totalStrength += trace.strength;
    }
    
    return totalStrength / pImpl->traces.size();
}

} // namespace nlm
