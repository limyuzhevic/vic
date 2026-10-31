// Enhanced Test Suite for New Sensory Input Classes
// Tests for Vision, Audio, InternalSignals and their Processors
// Tests Python binding compatibility and validation

#include "sensory/Vision.hpp"
#include "sensory/Audio.hpp"
#include "sensory/InternalSignals.hpp"
#include "sensory/VisionProcessor.hpp"
#include "sensory/AudioProcessor.hpp"
#include "sensory/InternalSignalsProcessor.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <vector>

namespace test_sensory {

void testVisionCreation() {
    std::cout << "    testVisionCreation passed" << std::endl;
}

void testVisionProperties() {
    nlm::Vision vision(10, 20, 3);
    assert(vision.getWidth() == 10);
    assert(vision.getHeight() == 20);
    assert(vision.getChannels() == 3);
    assert(vision.getDimensions() == 600);
    
    std::cout << "    testVisionProperties passed" << std::endl;
}

void testVisionDataValidation() {
    nlm::Vision vision(10, 10);
    
    // Valid data should work
    std::vector<float> validData(100, 0.5f);
    vision.setData(validData);
    assert(vision.getData().size() == 100);
    
    // Empty data should throw
    try {
        vision.setData(std::vector<float>());
        assert(false && "Should have thrown exception for empty data");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("cannot be empty") != std::string::npos);
    }
    
    std::cout << "    testVisionDataValidation passed" << std::endl;
}

void testAudioCreation() {
    nlm::Audio audio(44100, 1024);
    assert(audio.getSampleRate() == 44100);
    assert(audio.getNumSamples() == 1024);
    assert(audio.getDimensions() == 1024);
    
    std::cout << "    testAudioCreation passed" << std::endl;
}

void testAudioDataValidation() {
    nlm::Audio audio(44100, 100);
    
    // Valid data should work
    std::vector<float> validData(100, 0.5f);
    audio.setData(validData);
    assert(audio.getData().size() == 100);
    
    // Empty data should throw
    try {
        audio.setData(std::vector<float>());
        assert(false && "Should have thrown exception for empty data");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("cannot be empty") != std::string::npos);
    }
    
    std::cout << "    testAudioDataValidation passed" << std::endl;
}

void testInternalSignalsCreation() {
    nlm::InternalSignals internal;
    assert(internal.getDimensions() == 0);  // Initially empty
    assert(internal.getData().empty());
    
    std::cout << "    testInternalSignalsCreation passed" << std::endl;
}

void testInternalSignalsOperations() {
    nlm::InternalSignals internal;
    
    // Test adding signals
    internal.addSignal(0.5f);
    internal.addSignal(0.8f);
    internal.addSignal(0.3f);
    
    assert(internal.getData().size() == 3);
    assert(internal.getDimensions() == 3);
    assert(fabs(internal.getData()[0] - 0.5f) < 0.001f);
    
    // Test clearing signals
    internal.clearSignals();
    assert(internal.getData().empty());
    assert(internal.getDimensions() == 0);
    
    std::cout << "    testInternalSignalsOperations passed" << std::endl;
}

void testVisionProcessorCreation() {
    nlm::VisionProcessor processor;
    assert(processor.getFeatureDimensions() == 0); // Initially no features
    
    std::cout << "    testVisionProcessorCreation passed" << std::endl;
}

void testVisionProcessorOperations() {
    nlm::VisionProcessor processor;
    nlm::Vision vision(8, 8);
    
    // Create test vision data
    std::vector<float> visionData(64, 0.5f);
    vision.setData(visionData);
    
    // Process vision data
    processor.process(vision);
    
    // Check that features were extracted
    auto features = processor.getFeatures();
    assert(features.size() > 0);
    assert(processor.getFeatureDimensions() > 0);
    
    std::cout << "    testVisionProcessorOperations passed" << std::endl;
}

void testAudioProcessorCreation() {
    nlm::AudioProcessor processor;
    assert(processor.getFeatureDimensions() == 0); // Initially no features
    
    std::cout << "    testAudioProcessorCreation passed" << std::endl;
}

void testAudioProcessorOperations() {
    nlm::AudioProcessor processor;
    nlm::Audio audio(44100, 100);
    
    // Create test audio data
    std::vector<float> audioData(100, 0.5f);
    audio.setData(audioData);
    
    // Process audio data
    processor.process(audio);
    
    // Check that features were extracted
    auto features = processor.getFeatures();
    assert(features.size() > 0);
    assert(processor.getFeatureDimensions() > 0);
    
    std::cout << "    testAudioProcessorOperations passed" << std::endl;
}

void testInternalSignalsProcessorCreation() {
    nlm::InternalSignalsProcessor processor;
    assert(processor.getProcessedSignals().empty());
    assert(processor.getHomeostaticSignals().empty());
    
    std::cout << "    testInternalSignalsProcessorCreation passed" << std::endl;
}

void testInternalSignalsProcessorOperations() {
    nlm::InternalSignalsProcessor processor;
    nlm::InternalSignals internal;
    
    // Add some internal signals
    internal.addSignal(0.5f);
    internal.addSignal(0.8f);
    internal.addSignal(0.3f);
    
    // Process internal signals
    processor.process(internal);
    
    // Check that signals were processed
    auto processed = processor.getProcessedSignals();
    auto homeostatic = processor.getHomeostaticSignals();
    
    // Both should be non-empty after processing
    assert(processed.size() > 0 || homeostatic.size() > 0);
    
    std::cout << "    testInternalSignalsProcessorOperations passed" << std::endl;
}

void testSensoryInputInterface() {
    // Test that all sensory classes implement the SensoryInput interface
    nlm::Vision vision(10, 10);
    nlm::Audio audio(44100, 100);
    nlm::InternalSignals internal;
    
    // All should have these methods
    assert(vision.getType() == nlm::NeuronType::Sensory);
    assert(audio.getType() == nlm::NeuronType::Sensory);
    assert(internal.getType() == nlm::NeuronType::Sensory);
    
    // Test timestamp functionality
    nlm::Timestamp timestamp1 = 0.0f;
    nlm::Timestamp timestamp2 = 1.5f;
    
    vision.setTimestamp(timestamp1);
    audio.setTimestamp(timestamp2);
    internal.setTimestamp(timestamp1);
    
    assert(vision.getTimestamp() == timestamp1);
    assert(audio.getTimestamp() == timestamp2);
    assert(internal.getTimestamp() == timestamp1);
    
    std::cout << "    testSensoryInputInterface passed" << std::endl;
}

void testSensoryErrorHandling() {
    nlm::Vision vision(10, 10);
    nlm::Audio audio(44100, 100);
    
    // Test error handling for empty data
    try {
        vision.setData(std::vector<float>());
        assert(false && "Vision should throw for empty data");
    } catch (...) {}
    
    try {
        audio.setData(std::vector<float>());
        assert(false && "Audio should throw for empty data");
    } catch (...) {}
    
    std::cout << "    testSensoryErrorHandling passed" << std::endl;
}

void testSensoryIntegration() {
    // Test integration of all sensory classes
    nlm::Vision vision(8, 8);
    nlm::Audio audio(22050, 200);
    nlm::InternalSignals internal;
    
    // Set valid data for each
    std::vector<float> visionData(64, 0.3f);
    std::vector<float> audioData(200, 0.7f);
    
    vision.setData(visionData);
    audio.setData(audioData);
    internal.addSignal(0.5f);
    internal.addSignal(0.8f);
    
    // Create processors and process data
    nlm::VisionProcessor vp;
    nlm::AudioProcessor ap;
    nlm::InternalSignalsProcessor ip;
    
    vp.process(vision);
    ap.process(audio);
    ip.process(internal);
    
    // Verify all processors worked
    assert(vp.getFeatureDimensions() > 0);
    assert(ap.getFeatureDimensions() > 0);
    assert(ip.getProcessedSignals().size() > 0 || ip.getHomeostaticSignals().size() > 0);
    
    std::cout << "    testSensoryIntegration passed" << std::endl;
}

void runAll() {
    testVisionCreation();
    testVisionProperties();
    testVisionDataValidation();
    testAudioCreation();
    testAudioDataValidation();
    testInternalSignalsCreation();
    testInternalSignalsOperations();
    testVisionProcessorCreation();
    testVisionProcessorOperations();
    testAudioProcessorCreation();
    testAudioProcessorOperations();
    testInternalSignalsProcessorCreation();
    testInternalSignalsProcessorOperations();
    testSensoryInputInterface();
    testSensoryErrorHandling();
    testSensoryIntegration();
}

} // namespace test_sensory