// Test framework for integrated systems in NLM
// Comprehensive test coverage for all integrated systems

#ifndef INTEGRATED_SYSTEMS_TESTS_HPP
#define INTEGRATED_SYSTEMS_TESTS_HPP

#include <cassert>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <numeric>

namespace nlm {

// Forward declarations
class AssociativeMemory;
class PredictionSystem;
class ConceptFormation;
class Neuromodulator;
class Dopamine;
class WorkingMemory;
class PredictionError;
class Curiosity;
class Novelty;
class SensoryInput;
class Brain;

namespace test_integrated_systems {

// Test function declarations
void testAssociativeMemoryPatternCompletion();
void testAssociativeMemorySpreadActivation();
void testAssociativeMemoryMemoryRetention();
void testAssociativeMemoryIntegrationWithWorkingMemory();

void testPredictionSystemAccuracy();
void testPredictionSystemErrorTracking();
void testPredictionSystemConfidenceAdaptation();
void testPredictionSystemIntegrationWithCuriosity();

void testConceptFormationBasic();
void testConceptFormationStability();
void testConceptFormationGeneralization();
void testConceptFormationIntegrationWithEpisodicMemory();

void testNeuromodulationDopamine();
void testNeuromodulationPredictionError();
void testNeuromodulationCuriosity();
void testNeuromodulationNoveltyDetection();

void testCrossSystemMemoryPrediction();
void testCrossSystemConceptAttention();
void testCrossSystemPredictionDopamine();

void testPerformanceMemoryEfficiency();
void testPerformancePredictionAccuracyImprovement();
void testPerformanceSystemStability();

void runAllIntegratedSystemTests();

} // namespace test_integrated_systems

} // namespace nlm

#endif // INTEGRATED_SYSTEMS_TESTS_HPP

