// Test file for Enhanced Neuromodulation Systems
// Validates the integrated neuromodulation functionality

#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include <iostream>
#include <cassert>

using namespace nlm;

void testDopamineSystem() {
    std::cout << "Testing Dopamine System..." << std::endl;
    
    Dopamine dopamine;
    
    // Test configuration
    dopamine.configure(0.1f, 0.05f, 1.0f);
    
    // Test update method
    dopamine.update(0.001);
    
    // Test level access
    float level = dopamine.getLevel();
    assert(level >= 0.0f && level <= 1.2f); // Within expected range
    
    // Test plasticity factor
    float plasticity = dopamine.getPlasticityFactor();
    assert(plasticity >= 0.8f && plasticity <= 1.5f); // Within expected range
    
    // Test reset
    dopamine.reset();
    
    std::cout << "✓ Dopamine System tests passed" << std::endl;
}

void testCuriositySystem() {
    std::cout << "Testing Curiosity System..." << std::endl;
    
    Curiosity curiosity;
    
    // Test configuration
    curiosity.configure(0.05f, 0.1f);
    
    // Test update
    curiosity.update(0.001);
    
    // Test level access
    float level = curiosity.getLevel();
    assert(level >= 0.0f && level <= 1.0f);
    
    curiosity.reset();
    
    std::cout << "✓ Curiosity System tests passed" << std::endl;
}

void testNoveltySystem() {
    std::cout << "Testing Novelty System..." << std::endl;
    
    Novelty novelty;
    
    // Test threshold setting
    novelty.setThreshold(0.25f);
    
    // Test update
    novelty.update(0.001);
    
    // Test detection
    bool detected = novelty.isNoveltyDetected();
    // Should be false initially since no input provided
    
    // Test reset
    novelty.reset();
    
    std::cout << "✓ Novelty System tests passed" << std::endl;
}

void testPredictionErrorSystem() {
    std::cout << "Testing Prediction Error System..." << std::endl;
    
    PredictionError predictionError;
    
    // Test weight setting
    predictionError.setWeight(0.5f);
    
    // Test update (requires prediction error signal)
    predictionError.update(0.001);
    
    // Test error access
    float error = predictionError.getError();
    assert(error >= 0.0f && error <= 1.0f);
    
    // Test reset
    predictionError.reset();
    
    std::cout << "✓ Prediction Error System tests passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "Testing Neuromodulation Integration..." << std::endl;
    
    // Test that neuromodulators can be used together
    Dopamine dopamine;
    Curiosity curiosity;
    Novelty novelty;
    PredictionError predictionError;
    
    // Configure all systems
    dopamine.configure(0.1f, 0.05f, 1.0f);
    curiosity.configure(0.05f, 0.1f);
    novelty.setThreshold(0.3f);
    predictionError.setWeight(0.5f);
    
    // Update all systems
    dopamine.update(0.001);
    curiosity.update(0.001);
    novelty.update(0.001);
    predictionError.update(0.001);
    
    // Test that they provide different signals
    assert(dopamine.getLevel() != curiosity.getLevel());
    
    // Test reset all
    dopamine.reset();
    curiosity.reset();
    novelty.reset();
    predictionError.reset();
    
    std::cout << "✓ Neuromodulation Integration tests passed" << std::endl;
}

void testNeuromodulationStatistics() {
    std::cout << "Testing Neuromodulation Statistics..." << std::endl;
    
    Dopamine dopamine;
    dopamine.configure(0.1f, 0.05f, 1.0f);
    
    // Update multiple times
    for (int i = 0; i < 100; ++i) {
        dopamine.update(0.001);
    }
    
    // Test that level changes over time (due to decay)
    float finalLevel = dopamine.getLevel();
    float initialLevel = 0.1f; // Baseline level
    assert(finalLevel <= initialLevel * 1.1f); // Should not increase dramatically
    
    // Test plasticity factor range
    float plasticity = dopamine.getPlasticityFactor();
    assert(plasticity > 0.0f && plasticity <= 2.0f); // Should be in reasonable range
    
    dopamine.reset();
    
    std::cout << "✓ Neuromodulation Statistics tests passed" << std::endl;
}

int main() {
    std::cout << "=== NLM Neuromodulation Systems Test Suite ===" << std::endl;
    
    try {
        testDopamineSystem();
        testCuriositySystem();
        testNoveltySystem();
        testPredictionErrorSystem();
        testNeuromodulationIntegration();
        testNeuromodulationStatistics();
        
        std::cout << std::endl;
        std::cout << "=== All neuromodulation tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}