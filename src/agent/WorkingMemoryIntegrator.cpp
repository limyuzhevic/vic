#include "WorkingMemoryIntegrator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Implementation of WorkingMemoryIntegrator methods
// These methods integrate sensory input with working memory

// Integrate vision sensory input with working memory
trace.features.push_back(feature);
    }
    
    wm->storePattern(pattern, TraceType::Visual, calculatePatternPriority(trace));
}

// Integrate tactile sensory input with working memory
static void integrateTouchTrace(NeuralWorkingMemory* wm,
                                const SensoryPercept& percept,
                                WorkingMemoryTrace& trace) {
    if (!wm) return;
    
    const auto& touch = percept.getTouch();
    if (touch.empty()) return;
    
    std::vector<float> pattern;
    pattern.reserve(touch.size());
    
    for (size_t i = 0; i < touch.size(); ++i) {
        float normalized = std::clamp(touch[i], 0.0f, 1.0f);
        
        FeatureVector feature;
        feature.featureType = FeatureType::Tactile;
        feature.featureValue = normalized;
        feature.spatialCoordinates = Vector2D(i, 0);
        feature.salience = calculateTactileSalience(normalized, i);
        
        trace.features.push_back(feature);
    }
    
    wm->storePattern(pattern, TraceType::Tactile, calculatePatternPriority(trace));
}

// Integrate internal sensory signals with working memory
static void integrateInternalTrace(NeuralWorkingMemory* wm,
                                   const SensoryPercept& percept,
                                   WorkingMemoryTrace& trace) {
    if (!wm) return;
    
    const auto& internal = percept.getInternal();
    if (internal.empty()) return;
    
    std::vector<float> pattern;
    pattern.reserve(internal.size());
    
    for (size_t i = 0; i < internal.size(); ++i) {
        float transformed = transformInternalSignal(internal[i], i);
        
        FeatureVector feature;
        feature.featureType = FeatureType::Internal;
        feature.featureValue = transformed;
        feature.spatialCoordinates = Vector2D(0, i);
        feature.salience = calculateInternalSalience(transformed, i);
        
        trace.features.push_back(feature);
    }
    
    wm->storePattern(pattern, TraceType::Internal, calculatePatternPriority(trace));
}

// Integrate proprioceptive sensory input with working memory
static void integrateProprioceptionTrace(NeuralWorkingMemory* wm,
                                         const SensoryPercept& percept,
                                         WorkingMemoryTrace& trace) {
    if (!wm) return;
    
    const auto& proprio = percept.getProprioception();
    if (proprio.empty()) return;
    
    std::vector<float> pattern;
    pattern.reserve(proprio.size());
    
    for (size_t i = 0; i < proprio.size(); ++i) {
        float normalized = std::clamp(proprio[i], -1.0f, 1.0f);
        
        FeatureVector feature;
        feature.featureType = FeatureType::Proprioceptive;
        feature.featureValue = normalized;
        feature.spatialCoordinates = Vector2D(0, 0);
        feature.salience = calculateProprioceptiveSalience(normalized, i);
        
        trace.features.push_back(feature);
    }
    
    wm->storePattern(pattern, TraceType::Proprioceptive, calculatePatternPriority(trace));
}

// Update attentional selection based on working memory trace salience
static void updateAttentionBasedOnTraces(NeuralWorkingMemory* wm, Brain* brain) {
    if (!wm || !brain) return;
    
    auto* attention = brain->getAttention();
    if (!attention) {
        NLM_LOG_WARN("Attention system not available for working memory integration");
        return;
    }
    
    std::vector<AttentionFocus> focuses;
    
    for (const auto& trace : wm->getActiveTraces()) {
        float totalSalience = 0.0f;
        for (const auto& feature : trace.features) {
            totalSalience += feature.salience;
        }
        
        if (totalSalience > 0.1f) {
            AttentionFocus focus;
            focus.targetType = determineTargetType(trace);
            focus.priority = totalSalience;
            focus.duration = calculateFocusDuration(totalSalience);
            focus.features = trace.features;
            
            focuses.push_back(focus);
        }
    }
    
    attention->updateFocus(focuses);
    wm->applyAttentionLimit(static_cast<size_t>(focuses.size()));
}

// Calculate salience for visual features based on intensity and position
static float calculateVisualSalience(float intensity, size_t position) {
    float contrastSalience = intensity;
    float centralBias = 1.0f - std::abs(16.0f - (position % 16)) / 8.0f;
    return contrastSalience * centralBias * 0.8f + 0.2f;
}

// Calculate salience for tactile features based on intensity and novelty
static float calculateTactileSalience(float intensity, size_t position) {
    float intensitySalience = intensity;
    float noveltyBias = std::min(position, size_t(5)) / 5.0f;
    return intensitySalience * noveltyBias * 0.9f + 0.1f;
}

// Calculate salience for internal features based on magnitude and urgency
static float calculateInternalSalience(float value, size_t featureIndex) {
    float magnitude = std::abs(value);
    float urgencyBias = magnitude > 0.5f ? 1.0f : magnitude / 0.5f;
    return magnitude * urgencyBias * 0.7f + 0.3f;
}

// Calculate salience for proprioceptive features based on magnitude and novelty
static float calculateProprioceptiveSalience(float value, size_t position) {
    float magnitude = std::abs(value);
    float noveltyBias = std::min(position, size_t(3)) / 3.0f;
    return magnitude * noveltyBias * 0.6f + 0.4f;
}

// Calculate priority score for a working memory trace
static float calculatePatternPriority(const WorkingMemoryTrace& trace) {
    if (trace.features.empty()) return 0.0f;
    
    float priority = 0.0f;
    for (const auto& feature : trace.features) {
        priority += feature.salience;
    }
    return priority / static_cast<float>(trace.features.size());
}

// Transform internal signal values to meaningful ranges
static float transformInternalSignal(float value, size_t featureIndex) {
    switch (featureIndex) {
        case 0: return std::clamp(value, 0.0f, 1.0f); // Hunger
        case 1: return std::clamp(1.0f - value, 0.0f, 1.0f); // Thirst
        case 2: return std::clamp(value, 0.0f, 1.0f); // Arousal
        case 3: return std::clamp(value, 0.0f, 1.0f); // Fatigue
        default: return 0.5f;
    }
}

// Determine target type for attentional focus based on trace features
static FeatureType determineTargetType(const WorkingMemoryTrace& trace) {
    if (trace.features.empty()) return FeatureType::Visual;
    
    auto firstFeature = trace.features[0].featureType;
    if (firstFeature == FeatureType::Visual) return FeatureType::Visual;
    if (firstFeature == FeatureType::Tactile) return FeatureType::Tactile;
    if (firstFeature == FeatureType::Internal) return FeatureType::Internal;
    if (firstFeature == FeatureType::Proprioceptive) return FeatureType::Proprioceptive;
    
    return FeatureType::Visual;
}

// Calculate attention focus duration based on salience
static float calculateFocusDuration(float salience) {
    return 0.1f + salience * 0.9f; // 0.1s to 1.0s
}

} // namespace nlm