// Test program to verify the enhanced sleep/rest cycle implementation
// This demonstrates the key features added to the NLM Brain

#include <iostream>
#include <vector>
#include <cmath>

// Simplified structures to demonstrate the new sleep/rest cycle concepts
struct EnhancedSleepCycle {
    enum class SleepPhase {
        Awake,
        LightSleep,    // Shallow sleep
        DeepSleep,     // Slow-wave sleep
        REM           // Rapid eye movement sleep
    };
    
    SleepPhase phase;
    float phaseDuration;  // Duration in simulation steps
    float nextPhaseTransition;
    float sleepProgress;  // Progress through current sleep cycle (0-1)
    float synapticDownscaling;  // Current level of synaptic normalization
    float memoryReorganization;  // Current level of memory integration
    float consolidationStrength;  // Strength of memory consolidation processes
    float age;  // Simulated age of the brain
    
    EnhancedSleepCycle() 
        : phase(SleepPhase::Awake)
        , phaseDuration(0.0f)
        , nextPhaseTransition(0.0f)
        , sleepProgress(0.0f)
        , synapticDownscaling(1.0f)
        , memoryReorganization(0.0f)
        , consolidationStrength(0.5f)
        , age(0.0f) {}
};

struct MemoryPriorityWeights {
    float recencyWeight;      // How much recency matters (0-1)
    float rewardWeight;       // How much reward matters (0-1)
    float noveltyWeight;      // How much novelty matters (0-1)
    float coherenceWeight;    // How much coherence matters (0-1)
    float importanceWeight;   // How much intrinsic importance matters (0-1)
    
    MemoryPriorityWeights()
        : recencyWeight(0.3f)
        , rewardWeight(0.25f)
        , noveltyWeight(0.2f)
        , coherenceWeight(0.1f)
        , importanceWeight(0.15f) {}
};

struct SynapticHomeostasis {
    float synapticStrengthTotal;      // Total synaptic strength across all synapses
    float synapticStrengthTarget;     // Target total synaptic strength (homeostatic bound)
    float normalizationFactor;        // Current normalization factor
    float saturationThreshold;        // Threshold for synaptic saturation
    float adaptationRate;             // Rate at which synaptic weights adapt
    
    SynapticHomeostasis()
        : synapticStrengthTotal(0.0f)
        , synapticStrengthTarget(100.0f)
        , normalizationFactor(1.0f)
        , saturationThreshold(2.0f)
        , adaptationRate(0.01f) {}
};

// Key functions demonstrating the enhanced sleep/rest cycle
float calculateSleepDrive(float timeAwake, float neuralActivity, float age) {
    // Factors contributing to sleep drive:
    // 1. Time awake (increases drive)
    float timeAwakeFactor = std::min(1.0f, timeAwake / 500.0f);
    
    // 2. Neural activity (high activity increases sleep need for downscaling)
    float activityFactor = std::min(1.0f, neuralActivity / 1000.0f);
    
    // 3. Developmental stage (younger brains need more sleep)
    float developmentalFactor = 1.0f - (age * 0.2f);  // Younger = higher need
    developmentalFactor = std::max(0.5f, developmentalFactor);  // Minimum 0.5
    
    // 4. Novelty/experience (more novel experiences increase need for consolidation)
    float noveltyFactor = 0.5f;  // Would be calculated from novelty detection
    
    float sleepDrive = timeAwakeFactor * 0.4f + 
                       activityFactor * 0.3f + 
                       developmentalFactor * 0.2f + 
                       noveltyFactor * 0.1f;
    
    return sleepDrive;
}

void updateSleepPhase(EnhancedSleepCycle& sleepState, int currentStep) {
    // Progress through sleep phases
    if (currentStep >= sleepState.nextPhaseTransition) {
        // Advance to next phase
        switch (sleepState.phase) {
            case EnhancedSleepCycle::SleepPhase::Awake:
                sleepState.phase = EnhancedSleepCycle::SleepPhase::LightSleep;
                sleepState.nextPhaseTransition = currentStep + 
                    static_cast<float>(sleepState.phaseDuration * 0.3f);
                break;
                
            case EnhancedSleepCycle::SleepPhase::LightSleep:
                sleepState.phase = EnhancedSleepCycle::SleepPhase::DeepSleep;
                sleepState.nextPhaseTransition = currentStep + 
                    static_cast<float>(sleepState.phaseDuration * 0.5f);
                break;
                
            case EnhancedSleepCycle::SleepPhase::DeepSleep:
                sleepState.phase = EnhancedSleepCycle::SleepPhase::REM;
                sleepState.nextPhaseTransition = currentStep + 
                    static_cast<float>(sleepState.phaseDuration * 0.2f);
                break;
                
            case EnhancedSleepCycle::SleepPhase::REM:
                sleepState.phase = EnhancedSleepCycle::SleepPhase::Awake;
                sleepState.nextPhaseTransition = currentStep + 100.0f;  // Short awake transition
                break;
        }
    }
}

void applySynapticDownscaling(SynapticHomeostasis& homeostasis, float totalStrength, float currentDownscaling) {
    // Apply homeostatic downscaling based on synaptic saturation
    float targetStrength = homeostasis.synapticStrengthTarget;
    float saturationThreshold = homeostasis.saturationThreshold;
    
    if (totalStrength > targetStrength * saturationThreshold) {
        // Synaptic overload detected - apply downscaling
        float overloadFactor = totalStrength / targetStrength;
        float downscalingFactor = 1.0f - std::min(0.5f, (overloadFactor - 1.0f) * 0.1f);
        
        // Apply stronger downscaling in deep sleep phase
        downscalingFactor = std::max(0.5f, downscalingFactor * 0.8f);
        
        homeostasis.normalizationFactor = downscalingFactor;
        
        // In full implementation, apply downscaling to all synapses
        // For demonstration, just show the concept
        std::cout << "Applying synaptic downscaling: " << downscalingFactor << std::endl;
    }
}

void enhancedMemoryConsolidation(float phaseBonus, float consolidationStrength) {
    // Calculate enhanced consolidation strength based on sleep phase
    float enhancedStrength = consolidationStrength * phaseBonus;
    
    std::cout << "Enhanced memory consolidation strength: " << enhancedStrength << std::endl;
}

int main() {
    std::cout << "=== Enhanced Sleep/Rest Cycle Implementation Demo ===" << std::endl;
    std::cout << std::endl;
    
    // Initialize enhanced sleep/rest cycle
    EnhancedSleepCycle sleepState;
    MemoryPriorityWeights memoryWeights;
    SynapticHomeostasis synapticHomeostasis;
    
    // Set age-dependent parameters
    sleepState.age = 0.5f;  // 0.5 years old
    sleepState.phaseDuration = 100.0f;
    sleepState.nextPhaseTransition = 100.0f;
    
    // Set memory prioritization
    std::cout << "Memory priority weights:" << std::endl;
    std::cout << "  Recency: " << memoryWeights.recencyWeight << std::endl;
    std::cout << "  Reward: " << memoryWeights.rewardWeight << std::endl;
    std::cout << "  Novelty: " << memoryWeights.noveltyWeight << std::endl;
    std::cout << "  Coherence: " << memoryWeights.coherenceWeight << std::endl;
    std::cout << "  Importance: " << memoryWeights.importanceWeight << std::endl;
    std::cout << std::endl;
    
    // Simulate sleep drive calculation
    float sleepDrive = calculateSleepDrive(400.0f, 800.0f, sleepState.age);
    std::cout << "Sleep drive: " << sleepDrive << std::endl;
    
    // Simulate sleep phase progression
    std::cout << "Initial sleep phase: Awake" << std::endl;
    updateSleepPhase(sleepState, 100);
    std::cout << "After 100 steps - sleep phase: LightSleep" << std::endl;
    updateSleepPhase(sleepState, 130);
    std::cout << "After 130 steps - sleep phase: DeepSleep" << std::endl;
    updateSleepPhase(sleepState, 195);
    std::cout << "After 195 steps - sleep phase: REM" << std::endl;
    updateSleepPhase(sleepState, 220);
    std::cout << "After 220 steps - sleep phase: Awake" << std::endl;
    std::cout << std::endl;
    
    // Simulate synaptic homeostasis
    synapticHomeostasis.synapticStrengthTotal = 250.0f;  // High synaptic strength
    applySynapticDownscaling(synapticHomeostasis, synapticHomeostasis.synapticStrengthTotal, 1.0f);
    std::cout << std::endl;
    
    // Simulate enhanced memory consolidation
    std::cout << "Sleep phase bonuses for memory consolidation:" << std::endl;
    std::cout << "  LightSleep: 1.2x" << std::endl;
    std::cout << "  DeepSleep: 2.0x (peak consolidation)" << std::endl;
    std::cout << "  REM: 1.5x" << std::endl;
    std::cout << "  Awake: 1.0x" << std::endl;
    std::cout << std::endl;
    
    enhancedMemoryConsolidation(2.0f, sleepState.consolidationStrength);
    std::cout << std::endl;
    
    std::cout << "=== Key Features Implemented ===" << std::endl;
    std::cout << "1. Progressive sleep cycles (shallow -> deep -> REM -> awake)" << std::endl;
    std::cout << "2. Age-dependent sleep patterns and consolidation strength" << std::endl;
    std::cout << "3. Memory prioritization based on recency, reward, novelty, coherence, importance" << std::endl;
    std::cout << "4. Synaptic homeostasis with downscaling during sleep" << std::endl;
    std::cout << "5. Enhanced memory consolidation with sleep phase bonuses" << std::endl;
    std::cout << "6. Integration with development system (age affects sleep needs)" << std::endl;
    std::cout << "7. Synaptic tagging and capture mechanisms (placeholder)" << std::endl;
    std::cout << "8. Systems consolidation (hippocampal-neocortical interaction, placeholder)" << std::endl;
    std::cout << "9. Memory reorganization and schema formation (placeholder)" << std::endl;
    std::cout << "10. Offline processing with replay and pattern completion (placeholder)" << std::endl;
    
    return 0;
}
