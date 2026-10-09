#include "Vision.hpp"
#include "Audio.hpp"
#include "InternalSignals.hpp"
#include "Neuromodulator.hpp"
#include "PredictionSystem.hpp"
#include "MotorSystem.hpp"
#include "DevelopmentSystem.hpp"
#include "NeuralPlanner.hpp"
#include "ConceptFormation.hpp"
#include "AttentionalSelection.hpp"
#include "Memory.hpp"
#include "NeuralWorkingMemory.hpp"
#include "NeuralEpisodicMemory.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

namespace nlm {

void test_phase4_vision_comprehensive() {
    std::cout << "=== Phase 4 Vision System Comprehensive Tests ===" << std::endl;
    
    // Test 1: Vision quality and luminance
    std::cout << "Test 1: Vision quality and luminance..." << std::endl;
    Vision vision1(std::vector<float>(100, 0.5f), 10, 10, 1);
    
    float quality1 = vision1.getQuality();
    float luminance1 = vision1.getLuminance();
    
    assert(quality1 >= 0.0f);
    assert(luminance1 >= 0.0f);
    std::cout << "✓ Vision quality and luminance test passed" << std::endl;
    
    // Test 2: Vision contrast and features
    std::cout << "Test 2: Vision contrast and features..." << std::endl;
    Vision vision2(std::vector<float>(200, 0.8f), 10, 20, 1);
    
    float contrast1 = vision2.getContrast();
    std::vector<float> features1 = vision2.extractFeatures();
    
    assert(contrast1 >= 0.0f);
    assert(features1.size() > 0);
    std::cout << "✓ Vision contrast and features test passed" << std::endl;
    
    // Test 3: Vision clone and update
    std::cout << "Test 3: Vision clone and update..." << std::endl;
    Vision vision3(std::vector<float>(300, 0.3f), 10, 30, 1);
    Vision vision3_clone = vision3.clone();
    
    assert(vision3_clone.getWidth() == vision3.getWidth());
    assert(vision3_clone.getHeight() == vision3.getHeight());
    assert(vision3_clone.getChannels() == vision3.getChannels());
    
    vision3.update(0.016f);
    vision3_clone.update(0.016f);
    
    assert(vision3.getQuality() >= 0.0f);
    assert(vision3_clone.getQuality() >= 0.0f);
    std::cout << "✓ Vision clone and update test passed" << std::endl;
    
    // Test 4: Vision data manipulation
    std::cout << "Test 4: Vision data manipulation..." << std::endl;
    Vision vision4(std::vector<float>(400, 0.7f), 10, 40, 1);
    
    std::vector<float> originalData = vision4.getData();
    assert(originalData.size() == 400);
    
    std::vector<float> newData(400, 0.1f);
    vision4.setData(newData);
    
    std::vector<float> modifiedData = vision4.getData();
    assert(modifiedData.size() == 400);
    assert(modifiedData[0] == 0.1f);
    std::cout << "✓ Vision data manipulation test passed" << std::endl;
    
    std::cout << "=== Phase 4 Vision System Comprehensive Tests PASSED! ===" << std::endl << std::endl;
}

void test_phase4_audio_comprehensive() {
    std::cout << "=== Phase 4 Audio System Comprehensive Tests ===" << std::endl;
    
    // Test 1: Audio duration and energy
    std::cout << "Test 1: Audio duration and energy..." << std::endl;
    Audio audio1(std::vector<float>(44100, 0.5f), 44100);
    
    float duration1 = audio1.getDuration();
    float energy1 = audio1.getEnergy();
    
    assert(duration1 > 0.0f);
    assert(energy1 >= 0.0f);
    std::cout << "✓ Audio duration and energy test passed" << std::endl;
    
    // Test 2: Audio pitch and spectrum
    std::cout << "Test 2: Audio pitch and spectrum..." << std::endl;
    Audio audio2(std::vector<float>(48000, 0.8f), 48000);
    
    float pitch1 = audio2.getPitch();
    std::vector<float> spectrum1 = audio2.extractSpectralFeatures();
    
    assert(pitch1 >= 0.0f);
    assert(spectrum1.size() > 0);
    std::cout << "✓ Audio pitch and spectrum test passed" << std::endl;
    
    // Test 3: Audio clone and update
    std::cout << "Test 3: Audio clone and update..." << std::endl;
    Audio audio3(std::vector<float>(8000, 0.3f), 8000);
    Audio audio3_clone = audio3.clone();
    
    assert(audio3_clone.getSampleRate() == audio3.getSampleRate());
    assert(audio3_clone.getNumSamples() == audio3.getNumSamples());
    
    audio3.update(0.016f);
    audio3_clone.update(0.016f);
    
    assert(audio3.getEnergy() >= 0.0f);
    assert(audio3_clone.getEnergy() >= 0.0f);
    std::cout << "✓ Audio clone and update test passed" << std::endl;
    
    // Test 4: Audio data manipulation
    std::cout << "Test 4: Audio data manipulation..." << std::endl;
    Audio audio4(std::vector<float>(16000, 0.6f), 16000);
    
    std::vector<float> originalData = audio4.getData();
    assert(originalData.size() == 16000);
    
    std::vector<float> newData(16000, 0.2f);
    audio4.setData(newData);
    
    std::vector<float> modifiedData = audio4.getData();
    assert(modifiedData.size() == 16000);
    assert(modifiedData[0] == 0.2f);
    std::cout << "✓ Audio data manipulation test passed" << std::endl;
    
    // Test 5: Audio quality variations
    std::cout << "Test 5: Audio quality variations..." << std::endl;
    Audio audio5_low(std::vector<float>(22050, 0.1f), 22050);
    Audio audio5_high(std::vector<float>(22050, 0.9f), 22050);
    
    float quality_low = audio5_low.getQuality();
    float quality_high = audio5_high.getQuality();
    
    assert(quality_low >= 0.0f);
    assert(quality_high >= 0.0f);
    std::cout << "✓ Audio quality variations test passed" << std::endl;
    
    std::cout << "=== Phase 4 Audio System Comprehensive Tests PASSED! ===" << std::endl << std::endl;
}

void test_phase4_internal_signals_comprehensive() {
    std::cout << "=== Phase 4 Internal Signals System Comprehensive Tests ===" << std::endl;
    
    // Test 1: Internal signals states
    std::cout << "Test 1: Internal signals states..." << std::endl;
    InternalSignals signals1;
    
    float metabolic1 = signals1.getMetabolicState();
    float emotional1 = signals1.getEmotionalState();
    float stress1 = signals1.getStressLevel();
    float attention1 = signals1.getAttentionLevel();
    float energy1 = signals1.getEnergyLevel();
    
    assert(metabolic1 >= 0.0f);
    assert(emotional1 >= 0.0f);
    assert(stress1 >= 0.0f);
    assert(attention1 >= 0.0f);
    assert(energy1 >= 0.0f);
    std::cout << "✓ Internal signals states test passed" << std::endl;
    
    // Test 2: Internal signals signal addition
    std::cout << "Test 2: Internal signals signal addition..." << std::endl;
    InternalSignals signals2;
    
    signals2.addMetabolicSignal("heart_rate", 1.2f);
    signals2.addEmotionalSignal("joy", 0.8f);
    signals2.addCognitiveSignal("attention", 0.7f);
    signals2.addSomaticSignal("fatigue", 0.3f);
    
    signals2.addMetabolicSignal("respiration", 0.9f);
    signals2.addEmotionalSignal("fear", 0.6f);
    signals2.addCognitiveSignal("memory", 0.5f);
    signals2.addSomaticSignal("pain", 0.2f);
    
    float metabolic2 = signals2.getMetabolicState();
    float emotional2 = signals2.getEmotionalState();
    
    assert(metabolic2 >= 0.0f);
    assert(emotional2 >= 0.0f);
    std::cout << "✓ Internal signals signal addition test passed" << std::endl;
    
    // Test 3: Internal signals clone and update
    std::cout << "Test 3: Internal signals clone and update..." << std::endl;
    InternalSignals signals3;
    InternalSignals signals3_clone = signals3.clone();
    
    assert(signals3_clone.getMetabolicState() == signals3.getMetabolicState());
    assert(signals3_clone.getEmotionalState() == signals3.getEmotionalState());
    
    signals3.update(0.016f);
    signals3_clone.update(0.016f);
    
    assert(signals3.getStressLevel() >= 0.0f);
    assert(signals3_clone.getStressLevel() >= 0.0f);
    std::cout << "✓ Internal signals clone and update test passed" << std::endl;
    
    // Test 4: Internal signals fusion
    std::cout << "Test 4: Internal signals fusion..." << std::endl;
    InternalSignals signals4;
    Vision vision4(std::vector<float>(100, 0.5f), 10, 10, 1);
    Audio audio4(std::vector<float>(1000, 0.3f), 1000);
    
    signals4.fuseWithVision(vision4);
    signals4.fuseWithAudio(audio4);
    
    float metabolic4 = signals4.getMetabolicState();
    float emotional4 = signals4.getEmotionalState();
    
    assert(metabolic4 >= 0.0f);
    assert(emotional4 >= 0.0f);
    std::cout << "✓ Internal signals fusion test passed" << std::endl;
    
    // Test 5: Internal signals edge cases
    std::cout << "Test 5: Internal signals edge cases..." << std::endl;
    InternalSignals signals5_empty;
    
    assert(signals5_empty.getMetabolicState() >= 0.0f);
    assert(signals5_empty.getEmotionalState() >= 0.0f);
    
    // Test signal overwriting
    signals5_empty.addMetabolicSignal("test", 1.0f);
    signals5_empty.addMetabolicSignal("test", 0.0f);  // Overwrite
    assert(signals5_empty.getMetabolicState() >= 0.0f);
    
    std::cout << "✓ Internal signals edge cases test passed" << std::endl;
    
    std::cout << "=== Phase 4 Internal Signals System Comprehensive Tests PASSED! ===" << std::endl << std::endl;
}

void test_phase4_sensory_fusion_comprehensive() {
    std::cout << "=== Phase 4 Sensory Fusion System Comprehensive Tests ===" << std::endl;
    
    // Test 1: Sensory fusion creation
    std::cout << "Test 1: Sensory fusion creation..." << std::endl;
    nlm::SensoryFusion fusion;
    
    assert(fusion.getFusedFeatures().size() == 0);
    std::cout << "✓ Sensory fusion creation test passed" << std::endl;
    
    // Test 2: Adding multiple sensory inputs
    std::cout << "Test 2: Adding multiple sensory inputs..." << std::endl;
    Vision vision(std::vector<float>(100, 0.5f), 10, 10, 1);
    Audio audio(std::vector<float>(1000, 0.3f), 1000);
    InternalSignals internal;
    
    fusion.addSensoryInput(vision);
    fusion.addSensoryInput(audio);
    fusion.addSensoryInput(internal);
    
    std::vector<float> fusedFeatures = fusion.getFusedFeatures();
    assert(fusedFeatures.size() > 0);
    std::cout << "✓ Adding multiple sensory inputs test passed" << std::endl;
    
    // Test 3: Retrieving individual inputs
    std::cout << "Test 3: Retrieving individual inputs..." << std::endl;
    Vision retrievedVision = fusion.getVisionInput();
    Audio retrievedAudio = fusion.getAudioInput();
    InternalSignals retrievedInternal = fusion.getInternalSignalsInput();
    
    assert(retrievedVision.getWidth() == 10);
    assert(retrievedAudio.getSampleRate() == 1000);
    std::cout << "✓ Retrieving individual inputs test passed" << std::endl;
    
    // Test 4: Sensory fusion update
    std::cout << "Test 4: Sensory fusion update..." << std::endl;
    fusion.update(0.016f);
    
    std::vector<float> updatedFeatures = fusion.getFusedFeatures();
    assert(updatedFeatures.size() > 0);
    std::cout << "✓ Sensory fusion update test passed" << std::endl;
    
    // Test 5: Reset sensory fusion
    std::cout << "Test 5: Reset sensory fusion..." << std::endl;
    fusion.reset();
    assert(fusion.getFusedFeatures().size() == 0);
    std::cout << "✓ Reset sensory fusion test passed" << std::endl;
    
    // Test 6: Multi-round fusion
    std::cout << "Test 6: Multi-round fusion..." << std::endl;
    nlm::SensoryFusion fusion2;
    
    for (int i = 0; i < 5; i++) {
        Vision v(std::vector<float>(100 * (i + 1), 0.5f), 10, 10, 1);
        fusion2.addSensoryInput(v);
        fusion2.update(0.016f);
        
        std::vector<float> features = fusion2.getFusedFeatures();
        assert(features.size() > 0);
    }
    std::cout << "✓ Multi-round fusion test passed" << std::endl;
    
    std::cout << "=== Phase 4 Sensory Fusion System Comprehensive Tests PASSED! ===" << std::endl << std::endl;
}

void run_all_phase4_tests() {
    std::cout << "Running comprehensive Phase 4 tests..." << std::endl;
    std::cout << "==================================== " << std::endl;
    
    test_phase4_vision_comprehensive();
    test_phase4_audio_comprehensive();
    test_phase4_internal_signals_comprehensive();
    test_phase4_sensory_fusion_comprehensive();
    
    std::cout << "==================================== " << std::endl;
    std::cout << "All Phase 4 tests PASSED! ✓" << std::endl;
    std::cout << "==================================== " << std::endl;
}

} // namespace nlm