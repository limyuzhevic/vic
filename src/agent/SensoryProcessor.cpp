#include "SensoryProcessor.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

SensoryProcessor::SensoryProcessor(const SensoryConfig& config) 
    : noveltyLevel_(0.0f), dopamineModulation_(1.0f), config_(config) {
    // Pre-allocate memory for signals
    visionSignal_.reserve(config.visionNeurons_);
    touchSignal_.reserve(config.touchNeurons_);
    internalSignal_.reserve(config.internalNeurons_);
    proprioceptionSignal_.reserve(config.proprioceptionNeurons_);
    
    visionHistory_.reserve(config.visionNeurons_);
    touchHistory_.reserve(config.touchNeurons_);
    internalHistory_.reserve(config.internalNeurons_);
    proprioceptionHistory_.reserve(config.proprioceptionNeurons_);
    
    NLM_LOG_DEBUG("SensoryProcessor initialized with vision neurons: " +
                 std::to_string(config.visionNeurons_));
}

SensoryProcessor::~SensoryProcessor() {
    // Clean up
    visionSignal_.clear();
    touchSignal_.clear();
    internalSignal_.clear();
    proprioceptionSignal_.clear();
    
    visionHistory_.clear();
    touchHistory_.clear();
    internalHistory_.clear();
    proprioceptionHistory_.clear();
}

void SensoryProcessor::processSensoryInput(const SensoryPercept& percept) {
    // Store original inputs
    originalVision_ = percept.getVision();
    originalTouch_ = percept.getTouch();
    originalInternal_ = percept.getInternal();
    originalProprioception_ = percept.getProprioception();
    
    // Process each modality
    if (!originalVision_.empty()) {
        processVision(originalVision_);
    }
    
    if (!originalTouch_.empty()) {
        processTouch(originalTouch_);
    }
    
    if (!originalInternal_.empty()) {
        processInternal(originalInternal_);
    }
    
    if (!originalProprioception_.empty()) {
        processProprioception(originalProprioception_);
    }
    
    // Apply cross-modal integration
    if (config_.enableCrossModalIntegration_) {
        integrateModalities();
    }
    
    // Apply neuromodulatory effects
    applyNeuromodulation(dopamineModulation_, noveltyLevel_);
    
    // Update history for next time step
    updateSensoryHistory();
    
    // Compute novelty level
    computeNovelty(originalVision_, visionHistory_);
}

std::vector<float> SensoryProcessor::getAllProcessedSignals() const {
    std::vector<float> allSignals;
    allSignals.reserve(visionSignal_.size() + touchSignal_.size() + 
                      internalSignal_.size() + proprioceptionSignal_.size());
    
    allSignals.insert(allSignals.end(), visionSignal_.begin(), visionSignal_.end());
    allSignals.insert(allSignals.end(), touchSignal_.begin(), touchSignal_.end());
    allSignals.insert(allSignals.end(), internalSignal_.begin(), internalSignal_.end());
    allSignals.insert(allSignals.end(), proprioceptionSignal_.begin(), proprioceptionSignal_.end());
    
    return allSignals;
}

void SensoryProcessor::applyNeuromodulation(float dopamineLevel, float noveltyLevel) {
    dopamineModulation_ = dopamineLevel;
    noveltyLevel_ = noveltyLevel;
    
    // Apply neuromodulatory effects to signals
    if (dopamineLevel > 0.0f) {
        // Enhance processing with positive dopamine
        float enhancement = 1.0f + dopamineLevel * 0.5f;
        
        // Scale vision signal
        for (float& value : visionSignal_) {
            value = std::min(1.0f, value * enhancement);
        }
        
        // Scale touch signal
        for (float& value : touchSignal_) {
            value = std::min(1.0f, value * enhancement);
        }
    }
    
    if (noveltyLevel > config_.noveltyThreshold_) {
        // Novel stimulus leads to increased attention/modulation
        float attentionBoost = (noveltyLevel - config_.noveltyThreshold_) * 2.0f;
        
        // Boost internal signal for learning
        for (float& value : internalSignal_) {
            value = std::min(1.0f, value + attentionBoost * 0.1f);
        }
    }
}

void SensoryProcessor::updateSensoryHistory() {
    // Update history buffers
    visionHistory_ = visionSignal_;
    touchHistory_ = touchSignal_;
    internalHistory_ = internalSignal_;
    proprioceptionHistory_ = proprioceptionSignal_;
}

void SensoryProcessor::reset() {
    // Clear all signals
    originalVision_.clear();
    originalTouch_.clear();
    originalInternal_.clear();
    originalProprioception_.clear();
    
    visionSignal_.clear();
    touchSignal_.clear();
    internalSignal_.clear();
    proprioceptionSignal_.clear();
    
    visionHistory_.clear();
    touchHistory_.clear();
    internalHistory_.clear();
    proprioceptionHistory_.clear();
    
    // Reset neuromodulatory state
    noveltyLevel_ = 0.0f;
    dopamineModulation_ = 1.0f;
    
    // Reset statistics
    stats_ = ProcessingStats();
    
    NLM_LOG_DEBUG("SensoryProcessor reset");
}

bool SensoryProcessor::isValid() const {
    // Check that signal sizes match expected neuron counts
    if (visionSignal_.size() != config_.visionNeurons_) return false;
    if (touchSignal_.size() != config_.touchNeurons_) return false;
    if (internalSignal_.size() != config_.internalNeurons_) return false;
    if (proprioceptionSignal_.size() != config_.proprioceptionNeurons_) return false;
    
    // Check that values are within reasonable range
    for (float value : visionSignal_) {
        if (value < -1.0f || value > 1.0f) return false;
    }
    
    return true;
}

void SensoryProcessor::processVision(const std::vector<float>& input) {
    visionSignal_.resize(input.size(), 0.0f);
    
    // Copy input
    for (size_t i = 0; i < input.size() && i < visionSignal_.size(); ++i) {
        visionSignal_[i] = input[i];
    }
    
    // Apply processing strategy
    switch (config_.visionProcessingStrategy_) {
        case SensoryConfig::ProcessingStrategy::Linear:
            for (float& value : visionSignal_) {
                value = processLinear(value, config_.visionScaleFactor_, 0.0f);
            }
            break;
        case SensoryConfig::ProcessingStrategy::Logarithmic:
            for (float& value : visionSignal_) {
                value = processLogarithmic(value, config_.visionScaleFactor_);
            }
            break;
        case SensoryConfig::ProcessingStrategy::Sigmoid:
            for (float& value : visionSignal_) {
                value = processSigmoid(value, config_.visionScaleFactor_, 0.5f);
            }
            break;
        case SensoryConfig::ProcessingStrategy::Adaptive:
            for (float& value : visionSignal_) {
                value = processAdaptive(value, config_.visionScaleFactor_, stats_.visionMean_);
            }
            break;
        default:
            break;
    }
    
    // Apply filtering
    if (config_.enableVisionFiltering_) {
        applyFiltering(visionSignal_, config_.visionSmoothing_);
    }
    
    // Apply normalization
    if (config_.enableSensoryNormalization_) {
        applyNormalization(visionSignal_);
    }
    
    // Apply compression
    if (config_.enableSensoryCompression_) {
        applyCompression(visionSignal_);
    }
    
    // Apply temporal filtering
    if (config_.enableTemporalFiltering_) {
        applyTemporalFiltering(visionSignal_, 1.0f - config_.visionSmoothing_);
    }
    
    // Update statistics for adaptive processing
    if (config_.visionProcessingStrategy_ == SensoryConfig::ProcessingStrategy::Adaptive) {
        updateVisionStats();
    }
}

void SensoryProcessor::processTouch(const std::vector<float>& input) {
    touchSignal_.resize(input.size(), 0.0f);
    
    for (size_t i = 0; i < input.size() && i < touchSignal_.size(); ++i) {
        touchSignal_[i] = input[i];
    }
    
    // Apply processing strategy
    switch (config_.touchProcessingStrategy_) {
        case SensoryConfig::ProcessingStrategy::Linear:
            for (float& value : touchSignal_) {
                value = processLinear(value, config_.touchScaleFactor_, 0.0f);
            }
            break;
        case SensoryConfig::ProcessingStrategy::Logarithmic:
            for (float& value : touchSignal_) {
                value = processLogarithmic(value, config_.touchScaleFactor_);
            }
            break;
        case SensoryConfig::ProcessingStrategy::Sigmoid:
            for (float& value : touchSignal_) {
                value = processSigmoid(value, config_.touchScaleFactor_, 0.5f);
            }
            break;
        case SensoryConfig::ProcessingStrategy::Adaptive:
            for (float& value : touchSignal_) {
                value = processAdaptive(value, config_.touchScaleFactor_, stats_.touchMean_);
            }
            break;
        default:
            break;
    }
    
    // Apply filtering
    if (config_.enableSensoryFiltering_) {
        applyFiltering(touchSignal_, 1.0f - config_.touchNoiseLevel_);
    }
    
    // Apply normalization
    if (config_.enableSensoryNormalization_) {
        applyNormalization(touchSignal_);
    }
}

void SensoryProcessor::processInternal(const std::vector<float>& input) {
    internalSignal_.resize(input.size(), 0.0f);
    
    for (size_t i = 0; i < input.size() && i < internalSignal_.size(); ++i) {
        internalSignal_[i] = input[i];
    }
    
    // Apply processing strategy
    switch (config_.internalProcessingStrategy_) {
        case SensoryConfig::ProcessingStrategy::Linear:
            for (float& value : internalSignal_) {
                value = processLinear(value, config_.internalScaleFactor_, -1.0f);
            }
            break;
        case SensoryConfig::ProcessingStrategy::Logarithmic:
            for (float& value : internalSignal_) {
                value = processLogarithmic(value, config_.internalScaleFactor_);
            }
            break;
        case SensoryConfig::ProcessingStrategy::Sigmoid:
            for (float& value : internalSignal_) {
                value = processSigmoid(value, config_.internalScaleFactor_, 0.0f);
            }
            break;
        case SensoryConfig::ProcessingStrategy::Adaptive:
            for (float& value : internalSignal_) {
                value = processAdaptive(value, config_.internalScaleFactor_, stats_.internalMean_);
            }
            break;
        default:
            break;
    }
    
    // Apply smoothing
    if (config_.enableSensoryFiltering_) {
        applyFiltering(internalSignal_, config_.internalSmoothing_);
    }
    
    // Apply normalization
    if (config_.enableSensoryNormalization_) {
        applyNormalization(internalSignal_);
    }
}

void SensoryProcessor::processProprioception(const std::vector<float>& input) {
    proprioceptionSignal_.resize(input.size(), 0.0f);
    
    for (size_t i = 0; i < input.size() && i < proprioceptionSignal_.size(); ++i) {
        proprioceptionSignal_[i] = input[i];
    }
    
    // Apply processing strategy
    switch (config_.proprioceptionProcessingStrategy_) {
        case SensoryConfig::ProcessingStrategy::Linear:
            for (float& value : proprioceptionSignal_) {
                value = processLinear(value, config_.proprioceptionScaleFactor_, 0.0f);
            }
            break;
        case SensoryConfig::ProcessingStrategy::Logarithmic:
            for (float& value : proprioceptionSignal_) {
                value = processLogarithmic(value, config_.proprioceptionScaleFactor_);
            }
            break;
        case SensoryConfig::ProcessingStrategy::Sigmoid:
            for (float& value : proprioceptionSignal_) {
                value = processSigmoid(value, config_.proprioceptionScaleFactor_, 0.5f);
            }
            break;
        case SensoryConfig::ProcessingStrategy::Adaptive:
            for (float& value : proprioceptionSignal_) {
                value = processAdaptive(value, config_.proprioceptionScaleFactor_, stats_.internalMean_);
            }
            break;
        default:
            break;
    }
    
    // Apply normalization
    if (config_.enableSensoryNormalization_) {
        applyNormalization(proprioceptionSignal_);
    }
}

float SensoryProcessor::processLinear(float value, float scale, float offset) const {
    return value * scale + offset;
}

float SensoryProcessor::processLogarithmic(float value, float scale) const {
    // Convert to log scale for dynamic range compression
    if (value <= 0.0f) return 0.0f;
    return scale * std::log(1.0f + value);
}

float SensoryProcessor::processSigmoid(float value, float scale, float midpoint) const {
    // Sigmoid function for bounded output
    float x = (value - midpoint) * scale;
    return 1.0f / (1.0f + std::exp(-x));
}

float SensoryProcessor::processAdaptive(float value, float scale, float& history) const {
    // Simple adaptive processing based on running mean
    float targetMean = history * 0.5f + value * 0.5f;
    return processLinear(value, scale * (1.0f + targetMean), 0.0f);
}

void SensoryProcessor::applyFiltering(std::vector<float>& signal, float smoothing) const {
    if (smoothing <= 0.0f || signal.empty()) return;
    
    // Simple exponential moving average
    float alpha = 1.0f - smoothing;
    float sum = 0.0f;
    
    for (float& value : signal) {
        sum += value;
    }
    
    float avg = sum / signal.size();
    
    for (float& value : signal) {
        value = alpha * value + (1.0f - alpha) * avg;
    }
}

void SensoryProcessor::applyNormalization(std::vector<float>& signal) const {
    if (signal.empty()) return;
    
    // Min-max normalization to [0, 1]
    float minVal = *std::min_element(signal.begin(), signal.end());
    float maxVal = *std::max_element(signal.begin(), signal.end());
    
    if (maxVal - minVal > 0.0f) {
        for (float& value : signal) {
            value = (value - minVal) / (maxVal - minVal);
        }
    } else {
        // All values are the same
        for (float& value : signal) {
            value = 0.5f;
        }
    }
}

void SensoryProcessor::applyCompression(std::vector<float>& signal) const {
    // Dynamic range compression
    for (float& value : signal) {
        if (value > 0.8f) {
            value = 0.8f + (value - 0.8f) * 0.5f;
        } else if (value < -0.8f) {
            value = -0.8f + (value + 0.8f) * 0.5f;
        }
    }
}

void SensoryProcessor::applyTemporalFiltering(std::vector<float>& signal, float decay) const {
    if (decay <= 0.0f || signal.empty()) return;
    
    // Simple decay towards zero
    for (float& value : signal) {
        value *= decay;
    }
}

void SensoryProcessor::integrateModalities() {
    if (visionSignal_.empty() || touchSignal_.empty()) return;
    
    // Simple weighted sum of modalities
    size_t minSize = std::min({
        visionSignal_.size(), 
        touchSignal_.size(), 
        internalSignal_.size(),
        proprioceptionSignal_.size()
    });
    
    if (minSize == 0) return;
    
    // Integrate vision (highest weight)
    for (size_t i = 0; i < minSize; ++i) {
        float integrated = visionSignal_[i] * config_.integrationWeightVision_;
        
        if (i < touchSignal_.size()) {
            integrated += touchSignal_[i] * config_.integrationWeightTouch_;
        }
        
        if (i < internalSignal_.size()) {
            integrated += internalSignal_[i] * config_.integrationWeightInternal_;
        }
        
        if (i < proprioceptionSignal_.size()) {
            integrated += proprioceptionSignal_[i] * config_.integrationWeightProprioception_;
        }
        
        // Use integrated value for vision (main modality)
        visionSignal_[i] = integrated;
    }
}

void SensoryProcessor::computeNovelty(const std::vector<float>& current, 
                                     const std::vector<float>& previous) {
    if (current.size() != previous.size() || current.empty()) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    // Compute mean absolute difference
    float totalDiff = 0.0f;
    for (size_t i = 0; i < current.size(); ++i) {
        totalDiff += std::abs(current[i] - previous[i]);
    }
    
    float meanDiff = totalDiff / current.size();
    
    // Normalize to [0, 1] range
    noveltyLevel_ = std::min(1.0f, meanDiff * 10.0f); // Scale factor of 10 for sensitivity
    
    // Apply decay
    noveltyLevel_ *= config_.noveltyDecayRate_;
}

void SensoryProcessor::updateVisionStats() {
    // Update running statistics for adaptive processing
    if (visionSignal_.empty()) return;
    
    float sum = std::accumulate(visionSignal_.begin(), visionSignal_.end(), 0.0f);
    stats_.visionMean_ = sum / visionSignal_.size();
    
    // Update vision std (simplified calculation)
    float varianceSum = 0.0f;
    for (float value : visionSignal_) {
        float diff = value - stats_.visionMean_;
        varianceSum += diff * diff;
    }
    
    stats_.visionStd_ = std::sqrt(varianceSum / visionSignal_.size());
    
    stats_.updateCount_++;
}

} // namespace nlm