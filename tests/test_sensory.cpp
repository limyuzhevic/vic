// Sensory classes tests - Vision, Audio, InternalSignals
#include "sensory/Vision.hpp"
#include "sensory/Audio.hpp"
#include "sensory/InternalSignals.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_sensory {

void testVisionFromArray() {
    std::cout << "    Testing Vision.from_array()..." << std::endl;
    
    // Test Vision creation with different configurations
    nlm::Vision vision1;
    
    nlm::Vision vision2(84, 84, 1); // Grayscale
    assert(vision2.getWidth() == 84);
    assert(vision2.getHeight() == 84);
    assert(vision2.getChannels() == 1);
    
    nlm::Vision vision3(128, 128, 3); // RGB
    assert(vision3.getWidth() == 128);
    assert(vision3.getHeight() == 128);
    assert(vision3.getChannels() == 3);
    
    // Test setting data from array
    std::vector<float> testData(84 * 84, 0.5f);
    vision2.setData(testData);
    
    assert(vision2.getData().size() == testData.size());
    for (size_t i = 0; i < testData.size(); ++i) {
        assert(vision2.getData()[i] == testData[i]);
    }
    
    // Test dimension checking
    assert(vision2.getDimensions() == 84 * 84);
    assert(vision3.getDimensions() == 128 * 128 * 3);
    
    std::cout << "    testVisionFromArray passed" << std::endl;
}

void testAudioFromArray() {
    std::cout << "    Testing Audio.from_array()..." << std::endl;
    
    // Test Audio creation with sample rate and number of samples
    nlm::Audio audio1(44100, 44100); // 1 second of audio at 44.1kHz
    assert(audio1.getSampleRate() == 44100);
    assert(audio1.getNumSamples() == 44100);
    
    nlm::Audio audio2(16000, 8000); // 0.5 second at 16kHz
    assert(audio2.getSampleRate() == 16000);
    assert(audio2.getNumSamples() == 8000);
    
    // Test setting audio data
    std::vector<float> audioSamples(44100, 0.0f);
    for (int i = 0; i < 44100; ++i) {
        audioSamples[i] = 0.5f * sin(2 * 3.14159 * 440.0 * i / 44100.0); // 440Hz sine wave
    }
    
    audio1.setData(audioSamples);
    assert(audio1.getData().size() == audioSamples.size());
    for (size_t i = 0; i < audioSamples.size(); ++i) {
        assert(audio1.getData()[i] == audioSamples[i]);
    }
    
    // Test sample rate setting
    audio1.setSampleRate(48000);
    assert(audio1.getSampleRate() == 48000);
    
    std::cout << "    testAudioFromArray passed" << std::endl;
}

void testInternalSignalsFromDict() {
    std::cout << "    Testing InternalSignals.from_dict()..." << std::endl;
    
    nlm::InternalSignals signals;
    
    // Add signals
    signals.addSignal(0.1f);
    signals.addSignal(0.5f);
    signals.addSignal(0.9f);
    
    assert(signals.getData().size() == 3);
    assert(signals.getData()[0] == 0.1f);
    assert(signals.getData()[1] == 0.5f);
    assert(signals.getData()[2] == 0.9f);
    
    // Test clearing signals
    signals.clearSignals();
    assert(signals.getData().size() == 0);
    
    // Add more signals
    signals.addSignal(-0.3f);
    signals.addSignal(1.2f);
    
    assert(signals.getData().size() == 2);
    assert(signals.getData()[0] == -0.3f);
    assert(signals.getData()[1] == 1.2f);
    
    std::cout << "    testInternalSignalsFromDict passed" << std::endl;
}

void testSensoryArrayManipulation() {
    std::cout << "    Testing sensory array manipulation..." << std::endl;
    
    // Test Vision array operations
    nlm::Vision vision(10, 10, 1);
    std::vector<float> data(100, 0.0f);
    for (int i = 0; i < 100; ++i) {
        data[i] = static_cast<float>(i) / 100.0f;
    }
    vision.setData(data);
    
    // Test normalization
    const std::vector<float>& visionData = vision.getData();
    float minVal = visionData[0];
    float maxVal = visionData[0];
    for (float val : visionData) {
        minVal = std::min(minVal, val);
        maxVal = std::max(maxVal, val);
    }
    
    // Values should be in expected range (0.0 to 0.99)
    for (float val : visionData) {
        assert(val >= 0.0f && val <= 1.0f);
    }
    
    // Test reshaping through constructor
    nlm::Vision reshapedVision(5, 20, 1); // 5 * 20 = 100 pixels
    assert(reshapedVision.getWidth() == 5);
    assert(reshapedVision.getHeight() == 20);
    assert(reshapedVision.getChannels() == 1);
    
    std::cout << "    testSensoryArrayManipulation passed" << std::endl;
}

void testChannelAccess() {
    std::cout << "    Testing channel access..." << std::endl;
    
    // Test Vision channels
    nlm::Vision rgbVision(10, 10, 3);
    assert(rgbVision.getChannels() == 3);
    
    nlm::Vision grayscaleVision(10, 10, 1);
    assert(grayscaleVision.getChannels() == 1);
    
    // Test Audio channels (mono)
    nlm::Audio monoAudio(44100, 1024);
    assert(monoAudio.getNumSamples() == 1024);
    
    // Test InternalSignals (no channel concept but has multiple signals)
    nlm::InternalSignals signals;
    signals.addSignal(0.1f);
    signals.addSignal(0.2f);
    signals.addSignal(0.3f);
    
    assert(signals.getData().size() == 3);
    
    std::cout << "    testChannelAccess passed" << std::endl;
}

void runAll() {
    testVisionFromArray();
    testAudioFromArray();
    testInternalSignalsFromDict();
    testSensoryArrayManipulation();
    testChannelAccess();
    std::cout << "=== Sensory Tests PASSED ===" << std::endl;
}

} // namespace test_sensory