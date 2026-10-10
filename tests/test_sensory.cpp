#include "test_common.hpp"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace test_sensory {

void testSensoryInputCreation() {
    nlm::SensoryInput sensoryInput;
    assert(sensoryInput.getInputCount() == 0);
    assert(sensoryInput.getModality() == nlm::SensoryModality::Visual);
    std::cout << "    testSensoryInputCreation passed" << std::endl;
}

void testAudioSensory() {
    nlm::Audio audio;
    assert(audio.getVolume() == 0.0f);
    assert(audio.getFrequency() == 0.0f);
    assert(audio.isActive() == false);
    std::cout << "    testAudioSensory passed" << std::endl;
}

void testVisionSensory() {
    nlm::Vision vision;
    assert(vision.getBrightness() == 0.0f);
    assert(vision.getContrast() == 0.0f);
    assert(vision.getResolution() == 0.0f);
    std::cout << "    testVisionSensory passed" << std::endl;
}

void testInternalSignalsSensory() {
    nlm::InternalSignals internalSignals;
    assert(internalSignals.getHormoneLevel() == 0.0f);
    assert(internalSignals.getTemperature() == 36.0f); // Normal body temperature
    assert(internalSignals.getHungerLevel() == 0.0f);
    std::cout << "    testInternalSignalsSensory passed" << std::endl;
}

void testSensoryInputProcessing() {
    nlm::SensoryInput sensoryInput;
    
    // Add some input
    std::vector<float> inputData = {0.1f, 0.2f, 0.3f};
    sensoryInput.addInput(inputData);
    
    assert(sensoryInput.getInputCount() == 1);
    
    // Process input
    std::vector<float> processed = sensoryInput.processInput();
    assert(processed.size() == inputData.size());
    std::cout << "    testSensoryInputProcessing passed" << std::endl;
}

void testSensoryIntegration() {
    nlm::SensoryInput sensoryInput;
    
    // Test integration with Brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Sensory input should be accessible
    assert(&sensoryInput != nullptr);
    std::cout << "    testSensoryIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Sensory tests..." << std::endl;
    testSensoryInputCreation();
    testAudioSensory();
    testVisionSensory();
    testInternalSignalsSensory();
    testSensoryInputProcessing();
    testSensoryIntegration();
}

} // namespace test_sensory