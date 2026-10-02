#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../prediction/PredictionErrorSignal.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;                    // Current novelty level (0-1)
    float decayRate;                // Decay rate (0.05 = slower decay for persistence)
    float noveltyThreshold;         // Threshold for surprise (0.15 = moderate sensitivity)
    float explorationBias;         // Weight for exploration vs exploitation
    float predictionError;         // Prediction error from prediction system
    float bayesianSurprise;        // Bayesian surprise measure
    float adaptiveThreshold;       // Adaptive threshold based on recent experience
    std::vector<float> history;    // Novelty history for analysis
    std::vector<float> lastPattern; // Last observation pattern
    std::vector<float> surpriseHistory; // Recent surprise values
    
    // Bayesian surprise calculation components
    std::vector<float> currentPosterior;  // Current belief state
    std::vector<float> priorDistribution;  // Prior expectations
    std::vector<float> likelihood;       // Likelihood of observations
    
    // Exploration-exploitation components
    float explorationRate;         // Current exploration rate
    float maxExploration;         // Maximum exploration capacity
    float learningRate;            // Adaptation rate for exploration
    
    // Randomization for stochastic exploration
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> noiseDist;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.05f), noveltyThreshold(0.15f),
             explorationBias(0.3f), predictionError(0.0f), bayesianSurprise(0.0f),
             adaptiveThreshold(0.15f), explorationRate(0.1f), maxExploration(0.4f),
             learningRate(0.05f), rng(std::random_device{}()),
             uniformDist(0.0f, 1.0f), noiseDist(0.0f, 0.05f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Initialize posterior and prior distributions
    pImpl->currentPosterior.resize(10, 0.1f);  // 10-dimensional belief space
    pImpl->priorDistribution.resize(10, 0.1f);
    pImpl->likelihood.resize(10, 0.1f);
    
    // Set random seed for reproducibility
    pImpl->rng.seed(std::random_device{}());
    
    NLM_LOG_INFO("Novelty detection initialized with Bayesian surprise model");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    // Extract sensory patterns from observations
    std::vector<float> currentPattern, previousPattern;
    
    const SensoryInput* currentInput = observation.getSensoryInput();
    const SensoryInput* previousInput = previousObservation.getSensoryInput();
    
    if (currentInput) {
        currentPattern = currentInput->getData();
    }
    if (previousInput) {
        previousPattern = previousInput->getData();
    }
    
    detectNovelty(currentPattern, previousPattern);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        pImpl->bayesianSurprise = 0.0f;
        return;
    }
    
    // Update surprise history with prediction errors from neural prediction
    if (pImpl->brain) {
        // Try to get prediction error from brain (if available)
        // This simulates getting prediction errors from the neural prediction system
        pImpl->predictionError = computePredictionErrorFromBrain();
        pImpl->bayesianSurprise = computeBayesianSurprise(currentPattern, previousPattern);
    } else {
        // Fallback to simple pattern difference if brain not available
        pImpl->bayesianSurprise = computeBayesianSurprise(currentPattern, previousPattern);
        pImpl->predictionError = 1.0f - computePatternSimilarity(currentPattern, previousPattern);
    }
    
    // Combine prediction error and Bayesian surprise
    float surpriseSignal = pImpl->bayesianSurprise * 0.6f + pImpl->predictionError * 0.4f;
    
    // Apply noise for biological realism
    surpriseSignal += pImpl->noiseDist(pImpl->rng) * 0.1f;
    
    // Update novelty level with exponential moving average
    pImpl->level = pImpl->level * 0.7f + surpriseSignal * 0.3f;
    
    // Adaptive threshold adjustment based on recent surprise
    updateAdaptiveThreshold(surpriseSignal);
    
    // Clamp novelty level
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Update exploration rate based on novelty
    updateExplorationRate(surpriseSignal);
    
    // Record history
    pImpl->history.push_back(pImpl->level);
    pImpl->surpriseHistory.push_back(surpriseSignal);
    
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
        pImpl->surpriseHistory.erase(pImpl->surpriseHistory.begin());
    }
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty with biological realism
    float dtFloat = static_cast<float>(dt);
    
    // Natural decay component (slow for persistence)
    float naturalDecay = pImpl->decayRate * dtFloat;
    
    // Modulation by prediction confidence (less decay if prediction is confident)
    float predictionConfidence = getPredictionConfidence();
    float modulatedDecay = naturalDecay * (1.0f - predictionConfidence * 0.3f);
    
    // Update level with decay
    pImpl->level -= modulatedDecay;
    pImpl->level = std::max(0.0f, pImpl->level);
    
    // Update exploration rate with decay
    pImpl->explorationRate = pImpl->explorationRate * 0.95f + 0.05f;
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
    pImpl->surpriseHistory.clear();
    pImpl->surpriseHistory.reserve(1000);
}

// Helper methods

float Novelty::computePatternSimilarity(const std::vector<float>& pattern1, 
                                        const std::vector<float>& pattern2) const {
    if (pattern1.empty() || pattern2.empty()) return 0.0f;
    
    size_t compareLen = std::min(pattern1.size(), pattern2.size());
    float sum = 0.0f;
    
    for (size_t i = 0; i < compareLen; ++i) {
        sum += pattern1[i] * pattern2[i];
    }
    
    // Cosine similarity
    float norm1 = std::sqrt(std::accumulate(pattern1.begin(), pattern1.begin() + compareLen, 0.0f,
                                           [](float sum, float val) { return sum + val * val; }));
    float norm2 = std::sqrt(std::accumulate(pattern2.begin(), pattern2.begin() + compareLen, 0.0f,
                                           [](float sum, float val) { return sum + val * val; }));
    
    if (norm1 < 0.0001f || norm2 < 0.0001f) return 0.0f;
    
    return sum / (norm1 * norm2);
}

float Novelty::computeBayesianSurprise(const std::vector<float>& currentPattern,
                                       const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) return 0.0f;
    
    // Compute KL divergence between posterior and prior
    float klDivergence = 0.0f;
    size_t minSize = std::min(currentPattern.size(), previousPattern.size());
    
    // Create posterior distribution from current observation
    std::vector<float> posterior(minSize, 0.0f);
    for (size_t i = 0; i < minSize; ++i) {
        // Normalize observation to create probability distribution
        float maxVal = *std::max_element(currentPattern.begin(), currentPattern.begin() + minSize);
        float minVal = *std::min_element(currentPattern.begin(), currentPattern.begin() + minSize);
        
        if (maxVal > minVal) {
            posterior[i] = (currentPattern[i] - minVal) / (maxVal - minVal + 0.001f);
        } else {
            posterior[i] = 0.5f;
        }
    }
    
    // Ensure posterior is normalized
    float posteriorSum = std::accumulate(posterior.begin(), posterior.end(), 0.0f);
    if (posteriorSum > 0.0f) {
        for (size_t i = 0; i < minSize; ++i) {
            posterior[i] /= posteriorSum;
        }
    }
    
    // Compute KL divergence (surprise)
    for (size_t i = 0; i < minSize; ++i) {
        if (posterior[i] > 0.0f) {
            float priorProb = pImpl->priorDistribution[i];
            float klComponent = posterior[i] * std::log((posterior[i] + 0.001f) / (priorProb + 0.001f));
            klDivergence += klComponent;
        }
    }
    
    // Update prior distribution with current observation (learning)
    for (size_t i = 0; i < minSize && i < pImpl->priorDistribution.size(); ++i) {
        pImpl->priorDistribution[i] = pImpl->priorDistribution[i] * 0.95f + posterior[i] * 0.05f;
    }
    
    // Normalize updated prior
    float priorSum = std::accumulate(pImpl->priorDistribution.begin(), 
                                     pImpl->priorDistribution.end(), 0.0f);
    if (priorSum > 0.0f) {
        for (size_t i = 0; i < pImpl->priorDistribution.size(); ++i) {
            pImpl->priorDistribution[i] /= priorSum;
        }
    }
    
    return klDivergence;
}

float Novelty::getPredictionConfidence() const {
    if (!pImpl->brain) return 0.5f;
    
    // Try to get prediction confidence from brain
    // This would ideally come from the NeuralPrediction system
    // For now, return a default value based on novelty level
    float confidence = 1.0f - pImpl->level * 0.5f;
    return std::clamp(confidence, 0.1f, 1.0f);
}

float Novelty::computePredictionErrorFromBrain() {
    if (!pImpl->brain) return 0.0f;
    
    // Try to access prediction system in brain
    // In a real implementation, this would be passed from the prediction system
    // For now, estimate based on novelty level and recent history
    
    float avgSurprise = 0.0f;
    if (!pImpl->surpriseHistory.empty()) {
        avgSurprise = std::accumulate(pImpl->surpriseHistory.begin(),
                                      pImpl->surpriseHistory.end(), 0.0f) /
                                      pImpl->surpriseHistory.size();
    }
    
    return std::min(1.0f, pImpl->level + avgSurprise * 0.5f);
}

void Novelty::updateAdaptiveThreshold(float surpriseSignal) {
    // Adjust threshold based on recent surprise statistics
    if (pImpl->surpriseHistory.size() < 10) return;
    
    // Compute average surprise over recent history
    float recentSum = std::accumulate(pImpl->surpriseHistory.end() - std::min(20, static_cast<int>(pImpl->surpriseHistory.size())),
                                      pImpl->surpriseHistory.end(), 0.0f);
    int recentCount = std::min(20, static_cast<int>(pImpl->surpriseHistory.size()));
    float avgSurprise = recentSum / recentCount;
    
    // Adjust threshold based on average surprise
    if (avgSurprise > 0.5f) {
        // High average surprise -> lower threshold to detect more novel events
        pImpl->adaptiveThreshold = std::max(0.05f, pImpl->noveltyThreshold * 0.7f);
    } else if (avgSurprise < 0.2f) {
        // Low average surprise -> higher threshold to avoid false positives
        pImpl->adaptiveThreshold = std::min(0.3f, pImpl->noveltyThreshold * 1.3f);
    } else {
        // Moderate average surprise -> keep original threshold
        pImpl->adaptiveThreshold = pImpl->noveltyThreshold;
    }
}

void Novelty::updateExplorationRate(float surpriseSignal) {
    // Balance exploration vs exploitation based on novelty and prediction confidence
    float predictionConfidence = getPredictionConfidence();
    
    // High surprise + low confidence -> more exploration
    float explorationDrive = surpriseSignal * (1.0f - predictionConfidence);
    
    // Update exploration rate with biological adaptation
    float targetExploration = pImpl->maxExploration * explorationDrive * pImpl->explorationBias;
    pImpl->explorationRate = pImpl->explorationRate * (1.0f - pImpl->learningRate) + 
                            targetExploration * pImpl->learningRate;
    
    // Add stochastic component for exploration
    if (pImpl->uniformDist(pImpl->rng) < pImpl->explorationRate * 0.1f) {
        // Occasionally jump to higher exploration rate
        pImpl->explorationRate = std::min(pImpl->explorationRate * 1.5f, pImpl->maxExploration);
    }
}

} // namespace nlm
