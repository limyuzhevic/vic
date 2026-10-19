#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <numeric>
#include <queue>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Neural dynamics state
    std::mt19937 rng;
    std::normal_distribution<float> noiseDist;
    std::uniform_real_distribution<float> uniformDist;
    
    // Pattern completion state
    std::vector<std::vector<float>> patternMatrix;
    std::vector<float> globalFiringRates;
    std::vector<std::vector<float>> synapticWeights;
    
    // Activity dynamics
    std::vector<float> neuronActivations;
    std::vector<float> neuronFiringRates;
    std::vector<float> neuronMembranePotentials;
    
    // Temporal dynamics
    SimulationStep lastUpdateStep;
    float integrationConstant;
    
    Impl() : brain(nullptr), rng(std::chrono::steady_clock::now().time_since_epoch().count()),
             noiseDist(0.0f, 0.05f), uniformDist(0.0f, 1.0f),
             lastUpdateStep(0), integrationConstant(0.1f) {
        patternMatrix.reserve(100);
        globalFiringRates.reserve(100);
    }
};

NeuralWorkingMemory::NeuralWorkingMemory(size_t capacity, float decayRate)
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(capacity)
    , decayRate_(decayRate)
    , currentStep_(0)
    , currentTime_(0.0)
    , attractorTemp_(0.1f)
    , chunkingEnabled_(true)
    , attractorEnabled_(true)
    , totalConsolidations_(0)
    , totalEvictions_(0)
    , averageStrengthHistory_(0.0f) {
    memoryTraces_.reserve(capacity * 2);
    memoryNeurons_.reserve(capacity);
    memoryActivations_.reserve(capacity);
    memoryTimestamps_.reserve(capacity);
    activeTraces_.reserve(capacity);
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize neural components
    if (brain_) {
        // Create neural populations for working memory
        size_t numNeurons = std::min(capacity_ * 10, static_cast<size_t>(200));
        
        // Reserve space for neural dynamics
        pImpl->neuronActivations.resize(numNeurons, 0.0f);
        pImpl->neuronFiringRates.resize(numNeurons, 0.0f);
        pImpl->neuronMembranePotentials.resize(numNeurons, -65.0f); // resting potential
        pImpl->globalFiringRates.resize(numNeurons, 0.0f);
        
        // Initialize synaptic weights for maintenance connections
        pImpl->synapticWeights.resize(numNeurons, std::vector<float>(numNeurons, 0.0f));
        
        // Create maintenance connections
        for (size_t i = 0; i < numNeurons - 1; ++i) {
            pImpl->synapticWeights[i][i + 1] = 0.1f; // forward connections
            pImpl->synapticWeights[i + 1][i] = 0.1f; // backward connections (recurrent)
        }
    }
    
    NLM_LOG_INFO("NeuralWorkingMemory initialized with capacity {} and decay rate {}", capacity_, decayRate_);
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Convert pattern to neural activity
    neuralPopulationCoding(pattern);
    
    // Create or update memory trace
    WorkingMemoryTrace trace(memoryTraces_.size());
    trace.contentVector = pattern;
    trace.strength = strength;
    trace.confidence = 1.0f;
    trace.precision = 1.0f;
    trace.priority = strength;
    trace.salience = strength;
    trace.novelty = uniformDist(pImpl->rng);
    trace.decayFactor = 1.0f - decayRate_;
    trace.age = 0;
    trace.lastAccess = currentStep_;
    trace.attractorState = pImpl->patternMatrix.back();
    trace.reconstruction = pattern;
    
    // Store trace
    storeTrace(trace);
    
    // Update attractor dynamics
    if (attractorEnabled_) {
        attractorDynamics();
    }
    
    // Apply interference management
    manageInterference();
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
        memoryTraces_[idx].lastAccess = currentStep_;
        memoryTraces_[idx].strength = activation;
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
        
        // Create memory trace
        WorkingMemoryTrace trace(memoryTraces_.size());
        trace.neuronIds.push_back(neuron);
        trace.activations.push_back(activation);
        trace.lastAccess = currentStep_;
        trace.strength = activation;
        
        memoryTraces_.push_back(trace);
        activeTraces_.push_back(memoryTraces_.size() - 1);
    }
    
    // Inject current to maintain activation
    if (brain_) {
        brain_->injectCurrent(neuron, activation * 5.0f);
    }
}

void NeuralWorkingMemory::storeTrace(const WorkingMemoryTrace& trace) {
    // Check capacity and evict if needed
    if (memoryTraces_.size() >= capacity_) {
        evictItems();
    }
    
    // Add trace to memory
    memoryTraces_.push_back(trace);
    activeTraces_.push_back(memoryTraces_.size() - 1);
    
    // Update neuron representations
    size_t traceId = memoryTraces_.size() - 1;
    neuralPopulationCoding(trace.contentVector);
    
    // Create maintenance connections
    if (memoryTraces_.size() > 1) {
        size_t prevId = memoryTraces_.size() - 2;
        createRecurrentConnection(prevId, traceId, 0.5f * trace.strength);
    }
}

void NeuralWorkingMemory::neuralPopulationCoding(const std::vector<float>& pattern) {
    // Encode pattern into neural population
    size_t numNeurons = pImpl->neuronActivations.size();
    pImpl->patternMatrix.push_back(pattern);
    
    // Normalize pattern
    float patternNorm = std::sqrt(std::accumulate(pattern.begin(), pattern.end(), 0.0f,
                                                  [](float sum, float val) { return sum + val * val; }));
    
    if (patternNorm > 0.0f) {
        std::vector<float> normalizedPattern(pattern.size());
        for (size_t i = 0; i < pattern.size(); ++i) {
            normalizedPattern[i] = pattern[i] / patternNorm;
        }
        pImpl->patternMatrix.back() = normalizedPattern;
    }
    
    // Update neural activations based on pattern
    for (size_t i = 0; i < std::min(pattern.size(), numNeurons); ++i) {
        float targetActivation = normalizedPattern[i] * 0.8f + 0.1f; // Add baseline
        pImpl->neuronActivations[i] = pImpl->neuronActivations[i] * 0.9f + targetActivation * 0.1f;
        
        // Add noise for biological realism
        pImpl->neuronActivations[i] += pImpl->noiseDist(pImpl->rng);
        
        // Clamp to reasonable range
        pImpl->neuronActivations[i] = std::max(0.0f, std::min(pImpl->neuronActivations[i], 1.0f));
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    result.reserve(memoryTraces_.size());
    
    for (const auto& trace : memoryTraces_) {
        float avgActivation = 0.0f;
        size_t count = 0;
        
        for (size_t i = 0; i < trace.neuronIds.size() && i < trace.activations.size(); ++i) {
            avgActivation += trace.activations[i];
            count++;
        }
        
        if (count > 0) {
            avgActivation /= count;
        }
        
        result.push_back(avgActivation);
    }
    
    return result;
}

std::vector<float> NeuralWorkingMemory::retrievePattern(const std::vector<float>& cue) const {
    if (memoryTraces_.empty()) return {};
    
    // Compute similarity between cue and all stored patterns
    std::vector<float> similarities;
    similarities.reserve(memoryTraces_.size());
    
    for (const auto& trace : memoryTraces_) {
        float similarity = computePatternSimilarity(cue, trace.contentVector);
        similarities.push_back(similarity);
    }
    
    // Find most similar pattern
    size_t maxIndex = std::max_element(similarities.begin(), similarities.end()) - similarities.begin();
    
    if (maxIndex < memoryTraces_.size()) {
        // Return reconstructed pattern based on most similar trace
        return memoryTraces_[maxIndex].reconstruction;
    }
    
    return {};
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    currentStep_++;
    currentTime_ += dt;
    
    if (!brain_) return;
    
    // Update all traces
    for (auto& trace : memoryTraces_) {
        trace.age++;
        trace.lastAccess = currentStep_;
        
        // Apply decay
        trace.strength *= trace.decayFactor;
        
        // Update neural activations
        for (size_t i = 0; i < trace.neuronIds.size(); ++i) {
            NeuronId neuron = trace.neuronIds[i];
            float activation = trace.activations[i];
            
            if (activation > 0.1f) {
                // Inject maintenance current
                brain_->injectCurrent(neuron, activation * 2.0f);
                
                // Update firing rate
                pImpl->neuronFiringRates[neuron.index()] = activation * 20.0f; // Hz
                
                // Add noise
                pImpl->neuronFiringRates[neuron.index()] += pImpl->noiseDist(pImpl->rng);
            }
        }
    }
    
    // Update attractor dynamics
    if (attractorEnabled_) {
        attractorDynamics();
    }
    
    // Manage interference
    manageInterference();
    
    // Update pattern completion
    patternCompletion();
    
    // Run competition and selection
    runCompetition();
    
    // Rehearse active memories
    rehearseActiveMemories();
    
    // Update recurrent connections
    updateRecurrentConnections();
    
    // Apply synaptic plasticity
    synapticModulation(learningRate_);
    
    // Apply homeostatic plasticity
    homeostaticPlasticity();
    
    // Chunk information if enabled
    if (chunkingEnabled_) {
        chunkInformation();
    }
}

void NeuralWorkingMemory::patternCompletion() {
    if (memoryTraces_.empty()) return;
    
    for (auto& trace : memoryTraces_) {
        // Attractor dynamics for pattern completion
        if (trace.attractorState.size() > 0) {
            // Compute gradient towards attractor
            std::vector<float> gradient(trace.attractorState.size(), 0.0f);
            
            for (size_t i = 0; i < trace.attractorState.size(); ++i) {
                // Simple gradient descent towards attractor
                float error = trace.contentVector[i] - trace.attractorState[i];
                gradient[i] = error * 0.01f;
            }
            
            // Update attractor state
            for (size_t i = 0; i < trace.attractorState.size(); ++i) {
                if (i < trace.attractorState.size()) {
                    trace.attractorState[i] += gradient[i] * attractorTemp_;
                    
                    // Add thermal noise
                    trace.attractorState[i] += pImpl->noiseDist(pImpl->rng) * attractorTemp_;
                }
            }
            
            // Normalize attractor state
            float norm = std::sqrt(std::accumulate(trace.attractorState.begin(), 
                                                  trace.attractorState.end(), 0.0f,
                                                  [](float sum, float val) { return sum + val * val; }));
            if (norm > 0.0f) {
                for (float& val : trace.attractorState) {
                    val /= norm;
                }
            }
            
            // Reconstruct pattern from attractor
            trace.reconstruction = trace.attractorState;
        }
    }
}

void NeuralWorkingMemory::attractorDynamics() {
    if (pImpl->patternMatrix.empty()) return;
    
    // Update global attractor state
    size_t numPatterns = pImpl->patternMatrix.size();
    size_t dimension = pImpl->patternMatrix[0].size();
    
    // Compute energy landscape
    std::vector<float> globalAttractor(dimension, 0.0f);
    
    for (const auto& pattern : pImpl->patternMatrix) {
        for (size_t i = 0; i < dimension; ++i) {
            globalAttractor[i] += pattern[i];
        }
    }
    
    // Normalize
    float norm = std::sqrt(std::accumulate(globalAttractor.begin(), globalAttractor.end(), 0.0f,
                                          [](float sum, float val) { return sum + val * val; }));
    if (norm > 0.0f) {
        for (float& val : globalAttractor) {
            val /= norm;
        }
    }
    
    // Update global firing rates
    for (size_t i = 0; i < pImpl->globalFiringRates.size(); ++i) {
        // Attractor dynamics for global state
        float target = globalAttractor[i % dimension];
        pImpl->globalFiringRates[i] = pImpl->globalFiringRates[i] * 0.9f + target * 0.1f;
    }
}

void NeuralWorkingMemory::manageInterference() {
    // Compute interference scores
    for (auto& trace : memoryTraces_) {
        trace.interferenceScore = computeInterferenceScore(trace);
    }
    
    // Proactive interference control
    proactiveInterferenceControl();
    
    // Retroactive interference control
    retroactiveInterferenceControl();
}

float NeuralWorkingMemory::computeInterferenceScore(const WorkingMemoryTrace& trace) const {
    float totalInterference = 0.0f;
    
    for (uint64_t interfererId : trace.interferers) {
        if (interfererId < memoryTraces_.size()) {
            const auto& interferer = memoryTraces_[interfererId];
            
            // Compute similarity-based interference
            float similarity = computePatternSimilarity(trace.contentVector, interferer.contentVector);
            totalInterference += similarity * interferer.strength;
        }
    }
    
    return std::min(totalInterference, 1.0f);
}

void NeuralWorkingMemory::proactiveInterferenceControl() {
    // Suppress traces that are similar to current content
    std::vector<float> currentContent;
    if (!memoryTraces_.empty()) {
        currentContent = memoryTraces_.back().contentVector;
    }
    
    for (auto& trace : memoryTraces_) {
        if (&trace != &memoryTraces_.back()) {
            float similarity = computePatternSimilarity(currentContent, trace.contentVector);
            
            if (similarity > 0.7f) {
                // Suppress similar traces to reduce proactive interference
                trace.strength *= 0.8f;
                
                // Mark as interfering
                if (std::find(trace.interferers.begin(), trace.interferers.end(), memoryTraces_.size() - 1) == trace.interferers.end()) {
                    trace.interferers.push_back(memoryTraces_.size() - 1);
                }
            }
        }
    }
}

void NeuralWorkingMemory::retroactiveInterferenceControl() {
    // Update interference from new traces
    if (memoryTraces_.size() > 1) {
        for (size_t i = 0; i < memoryTraces_.size() - 1; ++i) {
            auto& olderTrace = memoryTraces_[i];
            auto& newerTrace = memoryTraces_.back();
            
            // Compute retroactive interference (new learning affecting old memory)
            float similarity = computePatternSimilarity(olderTrace.contentVector, newerTrace.contentVector);
            
            if (similarity > 0.5f) {
                // Apply retroactive interference
                olderTrace.strength *= (1.0f - similarity * 0.3f);
            }
        }
    }
}

void NeuralWorkingMemory::runCompetition() {
    winners_.clear();
    
    if (memoryTraces_.empty()) return;
    
    // Compute competition scores based on strength and salience
    std::vector<float> competitionScores;
    competitionScores.reserve(memoryTraces_.size());
    
    for (const auto& trace : memoryTraces_) {
        float score = trace.strength * trace.salience * (1.0f + trace.novelty * 0.5f);
        competitionScores.push_back(score);
    }
    
    // Normalize scores
    float maxScore = *std::max_element(competitionScores.begin(), competitionScores.end());
    if (maxScore > 0.0f) {
        for (float& score : competitionScores) {
            score /= maxScore;
        }
    }
    
    // Select winners (traces with scores above threshold)
    float competitionThreshold = 0.3f;
    
    for (size_t i = 0; i < memoryTraces_.size(); ++i) {
        if (competitionScores[i] >= competitionThreshold) {
            winners_.push_back(memoryNeurons_[i]);
        }
    }
    
    // Apply winner-take-all inhibition
    winnerTakeAll();
    
    // Apply balanced selection
    balancedWinnerSelection();
}

void NeuralWorkingMemory::winnerTakeAll() {
    if (winners_.size() <= 1) return;
    
    // Find strongest winner
    float maxStrength = 0.0f;
    NeuronId strongestWinner = INVALID_NEURON_ID;
    
    for (NeuronId winner : winners_) {
        float strength = getNeuronActivation(winner);
        if (strength > maxStrength) {
            maxStrength = strength;
            strongestWinner = winner;
        }
    }
    
    // Inhibit other winners
    if (brain_ && strongestWinner != INVALID_NEURON_ID) {
        for (NeuronId winner : winners_) {
            if (winner != strongestWinner) {
                brain_->injectCurrent(winner, -0.5f);
            }
        }
    }
}

void NeuralWorkingMemory::balancedWinnerSelection() {
    if (winners_.size() < 2) return;
    
    // Ensure diverse winners (not too similar)
    std::vector<bool> isSimilar(winners_.size(), false);
    
    for (size_t i = 0; i < winners_.size(); ++i) {
        for (size_t j = i + 1; j < winners_.size(); ++j) {
            float similarity = computePatternSimilarity(
                retrievePatternFromNeuron(winners_[i]),
                retrievePatternFromNeuron(winners_[j]));
            
            if (similarity > 0.8f) {
                isSimilar[i] = true;
                isSimilar[j] = true;
            }
        }
    }
    
    // Remove highly similar winners
    std::vector<NeuronId> filteredWinners;
    for (size_t i = 0; i < winners_.size(); ++i) {
        if (!isSimilar[i]) {
            filteredWinners.push_back(winners_[i]);
        }
    }
    
    if (filteredWinners.size() < winners_.size()) {
        winners_ = filteredWinners;
    }
}

std::vector<NeuronId> NeuralWorkingMemory::selectWinners(const std::vector<NeuronId>& candidates) const {
    if (candidates.empty()) return {};
    
    // Compute candidate scores
    std::vector<float> scores(candidates.size(), 0.0f);
    
    for (size_t i = 0; i < candidates.size(); ++i) {
        NeuronId neuron = candidates[i];
        scores[i] = getNeuronActivation(neuron) * getTopDownBias(neuron);
    }
    
    // Apply softmax for probabilistic selection
    float sum = std::accumulate(scores.begin(), scores.end(), 0.0f);
    if (sum > 0.0f) {
        std::vector<float> probabilities(scores.size());
        for (size_t i = 0; i < scores.size(); ++i) {
            probabilities[i] = scores[i] / sum;
        }
        
        // Select winners based on probabilities
        std::vector<NeuronId> selected;
        std::discrete_distribution<size_t> dist(probabilities.begin(), probabilities.end());
        
        // Select top 2-3 winners
        int numWinners = std::min(3, static_cast<int>(candidates.size()));
        for (int i = 0; i < numWinners; ++i) {
            size_t winnerIdx = dist(pImpl->rng);
            selected.push_back(candidates[winnerIdx]);
        }
        
        return selected;
    }
    
    // Default: select all candidates if no clear preference
    return candidates;
}

void NeuralWorkingMemory::chunkInformation() {
    if (memoryTraces_.size() < 2) return;
    
    // Compute similarity between traces
    std::vector<std::vector<float>> similarities(memoryTraces_.size(), std::vector<float>(memoryTraces_.size(), 0.0f));
    
    for (size_t i = 0; i < memoryTraces_.size(); ++i) {
        for (size_t j = i + 1; j < memoryTraces_.size(); ++j) {
            float similarity = computePatternSimilarity(
                memoryTraces_[i].contentVector, memoryTraces_[j].contentVector);
            
            similarities[i][j] = similarities[j][i] = similarity;
        }
    }
    
    // Group similar traces into chunks
    std::vector<bool> isChunked(memoryTraces_.size(), false);
    capacityManager_.chunks.clear();
    capacityManager_.itemToChunk.assign(memoryTraces_.size(), -1);
    capacityManager_.chunkMembers.clear();
    
    for (size_t i = 0; i < memoryTraces_.size(); ++i) {
        if (isChunked[i]) continue;
        
        // Start a new chunk
        std::vector<uint64_t> chunk = {i};
        isChunked[i] = true;
        
        // Find similar traces to add to this chunk
        for (size_t j = i + 1; j < memoryTraces_.size(); ++j) {
            if (!isChunked[j] && similarities[i][j] > 0.6f) {
                chunk.push_back(j);
                isChunked[j] = true;
            }
        }
        
        // Limit chunk size
        if (chunk.size() > capacityManager_.maxChunkSize) {
            // Split chunk if too large
            capacityManager_.chunks.push_back(std::vector<uint64_t>(chunk.begin(), 
                                                                   chunk.begin() + capacityManager_.maxChunkSize));
            std::vector<uint64_t> remaining(chunk.begin() + capacityManager_.maxChunkSize, chunk.end());
            
            // Process remaining items
            for (uint64_t id : remaining) {
                // Recursively chunk remaining items
                chunkInformationRecursive(id);
            }
        } else {
            capacityManager_.chunks.push_back(chunk);
        }
        
        // Update chunk members mapping
        for (uint64_t id : chunk) {
            capacityManager_.itemToChunk[id] = capacityManager_.chunks.size() - 1;
            capacityManager_.chunkMembers[capacityManager_.chunks.size() - 1].push_back(id);
        }
    }
}

void NeuralWorkingMemory::chunkInformationRecursive(uint64_t traceId) {
    // Recursively process remaining traces
    if (traceId >= memoryTraces_.size()) return;
    
    // Find similar traces for this item
    std::vector<uint64_t> similarIds;
    
    for (size_t i = 0; i < memoryTraces_.size(); ++i) {
        if (i == traceId) continue;
        
        float similarity = computePatternSimilarity(
            memoryTraces_[traceId].contentVector, memoryTraces_[i].contentVector);
        
        if (similarity > 0.6f) {
            similarIds.push_back(i);
        }
    }
    
    // Add similar items to same chunk
    for (uint64_t id : similarIds) {
        size_t chunkIndex = capacityManager_.itemToChunk[traceId];
        if (chunkIndex != -1) {
            capacityManager_.chunkMembers[chunkIndex].push_back(id);
            capacityManager_.itemToChunk[id] = chunkIndex;
        }
    }
}

void NeuralWorkingMemory::reorganizeMemory() {
    // Reorganize memory traces based on current state
    prioritizeItems();
    
    // Evict weak traces
    evictItems();
    
    // Update active trace list
    updateActiveTraces();
}

void NeuralWorkingMemory::prioritizeItems() {
    // Sort traces by priority (strength + salience + novelty)
    std::sort(memoryTraces_.begin(), memoryTraces_.end(),
              [this](const WorkingMemoryTrace& a, const WorkingMemoryTrace& b) {
                  float priorityA = a.strength * a.salience * (1.0f + a.novelty * 0.5f);
                  float priorityB = b.strength * b.salience * (1.0f + b.novelty * 0.5f);
                  return priorityA > priorityB;
              });
    
    // Sort neurons accordingly
    std::vector<NeuronId> sortedNeurons = memoryNeurons_;
    std::vector<float> sortedActivations = memoryActivations_;
    std::vector<SimulationStep> sortedTimestamps = memoryTimestamps_;
    
    std::vector<size_t> indices(memoryTraces_.size());
    std::iota(indices.begin(), indices.end(), 0);
    
    std::sort(indices.begin(), indices.end(),
              [&memoryTraces = memoryTraces_](size_t a, size_t b) {
                  float priorityA = memoryTraces[a].strength * memoryTraces[a].salience;
                  float priorityB = memoryTraces[b].strength * memoryTraces[b].salience;
                  return priorityA > priorityB;
              });
    
    // Reorder all arrays
    std::vector<WorkingMemoryTrace> sortedTraces;
    sortedTraces.reserve(memoryTraces_.size());
    
    for (size_t idx : indices) {
        sortedTraces.push_back(memoryTraces_[idx]);
    }
    
    memoryTraces_ = std::move(sortedTraces);
    
    // Update related arrays (neurons, activations, timestamps)
    std::vector<NeuronId> newNeurons;
    std::vector<float> newActivations;
    std::vector<SimulationStep> newTimestamps;
    
    newNeurons.reserve(memoryNeurons_.size());
    newActivations.reserve(memoryActivations_.size());
    newTimestamps.reserve(memoryTimestamps_.size());
    
    for (size_t idx : indices) {
        newNeurons.push_back(memoryNeurons_[idx]);
        newActivations.push_back(memoryActivations_[idx]);
        newTimestamps.push_back(memoryTimestamps_[idx]);
    }
    
    memoryNeurons_ = std::move(newNeurons);
    memoryActivations_ = std::move(newActivations);
    memoryTimestamps_ = std::move(newTimestamps);
}

void NeuralWorkingMemory::evictItems() {
    if (memoryTraces_.size() <= capacity_) return;
    
    // Compute eviction scores based on strength, age, interference
    std::vector<float> evictionScores;
    evictionScores.reserve(memoryTraces_.size());
    
    for (const auto& trace : memoryTraces_) {
        float score = (1.0f - trace.strength) * 0.5f +  // Weak strength
                     (trace.age / 1000.0f) * 0.3f +       // Old age
                     trace.interferenceScore * 0.2f;     // High interference
        evictionScores.push_back(score);
    }
    
    // Find items to evict (highest scores)
    size_t numToEvict = memoryTraces_.size() - capacity_;
    std::vector<size_t> indicesToEvict(evictionScores.size());
    std::iota(indicesToEvict.begin(), indicesToEvict.end(), 0);
    
    std::partial_sort(indicesToEvict.begin(), indicesToEvict.begin() + numToEvict, indicesToEvict.end(),
                      [&evictionScores](size_t a, size_t b) {
                          return evictionScores[a] > evictionScores[b];
                      });
    
    // Evict items in reverse order to maintain indices
    for (auto it = indicesToEvict.rbegin(); it != indicesToEvict.rend(); ++it) {
        size_t idx = *it;
        
        // Remove trace
        memoryTraces_.erase(memoryTraces_.begin() + idx);
        
        // Remove neuron if not referenced elsewhere
        auto neuronIt = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), memoryTraces_[idx].neuronIds[0]);
        if (neuronIt != memoryNeurons_.end()) {
            memoryNeurons_.erase(neuronIt);
        }
        
        // Remove activation
        if (idx < memoryActivations_.size()) {
            memoryActivations_.erase(memoryActivations_.begin() + idx);
        }
        
        // Remove timestamp
        if (idx < memoryTimestamps_.size()) {
            memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
        }
        
        totalEvictions_++;
    }
    
    // Update active trace indices
    updateActiveTraces();
}

void NeuralWorkingMemory::updateActiveTraces() {
    activeTraces_.clear();
    
    for (size_t i = 0; i < memoryTraces_.size(); ++i) {
        if (memoryTraces_[i].strength > 0.1f && memoryTraces_[i].isActive) {
            activeTraces_.push_back(i);
        }
    }
}

void NeuralWorkingMemory::rehearseActiveMemories() {
    for (size_t traceIdx : activeTraces_) {
        if (traceIdx < memoryTraces_.size()) {
            auto& trace = memoryTraces_[traceIdx];
            
            // Rehearse by strengthening
            if (trace.strength < 1.0f) {
                trace.strength = std::min(1.0f, trace.strength * 1.2f);
            }
            
            // Apply rehearsal to neurons
            for (NeuronId neuron : trace.neuronIds) {
                if (brain_) {
                    brain_->injectCurrent(neuron, trace.strength * 0.5f);
                }
            }
        }
    }
}

void NeuralWorkingMemory::neuralPopulationCoding(const std::vector<float>& pattern) {
    if (pattern.empty()) return;
    
    // Update pattern matrix
    pImpl->patternMatrix.push_back(pattern);
    
    // Keep only recent patterns
    if (pImpl->patternMatrix.size() > 50) {
        pImpl->patternMatrix.erase(pImpl->patternMatrix.begin());
    }
}

void NeuralWorkingMemory::synapticModulation(float learningRate) {
    if (pImpl->brain == nullptr) return;
    
    // STDP-like synaptic modulation
    for (size_t i = 0; i < pImpl->neuronActivations.size(); ++i) {
        float activation = pImpl->neuronActivations[i];
        
        if (activation > 0.5f) {
            // Potentiation for co-activation
            for (size_t j = 0; j < pImpl->neuronActivations.size(); ++j) {
                if (i != j && pImpl->neuronActivations[j] > 0.5f) {
                    // Hebbian-like potentiation
                    float delta = learningRate * activation * pImpl->neuronActivations[j];
                    pImpl->synapticWeights[i][j] += delta;
                    
                    // Constrain weights
                    pImpl->synapticWeights[i][j] = std::max(-1.0f, std::min(pImpl->synapticWeights[i][j], 1.0f));
                }
            }
        }
    }
}

void NeuralWorkingMemory::spikeTimingDependentPlasticity() {
    if (!brain_) return;
    
    // Simple STDP implementation
    for (size_t i = 0; i < pImpl->neuronActivations.size(); ++i) {
        float activation = pImpl->neuronActivations[i];
        
        if (activation > 0.5f) {
            // Pre-post spike pairing
            for (size_t j = 0; j < pImpl->neuronActivations.size(); ++j) {
                if (i != j && pImpl->neuronActivations[j] > 0.5f) {
                    // LTP if pre before post
                    float ltp = 0.01f * exp(-1.0f); // Time difference
                    pImpl->synapticWeights[i][j] += ltp;
                }
            }
        } else if (activation < 0.2f) {
            // LTD for low activation
            for (size_t j = 0; j < pImpl->neuronActivations.size(); ++j) {
                if (i != j && pImpl->neuronActivations[j] < 0.2f) {
                    float ltd = -0.01f * exp(-1.0f); // Time difference
                    pImpl->synapticWeights[i][j] += ltd;
                }
            }
        }
    }
}

void NeuralWorkingMemory::homeostaticPlasticity() {
    if (pImpl->brain == nullptr) return;
    
    // Maintain overall firing rate balance
    float totalActivation = 0.0f;
    for (float activation : pImpl->neuronActivations) {
        totalActivation += activation;
    }
    
    float targetRate = 0.5f;
    float currentRate = totalActivation / pImpl->neuronActivations.size();
    
    if (currentRate > targetRate + 0.1f) {
        // Downscale all activations
        float scale = (targetRate + 0.05f) / currentRate;
        for (float& activation : pImpl->neuronActivations) {
            activation *= scale;
        }
    } else if (currentRate < targetRate - 0.1f) {
        // Upscale all activations
        float scale = (targetRate - 0.05f) / currentRate;
        for (float& activation : pImpl->neuronActivations) {
            activation *= scale;
        }
    }
}

void NeuralWorkingMemory::createRecurrentConnection(size_t fromTrace, size_t toTrace, float strength) {
    // Create recurrent connection between memory traces
    if (fromTrace >= memoryTraces_.size() || toTrace >= memoryTraces_.size()) return;
    
    NeuronId fromNeuron = memoryNeurons_[fromTrace];
    NeuronId toNeuron = memoryNeurons_[toTrace];
    
    // Store connection
    pImpl->maintenanceSynapses.emplace_back(fromNeuron, toNeuron);
    
    // Apply connection strength
    if (brain_) {
        brain_->injectCurrent(toNeuron, strength * 2.0f);
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
    
    for (size_t i = 0; i < memoryTraces_.size(); ++i) {
        auto& trace = memoryTraces_[i];
        
        // Apply decay
        trace.strength *= (1.0f - decayRate_);
        
        if (trace.strength < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t idx = *it;
        
        // Remove from all arrays
        memoryTraces_.erase(memoryTraces_.begin() + idx);
        
        if (idx < memoryNeurons_.size()) {
            memoryNeurons_.erase(memoryNeurons_.begin() + idx);
        }
        
        if (idx < memoryActivations_.size()) {
            memoryActivations_.erase(memoryActivations_.begin() + idx);
        }
        
        if (idx < memoryTimestamps_.size()) {
            memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
        }
        
        // Update synapse references
        auto synapseIt = std::remove_if(pImpl->maintenanceSynapses.begin(), pImpl->maintenanceSynapses.end(),
                                       [idx](const auto& conn) {
                                           return conn.first.index() == idx || conn.second.index() == idx;
                                       });
        pImpl->maintenanceSynapses.erase(synapseIt, pImpl->maintenanceSynapses.end());
    }
    
    // Update active traces
    updateActiveTraces();
}

void NeuralWorkingMemory::clear() {
    memoryTraces_.clear();
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    activeTraces_.clear();
    pImpl->maintenanceSynapses.clear();
    
    // Clear pattern matrix
    pImpl->patternMatrix.clear();
    
    // Reset statistics
    totalConsolidations_ = 0;
    totalEvictions_ = 0;
    averageStrengthHistory_ = 0.0f;
}

void NeuralWorkingMemory::consolidate(float threshold) {
    if (!brain_) return;
    
    for (auto& trace : memoryTraces_) {
        if (trace.strength >= threshold && !trace.isConsolidated) {
            // Mark for consolidation
            trace.isConsolidated = true;
            
            // Weaken trace in working memory (will be moved to LTM)
            trace.strength *= 0.5f;
            
            // Record consolidation
            totalConsolidations_++;
            
            // Apply consolidation effects
            for (NeuronId neuron : trace.neuronIds) {
                // Weaken synaptic connections in working memory
                brain_->injectCurrent(neuron, -0.2f);
            }
        }
    }
}

void NeuralWorkingMemory::setAttention(Brain* brain) {
    brain_ = brain;
    
    // Apply attention modulation
    if (brain_) {
        for (size_t i = 0; i < winners_.size() && i < memoryNeurons_.size(); ++i) {
            brain_->injectCurrent(winners_[i], 0.5f);
        }
    }
}

void NeuralWorkingMemory::applyTopDownBias(NeuronId neuron, float biasStrength) {
    topDownBias_[neuron.value] += biasStrength;
    
    // Apply bias to memory activation
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = std::min(1.0f, memoryActivations_[idx] + biasStrength * 0.1f);
    }
}

void NeuralWorkingMemory::applyBottomUpSalience(NeuronId neuron, float salienceStrength) {
    bottomUpSalience_[neuron.value] += salienceStrength;
    
    // Apply salience to memory activation
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = std::min(1.0f, memoryActivations_[idx] + salienceStrength * 0.05f);
    }
}

float NeuralWorkingMemory::getMemoryActivity() const {
    if (memoryActivations_.empty()) return 0.0f;
    
    float total = 0.0f;
    for (float activation : memoryActivations_) {
        total += activation;
    }
    
    return total / memoryActivations_.size();
}

float NeuralWorkingMemory::getMemoryStrength() const {
    if (memoryTraces_.empty()) return 0.0f;
    
    float totalStrength = 0.0f;
    for (const auto& trace : memoryTraces_) {
        totalStrength += trace.strength;
    }
    
    return totalStrength / memoryTraces_.size();
}

std::vector<float> NeuralWorkingMemory::getCurrentActivity() const {
    std::vector<float> result;
    result.reserve(memoryNeurons_.size());
    
    for (float activation : memoryActivations_) {
        result.push_back(activation);
    }
    
    return result;
}

bool NeuralWorkingMemory::isStable() const {
    if (memoryTraces_.empty()) return true;
    
    // Check if attractor states have converged
    float variance = 0.0f;
    size_t count = 0;
    
    for (const auto& trace : memoryTraces_) {
        if (trace.attractorState.size() > 1) {
            float mean = std::accumulate(trace.attractorState.begin(), trace.attractorState.end(), 0.0f) / trace.attractorState.size();
            
            for (float val : trace.attractorState) {
                variance += (val - mean) * (val - mean);
                count++;
            }
        }
    }
    
    if (count > 0) {
        variance /= count;
        return variance < 0.01f; // Low variance indicates stability
    }
    
    return true;
}

float NeuralWorkingMemory::getAverageStrength() const {
    if (memoryTraces_.empty()) return 0.0f;
    
    float total = 0.0f;
    for (const auto& trace : memoryTraces_) {
        total += trace.strength;
    }
    
    return total / memoryTraces_.size();
}

float NeuralWorkingMemory::getMaxInterference() const {
    if (memoryTraces_.empty()) return 0.0f;
    
    float maxInterference = 0.0f;
    for (const auto& trace : memoryTraces_) {
        maxInterference = std::max(maxInterference, trace.interferenceScore);
    }
    
    return maxInterference;
}

SimulationStep NeuralWorkingMemory::getOldestTraceAge() const {
    if (memoryTraces_.empty()) return 0;
    
    SimulationStep maxAge = 0;
    for (const auto& trace : memoryTraces_) {
        maxAge = std::max(maxAge, trace.age);
    }
    
    return maxAge;
}

std::vector<float> NeuralWorkingMemory::getPatternVector() const {
    std::vector<float> result;
    
    for (const auto& trace : memoryTraces_) {
        float patternStrength = trace.strength * trace.salience * (1.0f + trace.novelty * 0.5f);
        
        for (float val : trace.contentVector) {
            result.push_back(val * patternStrength);
        }
    }
    
    return result;
}

void NeuralWorkingMemory::integrateWithBrain(Brain* brain) {
    brain_ = brain;
    
    if (!brain_) return;
    
    // Synchronize with brain neurons
    synchronizeWithNeurons();
    
    // Update neuron activity
    updateNeuronActivity();
}

void NeuralWorkingMemory::synchronizeWithNeurons() {
    // Synchronize working memory with brain neurons
    for (size_t i = 0; i < memoryNeurons_.size() && i < memoryTraces_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        // Sync with brain neuron
        if (brain_) {
            brain_->injectCurrent(neuron, activation * 5.0f);
        }
    }
}

void NeuralWorkingMemory::updateNeuronActivity() {
    if (!brain_) return;
    
    // Update neural activity based on working memory state
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        // Update firing rate
        pImpl->neuronFiringRates[neuron.index()] = activation * 20.0f;
        
        // Apply recurrent connections
        for (const auto& conn : pImpl->maintenanceSynapses) {
            if (conn.second == neuron) {
                // Find source activation
                float sourceActivation = 0.0f;
                auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.first);
                if (it != memoryNeurons_.end()) {
                    size_t idx = std::distance(memoryNeurons_.begin(), it);
                    sourceActivation = memoryActivations_[idx];
                }
                
                // Apply recurrent input
                if (brain_) {
                    brain_->injectCurrent(neuron, sourceActivation * pImpl->synapticWeights[conn.first.index()][conn.second.index()]);
                }
            }
        }
    }
}

void NeuralWorkingMemory::step(SimulationStep step) {
    currentStep_ = step;
    currentTime_ = static_cast<Timestamp>(step) * 0.01; // Assuming 0.01s per step
    
    // Update all working memory systems
    update(0.01); // 10ms timestep
}

void NeuralWorkingMemory::logMemoryState() const {
    float activity = getMemoryActivity();
    float strength = getMemoryStrength();
    float interference = getMaxInterference();
    
    NLM_LOG_INFO("Working Memory State - Activity: {:.3f}, Strength: {:.3f}, Interference: {:.3f}, ",
                 activity, strength, interference);
}

void NeuralWorkingMemory::printMemoryStats() const {
    std::cout << "Working Memory Statistics:" << std::endl;
    std::cout << "  Active traces: " << getActiveTraceCount() << "/" << capacity_ << std::endl;
    std::cout << "  Memory activity: " << getMemoryActivity() << std::endl;
    std::cout << "  Average strength: " << getAverageStrength() << std::endl;
    std::cout << "  Max interference: " << getMaxInterference() << std::endl;
    std::cout << "  Oldest trace age: " << getOldestTraceAge() << std::endl;
    std::cout << "  Consolation consolidations: " << totalConsolidations_ << std::endl;
    std::cout << "  Evictions: " << totalEvictions_ << std::endl;
}

std::string NeuralWorkingMemory::getMemorySummary() const {
    std::stringstream ss;
    ss << "WorkingMemory[traces=" << getActiveTraceCount() 
       << ",activity=" << getMemoryActivity()
       << ",strength=" << getAverageStrength()
       << ",interference=" << getMaxInterference() << "]";
    return ss.str();
}

// Helper functions
float NeuralWorkingMemory::getTopDownBias(NeuronId neuron) const {
    auto it = topDownBias_.find(neuron.value);
    return (it != topDownBias_.end()) ? it->second : 0.0f;
}

std::vector<float> NeuralWorkingMemory::retrievePatternFromNeuron(NeuronId neuron) const {
    for (const auto& trace : memoryTraces_) {
        if (std::find(trace.neuronIds.begin(), trace.neuronIds.end(), neuron) != trace.neuronIds.end()) {
            return trace.contentVector;
        }
    }
    return {};
}