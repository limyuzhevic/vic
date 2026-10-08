#pragma once

#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <type_traits>

namespace nlm {

/**
 * Macro for experiment phase definitions
 * Reduces code duplication in experiment phase setup
 */
#define EXPERIMENT_PHASE(name, phase_id, setup_func, run_func, cleanup_func) \
    struct Phase##name { \
        static constexpr const char* NAME = #name; \
        static constexpr int ID = phase_id; \
        static void setup(Brain* brain) { setup_func(brain); } \
        static float run(Brain* brain, uint64_t steps) { return run_func(brain, steps); } \
        static void cleanup(Brain* brain) { cleanup_func(brain); } \
    };

/**
 * Macro for memory system integration validation
 * Standardized memory checks across experiments
 */
#define VALIDATE_MEMORY_SYSTEMS(brain) \
    do { \
        if (!(brain)) return false; \
        if (!brain->getWorkingMemory()) { NLM_LOG_ERROR("Working memory not available"); return false; } \
        if (!brain->getEpisodicMemory()) { NLM_LOG_ERROR("Episodic memory not available"); return false; } \
        if (!brain->getDopamine()) { NLM_LOG_ERROR("Neuromodulation not available"); return false; } \
    } while(0)

/**
 * Macro for common experiment result structure
 * Standardizes experiment result data collection
 */
#define EXPERIMENT_RESULT_STRUCT(name, fields...) \
    struct name { \
        fields \
        name() { reset(); } \
        void reset() { /* Initialize all fields to default values */ } \
        std::string toCSV() const { /* Generate CSV string */ return ""; } \
    };

/**
 * Template function for common experiment patterns
 * Handles different brain types with type safety
 */
template<typename BrainType>
float runExperimentTemplate(
    BrainType* brain,
    uint64_t steps,
    float (*getReward)(BrainType*),
    std::function<void(uint64_t, float)> progressCallback = nullptr
) {
    static_assert(std::is_pointer_v<BrainType*>, "BrainType must be a pointer type");
    
    float totalReward = 0.0f;
    for (uint64_t step = 0; step < steps; ++step) {
        float reward = getReward(brain);
        totalReward += reward;
        
        if (progressCallback) {
            progressCallback(step, reward);
        }
        
        brain->step(step, step * 0.001);
    }
    
    return totalReward;
}

/**
 * Helper function for memory system benchmark
 * Common pattern for testing memory system performance
 */
float benchmarkMemorySystem(
    Brain* brain,
    uint64_t testSteps,
    const std::vector<float>& testPattern
) {
    if (!brain) return 0.0f;
    
    auto* wm = brain->getWorkingMemory();
    if (!wm) return 0.0f;
    
    float totalRecall = 0.0f;
    for (uint64_t step = 0; step < testSteps; ++step) {
        wm->store(testPattern, 1.0f);
        auto retrieved = wm->retrieve();
        float recall = RepresentationAnalyzer::computePatternSimilarity(testPattern, retrieved);
        totalRecall += recall;
        wm->update(0.001f);
    }
    
    return totalRecall / testSteps;
}

/**
 * Common reward calculation for memory-based tasks
 * Standardized approach to reward based on memory performance
 */
float calculateMemoryBasedReward(
    Brain* brain,
    const std::vector<float>& targetPattern,
    float baseReward = 1.0f
) {
    if (!brain) return 0.0f;
    
    float similarity = 0.0f;
    auto* wm = brain->getWorkingMemory();
    if (wm) {
        auto retrieved = wm->retrieve();
        similarity = RepresentationAnalyzer::computePatternSimilarity(targetPattern, retrieved);
    }
    
    return baseReward * similarity;
}

/**
 * Macro for experiment timing utilities
 * Standardized timing across experiments
 */
#define EXPERIMENT_TIMER \
    auto startTime = std::chrono::high_resolution_clock::now(); \
    auto endTime = std::chrono::high_resolution_clock::now(); \
    double duration = std::chrono::duration<double>(endTime - startTime).count()

/**
 * Common callback template for experiment progress
 * Standardized progress reporting
 */
template<typename Func>
void reportProgress(Func callback, uint64_t currentStep, uint64_t totalSteps, float progress) {
    if (callback) {
        callback(currentStep, totalSteps, progress);
    }
}

} // namespace nlm
