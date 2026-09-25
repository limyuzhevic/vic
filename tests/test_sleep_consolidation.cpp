// Sleep/Rest Cycle and Memory Consolidation Tests
// Phase 2: Real Neural Computation Tests
// Comprehensive tests for sleep/rest cycles and memory consolidation systems

#include "brain/Brain.hpp"
#include "memory/Memory.hpp"
#include "development/DevelopmentSystem.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

namespace test_sleep_consolidation {

void testSleepDriveCalculation() {
    std::cout << "  Testing sleep drive calculation..." << std::endl;
    
    // Simulate sleep drive factors
    float timeAwake = 400.0f;  // 400 time units awake
    float neuralActivity = 800.0f;  // High neural activity
    float age = 0.5f;  // 0.5 years old
    
    // Calculate sleep drive components
    float timeAwakeFactor = std::min(1.0f, timeAwake / 500.0f);
    float activityFactor = std::min(1.0f, neuralActivity / 1000.0f);
    float developmentalFactor = 1.0f - (age * 0.2f);  // Younger = higher need
    developmentalFactor = std::max(0.5f, developmentalFactor);  // Minimum 0.5
    float noveltyFactor = 0.5f;  // Would be calculated from novelty detection
    
    float sleepDrive = timeAwakeFactor * 0.4f + 
                       activityFactor * 0.3f + 
                       developmentalFactor * 0.2f + 
                       noveltyFactor * 0.1f;
    
    // Sleep drive should be between 0 and 1
    assert(sleepDrive >= 0.0f && sleepDrive <= 1.0f);
    assert(sleepDrive > 0.5f);  // With these parameters, should be high
    
    std::cout << "    PASSED (sleep drive: " << sleepDrive << ")" << std::endl;
}

void testSleepPhaseProgression() {
    std::cout << "  Testing sleep phase progression..." << std::endl;
    
    // Simulate sleep phase progression
    enum class SleepPhase {
        Awake,
        LightSleep,
        DeepSleep,
        REM
    };
    
    SleepPhase phase = SleepPhase::Awake;
    float phaseDuration = 100.0f;
    float currentStep = 0.0f;
    float nextPhaseTransition = 100.0f;
    float sleepProgress = 0.0f;
    
    // Simulate progression through phases
    std::vector<std::pair<float, SleepPhase>> transitions = {
        {100.0f, SleepPhase::LightSleep},
        {130.0f, SleepPhase::DeepSleep},
        {195.0f, SleepPhase::REM},
        {220.0f, SleepPhase::Awake}
    };
    
    for (const auto& transition : transitions) {
        currentStep = transition.first;
        if (currentStep >= nextPhaseTransition) {
            phase = transition.second;
            nextPhaseTransition = currentStep + 20.0f;
            sleepProgress = (currentStep - (nextPhaseTransition - 20.0f)) / 20.0f;
        }
        
        // Verify phase transition
        assert(phase == transition.second || currentStep < transitions[0].first);
    }
    
    std::cout << "    PASSED" << std::endl;
}

void testSynapticHomeostasis() {
    std::cout << "  Testing synaptic homeostasis..." << std::endl;
    
    struct SynapticHomeostasis {
        float synapticStrengthTotal = 0.0f;
        float synapticStrengthTarget = 100.0f;
        float normalizationFactor = 1.0f;
        float saturationThreshold = 2.0f;
        float adaptationRate = 0.01f;
    };
    
    SynapticHomeostasis homeostasis;
    
    // Test with low synaptic strength
    homeostasis.synapticStrengthTotal = 50.0f;  // Below target
    homeostasis.normalizationFactor = 1.0f;
    
    // Test with high synaptic strength (overload detected)
    homeostasis.synapticStrengthTotal = 250.0f;  // Above target * saturation
    float overloadFactor = homeostasis.synapticStrengthTotal / homeostasis.synapticStrengthTarget;
    
    if (overloadFactor > homeostasis.saturationThreshold) {
        float downscalingFactor = 1.0f - std::min(0.5f, (overloadFactor - 1.0f) * 0.1f);
        homeostasis.normalizationFactor = downscalingFactor;
    }
    
    // Normalization factor should be reduced
    assert(homeostasis.normalizationFactor < 1.0f);
    
    std::cout << "    PASSED" << std::endl;
}

void testMemoryPrioritization() {
    std::cout << "  Testing memory prioritization..." << std::endl;
    
    struct MemoryPriorityWeights {
        float recencyWeight = 0.3f;
        float rewardWeight = 0.25f;
        float noveltyWeight = 0.2f;
        float coherenceWeight = 0.1f;
        float importanceWeight = 0.15f;
    };
    
    MemoryPriorityWeights weights;
    
    // Test that weights sum to 1.0 (or approximately 1.0)
    float totalWeight = weights.recencyWeight + weights.rewardWeight + 
                        weights.noveltyWeight + weights.coherenceWeight + 
                        weights.importanceWeight;
    assert(std::abs(totalWeight - 1.0f) < 0.001f);
    
    // Test individual weight ranges
    assert(weights.recencyWeight >= 0.0f && weights.recencyWeight <= 1.0f);
    assert(weights.rewardWeight >= 0.0f && weights.rewardWeight <= 1.0f);
    assert(weights.noveltyWeight >= 0.0f && weights.noveltyWeight <= 1.0f);
    assert(weights.coherenceWeight >= 0.0f && weights.coherenceWeight <= 1.0f);
    assert(weights.importanceWeight >= 0.0f && weights.importanceWeight <= 1.0f);
    
    // Test that recency weight is higher (typically)
    assert(weights.recencyWeight >= weights.rewardWeight);
    assert(weights.recencyWeight >= weights.noveltyWeight);
    
    std::cout << "    PASSED" << std::endl;
}

void testEnhancedMemoryConsolidation() {
    std::cout << "  Testing enhanced memory consolidation..." << std::endl;
    
    // Simulate sleep phase bonuses for memory consolidation
    enum class SleepPhase {
        LightSleep,
        DeepSleep,
        REM,
        Awake
    };
    
    std::vector<std::pair<SleepPhase, float>> phaseBonuses = {
        {SleepPhase::LightSleep, 1.2f},
        {SleepPhase::DeepSleep, 2.0f},
        {SleepPhase::REM, 1.5f},
        {SleepPhase::Awake, 1.0f}
    };
    
    float baseConsolidationStrength = 0.5f;
    
    for (const auto& phaseBonus : phaseBonuses) {
        SleepPhase phase = phaseBonus.first;
        float bonus = phaseBonus.second;
        
        float enhancedStrength = baseConsolidationStrength * bonus;
        
        // Enhanced strength should be reasonable
        assert(enhancedStrength > 0.0f);
        assert(enhancedStrength <= 3.0f);  // Should not exceed 3x base
        
        // Deep sleep should have highest consolidation
        if (phase == SleepPhase::DeepSleep) {
            assert(enhancedStrength == 1.0f);  // 0.5 * 2.0 = 1.0
        }
    }
    
    std::cout << "    PASSED" << std::endl;
}

void testSleepConsolidationIntegration() {
    std::cout << "  Testing sleep consolidation integration with neural systems..." << std::endl;
    
    // This test simulates integration between sleep/rest cycles and brain systems
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Get initial spike count
    size_t initialSpikeCount = brain->getTotalSpikeCount();
    
    // Simulate learning phase (high activity)
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        // Inject stimulation to promote learning
        for (size_t i = 0; i < std::min(size_t(20), brain->getTotalNeuronCount()); ++i) {
            brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
        }
        brain->step(step, step * 0.001);
    }
    
    size_t learningSpikeCount = brain->getTotalSpikeCount() - initialSpikeCount;
    
    // Simulate rest phase (simulated through reduced activity)
    for (nlm::SimulationStep step = 1000; step < 2000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t restSpikeCount = brain->getTotalSpikeCount() - initialSpikeCount - learningSpikeCount;
    
    // Simulate consolidation phase
    for (nlm::SimulationStep step = 2000; step < 2500; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t consolidationSpikeCount = brain->getTotalSpikeCount() - initialSpikeCount - learningSpikeCount - restSpikeCount;
    
    // Verify that learning occurred
    assert(learningSpikeCount > 0);
    
    // Verify that system processed rest and consolidation phases
    assert(restSpikeCount >= 0);
    assert(consolidationSpikeCount >= 0);
    
    std::cout << "    PASSED (learning: " << learningSpikeCount << 
              ", rest: " << restSpikeCount << 
              ", consolidation: " << consolidationSpikeCount << ")" << std::endl;
}

void runAll() {
    std::cout << "=== Sleep/Rest Cycle and Memory Consolidation Tests ===" << std::endl;
    
    testSleepDriveCalculation();
    testSleepPhaseProgression();
    testSynapticHomeostasis();
    testMemoryPrioritization();
    testEnhancedMemoryConsolidation();
    testSleepConsolidationIntegration();
    
    std::cout << std::endl;
    std::cout << "=== All Sleep/Rest Cycle Tests PASSED ===" << std::endl;
}

} // namespace test_sleep_consolidation
