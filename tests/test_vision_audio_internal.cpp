#include "Vision.hpp"
#include "Audio.hpp"
#include "InternalSignals.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

namespace nlm {

void test_vision_system() {
    std::cout << "Testing Vision System (Phase 4)" << std::endl;
    std::cout << "================================" << std::endl;
    
    // Test 1: Basic vision creation and properties
    std::cout << "Test 1: Basic vision creation..." << std::endl;
    std::vector<float> visionData(128 * 96 * 3, 0.5f);
    Vision visionInput(visionData, 128, 96, 3);
    
    assert(visionInput.getWidth() == 128);
    assert(visionInput.getHeight() == 96);
    assert(visionInput.getChannels() == 3);
    assert(visionInput.getQuality() >= 0.0f);
    assert(visionInput.getLuminance() >= 0.0f);
    assert(visionInput.getContrast() >= 0.0f);
    
    std::cout << "✓ Basic vision creation passed" << std::endl;
    
    // Test 2: Feature extraction
    std::cout << "Test 2: Feature extraction..." << std::endl;
    std::vector<float> features = visionInput.extractFeatures();
    assert(features.size() > 0);
    
    // Test 3: Data manipulation
    std::cout << "Test 3: Data manipulation..." << std::endl;
    std::vector<float> newData = visionInput.getData();
    assert(newData.size() == visionData.size());
    
    visionInput.setData(visionData);
    std::cout << "✓ Data manipulation passed" << std::endl;
    
    // Test 4: Clone functionality
    std::cout << "Test 4: Clone functionality..." << std::endl;
    Vision clonedVision = visionInput.clone();
    assert(clonedVision.getWidth() == visionInput.getWidth());
    assert(clonedVision.getHeight() == visionInput.getHeight());
    assert(clonedVision.getChannels() == visionInput.getChannels());
    std::cout << "✓ Clone functionality passed" << std::endl;
    
    // Test 5: Fusion methods
    std::cout << "Test 5: Fusion methods..." << std::endl;
    Audio audioInput(std::vector<float>(44100, 0.3f), 44100);
    InternalSignals internalInput;
    
    visionInput.fuseWithAudio(audioInput);
    visionInput.fuseWithInternalSignals(internalInput);
    
    audioInput.fuseWithVision(visionInput);
    audioInput.fuseWithInternalSignals(internalInput);
    
    internalInput.fuseWithVision(visionInput);
    internalInput.fuseWithAudio(audioInput);
    
    std::cout << "✓ Fusion methods passed" << std::endl;
    
    // Test 6: Update with delta time
    std::cout << "Test 6: Update with delta time..." << std::endl;
    float deltaTime = 0.016f;  // 60 Hz
    visionInput.update(deltaTime);
    
    float quality1 = visionInput.getQuality();
    visionInput.update(deltaTime);
    float quality2 = visionInput.getQuality();
    
    assert(quality1 >= 0.0f);
    assert(quality2 >= 0.0f);
    std::cout << "✓ Update with delta time passed" << std::endl;
    
    // Test 7: Edge cases
    std::cout << "Test 7: Edge cases..." << std::endl;
    
    // Empty data
    Vision emptyVision({}, 1, 1, 1);
    assert(emptyVision.getQuality() >= 0.0f);
    
    // Single pixel
    Vision singlePixelVision({1.0f}, 1, 1, 1);
    assert(singlePixelVision.getQuality() >= 0.0f);
    
    // Large dimensions
    std::vector<float> largeData(1000 * 1000 * 3, 0.2f);
    Vision largeVision(largeData, 1000, 1000, 3);
    assert(largeVision.getWidth() == 1000);
    assert(largeVision.getHeight() == 1000);
    
    std::cout << "✓ Edge cases passed" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Vision System tests PASSED! ✓" << std::endl;
    std::cout << "================================" << std::endl;
}

void test_audio_system() {
    std::cout << "Testing Audio System (Phase 4)" << std::endl;
    std::cout << "===============================" << std::endl;
    
    // Test 1: Basic audio creation and properties
    std::cout << "Test 1: Basic audio creation..." << std::endl;
    std::vector<float> audioData(44100, 0.3f);
    Audio audioInput(audioData, 44100);
    
    assert(audioInput.getSampleRate() == 44100);
    assert(audioInput.getNumSamples() == 44100);
    assert(audioInput.getDuration() > 0.0f);
    assert(audioInput.getQuality() >= 0.0f);
    assert(audioInput.getEnergy() >= 0.0f);
    assert(audioInput.getPitch() >= 0.0f);
    
    std::cout << "✓ Basic audio creation passed" << std::endl;
    
    // Test 2: Feature extraction
    std::cout << "Test 2: Feature extraction..." << std::endl;
    std::vector<float> spectrum = audioInput.extractSpectralFeatures();
    assert(spectrum.size() > 0);
    
    // Test 3: Data manipulation
    std::cout << "Test 3: Data manipulation..." << std::endl;
    std::vector<float> newData = audioInput.getData();
    assert(newData.size() == audioData.size());
    
    audioInput.setData(audioData);
    std::cout << "✓ Data manipulation passed" << std::endl;
    
    // Test 4: Clone functionality
    std::cout << "Test 4: Clone functionality..." << std::endl;
    Audio clonedAudio = audioInput.clone();
    assert(clonedAudio.getSampleRate() == audioInput.getSampleRate());
    assert(clonedAudio.getNumSamples() == audioInput.getNumSamples());
    std::cout << "✓ Clone functionality passed" << std::endl;
    
    // Test 5: Fusion methods
    std::cout << "Test 5: Fusion methods..." << std::endl;
    Vision visionInput(std::vector<float>(128 * 96 * 3, 0.5f), 128, 96, 3);
    InternalSignals internalInput;
    
    audioInput.fuseWithVision(visionInput);
    audioInput.fuseWithInternalSignals(internalInput);
    
    visionInput.fuseWithAudio(audioInput);
    internalInput.fuseWithAudio(audioInput);
    
    std::cout << "✓ Fusion methods passed" << std::endl;
    
    // Test 6: Update with delta time
    std::cout << "Test 6: Update with delta time..." << std::endl;
    float deltaTime = 0.016f;  // 60 Hz
    audioInput.update(deltaTime);
    
    float energy1 = audioInput.getEnergy();
    audioInput.update(deltaTime);
    float energy2 = audioInput.getEnergy();
    
    assert(energy1 >= 0.0f);
    assert(energy2 >= 0.0f);
    std::cout << "✓ Update with delta time passed" << std::endl;
    
    // Test 7: Edge cases
    std::cout << "Test 7: Edge cases..." << std::endl;
    
    // Empty data
    Audio emptyAudio({}, 44100);
    assert(emptyAudio.getDuration() > 0.0f);
    
    // Single sample
    Audio singleSampleAudio({1.0f}, 44100);
    assert(singleSampleAudio.getNumSamples() == 1);
    
    // Different sample rates
    Audio audio8k(std::vector<float>(8800, 0.1f), 8800);
    Audio audio16k(std::vector<float>(16800, 0.2f), 16800);
    assert(audio8k.getSampleRate() == 8800);
    assert(audio16k.getSampleRate() == 16800);
    
    std::cout << "✓ Edge cases passed" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Audio System tests PASSED! ✓" << std::endl;
    std::cout << "===============================" << std::endl;
}

void test_internal_signals_system() {
    std::cout << "Testing Internal Signals System (Phase 4)" << std::endl;
    std::cout << "============================================" << std::endl;
    
    // Test 1: Basic internal signals creation
    std::cout << "Test 1: Basic internal signals creation..." << std::endl;
    InternalSignals internalInput;
    
    assert(internalInput.getMetabolicState() >= 0.0f);
    assert(internalInput.getEmotionalState() >= 0.0f);
    assert(internalInput.getStressLevel() >= 0.0f);
    assert(internalInput.getAttentionLevel() >= 0.0f);
    assert(internalInput.getEnergyLevel() >= 0.0f);
    
    std::cout << "✓ Basic internal signals creation passed" << std::endl;
    
    // Test 2: Signal addition
    std::cout << "Test 2: Signal addition..." << std::endl;
    internalInput.addMetabolicSignal("heart_rate", 1.2f);
    internalInput.addEmotionalSignal("joy", 0.8f);
    internalInput.addCognitiveSignal("attention", 0.7f);
    internalInput.addSomaticSignal("fatigue", 0.3f);
    
    internalInput.addMetabolicSignal("respiration", 0.9f);
    internalInput.addEmotionalSignal("fear", 0.6f);
    internalInput.addCognitiveSignal("memory", 0.5f);
    internalInput.addSomaticSignal("pain", 0.2f);
    
    std::cout << "✓ Signal addition passed" << std::endl;
    
    // Test 3: Signal retrieval
    std::cout << "Test 3: Signal retrieval..." << std::endl;
    float metabolicState = internalInput.getMetabolicState();
    float emotionalState = internalInput.getEmotionalState();
    float stressLevel = internalInput.getStressLevel();
    float attentionLevel = internalInput.getAttentionLevel();
    
    assert(metabolicState >= 0.0f);
    assert(emotionalState >= 0.0f);
    assert(stressLevel >= 0.0f);
    assert(attentionLevel >= 0.0f);
    std::cout << "✓ Signal retrieval passed" << std::endl;
    
    // Test 4: Clone functionality
    std::cout << "Test 4: Clone functionality..." << std::endl;
    InternalSignals clonedInternal = internalInput.clone();
    assert(clonedInternal.getMetabolicState() == internalInput.getMetabolicState());
    assert(clonedInternal.getEmotionalState() == internalInput.getEmotionalState());
    std::cout << "✓ Clone functionality passed" << std::endl;
    
    // Test 5: Fusion methods
    std::cout << "Test 5: Fusion methods..." << std::endl;
    Vision visionInput(std::vector<float>(128 * 96 * 3, 0.5f), 128, 96, 3);
    Audio audioInput(std::vector<float>(44100, 0.3f), 44100);
    
    internalInput.fuseWithVision(visionInput);
    internalInput.fuseWithAudio(audioInput);
    
    visionInput.fuseWithInternalSignals(internalInput);
    audioInput.fuseWithInternalSignals(internalInput);
    
    std::cout << "✓ Fusion methods passed" << std::endl;
    
    // Test 6: Update with delta time
    std::cout << "Test 6: Update with delta time..." << std::endl;
    float deltaTime = 0.016f;  // 60 Hz
    internalInput.update(deltaTime);
    
    float stress1 = internalInput.getStressLevel();
    internalInput.update(deltaTime);
    float stress2 = internalInput.getStressLevel();
    
    assert(stress1 >= 0.0f);
    assert(stress2 >= 0.0f);
    std::cout << "✓ Update with delta time passed" << std::endl;
    
    // Test 7: Edge cases
    std::cout << "Test 7: Edge cases..." << std::endl;
    
    // Empty signals
    InternalSignals emptyInternal;
    assert(emptyInternal.getMetabolicState() >= 0.0f);
    assert(emptyInternal.getEmotionalState() >= 0.0f);
    
    // Signal removal
    internalInput.addMetabolicSignal("test", 1.0f);
    internalInput.addMetabolicSignal("test", 0.0f);  // Overwrite
    assert(internalInput.getMetabolicState() >= 0.0f);
    
    std::cout << "✓ Edge cases passed" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Internal Signals System tests PASSED! ✓" << std::endl;
    std::cout << "============================================" << std::endl;
}

void test_sensory_fusion_system() {
    std::cout << "Testing Sensory Fusion System (Phase 4)" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    // Create all sensory inputs
    std::cout << "Creating sensory inputs..." << std::endl;
    std::vector<float> visionData(128 * 96 * 3, 0.5f);
    Vision visionInput(visionData, 128, 96, 3);
    
    std::vector<float> audioData(44100, 0.3f);
    Audio audioInput(audioData, 44100);
    
    InternalSignals internalInput;
    
    // Test 1: Sensory fusion creation
    std::cout << "Test 1: Sensory fusion creation..." << std::endl;
    nlm::SensoryFusion fusion;
    
    assert(fusion.getFusedFeatures().size() == 0);
    std::cout << "✓ Sensory fusion creation passed" << std::endl;
    
    // Test 2: Adding inputs to fusion
    std::cout << "Test 2: Adding inputs to fusion..." << std::endl;
    fusion.addSensoryInput(visionInput);
    fusion.addSensoryInput(audioInput);
    fusion.addSensoryInput(internalInput);
    
    std::vector<float> fusedFeatures = fusion.getFusedFeatures();
    assert(fusedFeatures.size() > 0);
    std::cout << "✓ Adding inputs to fusion passed" << std::endl;
    
    // Test 3: Getting individual inputs
    std::cout << "Test 3: Getting individual inputs..." << std::endl;
    Vision retrievedVision = fusion.getVisionInput();
    Audio retrievedAudio = fusion.getAudioInput();
    InternalSignals retrievedInternal = fusion.getInternalSignalsInput();
    
    assert(retrievedVision.getWidth() == 128);
    assert(retrievedAudio.getSampleRate() == 44100);
    std::cout << "✓ Getting individual inputs passed" << std::endl;
    
    // Test 4: Fusion update
    std::cout << "Test 4: Fusion update..." << std::endl;
    float deltaTime = 0.016f;
    fusion.update(deltaTime);
    
    std::vector<float> updatedFeatures = fusion.getFusedFeatures();
    assert(updatedFeatures.size() > 0);
    std::cout << "✓ Fusion update passed" << std::endl;
    
    // Test 5: Reset fusion
    std::cout << "Test 5: Reset fusion..." << std::endl;
    fusion.reset();
    assert(fusion.getFusedFeatures().size() == 0);
    std::cout << "✓ Reset fusion passed" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Sensory Fusion System tests PASSED! ✓" << std::endl;
    std::cout << "==========================================" << std::endl;
}

void run_all_vision_tests() {
    std::cout << "Running comprehensive Vision and Audio tests..." << std::endl;
    std::cout << "=====================================================" << std::endl;
    
    test_vision_system();
    test_audio_system();
    test_internal_signals_system();
    test_sensory_fusion_system();
    
    std::cout << "=====================================================" << std::endl;
    std::cout << "All Vision, Audio, and InternalSignals tests PASSED! ✓" << std::endl;
    std::cout << "=====================================================" << std::endl;
}

} // namespace nlm