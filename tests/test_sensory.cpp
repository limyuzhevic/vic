// Sensory System Tests
// Tests for vision, audio, internal signals, sensory input

#include "sensory/Vision.hpp"
#include "sensory/Audio.hpp"
#include "sensory/InternalSignals.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_sensory {

void testVision() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::Vision vision(config);
    
    // Test initialization
    vision.initialize(640, 480, 3);
    
    assert(vision.getWidth() == 640);
    assert(vision.getHeight() == 480);
    assert(vision.getChannels() == 3);
    
    // Test data setting
    std::vector<float> data(640 * 480 * 3, 0.5f);
    vision.setData(data);
    
    assert(vision.getDimensions() == 3);
    
    std::cout << "    testVision passed" << std::endl;
}

void testAudio() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::Audio audio(config);
    
    // Test initialization
    audio.initialize(44100, 1024);
    
    assert(audio.getSampleRate() == 44100);
    assert(audio.getNumSamples() == 1024);
    
    // Test data setting
    std::vector<float> data(1024, 0.3f);
    audio.setData(data);
    
    std::cout << "    testAudio passed" << std::endl;
}

void testInternalSignals() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::InternalSignals signals(config);
    
    // Test basic operations
    assert(signals.getSignalCount() == 0);
    
    // Test signal adding
    signals.addSignal(0.5f);
    assert(signals.getSignalCount() == 1);
    
    // Test signal retrieval
    std::vector<float> all_signals = signals.getAllSignals();
    assert(all_signals.size() == 1);
    assert(std::abs(all_signals[0] - 0.5f) < 0.01f);
    
    // Test signal clearing
    signals.clearSignals();
    assert(signals.getSignalCount() == 0);
    
    std::cout << "    testInternalSignals passed" << std::endl;
}

void runAll() {
    testVision();
    testAudio();
    testInternalSignals();
}

} // namespace test_sensory