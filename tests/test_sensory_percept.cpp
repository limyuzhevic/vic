#include "agent/SensoryPercept.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>

namespace test_sensory_percept {

void testSensoryPerceptDefaultConstructor() {
    nlm::SensoryPercept percept;
    
    // Should initialize with default dimensions
    assert(percept.getVisionWidth() == 16);
    assert(percept.getVisionHeight() == 16);
    assert(percept.getTouchSize() == 8);
    assert(percept.getInternalSize() == 4);
    assert(percept.getProprioceptionSize() == 6);
    assert(percept.getAudioSize() == 0);
    
    // Check that vectors are properly sized
    assert(percept.getVision().size() == 256); // 16x16
    assert(percept.getTouch().size() == 8);
    assert(percept.getInternal().size() == 4);
    assert(percept.getProprioception().size() == 6);
    assert(percept.getAudio().size() == 0);
    
    // Check total input size
    assert(percept.getTotalSensoryInputSize() == 274); // 256 + 8 + 4 + 6 + 0
    
    // Should validate by default
    assert(percept.validate());
    
    std::cout << "    testSensoryPerceptDefaultConstructor passed" << std::endl;
}

void testSensoryPerceptConfigurableConstructor() {
    // Test with custom dimensions
    nlm::SensoryPercept percept(32, 24, 12, 8, 10, 5);
    
    assert(percept.getVisionWidth() == 32);
    assert(percept.getVisionHeight() == 24);
    assert(percept.getTouchSize() == 12);
    assert(percept.getInternalSize() == 8);
    assert(percept.getProprioceptionSize() == 10);
    assert(percept.getAudioSize() == 5);
    
    // Check that vectors are properly sized
    assert(percept.getVision().size() == 768); // 32x24
    assert(percept.getTouch().size() == 12);
    assert(percept.getInternal().size() == 8);
    assert(percept.getProprioception().size() == 10);
    assert(percept.getAudio().size() == 5);
    
    // Check total input size
    assert(percept.getTotalSensoryInputSize() == 803); // 768 + 12 + 8 + 10 + 5
    
    std::cout << "    testSensoryPerceptConfigurableConstructor passed" << std::endl;
}

void testSensoryPerceptConfigure() {
    nlm::SensoryPercept percept;
    
    // Configure with different dimensions
    percept.configure(64, 48, 20, 16, 14, 8);
    
    assert(percept.getVisionWidth() == 64);
    assert(percept.getVisionHeight() == 48);
    assert(percept.getTouchSize() == 20);
    assert(percept.getInternalSize() == 16);
    assert(percept.getProprioceptionSize() == 14);
    assert(percept.getAudioSize() == 8);
    
    // Check that vectors were resized correctly
    assert(percept.getVision().size() == 3072); // 64x48
    assert(percept.getTouch().size() == 20);
    assert(percept.getInternal().size() == 16);
    assert(percept.getProprioception().size() == 14);
    assert(percept.getAudio().size() == 8);
    
    std::cout << "    testSensoryPerceptConfigure passed" << std::endl;
}

void testSensoryPerceptVision() {
    nlm::SensoryPercept percept;
    
    // Test vision data
    std::vector<float> vision(256, 0.0f);
    for (size_t i = 0; i < 256; ++i) {
        vision[i] = static_cast<float>(i) / 255.0f;
    }
    
    percept.setVision(vision);
    
    const auto& retrievedVision = percept.getVision();
    assert(retrievedVision.size() == 256);
    assert(retrievedVision[0] == 0.0f);
    assert(retrievedVision[128] == 0.5f);
    assert(retrievedVision[255] == 1.0f);
    
    std::cout << "    testSensoryPerceptVision passed" << std::endl;
}

void testSensoryPerceptTouch() {
    nlm::SensoryPercept percept(32, 32, 16, 12, 10, 5);
    
    std::vector<float> touch(16, 0.0f);
    touch[0] = 0.1f;
    touch[7] = 0.9f;
    touch[15] = 0.5f;
    
    percept.setTouch(touch);
    
    const auto& retrievedTouch = percept.getTouch();
    assert(retrievedTouch.size() == 16);
    assert(retrievedTouch[0] == 0.1f);
    assert(retrievedTouch[7] == 0.9f);
    assert(retrievedTouch[15] == 0.5f);
    
    std::cout << "    testSensoryPerceptTouch passed" << std::endl;
}

void testSensoryPerceptInternal() {
    nlm::SensoryPercept percept(32, 32, 16, 12, 10, 5);
    
    std::vector<float> internal(12, 0.0f);
    for (size_t i = 0; i < 12; ++i) {
        internal[i] = static_cast<float>(i) / 11.0f;
    }
    
    percept.setInternal(internal);
    
    const auto& retrievedInternal = percept.getInternal();
    assert(retrievedInternal.size() == 12);
    assert(retrievedInternal[0] == 0.0f);
    assert(retrievedInternal[11] == 1.0f);
    
    std::cout << "    testSensoryPerceptInternal passed" << std::endl;
}

void testSensoryPerceptProprioception() {
    nlm::SensoryPercept percept(32, 32, 16, 12, 10, 5);
    
    std::vector<float> proprioception(10, 0.5f);
    
    percept.setProprioception(proprioception);
    
    const auto& retrievedProprioception = percept.getProprioception();
    assert(retrievedProprioception.size() == 10);
    assert(retrievedProprioception[0] == 0.5f);
    assert(retrievedProprioception[9] == 0.5f);
    
    std::cout << "    testSensoryPerceptProprioception passed" << std::endl;
}

void testSensoryPerceptAudio() {
    nlm::SensoryPercept percept(32, 32, 16, 12, 10, 5);
    
    std::vector<float> audio(5, 0.0f);
    audio[0] = 0.8f;
    audio[4] = 0.2f;
    
    percept.setAudio(audio);
    
    const auto& retrievedAudio = percept.getAudio();
    assert(retrievedAudio.size() == 5);
    assert(retrievedAudio[0] == 0.8f);
    assert(retrievedAudio[4] == 0.2f);
    
    std::cout << "    testSensoryPerceptAudio passed" << std::endl;
}

void testSensoryPerceptGetAllSignals() {
    nlm::SensoryPercept percept(8, 8, 4, 2, 3, 1);
    
    std::vector<float> vision(64, 0.1f);
    std::vector<float> touch(4, 0.2f);
    std::vector<float> internal(2, 0.3f);
    std::vector<float> proprioception(3, 0.4f);
    std::vector<float> audio(1, 0.5f);
    
    percept.setVision(vision);
    percept.setTouch(touch);
    percept.setInternal(internal);
    percept.setProprioception(proprioception);
    percept.setAudio(audio);
    
    std::vector<float> allSignals = percept.getAllSignals();
    
    // Expected: vision (64) + touch (4) + internal (2) + proprioception (3) + audio (1) = 74
    assert(allSignals.size() == 74);
    
    // Check that vision signals come first
    for (size_t i = 0; i < 64; ++i) {
        assert(allSignals[i] == 0.1f);
    }
    
    // Check that touch signals come after vision
    assert(allSignals[64] == 0.2f);
    assert(allSignals[65] == 0.2f);
    assert(allSignals[66] == 0.2f);
    assert(allSignals[67] == 0.2f);
    
    std::cout << "    testSensoryPerceptGetAllSignals passed" << std::endl;
}

void testSensoryPerceptValidation() {
    nlm::SensoryPercept percept;
    
    // Should validate by default
    assert(percept.validate());
    
    // Configure custom sizes
    percept.configure(16, 16, 8, 4, 6, 2);
    
    // Should still validate
    assert(percept.validate());
    
    // Test with mismatched sizes (this should fail validation)
    nlm::SensoryPercept percept2(16, 16, 8, 4, 6, 2);
    percept2.setVision(std::vector<float>(300, 0.5f)); // Wrong size for 16x16
    
    // Should fail validation
    assert(!percept2.validate());
    
    std::cout << "    testSensoryPerceptValidation passed" << std::endl;
}

void testSensoryPerceptTimestamp() {
    nlm::SensoryPercept percept;
    
    assert(percept.getTimestamp() == 0.0);
    
    percept.setTimestamp(123.456);
    assert(percept.getTimestamp() == 123.456);
    
    std::cout << "    testSensoryPerceptTimestamp passed" << std::endl;
}

void testSensoryPerceptVisionArea() {
    nlm::SensoryPercept percept(10, 20);
    
    assert(percept.getVisionWidth() == 10);
    assert(percept.getVisionHeight() == 20);
    assert(percept.getVisionArea() == 200); // 10 * 20
    
    std::cout << "    testSensoryPerceptVisionArea passed" << std::endl;
}

void testSensoryPerceptTotalSensoryInputSize() {
    nlm::SensoryPercept percept;
    
    // Default configuration
    assert(percept.getTotalSensoryInputSize() == 274); // 256 + 8 + 4 + 6 + 0
    
    // Custom configuration
    nlm::SensoryPercept percept2(32, 24, 16, 12, 10, 8);
    assert(percept2.getTotalSensoryInputSize() == 803); // 768 + 16 + 12 + 10 + 8
    
    std::cout << "    testSensoryPerceptTotalSensoryInputSize passed" << std::endl;
}

void testSensoryPerceptResizeVectors() {
    nlm::SensoryPercept percept;
    
    // Add some data
    std::vector<float> vision = percept.getVision();
    for (size_t i = 0; i < vision.size(); ++i) {
        vision[i] = static_cast<float>(i);
    }
    percept.setVision(vision);
    
    // Configure new sizes
    percept.configure(8, 8, 4, 2, 3, 1);
    
    // Check that vectors were resized
    assert(percept.getVision().size() == 64); // 8x8
    assert(percept.getTouch().size() == 4);
    assert(percept.getInternal().size() == 2);
    assert(percept.getProprioception().size() == 3);
    assert(percept.getAudio().size() == 1);
    
    // Check that original data was preserved (up to the new size)
    for (size_t i = 0; i < std::min<size_t>(vision.size(), 64); ++i) {
        assert(percept.getVision()[i] == static_cast<float>(i));
    }
    
    std::cout << "    testSensoryPerceptResizeVectors passed" << std::endl;
}

void testSensoryPerceptConstructorVariadic() {
    // Test the variadic constructor for different combinations
    nlm::SensoryPercept percept1(10, 10);
    assert(percept1.getVisionWidth() == 10);
    assert(percept1.getVisionHeight() == 10);
    
    nlm::SensoryPercept percept2(10, 10, 8);
    assert(percept2.getVisionWidth() == 10);
    assert(percept2.getVisionHeight() == 10);
    assert(percept2.getTouchSize() == 8);
    
    nlm::SensoryPercept percept3(10, 10, 8, 4);
    assert(percept3.getInternalSize() == 4);
    
    nlm::SensoryPercept percept4(10, 10, 8, 4, 6);
    assert(percept4.getProprioceptionSize() == 6);
    
    nlm::SensoryPercept percept5(10, 10, 8, 4, 6, 2);
    assert(percept5.getAudioSize() == 2);
    
    std::cout << "    testSensoryPerceptConstructorVariadic passed" << std::endl;
}

void runAll() {
    testSensoryPerceptDefaultConstructor();
    testSensoryPerceptConfigurableConstructor();
    testSensoryPerceptConfigure();
    testSensoryPerceptVision();
    testSensoryPerceptTouch();
    testSensoryPerceptInternal();
    testSensoryPerceptProprioception();
    testSensoryPerceptAudio();
    testSensoryPerceptGetAllSignals();
    testSensoryPerceptValidation();
    testSensoryPerceptTimestamp();
    testSensoryPerceptVisionArea();
    testSensoryPerceptTotalSensoryInputSize();
    testSensoryPerceptResizeVectors();
    testSensoryPerceptConstructorVariadic();
}

} // namespace test_sensory_percept
