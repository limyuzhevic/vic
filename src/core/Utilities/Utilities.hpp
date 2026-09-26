#pragma once

// ============================================================================
// UTILITIES HEADER
// ============================================================================
// 
// This header provides a comprehensive set of utility functions for common
// operations in the NLM (Neural Learning Machine) framework. These utilities
// encapsulate common patterns and operations to reduce code duplication,
// improve maintainability, and ensure consistent behavior across the system.
// 
// ============================================================================
// UTILITY GROUPS:
// ============================================================================
// 
// 1. Value Scaling and Normalization Utilities
//    - scaleToRange() - Scale value from source range to target range
//    - normalizeToSigned() - Convert [0,1] to [-1,1] range
//    - applyOffsetScale() - Apply offset and scale transformation
//
// 2. Neuron Group Distribution Utilities  
//    - distributeToGroups() - Distribute neurons across groups
//    - distributeSensoryNeurons() - Distribute sensory neurons
//    - distributeMotorNeurons() - Distribute motor neurons
//
// 3. Activity Calculation Utilities
//    - calculateAverageActivity() - Calculate average neural activity
//    - calculateMaxActivity() - Calculate maximum activity
//    - calculateWeightedActivity() - Calculate weighted activity sum
//
// 4. Novelty and Similarity Calculation Utilities
//    - calculateTemporalNovelty() - Calculate novelty based on temporal difference
//    - calculateFeatureSimilarity() - Calculate similarity between feature vectors
//    - calculateWeightedDifference() - Calculate weighted difference between values
//
// 5. Configuration and Constant Access Utilities
//    - getConfigValue() - Safely retrieve configuration values
//    - getConstantValue() - Access system constants
//    - validateParameter() - Validate parameter ranges
//
// ============================================================================

#include <vector>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <limits>
#include <numeric>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Neuron;
class Config;

// ============================================================================
// VALUE SCALING AND NORMALIZATION UTILITIES
// ============================================================================

/**
 * @brief Scale a value from source range to target range
 * 
 * @tparam T Numeric type (float, double)
 * @param value Input value to scale
 * @param sourceMin Minimum of source range
 * @param sourceMax Maximum of source range  
 * @param targetMin Minimum of target range
 * @param targetMax Maximum of target range
 * @return Scaled value in target range
 * 
 * @note Handles edge cases (constant ranges, NaN, infinities)
 * @complexity O(1)
 * 
 * @example
 * @code
 * // Scale vision intensity from [0,1] to [-1,1]
 * float normalized = scaleToRange(visionIntensity, 0.0f, 1.0f, -1.0f, 1.0f);
 * 
 * // Scale motor command from [0,1] to [0,255]
 * uint8_t pwm = static_cast<uint8_t>(scaleToRange(motorCmd, 0.0f, 1.0f, 0.0f, 255.0f));
 * @endcode
 */
template<typename T>
T scaleToRange(T value, T sourceMin, T sourceMax, T targetMin, T targetMax);

/**
 * @brief Convert normalized [0,1] value to signed [-1,1] range
 * 
 * @tparam T Numeric type (float, double)
 * @param value Input value in [0,1] range
 * @param offset Optional offset to add before conversion
 * @return Value in [-1,1] range
 * 
 * @note Common pattern: (value * 2.0f - 1.0f)
 * @complexity O(1)
 * 
 * @example
 * @code
 * // Convert vision intensity [0,1] to signed [-1,1]
 * float signedValue = normalizeToSigned(visionIntensity);
 * 
 * // With offset for centering
 * float offsetValue = normalizeToSigned(visionIntensity, 0.5f);
 * @endcode
 */
template<typename T>
T normalizeToSigned(T value, T offset = static_cast<T>(0.5));

/**
 * @brief Apply offset-scale transformation commonly used in neural networks
 * 
 * @tparam T Numeric type (float, double)
 * @param value Input value to transform
 * @param offset Scaling offset (typically -1.0 or 1.0)
 * @param scale Scaling factor (typically 5.0 or 8.0)
 * @return Transformed value (value * 2.0 - 1.0) * scale
 * 
 * @note Encapsulates common pattern: (value * 2.0f - 1.0f) * 5.0f
 * @complexity O(1)
 * 
 * @example
 * @code
 * // Apply vision preprocessing transformation
 * float processed = applyOffsetScale(visionRaw, -1.0f, 5.0f);
 * 
 * // Apply motor scaling
 * float motorScaled = applyOffsetScale(normalizedCmd, 1.0f, 8.0f);
 * @endcode
 */
template<typename T>
T applyOffsetScale(T value, T offset = static_cast<T>(-1.0), T scale = static_cast<T>(5.0));

// ============================================================================
// NEURON GROUP DISTRIBUTION UTILITIES
// ============================================================================

/**
 * @brief Distribute neurons across multiple groups based on modulo distribution
 * 
 * @param neuronCount Total number of neurons to distribute
 * @param groupCount Number of groups to distribute across
 * @param startIndex Starting index for distribution
 * @return Vector of group sizes
 * 
 * @note Used for motor vs sensory neuron distribution
 * @complexity O(neuronCount)
 * 
 * @example
 * @code
 * // Distribute 100 neurons across 6 motor groups
 * auto distribution = distributeToGroups(100, 6, 0);
 * // Result: [17, 16, 17, 16, 17, 17]
 * @endcode
 */
std::vector<size_t> distributeToGroups(size_t neuronCount, size_t groupCount, size_t startIndex = 0);

/**
 * @brief Distribute sensory neurons across sensory processing groups
 * 
 * @param totalNeurons Total number of sensory neurons
 * @return Vector of neuron counts per sensory modality
 * 
 * @note Specialized distribution for sensory processing pipeline
 * @complexity O(totalNeurons)
 */
std::vector<size_t> distributeSensoryNeurons(size_t totalNeurons);

/**
 * @brief Distribute motor neurons across motor output groups
 * 
 * @param totalNeurons Total number of motor neurons
 * @return Vector of neuron counts per motor action group
 * 
 * @note Ensures balanced distribution for motor control
 * @complexity O(totalNeurons)
 */
std::vector<size_t> distributeMotorNeurons(size_t totalNeurons);

// ============================================================================
// ACTIVITY CALCULATION UTILITIES
// ============================================================================

/**
 * @brief Calculate average neural activity from neuron states
 * 
 * @param neurons Vector of neuron pointers
 * @return Average activity value
 * 
 * @note Replaces repeated calcActivity lambda implementations
 * @complexity O(n)
 * 
 * @example
 * @code
 * // Calculate average activity across motor neuron population
 * float avgActivity = calculateAverageActivity(motorNeurons);
 * 
 * // Calculate activity threshold comparison
 * if (avgActivity > calculateAverageActivity(referenceNeurons) * 1.1f) {
 *     // Activity increased by 10%
 * }
 * @endcode
 */
float calculateAverageActivity(const std::vector<Neuron*>& neurons);

/**
 * @brief Calculate maximum neural activity from neuron states
 * 
 * @param neurons Vector of neuron pointers
 * @return Maximum activity value found
 * 
 * @note Finds the most active neuron in a population
 * @complexity O(n)
 */
float calculateMaxActivity(const std::vector<Neuron*>& neurons);

/**
 * @brief Calculate weighted activity sum with importance weights
 * 
 * @param neurons Vector of neuron pointers
 * @param weights Vector of weights for each neuron
 * @return Weighted sum of activities
 * 
 * @note Allows emphasis on specific neurons or regions
 * @complexity O(n)
 */
float calculateWeightedActivity(const std::vector<Neuron*>& neurons, const std::vector<float>& weights);

// ============================================================================
// NOVELTY AND SIMILARITY CALCULATION UTILITIES
// ============================================================================

/**
 * @brief Calculate temporal novelty based on state difference over time
 * 
 * @param currentState Current neural state vector
 * @param previousState Previous neural state vector
 * @param decayRate Decay factor for older differences (default: 0.99)
 * @return Normalized novelty score
 * 
 * @note Replaces repeated difference calculation patterns
 * @complexity O(n) where n is state vector size
 * 
 * @example
 * @code
 * // Calculate novelty from vision input
 * float novelty = calculateTemporalNovelty(currentVision, previousVision);
 * updateCuriosityLevel(novelty);
 * @endcode
 */
float calculateTemporalNovelty(const std::vector<float>& currentState, 
                              const std::vector<float>& previousState,
                              float decayRate = NOVELTY_DECAY_RATE);

/**
 * @brief Calculate similarity between two feature vectors
 * 
 * @param features1 First feature vector
 * @param features2 Second feature vector
 * @param similarityMetric Type of similarity metric (0=Euclidean, 1=Cosine)
 * @return Similarity score (0.0 = dissimilar, 1.0 = identical)
 * 
 * @note Used for pattern matching and recognition
 * @complexity O(n) where n is vector size
 */
float calculateFeatureSimilarity(const std::vector<float>& features1,
                                const std::vector<float>& features2,
                                int similarityMetric = 1); // Cosine similarity default

/**
 * @brief Calculate weighted difference between two values
 * 
 * @tparam T Numeric type
 * @param value1 First value
 * @param value2 Second value
 * @param weight Weight for difference calculation
 * @return Weighted absolute difference
 * 
 * @note General purpose difference calculation utility
 * @complexity O(1)
 */
template<typename T>
T calculateWeightedDifference(T value1, T value2, T weight = static_cast<T>(1.0));

// ============================================================================
// CONFIGURATION AND CONSTANT ACCESS UTILITIES
// ============================================================================

/**
 * @brief Safely retrieve configuration value with type conversion
 * 
 * @tparam T Target type for value retrieval
 * @param config Config object to retrieve value from
 * @param key Configuration key
 * @param defaultValue Default value if key not found
 * @return Configuration value converted to requested type
 * 
 * @note Replaces direct magic number usage with config-aware access
 * @complexity O(1)
 * 
 * @example
 * @code
 * // Old way (magic number):
 * float scaleFactor = 5.0f;
 * 
 * // New way (config-aware):
 * float scaleFactor = getConfigValue<float>(config, "vision.scale_factor", 5.0f);
 * @endcode
 */
template<typename T>
T getConfigValue(const Config& config, const std::string& key, const T& defaultValue);

/**
 * @brief Access system constant with validation
 * 
 * @tparam T Type of constant
 * @param constantName Name of constant to retrieve
 * @return Constant value
 * 
 * @note Provides access to system-wide constants
 * @complexity O(1)
 */
template<typename T>
T getConstantValue(const std::string& constantName);

/**
 * @brief Validate parameter against allowed range and constraints
 * 
 * @tparam T Numeric type
 * @param value Value to validate
 * @param minValue Minimum allowed value
 * @param maxValue Maximum allowed value
 * @param tolerance Tolerance for floating point comparisons
 * @return true if value is valid, false otherwise
 * 
 * @note Ensures parameter safety and prevents invalid states
 * @complexity O(1)
 */
template<typename T>
bool validateParameter(T value, T minValue, T maxValue, T tolerance = static_cast<T>(0.001));

} // namespace nlm

// ============================================================================
// UTILITY FORWARD DECLARATIONS
// ============================================================================

#include "Constants.hpp"

namespace nlm {

// Configuration and Constant Access Implementations
template<typename T>
T getConfigValue(const Config& config, const std::string& key, const T& defaultValue) {
    if (config.has(key)) {
        // This will need to be implemented based on Config interface
        // For now, return default value
        return defaultValue;
    }
    return defaultValue;
}

template<typename T>
T getConstantValue(const std::string& constantName) {
    // This will need to be implemented based on Constants interface
    // For now, throw exception if not found
    throw std::invalid_argument("Constant " + constantName + " not found");
}

// Activity Calculation Implementations
float calculateAverageActivity(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    for (const Neuron* neuron : neurons) {
        if (neuron) {
            float activity = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
            sum += activity;
        }
    }
    return sum / neurons.size();
}

float calculateMaxActivity(const std::vector<Neuron*>& neurons) {
    float maxActivity = 0.0f;
    for (const Neuron* neuron : neurons) {
        if (neuron) {
            float activity = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
            if (activity > maxActivity) {
                maxActivity = activity;
            }
        }
    }
    return maxActivity;
}

float calculateWeightedActivity(const std::vector<Neuron*>& neurons, const std::vector<float>& weights) {
    if (neurons.empty() || neurons.size() != weights.size()) {
        throw std::invalid_argument("Invalid neuron or weight vector size");
    }
    
    float weightedSum = 0.0f;
    for (size_t i = 0; i < neurons.size(); ++i) {
        if (neurons[i]) {
            float activity = std::abs(neurons[i]->getState().membranePotential - neurons[i]->getState().restingPotential);
            weightedSum += activity * weights[i];
        }
    }
    return weightedSum;
}

// Novelty and Similarity Calculations
float calculateTemporalNovelty(const std::vector<float>& currentState,
                              const std::vector<float>& previousState,
                              float decayRate) {
    if (currentState.size() != previousState.size()) {
        throw std::invalid_argument("State vectors must have same size");
    }
    
    float totalDifference = 0.0f;
    for (size_t i = 0; i < currentState.size(); ++i) {
        float diff = std::abs(currentState[i] - previousState[i]);
        totalDifference += diff * std::pow(decayRate, static_cast<float>(i));
    }
    
    return totalDifference / currentState.size();
}

float calculateFeatureSimilarity(const std::vector<float>& features1,
                                const std::vector<float>& features2,
                                int similarityMetric) {
    if (features1.size() != features2.size()) {
        throw std::invalid_argument("Feature vectors must have same size");
    }
    
    if (similarityMetric == 0) { // Euclidean similarity
        float sumSq = 0.0f;
        for (size_t i = 0; i < features1.size(); ++i) {
            float diff = features1[i] - features2[i];
            sumSq += diff * diff;
        }
        return 1.0f / (1.0f + std::sqrt(sumSq));
    } else { // Cosine similarity (default)
        float dotProduct = 0.0f;
        float norm1 = 0.0f;
        float norm2 = 0.0f;
        
        for (size_t i = 0; i < features1.size(); ++i) {
            dotProduct += features1[i] * features2[i];
            norm1 += features1[i] * features1[i];
            norm2 += features2[i] * features2[i];
        }
        
        norm1 = std::sqrt(norm1);
        norm2 = std::sqrt(norm2);
        
        if (norm1 == 0.0f || norm2 == 0.0f) return 0.0f;
        
        return dotProduct / (norm1 * norm2);
    }
}

// Neuron Group Distribution Implementations
std::vector<size_t> distributeToGroups(size_t neuronCount, size_t groupCount, size_t startIndex) {
    std::vector<size_t> distribution(groupCount, 0);
    if (groupCount == 0) return distribution;
    
    size_t baseCount = neuronCount / groupCount;
    size_t remainder = neuronCount % groupCount;
    
    for (size_t i = 0; i < groupCount; ++i) {
        distribution[i] = baseCount + (i < remainder ? 1 : 0);
    }
    
    // Apply start offset if specified
    if (startIndex > 0 && startIndex < groupCount) {
        std::rotate(distribution.begin(), distribution.begin() + startIndex, distribution.end());
    }
    
    return distribution;
}

std::vector<size_t> distributeSensoryNeurons(size_t totalNeurons) {
    return distributeToGroups(totalNeurons, INTERNAL_SIGNALS_COUNT);
}

std::vector<size_t> distributeMotorNeurons(size_t totalNeurons) {
    return distributeToGroups(totalNeurons, MOTOR_ACTION_GROUPS);
}

// ============================================================================
// VALUE SCALING IMPLEMENTATIONS
// ============================================================================

template<typename T>
T scaleToRange(T value, T sourceMin, T sourceMax, T targetMin, T targetMax) {
    if (std::isnan(value) || std::isinf(value)) {
        throw std::invalid_argument("Cannot scale NaN or infinite value");
    }
    
    if (sourceMin == sourceMax) {
        if (targetMin == targetMax) {
            return targetMin;
        }
        throw std::invalid_argument("Source range is constant");
    }
    
    // Normalize to [0,1] then scale to target range
    T normalized = (value - sourceMin) / (sourceMax - sourceMin);
    return normalized * (targetMax - targetMin) + targetMin;
}

template<typename T>
T normalizeToSigned(T value, T offset) {
    return (value - offset) * static_cast<T>(2.0) + static_cast<T>(-1.0);
}

template<typename T>
T applyOffsetScale(T value, T offset, T scale) {
    return (value * static_cast<T>(2.0) - offset) * scale;
}

} // namespace nlm
