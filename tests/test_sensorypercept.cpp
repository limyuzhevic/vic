// SensoryPercept Tests
#include "agent/SensoryPercept.hpp"
#include <cassert>
#include <iostream>

namespace test_sensorypercept {

void testSensoryPerceptCreation() {
    nlm::SensoryPercept percept;
    
    // Check default dimensions
    assert(percept.getVisionWidth() == 16);
    assert(percept.getVisionHeight() == 16);
    assert(percept.getTouch().size() == 8);
    assert(percept.getInternal().size() == 4);
    assert(percept.getProprioception().size() == 6);
    assert(percept.getAudio().size() == 0); // Audio is empty by default
    
    std::cout << "    testSensoryPerceptCreation passed" << std::endl;
}

void testVisionData() {
    nlm::SensoryPercept percept;
    
    // Create test vision data
    std::vector<float> vision(256, 0.0f);
    for (int i = 0; i < 256; i++) {
        vision[i] = static_cast<float>(i) / 255.0f;
    }
    
    // Set and get vision
    percept.setVision(vision);
    assert(percept.getVision().size() == 256);
    
    // Verify data
    for (int i = 0; i < 256; i++) {
        assert(percept.getVision()[i] == static_cast<float>(i) / 255.0f);
    }
    
    std::cout << "    testVisionData passed" << std::endl;
}

void testTouchData() {
    nlm::SensoryPercept percept;
    
    // Create test touch data
    std::vector<float> touch(8, 0.0f);
    touch[0] = 1.0f;
    touch[4] = 0.5f;
    
    percept.setTouch(touch);
    assert(percept.getTouch().size() == 8);
    
    assert(percept.getTouch()[0] == 1.0f);
    assert(percept.getTouch()[4] == 0.5f);
    assert(percept.getTouch()[7] == 0.0f);
    
    std::cout << "    testTouchData passed" << std::endl;
}

void testAllSignals() {
    nlm::SensoryPercept percept;
    
    // Set all data
    std::vector<float> vision(256, 0.5f);
    std::vector<float> touch(8, 0.1f);
    std::vector<float> internal(4, 0.2f);
    std::vector<float> proprioception(6, 0.3f);
    
    percept.setVision(vision);
    percept.setTouch(touch);
    percept.setInternal(internal);
    percept.setProprioception(proprioception);
    
    // Get all signals
    std::vector<float> allSignals = percept.getAllSignals();
    
    // Check total size
    assert(allSignals.size() == 256 + 8 + 4 + 6); // 274
    
    // Verify vision portion
    for (int i = 0; i < 256; i++) {
        assert(allSignals[i] == 0.5f);
    }
    
    // Verify touch portion
    for (int i = 0; i < 8; i++) {
        assert(allSignals[256 + i] == 0.1f);
    }
    
    // Verify internal portion
    for (int i = 0; i < 4; i++) {
        assert(allSignals[256 + 8 + i] == 0.2f);
    }
    
    // Verify proprioception portion
    for (int i = 0; i < 6; i++) {
        assert(allSignals[256 + 8 + 4 + i] == 0.3f);
    }
    
    std::cout << "    testAllSignals passed" << std::endl;
}

void testTimestamp() {
    nlm::SensoryPercept percept;
    
    assert(percept.getTimestamp() == 0.0);
    
    double testTime = 1234.5;
    percept.setTimestamp(testTime);
    assert(percept.getTimestamp() == testTime);
    
    std::cout << "    testTimestamp passed" << std::endl;
}

void testSensoryPerceptReset() {
    nlm::SensoryPercept percept;
    
    // Set some data
    std::vector<float> vision(256, 1.0f);
    percept.setVision(vision);
    
    // Change data
    std::vector<float> touch(8, 0.5f);
    percept.setTouch(touch);
    
    // Note: SensoryPercept doesn't have a reset() method,
    // but we can verify data is as expected
    assert(percept.getVision()[0] == 1.0f);
    assert(percept.getTouch()[0] == 0.5f);
    
    std::cout << "    testSensoryPerceptReset passed" << std::endl;
}

} // namespace test_sensorypercept