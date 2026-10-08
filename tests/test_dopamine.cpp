#include "neuromodulation/Neuromodulator.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

int main() {
    // Initialize logging - use getGlobal() as per Logger.hpp interface
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    std::cout << "=== Dopamine Neuromodulator Test ===" << std::endl;
    
    // Create dopamine neuromodulator
    Dopamine dopamine;
    
    // Test initial state
    std::cout << "Initial level: " << dopamine.getLevel() << std::endl;
    std::cout << "Initial plasticity factor: " << dopamine.getPlasticityFactor() << std::endl;
    
    // Set expected value for RPE calculation
    dopamine.setExpectedValue(0.5f);
    
    // Test baseline dynamics
    std::cout << "\n--- Testing Baseline Dynamics ---" << std::endl;
    for (int i = 0; i < 10; ++i) {
        dopamine.update(0.1f); // 100ms update
        std::cout << "Step " << i << ": Level = " << dopamine.getLevel() 
                  << ", Plasticity = " << dopamine.getPlasticityFactor() << std::endl;
    }
    
    // Test reward signaling (positive RPE)
    std::cout << "\n--- Testing Reward Signaling (Positive RPE) ---" << std::endl;
    dopamine.signalReward(1.0f); // Large reward
    std::cout << "After reward (actual=1.0, expected=0.5): Level = " << dopamine.getLevel() << std::endl;
    
    dopamine.update(0.1f);
    std::cout << "After decay: Level = " << dopamine.getLevel() << std::endl;
    
    // Test negative RPE signaling
    std::cout << "\n--- Testing Negative RPE Signaling ---" << std::endl;
    dopamine.setExpectedValue(1.0f); // Set high expectation
    dopamine.signalRewardPredictionError(-0.5f); // Negative RPE
    std::cout << "After negative RPE (actual=0.5, expected=1.0): Level = " << dopamine.getLevel() << std::endl;
    
    dopamine.update(0.1f);
    std::cout << "After decay: Level = " << dopamine.getLevel() << std::endl;
    
    // Test STDP modulation
    std::cout << "\n--- Testing STDP Modulation ---" << std::endl;
    dopamine.updateSTDP(0.01f, 10.0f); // Positive STDP
    std::cout << "After positive STDP: Level = " << dopamine.getLevel() << std::endl;
    
    dopamine.updateSTDP(-0.005f, -5.0f); // Negative STDP
    std::cout << "After negative STDP: Level = " << dopamine.getLevel() << std::endl;
    
    // Test adaptation
    std::cout << "\n--- Testing Adaptation ---" << std::endl;
    dopamine.setExpectedValue(0.0f);
    dopamine.signalReward(0.2f);
    std::cout << "Before adaptation: Level = " << dopamine.getLevel() << std::endl;
    
    dopamine.adaptBaseline(0.8f, 0.2f);
    std::cout << "After adaptation: Level = " << dopamine.getLevel() << std::endl;
    
    // Get statistics
    std::cout << "\n--- Statistics ---" << std::endl;
    auto stats = dopamine.getStatistics();
    std::cout << "Average level: " << stats.averageLevel << std::endl;
    std::cout << "Peak level: " << stats.peakLevel << std::endl;
    std::cout << "Min level: " << stats.minLevel << std::endl;
    std::cout << "Max level: " << stats.maxLevel << std::endl;
    std::cout << "Average plasticity: " << stats.averagePlasticity << std::endl;
    
    std::cout << "\n=== Test Complete ===" << std::endl;
    return 0;
}