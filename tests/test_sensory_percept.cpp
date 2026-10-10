// SensoryPercept Tests
#include "agent/SensoryPercept.hpp"
#include "test_framework_utils.h"
#include <cassert>
#include <iostream>

namespace test_sensory_percept {

void testSensoryPerceptCreation() {
    nlm::SensoryPercept percept;
    
    assert(percept.getVisionWidth() == 16);
    assert(percept.getVisionHeight() == 16);
    assert(percept.getVision().size() == 256);
    assert(percept.getTouch().size() == 8);
    assert(percept.getInternal().size() == 4);
    assert(percept.getProprioception().size() == 6);
    
    std::cout << "    testSensoryPerceptCreation passed" << std::endl;
}

void testSensoryPerceptVision() {
    nlm::SensoryPercept percept;
    
    std::vector<float> vision = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    percept.setVision(vision);
    
    assert(percept.getVision().size() == vision.size());
    assert(percept.getVision()[0] == 0.1f);
    
    std::cout << "    testSensoryPerceptVision passed" << std::endl;
}

void testSensoryPerceptTouch() {
    nlm::SensoryPercept percept;
    
    std::vector<float> touch = {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f};
    percept.setTouch(touch);
    
    assert(percept.getTouch().size() == 8);
    assert(percept.getTouch()[0] == 0.0f);
    assert(percept.getTouch()[1] == 1.0f);
    
    std::cout << "    testSensoryPerceptTouch passed" << std::endl;
}

void testSensoryPerceptInternal() {
    nlm::SensoryPercept percept;
    
    std::vector<float> internal = {0.5f, -0.5f, 0.0f, 1.0f};
    percept.setInternal(internal);
    
    assert(percept.getInternal().size() == 4);
    assert(percept.getInternal()[0] == 0.5f);
    assert(percept.getInternal()[1] == -0.5f);
    
    std::cout << "    testSensoryPerceptInternal passed" << std::endl;
}

void testSensoryPerceptProprioception() {
    nlm::SensoryPercept percept;
    
    std::vector<float> proprioception = {0.0f, 1.0f, -1.0f, 0.5f, -0.5f, 0.8f};
    percept.setProprioception(proprioception);
    
    assert(percept.getProprioception().size() == 6);
    assert(percept.getProprioception()[0] == 0.0f);
    assert(percept.getProprioception()[1] == 1.0f);
    
    std::cout << "    testSensoryPerceptProprioception passed" << std::endl;
}

void testSensoryPerceptAudio() {
    nlm::SensoryPercept percept;
    
    std::vector<float> audio = {0.2f, 0.4f, 0.6f, 0.8f};
    percept.setAudio(audio);
    
    assert(percept.getAudio().size() == 4);
    assert(percept.getAudio()[0] == 0.2f);
    assert(percept.getAudio()[3] == 0.8f);
    
    std::cout << "    testSensoryPerceptAudio passed" << std::endl;
}

void testSensoryPerceptTimestamp() {
    nlm::SensoryPercept percept;
    
    assert(percept.getTimestamp() == 0.0);
    
    percept.setTimestamp(123.45);
    assert(percept.getTimestamp() == 123.45);
    
    std::cout << "    testSensoryPerceptTimestamp passed" << std::endl;
}

void testSensoryPerceptAllSignals() {
    nlm::SensoryPercept percept;
    
    // Set up all signals
    std::vector<float> vision = {0.1f, 0.2f, 0.3f};
    percept.setVision(vision);
    
    std::vector<float> touch = {0.0f, 1.0f};
    percept.setTouch(touch);
    
    std::vector<float> internal = {0.5f, -0.5f};
    percept.setInternal(internal);
    
    std::vector<float> proprioception = {0.0f, 1.0f};
    percept.setProprioception(proprioception);
    
    std::vector<float> audio = {0.2f, 0.4f};
    percept.setAudio(audio);
    
    // Get all signals
    std::vector<float> allSignals = percept.getAllSignals();
    
    // Should contain all signal data concatenated
    size_t expectedSize = vision.size() + touch.size() + internal.size() + 
                         proprioception.size() + audio.size();
    assert(allSignals.size() == expectedSize);
    
    std::cout << "    testSensoryPerceptAllSignals passed" << std::endl;
}

void testSensoryPerceptIntegration() {
    // Test integration with other systems
    nlm::SensoryPercept percept;
    
    // Set up a complete sensory percept
    std::vector<float> vision(256, 0.5f);
    percept.setVision(vision);
    
    std::vector<float> touch(8, 0.0f);
    percept.setTouch(touch);
    
    std::vector<float> internal(4, 0.0f);
    percept.setInternal(internal);
    
    std::vector<float> proprioception(6, 0.0f);
    percept.setProprioception(proprioception);
    
    // Verify the percept is properly configured
    assert(percept.getVision().size() == 256);
    assert(percept.getTouch().size() == 8);
    assert(percept.getInternal().size() == 4);
    assert(percept.getProprioception().size() == 6);
    assert(percept.getProprioception().size() == 6);
    
    // Test timestamp
    percept.setTimestamp(100.0);
    assert(percept.getTimestamp() == 100.0);
    
    std::cout << "    testSensoryPerceptIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running SensoryPercept tests..." << std::endl;
    testSensoryPerceptCreation();
    testSensoryPerceptVision();
    testSensoryPerceptTouch();
    testSensoryPerceptInternal();
    testSensoryPerceptProprioception();
    testSensoryPerceptAudio();
    testSensoryPerceptTimestamp();
    testSensoryPerceptAllSignals();
    testSensoryPerceptIntegration();
}

} // namespace test_sensory_percept
