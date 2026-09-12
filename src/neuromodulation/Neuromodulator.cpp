#include "Neuromodulator.hpp"
#include "Dopamine.hpp"
#include "Acetylcholine.hpp"
#include "Norepinephrine.hpp"
#include "Serotonin.hpp"
#include <algorithm>

namespace nlm {

// Phase 2: Complete Neuromodulator Implementations

// Neuromodulator statistics collection
struct NeuromodulatorStatistics {
    float totalDopamine = 0.0f;
    float totalACh = 0.0f;
    float totalNE = 0.0f;
    float total5HT = 0.0f;
    float averagePlasticity = 0.0f;
    size_t updateCount = 0;
};

// Global neuromodulation statistics
static NeuromodulatorStatistics gStats;

// Helper function to update statistics
static void UpdateNeuromodulatorStats(const Dopamine& da, const Acetylcholine& ach, 
                                       const Norepinephrine& ne, const Serotonin& ht) {
    gStats.totalDopamine += da.getLevel();
    gStats.totalACh += ach.getLevel();
    gStats.totalNE += ne.getLevel();
    gStats.total5HT += ht.getLevel();
    
    float avgPlasticity = (da.getPlasticityFactor() + ach.getPlasticityFactor() + 
                          ne.getPlasticityFactor() + ht.getPlasticityFactor()) / 4.0f;
    gStats.averagePlasticity += avgPlasticity;
    gStats.updateCount++;
}

void ApplyAllNeuromodulators(const Dopamine& da, const Acetylcholine& ach,
                             const Norepinephrine& ne, const Serotonin& ht,
                             float& excitability, float& learningRate) {
    // Apply dopamine effects on excitability and learning rate
    float daExcitability = 0.0f;
    float daLearningRate = 0.0f;
    da.applyToExcitability(daExcitability);
    da.applyToLearningRate(daLearningRate);
    excitability += daExcitability;
    learningRate = std::max(learningRate, daLearningRate);
    
    // Apply acetylcholine effects
    float achExcitability = 0.0f;
    ach.applyToExcitability(achExcitability);
    ach.applyToLearningRate(achLearningRate);
    excitability += achExcitability;
    learningRate = std::max(learningRate, achLearningRate);
    
    // Apply norepinephrine effects
    float neExcitability = 0.0f;
    float neLearningRate = 0.0f;
    ne.applyToExcitability(neExcitability);
    ne.applyToLearningRate(neLearningRate);
    excitability += neExcitability;
    learningRate = std::max(learningRate, neLearningRate);
    
    // Apply serotonin effects
    float htExcitability = 0.0f;
    float htLearningRate = 0.0f;
    ht.applyToExcitability(htExcitability);
    ht.applyToLearningRate(htLearningRate);
    excitability += htExcitability;
    learningRate = std::max(learningRate, htLearningRate);
}

} // namespace nlm
