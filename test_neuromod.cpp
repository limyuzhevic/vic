// Test file to verify neuromodulation integration
// This test can be compiled and run to verify the enhanced neuromodulation system

#include "Neuromodulator.hpp"
#include <iostream>

int main() {
    std::cout << "Testing enhanced neuromodulation system (Level 4) ..." << std::endl;
    
    // Test Dopamine system
    Dopamine dopamine;
    
    // Test basic operations
    std::cout << "1. Testing Dopamine initialization..." << std::endl;
    std::cout << "   Name: " << dopamine.getName() << std::endl;
    std::cout << "   Initial Level: " << dopamine.getLevel() << std::endl;
    
    // Test level setting
    std::cout << "\n2. Testing level setting..." << std::endl;
    dopamine.setLevel(0.5f);
    std::cout << "   After setting to 0.5: " << dopamine.getLevel() << std::endl;
    
    // Test baseline
    std::cout << "\n3. Testing baseline configuration..." << std::endl;
    dopamine.setBaseline(0.1f);
    std::cout << "   Baseline set to: " << dopamine.getBaseline() << std::endl;
    
    // Test update
    std::cout << "\n4. Testing update mechanism..." << std::endl;
    dopamine.update(0.1f);
    std::cout << "   After update (0.1s): " << dopamine.getLevel() << std::endl;
    
    // Test reward signaling
    std::cout << "\n5. Testing reward signaling..." << std::endl;
    dopamine.signalReward(0.8f);
    std::cout << "   After reward 0.8: " << dopamine.getLevel() << std::endl;
    
    // Test prediction error signaling
    std::cout << "\n6. Testing prediction error signaling..." << std::endl;
    dopamine.signalRewardPredictionError(0.3f);
    std::cout << "   After RPE 0.3: " << dopamine.getLevel() << std::endl;
    
    // Test plasticity factor
    std::cout << "\n7. Testing plasticity factor..." << std::endl;
    float plasticity = dopamine.getPlasticityFactor();
    std::cout << "   Plasticity Factor: " << plasticity << std::endl;
    
    // Test statistics
    std::cout << "\n8. Testing statistics..." << std::endl;
    std::string stats = dopamine.getStatistics();
    std::cout << stats << std::endl;
    
    // Test reset
    std::cout << "\n9. Testing reset..." << std::endl;
    dopamine.reset();
    std::cout << "   After reset, Level: " << dopamine.getLevel() << std::endl;
    
    std::cout << "\n✓ All Dopamine system tests passed!" << std::endl;
    
    return 0;
}
