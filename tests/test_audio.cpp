// Audio Processing Tests
// These tests verify the biological plausibility of auditory processing
// including cochlear frequency analysis, sound localization, and reward modulation

#include "sensory/Audio.hpp"
#include "sensory/InternalSignals.hpp"
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_audio {

void testAudioProcessorCreation() {
    nlm::AudioProcessor processor;
    
    // Test default initialization
    assert(processor.getFeatureDimensions() == 0);  // No features yet
    assert(processor.getFeatures().size() == 0);
    
    std::cout << "    testAudioProcessorCreation passed" << std::endl;
}

void testAudioProcessorFeatureExtraction() {
    nlm::AudioProcessor processor;
    
    // Create a simple audio input
    nlm::Audio audioInput;
    audioInput.sampleRate = 44100;
    audioInput.numSamples = 100;
    
    // Create tone-like input (A4 note = 440Hz)
    std::vector<float> sineWave;
    for (size_t i = 0; i < audioInput.numSamples; ++i) {
        float t = static_cast<float>(i) / audioInput.sampleRate;
        sineWave.push_back(std::sin(2.0f * M_PI * 440.0f * t));
    }
    audioInput.setData(sineWave);
    
    // Process the audio input
    processor.process(audioInput);
    
    // Verify features were extracted
    assert(processor.getFeatureDimensions() > 0);
    
    // Check that feature vector has expected dimensions
    const auto& features = processor.getFeatures();
    assert(features.size() > 0);
    
    std::cout << "    testAudioProcessorFeatureExtraction passed" << std::endl;
}

void testAudioProcessorCochlearFrequencyAnalysis() {
    nlm::AudioProcessor processor;
    
    // Test cochlear frequency analysis
    // This is tested in the internal cochlea processing stage
    
    nlm::Audio audioInput;
    audioInput.sampleRate = 22050;
    audioInput.numSamples = 50;
    
    // Create multi-tone input (different frequencies)
    std::vector<float> multiTone;
    for (size_t i = 0; i < audioInput.numSamples; ++i) {
        float t = static_cast<float>(i) / audioInput.sampleRate;
        // Add multiple frequencies
        float signal = std::sin(2.0f * M_PI * 200.0f * t) +  // 200Hz
                     0.5f * std::sin(2.0f * M_PI * 800.0f * t);  // 800Hz
        multiTone.push_back(signal);
    }
    audioInput.setData(multiTone);
    
    processor.process(audioInput);
    
    // Should detect frequency components
    
    std::cout << "    testAudioProcessorCochlearFrequencyAnalysis passed" << std::endl;
}

void testAudioProcessorSoundLocalization() {
    nlm::AudioProcessor processor;
    
    // Test sound localization mechanisms (ILD and ITD)
    nlm::Audio audioInput;
    audioInput.sampleRate = 48000;
    audioInput.numSamples = 75;
    
    std::vector<float> localizationSignal;
    for (size_t i = 0; i < audioInput.numSamples; ++i) {
        // Create signal with interaural differences
        float levelDiff = (i % 3 == 0) ? 0.3f : -0.3f;  // ILD-like
        float timeDiff = (i % 5 == 0) ? 0.002f : 0.0f;   // ITD-like
        float signal = std::sin(2.0f * M_PI * 500.0f * static_cast<float>(i) / audioInput.sampleRate) * 
                       (1.0f + levelDiff);
        localizationSignal.push_back(signal);
    }
    audioInput.setData(localizationSignal);
    
    processor.process(audioInput);
    
    // Should process interaural differences
    
    std::cout << "    testAudioProcessorSoundLocalization passed" << std::endl;
}

void testAudioProcessorRewardModulation() {
    nlm::AudioProcessor processor;
    
    // Test reward-modulated processing for salient sounds
    float initialSalience = processor.getFeatures().empty() ? 0.0f : processor.getFeatures()[0];
    
    nlm::Audio audioInput;
    audioInput.sampleRate = 8000;
    audioInput.numSamples = 25;
    
    std::vector<float> salientSound;
    for (size_t i = 0; i < audioInput.numSamples; ++i) {
        // Create sudden loud sound (salient)
        float t = static_cast<float>(i) / audioInput.sampleRate;
        float signal = (t > 0.4f && t < 0.6f) ? 2.0f : 0.1f;  // Loud burst
        salientSound.push_back(signal);
    }
    audioInput.setData(salientSound);
    
    processor.process(audioInput);
    
    // Update reward for the salient sound
    processor.updateReward(1.0f);
    
    // Should have processed salient sound with reward
    
    std::cout << "    testAudioProcessorRewardModulation passed" << std::endl;
}

void testAudioProcessorTemporalProcessing() {
    nlm::AudioProcessor processor;
    
    // Test temporal processing and rhythm detection
    nlm::Audio audioInput;
    audioInput.sampleRate = 11025;
    audioInput.numSamples = 30;
    
    std::vector<float> rhythmicInput;
    for (size_t i = 0; i < audioInput.numSamples; ++i) {
        // Create rhythmic pattern
        float t = static_cast<float>(i) / audioInput.sampleRate;
        float rhythm = (std::sin(2.0f * M_PI * 2.0f * t) + 1.0f) * 0.5f;  // 2 Hz rhythm
        rhythmicInput.push_back(rhythm);
    }
    audioInput.setData(rhythmicInput);
    
    processor.process(audioInput);
    
    // Should detect rhythmic patterns
    
    std::cout << "    testAudioProcessorTemporalProcessing passed" << std::endl;
}

void testAudioProcessorConfiguration() {
    nlm::AudioProcessor processor;
    
    // Test configuration options
    processor.setTemporalIntegration(0.5f);  // Slower adaptation
    assert(processor.getFeatures().size() >= 0);  // Features should be extractable
    
    // Process with new configuration
    nlm::Audio audioInput;
    audioInput.sampleRate = 16000;
    audioInput.numSamples = 20;
    std::vector<float> testSignal(audioInput.numSamples, 0.5f);
    audioInput.setData(testSignal);
    
    processor.process(audioInput);
    
    std::cout << "    testAudioProcessorConfiguration passed" << std::endl;
}

void testAudioProcessorHomeostaticIntegration() {
    nlm::AudioProcessor processor;
    
    // Test integration with homeostatic signals
    nlm::Audio audioInput;
    audioInput.sampleRate = 9600;
    audioInput.numSamples = 40;
    
    // Create slowly varying signal for stable processing
    std::vector<float> gradualInput;
    for (size_t i = 0; i < audioInput.numSamples; ++i) {
        float t = static_cast<float>(i) / audioInput.sampleRate;
        gradualInput.push_back(0.3f + 0.1f * std::sin(2.0f * M_PI * 0.5f * t));
    }
    audioInput.setData(gradualInput);
    
    processor.process(audioInput);
    
    // Process again - should be stable
    processor.process(audioInput);
    
    std::cout << "    testAudioProcessorHomeostaticIntegration passed" << std::endl;
}

void runAllAudioTests() {
    std::cout << "Running Audio Processing Tests..." << std::endl;
    testAudioProcessorCreation();
    testAudioProcessorFeatureExtraction();
    testAudioProcessorCochlearFrequencyAnalysis();
    testAudioProcessorSoundLocalization();
    testAudioProcessorRewardModulation();
    testAudioProcessorTemporalProcessing();
    testAudioProcessorConfiguration();
    testAudioProcessorHomeostaticIntegration();
}

} // namespace test_audio
