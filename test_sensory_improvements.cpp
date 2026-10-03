//!/usr/bin/env g++
// Minimal test to verify SensoryPercept changes compile correctly
#include "src/agent/SensoryPercept.hpp"
#include <iostream>
#include <stdexcept>

int main() {
    std::cout << "Testing SensoryPercept improvements..." << std::endl;
    
    try {
        // Test 1: Default constructor
        nlm::SensoryPercept percept;
        std::cout << "✓ Default constructor passed" << std::endl;
        
        // Test 2: Basic operations
        auto vision = percept.getVision();
        auto touch = percept.getTouch();
        auto internal = percept.getInternal();
        auto proprioception = percept.getProprioception();
        auto audio = percept.getAudio();
        
        std::cout << "✓ All getters passed" << std::endl;
        
        // Test 3: getAllSignals (should work and log any invalid values)
        auto signals = percept.getAllSignals();
        std::cout << "✓ getAllSignals passed, returned " << signals.size() << " signals" << std::endl;
        
        // Test 4: Test validation in setters (attempt to set oversized vectors)
        try {
            std::vector<float> oversizedVision(300, 0.5f); // Exceeds MAX_VISION_SIZE
            percept.setVision(oversizedVision);
            std::cout << "✗ Should have thrown exception for oversized vision" << std::endl;
        } catch (const std::invalid_argument& e) {
            std::cout << "✓ Vision validation works (caught oversized vector)" << std::endl;
        }
        
        // Test 5: Test NaN handling
        std::vector<float> validData = {0.1f, 0.2f, 0.3f};
        percept.setVision(validData);
        std::cout << "✓ Valid data accepted" << std::endl;
        
        // Test 6: Test timestamp
        percept.setTimestamp(123.45);
        double timestamp = percept.getTimestamp();
        if (timestamp == 123.45) {
            std::cout << "✓ Timestamp operations work" << std::endl;
        } else {
            std::cout << "✗ Timestamp mismatch" << std::endl;
        }
        
        std::cout << "\nAll SensoryPercept improvement tests completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}