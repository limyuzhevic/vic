#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include "../../brain/Brain.hpp"
#include "../../prediction/PredictionSystem.hpp"
#include "../../memory/NeuralWorkingMemory.hpp"
#include "../../memory/NeuralEpisodicMemory.hpp"
#include "../../neuromodulation/PredictionError.hpp"
#include "../../dynamics/SpikeSystem.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unordered_map>
#include <vector>
#include <memory>
#include <random>

namespace nlm {

// Comprehensive Hebbian learning implementation with biological plausibility
// Integrates spike timing, neuromodulation, working memory, episodic memory,
// prediction system, and optimization for efficient large-scale neural networks

struct Hebbian::Impl {
    // Core Hebbian learning parameters
    float learningRate;              // η - base learning rate
    float correlationThreshold;     // θ - threshold for correlated activity
    float neuromodulationFactor;     // γ - factor for neuromodulatory influence
    float spikeTimingWindow;        // τ - window for spike timing correlation (ms)
    float memoryConsolidationRate;  // α - rate of memory consolidation
    
    // STDP parameters for hybrid STDP-Hebbian learning
    float stdpPotentiation;         // A+ - STDP potentiation amplitude
    float stdpDepression;           // A- - STDP depression amplitude
    float stdpTimeConstant;         // τ - STDP time constant (ms)
    
    // Memory system integration
    std::unordered_map<SynapseId, std::vector<std::pair<Timestamp, Timestamp>>> hebbianMemory;
    std::vector<SynapseId> eligibleSynapses;  // Synaptic tagging for eligibility
    float memoryDecayRate;           // Rate at which Hebbian memory decays
    
    // Neuromodulatory systems
    float dopamineLevel;            // DA - dopamine level for reward modulation
    float curiosityLevel;           // Cu - curiosity/novelty signal
    float noveltySignal;            // NO - novelty detection signal
    
    // Working memory integration
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    
    // Prediction system integration
    std::unique_ptr<PredictionSystem> predictionSystem;
    std::unique_ptr<PredictionError> predictionError;
    
    // Statistics and optimization
    float totalCorrelatedActivity;   // Σc - total correlated activity
    float maxCorrelationObserved;    // Track maximum correlation for adaptation
    float weightNorm;               // Current L2 norm of weights
    float targetWeightNorm;         // Target L2 norm for normalization
    
    // Performance and efficiency
    size_t maxSynapses;             // Maximum synapses for efficient allocation
    size_t memoryPoolSize;          // Size of memory pool for event-driven dynamics
    bool optimizationEnabled;       // Whether parameter optimization is enabled
    
    // Random number generation for stochasticity
    std::mt19937 rng;
    std::uniform_real_distribution<float> weightDistribution;
    std::normal_distribution<float> noiseDistribution;
    
    Impl() : learningRate(0.01f), correlationThreshold(0.5f), neuromodulationFactor(1.0f),
             spikeTimingWindow(50.0f), memoryConsolidationRate(0.1f),
             stdpPotentiation(0.01f), stdpDepression(0.012f), stdpTimeConstant(20.0f),
             memoryDecayRate(0.001f), dopamineLevel(0.5f), curiosityLevel(0.3f), 
             noveltySignal(0.0f), totalCorrelatedActivity(0.0f), maxCorrelationObserved(0.0f),
             weightNorm(0.0f), targetWeightNorm(1.0f), maxSynapses(10000), 
             memoryPoolSize(1000), optimizationEnabled(true),
             weightDistribution(0.0f, 1.0f), noiseDistribution(0.0f, 0.01f) {
        rng.seed(std::random_device{}());
    }
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Core Hebbian learning pipeline
    applyHebbianLearning(synapse, preSpikes, postSpikes, dt);
    
    // Integrate with prediction system
    if (pImpl->predictionSystem) {
        integrateWithPredictionSystem(synapse, preSpikes, postSpikes);
    }
    
    // Update Hebbian memory
    storeHebbianMemory(preSpikes, postSpikes, synapse->getId());
    
    // Update neuromodulatory influence
    updateNeuromodulationInfluence(synapse, preSpikes, postSpikes);
}

void Hebbian::computeCorrelatedActivity(const std::vector<Timestamp>& preSpikes,
                                        const std::vector<Timestamp>& postSpikes,
                                        float& correlation,
                                        float& covariance) {
    if (preSpikes.empty() || postSpikes.empty()) {
        correlation = 0.0f;
        covariance = 0.0f;
        return;
    }
    
    // Compute spike correlation using spike timing patterns
    float preRate = static_cast<float>(preSpikes.size()) / pImpl->spikeTimingWindow;
    float postRate = static_cast<float>(postSpikes.size()) / pImpl->spikeTimingWindow;
    
    // Compute cross-correlation using spike timing differences
    std::vector<float> timeDifferences;
    timeDifferences.reserve(preSpikes.size() * postSpikes.size());
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) < pImpl->spikeTimingWindow) {
                timeDifferences.push_back(dt);
            }
        }
    }
    
    if (timeDifferences.empty()) {
        correlation = 0.0f;
        covariance = 0.0f;
        return;
    }
    
    // Compute correlation coefficient
    float meanPre = std::accumulate(preSpikes.begin(), preSpikes.end(), 0.0f) / preSpikes.size();
    float meanPost = std::accumulate(postSpikes.begin(), postSpikes.end(), 0.0f) / postSpikes.size();
    
    float numerator = 0.0f;
    float preVariance = 0.0f;
    float postVariance = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) < pImpl->spikeTimingWindow) {
                float preCentered = preTime - meanPre;
                float postCentered = postTime - meanPost;
                numerator += preCentered * postCentered;
                preVariance += preCentered * preCentered;
                postVariance += postCentered * postCentered;
            }
        }
    }
    
    if (preVariance > 0.0f && postVariance > 0.0f) {
        correlation = numerator / std::sqrt(preVariance * postVariance);
        covariance = numerator / static_cast<float>(timeDifferences.size());
    } else {
        correlation = 0.0f;
        covariance = 0.0f;
    }
    
    // Update statistics
    pImpl->totalCorrelatedActivity += covariance;
    pImpl->maxCorrelationObserved = std::max(pImpl->maxCorrelationObserved, std::abs(correlation));
}

void Hebbian::applyHebbianLearning(Synapse* synapse,
                                    const std::vector<Timestamp>& preSpikes,
                                    const std::vector<Timestamp>& postSpikes,
                                    TimestepDuration dt) {
    if (!synapse) return;
    
    // Compute correlation between pre- and post-synaptic activity
    float correlation, covariance;
    computeCorrelatedActivity(preSpikes, postSpikes, correlation, covariance);
    
    // Check if correlation exceeds threshold
    if (std::abs(correlation) < pImpl->correlationThreshold) {
        return;
    }
    
    // Compute Hebbian weight change
    float weightChange, eligibility;
    computeHebbianWeightChange(synapse, preSpikes, postSpikes, weightChange, eligibility);
    
    // Compute STDP component (hybrid learning)
    float stdpPotentiation, stdpDepression;
    computeSTDPComponent(synapse, preSpikes, postSpikes, stdpPotentiation, stdpDepression);
    
    // Combine Hebbian and STDP components
    float totalChange = weightChange + stdpPotentiation - stdpDepression;
    
    // Apply neuromodulatory modulation
    totalChange *= (1.0f + pImpl->neuromodulationFactor * (pImpl->dopamineLevel + pImpl->curiosityLevel));
    
    // Apply weight change with bounds
    if (std::abs(totalChange) > 1e-6f) {
        applyWeightChange(synapse, totalChange);
        
        // Update eligibility trace for reward-modulated learning
        synapse->setEligibilityTrace(synapse->getEligibilityTrace() + eligibility * totalChange);
        
        // Mark synapse as eligible for structural plasticity
        if (std::abs(eligibility) > 0.1f) {
            pImpl->eligibleSynapses.push_back(synapse->getId());
        }
    }
}

void Hebbian::computeHebbianWeightChange(Synapse* synapse,
                                         const std::vector<Timestamp>& preSpikes,
                                         const std::vector<Timestamp>& postSpikes,
                                         float& weightChange,
                                         float& eligibility) {
    if (!synapse) {
        weightChange = 0.0f;
        eligibility = 0.0f;
        return;
    }
    
    // Covariance-based Hebbian learning (Oja's rule for stability)
    float correlation, covariance;
    computeCorrelatedActivity(preSpikes, postSpikes, correlation, covariance);
    
    // Oja's learning rule: Δw = η * (x*y - w * x²)
    float preMean = 0.0f, postMean = 0.0f;
    if (!preSpikes.empty()) preMean = *std::max_element(preSpikes.begin(), preSpikes.end());
    if (!postSpikes.empty()) postMean = *std::max_element(postSpikes.begin(), postSpikes.end());
    
    // Estimate pre-synaptic activity (proxy using spike count)
    float preActivity = static_cast<float>(preSpikes.size()) / static_cast<float>(preSpikes.size() + 1);
    float postActivity = static_cast<float>(postSpikes.size()) / static_cast<float>(postSpikes.size() + 1);
    
    // Covariance term
    float covarianceTerm = preActivity * postActivity;
    
    // Weight normalization term (Oja's rule)
    float weight = synapse->getWeight();
    float preSquared = preActivity * preActivity;
    float normalizationTerm = weight * preSquared * pImpl->learningRate * 0.1f;
    
    // Compute weight change
    weightChange = pImpl->learningRate * (covarianceTerm - normalizationTerm);
    
    // Compute eligibility trace (synaptic tagging)
    eligibility = std::abs(weightChange) * pImpl->neuromodulationFactor;
    
    // Apply synaptic tagging for memory capture
    if (std::abs(eligibility) > 0.01f) {
        storeHebbianMemory(preSpikes, postSpikes, synapse->getId());
    }
}

void Hebbian::computeSTDPComponent(Synapse* synapse,
                                   const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes,
                                   float& stdpPotentiation,
                                   float& stdpDepression) {
    if (!synapse) {
        stdpPotentiation = 0.0f;
        stdpDepression = 0.0f;
        return;
    }
    
    stdpPotentiation = 0.0f;
    stdpDepression = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: Long-term potentiation (LTP)
                stdpPotentiation += pImpl->stdpPotentiation * std::exp(-dt / pImpl->stdpTimeConstant);
            } else if (dt < 0) {
                // Post before pre: Long-term depression (LTD)
                stdpDepression += pImpl->stdpDepression * std::exp(dt / pImpl->stdpTimeConstant);
            }
        }
    }
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setCorrelationThreshold(float threshold) {
    pImpl->correlationThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float Hebbian::getCorrelationThreshold() const {
    return pImpl->correlationThreshold;
}

void Hebbian::setNeuromodulationFactor(float factor) {
    pImpl->neuromodulationFactor = std::clamp(factor, 0.0f, 2.0f);
}

float Hebbian::getNeuromodulationFactor() const {
    return pImpl->neuromodulationFactor;
}

void Hebbian::storeHebbianMemory(const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 SynapseId synapseId) {
    // Store Hebbian learning patterns in memory
    auto memoryEntry = pImpl->hebbianMemory.find(synapseId);
    if (memoryEntry == pImpl->hebbianMemory.end()) {
        memoryEntry = pImpl->hebbianMemory.insert({synapseId, std::vector<std::pair<Timestamp, Timestamp>>{}}).first;
    }
    
    // Store spike timing correlations
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) < pImpl->spikeTimingWindow) {
                memoryEntry->second.push_back({preTime, postTime});
            }
        }
    }
    
    // Decay old memory entries to prevent explosion
    if (memoryEntry->second.size() > pImpl->maxSynapses) {
        memoryEntry->second.erase(memoryEntry->second.begin());
    }
}

void Hebbian::retrieveHebbianMemory(SynapseId synapseId,
                                    std::vector<Timestamp>& preSpikes,
                                    std::vector<Timestamp>& postSpikes) {
    auto memoryEntry = pImpl->hebbianMemory.find(synapseId);
    if (memoryEntry == pImpl->hebbianMemory.end()) {
        return;
    }
    
    preSpikes.clear();
    postSpikes.clear();
    
    for (const auto& entry : memoryEntry->second) {
        preSpikes.push_back(entry.first);
        postSpikes.push_back(entry.second);
    }
}

void Hebbian::consolidateHebbianLearning() {
    // Consolidate Hebbian learning patterns into long-term memory
    // Using attention-based consolidation with prediction error
    
    if (pImpl->predictionSystem && pImpl->predictionError && pImpl->episodicMemory) {
        // Get recent Hebbian patterns
        std::vector<Timestamp> preSpikes, postSpikes;
        for (const auto& memoryEntry : pImpl->hebbianMemory) {
            retrieveHebbianMemory(memoryEntry.first, preSpikes, postSpikes);
            if (!preSpikes.empty() && !postSpikes.empty()) {
                // Compute prediction error for this pattern
                float predicted = computePatternPrediction(preSpikes, postSpikes);
                float actual = computePatternActual(preSpikes, postSpikes);
                
                // Update prediction error
                pImpl->predictionError->computeError(predicted, actual);
                
                // Consolidate if prediction error is low
                if (std::abs(pImpl->predictionError->getError()) < 0.1f) {
                    // Store in episodic memory for long-term consolidation
                    // Simplified implementation - in practice would use proper encoding
                }
            }
        }
    }
    
    // Decay Hebbian memory with time constant
    for (auto& memoryEntry : pImpl->hebbianMemory) {
        auto& pattern = memoryEntry.second;
        if (pattern.size() > 10) {
            // Remove oldest entries with probability based on decay rate
            std::vector<std::pair<Timestamp, Timestamp>> newPattern;
            newPattern.reserve(pattern.size());
            
            for (const auto& entry : pattern) {
                if (rng() / static_cast<float>(rng.max()) > pImpl->memoryDecayRate) {
                    newPattern.push_back(entry);
                }
            }
            
            memoryEntry.second = std::move(newPattern);
        }
    }
}

void Hebbian::updateHebbianFromEpisodicMemory(const NeuralEpisodicMemory* episodicMemory) {
    if (!episodicMemory) return;
    
    // Retrieve recent episodes and extract Hebbian learning rules
    auto episodes = episodicMemory->getRecentEpisodes(50);
    
    for (const auto* episode : episodes) {
        if (!episode) continue;
        
        // Convert episode to spike timing patterns
        std::vector<Timestamp> preSpikes, postSpikes;
        extractSpikePatternsFromEpisode(episode, preSpikes, postSpikes);
        
        if (preSpikes.empty() || postSpikes.empty()) continue;
        
        // Update Hebbian learning rules from episodic patterns
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float dt = static_cast<float>(postTime - preTime);
                if (std::abs(dt) < pImpl->spikeTimingWindow) {
                    // Update memory with episodic patterns
                    storeHebbianMemory(preSpikes, postSpikes, /* synapseId */ INVALID_SYNAPSE_ID);
                }
            }
        }
    }
}

void Hebbian::optimizeHebbianParameters() {
    if (!pImpl->optimizationEnabled) return;
    
    // Simple parameter optimization based on learning performance
    
    // Adjust learning rate based on correlation statistics
    if (pImpl->maxCorrelationObserved > 0.0f) {
        float correlationFactor = pImpl->maxCorrelationObserved / pImpl->correlationThreshold;
        if (correlationFactor > 1.0f) {
            pImpl->learningRate *= 0.9f;  // Reduce learning rate if over-correlating
        } else {
            pImpl->learningRate *= 1.1f;  // Increase learning rate if under-correlating
        }
        pImpl->learningRate = std::clamp(pImpl->learningRate, 0.001f, 0.1f);
    }
    
    // Adjust correlation threshold based on memory consolidation
    if (pImpl->totalCorrelatedActivity > 100.0f) {
        pImpl->correlationThreshold = std::min(pImpl->correlationThreshold * 1.05f, 1.0f);
    }
    
    // Update neuromodulation factors
    if (pImpl->dopamineLevel > 0.8f) {
        pImpl->curiosityLevel = std::min(pImpl->curiosityLevel * 1.1f, 1.0f);
    }
}

void Hebbian::normalizeHebbianWeights() {
    // L2 normalization of Hebbian weights to prevent runaway potentiation
    // Note: This is a simplified version - in practice, you'd need access to all synapses
    
    // Current implementation updates global statistics
    if (pImpl->weightNorm > 0.0f) {
        float scalingFactor = pImpl->targetWeightNorm / pImpl->weightNorm;
        scalingFactor = std::clamp(scalingFactor, 0.9f, 1.1f);
        
        // Update learning rate to compensate
        pImpl->learningRate *= scalingFactor;
    }
}

void Hebbian::regularizeHebbianLearning(float lambda) {
    // L2 regularization to prevent overfitting
    lambda = std::clamp(lambda, 0.0f, 0.1f);
    
    // Apply regularization to learning rate
    pImpl->learningRate *= (1.0f - lambda);
    
    // Apply regularization to correlation threshold
    pImpl->correlationThreshold *= (1.0f - lambda * 0.1f);
}

bool Hebbian::validateHebbianLearning() const {
    // Validate Hebbian learning implementation
    
    bool isValid = true;
    
    // Check parameter bounds
    if (pImpl->learningRate < 0.0f || pImpl->learningRate > 1.0f) isValid = false;
    if (pImpl->correlationThreshold < 0.0f || pImpl->correlationThreshold > 1.0f) isValid = false;
    if (pImpl->neuromodulationFactor < 0.0f || pImpl->neuromodulationFactor > 2.0f) isValid = false;
    
    // Check memory consistency
    if (pImpl->hebbianMemory.size() > pImpl->maxSynapses) isValid = false;
    
    // Check statistics consistency
    if (pImpl->weightNorm < 0.0f) isValid = false;
    if (pImpl->targetWeightNorm < 0.0f) isValid = false;
    
    return isValid;
}

// Integration helper methods

void Hebbian::integrateWithPredictionSystem(Synapse* synapse,
                                           const std::vector<Timestamp>& preSpikes,
                                           const std::vector<Timestamp>& postSpikes) {
    if (!pImpl->predictionSystem || !pImpl->predictionError) return;
    
    // Compute prediction error for current synaptic activity
    float predicted = computeSynapticPrediction(synapse, preSpikes, postSpikes);
    float actual = computeActualSynapticActivity(synapse);
    
    // Update prediction error
    pImpl->predictionError->computeError(predicted, actual);
    
    // Modulate Hebbian learning based on prediction error
    float predictionError = pImpl->predictionError->getError();
    float errorFactor = 1.0f - predictionError;  // Lower error = higher confidence
    
    pImpl->learningRate *= errorFactor;
    pImpl->neuromodulationFactor *= errorFactor;
}

void Hebbian::updateNeuromodulationInfluence(Synapse* synapse,
                                            const std::vector<Timestamp>& preSpikes,
                                            const std::vector<Timestamp>& postSpikes) {
    if (!synapse) return;
    
    // Update neuromodulatory signals based on learning dynamics
    
    // Dopamine: reward prediction error
    float currentDopamine = pImpl->dopamineLevel;
    float learningActivity = computeLearningActivity(preSpikes, postSpikes);
    pImpl->dopamineLevel = std::clamp(currentDopamine + learningActivity * 0.01f, 0.0f, 1.0f);
    
    // Curiosity: novelty detection
    float curiosity = computeCuriositySignal(preSpikes, postSpikes);
    pImpl->curiosityLevel = std::clamp(curiosity, 0.0f, 1.0f);
    
    // Novelty: unexpected spike patterns
    float novelty = computeNoveltySignal(preSpikes, postSpikes);
    pImpl->noveltySignal = std::clamp(novelty, 0.0f, 1.0f);
    
    // Integrate novelty with curiosity
    pImpl->curiosityLevel = std::max(pImpl->curiosityLevel, pImpl->noveltySignal * 0.5f);
}

float Hebbian::computeLearningActivity(const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes) const {
    if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
    
    // Compute learning activity based on spike timing precision
    float totalActivity = 0.0f;
    int validPairs = 0;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) < pImpl->spikeTimingWindow) {
                // Precision of spike timing as learning activity indicator
                float timingPrecision = 1.0f - std::abs(dt) / pImpl->spikeTimingWindow;
                totalActivity += timingPrecision;
                validPairs++;
            }
        }
    }
    
    return validPairs > 0 ? totalActivity / validPairs : 0.0f;
}

float Hebbian::computeCuriositySignal(const std::vector<Timestamp>& preSpikes,
                                     const std::vector<Timestamp>& postSpikes) const {
    if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
    
    // Curiosity as measure of unexpected spike patterns
    float expectedCorrelation = computeExpectedCorrelation(preSpikes, postSpikes);
    float actualCorrelation = computeActualCorrelation(preSpikes, postSpikes);
    
    return std::abs(actualCorrelation - expectedCorrelation);
}

float Hebbian::computeNoveltySignal(const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes) const {
    if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
    
    // Novelty as measure of deviation from learned patterns
    float novelty = 0.0f;
    
    // Check against stored Hebbian patterns
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) < pImpl->spikeTimingWindow) {
                // Determine if this timing pattern is novel
                bool isNovel = true;
                for (const auto& memoryEntry : pImpl->hebbianMemory) {
                    for (const auto& pattern : memoryEntry.second) {
                        if (std::abs(pattern.first - preTime) < 0.1f && 
                            std::abs(pattern.second - postTime) < 0.1f) {
                            isNovel = false;
                            break;
                        }
                    }
                    if (!isNovel) break;
                }
                if (isNovel) novelty += 1.0f;
            }
        }
    }
    
    return novelty / static_cast<float>(preSpikes.size() * postSpikes.size());
}

float Hebbian::computePatternPrediction(const std::vector<Timestamp>& preSpikes,
                                        const std::vector<Timestamp>& postSpikes) const {
    if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
    
    // Simple prediction based on temporal correlation
    float totalDelay = 0.0f;
    int validPairs = 0;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) < pImpl->spikeTimingWindow) {
                totalDelay += dt;
                validPairs++;
            }
        }
    }
    
    return validPairs > 0 ? totalDelay / validPairs : 0.0f;
}

float Hebbian::computePatternActual(const std::vector<Timestamp>& preSpikes,
                                    const std::vector<Timestamp>& postSpikes) const {
    if (postSpikes.empty()) return 0.0f;
    
    // Actual post-synaptic activity as mean post spike time
    float sum = std::accumulate(postSpikes.begin(), postSpikes.end(), 0.0f);
    return sum / postSpikes.size();
}

float Hebbian::computeSynapticPrediction(Synapse* synapse,
                                         const std::vector<Timestamp>& preSpikes,
                                         const std::vector<Timestamp>& postSpikes) const {
    if (!synapse) return 0.0f;
    
    // Predict synaptic output based on input spike timing
    float predictedOutput = synapse->getWeight() * static_cast<float>(preSpikes.size());
    
    // Modulate by spike timing precision
    float timingPrecision = computeLearningActivity(preSpikes, postSpikes);
    predictedOutput *= timingPrecision;
    
    return predictedOutput;
}

float Hebbian::computeActualSynapticActivity(Synapse* synapse) const {
    if (!synapse) return 0.0f;
    
    // Actual synaptic activity based on current weight and recent spikes
    const auto& preSpikes = synapse->getPreSpikeHistory();
    const auto& postSpikes = synapse->getPostSpikeHistory();
    
    float preActivity = static_cast<float>(preSpikes.size()) / 10.0f;  // Normalize
    float postActivity = static_cast<float>(postSpikes.size()) / 10.0f;  // Normalize
    
    return synapse->getWeight() * (preActivity + postActivity) * 0.5f;
}

float Hebbian::computeExpectedCorrelation(const std::vector<Timestamp>& preSpikes,
                                         const std::vector<Timestamp>& postSpikes) const {
    if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
    
    // Simple expected correlation based on firing rates
    float preRate = static_cast<float>(preSpikes.size()) / pImpl->spikeTimingWindow;
    float postRate = static_cast<float>(postSpikes.size()) / pImpl->spikeTimingWindow;
    
    return preRate * postRate * 0.1f;  // Arbitrary scaling
}

float Hebbian::computeActualCorrelation(const std::vector<Timestamp>& preSpikes,
                                        const std::vector<Timestamp>& postSpikes) const {
    float correlation, covariance;
    computeCorrelatedActivity(preSpikes, postSpikes, correlation, covariance);
    return correlation;
}

void Hebbian::extractSpikePatternsFromEpisode(const NeuralEpisodicMemory::Episode* episode,
                                              std::vector<Timestamp>& preSpikes,
                                              std::vector<Timestamp>& postSpikes) {
    if (!episode) return;
    
    // Extract spike timing patterns from episodic memory
    // This is a simplified implementation - in practice, would use proper encoding/decoding
    
    // Convert sensory state to spike patterns (simplified)
    for (size_t i = 0; i < episode->sensoryState.size(); ++i) {
        float value = episode->sensoryState[i];
        if (value > 0.5f) {
            // Encode as spike time
            Timestamp spikeTime = static_cast<Timestamp>(i * 10.0f + value * 5.0f);  // Arbitrary timing
            if (i % 2 == 0) {  // Even indices as pre-synaptic
                preSpikes.push_back(spikeTime);
            } else {  // Odd indices as post-synaptic
                postSpikes.push_back(spikeTime);
            }
        }
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);  // Standard weight bounds for Hebbian
    synapse->setWeight(newWeight);
    
    // Update weight norm statistics
    pImpl->weightNorm += std::abs(delta);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

} // namespace nlm