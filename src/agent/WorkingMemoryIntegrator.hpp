#pragma once

#include "../Memory/NeuralWorkingMemory.hpp"
#include "../Sensory/SensoryPercept.hpp"

namespace nlm {

class WorkingMemoryIntegrator {
public:
    static void integrateWithSensoryInput(
        Brain* brain, 
        const SensoryPercept& percept,
        uint64_t step,
        double timestep) {
        
        if (!brain) return;
        
        // Get working memory system
        auto* wm = brain->getWorkingMemory();
        if (!wm) {
            NLM_LOG_WARN("Working memory not available - integration failed");
            return;
        }
        
        // Create working memory trace from sensory input
        WorkingMemoryTrace trace;
        trace.step = step;
        trace.timestep = timestep;
        trace.priority = 1.0f; // Default priority
        
        // Integrate sensory features
        integrateVisionTrace(wm, percept, trace);
        integrateTouchTrace(wm, percept, trace);
        integrateInternalTrace(wm, percept, trace);
        integrateProprioceptionTrace(wm, percept, trace);
        
        // Add novelty as trace priority
        if (auto* agentBrain = dynamic_cast<AgentBrain*>(brain)) {
            float novelty = agentBrain->getNoveltyLevel();
            trace.priority += novelty * 0.5f;
        }
        
        // Store trace in working memory
        wm->addActiveTrace(trace);
        
        // Update attention based on trace salience
        updateAttentionBasedOnTraces(wm, brain);
        
        NLM_LOG_DEBUG("Working memory integrated: " + std::to_string(wm->getActiveTraces()) + " traces");
    }
    
private:
    static void integrateVisionTrace(NeuralWorkingMemory* wm, 
                                     const SensoryPercept& percept,
                                     WorkingMemoryTrace& trace) {
        if (!wm) return;
        
        const auto& vision = percept.getVision();
        if (vision.empty()) return;
        
        // Create pattern representation of vision input
        std::vector<float> pattern;
        pattern.reserve(vision.size());
        
        for (size_t i = 0; i < vision.size(); ++i) {
            // Normalize and quantize vision values
            float normalized = std::clamp(vision[i], 0.0f, 1.0f);
            
            // Create feature vector
            FeatureVector feature;
            feature.featureType = FeatureType::Visual;
            feature.featureValue = normalized;
            feature.spatialCoordinates = Vector2D(i % 16, i / 16); // Assuming 16x16 vision
            feature.salience = calculateVisualSalience(normalized, i);
            
            trace.features.push_back(feature);
        }
        
        // Store pattern in working memory
        wm->storePattern(pattern, TraceType::Visual, calculatePatternPriority(trace));
    }
    
    static void integrateTouchTrace(NeuralWorkingMemory* wm,
                                    const SensoryPercept& percept,
                                    WorkingMemoryTrace& trace) {
        if (!wm) return;
        
        const auto& touch = percept.getTouch();
        if (touch.empty()) return;
        
        // Create tactile pattern representation
        std::vector<float> pattern;
        pattern.reserve(touch.size());
        
        for (size_t i = 0; i < touch.size(); ++i) {
            float normalized = std::clamp(touch[i], 0.0f, 1.0f);
            
            FeatureVector feature;
            feature.featureType = FeatureType::Tactile;
            feature.featureValue = normalized;
            feature.spatialCoordinates = Vector2D(i, 0); // Assuming linear arrangement
            feature.salience = calculateTactileSalience(normalized, i);
            
            trace.features.push_back(feature);
        }
        
        wm->storePattern(pattern, TraceType::Tactile, calculatePatternPriority(trace));
    }
    
    static void integrateInternalTrace(NeuralWorkingMemory* wm,
                                       const SensoryPercept& percept,
                                       WorkingMemoryTrace& trace) {
        if (!wm) return;
        
        const auto& internal = percept.getInternal();
        if (internal.empty()) return;
        
        // Internal signals: hunger, thirst, arousal, etc.
        std::vector<float> pattern;
        pattern.reserve(internal.size());
        
        for (size_t i = 0; i < internal.size(); ++i) {
            // Transform internal values to meaningful features
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
    
    static void integrateProprioceptionTrace(NeuralWorkingMemory* wm,
                                             const SensoryPercept& percept,
                                             WorkingMemoryTrace& trace) {
        if (!wm) return;
        
        const auto& proprio = percept.getProprioception();
        if (proprio.empty()) return;
        
        // Proprioceptive information: body position, movement
        std::vector<float> pattern;
        pattern.reserve(proprio.size());
        
        for (size_t i = 0; i < proprio.size(); ++i) {
            float normalized = std::clamp(proprio[i], -1.0f, 1.0f);
            
            FeatureVector feature;
            feature.featureType = FeatureType::Proprioceptive;
            feature.featureValue = normalized;
            feature.spatialCoordinates = Vector2D(0, 0); // Body-centered coordinates
            feature.salience = calculateProprioceptiveSalience(normalized, i);
            
            trace.features.push_back(feature);
        }
        
        wm->storePattern(pattern, TraceType::Proprioceptive, calculatePatternPriority(trace));
    }
    
    static void updateAttentionBasedOnTraces(NeuralWorkingMemory* wm, Brain* brain) {
        if (!wm || !brain) return;
        
        // Get attentional selection system
        auto* attention = brain->getAttention();
        if (!attention) {
            NLM_LOG_WARN("Attention system not available");
            return;
        }
        
        // Analyze trace salience and update attention
        std::vector<AttentionFocus> focuses;
        
        for (const auto& trace : wm->getActiveTraces()) {
            float totalSalience = 0.0f;
            for (const auto& feature : trace.features) {
                totalSalience += feature.salience;
            }
            
            if (totalSalience > 0.1f) { // Threshold for attention
                AttentionFocus focus;
                focus.targetType = determineTargetType(trace);
                focus.priority = totalSalience;
                focus.duration = calculateFocusDuration(totalSalience);
                focus.features = trace.features;
                
                focuses.push_back(focus);
            }
        }
        
        // Apply attentional selection
        attention->updateFocus(focuses);
        
        // Limit number of attended traces
        wm->applyAttentionLimit(static_cast<size_t>(focuses.size()));
    }
    
    static float calculateVisualSalience(float intensity, size_t position) {
        // Higher salience for high contrast, central vision
        float contrastSalience = intensity;
        float centralBias = 1.0f - std::abs(16.0f - (position % 16)) / 8.0f;
        return contrastSalience * centralBias * 0.8f + 0.2f;
    }
    
    static float calculateTactileSalience(float intensity, size_t position) {
        // Higher salience for strong, sudden touches
        float intensitySalience = intensity;
        float noveltyBias = std::min(position, size_t(5)) / 5.0f; // Higher salience for first touches
        return intensitySalience * noveltyBias * 0.9f + 0.1f;
    }
    
    static float calculateInternalSalience(float value, size_t featureIndex) {
        // Higher salience for extreme internal states (hunger, arousal)
        float magnitude = std::abs(value);
        float urgencyBias = magnitude > 0.5f ? 1.0f : magnitude / 0.5f;
        return magnitude * urgencyBias * 0.7f + 0.3f;
    }
    
    static float calculateProprioceptiveSalience(float value, size_t position) {
        // Higher salience for unexpected body states
        float magnitude = std::abs(value);
        float noveltyBias = std::min(position, size_t(3)) / 3.0f; // Higher salience for new positions
        return magnitude * noveltyBias * 0.6f + 0.4f;
    }
    
    static float calculatePatternPriority(const WorkingMemoryTrace& trace) {
        float priority = 0.0f;
        for (const auto& feature : trace.features) {
            priority += feature.salience;
        }
        return priority / std::max<size_t>(trace.features.size(), 1);
    }
    
    static float transformInternalSignal(float value, size_t featureIndex) {
        // Transform internal signal values to meaningful range
        switch (featureIndex) {
            case 0: return std::clamp(value, 0.0f, 1.0f); // Hunger
            case 1: return std::clamp(1.0f - value, 0.0f, 1.0f); // Thirst
            case 2: return std::clamp(value, 0.0f, 1.0f); // Arousal
            case 3: return std::clamp(value, 0.0f, 1.0f); // Fatigue
            default: return 0.5f;
        }
    }
    
    static FeatureType determineTargetType(const WorkingMemoryTrace& trace) {
        // Determine what the trace represents for attentional targeting
        if (trace.features.empty()) return FeatureType::Visual;
        
        auto firstFeature = trace.features[0].featureType;
        if (firstFeature == FeatureType::Visual) return FeatureType::Visual;
        if (firstFeature == FeatureType::Tactile) return FeatureType::Tactile;
        if (firstFeature == FeatureType::Internal) return FeatureType::Internal;
        if (firstFeature == FeatureType::Proprioceptive) return FeatureType::Proprioceptive;
        
        return FeatureType::Visual; // Default
    }
    
    static float calculateFocusDuration(float salience) {
        // Longer attention for high salience features
        return 0.1f + salience * 0.9f; // 0.1s to 1.0s
    }
};

} // namespace nlm