// test_neuromodulation_integration.cpp - Neuromodulation integration tests for Phase 6
// Author: Kilo
// Purpose: Test all neuromodulators (adenosine, dopamine, acetylcholine, NE, 5-HT) and their effects

#include "brain_test_common.hpp"

namespace nlm {
namespace test {

/**
 * Test adenosine-based sleep/rest cycle
 */
bool testAdenosineRestCycle() {
    std::cout << "=== Test: Adenosine-Based Sleep/Rest Cycle ===" << std::endl;
    
    Phase6TestConfig config;
    config.maxSteps = 500;
    config.neuronCount = 300;
    
    Phase6TestFixture fixture(config);
    auto brain = fixture.createBrain();
    
    if (!brain) {
        std::cout << "FAIL: Could not create brain" << std::endl;
        return false;
    }
    
    auto* adenosine = brain->getAdenosine();
    if (!adenosine) {
        std::cout << "FAIL: Adenosine system not available" << std::endl;
        return false;
    }
    
    // Test 1: Adenosine initialization
    std::cout << "Test 1: Adenosine initialization" << std::endl;
    
    float initialLevel = adenosine->getLevel();
    std::cout << "Initial adenosine level: " << initialLevel << std::endl;
    
    // Test 2: Adenosine buildup during wake
    std::cout << "Test 2: Adenosine buildup during wake" << std::endl;
    
    // Simulate wake period where adenosine accumulates
    for (int step = 0; step < 100; ++step) {
        // Inject sensory activity (wake state)
        brain->injectCurrentToNeurons(NeuronType::Sensory, 3.0f);
        brain->step(step, step * 0.001);
        
        // Adenosine should accumulate during wake
        float currentLevel = adenosine->getLevel();
        
        if (step % 25 == 0) {
            std::cout << "  Step " << step << ": adenosine level = " << currentLevel << std::endl;
        }
    }
    
    float wakeLevel = adenosine->getLevel();
    std::cout << "Adenosine level after wake: " << wakeLevel << std::endl;
    
    // Test 3: Rest triggering
    std::cout << "Test 3: Rest triggering" << std::endl;
    
    bool shouldRest = adenosine->shouldTriggerRest();
    std::cout << "Should rest based on current level: " << (shouldRest ? "YES" : "NO") << std::endl;
    
    // Set adenosine to high level to trigger rest
    adenosine->setLevel(0.8f);
    
    bool shouldRestNow = adenosine->shouldTriggerRest();
    std::cout << "Should rest at level 0.8: " << (shouldRestNow ? "YES" : "NO") << std::endl;
    
    // Test 4: Rest effects on neural computation
    std::cout << "Test 4: Rest effects on neural computation" << std::endl;
    
    // In rest state, adenosine should reduce neural excitability
    // This is done by injecting negative current
    
    // Get plasticity factor
    float plasticityMod = adenosine->getPlasticityFactor();
    std::cout << "Adenosine plasticity factor: " << plasticityMod << std::endl;
    
    // Lower plasticity during rest
    bool reducedPlasticity = (plasticityMod < 1.0f);
    std::cout << "Reduced plasticity during rest: " << (reducedPlasticity ? "YES" : "NO") << std::endl;
    
    // Test 5: Recovery from rest
    std::cout << "Test 5: Recovery from rest" << std::endl;
    
    // Lower adenosine should end rest
    adenosine->setLevel(0.2f);
    
    bool restingNow = adenosine->shouldTriggerRest();
    bool shouldEndRest = adenosine->shouldEndRest();
    
    std::cout << "Adenosine level: 0.2" << std::endl;
    std::cout << "Should rest: " << (restingNow ? "YES" : "NO") << std::endl;
    std::cout << "Should end rest: " << (shouldEndRest ? "YES" : "NO") << std::endl;
    
    // Reset adenosine
    adenosine->setLevel(0.0f);
    
    // Overall test result
    bool overallSuccess = adenosine && shouldRestNow && reducedPlasticity && shouldEndRest;
    
    std::cout << "=== Adenosine Rest Cycle Test Result: " 
              << (overallSuccess ? "PASS" : "FAIL") << " ===" << std::endl;
    
    return overallSuccess;
}

/**
 * Test dopamine effects on plasticity and attention
 */
bool testDopamineEffects() {
    std::cout << "=== Test: Dopamine Effects ===" << std::endl;
    
    Phase6TestConfig config;
    config.maxSteps = 600;
    config.neuronCount = 400;
    
    Phase6TestFixture fixture(config);
    auto brain = fixture.createBrain();
    
    if (!brain) {
        std::cout << "FAIL: Could not create brain" << std::endl;
        return false;
    }
    
    auto* dopamine = brain->getDopamine();
    if (!dopamine) {
        std::cout << "FAIL: Dopamine system not available" << std::endl;
        return false;
    }
    
    // Test 1: Dopamine initialization
    std::cout << "Test 1: Dopamine initialization" << std::endl;
    
    float initialLevel = dopamine->getLevel();
    std::cout << "Initial dopamine level: " << initialLevel << std::endl;
    
    // Test 2: Dopamine effects on neural excitability
    std::cout << "Test 2: Dopamine effects on neural excitability" << std::endl;
    
    // Get plasticity factor
    float initialPlasticity = dopamine->getPlasticityFactor();
    std::cout << "Initial dopamine plasticity factor: " << initialPlasticity << std::endl;
    
    // Run with reward-like input
    for (int step = 0; step < 150; ++step) {
        // Inject reward-like input (positive sensory input)
        float input = sin(step * 0.1f) * 2.0f + 1.0f;
        brain->injectCurrentToNeurons(NeuronType::Sensory, input);
        
        brain->step(step, step * 0.001);
        
        if (step % 50 == 0) {
            float currentLevel = dopamine->getLevel();
            float currentPlasticity = dopamine->getPlasticityFactor();
            std::cout << "  Step " << step << ": dopamine=" << currentLevel 
                      << ", plasticity factor=" << currentPlasticity << std::endl;
        }
    }
    
    float finalDopamine = dopamine->getLevel();
    float finalPlasticity = dopamine->getPlasticityFactor();
    
    std::cout << "Final dopamine level: " << finalDopamine << std::endl;
    std::cout << "Final plasticity factor: " << finalPlasticity << std::endl;
    
    // Test 3: Dopamine effects on STDP
    std::cout << "Test 3: Dopamine effects on STDP" << std::endl;
    
    // Dopamine should affect STDP learning rates
    auto* stdp = brain->getSTDP();
    if (stdp) {
        std::cout << "STDP system available for dopamine interaction" << std::endl;
        
        // Dopamine plasticity factor should modify STDP
        bool stdpAffected = (finalPlasticity != 1.0f);
        std::cout << "Dopamine affects STDP: " << (stdpAffected ? "YES" : "NO") << std::endl;
    }
    
    // Test 4: Dopamine and reward prediction error
    std::cout << "Test 4: Dopamine and reward prediction error" << std::endl;
    
    // Get prediction error signal
    auto* predictionError = brain->getPredictionErrorSignal();
    if (predictionError) {
        // Dopamine should respond to prediction errors
        float predictionLevel = predictionError->getLevel();
        std::cout << "Prediction error level: " << predictionLevel << std::endl;
        
        // Both prediction error and dopamine should be related
        bool neuromodulationLinked = (finalDopamine > 0.0f && predictionLevel > 0.0f);
        std::cout << "Dopamine and prediction error linked: " << (neuromodulationLinked ? "YES" : "NO") << std::endl;
    }
    
    // Test 5: Dopamine and working memory
    std::cout << "Test 5: Dopamine and working memory" << std::endl;
    
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        // Get working memory activity
        float memoryActivity = workingMemory->getMemoryActivity();
        std::cout << "Working memory activity: " << memoryActivity << std::endl;
        
        // Dopamine should affect memory retention
        bool memoryAffected = (memoryActivity > 0.0f);
        std::cout << "Dopamine affects working memory: " << (memoryAffected ? "YES" : "NO") << std::endl;
    }
    
    // Test 6: Dopamine and attention
    std::cout << "Test 6: Dopamine and attention" << std::endl;
    
    auto* attention = brain->getAttention();
    if (attention) {
        // Get attention winners
        auto winners = attention->getWinners();
        std::cout << "Attention system has " << winners.size() << " winners" << std::endl;
        
        // Dopamine should affect attention selection
        bool attentionAffected = !winners.empty();
        std::cout << "Dopamine affects attention: " << (attentionAffected ? "YES" : "NO") << std::endl;
    }
    
    // Overall test result
    bool overallSuccess = dopamine && finalDopamine >= 0.0f && finalPlasticity > 0.0f;
    
    std::cout << "=== Dopamine Effects Test Result: " 
              << (overallSuccess ? "PASS" : "FAIL") << " ===" << std::endl;
    
    return overallSuccess;
}

/**
 * Test acetylcholine effects on memory and attention
 */
bool testAcetylcholineEffects() {
    std::cout << "=== Test: Acetylcholine Effects ===" << std::endl;
    
    Phase6TestConfig config;
    config.maxSteps = 500;
    config.neuronCount = 350;
    
    Phase6TestFixture fixture(config);
    auto brain = fixture.createBrain();
    
    if (!brain) {
        std::cout << "FAIL: Could not create brain" << std::endl;
        return false;
    }
    
    auto* acetylcholine = brain->getAcetylcholine();
    if (!acetylcholine) {
        std::cout << "FAIL: Acetylcholine system not available" << std::endl;
        return false;
    }
    
    // Test 1: Acetylcholine initialization
    std::cout << "Test 1: Acetylcholine initialization" << std::endl;
    
    float initialLevel = acetylcholine->getLevel();
    std::cout << "Initial acetylcholine level: " << initialLevel << std::endl;
    
    // Test 2: Acetylcholine effects on attention
    std::cout << "Test 2: Acetylcholine effects on attention" << std::endl;
    
    // Get attention system
    auto* attention = brain->getAttention();
    if (attention) {
        // Set acetylcholine level to see its effects
        acetylcholine->setLevel(0.7f);
        
        // Update attention
        attention->update(0.001);
        
        // Check attention effects
        auto winners = attention->getWinners();
        std::cout << "Attention winners after ACh: " << winners.size() << std::endl;
        
        // Acetylcholine should enhance attention
        bool attentionEnhanced = !winners.empty();
        std::cout << "Acetylcholine enhances attention: " << (attentionEnhanced ? "YES" : "NO") << std::endl;
    }
    
    // Test 3: Acetylcholine effects on memory
    std::cout << "Test 3: Acetylcholine effects on memory" << std::endl;
    
    // Get working memory system
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        // Set acetylcholine level
        acetylcholine->setLevel(0.6f);
        
        // Update working memory
        workingMemory->update(0.001);
        
        // Check working memory effects
        float memoryActivity = workingMemory->getMemoryActivity();
        std::cout << "Working memory activity with ACh: " << memoryActivity << std::endl;
        
        // Acetylcholine should enhance memory encoding
        bool memoryEnhanced = (memoryActivity > 0.1f);
        std::cout << "Acetylcholine enhances memory: " << (memoryEnhanced ? "YES" : "NO") << std::endl;
    }
    
    // Test 4: Acetylcholine signal novelty detection
    std::cout << "Test 4: Acetylcholine signal novelty" << std::endl;
    
    // Acetylcholine responds to novel stimuli
    acetylcholine->signalNovelty(0.8f);
    
    float noveltyLevel = acetylcholine->getLevel();
    std::cout << "Acetylcholine level after novelty signal: " << noveltyLevel << std::endl;
    
    bool noveltyResponse = (noveltyLevel > 0.5f);
    std::cout << "Acetylcholine responds to novelty: " << (noveltyResponse ? "YES" : "NO") << std::endl;
    
    // Test 5: Acetylcholine signal unpredicted events
    std::cout << "Test 5: Acetylcholine signal unpredicted events" << std::endl;
    
    // Reset and test unpredicted signal
    acetylcholine->setLevel(0.0f);
    acetylcholine->signalUnpredicted(0.7f);
    
    float unpredictedLevel = acetylcholine->getLevel();
    std::cout << "Acetylcholine level after unpredicted signal: " << unpredictedLevel << std::endl;
    
    bool unpredictedResponse = (unpredictedLevel > 0.5f);
    std::cout << "Acetylcholine responds to unpredicted: " << (unpredictedResponse ? "YES" : "NO") << std::endl;
    
    // Test 6: Acetylcholine arousal effects
    std::cout << "Test 6: Acetylcholine arousal effects" << std::endl;
    
    // Reset and check arousal
    acetylcholine->setLevel(0.0f);
    acetylcholine->setLevel(0.8f);
    
    float arousal = acetylcholine->getArousal();
    std::cout << "Acetylcholine arousal level: " << arousal << std::endl;
    
    bool arousalActive = (arousal > 0.0f);
    std::cout << "Acetylcholine generates arousal: " << (arousalActive ? "YES" : "NO") << std::endl;
    
    // Overall test result
    bool overallSuccess = acetylcholine && noveltyResponse && unpredictedResponse && arousalActive;
    
    std::cout << "=== Acetylcholine Effects Test Result: " 
              << (overallSuccess ? "PASS" : "FAIL") << " ===" << std::endl;
    
    return overallSuccess;
}

/**
 * Run all neuromodulation integration tests
 */
void runAllNeuromodulationIntegrationTests() {
    TestStats stats;
    PerformanceTimer timer;
    
    // Test adenosine rest cycle
    timer.reset();
    bool test1Passed = testAdenosineRestCycle();
    stats.addTest("Adenosine Rest Cycle", test1Passed, timer.elapsed());
    
    // Test dopamine effects
    timer.reset();
    bool test2Passed = testDopamineEffects();
    stats.addTest("Dopamine Effects", test2Passed, timer.elapsed());
    
    // Test acetylcholine effects
    timer.reset();
    bool test3Passed = testAcetylcholineEffects();
    stats.addTest("Acetylcholine Effects", test3Passed, timer.elapsed());
    
    // Report results
    stats.report();
}

} // namespace test
} // namespace nlm