#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <vector>
#include <array>
#include <cmath>

namespace nlm {

// Developmental milestone structure
struct DevelopmentalMilestone {
    const char* name;
    DevelopmentalStage stage;
    double age;  // Age when milestone is achieved
    float progressThreshold;  // Progress needed to reach milestone
    bool achieved;
    double achievedAge;  // Actual age when achieved
    
    DevelopmentalMilestone(const char* n, DevelopmentalStage s, double a, float p)
        : name(n), stage(s), age(a), progressThreshold(p), achieved(false), achievedAge(0.0) {}
};

// Developmental metrics for tracking brain development
struct DevelopmentalMetrics {
    // Structural metrics
    float synapticDensity;
    float connectivityDepth;
    float regionalSpecialization;
    
    // Functional metrics
    float workingMemoryCapacity;
    float planningComplexity;
    float cognitiveFlexibility;
    
    // Behavioral metrics
    float exploratoryBehavior;
    float habituationRate;
    float attentionSpan;
    
    // Integration metrics
    float memoryConsolidationRate;
    float skillAcquisitionRate;
    float experienceIntegrationEfficiency;
    
    // Aging metrics
    float neuralEfficiency;
    float metabolicRate;
    
    DevelopmentalMetrics()
        : synapticDensity(0.0f), connectivityDepth(0.0f), regionalSpecialization(0.0f)
        , workingMemoryCapacity(0.0f), planningComplexity(0.0f), cognitiveFlexibility(0.0f)
        , exploratoryBehavior(0.0f), habituationRate(0.0f), attentionSpan(0.0f)
        , memoryConsolidationRate(0.0f), skillAcquisitionRate(0.0f), experienceIntegrationEfficiency(0.0f)
        , neuralEfficiency(0.0f), metabolicRate(0.0f) {}
};

// Neural architecture specialization
struct NeuralRegionSpecialization {
    RegionId regionId;
    float sensoryProcessing;  // 0-1
    float motorControl;       // 0-1
    float internalRegulation;  // 0-1
    float interconnectivity;   // 0-1
    float myelinationLevel;   // 0-1
    
    NeuralRegionSpecialization(RegionId rid)
        : regionId(rid), sensoryProcessing(0.0f), motorControl(0.0f), 
          internalRegulation(0.0f), interconnectivity(0.0f), myelinationLevel(0.0f) {}
};

// Cognitive capability development
struct CognitiveCapabilities {
    float workingMemoryCapacity;      // Number of items
    float planningDepth;              // How many steps ahead
    float temporalAbstraction;        // Time scale of planning
    float causalReasoning;            // Ability to understand cause-effect
    float abstractThinking;           // Pattern recognition across domains
    float creativity;                 // Novel combination of ideas
    
    CognitiveCapabilities()
        : workingMemoryCapacity(1.0f), planningDepth(1.0f), temporalAbstraction(1.0f)
        , causalReasoning(0.0f), abstractThinking(0.0f), creativity(0.0f) {}
};

// Behavioral profile
struct BehavioralProfile {
    float curiosityDrive;         // Exploration motivation
    float noveltySeeking;          // Response to new stimuli
    float habituationRate;         // Speed of habituation
    float stressResponsiveness;   // Stress reactivity
    float socialTendencies;       // Social behavior (0-1)
    float aggressionLevel;        // Aggression (0-1)
    
    BehavioralProfile()
        : curiosityDrive(0.5f), noveltySeeking(0.5f), habituationRate(0.5f)
        , stressResponsiveness(0.5f), socialTendencies(0.5f), aggressionLevel(0.1f) {}
};

} // namespace nlm