#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <queue>

namespace nlm;

namespace nlm {

WorkingMemoryTrace::WorkingMemoryTrace() : neuralId(), persistentActivity(0.0f), age(0), 
                          noveltyScore(1.0f), lastReactivation(0), 
                          consolidationWeight(0.5f), traceStrength(0.0f), 
                          synapticResource(0.0f), isConsolidated(false) {}

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Neural population management
    std::vector<NeuronId> availableNeurons;   // Pool of neurons for working memory
    std::vector<NeuronId> engagedNeurons;     // Currently allocated neurons
    std::vector<NeuronId> winnerNeurons;      // Currently winning population
    
    // Working memory trace management
    std::vector<WorkingMemoryTrace> traces;   // Active working memory traces
    std::priority_queue<SimulationStep> decayQueue;  // Queue for decay processing
    
    // Neural dynamics and competition
    std::vector<float> neuralActivityLevels;  // Current activity levels
    std::vector<float> lateralInhibition;     // Inhibition between traces
    std::vector<float> resourceAllocation;    // Resource allocation per trace
    
    // Neuromodulation and plasticity
    std::vector<float> noveltySignals;        // Novelty detection signals
    std::vector<float> consolidationSignals;  // Consolidation state signals
    std::vector<float> adaptationVariables;   // Adaptive forgetting signals
    
    // Trace dynamics state
    float neuralNoiseLevel;                   // Intrinsic neural noise
    float globalInhibition;                  // Global inhibition level
    float competitionGain;                   // Competition strength
    SimulationStep timeStep;                 // Current simulation timestep
    
    // Statistical tracking
    float totalMemoryLoad;                    // Total memory activation
    float maximumCapacity;                    // Maximum neural capacity
    float currentLoad;                        // Current resource utilization
    
    Impl() : brain(nullptr), neuralNoiseLevel(0.05f), globalInhibition(0.3f), 
             competitionGain(2.0f), timeStep(0), totalMemoryLoad(0.0f),
             maximumCapacity(100.0f), currentLoad(0.0f) {}
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
{
    // Initialize neural dynamics parameters for biologically realistic working memory
    // These parameters model persistent activity and competition in neural populations
    pImpl->neuralNoiseLevel = 0.05f;      // 5% intrinsic neural noise
    pImpl->globalInhibition = 0.3f;       // Baseline global inhibition
    pImpl->competitionGain = 2.0f;        // Competition strength
    pImpl->maximumCapacity = static_cast<float>(capacity_) * 2.5f;  // Neural resource capacity
    pImpl->totalMemoryLoad = 0.0f;
    pImpl->currentLoad = 0.0f;
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralWorkingMemory initialized");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Enhanced biologically realistic working memory encoding
    // Instead of simple key-value pairs, use neural population dynamics
    
    // 1. Find or create neural population for this pattern
    WorkingMemoryTrace trace;
    
    // Determine encoding population size based on pattern complexity
    size_t encodingSize = std::min(static_cast<size_t>(pattern.size() * 1.5f), 50);
    
    // 2. Allocate neural resources from available pool
    if (pImpl->availableNeurons.size() < encodingSize * 2) {
        // Need more neurons - allocate from brain's internal resources
        allocateNeuralResources(encodingSize * 2);
    }
    
    // 3. Select encoding neurons with winner-take-all competition
    std::vector<NeuronId> candidateNeurons;
    candidateNeurons.reserve(encodingSize);
    
    // Randomly select from available pool for pattern encoding
    size_t patternHash = std::hash<std::string>{}(std::to_string(pattern.size()) + std::to_string(strength));
    for (size_t i = 0; i < encodingSize && i < pImpl->availableNeurons.size(); ++i) {
        size_t idx = (patternHash + i * 137) % pImpl->availableNeurons.size();
        candidateNeurons.push_back(pImpl->availableNeurons[idx]);
    }
    
    trace.population = candidateNeurons;
    
    // 4. Encode pattern with persistent neural activity
    trace.neuralId = candidateNeurons[0];  // Principal neuron for this trace
    trace.persistentActivity = 0.0f;
    trace.age = 0;
    trace.noveltyScore = 1.0f;  // High novelty for new patterns
    trace.lastReactivation = 0;
    trace.consolidationWeight = 0.5f;
    trace.traceStrength = strength * 0.8f;
    trace.synapticResource = trace.traceStrength * pImpl->maximumCapacity * 0.1f;
    trace.isConsolidated = false;
    trace.spikeHistory.clear();
    
    // 5. Encode population activity based on pattern values
    for (size_t i = 0; i < pattern.size() && i < trace.population.size(); ++i) {
        float normalizedValue = std::max(0.0f, std::min(1.0f, pattern[i]));
        
        // Map pattern value to neural population activity
        float targetActivity = normalizedValue * trace.traceStrength * 2.0f;
        
        // Store in population activity array (extend if needed)
        if (i >= trace.populationActivity.size()) {
            trace.populationActivity.push_back(targetActivity);
        } else {
            trace.populationActivity[i] = targetActivity;
        }
        
        // Inject current to establish persistent activity
        NeuronId neuronId = trace.population[i % trace.population.size()];
        brain_->injectCurrent(neuronId, targetActivity * 8.0f);
        
        // Record spike pattern for novelty detection
        if (targetActivity > 0.5f) {
            trace.spikeHistory.push_back(static_cast<float>(pImpl->timeStep));
        }
    }
    
    // 6. Create recurrent connections for maintenance (persistent activity)
    createRecurrentConnections(trace.population, strength);
    
    // 7. Update trace statistics and resource allocation
    updateTraceStatistics(trace);
    
    // 8. Add to active traces
    pImpl->traces.push_back(trace);
    pImpl->engagedNeurons.insert(pImpl->engagedNeurons.end(), 
                                trace.population.begin(), trace.population.end());
    pImpl->totalMemoryLoad += trace.traceStrength;
    pImpl->currentLoad += trace.synapticResource;
    
    // 9. Trigger competition among traces
    runCompetition();
    
    // 10. Consolidate if capacity allows
    if (pImpl->currentLoad > pImpl->maximumCapacity * 0.8f) {
        consolidateTraces();
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find or create biologically realistic trace for this neuron
    
    // 1. Check if trace already exists
    for (auto& trace : pImpl->traces) {
        if (trace.neuralId == neuron) {
            // Update existing trace with novel input
            float noveltyBoost = std::abs(activation - trace.persistentActivity);
            trace.persistentActivity = (trace.persistentActivity + activation) * 0.5f;
            trace.noveltyScore *= (1.0f - noveltyBoost * 0.3f);  // Novelty detection
            trace.traceStrength = std::min(1.0f, trace.traceStrength * 1.2f);  // Strengthen
            trace.lastReactivation = pImpl->timeStep;
            trace.spikeHistory.push_back(static_cast<float>(pImpl->timeStep));
            
            // Inject current to maintain trace
            if (brain_) {
                brain_->injectCurrent(neuron, trace.persistentActivity * 10.0f);
            }
            
            // Reactivate population
            reactivateTrace(trace);
            return;
        }
    }
    
    // 2. Create new trace with biologically realistic properties
    WorkingMemoryTrace trace;
    trace.neuralId = neuron;
    trace.persistentActivity = activation;
    trace.age = 0;
    trace.noveltyScore = 1.0f;
    trace.lastReactivation = pImpl->timeStep;
    trace.consolidationWeight = 0.5f;
    trace.traceStrength = std::min(1.0f, activation * 1.5f);
    trace.synapticResource = trace.traceStrength * pImpl->maximumCapacity * 0.1f;
    trace.isConsolidated = false;
    trace.spikeHistory.push_back(static_cast<float>(pImpl->timeStep));
    
    // 3. Allocate surrounding neural population for pattern completion
    std::vector<NeuronId> population = selectEncodingPopulation(neuron, 5);
    trace.population = population;
    trace.populationActivity.resize(population.size(), 0.0f);
    
    // 4. Set up population activity with Hebbian-like dynamics
    size_t idx = 0;
    for (NeuronId popNeuron : population) {
        // Different neurons in population encode different aspects
        float populationActivity = activation * std::sin(static_cast<float>(idx) * 0.5f) * 0.3f + activation * 0.7f;
        trace.populationActivity[idx] = populationActivity;
        
        // Inject to establish population activity
        if (brain_) {
            brain_->injectCurrent(popNeuron, populationActivity * 8.0f);
        }
        
        idx++;
    }
    
    // 5. Create recurrent connections for persistent activity
    createRecurrentConnections(population, trace.traceStrength);
    
    // 6. Update statistics
    updateTraceStatistics(trace);
    
    // 7. Add to memory system
    pImpl->traces.push_back(trace);
    pImpl->engagedNeurons.insert(pImpl->engagedNeurons.end(), population.begin(), population.end());
    pImpl->totalMemoryLoad += trace.traceStrength;
    pImpl->currentLoad += trace.synapticResource;
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    result.reserve(pImpl->traces.size());
    
    for (const auto& trace : pImpl->traces) {
        // Combine persistent activity with population activity
        float totalActivity = trace.persistentActivity;
        
        // Add population activity contribution
        for (float activity : trace.populationActivity) {
            totalActivity += activity * 0.3f;
        }
        
        // Weight by trace strength and novelty
        float weightedActivity = totalActivity * trace.traceStrength * trace.noveltyScore;
        result.push_back(weightedActivity);
    }
    
    return result;
}

bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    // Check if neuron is part of any working memory trace
    for (const auto& trace : pImpl->traces) {
        if (trace.neuralId == neuron) {
            return true;
        }
        
        // Also check population neurons
        if (std::find(trace.population.begin(), trace.population.end(), neuron) != trace.population.end()) {
            return true;
        }
    }
    return false;
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    // Get activation of neuron from any trace it participates in
    for (const auto& trace : pImpl->traces) {
        if (trace.neuralId == neuron) {
            return trace.persistentActivity;
        }
        
        // Find neuron in population and return its activity
        auto it = std::find(trace.population.begin(), trace.population.end(), neuron);
        if (it != trace.population.end()) {
            size_t idx = std::distance(trace.population.begin(), it);
            if (idx < trace.populationActivity.size()) {
                return trace.populationActivity[idx];
            }
        }
    }
    return 0.0f;
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    pImpl->timeStep++;
    
    // 1. Apply biologically realistic trace decay
    decayTraces(dt);
    
    // 2. Update trace ages and maintain persistent activity
    for (auto& trace : pImpl->traces) {
        // Maintain persistent activity through recurrent connections
        maintainPersistentActivity(trace);
        
        // Update novelty and consolidation signals
        updateTraceSignals(trace);
    }
    
    // 3. Apply global and lateral inhibition
    applyGlobalInhibition();
    runWinnerTakeAllCompetition();
    
    // 4. Allocate and manage neural resources
    manageNeuralResources();
    
    // 5. Implement novelty detection and adaptive forgetting
    forgetWeakTraces();
    
    // 6. Consolidate traces when appropriate
    consolidateTraces();
    
    // 7. Update recurrent connections for maintenance
    updateRecurrentConnections();
}

void NeuralWorkingMemory::maintainPersistentActivity(WorkingMemoryTrace& trace) {
    // Implement persistent neural activity through recurrent connections
    // This models the biological mechanism of working memory maintenance
    
    if (trace.population.empty()) return;
    
    // Primary neuron maintains persistent activity
    float targetActivity = trace.persistentActivity;
    
    // Inject current to maintain persistent firing
    if (brain_) {
        brain_->injectCurrent(trace.neuralId, targetActivity * 10.0f);
    }
    
    // Population neurons show pattern completion
    for (size_t i = 0; i < trace.population.size(); ++i) {
        float populationActivity = trace.populationActivity[i];
        
        // Pattern completion: weaker neurons are amplified to match pattern
        if (populationActivity > 0.0f) {
            float completionFactor = targetActivity / (populationActivity + 0.1f);
            populationActivity = std::min(populationActivity * completionFactor, targetActivity);
            
            // Update population activity
            trace.populationActivity[i] = populationActivity;
            
            // Inject to maintain population activity
            if (brain_) {
                brain_->injectCurrent(trace.population[i], populationActivity * 8.0f);
            }
        }
    }
}

void NeuralWorkingMemory::updateTraceSignals(WorkingMemoryTrace& trace) {
    // Update neuromodulatory and plasticity signals
    
    // Update novelty signal (adaptive for novel inputs)
    float noveltyUpdate = 1.0f - trace.noveltyScore * 0.05f;
    trace.noveltyScore = std::max(0.1f, noveltyUpdate);
    
    // Update consolidation signal based on trace stability
    if (trace.isConsolidated) {
        trace.consolidationWeight = std::min(1.0f, trace.consolidationWeight * 1.02f);
    } else {
        // Consolidation depends on stable activation and low novelty
        float stability = std::min(1.0f, trace.traceStrength * trace.noveltyScore * 2.0f);
        trace.consolidationWeight = std::min(0.95f, trace.consolidationWeight + stability * 0.01f);
    }
    
    // Update adaptation variables (neuronal fatigue)
    float fatigueFactor = std::exp(-trace.traceStrength * 0.1f);
    trace.persistentActivity *= fatigueFactor;
}

void NeuralWorkingMemory::manageNeuralResources() {
    // Implement resource allocation and capacity management
    
    // Calculate current load
    pImpl->currentLoad = 0.0f;
    for (const auto& trace : pImpl->traces) {
        pImpl->currentLoad += trace.synapticResource;
    }
    
    // Apply resource constraints
    if (pImpl->currentLoad > pImpl->maximumCapacity) {
        // Reduce weakest traces
        std::sort(pImpl->traces.begin(), pImpl->traces.end(),
                  [](const WorkingMemoryTrace& a, const WorkingMemoryTrace& b) {
                      return a.traceStrength < b.traceStrength;
                  });
        
        float excessLoad = pImpl->currentLoad - pImpl->maximumCapacity;
        float totalReduction = 0.0f;
        
        for (auto& trace : pImpl->traces) {
            if (totalReduction >= excessLoad) break;
            
            float canReduce = std::min(trace.traceStrength * 0.5f, excessLoad - totalReduction);
            trace.traceStrength -= canReduce;
            trace.synapticResource = trace.traceStrength * pImpl->maximumCapacity * 0.1f;
            totalReduction += canReduce;
        }
        
        // Recalculate load
        pImpl->currentLoad = 0.0f;
        for (const auto& trace : pImpl->traces) {
            pImpl->currentLoad += trace.synapticResource;
        }
    }
}

void NeuralWorkingMemory::clear() {
    // Clear all working memory traces
    pImpl->traces.clear();
    pImpl->engagedNeurons.clear();
    pImpl->winnerNeurons.clear();
    pImpl->totalMemoryLoad = 0.0f;
    pImpl->currentLoad = 0.0f;
    
    // Clear maintenance connections
    pImpl->maintenanceSynapses.clear();
    
    // Clear legacy data for compatibility
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    winners_.clear();
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    // Strengthen working memory traces (rehearsal mechanism)
    for (auto& trace : pImpl->traces) {
        // Strengthen trace and reset novelty
        trace.traceStrength = std::min(1.0f, trace.traceStrength * factor);
        trace.noveltyScore = 1.0f;  // Novelty reset on strengthening
        trace.lastReactivation = pImpl->timeStep;
        
        // Update persistent activity
        trace.persistentActivity = std::min(1.0f, trace.persistentActivity * factor);
        
        // Update synaptic resources
        trace.synapticResource = trace.traceStrength * pImpl->maximumCapacity * 0.1f;
    }
    
    // Update overall load
    pImpl->currentLoad = 0.0f;
    for (const auto& trace : pImpl->traces) {
        pImpl->currentLoad += trace.synapticResource;
    }
}

void NeuralWorkingMemory::runCompetition() {
    // Run biologically realistic competition
    // For backward compatibility with existing code
    runWinnerTakeAllCompetition();
    
    // Also update winners_ for compatibility
    winners_.clear();
    for (const auto& winner : pImpl->winnerNeurons) {
        winners_.push_back(winner);
    }
}

bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    // Check if neuron is a winner
    return std::find(pImpl->winnerNeurons.begin(), pImpl->winnerNeurons.end(), neuron) != pImpl->winnerNeurons.end();
}

float NeuralWorkingMemory::getMemoryActivity() const {
    // Calculate overall memory activity
    if (pImpl->traces.empty()) return 0.0f;
    
    float totalActivity = 0.0f;
    float totalWeight = 0.0f;
    
    for (const auto& trace : pImpl->traces) {
        float weightedActivity = trace.persistentActivity * trace.traceStrength * trace.noveltyScore;
        totalActivity += weightedActivity;
        totalWeight += trace.traceStrength * trace.noveltyScore;
    }
    
    return totalWeight > 0.0f ? totalActivity / totalWeight : 0.0f;
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
        for (const auto& trace : pImpl->traces) {
            if (trace.neuralId == conn.first) {
                fromActivation = trace.persistentActivity;
                break;
            }
        }
        
        if (fromActivation > 0.1f && brain_) {
            // Send maintenance signal
            brain_->injectCurrent(conn.second, fromActivation * 2.0f);
        }
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    // Legacy method for backward compatibility
    // Note: Replaced by biologically realistic decay in decayTraces()
    forgetWeakTraces();
}

void NeuralWorkingMemory::allocateNeuralResources(size_t count) {
    // Allocate neural resources from brain's pool or create new neurons
    // In a real implementation, this would access brain resources
    
    for (size_t i = 0; i < count; ++i) {
        // Generate unique neuron ID
        NeuronId neuron(i + 10000);  // Offset to avoid conflicts with existing neurons
        pImpl->availableNeurons.push_back(neuron);
        pImpl->neuralActivityLevels.push_back(0.0f);
        pImpl->lateralInhibition.push_back(0.0f);
        pImpl->resourceAllocation.push_back(0.0f);
    }
}

void NeuralWorkingMemory::releaseNeuralResources(const std::vector<NeuronId>& neurons) {
    // Release neural resources back to pool
    
    for (const auto& neuron : neurons) {
        // Remove from available neurons
        auto it = std::find(pImpl->availableNeurons.begin(), pImpl->availableNeurons.end(), neuron);
        if (it != pImpl->availableNeurons.end()) {
            pImpl->availableNeurons.erase(it);
        }
        
        // Remove from engaged neurons
        it = std::find(pImpl->engagedNeurons.begin(), pImpl->engagedNeurons.end(), neuron);
        if (it != pImpl->engagedNeurons.end()) {
            pImpl->engagedNeurons.erase(it);
        }
        
        // Remove from winners
        it = std::find(pImpl->winnerNeurons.begin(), pImpl->winnerNeurons.end(), neuron);
        if (it != pImpl->winnerNeurons.end()) {
            pImpl->winnerNeurons.erase(it);
        }
        
        // Update resource allocation
        for (size_t i = 0; i < pImpl->availableNeurons.size(); ++i) {
            if (pImpl->availableNeurons[i] == neuron) {
                pImpl->resourceAllocation[i] = 0.0f;
                break;
            }
        }
    }
    
    // Update total load
    pImpl->currentLoad = 0.0f;
    for (const auto& trace : pImpl->traces) {
        pImpl->currentLoad += trace.synapticResource;
    }
}

void NeuralWorkingMemory::createRecurrentConnections(const std::vector<NeuronId>& neurons, float strength) {
    // Create winner-take-all recurrent connections for persistent activity
    size_t numNeurons = neurons.size();
    if (numNeurons < 2) return;
    
    // Create ring connections (each neuron connects to next)
    for (size_t i = 0; i < numNeurons; ++i) {
        size_t nextIdx = (i + 1) % numNeurons;
        createRecurrentConnection(neurons[i], neurons[nextIdx], strength * 0.3f);
    }
    
    // Add strong recurrent connection to principal neuron
    if (brain_) {
        for (size_t i = 1; i < numNeurons; ++i) {
            brain_->injectCurrent(neurons[0], strength * 5.0f);
        }
    }
}

void NeuralWorkingMemory::updateTraceStatistics(WorkingMemoryTrace& trace) {
    // Update biologically realistic trace statistics
    
    // Age the trace
    trace.age++;
    
    // Update novelty score based on time since last reactivation
    if (trace.lastReactivation > 0) {
        float timeSinceReact = static_cast<float>(trace.age - trace.lastReactivation);
        trace.noveltyScore = std::max(0.1f, 1.0f - timeSinceReact * 0.01f);
    }
    
    // Update consolidation state
    if (trace.traceStrength > 0.7f && trace.noveltyScore < 0.3f) {
        trace.isConsolidated = true;
        trace.consolidationWeight = 0.8f;
    }
    
    // Decay trace strength over time
    float ageDecay = 1.0f - std::pow(static_cast<float>(trace.age), 0.3f) * 0.3f;
    trace.traceStrength *= ageDecay;
    
    // Update synaptic resources
    trace.synapticResource = trace.traceStrength * pImpl->maximumCapacity * 0.1f;
}

void NeuralWorkingMemory::reactivateTrace(WorkingMemoryTrace& trace) {
    // Reactivate trace with persistent activity
    trace.lastReactivation = pImpl->timeStep;
    trace.noveltyScore = 1.0f;  // Reset novelty on reactivation
    
    // Boost trace strength through reinforcement
    trace.traceStrength = std::min(1.0f, trace.traceStrength * 1.2f);
    
    // Inject current to maintain persistent activity
    if (brain_) {
        brain_->injectCurrent(trace.neuralId, trace.persistentActivity * 10.0f);
    }
    
    // Reactivate population neurons
    for (size_t i = 0; i < trace.population.size(); ++i) {
        if (trace.populationActivity[i] > 0.1f) {
            if (brain_) {
                brain_->injectCurrent(trace.population[i], trace.populationActivity[i] * 8.0f);
            }
        }
    }
}

void NeuralWorkingMemory::selectEncodingPopulation(const WorkingMemoryTrace& trace) {
    // Select optimal neural population for encoding based on trace properties
    if (pImpl->availableNeurons.empty()) return;
    
    // Use hash-based selection for deterministic population allocation
    size_t seed = static_cast<size_t>(trace.neuralId.getId()) * 1664525 + 1013904223;
    std::vector<NeuronId> selectedPopulation;
    
    for (size_t i = 0; i < 5 && i < pImpl->availableNeurons.size(); ++i) {
        size_t idx = (seed + i * 1103515245) % pImpl->availableNeurons.size();
        selectedPopulation.push_back(pImpl->availableNeurons[idx]);
    }
    
    // Update population activity based on trace properties
    for (size_t i = 0; i < selectedPopulation.size(); ++i) {
        // Different neurons encode different aspects with Hebbian-like dynamics
        float baseActivity = trace.persistentActivity * 0.7f;
        float modulation = std::sin(static_cast<float>(i) * 0.4f) * trace.traceStrength * 0.3f;
        pImpl->neuralActivityLevels[i] = baseActivity + modulation;
    }
}

void NeuralWorkingMemory::runWinnerTakeAllCompetition() {
    // Implement biologically realistic winner-take-all competition
    pImpl->winnerNeurons.clear();
    
    if (pImpl->traces.empty()) return;
    
    // Compute competition scores based on trace strength and novelty
    std::vector<float> competitionScores;
    competitionScores.reserve(pImpl->traces.size());
    
    for (const auto& trace : pImpl->traces) {
        // Competition score combines trace strength and novelty
        float score = trace.traceStrength * 0.6f + trace.noveltyScore * 0.4f;
        competitionScores.push_back(score);
    }
    
    // Apply global inhibition to normalize scores
    float maxScore = *std::max_element(competitionScores.begin(), competitionScores.end());
    if (maxScore > 0.0f) {
        for (float& score : competitionScores) {
            score = std::max(0.0f, score - pImpl->globalInhibition * maxScore);
        }
    }
    
    // Winner selection
    float competitionThreshold = pImpl->competitionGain * 0.5f;
    
    for (size_t i = 0; i < pImpl->traces.size(); ++i) {
        if (competitionScores[i] >= competitionThreshold) {
            pImpl->winnerNeurons.push_back(pImpl->traces[i].neuralId);
            
            // Apply excitation to winners
            if (brain_) {
                brain_->injectCurrent(pImpl->traces[i].neuralId, 5.0f);
            }
        }
    }
    
    // Update lateral inhibition between traces
    applyLateralInhibition();
}

void NeuralWorkingMemory::applyLateralInhibition() {
    // Implement lateral inhibition between competing traces
    
    if (pImpl->winnerNeurons.empty() || pImpl->traces.empty()) return;
    
    // For each winning trace, apply strong inhibition to non-winners
    std::vector<bool> isWinner(pImpl->traces.size(), false);
    for (const auto& winner : pImpl->winnerNeurons) {
        for (size_t i = 0; i < pImpl->traces.size(); ++i) {
            if (pImpl->traces[i].neuralId == winner) {
                isWinner[i] = true;
                break;
            }
        }
    }
    
    // Apply lateral inhibition
    for (size_t i = 0; i < pImpl->traces.size(); ++i) {
        if (!isWinner[i] && brain_) {
            // Strong inhibition to suppress non-winning traces
            // Find and inhibit associated neurons
            for (const auto& neuron : pImpl->traces[i].population) {
                brain_->injectCurrent(neuron, -pImpl->traces[i].traceStrength * 3.0f);
            }
        }
    }
}

void NeuralWorkingMemory::applyGlobalInhibition() {
    // Apply global inhibition based on overall memory load
    
    float loadRatio = pImpl->currentLoad / pImpl->maximumCapacity;
    
    if (loadRatio > 0.7f) {
        // High load - increase global inhibition
        pImpl->globalInhibition = std::min(0.5f, 0.3f + (loadRatio - 0.7f) * 0.67f);
    } else {
        // Low load - decrease global inhibition
        pImpl->globalInhibition = std::max(0.1f, 0.3f - (0.7f - loadRatio) * 0.5f);
    }
}

void NeuralWorkingMemory::getNovelNeurons() const {
    // Identify neurons with high novelty scores
    // This would return neurons that have recent novel inputs
}

void NeuralWorkingMemory::forgetWeakTraces() {
    // Implement adaptive forgetting based on novelty and competition
    
    // Remove traces with low novelty scores and weak strength
    auto it = std::remove_if(pImpl->traces.begin(), pImpl->traces.end(),
        [this](const WorkingMemoryTrace& trace) {
            // Forgetting criteria:
            // 1. Very low novelty
            // 2. Very weak trace
            // 3. Old and not consolidated
            return (trace.noveltyScore < 0.2f && trace.traceStrength < 0.1f) ||
                   (trace.age > 1000 && !trace.isConsolidated);
        });
    
    pImpl->traces.erase(it, pImpl->traces.end());
    
    // Update engaged neurons and resources
    pImpl->engagedNeurons.clear();
    for (const auto& trace : pImpl->traces) {
        pImpl->engagedNeurons.insert(pImpl->engagedNeurons.end(), 
                                    trace.population.begin(), trace.population.end());
    }
    
    // Recalculate load
    pImpl->currentLoad = 0.0f;
    for (const auto& trace : pImpl->traces) {
        pImpl->currentLoad += trace.synapticResource;
    }
}

void NeuralWorkingMemory::consolidateTraces() {
    // Consolidate traces for long-term storage
    
    float consolidationThreshold = pImpl->maximumCapacity * 0.3f;
    
    for (auto& trace : pImpl->traces) {
        if (!trace.isConsolidated && trace.synapticResource >= consolidationThreshold) {
            trace.isConsolidated = true;
            trace.consolidationWeight = 0.9f;
            
            // Consolidate synaptic resources
            trace.synapticResource *= 1.5f;
            
            // Reduce trace strength slightly (long-term storage)
            trace.traceStrength *= 0.8f;
        }
    }
}

void NeuralWorkingMemory::decayTraces(TimestepDuration dt) {
    // Implement trace decay based on neuronal fatigue and time
    
    for (auto& trace : pImpl->traces) {
        // Neuronal fatigue-based decay
        float fatigueDecay = std::exp(-trace.traceStrength * decayRate_ * dt);
        trace.traceStrength *= fatigueDecay;
        
        // Age-dependent decay
        float ageDecay = std::exp(-static_cast<float>(trace.age) * decayRate_ * 0.01f);
        trace.traceStrength *= ageDecay;
        
        // Spike pattern-based forgetting
        if (!trace.spikeHistory.empty()) {
            // Calculate spike pattern stability
            float patternStability = 1.0f;
            if (trace.spikeHistory.size() > 3) {
                float recentPattern = trace.spikeHistory.back();
                for (size_t i = 1; i < trace.spikeHistory.size(); ++i) {
                    float prevPattern = trace.spikeHistory[i-1];
                    patternStability *= std::exp(-std::abs(recentPattern - prevPattern) * 0.01f);
                }
                trace.traceStrength *= patternStability;
            }
        }
        
        // Update synaptic resources
        trace.synapticResource = trace.traceStrength * pImpl->maximumCapacity * 0.1f;
        
        // Decay novelty score
        trace.noveltyScore *= 0.95f;
    }
    
    // Update load after decay
    pImpl->currentLoad = 0.0f;
    for (const auto& trace : pImpl->traces) {
        pImpl->currentLoad += trace.synapticResource;
    }
}

} // namespace nlm

// AttentionalSelection Implementation remains unchanged for brevity

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