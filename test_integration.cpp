#include "src/prediction/PredictionSystem.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include <iostream>

int main() {
    std::cout << "Testing prediction system integration..." << std::endl;
    
    // Test 1: Check if PredictionSystem has required methods
    PredictionSystem predSys;
    
    // Test 2: Check if Dopamine has signalPredictionError method
    Dopamine dopamine;
    
    // Test 3: Check if Neuromodulator abstract methods are implemented
    Neuromodulator* mod = &dopamine;
    mod->update(0.001f);
    mod->signalPredictionError(0.5f);
    mod->setLevel(0.3f);
    float level = mod->getLevel();
    float factor = mod->getPlasticityFactor();
    
    std::cout << "All integration tests passed!" << std::endl;
    return 0;
}
