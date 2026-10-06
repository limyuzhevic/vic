// Sensory Tests
#include "agent/SensoryPercept.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_sensory {

void testConstructor() {
    nlm::SensoryPercept percept;
    
    // Check initial state
    assert(percept.getVision().empty());
    assert(percept.getVisionWidth() == 0);
    assert(percept.getVisionHeight() == 0);
    assert(percept.getTouch().empty());
    assert(percept.getInternal().empty());
    assert(percept.getProprioception().empty());
    assert(percept.getAudio().empty());
    assert(percept.getTimestamp() == 0.0);
    
    // Check all signals concatenated
    std::vector<float> allSignals = percept.getAllSignals();
    assert(allSignals.empty());
    
    std::cout << "    testConstructor passed" << std::endl;
}

void testSettersAndGetters() {
    nlm::SensoryPercept percept;
    
    // Set vision data
    std::vector<float> visionData = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f};
    percept.setVision(visionData);
    
    // Vision width and height are automatically calculated from the data
    // For 6 elements, width=sqrt(6)≈2, height=6/2=3
    assert(percept.getVision() == visionData);
    assert(percept.getVisionWidth() == 2);
    assert(percept.getVisionHeight() == 3);
    
    // Set touch data
    std::vector<float> touchData = {0.0f, 1.0f, 0.0f};
    percept.setTouch(touchData);
    assert(percept.getTouch() == touchData);
    
    // Set internal data
    std::vector<float> internalData = {0.5f, 0.5f};
    percept.setInternal(internalData);
    assert(percept.getInternal() == internalData);
    
    // Set proprioception data
    std::vector<float> proprioceptionData = {0.0f, 1.0f, 0.0f, 1.0f};
    percept.setProprioception(proprioceptionData);
    assert(percept.getProprioception() == proprioceptionData);
    
    // Set audio data
    std::vector<float> audioData = {0.3f, 0.7f};
    percept.setAudio(audioData);
    assert(percept.getAudio() == audioData);
    
    // Set timestamp
    percept.setTimestamp(123.45);
    assert(percept.getTimestamp() == 123.45);
    
    std::cout << "    testSettersAndGetters passed" << std::endl;
}

void testGetAllSignals() {
    nlm::SensoryPercept percept;
    
    // Set all sensory signals with different values to distinguish them
    std::vector<float> visionData = {1.0f, 2.0f, 3.0f};
    std::vector<float> touchData = {4.0f, 5.0f};
    std::vector<float> internalData = {6.0f, 7.0f, 8.0f};
    std::vector<float> proprioceptionData = {9.0f};
    std::vector<float> audioData = {10.0f};
    
    percept.setVision(visionData);
    percept.setTouch(touchData);
    percept.setInternal(internalData);
    percept.setProprioception(proprioceptionData);
    percept.setAudio(audioData);
    
    // Get all signals concatenated
    std::vector<float> allSignals = percept.getAllSignals();
    
    // Check the concatenated result
    std::vector<float> expected = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f};
    assert(allSignals == expected);
    
    std::cout << "    testGetAllSignals passed" << std::endl;
}

void testEmptySignals() {
    nlm::SensoryPercept percept;
    
    // Set some signals, leave others empty
    percept.setVision(std::vector<float>{1.0f, 2.0f});
    percept.setTouch(std::vector<float>{});  // Empty
    percept.setInternal(std::vector<float>{3.0f});
    percept.setProprioception(std::vector<float>{});  // Empty
    percept.setAudio(std::vector<float>{4.0f});
    
    std::vector<float> allSignals = percept.getAllSignals();
    std::vector<float> expected = {1.0f, 2.0f, 3.0f, 4.0f};
    
    assert(allSignals == expected);
    
    std::cout << "    testEmptySignals passed" << std::endl;
}

void runAll() {
    testConstructor();
    testSettersAndGetters();
    testGetAllSignals();
    testEmptySignals();
}

} // namespace test_sensory